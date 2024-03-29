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
#include <Urho3D/Engine/EngineDefs.h>
#include <Urho3D/IO/Deserializer.h>
#include <Urho3D/IO/Log.h>
#include <Urho3D/IO/Serializer.h>
#include <Urho3D/Resource/JSONValue.h>
#include <Urho3D/Resource/XMLElement.h>
#include "BinaryParameter.h"
#include "Config.h"
#include "ConfigDefs.h"

using namespace Urho3D;

void Config::Initialize(Urho3D::VariantMap& engineParameters,
						Urho3D::VariantMap& shellParameters,
						const Urho3D::XMLElement& source)
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
			(IsEngine(name) ? engineParameters : shellParameters)[name] = value;
	}

	auto it = engineParameters.Find(ECP_RESOLUTION);
	if (it != engineParameters.End())
	{
		const IntVector3 resolution = StrToRes(it->second_.GetString());
		engineParameters[EP_WINDOW_WIDTH] = resolution.x_;
		engineParameters[EP_WINDOW_HEIGHT] = resolution.y_;
		engineParameters[EP_REFRESH_RATE] = resolution.z_;
		engineParameters.Erase(ECP_RESOLUTION);
	}
	it = engineParameters.Find(ECP_WINDOW_MODE);
	if (it != engineParameters.End())
	{
		const int windowMode = it->second_.GetInt();
		engineParameters[EP_FULL_SCREEN] = windowMode >= 1;
		engineParameters[EP_BORDERLESS] = windowMode == 2;
		engineParameters.Erase(ECP_WINDOW_MODE);
	}
}

bool Config::Load(Urho3D::Deserializer& source)
{
	const unsigned size = source.ReadUInt();
	if (!size)
		return false;
	StringHash name;
	Variant value;
	for (unsigned i = 0; i < size; ++i)
	{
		name = source.ReadStringHash();
		value = source.ReadVariant();
		Apply(name, value);
	}
	return true;
}

bool Config::Save(Urho3D::Serializer& dest) const
{
	dest.WriteUInt(parameters_.Size());
	for (const auto& it : parameters_)
	{
		dest.WriteStringHash(it.first_);
		dest.WriteVariant(it.second_->Read());
	}
	return true;
}

bool Config::LoadXML(const Urho3D::XMLElement& source)
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
			Apply(name, value);
	}
	return true;
}

bool Config::SaveXML(Urho3D::XMLElement& dest) const
{
	XMLElement parameter;
	for (const auto& p : parameters_)
	{
		parameter = dest.CreateChild("parameter");
		parameter.SetAttribute("name", names_.Find(p.first_)->second_);
		parameter.SetVariant(p.second_->Read());
	}
	return true;
}

bool Config::LoadJSON(const Urho3D::JSONValue& source)
{
	String name;
	Variant value;
	const JSONValue& array = source.Get("Array");
	for (const JSONValue& parameter : array.GetArray())
	{
		name = parameter.Get("name").GetString("");
		value = parameter.GetVariant();
		if (name.Empty())
			URHO3D_LOGWARNING("Failed to setup config parameter: name is empty.");
		else if (value.IsEmpty())
			URHO3D_LOGWARNINGF("Failed to setup config parameter %s: value is empty.", name.CString());
		else if (!parameters_.Contains(name))
			URHO3D_LOGWARNINGF("Failed to setup config parameter %s: parameter is not registered yet.", name.CString());
		else
			Apply(name, value);
	}
	return true;
}

bool Config::SaveJSON(Urho3D::JSONValue& dest) const
{
	JSONArray array;
	JSONValue parameter;
	for (const auto& p : parameters_)
	{
		parameter.Set("name", names_.Find(p.first_)->second_);
		parameter.SetVariant(p.second_->Read());
		array.Push(parameter);
	}
	dest.Set("config", array);
	return true;
}

void Config::Apply(const Urho3D::VariantMap& parameters)
{
	for (const auto& p : parameters)
		Apply(p.first_, p.second_);
	ApplyComplex();
}

void Config::Apply(Urho3D::StringHash name, const Urho3D::Variant& value)
{
	auto it = parameters_.Find(name);
	if (it != parameters_.End())
		it->second_->Write(value);
}

void Config::ApplyComplex()
{
	for (auto& p : storages_)
		p.second_->Apply();
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
		ret.Reserve(it->second_.Size());
		for (StringHash setting : it->second_)
			ret.Push(*names_[setting]);
		return ret;
	}
	else
	{
		URHO3D_LOGERRORF("Failed to load unregistered settings tab parameters.");
		return Variant::emptyStringVector;
	}
}

DynamicParameter* Config::GetParameter(Urho3D::StringHash parameter) const
{
	const auto it = parameters_.Find(parameter);
	return it != parameters_.End() ? it->second_ : nullptr;
}

bool Config::RegisterParameter(DynamicParameter* parameter, const Urho3D::String& name, Urho3D::StringHash settingsTab)
{
	auto itSettingsTab = settings_.Find(settingsTab);
	if (itSettingsTab == settings_.End())
	{
		URHO3D_LOGERRORF("Failed to register config parameter %s in unknown settings tab.", name.CString());
		return false;
	}
	if (parameters_.Contains(name))
	{
		URHO3D_LOGERRORF("Failed to register already registered config parameter %s.", name.CString());
		return false;
	}
	parameters_[name] = parameter;
	names_[name] = name;
	itSettingsTab->second_.Push(name);
	return true;
}

void Config::RemoveParameter(Urho3D::StringHash parameter)
{
	auto itParameter = parameters_.Find(parameter);
	if (itParameter != parameters_.End())
	{
		auto itSetting = settings_.Find(itParameter->second_->GetSettingsTab());
		if (itSetting == settings_.End())
			names_.Erase(parameter);
		else
			settings_.Erase(itSetting);
		enumConstructors_.Erase(itParameter->first_);
		parameters_.Erase(itParameter);
	}
	else
		URHO3D_LOGWARNING("Failed to remove non-existent config parameter.");
}

EnumConstructor* Config::GetEnum(Urho3D::StringHash parameter) const
{
	const auto it = enumConstructors_.Find(parameter);
	return it != enumConstructors_.End() ? it->second_ : nullptr;
}

bool Config::RegisterEnum(EnumConstructor* constructor, Urho3D::StringHash parameter)
{
	if (parameters_.Contains(parameter))
	{
		enumConstructors_[parameter] = constructor;
		return true;
	}
	else
	{
		URHO3D_LOGWARNING("Failed to assign enum constructor to non-existent config parameter.");
		return false;
	}
}

void Config::RemoveEnum(Urho3D::StringHash parameter) { enumConstructors_.Erase(parameter); }

Urho3D::WeakPtr<ComplexParameter> Config::GetComplexStorage(Urho3D::StringHash cathegory) const
{
	const auto it = storages_.Find(cathegory);
	return it != storages_.End() ? it->second_ : nullptr;
}

bool Config::RegisterComplexStorage(ComplexParameter* storage, Urho3D::StringHash cathegory)
{
	if (!storages_.Contains(cathegory))
	{
		storages_[cathegory] = storage;
		return true;
	}
	else
	{
		URHO3D_LOGWARNING("Failed to register already existent complex config storage.");
		return false;
	}
}

void Config::RemoveComplexStorage(Urho3D::StringHash cathegory) { storages_.Erase(cathegory); }

const Urho3D::String& Config::GetName(Urho3D::StringHash parameter) const
{
	const auto it = names_.Find(parameter);
	return it != names_.End() ? it->second_ : Urho3D::String::EMPTY;
}

Urho3D::VariantType Config::GetType(Urho3D::StringHash parameter) const
{
	const auto it = parameters_.Find(parameter);
	return it != parameters_.End() ? it->second_->GetType() : Urho3D::VariantType::VAR_NONE;
}

bool Config::IsEngine(Urho3D::StringHash parameter) const
{
	const auto it = parameters_.Find(parameter);
	return it != parameters_.End() ? it->second_->IsEngine() : false;
}

bool Config::IsEnum(Urho3D::StringHash parameter) const { return enumConstructors_.Contains(parameter); }

bool Config::IsLocalized(Urho3D::StringHash parameter) const
{
	const auto it = enumConstructors_.Find(parameter);
	return it != parameters_.End() ? it->second_->IsLocalized() : false;
}

Urho3D::Variant Config::ReadValue(Urho3D::StringHash parameter) const
{
	const auto it = parameters_.Find(parameter);
	return it != parameters_.End() ? it->second_->Read() : Urho3D::Variant::EMPTY;
}

void Config::WriteValue(Urho3D::StringHash parameter, const Urho3D::Variant& value)
{
	const auto it = parameters_.Find(parameter);
	if (it != parameters_.End())
		it->second_->Write(value);
}

EnumVector Config::ConstructEnum(Urho3D::StringHash parameter) const
{
	const auto it = enumConstructors_.Find(parameter);
	return it != enumConstructors_.End() ? it->second_->Create() : EnumVector{};
}

Urho3D::String Config::GetDebugString() const
{
	String ret;
	StringVector tabs = GetSettingsTabs();
	StringVector parameters;
	Variant value;
	EnumVector enumVector;
	SharedPtr<DynamicParameter> parameter;
	for (const String& tabName : tabs)
	{
		ret.Append(tabName).Append('\n');
		parameters = GetSettings(tabName);
		for (const String& parameterName : parameters)
		{
			parameter = *parameters_[parameterName];
			value = parameter->Read();
			ret.Append("\t").Append(parameterName).Append('\n');
			ret.Append("\t\tType  = ").Append(value.GetTypeName()).Append('\n');
			ret.Append("\t\tValue = ").Append(value.ToString()).Append('\n');
			ret.Append("\t\tStore = ");
			if (parameter->IsEngine())
				ret.Append("ENGINE\n");
			else
				ret.Append("CUSTOM\n");
			if (IsEnum(parameterName))
			{
				ret.Append("\t\tEnum Variants:\n");
				enumVector = (*enumConstructors_[parameterName])->Create();
				for (const EnumVariant& enumVariant : enumVector)
					ret.Append("\t\t\t")
						.Append(enumVariant.caption_)
						.Append(" = ")
						.Append(enumVariant.value_.ToString())
						.Append('\n');
			}
		}
	}
	return ret;
}

bool Config::RegisterSimpleParameter(const Urho3D::String& name,
									 Urho3D::VariantType type,
									 Urho3D::StringHash settingsTab,
									 bool isEngine,
									 SimpleReaderFunc&& reader,
									 SimpleWriterFunc&& writer)
{
	return RegisterParameter(
		new SimpleBinaryParameter(std::move(reader), std::move(writer), type, settingsTab, isEngine),
		name,
		settingsTab);
}

bool Config::RegisterSimpleEnumParameter(const Urho3D::String& name,
										 Urho3D::VariantType type,
										 Urho3D::StringHash settingsTab,
										 bool isEngine,
										 bool isLocalized,
										 SimpleReaderFunc&& reader,
										 SimpleWriterFunc&& writer,
										 EnumConstructorFunc&& enumer)
{
	const bool success =
		RegisterParameter(new SimpleBinaryParameter(std::move(reader), std::move(writer), type, settingsTab, isEngine),
						  name,
						  settingsTab);
	if (success)
		RegisterEnum(new BinaryEnumConstructor(std::move(enumer), isLocalized), name);
	return success;
}

bool Config::RegisterSimpleEnumParameter(const Urho3D::String& name,
										 Urho3D::VariantType type,
										 Urho3D::StringHash settingsTab,
										 bool isEngine,
										 bool isLocalized,
										 SimpleReaderFunc&& reader,
										 SimpleWriterFunc&& writer,
										 EnumVector&& enumVector)
{
	const bool success =
		RegisterParameter(new SimpleBinaryParameter(std::move(reader), std::move(writer), type, settingsTab, isEngine),
						  name,
						  settingsTab);
	if (success)
		RegisterEnum(new StaticEnumConstructor(std::move(enumVector), isLocalized), name);
	return success;
}

bool Config::RegisterComplexParameter(const Urho3D::String& name,
									  Urho3D::VariantType type,
									  Urho3D::StringHash settingsTab,
									  bool isEngine,
									  ComplexParameter* storage,
									  SimpleReaderFunc&& reader)
{
	return RegisterParameter(new ComplexBinaryParameter(storage, std::move(reader), name, type, settingsTab, isEngine),
							 name,
							 settingsTab);
}

bool Config::RegisterComplexEnumParameter(const Urho3D::String& name,
										  Urho3D::VariantType type,
										  Urho3D::StringHash settingsTab,
										  bool isEngine,
										  bool isLocalized,
										  ComplexParameter* storage,
										  SimpleReaderFunc&& reader,
										  EnumConstructorFunc&& enumer)
{
	const bool success =
		RegisterParameter(new ComplexBinaryParameter(storage, std::move(reader), name, type, settingsTab, isEngine),
						  name,
						  settingsTab);
	if (success)
		RegisterEnum(new BinaryEnumConstructor(std::move(enumer), isLocalized), name);
	return success;
}

bool Config::RegisterComplexEnumParameter(const Urho3D::String& name,
										  Urho3D::VariantType type,
										  Urho3D::StringHash settingsTab,
										  bool isEngine,
										  bool isLocalized,
										  ComplexParameter* storage,
										  SimpleReaderFunc&& reader,
										  EnumVector&& enumVector)
{
	const bool success =
		RegisterParameter(new ComplexBinaryParameter(storage, std::move(reader), name, type, settingsTab, isEngine),
						  name,
						  settingsTab);
	if (success)
		RegisterEnum(new StaticEnumConstructor(std::move(enumVector), isLocalized), name);
	return success;
}

ComplexParameter*
Config::RegisterBinaryComplexStorage(Urho3D::StringHash cathegory, bool engine, ComplexWriterFunc&& writer)
{
	ComplexParameter* ret = new BinaryComplexParameter(std::move(writer), engine);
	RegisterComplexStorage(ret, cathegory);
	return ret;
}

Urho3D::IntVector3 Config::StrToRes(const Urho3D::String& str)
{
	const StringVector firstSplit = str.Split('x', true);
	const StringVector secondSplit = firstSplit[1].Split(':', true);
	return {ToInt(firstSplit[0]), ToInt(secondSplit[0]), ToInt(secondSplit[1])};
}

Urho3D::String Config::ResToStr(const Urho3D::IntVector3& res) { return ToString("%dx%d:%d", res.x_, res.y_, res.z_); }
