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
#include <Urho3D/Resource/Localization.h>
#include <Urho3D/UI/MessageBox.h>
#include <Urho3D/UI/Text.h>
#include <Urho3D/UI/UIEvents.h>
#include "FrontState.h"
#include "FrontStateMachine.h"

using namespace Urho3D;

FrontState::FrontState(Urho3D::Context* context)
	: Object(context)
	, message_(nullptr)
	, interactives_(0)
{
	SubscribeToEvent(E_KEYDOWN, URHO3D_HANDLER(FrontState, OnKeyDown));
}

void FrontState::Exit() { ReleaseSelf(); }

Dialog* FrontState::GetDialog(Urho3D::StringHash type) const
{
	const auto it = dialogs_.Find(type);
	return it != dialogs_.End() ? it->second_.Get() : nullptr;
}

Dialog* FrontState::CreateDialog(Urho3D::StringHash type)
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

void FrontState::RemoveDialog(Urho3D::StringHash type)
{
	auto it = dialogs_.Find(type);
	if (it != dialogs_.End())
	{
		OnDialogRemove(it->second_);
		dialogs_.Erase(it);
	}
}

void FrontState::RemoveAllDialogs()
{
	dialogs_.Clear();
	interactives_ = 0;
}

void FrontState::ShowErrorMessage(const Urho3D::String& text, const Urho3D::String& title)
{
	if (message_)
		return;

	ShowMessageBox(text, title);

	Localization* l10n = GetSubsystem<Localization>();
	URHO3D_LOGERRORF("%s: %s", l10n->Get(title).CString(), l10n->Get(text).CString());

	Text* okText = message_->GetWindow()->GetChild("OkButton", true)->GetChildStaticCast<Text>(0);
	okText->SetText("OK");
	okText->SetAutoLocalizable(true);
}

void FrontState::ShowQuestionMessage(const Urho3D::String& text, const Urho3D::String& title)
{
	if (message_)
		return;

	ShowMessageBox(text, title);
	EnableCancelButton();

	UIElement* window = message_->GetWindow();

	Text* okText = window->GetChild("OkButton", true)->GetChildStaticCast<Text>(0);
	okText->SetText("Yes");
	okText->SetAutoLocalizable(true);

	Text* cancelText = window->GetChild("CancelButton", true)->GetChildStaticCast<Text>(0);
	cancelText->SetText("No");
	cancelText->SetAutoLocalizable(true);
}

void FrontState::ShowMessageBox(const Urho3D::String& text, const Urho3D::String& title)
{
	Localization* l10n = GetSubsystem<Localization>();
	message_ = new MessageBox(context_, l10n->Get(text), l10n->Get(title));
	SubscribeToEvent(E_MESSAGEACK, URHO3D_HANDLER(FrontState, OnMessageACK));
}

void FrontState::EnableCancelButton()
{
	UIElement* cancel = message_->GetWindow()->GetChild("CancelButton", true);
	cancel->SetFocus(true);
	cancel->SetVisible(true);
}

bool FrontState::ReleaseSelf()
{
	if (interactives_)
		GetSubsystem<Input>()->SetMouseVisible(false);
	return GetSubsystem<FrontStateMachine>()->ProcessStateChanging();
}

void FrontState::OnDialogAdd(Dialog* widget)
{
	if (widget->IsCloseable())
		++closeables_;
	if (widget->IsInteractive())
		IncInteractives();
}

void FrontState::OnDialogRemove(Dialog* widget)
{
	if (widget->IsCloseable())
		--closeables_;
	if (widget->IsInteractive())
		DecInteractives();
}

void FrontState::IncInteractives()
{
	if (interactives_ == 0)
	{
		SetMouseVisible(true);
		SetSceneUpdate(false);
	}
	if (interactives_ < 0xff)
		++interactives_;
}

void FrontState::DecInteractives()
{
	if (interactives_ == 1)
	{
		SetMouseVisible(false);
		SetSceneUpdate(true);
	}
	if (interactives_ > 0)
		--interactives_;
}

void FrontState::SetMouseVisible(bool visible) const { GetSubsystem<Input>()->SetMouseVisible(visible); }

void FrontState::CloseFrontDialog()
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

void FrontState::OnEscapePressed()
{
	if (interactives_)
		CloseFrontDialog();
	else
		BackState();
}

void FrontState::ToggleConsole()
{
	Console* console = GetSubsystem<Console>();
	console->Toggle();
	if (console->IsVisible())
		IncInteractives();
	else
		DecInteractives();
}

void FrontState::ToggleDebugHud()
{
	DebugHud* debugHud = GetSubsystem<DebugHud>();
	debugHud->SetMode(debugHud->GetMode() ? DEBUGHUD_SHOW_NONE : DEBUGHUD_SHOW_ALL);
}

void FrontState::OnKeyDown(Urho3D::StringHash, Urho3D::VariantMap& eventData)
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
	case KEY_F2:
		ToggleDebugHud();
		break;
	}
}

void FrontState::OnMessageACK(Urho3D::StringHash, Urho3D::VariantMap&)
{
	message_ = nullptr;
	UnsubscribeFromEvent(E_MESSAGEACK);
}
