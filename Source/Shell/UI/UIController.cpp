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
#include <Urho3D/Input/Input.h>
#include <Urho3D/Input/InputEvents.h>
#include <Urho3D/UI/Cursor.h>
#include <Urho3D/UI/UI.h>
#include "LoadGameMenu.h"
#include "MainMenuWindow.h"
#include "NewGameMenu.h"
#include "ServersListWindow.h"
#include "SettingsWindow.h"
#include "UIController.h"

using namespace Urho3D;

UIController::UIController(Urho3D::Context* context)
	: Urho3D::Object(context)
	, interactives_(0)
{
	context->RegisterFactory<LoadGameMenu>();
	context->RegisterFactory<MainMenuWindow>();
	context->RegisterFactory<NewGameMenu>();
	context->RegisterFactory<ServersListWindow>();
	context->RegisterFactory<SettingsWindow>();

	SubscribeToEvent(E_KEYDOWN, URHO3D_HANDLER(UIController, OnKeyDown));
}

void UIController::CreateDialog(Urho3D::StringHash type)
{
	SharedPtr<Object> object = context_->CreateObject(type);
	if (object.Null())
	{
		URHO3D_LOGERROR("Failed to create unregistered UI widget.");
		return;
	}
	SharedPtr<Widget> widget;
	widget.DynamicCast(object);
	if (widget.Null())
	{
		URHO3D_LOGERROR("Failed to create UI widget: given type is not a widget.");
		return;
	}
	widgets_[type] = widget;
	PostWidgetAdd(widget);
}

void UIController::RemoveDialog(Urho3D::StringHash type)
{
	auto it = widgets_.Find(type);
	if (it != widgets_.End())
	{
		PreWidgetRemove(it->second_);
		widgets_.Erase(it);
	}
}

void UIController::RemoveAllDialogs()
{
	widgets_.Clear();
	interactives_ = 0;
}

void UIController::PostWidgetAdd(Widget* widget)
{
	if (widget->IsCloseable())
		++closeables_;
	if (widget->IsInteractive())
	{
		if (interactives_ == 0)
			SetMouseVisible(true);
		++interactives_;
	}
}

void UIController::PreWidgetRemove(Widget* widget)
{
	if (widget->IsCloseable())
		++closeables_;
	if (widget->IsInteractive())
	{
		if (interactives_ == 1)
			SetMouseVisible(false);
		--interactives_;
	}
}

void UIController::SetMouseVisible(bool visible) const
{
	Cursor* cursor = nullptr;
	if (visible)
	{
		cursor = new Cursor(context_);
		cursor->SetPosition(GetSubsystem<Input>()->GetMousePosition());
		cursor->SetStyleAuto();
	}
	GetSubsystem<UI>()->SetCursor(cursor);
}

void UIController::CloseFrontDialog()
{
	for (auto it = widgets_.Begin(); it != widgets_.End(); ++it)
		if (it->second_->IsFrontElement() && it->second_->IsCloseable())
		{
			PreWidgetRemove(it->second_);
			widgets_.Erase(it);
			return;
		}
}

void UIController::OnKeyDown(Urho3D::StringHash, Urho3D::VariantMap& eventData)
{
	using namespace KeyDown;
	if (eventData[P_KEY].GetInt() == Key::KEY_ESCAPE)
		CloseFrontDialog();
}
