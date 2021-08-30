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

#include <Urho3D/Core/Context.h>
#include <Urho3D/Engine/Console.h>
#include <Urho3D/Engine/DebugHud.h>
#include <Urho3D/IO/Log.h>
#include <Urho3D/Input/Input.h>
#include <Urho3D/Input/InputEvents.h>
#include "ShellState.h"
#include "ShellStateMachine.h"
#include "UI/LoadGameDialog.h"
#include "UI/MainMenuDialog.h"
#include "UI/NewGameDialog.h"
#include "UI/ServersListDialog.h"
#include "UI/SettingsDialog.h"

using namespace Urho3D;

ShellState::ShellState(Urho3D::Context* context)
	: Object(context)
	, interactives_(0)
{
	context->RegisterFactory<LoadGameDialog>();
	context->RegisterFactory<MainMenuDialog>();
	context->RegisterFactory<NewGameDialog>();
	context->RegisterFactory<ServersListDialog>();
	context->RegisterFactory<SettingsDialog>();

	SubscribeToEvent(E_KEYDOWN, URHO3D_HANDLER(ShellState, OnKeyDown));
}

void ShellState::Exit() { ReleaseSelf(); }

Dialog* ShellState::CreateDialog(Urho3D::StringHash type)
{
	SharedPtr<Object> object = context_->CreateObject(type);
	if (object.Null())
	{
		URHO3D_LOGERROR("Failed to create unregistered UI dialog.");
		return nullptr;
	}
	SharedPtr<Dialog> dialog;
	dialog.DynamicCast(object);
	if (dialog.Null())
	{
		URHO3D_LOGERROR("Failed to create UI dialog: given type is not a dialog.");
		return nullptr;
	}
	dialog->SetParent(this);
	dialogs_[type] = dialog;
	OnDialogAdd(dialog);
	return dialog.Get();
}

void ShellState::RemoveDialog(Urho3D::StringHash type)
{
	auto it = dialogs_.Find(type);
	if (it != dialogs_.End())
	{
		OnDialogRemove(it->second_);
		dialogs_.Erase(it);
	}
}

void ShellState::RemoveAllDialogs()
{
	dialogs_.Clear();
	interactives_ = 0;
}

bool ShellState::ReleaseSelf()
{
	if (interactives_)
		GetSubsystem<Input>()->SetMouseVisible(false);
	return GetSubsystem<ShellStateMachine>()->ProcessStateChanging();
}

void ShellState::OnDialogAdd(Dialog* widget)
{
	if (widget->IsCloseable())
		++closeables_;
	if (widget->IsInteractive())
		IncInteractives();
}

void ShellState::OnDialogRemove(Dialog* widget)
{
	if (widget->IsCloseable())
		--closeables_;
	if (widget->IsInteractive())
		DecInteractives();
}

void ShellState::IncInteractives()
{
	if (interactives_ == 0)
	{
		SetMouseVisible(true);
		SetSceneUpdate(false);
	}
	if (interactives_ < 0xff)
		++interactives_;
}

void ShellState::DecInteractives()
{
	if (interactives_ == 1)
	{
		SetMouseVisible(false);
		SetSceneUpdate(true);
	}
	if (interactives_ > 0)
		--interactives_;
}

void ShellState::SetMouseVisible(bool visible) const { GetSubsystem<Input>()->SetMouseVisible(visible); }

void ShellState::CloseFrontDialog()
{
	// TODO: Optimize for faster lookup
	for (auto it = dialogs_.Begin(); it != dialogs_.End(); ++it)
		if (it->second_->IsFrontElement() && it->second_->IsCloseable())
		{
			OnDialogRemove(it->second_);
			dialogs_.Erase(it);
			return;
		}
}

void ShellState::OnEscapePressed()
{
	if (interactives_)
		CloseFrontDialog();
	else
		BackState();
}

void ShellState::ToggleConsole()
{
	Console* console = GetSubsystem<Console>();
	console->Toggle();
	if (console->IsVisible())
		IncInteractives();
	else
		DecInteractives();
}

void ShellState::OnKeyDown(Urho3D::StringHash, Urho3D::VariantMap& eventData)
{
	using namespace KeyDown;
	switch (eventData[P_KEY].GetInt())
	{
	case KEY_ESCAPE:
		OnEscapePressed();
		break;
	case KEY_F1:
		ToggleConsole();
		break;
	}
}
