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

#include <Urho3D/Core/StringUtils.h>
#include <Urho3D/UI/Button.h>
#include <Urho3D/UI/CheckBox.h>
#include <Urho3D/UI/DropDownList.h>
#include <Urho3D/UI/LineEdit.h>
#include <Urho3D/UI/Slider.h>
#include <Urho3D/UI/Text.h>
#include <Urho3D/UI/UIEvents.h>
#include "Config/Config.h"
#include "ConfigSettingsList.h"

#define CB_ITEM_VALUE "Item"

using namespace Urho3D;

ConfigSettingsList::ConfigSettingsList(Urho3D::StringHash settingsTab,
									   Urho3D::Context* context,
									   Urho3D::ListView* settings)
	: SettingsList(context, settings)
{
	const Config* config = GetSubsystem<Config>();
	const StringVector parameters = config->GetSettings(settingsTab);

	UIElement* item;
	Variant value;
	for (const String& parameter : parameters)
	{
		item = CreateCaption(parameter);
		value = config->ReadValue(parameter);
		if (config->IsEnum(parameter))
		{
			const bool localized = config->IsLocalized(parameter);
			const EnumVector items = config->ConstructEnum(parameter);
			CreateParameterEnum(parameter, items, value, item, localized);
		}
		else
			switch (config->GetType(parameter))
			{
			case VAR_BOOL:
				CreateParameterBool(parameter, value.GetBool(), item);
				break;
			case VAR_FLOAT:
				CreateParameterFloat(parameter, value.GetFloat(), item);
				break;
			case VAR_STRING:
				CreateParameterString(parameter, value.GetString(), item);
				break;
			default:
				break;
			}
	}
}

void ConfigSettingsList::Apply() { GetSubsystem<Config>()->Apply(changedParameters_); }

void ConfigSettingsList::CreateParameterBool(const Urho3D::String& parameterName, bool value, Urho3D::UIElement* parent)
{
	CheckBox* checkBox = parent->CreateChild<CheckBox>();
	checkBox->SetName(parameterName);
	checkBox->SetChecked(value);
	checkBox->SetStyleAuto();
	checkBox->SubscribeToEvent(checkBox, E_TOGGLED, URHO3D_HANDLER(ConfigSettingsList, OnBoolChanged));
}

void ConfigSettingsList::CreateParameterFloat(const Urho3D::String& parameterName,
											  float value,
											  Urho3D::UIElement* parent)
{
	UIElement* group = parent->CreateChild<UIElement>();
	group->SetLayout(LM_HORIZONTAL, 4, IntRect::ZERO);
	group->SetStyleAuto();

	Slider* slider = group->CreateChild<Slider>();
	slider->SetName(parameterName);
	slider->SetValue(value);
	slider->SetStyleAuto();
	slider->SubscribeToEvent(slider, E_SLIDERCHANGED, URHO3D_HANDLER(ConfigSettingsList, OnFloatSliderChanged));

	LineEdit* lineEdit = group->CreateChild<LineEdit>();
	lineEdit->SetText(ToString("%f", value).Substring(0, 4));
	lineEdit->SetStyleAuto();
	lineEdit->SubscribeToEvent(lineEdit, E_TEXTCHANGED, URHO3D_HANDLER(ConfigSettingsList, OnFloatTextChanged));
}

void ConfigSettingsList::CreateParameterString(const Urho3D::String& parameterName,
											   const Urho3D::String& value,
											   Urho3D::UIElement* parent)
{
	LineEdit* lineEdit = parent->CreateChild<LineEdit>();
	lineEdit->SetName(parameterName);
	lineEdit->SetText(value);
	lineEdit->SetStyleAuto();
	lineEdit->SubscribeToEvent(lineEdit, E_TEXTFINISHED, URHO3D_HANDLER(ConfigSettingsList, OnStringChanged));
}

void ConfigSettingsList::CreateParameterEnum(const Urho3D::String& parameterName,
											 const EnumVector& items,
											 const Urho3D::Variant& value,
											 Urho3D::UIElement* parent,
											 bool localized)
{
	DropDownList* dropDownList = parent->CreateChild<DropDownList>();
	dropDownList->SetName(parameterName);
	dropDownList->SetLayout(LM_HORIZONTAL, 0, {4, 4, 4, 4});
	dropDownList->SetResizePopup(true);
	dropDownList->SetStyleAuto();
	dropDownList->SubscribeToEvent(dropDownList, E_ITEMSELECTED, URHO3D_HANDLER(ConfigSettingsList, OnEnumChanged));

	SharedPtr<UIElement> item;
	Text* text;
	unsigned itemId = 0;
	for (const EnumVariant& p : items)
	{
		item = MakeShared<UIElement>(context_);
		dropDownList->AddItem(item);
		item->SetLayout(LM_HORIZONTAL, 0, {4, 4, 4, 4});
		item->SetVar(CB_ITEM_VALUE, p.value_);
		item->SetStyleAuto();

		text = item->CreateChild<Text>();
		text->SetVerticalAlignment(VA_CENTER);
		text->SetText(p.caption_);
		text->SetAutoLocalizable(localized);
		text->SetStyleAuto();

		if (p.value_ == value)
			dropDownList->SetSelection(itemId);
		++itemId;
	}
}

void ConfigSettingsList::OnBoolChanged(Urho3D::StringHash, Urho3D::VariantMap& eventData)
{
	using namespace Toggled;
	const CheckBox* checkBox = static_cast<const CheckBox*>(eventData[P_ELEMENT].GetPtr());
	const String& name = checkBox->GetName();
	changedParameters_[name] = eventData[P_STATE].GetBool();
}

void ConfigSettingsList::OnEnumChanged(Urho3D::StringHash, Urho3D::VariantMap& eventData)
{
	using namespace ItemSelected;
	const DropDownList* dropDownList = static_cast<const DropDownList*>(eventData[P_ELEMENT].GetPtr());
	const String& name = dropDownList->GetName();
	const int itemId = eventData[P_SELECTION].GetInt();
	const UIElement* item = dropDownList->GetItem(itemId);
	const Variant& value = item->GetVar(CB_ITEM_VALUE);
	changedParameters_[name] = value;
}

void ConfigSettingsList::OnFloatSliderChanged(Urho3D::StringHash, Urho3D::VariantMap& eventData)
{
	using namespace SliderChanged;
	const Slider* slider = static_cast<const Slider*>(eventData[P_ELEMENT].GetPtr());
	const String& name = slider->GetName();
	const float value = eventData[P_VALUE].GetFloat();
	changedParameters_[name] = value;
	LineEdit* lineEdit = slider->GetParent()->GetChildStaticCast<LineEdit>(1);
	lineEdit->SetText(ToString("%f", value).Substring(0, 4));
}

void ConfigSettingsList::OnFloatTextChanged(Urho3D::StringHash, Urho3D::VariantMap& eventData)
{
	using namespace TextChanged;
	const LineEdit* lineEdit = static_cast<const LineEdit*>(eventData[P_ELEMENT].GetPtr());
	Slider* slider = lineEdit->GetParent()->GetChildStaticCast<Slider>(0);
	const String& value = eventData[P_TEXT].GetString();
	slider->SetValue(ToFloat(value));
}

void ConfigSettingsList::OnStringChanged(Urho3D::StringHash, Urho3D::VariantMap& eventData)
{
	using namespace TextFinished;
	const LineEdit* lineEdit = static_cast<const LineEdit*>(eventData[P_ELEMENT].GetPtr());
	Slider* slider = lineEdit->GetParent()->GetChildStaticCast<Slider>(0);
	const String& name = slider->GetName();
	const String& value = eventData[P_TEXT].GetString();
	changedParameters_[name] = value;
}
