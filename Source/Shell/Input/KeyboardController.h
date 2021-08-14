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

#ifndef KEYBOARDCONTROLLER_H
#define KEYBOARDCONTROLLER_H

#include "InputController.h"
#include "Urho3DShellAPI.h"

class URHO3DSHELLAPI_EXPORT KeyboardController : public InputController
{
	URHO3D_OBJECT(KeyboardController, InputController)

public:
	using InputController::InputController;

	void ReadControls(Urho3D::Controls& controls) const override;
	Urho3D::String GetKeyName(unsigned keyCode) const override;
	unsigned GetKeyCode(const Urho3D::String& keyName) const override;
	void StartBinding(Urho3D::StringHash action) override;
	void EndBinding() override;

private:
	bool Enable() override;
	bool Disable() override;

	void OnKeyDown(Urho3D::StringHash, Urho3D::VariantMap& eventData);
	void OnKeyUp(Urho3D::StringHash, Urho3D::VariantMap& eventData);
	void OnMouseDown(Urho3D::StringHash, Urho3D::VariantMap& eventData);
	void OnMouseUp(Urho3D::StringHash, Urho3D::VariantMap& eventData);
};

#endif // KEYBOARDCONTROLLER_H
