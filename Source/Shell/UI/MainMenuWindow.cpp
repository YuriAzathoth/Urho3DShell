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

#include <Urho3D/UI/UIEvents.h>
#include "MainMenuWindow.h"

using namespace Urho3D;

MainMenuWindow::MainMenuWindow(Urho3D::Context* context)
	: Widget(context)
{
	LoadLayout("UI/MainMenu.xml");
	SetInteractive(true);

	SubscribeToEvent(root_->GetChild("StartGame", true), E_PRESSED, URHO3D_HANDLER(MainMenuWindow, OnStartGame));
	SubscribeToEvent(root_->GetChild("Connect", true), E_PRESSED, URHO3D_HANDLER(MainMenuWindow, OnConnect));
	SubscribeToEvent(root_->GetChild("Mods", true), E_PRESSED, URHO3D_HANDLER(MainMenuWindow, OnMods));
	SubscribeToEvent(root_->GetChild("Settings", true), E_PRESSED, URHO3D_HANDLER(MainMenuWindow, OnSettings));
	SubscribeToEvent(root_->GetChild("Exit", true), E_PRESSED, URHO3D_HANDLER(MainMenuWindow, OnExit));
}

void MainMenuWindow::OnStartGame(Urho3D::StringHash, Urho3D::VariantMap&) { CreateDialog("StartGameWindow"); }
void MainMenuWindow::OnConnect(Urho3D::StringHash, Urho3D::VariantMap&) {}
void MainMenuWindow::OnMods(Urho3D::StringHash, Urho3D::VariantMap&) {}
void MainMenuWindow::OnSettings(Urho3D::StringHash, Urho3D::VariantMap&) { CreateDialog("SettingsWindow"); }

void MainMenuWindow::OnExit(Urho3D::StringHash, Urho3D::VariantMap&) {}
