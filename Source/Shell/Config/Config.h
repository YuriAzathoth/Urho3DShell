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

#ifndef CONFIG_H
#define CONFIG_H

#include <Urho3D/Core/Object.h>
#include "ComplexParameterStorage.h"
#include "ConfigTypes.h"
#include "DynamicParameter.h"
#include "EnumVariant.h"
#include "Urho3DShellAPI.h"

namespace Urho3D
{
class Deserializer;
class JSONValue;
class Serializer;
class UIElement;
class XMLElement;
} // namespace Urho3D

class URHO3DSHELLAPI_EXPORT Config : public Urho3D::Object
{
	URHO3D_OBJECT(Config, Urho3D::Object)

public:
	using Urho3D::Object::Object;

	void Initialize(Urho3D::VariantMap& engineParameters,
					Urho3D::VariantMap& shellParameters,
					const Urho3D::XMLElement& source);

	bool Load(Urho3D::Deserializer& source);
	bool Save(Urho3D::Serializer& dest) const;
	bool LoadXML(const Urho3D::XMLElement& source);
	bool SaveXML(Urho3D::XMLElement& dest) const;
	bool LoadJSON(const Urho3D::JSONValue& source);
	bool SaveJSON(Urho3D::JSONValue& dest) const;

	void Apply(const Urho3D::VariantMap& parameters);
	void Apply(Urho3D::StringHash name, const Urho3D::Variant& value);
	void ApplyComplex();

	void RegisterSettingsTab(const Urho3D::String& tabName);
	void RemoveSettingsTab(Urho3D::StringHash tab);
	Urho3D::StringVector GetSettingsTabs() const;
	Urho3D::StringVector GetSettings(Urho3D::StringHash settingsTab) const;

	bool RegisterParameter(DynamicParameter* parameter, const Urho3D::String& name, Urho3D::StringHash settingsTab);

	bool RegisterSimpleParameter(const Urho3D::String& name,
								 Urho3D::VariantType type,
								 Urho3D::StringHash settingsTab,
								 bool isEngine,
								 SimpleReaderFunc&& reader,
								 SimpleWriterFunc&& writer);

	bool RegisterSimpleEnumParameter(const Urho3D::String& name,
									 Urho3D::VariantType type,
									 Urho3D::StringHash settingsTab,
									 bool isEngine,
									 bool isLocalized,
									 SimpleReaderFunc&& reader,
									 SimpleWriterFunc&& writer,
									 EnumConstructorFunc&& enumer);

	bool RegisterComplexParameter(const Urho3D::String& name,
								  Urho3D::VariantType type,
								  Urho3D::StringHash settingsTab,
								  bool isEngine,
								  Urho3D::WeakPtr<ComplexParameterStorage> storage,
								  SimpleReaderFunc&& reader);

	bool RegisterComplexEnumParameter(const Urho3D::String& name,
									  Urho3D::VariantType type,
									  Urho3D::StringHash settingsTab,
									  bool isEngine,
									  bool isLocalized,
									  Urho3D::WeakPtr<ComplexParameterStorage> storage,
									  SimpleReaderFunc&& reader,
									  EnumConstructorFunc&& enumer);

	void RemoveParameter(Urho3D::StringHash parameter);

	void RegisterEnum(Urho3D::StringHash parameter, EnumConstructorFunc enumConstructor);

	Urho3D::WeakPtr<ComplexParameterStorage> RegisterComplexStorage(Urho3D::StringHash cathegory,
																	ComplexWriterFunc writer);
	Urho3D::WeakPtr<ComplexParameterStorage> GetComplexStorage(Urho3D::StringHash cathegory);
	void RemoveComplexStorage(Urho3D::StringHash cathegory);

	const Urho3D::String& GetName(Urho3D::StringHash parameter) const;
	Urho3D::VariantType GetType(Urho3D::StringHash parameter) const;
	bool IsEnum(Urho3D::StringHash parameter) const;
	bool IsEngine(Urho3D::StringHash parameter) const;
	bool IsLocalized(Urho3D::StringHash parameter) const;
	Urho3D::Variant ReadValue(Urho3D::StringHash parameter) const;
	void WriteValue(Urho3D::StringHash parameter, const Urho3D::Variant& value);
	EnumVector ConstructEnum(Urho3D::StringHash parameter) const;

	Urho3D::String GetDebugString() const;

	void RegisterClientParameters();
	void RegisterServerParameters();

private:
	Urho3D::HashMap<Urho3D::StringHash, Urho3D::SharedPtr<DynamicParameter>> parameters_;
	Urho3D::HashMap<Urho3D::StringHash, EnumConstructorFunc> enumConstructors_;
	Urho3D::HashMap<Urho3D::StringHash, Urho3D::SharedPtr<ComplexParameterStorage>> storages_;
	Urho3D::HashMap<Urho3D::StringHash, Urho3D::PODVector<Urho3D::StringHash>> settings_;
	Urho3D::StringMap names_;

private:
	static Urho3D::IntVector3 StrToRes(const Urho3D::String& str);
	static Urho3D::String ResToStr(const Urho3D::IntVector3& res);
};

#endif // CONFIG_H
