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
#include "FrontState/FrontStateMachine.h"
#include "FrontState/MainMenuState.h"
#include "PauseDialog.h"

using namespace Urho3D;

PauseDialog::PauseDialog(Urho3D::Context* context)
	: Dialog(context)
{
	LoadLayout("UI/PauseDialog.xml");
	SetCloseable(true);
	SetInteractive(true);

	SubscribeToEvent(root_->GetChild("Resume", true), E_PRESSED, URHO3D_HANDLER(PauseDialog, OnResume));
	SubscribeToEvent(root_->GetChild("LoadGame", true), E_PRESSED, URHO3D_HANDLER(PauseDialog, OnLoadGame));
	SubscribeToEvent(root_->GetChild("SaveGame", true), E_PRESSED, URHO3D_HANDLER(PauseDialog, OnSaveGame));
	SubscribeToEvent(root_->GetChild("Settings", true), E_PRESSED, URHO3D_HANDLER(PauseDialog, OnSettings));
	SubscribeToEvent(root_->GetChild("Exit", true), E_PRESSED, URHO3D_HANDLER(PauseDialog, OnExit));
}

void PauseDialog::OnResume(Urho3D::StringHash, Urho3D::VariantMap&) { Close(); }
void PauseDialog::OnLoadGame(Urho3D::StringHash, Urho3D::VariantMap&) { GetParent()->CreateDialog("LoadGameDialog"); }
void PauseDialog::OnSaveGame(Urho3D::StringHash, Urho3D::VariantMap&)
{ /*TODO: SaveGameDialog*/
}
void PauseDialog::OnSettings(Urho3D::StringHash, Urho3D::VariantMap&) { GetParent()->CreateDialog("SettingsDialog"); }

void PauseDialog::OnExit(Urho3D::StringHash, Urho3D::VariantMap&)
{
	GetSubsystem<FrontStateMachine>()->Push<MainMenuState>();
}
