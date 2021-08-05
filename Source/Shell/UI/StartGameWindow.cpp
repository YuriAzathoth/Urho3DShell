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

#include <Urho3D/UI/Button.h>
#include <Urho3D/UI/CheckBox.h>
#include <Urho3D/UI/LineEdit.h>
#include <Urho3D/UI/ListView.h>
#include <Urho3D/UI/Text.h>
#include <Urho3D/UI/UIEvents.h>
#include "StartGameWindow.h"

using namespace Urho3D;

static const StringHash VAR_GAMENAME = "GameName";

StartGameWindow::StartGameWindow(Urho3D::Context* context)
	: Widget(context)
	, spawnedButton_(nullptr)
{
	LoadLayout("UI/GamesMenu.xml");
	gamesList_ = root_->GetChildStaticCast<ListView>("GamesList", true);
	serverPanel_ = root_->GetChild("ServerPanel", true);
	server_ = root_->GetChildStaticCast<CheckBox>("Server", true);
	serverName_ = root_->GetChildStaticCast<LineEdit>("ServerName", true);
	serverPass_ = root_->GetChildStaticCast<LineEdit>("ServerPass", true);

	SetCloseable(true);
	SetInteractive(true);
	SetServerPanelVisible(false);

	BindButtonToClose(root_->GetChild("CloseButton", true));
	SubscribeToEvent(gamesList_, E_ITEMSELECTED, URHO3D_HANDLER(StartGameWindow, OnItemSelected));
	SubscribeToEvent(root_->GetChild("Server", true), E_TOGGLED, URHO3D_HANDLER(StartGameWindow, OnServerToggled));
}

void StartGameWindow::AddGame(const Urho3D::String& caption, const Urho3D::String& gameName)
{
	SharedPtr<UIElement> item = MakeShared<UIElement>(context_);
	gamesList_->AddItem(item);
	item->SetVar(VAR_GAMENAME, gameName);
	item->SetLayout(LM_HORIZONTAL, 0, {4, 4, 4, 4});
	item->SetStyleAuto();

	Text* text = item->CreateChild<Text>();
	text->SetText(caption);
	text->SetStyleAuto();
}

void StartGameWindow::ClearGamesList()
{
	spawnedButton_ = nullptr;
	gamesList_->RemoveAllItems();
}

void StartGameWindow::SetTitle(const Urho3D::String& title)
{
	GetRoot()->GetChildStaticCast<Text>("Title", true)->SetText(title);
}

void StartGameWindow::SetServerSettingsVisible(bool visible)
{
	GetRoot()->GetChild("ServerCheckbox", true)->SetVisible(visible);
	SetServerPanelVisible(visible ? server_->IsChecked() : false);
}

void StartGameWindow::SetServerPanelVisible(bool visible)
{
	serverPanel_->SetVisible(visible);
	ShrinkSize();
}

void StartGameWindow::OnItemSelected(Urho3D::StringHash, Urho3D::VariantMap&)
{
	if (spawnedButton_)
		spawnedButton_->Remove();

	spawnedButton_ = gamesList_->GetSelectedItem()->CreateChild<Button>();
	spawnedButton_->SetLayout(LM_VERTICAL, 0, {4, 4, 4, 4});
	spawnedButton_->SetStyleAuto();

	Text* caption = spawnedButton_->CreateChild<Text>();
	caption->SetText("Start");
	caption->SetTextAlignment(HA_CENTER);
	caption->SetAutoLocalizable(true);
	caption->SetStyleAuto();

	spawnedButton_->SubscribeToEvent(spawnedButton_, E_PRESSED, URHO3D_HANDLER(StartGameWindow, OnStart));
}

void StartGameWindow::OnServerToggled(Urho3D::StringHash, Urho3D::VariantMap& eventData)
{
	using namespace Toggled;
	SetServerPanelVisible(eventData[P_STATE].GetBool());
}

void StartGameWindow::OnStart(Urho3D::StringHash, Urho3D::VariantMap&)
{
	const UIElement* item = spawnedButton_->GetParent();
	const String& gameName = item->GetVar(VAR_GAMENAME).GetString();
	if (server_->IsChecked())
	{
		const String& serverName = serverName_->GetText();
		const String& serverPass = serverPass_->GetText();
		Start(gameName, serverName, serverPass);
	}
	else
		Start(gameName);
}
