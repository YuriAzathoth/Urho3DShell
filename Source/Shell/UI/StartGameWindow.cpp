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

#include <Urho3D/IO/FileSystem.h>
#include <Urho3D/IO/PackageFile.h>
#include <Urho3D/Resource/ResourceCache.h>
#include <Urho3D/UI/Button.h>
#include <Urho3D/UI/LineEdit.h>
#include <Urho3D/UI/ListView.h>
#include <Urho3D/UI/Text.h>
#include <Urho3D/UI/UIEvents.h>
#include "StartGameWindow.h"

using namespace Urho3D;

StartGameWindow::StartGameWindow(Urho3D::Context* context)
	: Widget(context)
	, spawnedControls_(nullptr)
{
	LoadLayout("UI/StartGameMenu.xml");
	levelsList_ = root_->GetChildStaticCast<ListView>("LevelsList", true);
	serverPanel_ = root_->GetChild("ServerPanel", true);
	serverName_ = root_->GetChildStaticCast<LineEdit>("ServerName", true);
	serverPass_ = root_->GetChildStaticCast<LineEdit>("ServerPass", true);

	SetCloseable(true);
	SetInteractive(true);
	SetServerPanelVisible(false);

	BindButtonToClose(root_->GetChild("CloseButton", true));

	SubscribeToEvent(root_->GetChild("NewGame", true), E_PRESSED, URHO3D_HANDLER(StartGameWindow, OnNewGame));
	SubscribeToEvent(root_->GetChild("LoadGame", true), E_PRESSED, URHO3D_HANDLER(StartGameWindow, OnLoadGame));

	SubscribeToEvent(levelsList_, E_ITEMSELECTED, URHO3D_HANDLER(StartGameWindow, OnItemSelected));

	SubscribeToEvent(root_->GetChild("Server", true), E_TOGGLED, URHO3D_HANDLER(StartGameWindow, OnServerToggled));

	ShowNewGameTab();
}

void StartGameWindow::ShowNewGameTab()
{
	ClearScenesList();
	FillScenesList(ScanScenesInPackages());
}

void StartGameWindow::ShowLoadGameTab()
{
	ClearScenesList();
}

void StartGameWindow::FillScenesList(const Urho3D::StringVector& scenes)
{
	SharedPtr<UIElement> item;
	Text* text;
	for (const String& filename : ScanScenesInPackages())
	{
		item = MakeShared<UIElement>(context_);
		levelsList_->AddItem(item);
		item->SetName(filename);
		item->SetLayout(LM_HORIZONTAL, 0, {4, 4, 4, 4});
		item->SetStyleAuto();

		text = item->CreateChild<Text>();
		text->SetText(filename.Replaced("Scenes/", ""));
		text->SetStyleAuto();
	}
}

void StartGameWindow::ClearScenesList()
{
	spawnedControls_ = nullptr;
	levelsList_->RemoveAllItems();
}

void StartGameWindow::SetServerPanelVisible(bool visible)
{
	serverPanel_->SetVisible(visible);
	ShrinkSize();
}

Urho3D::StringVector StartGameWindow::ScanScenesInPackages() const
{
	StringVector ret;
	for (const SharedPtr<PackageFile>& package : GetSubsystem<ResourceCache>()->GetPackageFiles())
	{
		const StringVector resources = package->GetEntryNames();
		for (const String& filename : resources)
			if (filename.StartsWith("Scenes/") &&
				(filename.EndsWith(".xml") || filename.EndsWith(".bin") || filename.EndsWith(".json")))
				ret.Push(filename);
	}
	return ret;
}

Urho3D::StringVector StartGameWindow::ScanScenesInPath(const Urho3D::String& path) const
{
	StringVector ret;
	GetSubsystem<FileSystem>()->ScanDir(ret, path, "*.xml|*.bin|*.json", SCAN_FILES, true);
	return ret;
}

void StartGameWindow::OnNewGame(Urho3D::StringHash, Urho3D::VariantMap&) { ShowNewGameTab(); }
void StartGameWindow::OnLoadGame(Urho3D::StringHash, Urho3D::VariantMap&) { ShowLoadGameTab(); }

void StartGameWindow::OnItemSelected(Urho3D::StringHash, Urho3D::VariantMap& eventData)
{
	if (spawnedControls_)
		spawnedControls_->Remove();

	using namespace ItemSelected;
	const int selection = eventData[P_SELECTION].GetInt();
	UIElement* selectedItem = levelsList_->GetItem(selection);
	const String& sceneName = selectedItem->GetName();

	Button* button = selectedItem->CreateChild<Button>();
	button->SetLayout(LM_VERTICAL, 0, {4, 4, 4, 4});
	button->SetStyleAuto();
	spawnedControls_ = button;
	button->SubscribeToEvent(button, E_PRESSED, [sceneName](StringHash, VariantMap&) {});

	Text* caption = button->CreateChild<Text>();
	caption->SetText("Start");
	caption->SetTextAlignment(HA_CENTER);
	caption->SetAutoLocalizable(true);
	caption->SetStyleAuto();
}

void StartGameWindow::OnServerToggled(Urho3D::StringHash, Urho3D::VariantMap& eventData)
{
	using namespace Toggled;
	SetServerPanelVisible(eventData[P_STATE].GetBool());
}
