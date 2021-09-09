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

#include <Urho3D/Engine/Engine.h>
#include <Urho3D/UI/UIEvents.h>
#include "MainMenuDialog.h"
#include "ShellState/ShellState.h"

using namespace Urho3D;

MainMenuDialog::MainMenuDialog(Urho3D::Context* context)
	: Dialog(context)
{
	LoadLayout("UI/MainMenuDialog.xml");
	SetInteractive(true);

	SubscribeToEvent(root_->GetChild("NewGame", true), E_PRESSED, URHO3D_HANDLER(MainMenuDialog, OnNewGame));
	SubscribeToEvent(root_->GetChild("LoadGame", true), E_PRESSED, URHO3D_HANDLER(MainMenuDialog, OnLoadGame));
	SubscribeToEvent(root_->GetChild("Connect", true), E_PRESSED, URHO3D_HANDLER(MainMenuDialog, OnConnect));
	SubscribeToEvent(root_->GetChild("Mods", true), E_PRESSED, URHO3D_HANDLER(MainMenuDialog, OnMods));
	SubscribeToEvent(root_->GetChild("Settings", true), E_PRESSED, URHO3D_HANDLER(MainMenuDialog, OnSettings));
	SubscribeToEvent(root_->GetChild("Exit", true), E_PRESSED, URHO3D_HANDLER(MainMenuDialog, OnExit));
}

void MainMenuDialog::OnNewGame(Urho3D::StringHash, Urho3D::VariantMap&) { GetParent()->CreateDialog("NewGameDialog"); }

void MainMenuDialog::OnLoadGame(Urho3D::StringHash, Urho3D::VariantMap&)
{
	GetParent()->CreateDialog("LoadGameDialog");
}

void MainMenuDialog::OnConnect(Urho3D::StringHash, Urho3D::VariantMap&)
{
	GetParent()->CreateDialog("ServersListDialog");
}

void MainMenuDialog::OnMods(Urho3D::StringHash, Urho3D::VariantMap&) {}

void MainMenuDialog::OnSettings(Urho3D::StringHash, Urho3D::VariantMap&)
{
	GetParent()->CreateDialog("SettingsDialog");
}

void MainMenuDialog::OnExit(Urho3D::StringHash, Urho3D::VariantMap&)
{
	GetParent()->ShowQuestionMessage("ExitConfirmTitle", "ExitConfirmText");
	SubscribeToEvent(E_MESSAGEACK, URHO3D_HANDLER(MainMenuDialog, OnExitACK));
}

void MainMenuDialog::OnExitACK(Urho3D::StringHash, Urho3D::VariantMap& eventData)
{
	using namespace MessageACK;
	const bool ok = eventData[P_OK].GetBool();
	if (ok)
		GetSubsystem<Engine>()->Exit();
}
