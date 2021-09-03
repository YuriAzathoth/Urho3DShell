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
#include "Config/Config.h"
#include "Config/ScriptParameter.h"

using namespace Urho3D;

static void RegisterObjectTypes(asIScriptEngine* engine)
{
	engine->RegisterObjectType("Config", 0, asOBJ_REF);
	engine->RegisterObjectType("DynamicParameter", 0, asOBJ_REF);
	engine->RegisterObjectType("EnumVariant", sizeof(EnumVariant), asOBJ_VALUE | asGetTypeTraits<AllocatorNode>());
}

static Config* GetConfig() { return GetScriptContext()->GetSubsystem<Config>(); }

static void CreateEnumVariantV(EnumVariant* ptr) { new (ptr) EnumVariant; }
static void CreateEnumVariantSVar(EnumVariant* ptr, const String& caption, const Variant& value)
{
	new (ptr) EnumVariant(caption, value);
}

static void RegisterClasses(asIScriptEngine* engine)
{
	engine->RegisterGlobalFunction("Config@+ get_config()", AS_FUNCTION(GetConfig), AS_CALL_CDECL);
	RegisterSubclass<Object, Config>(engine, "Object", "Config");
	RegisterSubclass<RefCounted, Config>(engine, "RefCounted", "Config");

	RegisterSubclass<RefCounted, DynamicParameter>(engine, "RefCounted", "DynamicParameter");

	engine->RegisterObjectBehaviour("EnumVariant",
									asBEHAVE_CONSTRUCT,
									"void f()",
									AS_FUNCTION_OBJFIRST(CreateEnumVariantV),
									AS_CALL_CDECL_OBJFIRST);
	engine->RegisterObjectBehaviour("EnumVariant",
									asBEHAVE_CONSTRUCT,
									"void f(const String& in, const Variant&in)",
									AS_FUNCTION_OBJFIRST(CreateEnumVariantSVar),
									AS_CALL_CDECL_OBJFIRST);
	engine->RegisterObjectBehaviour("EnumVariant",
									asBEHAVE_DESTRUCT,
									"void f()",
									AS_DESTRUCTOR(EnumVariant),
									AS_CALL_CDECL_OBJFIRST);
}

template <typename T> static CScriptArray* ConstructEnum(T* _ptr, StringHash parameter)
{
	const EnumVector result = _ptr->ConstructEnum(parameter);
	return VectorToArray<EnumVariant>(result, "Array<EnumVariant>");
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

static void RegisterParameter(Config* config,
							  const String& name,
							  VariantType type,
							  StringHash settingsTab,
							  bool isEngine,
							  const String& readerFunc,
							  const String& writerFunc)
{
	SharedPtr<ScriptParameter> parameter =
		MakeShared<ScriptParameter>(type, settingsTab, isEngine, readerFunc, writerFunc);
	if (parameter->Success())
		config->RegisterParameter(parameter, name, settingsTab);
}

#if defined(__GNUC__) || defined(__GNUG__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wconversion"
#endif // defined (__GNUC__) || defined(__GNUG__)

#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wconversion"
#endif // defined(__clang__)

template <typename T> void RegisterMembers_Config(asIScriptEngine* engine, const char* className)
{
	RegisterMembers_Object<Config>(engine, "Config");

	engine->RegisterObjectMethod(className, "bool Load(const Serializer&in)", AS_METHOD(T, Load), AS_CALL_THISCALL);
	engine->RegisterObjectMethod(className, "bool Save(Deserializer&out) const", AS_METHOD(T, Save), AS_CALL_THISCALL);
	engine->RegisterObjectMethod(className,
								 "bool LoadXML(const XMLElement&in)",
								 AS_METHODPR(T, LoadXML, (const XMLElement&), bool),
								 AS_CALL_THISCALL);
	engine->RegisterObjectMethod(className,
								 "bool SaveXML(XMLElement&out) const",
								 AS_METHOD(T, SaveXML),
								 AS_CALL_THISCALL);
	engine->RegisterObjectMethod(className,
								 "bool LoadJSON(const JSONValue&in)",
								 AS_METHOD(T, LoadJSON),
								 AS_CALL_THISCALL);
	engine->RegisterObjectMethod(className,
								 "bool SaveJSON(JSONValue&out) const",
								 AS_METHOD(T, SaveJSON),
								 AS_CALL_THISCALL);

	engine->RegisterObjectMethod(className,
								 "void Apply(const VariantMap&in)",
								 AS_METHODPR(T, Apply, (const VariantMap&), void),
								 AS_CALL_THISCALL);
	engine->RegisterObjectMethod(className,
								 "void Apply(StringHash, const Variant&in)",
								 AS_METHODPR(T, Apply, (StringHash, const Variant&), void),
								 AS_CALL_THISCALL);
	engine->RegisterObjectMethod(className, "void ApplyComplex()", AS_METHOD(T, ApplyComplex), AS_CALL_THISCALL);

	engine->RegisterObjectMethod(className,
								 "void RegisterSettingsTab(const String&in)",
								 AS_METHOD(T, RegisterSettingsTab),
								 AS_CALL_THISCALL);
	engine->RegisterObjectMethod(className,
								 "void RemoveSettingsTab(StringHash)",
								 AS_METHOD(T, RemoveSettingsTab),
								 AS_CALL_THISCALL);
	engine->RegisterObjectMethod(className,
								 "Array<String>@ get_tabs() const",
								 AS_FUNCTION_OBJFIRST(GetConfigTabs<T>),
								 AS_CALL_CDECL_OBJFIRST);
	engine->RegisterObjectMethod(className,
								 "Array<String>@ GetSettings(StringHash)",
								 AS_FUNCTION_OBJFIRST(GetConfigSettings<T>),
								 AS_CALL_CDECL_OBJFIRST);

	engine->RegisterObjectMethod(
		className,
		"void RegisterParameter(const String&in, VariantType, StringHash, bool, const String&in, const String&in)",
		AS_FUNCTION_OBJFIRST(RegisterParameter),
		AS_CALL_CDECL_OBJFIRST);
	engine->RegisterObjectMethod(className,
								 "void RemoveParameter(StringHash)",
								 AS_METHOD(T, RemoveParameter),
								 AS_CALL_THISCALL);
	engine->RegisterObjectMethod(className,
								 "DynamicParameter& GetParameter(StringHash) const",
								 AS_METHOD(T, GetParameter),
								 AS_CALL_THISCALL);

	engine->RegisterObjectMethod(className,
								 "const String& GetName(StringHash) const",
								 AS_METHODPR(T, GetName, (StringHash) const, const String&),
								 AS_CALL_THISCALL);
	engine->RegisterObjectMethod(className,
								 "VariantType GetType(StringHash) const",
								 AS_METHODPR(T, GetType, (StringHash) const, VariantType),
								 AS_CALL_THISCALL);
	engine->RegisterObjectMethod(className, "bool IsEnum(StringHash) const", AS_METHOD(T, IsEnum), AS_CALL_THISCALL);
	engine->RegisterObjectMethod(className,
								 "bool IsLocalized(StringHash) const",
								 AS_METHOD(T, IsLocalized),
								 AS_CALL_THISCALL);
	engine->RegisterObjectMethod(className,
								 "Variant ReadValue(StringHash) const",
								 AS_METHOD(T, ReadValue),
								 AS_CALL_THISCALL);
	engine->RegisterObjectMethod(className,
								 "void WriteValue(StringHash, const Variant&in)",
								 AS_METHOD(T, ReadValue),
								 AS_CALL_THISCALL);

	// TODO: Fix creating Array<EnumVariant> contains empty data
	engine->RegisterObjectMethod(className,
								 "Array<EnumVariant>@ ConstructEnum(StringHash)",
								 AS_FUNCTION_OBJFIRST(ConstructEnum<T>),
								 AS_CALL_CDECL_OBJFIRST);

	engine->RegisterObjectMethod(className,
								 "String get_debugString() const",
								 AS_METHOD(T, GetDebugString),
								 AS_CALL_THISCALL);
}

template <typename T> void RegisterMembers_EnumVariant(asIScriptEngine* engine, const char* className)
{
	engine->RegisterObjectMethod(className,
								 "EnumVariant& opAssign(const EnumVariant&in) const",
								 AS_METHODPR(T, operator=, (const EnumVariant&), EnumVariant&),
								 AS_CALL_THISCALL);
	engine->RegisterObjectProperty(className, "String caption", offsetof(T, caption_));
	engine->RegisterObjectProperty(className, "Variant value", offsetof(T, value_));
}

template <typename T> void RegisterMembers_DynamicParameter(asIScriptEngine* engine, const char* className)
{
	RegisterMembers_RefCounted<T>(engine, className);
	engine->RegisterObjectMethod(className, "Variant Read()", AS_METHOD(T, Read), AS_CALL_THISCALL);
	engine->RegisterObjectMethod(className, "void Write(const Variant&in)", AS_METHOD(T, Write), AS_CALL_THISCALL);
	engine->RegisterObjectMethod(className,
								 "StringHash get_settingsTab() const",
								 AS_METHOD(T, GetSettingsTab),
								 AS_CALL_THISCALL);
	engine->RegisterObjectMethod(className, "VariantType get_type() const", AS_METHOD(T, GetType), AS_CALL_THISCALL);
	engine->RegisterObjectMethod(className, "bool get_engine() const", AS_METHOD(T, IsEngine), AS_CALL_THISCALL);
}

#if defined(__GNUC__) || defined(__GNUG__)
#pragma GCC diagnostic pop
#endif // defined (__GNUC__) || defined(__GNUG__)

#if defined(__clang__)
#pragma clang diagnostic pop
#endif // defined(__clang__)

static void RegisterMembers(asIScriptEngine* engine)
{
	RegisterMembers_DynamicParameter<DynamicParameter>(engine, "DynamicParameter");
	RegisterMembers_EnumVariant<EnumVariant>(engine, "EnumVariant");
	RegisterMembers_Config<Config>(engine, "Config");
}

void RegisterConfigAPI(asIScriptEngine* engine)
{
	RegisterObjectTypes(engine);
	RegisterClasses(engine);
	RegisterMembers(engine);
}
