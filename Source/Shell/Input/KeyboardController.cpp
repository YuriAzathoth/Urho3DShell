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

#include <Urho3D/Input/Controls.h>
#include <Urho3D/Input/Input.h>
#include <Urho3D/Input/InputConstants.h>
#include "KeyboardController.h"

#define LEFT_BUTTON_NAME "LMB"
#define RIGHT_BUTTON_NAME "RMB"
#define MIDDLE_BUTTON_NAME "MMB"
#define AUX_BUTTON_1_NAME "X1"
#define AUX_BUTTON_2_NAME "X2"

using namespace Urho3D;

KeyboardController::KeyboardController(Urho3D::Context* context)
	: InputController(context)
{
}

void KeyboardController::ReadControls(Urho3D::Controls& controls) const
{
	const Input* input = GetSubsystem<Input>();

	controls.pitch_ = static_cast<float>(input->GetMouseMoveY()) * GetSensitivity();
	controls.yaw_ = static_cast<float>(input->GetMouseMoveX()) * GetSensitivity();

	for (const auto& p : GetRemoteBindings())
		controls.Set(input->GetKeyDown(static_cast<Key>(p.first_)), true);
}

Urho3D::String KeyboardController::GetKeyName(unsigned keyCode) const
{
	switch (keyCode)
	{
	case MouseButton::MOUSEB_LEFT:
		return LEFT_BUTTON_NAME;
	case MouseButton::MOUSEB_RIGHT:
		return RIGHT_BUTTON_NAME;
	case MouseButton::MOUSEB_MIDDLE:
		return MIDDLE_BUTTON_NAME;
	case MouseButton::MOUSEB_X1:
		return AUX_BUTTON_1_NAME;
	case MouseButton::MOUSEB_X2:
		return AUX_BUTTON_2_NAME;
	default:
		return GetSubsystem<Input>()->GetKeyName(static_cast<Key>(keyCode));
	}
}

unsigned KeyboardController::GetKeyCode(const Urho3D::String& keyName) const
{
	if (keyName == LEFT_BUTTON_NAME)
		return MouseButton::MOUSEB_LEFT;
	else if (keyName == RIGHT_BUTTON_NAME)
		return MouseButton::MOUSEB_RIGHT;
	else if (keyName == MIDDLE_BUTTON_NAME)
		return MouseButton::MOUSEB_MIDDLE;
	else if (keyName == AUX_BUTTON_1_NAME)
		return MouseButton::MOUSEB_X1;
	else if (keyName == AUX_BUTTON_2_NAME)
		return MouseButton::MOUSEB_X2;
	else
		return static_cast<unsigned>(GetSubsystem<Input>()->GetKeyFromName(keyName));
}

bool KeyboardController::Enable()
{
	SubscribeToEvent(E_KEYDOWN, URHO3D_HANDLER(KeyboardController, OnKeyDown));
	SubscribeToEvent(E_KEYUP, URHO3D_HANDLER(KeyboardController, OnKeyUp));
	return true;
}

bool KeyboardController::Disable()
{
	UnsubscribeFromAllEvents();
	return true;
}

void KeyboardController::OnKeyDown(Urho3D::StringHash, Urho3D::VariantMap& eventData)
{
	using namespace KeyDown;
	const unsigned key = static_cast<unsigned>(eventData[P_KEY].GetInt());
	SendAction(key, true);
}

void KeyboardController::OnKeyUp(Urho3D::StringHash, Urho3D::VariantMap& eventData)
{
	using namespace KeyUp;
	const unsigned key = static_cast<unsigned>(eventData[P_KEY].GetInt());
	SendAction(key, false);
}
