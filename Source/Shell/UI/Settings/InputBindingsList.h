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

#ifndef INPUTBINDINGSLIST_H
#define INPUTBINDINGSLIST_H

#include "Config/Config.h"
#include "SettingsList.h"

namespace Urho3D
{
class DropDownList;
class Slider;
} // namespace Urho3D
class InputController;

class InputBindingsList : public SettingsList
{
	URHO3D_OBJECT(InputBindingsList, SettingsList)

public:
	InputBindingsList(Urho3D::Context* context, Urho3D::ListView* settings);
	~InputBindingsList();

	void Apply() override;

private:
	Urho3D::StringVector CreateControllersList();
	void CreateSensitivitySlider();
	void CreateActionBinding(const Urho3D::String& actionName);
	void LoadControllerSettings(const InputController* controller);
	const Urho3D::String& GetCurrentControllerName() const;
	InputController* GetCurrentController() const;
	void StopBinding();

	void OnControllerSelected(Urho3D::StringHash, Urho3D::VariantMap&);
	void OnBindingBegin(Urho3D::StringHash, Urho3D::VariantMap& eventData);
	void OnBindingEnd(Urho3D::StringHash, Urho3D::VariantMap& eventData);

	struct ChangedParameters
	{
		Urho3D::HashMap<Urho3D::StringHash, unsigned> bindings_;
		float sensitivity_;
		bool sensitivityChanged_;
	};

	Urho3D::HashMap<Urho3D::StringHash, ChangedParameters> changedBindings_;
	Urho3D::HashMap<Urho3D::StringHash, Urho3D::UIElement*> actions_;
	Urho3D::DropDownList* currentCtlr_;
	Urho3D::Slider* sensitivity_;
	bool binding_;
};

#endif // INPUTBINDINGSLIST_H
