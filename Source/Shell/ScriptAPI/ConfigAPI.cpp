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

#include <Urho3D/AngelScript/Generated_Members.h>
//#include <new>
#include "Config/Config.h"

using namespace Urho3D;

static Config* CreateConfig() { return new Config(GetScriptContext()); }

static Config* GetConfig() { return GetScriptContext()->GetSubsystem<Config>(); }

template <typename T> static CScriptArray* ConstructEnum(T* _ptr, StringHash parameter)
{
	const Config::EnumVector result = _ptr->ConstructEnum(parameter);
	return VectorToArray<Config::EnumVariant>(result, "Array<EnumVariant>");
}

template <typename T> static CScriptArray* GetConfigTabs(T* _ptr)
{
	const StringVector& result = _ptr->GetSettingsTabs();
	return VectorToArray<String>(result, "Array<String>");
}

template <typename T> static CScriptArray* GetConfigSettings(T* _ptr, StringHash tab)
{
	const StringVector& result = _ptr->GetSettings(tab);
	return VectorToArray<String>(result, "Array<String>");
}

static void RegisterEnumVariantAPI(asIScriptEngine* engine);

void RegisterConfigAPI(asIScriptEngine* engine)
{
	RegisterEnumVariantAPI(engine);

	engine->RegisterObjectType("Config", 0, asOBJ_REF);

	engine->RegisterObjectBehaviour("Config",
									asBEHAVE_FACTORY,
									"Config@+ f()",
									AS_FUNCTION(CreateConfig),
									AS_CALL_CDECL);

	engine->RegisterGlobalFunction("Config@+ get_config()", AS_FUNCTION(GetConfig), AS_CALL_CDECL);

	RegisterSubclass<Object, Config>(engine, "Object", "Config");
	RegisterSubclass<RefCounted, Config>(engine, "RefCounted", "Config");

	RegisterMembers_Object<Config>(engine, "Config");

	engine->RegisterObjectMethod("Config", "bool Load(const Serializer&in)", AS_METHOD(Config, Load), AS_CALL_THISCALL);
	engine->RegisterObjectMethod("Config",
								 "bool Save(Deserializer&out) const",
								 AS_METHOD(Config, Save),
								 AS_CALL_THISCALL);
	engine->RegisterObjectMethod("Config",
								 "bool LoadXML(const XMLElement&in)",
								 AS_METHODPR(Config, LoadXML, (const XMLElement&), bool),
								 AS_CALL_THISCALL);
	engine->RegisterObjectMethod("Config",
								 "bool SaveXML(XMLElement&out) const",
								 AS_METHOD(Config, SaveXML),
								 AS_CALL_THISCALL);
	engine->RegisterObjectMethod("Config",
								 "bool LoadJSON(const JSONValue&in)",
								 AS_METHOD(Config, LoadJSON),
								 AS_CALL_THISCALL);
	engine->RegisterObjectMethod("Config",
								 "bool SaveJSON(JSONValue&out) const",
								 AS_METHOD(Config, SaveJSON),
								 AS_CALL_THISCALL);

	engine->RegisterObjectMethod("Config",
								 "void Apply(const VariantMap&in)",
								 AS_METHOD(Config, Apply),
								 AS_CALL_THISCALL);
	engine->RegisterObjectMethod("Config",
								 "void ExtractEngineParameters(VariantMap&, VariantMap&) const",
								 AS_METHOD(Config, ExtractEngineParameters),
								 AS_CALL_THISCALL);

	engine->RegisterObjectMethod("Config",
								 "void RegisterSettingsTab(const String&in)",
								 AS_METHOD(Config, RegisterSettingsTab),
								 AS_CALL_THISCALL);
	engine->RegisterObjectMethod("Config",
								 "void RemoveSettingsTab(StringHash)",
								 AS_METHOD(Config, RemoveSettingsTab),
								 AS_CALL_THISCALL);
	engine->RegisterObjectMethod("Config",
								 "Array<String>@ get_tabs() const",
								 AS_FUNCTION_OBJFIRST(GetConfigTabs<Config>),
								 AS_CALL_CDECL_OBJFIRST);
	engine->RegisterObjectMethod("Config",
								 "Array<String>@ GetSettings(StringHash)",
								 AS_FUNCTION_OBJFIRST(GetConfigSettings<Config>),
								 AS_CALL_CDECL_OBJFIRST);

	engine->RegisterObjectMethod("Config",
								 "bool RegisterParameter(const String&in, VariantType, StringHash, bool, bool)",
								 AS_METHOD(Config, RegisterParameter),
								 AS_CALL_THISCALL);
	engine->RegisterObjectMethod("Config",
								 "void RemoveParameter(StringHash)",
								 AS_METHOD(Config, RemoveParameter),
								 AS_CALL_THISCALL);

	// TODO: Script parameter Readers/Writers

	engine->RegisterObjectMethod("Config",
								 "const String& GetName(StringHash) const",
								 AS_METHODPR(Config, GetName, (StringHash) const, const String&),
								 AS_CALL_THISCALL);
	engine->RegisterObjectMethod("Config",
								 "VariantType GetType(StringHash) const",
								 AS_METHODPR(Config, GetType, (StringHash) const, VariantType),
								 AS_CALL_THISCALL);
	engine->RegisterObjectMethod("Config",
								 "bool IsEnum(StringHash) const",
								 AS_METHOD(Config, IsEnum),
								 AS_CALL_THISCALL);
	engine->RegisterObjectMethod("Config",
								 "bool IsLocalized(StringHash) const",
								 AS_METHOD(Config, IsLocalized),
								 AS_CALL_THISCALL);
	engine->RegisterObjectMethod("Config",
								 "Variant ReadValue(StringHash) const",
								 AS_METHOD(Config, ReadValue),
								 AS_CALL_THISCALL);
	engine->RegisterObjectMethod("Config",
								 "void WriteValue(StringHash, const Variant&in)",
								 AS_METHOD(Config, ReadValue),
								 AS_CALL_THISCALL);

	// TODO: Fix creating Array<EnumVariant> contains empty data
	engine->RegisterObjectMethod("Config",
								 "Array<EnumVariant>@ ConstructEnum(StringHash)",
								 AS_FUNCTION_OBJFIRST(ConstructEnum<Config>),
								 AS_CALL_CDECL_OBJFIRST);

	engine->RegisterObjectMethod("Config",
								 "String get_debugString() const",
								 AS_METHOD(Config, GetDebugString),
								 AS_CALL_THISCALL);
}

using EnumVariant = Config::EnumVariant;

static void NewEnumVariantV(EnumVariant* ptr) { new (ptr) EnumVariant; }
static void NewEnumVariantSVar(EnumVariant* ptr, const String& caption, const Variant& value)
{
	new (ptr) EnumVariant(caption, value);
}

void RegisterEnumVariantAPI(asIScriptEngine* engine)
{
	engine->RegisterObjectType("EnumVariant", sizeof(EnumVariant), asOBJ_VALUE | asGetTypeTraits<AllocatorNode>());

	engine->RegisterObjectBehaviour("EnumVariant",
									asBEHAVE_CONSTRUCT,
									"void f()",
									AS_FUNCTION_OBJFIRST(NewEnumVariantV),
									AS_CALL_CDECL_OBJFIRST);
	engine->RegisterObjectBehaviour("EnumVariant",
									asBEHAVE_CONSTRUCT,
									"void f(const String& in, const Variant&in)",
									AS_FUNCTION_OBJFIRST(NewEnumVariantSVar),
									AS_CALL_CDECL_OBJFIRST);
	engine->RegisterObjectBehaviour("EnumVariant",
									asBEHAVE_DESTRUCT,
									"void f()",
									AS_DESTRUCTOR(EnumVariant),
									AS_CALL_CDECL_OBJFIRST);

	engine->RegisterObjectMethod("EnumVariant",
								 "EnumVariant& opAssign(const EnumVariant&in) const",
								 AS_METHODPR(EnumVariant, operator=, (const EnumVariant&), EnumVariant&),
								 AS_CALL_THISCALL);

	engine->RegisterObjectProperty("EnumVariant", "String caption", offsetof(EnumVariant, caption_));
	engine->RegisterObjectProperty("EnumVariant", "Variant value", offsetof(EnumVariant, value_));
}
