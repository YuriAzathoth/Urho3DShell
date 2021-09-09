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
#include <Urho3D/IO/Log.h>
#include <Urho3D/Resource/Localization.h>
#include <Urho3D/UI/MessageBox.h>
#include <Urho3D/UI/Text.h>
#include <Urho3D/UI/UIEvents.h>
#include "Notices.h"

using namespace Urho3D;

Notices::Notices(Urho3D::Context* context)
	: Object(context)
	, message_(nullptr)
	, exist_(false)
{
}

void Notices::Error(const Urho3D::String& text, const Urho3D::String& title)
{
	ShowToConsole(text, title);
	ShowDialog(text, title);

	Text* okText = message_->GetWindow()->GetChild("OkButton", true)->GetChildStaticCast<Text>(0);
	okText->SetText("OK");
	okText->SetAutoLocalizable(true);
}

void Notices::Question(const Urho3D::String& text, const Urho3D::String& title)
{
	ShowDialog(text, title);
	EnableCancelButton();

	UIElement* window = message_->GetWindow();

	Text* okText = window->GetChild("OkButton", true)->GetChildStaticCast<Text>(0);
	okText->SetText("Yes");
	okText->SetAutoLocalizable(true);

	Text* cancelText = window->GetChild("CancelButton", true)->GetChildStaticCast<Text>(0);
	cancelText->SetText("No");
	cancelText->SetAutoLocalizable(true);
}

void Notices::ShowDialog(const Urho3D::String& text, const Urho3D::String& title)
{
	Localization* l10n = GetSubsystem<Localization>();
	message_ = new MessageBox(context_, l10n->Get(text), l10n->Get(title));
	exist_ = true;
	SubscribeToEvent(E_MESSAGEACK, URHO3D_HANDLER(Notices, OnMessageACK));
}

void Notices::ShowToConsole(const Urho3D::String& text, const Urho3D::String& title)
{
	Localization* l10n = GetSubsystem<Localization>();
	URHO3D_LOGERRORF("%s: %s", l10n->Get(title).CString(), l10n->Get(text).CString());
}

void Notices::EnableCancelButton()
{
	UIElement* cancel = message_->GetWindow()->GetChild("CancelButton", true);
	cancel->SetFocus(true);
	cancel->SetVisible(true);
}

void Notices::OnMessageACK(Urho3D::StringHash, Urho3D::VariantMap& eventData)
{
	using namespace MessageACK;
	message_ = nullptr;
	exist_ = false;
	UnsubscribeFromEvent(E_MESSAGEACK);
}
