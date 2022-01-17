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
#include <Urho3D/UI/UIElement.h>
#include "SettingsList.h"

using namespace Urho3D;

SettingsList::SettingsList(Urho3D::Context* context, Urho3D::ListView* settings)
	: Object(context)
	, settings_(settings)
{
}

Urho3D::UIElement* SettingsList::CreateCaption(const Urho3D::String& parameterName)
{
	SharedPtr<UIElement> item = MakeShared<UIElement>(context_);
	settings_->AddItem(item);
	item->SetLayout(LM_HORIZONTAL, 8, {4, 4, 4, 4});
	item->SetStyleAuto();

	Text* caption = item->CreateChild<Text>();
	caption->SetText(parameterName);
	caption->SetStyleAuto();
	return item.Get();
}
