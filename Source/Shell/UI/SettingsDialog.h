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

#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include "Config/Config.h"
#include "Dialog.h"

namespace Urho3D
{
class ListView;
}

class SettingsDialog : public Dialog
{
	URHO3D_OBJECT(SettingsDialog, Dialog)

public:
	explicit SettingsDialog(Urho3D::Context* context);

private:
	Urho3D::UIElement* CreateSettingsTab(const Urho3D::String& settingsTab);
	void ShowSettingsTab(Urho3D::StringHash settingsTab);

	Urho3D::UIElement* CreateCaption(const Urho3D::String& parameterName);
	void CreateParameterBool(const Urho3D::String& parameterName, bool value, Urho3D::UIElement* parent);
	void CreateParameterFloat(const Urho3D::String& parameterName, float value, Urho3D::UIElement* parent);
	void
	CreateParameterString(const Urho3D::String& parameterName, const Urho3D::String& value, Urho3D::UIElement* parent);
	void CreateParameterEnum(const Urho3D::String& parameterName,
							 const Config::EnumVector& items,
							 const Urho3D::Variant& value,
							 Urho3D::UIElement* parent,
							 bool localized);

	void OnOkPressed(Urho3D::StringHash, Urho3D::VariantMap&);
	void OnApplyPressed(Urho3D::StringHash, Urho3D::VariantMap&);
	void OnClosePressed(Urho3D::StringHash, Urho3D::VariantMap&);

	void OnBoolChanged(Urho3D::StringHash, Urho3D::VariantMap& eventData);
	void OnEnumChanged(Urho3D::StringHash, Urho3D::VariantMap& eventData);
	void OnFloatSliderChanged(Urho3D::StringHash, Urho3D::VariantMap& eventData);
	void OnFloatTextChanged(Urho3D::StringHash, Urho3D::VariantMap& eventData);
	void OnStringChanged(Urho3D::StringHash, Urho3D::VariantMap& eventData);

	Urho3D::VariantMap changedParameters_;
	Urho3D::UIElement* settingsTabs_;
	Urho3D::ListView* settings_;
};

#endif // SETTINGSDIALOG_H
