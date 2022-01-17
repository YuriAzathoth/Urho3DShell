//
// Copyright (c) 2021-2022 Yuriy Zinchenko.
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
#include <Urho3D/UI/ListView.h>
#include <Urho3D/UI/Text.h>
#include <Urho3D/UI/UIEvents.h>
#include "Settings/ConfigSettingsList.h"
#include "Settings/InputBindingsList.h"
#include "SettingsDialog.h"

using namespace Urho3D;

SettingsDialog::SettingsDialog(Urho3D::Context* context)
	: Dialog(context)
{
	LoadLayout("UI/SettingsDialog.xml");
	SetCloseable(true);
	SetInteractive(true);

	SubscribeToEvent(root_->GetChild("CloseButton", true), E_PRESSED, URHO3D_HANDLER(SettingsDialog, OnClosePressed));
	SubscribeToEvent(root_->GetChild("Cancel", true), E_PRESSED, URHO3D_HANDLER(SettingsDialog, OnClosePressed));
	SubscribeToEvent(root_->GetChild("Ok", true), E_PRESSED, URHO3D_HANDLER(SettingsDialog, OnOkPressed));
	SubscribeToEvent(root_->GetChild("Apply", true), E_PRESSED, URHO3D_HANDLER(SettingsDialog, OnApplyPressed));

	settingsTabs_ = root_->GetChild("SettingsTabs", true);
	settings_ = root_->GetChildStaticCast<ListView>("SettingsList", true);

	const StringVector tabs = GetSubsystem<Config>()->GetSettingsTabs();
	StringHash settingsTab;
	UIElement* tabButton;
	for (const String& tabName : tabs)
	{
		tabButton = CreateSettingsTab(tabName);
		settingsTab = tabName;
		SubscribeToEvent(tabButton,
						 E_PRESSED,
						 [this, settingsTab](StringHash, VariantMap&) { ShowSettingsTab(settingsTab); });
	}

	tabButton = CreateSettingsTab("Controls");
	SubscribeToEvent(tabButton, E_PRESSED, [this](StringHash, VariantMap&) { ShowControlsTab(); });

	ShowSettingsTab(tabs[0]);
}

void SettingsDialog::ShowSettingsTab(Urho3D::StringHash settingsTab)
{
	settings_->RemoveAllItems();
	settingsList_ = new ConfigSettingsList(settingsTab, context_, settings_);
}

void SettingsDialog::ShowControlsTab()
{
	settings_->RemoveAllItems();
	settingsList_ = new InputBindingsList(context_, settings_);
}

Urho3D::UIElement* SettingsDialog::CreateSettingsTab(const Urho3D::String& settingsTab)
{
	Button* button = settingsTabs_->CreateChild<Button>();
	button->SetLayout(LM_VERTICAL, 0, {4, 4, 4, 4});
	button->SetStyleAuto();

	Text* caption = button->CreateChild<Text>();
	caption->SetText(settingsTab);
	caption->SetHorizontalAlignment(HA_CENTER);
	caption->SetAutoLocalizable(true);
	caption->SetStyleAuto();

	return button;
}

void SettingsDialog::OnOkPressed(Urho3D::StringHash, Urho3D::VariantMap&)
{
	settingsList_->Apply();
	Close();
}

void SettingsDialog::OnApplyPressed(Urho3D::StringHash, Urho3D::VariantMap&) { settingsList_->Apply(); }
void SettingsDialog::OnClosePressed(Urho3D::StringHash, Urho3D::VariantMap&) { Close(); }
