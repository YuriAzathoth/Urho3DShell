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

#ifndef CONFIGSETTINGSLIST_H
#define CONFIGSETTINGSLIST_H

#include "Config/EnumVariant.h"
#include "SettingsList.h"

class ConfigSettingsList : public SettingsList
{
	URHO3D_OBJECT(ConfigSettingsList, SettingsList)

public:
	ConfigSettingsList(Urho3D::StringHash settingsTab, Urho3D::Context* context, Urho3D::ListView* settings);

	void Apply() override;

protected:
	void CreateParameterBool(const Urho3D::String& parameterName, bool value, Urho3D::UIElement* parent);
	void CreateParameterFloat(const Urho3D::String& parameterName, float value, Urho3D::UIElement* parent);
	void
	CreateParameterString(const Urho3D::String& parameterName, const Urho3D::String& value, Urho3D::UIElement* parent);
	void CreateParameterEnum(const Urho3D::String& parameterName,
							 const EnumVector& items,
							 const Urho3D::Variant& value,
							 Urho3D::UIElement* parent,
							 bool localized);

	void OnBoolChanged(Urho3D::StringHash, Urho3D::VariantMap& eventData);
	void OnEnumChanged(Urho3D::StringHash, Urho3D::VariantMap& eventData);
	void OnFloatSliderChanged(Urho3D::StringHash, Urho3D::VariantMap& eventData);
	void OnFloatTextChanged(Urho3D::StringHash, Urho3D::VariantMap& eventData);
	void OnStringChanged(Urho3D::StringHash, Urho3D::VariantMap& eventData);

	Urho3D::VariantMap changedParameters_;
};

#endif // CONFIGSETTINGSLIST_H
