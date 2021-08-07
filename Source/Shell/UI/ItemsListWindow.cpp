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
#include "ItemsListWindow.h"

using namespace Urho3D;

static const StringHash VAR_GAMENAME = "GameName";

ItemsListWindow::ItemsListWindow(Urho3D::Context* context)
	: Dialog(context)
	, spawnedButton_(nullptr)
{
	LoadLayout("UI/GamesMenu.xml");
	itemList_ = root_->GetChildStaticCast<ListView>("GamesList", true);
	serverPanel_ = root_->GetChild("ServerPanel", true);
	server_ = root_->GetChildStaticCast<CheckBox>("Server", true);
	serverName_ = root_->GetChildStaticCast<LineEdit>("ServerName", true);
	serverPass_ = root_->GetChildStaticCast<LineEdit>("ServerPass", true);

	SetCloseable(true);
	SetInteractive(true);
	SetServerPanelVisible(false);

	BindButtonToClose(root_->GetChild("CloseButton", true));
	SubscribeToEvent(itemList_, E_ITEMSELECTED, URHO3D_HANDLER(ItemsListWindow, OnItemSelected));
	SubscribeToEvent(root_->GetChild("Server", true), E_TOGGLED, URHO3D_HANDLER(ItemsListWindow, OnServerToggled));
}

void ItemsListWindow::AddItem(const Urho3D::String& caption, const Urho3D::String& itemName)
{
	SharedPtr<UIElement> item = MakeShared<UIElement>(context_);
	itemList_->AddItem(item);
	item->SetVar(VAR_GAMENAME, itemName);
	item->SetLayout(LM_HORIZONTAL, 0, {4, 4, 4, 4});
	item->SetStyleAuto();

	Text* text = item->CreateChild<Text>();
	text->SetText(caption);
	text->SetStyleAuto();
}

void ItemsListWindow::RemoveAllItems()
{
	spawnedButton_ = nullptr;
	itemList_->RemoveAllItems();
}

void ItemsListWindow::SetTitle(const Urho3D::String& title)
{
	GetRoot()->GetChildStaticCast<Text>("Title", true)->SetText(title);
}

void ItemsListWindow::SetServerSettingsVisible(bool visible)
{
	GetRoot()->GetChild("ServerCheckbox", true)->SetVisible(visible);
	SetServerPanelVisible(visible ? server_->IsChecked() : false);
}

void ItemsListWindow::SetServerPanelVisible(bool visible)
{
	serverPanel_->SetVisible(visible);
	ShrinkSize();
}

void ItemsListWindow::OnItemSelected(Urho3D::StringHash, Urho3D::VariantMap&)
{
	if (spawnedButton_)
		spawnedButton_->Remove();

	spawnedButton_ = itemList_->GetSelectedItem()->CreateChild<Button>();
	spawnedButton_->SetLayout(LM_VERTICAL, 0, {4, 4, 4, 4});
	spawnedButton_->SetStyleAuto();

	Text* caption = spawnedButton_->CreateChild<Text>();
	caption->SetText("Start");
	caption->SetTextAlignment(HA_CENTER);
	caption->SetAutoLocalizable(true);
	caption->SetStyleAuto();

	spawnedButton_->SubscribeToEvent(spawnedButton_, E_PRESSED, URHO3D_HANDLER(ItemsListWindow, OnStart));
}

void ItemsListWindow::OnServerToggled(Urho3D::StringHash, Urho3D::VariantMap& eventData)
{
	using namespace Toggled;
	SetServerPanelVisible(eventData[P_STATE].GetBool());
}

void ItemsListWindow::OnStart(Urho3D::StringHash, Urho3D::VariantMap&)
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
