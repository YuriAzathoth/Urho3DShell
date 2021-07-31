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

#include <Urho3D/Core/StringUtils.h>
#include <Urho3D/Engine/EngineDefs.h>
#include <Urho3D/IO/Log.h>
#include <Urho3D/Resource/XMLElement.h>
#include <Urho3D/UI/CheckBox.h>
#include <Urho3D/UI/DropDownList.h>
#include <Urho3D/UI/LineEdit.h>
#include <Urho3D/UI/Slider.h>
#include <Urho3D/UI/Text.h>
#include <Urho3D/UI/UIEvents.h>
#include "Config.h"
#include "ConfigDefs.h"

#define CB_ITEM_VALUE "Item"

using namespace Urho3D;

void Config::LoadXML(const Urho3D::XMLElement& source)
{
	String name;
	Variant value;
	for (XMLElement parameter = source.GetChild("parameter"); !parameter.IsNull(); parameter = parameter.GetNext())
	{
		name = parameter.GetAttribute("name");
		value = parameter.GetVariant();
		if (name.Empty())
			URHO3D_LOGWARNING("Failed to setup config parameter: name is empty.");
		else if (value.IsEmpty())
			URHO3D_LOGWARNINGF("Failed to setup config parameter %s: value is empty.", name.CString());
		else if (!parameters_.Contains(name))
			URHO3D_LOGWARNINGF("Failed to setup config parameter %s: parameter is not registered yet.", name.CString());
		else
			changedParameters_[name] = value;
	}
}

void Config::SaveXML(Urho3D::XMLElement& dst) const
{
	XMLElement parameter;
	for (const auto& p : parameters_)
	{
		parameter = dst.CreateChild("parameter");
		parameter.SetAttribute("name", *names_[p.first_]);
		parameter.SetVariant(p.second_.reader_->Read());
	}
}

void Config::Apply(bool engineToo)
{
	Parameter* parameter;
	for (const auto& p : changedParameters_)
	{
		parameter = &parameters_[p.first_];
		if (engineToo || !(engineToo || parameter->flags_ & ParameterFlags::ENGINE))
			parameter->writer_->Write(p.second_);
	}
	changedParameters_.Clear();

	ComplexStorage* storage;
	for (auto& p : storages_)
	{
		storage = p.second_.Get();
		if (!storage->parameters_.Empty() && (engineToo || !(engineToo || storage->isEngine_)))
		{
			storage->writer_(p.second_->parameters_);
			storage->parameters_.Clear();
		}
	}
}

void Config::Clear()
{
	changedParameters_.Clear();
	for (auto& p : storages_)
		p.second_->parameters_.Clear();
}

void Config::ExtractEngineParameters(Urho3D::VariantMap& dst)
{
	// TODO: Implement registrable engine parameters forwarding
	auto it = changedParameters_.Find(ECP_RESOLUTION);
	if (it != changedParameters_.End())
	{
		const IntVector3 resolution = StrToRes(it->second_.GetString());
		dst[EP_WINDOW_WIDTH] = resolution.x_;
		dst[EP_WINDOW_HEIGHT] = resolution.y_;
		dst[EP_REFRESH_RATE] = resolution.z_;
	}
	it = changedParameters_.Find(ECP_WINDOW_MODE);
	if (it != changedParameters_.End())
	{
		const int windowMode = it->second_.GetInt();
		dst[EP_FULL_SCREEN] = windowMode >= 1;
		dst[EP_BORDERLESS] = windowMode == 2;
	}
	for (it = changedParameters_.Begin(); it != changedParameters_.End();)
		if (parameters_[it->first_].flags_ & ParameterFlags::ENGINE)
		{
			dst.Insert(it);
			it = changedParameters_.Erase(it);
		}
		else
			++it;
}

void Config::RegisterSettingsTab(const Urho3D::String& tabName)
{
	if (!settings_.Contains(tabName))
	{
		settings_[tabName]; // Create default constructed object in map
		names_[tabName] = tabName;
	}
	else
		URHO3D_LOGWARNINGF("Failed to register already registered settings tab %s.", tabName.CString());
}

void Config::RemoveSettingsTab(Urho3D::StringHash tab)
{
	auto itTab = settings_.Find(tab);
	if (itTab != settings_.End())
	{
		const auto itParameter = parameters_.Find(tab);
		if (itParameter == parameters_.End())
			names_.Erase(tab);
		settings_.Erase(itTab);
	}
	else
		URHO3D_LOGWARNING("Failed to remove non-existent settings tab.");
}

Urho3D::StringVector Config::GetSettingsTabs() const
{
	StringVector ret;
	ret.Reserve(settings_.Size());
	for (const auto& p : settings_)
		ret.Push(*names_[p.first_]);
	return ret;
}

Urho3D::StringVector Config::GetSettings(Urho3D::StringHash settingsTab) const
{
	const auto it = settings_.Find(settingsTab);
	if (it != settings_.End())
	{
		StringVector ret;
		const SettingsTab& tab = it->second_;
		ret.Reserve(tab.settings_.Size());
		for (StringHash setting : tab.settings_)
			ret.Push(*names_[setting]);
		return ret;
	}
	else
	{
		URHO3D_LOGERRORF("Failed to load unregistered settings tab parameters.");
		return Variant::emptyStringVector;
	}
}

bool Config::RegisterParameter(const Urho3D::String& parameterName,
							   Urho3D::VariantType type,
							   Urho3D::StringHash settingsTab,
							   bool isEngine,
							   bool localized)
{
	auto it = settings_.Find(settingsTab);
	if (!parameters_.Contains(parameterName) && it != settings_.End())
	{
		Parameter& parameter = parameters_[parameterName];
		parameter.settingsTab_ = settingsTab;
		parameter.type_ = type;
		if (isEngine)
			parameter.flags_ |= ParameterFlags::ENGINE;
		if (localized)
			parameter.flags_ |= ParameterFlags::LOCALIZED;
		it->second_.settings_.Push(parameterName);
		names_[parameterName] = parameterName;
		return true;
	}
	else
	{
		URHO3D_LOGWARNINGF("Failed to register config parameter %s: parameter is already registered.",
						   parameterName.CString());
		return false;
	}
}

void Config::RemoveParameter(Urho3D::StringHash parameter)
{
	auto itParameter = parameters_.Find(parameter);
	if (itParameter != parameters_.End())
	{
		auto itSetting = settings_.Find(itParameter->second_.settingsTab_);
		if (itSetting == settings_.End())
			names_.Erase(parameter);
		else
			settings_.Erase(itSetting);
		if (itParameter->second_.flags_ & ParameterFlags::ENUM)
			enumConstructors_.Erase(itParameter->first_);
		parameters_.Erase(itParameter);
	}
	else
		URHO3D_LOGWARNING("Failed to remove non-existent config parameter.");
}

void Config::RegisterReader(Urho3D::StringHash parameter, SimpleReaderFunc reader)
{
	auto it = parameters_.Find(parameter);
	if (it != parameters_.End())
		it->second_.reader_ = new SimpleReader(reader);
	else
		URHO3D_LOGWARNING("Failed to assign reader to non-existent config parameter.");
}

void Config::RegisterWriter(Urho3D::StringHash parameter, SimpleWriterFunc writer)
{
	auto it = parameters_.Find(parameter);
	if (it != parameters_.End())
		it->second_.writer_ = new SimpleWriter(writer);
	else
		URHO3D_LOGWARNING("Failed to assign writer to non-existent config parameter.");
}

void Config::RegisterEnum(Urho3D::StringHash parameter, EnumConstructorFunc enumConstructor)
{
	auto it = parameters_.Find(parameter);
	if (it != parameters_.End())
	{
		it->second_.flags_ |= ParameterFlags::ENUM;
		enumConstructors_[parameter] = enumConstructor;
	}
	else
		URHO3D_LOGWARNING("Failed to assign enum constructor to non-existent config parameter.");
}

bool Config::RegisterComplexStorage(Urho3D::StringHash cathegory, ComplexWriterFunc writer)
{
	if (!storages_.Contains(cathegory))
	{
		SharedPtr<ComplexStorage>& storage = storages_[cathegory];
		storage = MakeShared<ComplexStorage>();
		storage->writer_ = writer;
		return true;
	}
	else
	{
		URHO3D_LOGWARNING("Failed to register already existent complex config storage.");
		return false;
	}
}

bool Config::RegisterComplexWriter(Urho3D::StringHash parameter, Urho3D::StringHash cathegory)
{
	auto itParameter = parameters_.Find(parameter);
	auto itStorage = storages_.Find(cathegory);
	if (itParameter != parameters_.End() && itStorage != storages_.End())
	{
		itParameter->second_.writer_ = new ComplexWriter(itStorage->second_, parameter);
		itStorage->second_->isEngine_ = itParameter->second_.flags_ & ParameterFlags::ENGINE;
		return true;
	}
	else
	{
		if (itParameter == parameters_.End())
			URHO3D_LOGWARNING("Failed to assign writer to non-existent config parameter.");
		if (itStorage == storages_.End())
			URHO3D_LOGWARNING("Failed to assign writer to non-existent complex config storage.");
		return false;
	}
}

void Config::GetDebugString(Urho3D::String& dst) const
{
	// ATTENTION! Slow code! Use it ONLY for debugging purposes!
	StringVector tabs = GetSettingsTabs();
	StringVector parameters;
	Variant value;
	EnumVector enumVector;
	for (const String& tabName : tabs)
	{
		dst.Append(tabName).Append('\n');
		parameters = GetSettings(tabName);
		for (const String& parameterName : parameters)
		{
			const Parameter& parameter = *parameters_[parameterName];
			value = parameter.reader_->Read();
			dst.Append("\t").Append(parameterName).Append('\n');
			dst.Append("\t\tType  = ").Append(value.GetTypeName()).Append('\n');
			dst.Append("\t\tValue = ").Append(value.ToString()).Append('\n');
			dst.Append("\t\tStore = ");
			if (parameter.flags_ & ParameterFlags::ENGINE)
				dst.Append("ENGINE\n");
			else
				dst.Append("CUSTOM\n");
			if (parameter.flags_ & ParameterFlags::ENUM)
			{
				dst.Append("\t\tEnum Variants:\n");
				enumVector = (*enumConstructors_[parameterName])();
				for (const EnumVariant& enumVariant : enumVector)
					dst.Append("\t\t\t")
						.Append(enumVariant.caption_)
						.Append(" = ")
						.Append(enumVariant.value_.ToString())
						.Append('\n');
			}
		}
	}
}

void Config::CreateParameterControl(Urho3D::StringHash parameter, Urho3D::UIElement* parent)
{
	const auto it = parameters_.Find(parameter);
	if (it == parameters_.End())
	{
		URHO3D_LOGWARNING("Failed to construct settings control for unregistered parameter.");
		return;
	}

	const String parameterName = names_[parameter];
	const Variant value = it->second_.reader_->Read();

	EnumVector items;
	if (it->second_.flags_ & ParameterFlags::ENUM)
	{
		const bool localized = it->second_.flags_ & ParameterFlags::LOCALIZED;
		items = enumConstructors_[it->first_]();
		CreateParameterEnum(parameterName, items, value, parent, localized);
		items.Clear();
	}
	else
		switch (it->second_.type_)
		{
		case VAR_BOOL:
			CreateParameterBool(parameterName, value.GetBool(), parent);
			break;
		case VAR_FLOAT:
			CreateParameterFloat(parameterName, value.GetFloat(), parent);
			break;
		case VAR_STRING:
			CreateParameterString(parameterName, value.GetString(), parent);
			break;
		default:
			URHO3D_LOGWARNING("Failed to construct settings control for parameter with unsupported type.");
		}
}

void Config::CreateParameterBool(const Urho3D::String& parameterName, bool value, Urho3D::UIElement* parent)
{
	CheckBox* checkBox = parent->CreateChild<CheckBox>();
	checkBox->SetName(parameterName);
	checkBox->SetChecked(value);
	checkBox->SetStyleAuto();
	checkBox->SubscribeToEvent(checkBox, E_TOGGLED, URHO3D_HANDLER(Config, OnBoolChanged));
}

void Config::CreateParameterFloat(const Urho3D::String& parameterName, float value, Urho3D::UIElement* parent)
{
	UIElement* group = parent->CreateChild<UIElement>();
	group->SetLayout(LM_HORIZONTAL, 4, IntRect::ZERO);
	group->SetStyleAuto();

	Slider* slider = group->CreateChild<Slider>();
	slider->SetName(parameterName);
	slider->SetValue(value);
	slider->SetStyleAuto();
	slider->SubscribeToEvent(slider, E_SLIDERCHANGED, URHO3D_HANDLER(Config, OnFloatSliderChanged));

	LineEdit* lineEdit = group->CreateChild<LineEdit>();
	lineEdit->SetText(ToString("%f", value).Substring(0, 4));
	lineEdit->SetStyleAuto();
	lineEdit->SubscribeToEvent(lineEdit, E_TEXTCHANGED, URHO3D_HANDLER(Config, OnFloatTextChanged));
}

void Config::CreateParameterString(const Urho3D::String& parameterName,
								   const Urho3D::String& value,
								   Urho3D::UIElement* parent)
{
	LineEdit* lineEdit = parent->CreateChild<LineEdit>();
	lineEdit->SetName(parameterName);
	lineEdit->SetText(value);
	lineEdit->SetStyleAuto();
	lineEdit->SubscribeToEvent(lineEdit, E_TEXTFINISHED, URHO3D_HANDLER(Config, OnStringChanged));
}

void Config::CreateParameterEnum(const Urho3D::String& parameterName,
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
	dropDownList->SubscribeToEvent(dropDownList, E_ITEMSELECTED, URHO3D_HANDLER(Config, OnEnumChanged));

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

void Config::OnBoolChanged(Urho3D::StringHash, Urho3D::VariantMap& eventData)
{
	using namespace Toggled;
	const CheckBox* checkBox = static_cast<const CheckBox*>(eventData[P_ELEMENT].GetPtr());
	const String& name = checkBox->GetName();
	changedParameters_[name] = eventData[P_STATE].GetBool();
}

void Config::OnEnumChanged(Urho3D::StringHash, Urho3D::VariantMap& eventData)
{
	using namespace ItemSelected;
	const DropDownList* dropDownList = static_cast<const DropDownList*>(eventData[P_ELEMENT].GetPtr());
	const String& name = dropDownList->GetName();
	const int itemId = eventData[P_SELECTION].GetInt();
	const UIElement* item = dropDownList->GetItem(itemId);
	const Variant& value = item->GetVar(CB_ITEM_VALUE);
	changedParameters_[name] = value;
}

void Config::OnFloatSliderChanged(Urho3D::StringHash, Urho3D::VariantMap& eventData)
{
	using namespace SliderChanged;
	const Slider* slider = static_cast<const Slider*>(eventData[P_ELEMENT].GetPtr());
	const String& name = slider->GetName();
	const float value = eventData[P_VALUE].GetFloat();
	changedParameters_[name] = value;
	LineEdit* lineEdit = slider->GetParent()->GetChildStaticCast<LineEdit>(1);
	lineEdit->SetText(ToString("%f", value).Substring(0, 4));
}

void Config::OnFloatTextChanged(Urho3D::StringHash, Urho3D::VariantMap& eventData)
{
	using namespace TextChanged;
	const LineEdit* lineEdit = static_cast<const LineEdit*>(eventData[P_ELEMENT].GetPtr());
	Slider* slider = lineEdit->GetParent()->GetChildStaticCast<Slider>(0);
	const String& value = eventData[P_TEXT].GetString();
	slider->SetValue(ToFloat(value));
}

void Config::OnStringChanged(Urho3D::StringHash, Urho3D::VariantMap& eventData)
{
	using namespace TextFinished;
	const LineEdit* lineEdit = static_cast<const LineEdit*>(eventData[P_ELEMENT].GetPtr());
	Slider* slider = lineEdit->GetParent()->GetChildStaticCast<Slider>(0);
	const String& name = slider->GetName();
	const String& value = eventData[P_TEXT].GetString();
	changedParameters_[name] = value;
}
