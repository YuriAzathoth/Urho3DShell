//
// Copyright (c) 2021 Yuriy Zinchenko.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//

#include <Urho3D/Network/Network.h>
#include <Urho3D/Network/NetworkEvents.h>
#include <Urho3D/UI/Button.h>
#include <Urho3D/UI/ListView.h>
#include <Urho3D/UI/Text.h>
#include <Urho3D/UI/UIEvents.h>
#include "Core/Shell.h"
#include "Network/ServerDefs.h"
#include "ServersListWindow.h"
#include "ShellState/ShellState.h"

using namespace Urho3D;

static const StringHash VAR_ADDRESS = "Address";

ServersListWindow::ServersListWindow(Urho3D::Context* context)
	: Widget(context)
	, spawnedButton_(nullptr)
{
	LoadLayout("UI/ServersList.xml");
	serversList_ = root_->GetChildStaticCast<ListView>("ServersList", true);
	BindButtonToClose(root_->GetChild("CloseButton", true));

	SetCloseable(true);
	SetInteractive(true);

	SubscribeToEvent(E_NETWORKHOSTDISCOVERED, URHO3D_HANDLER(ServersListWindow, OnNetworkHostDiscovered));
	SubscribeToEvent(serversList_, E_ITEMSELECTED, URHO3D_HANDLER(ServersListWindow, OnItemSelected));

	GetSubsystem<Network>()->DiscoverHosts(GetSubsystem<Shell>()->GetPort());
}

void ServersListWindow::OnConnect(Urho3D::StringHash, Urho3D::VariantMap&)
{
	const UIElement* item = spawnedButton_->GetParent();
	const String& address = item->GetVar(VAR_ADDRESS).GetString();
	GetSubsystem<ShellState>()->StartClient(address);
}

void ServersListWindow::OnItemSelected(Urho3D::StringHash, Urho3D::VariantMap&)
{
	if (spawnedButton_)
		spawnedButton_->Remove();

	spawnedButton_ = serversList_->GetSelectedItem()->CreateChild("Button");
	spawnedButton_->SetLayout(LM_VERTICAL, 0, {4, 4, 4, 4});
	spawnedButton_->SetStyleAuto();

	Text* caption = spawnedButton_->CreateChild<Text>();
	caption->SetText("Connect");
	caption->SetTextAlignment(HA_CENTER);
	caption->SetAutoLocalizable(true);
	caption->SetStyleAuto();

	spawnedButton_->SubscribeToEvent(spawnedButton_, E_PRESSED, URHO3D_HANDLER(ServersListWindow, OnConnect));
}

void ServersListWindow::OnNetworkHostDiscovered(Urho3D::StringHash, Urho3D::VariantMap& eventData)
{
	using namespace NetworkHostDiscovered;
	const VariantMap& hostBeacon = eventData[P_BEACON].GetVariantMap();
	if (!hostBeacon.Empty())
	{
		const String& address = eventData[P_ADDRESS].GetString();

		SharedPtr<UIElement> item = MakeShared<UIElement>(context_);
		serversList_->AddItem(item);
		item->SetVar(VAR_ADDRESS, address);
		item->SetLayout(LM_HORIZONTAL, 0, {4, 4, 4, 4});
		item->SetStyleAuto();

		const String& serverName = hostBeacon[SV_NAME]->GetString();
		Text* text = item->CreateChild<Text>();
		text->SetText(serverName);
		text->SetStyleAuto();
	}
}
