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

#include <Urho3D/AngelScript/Generated_Members.h>
#include "Plugin/PluginsRegistry.h"

using namespace Urho3D;

static PluginsRegistry* CreatePluginsRegistry() { return new PluginsRegistry(GetScriptContext()); }

static PluginsRegistry* GetPluginsRegistry() { return GetScriptContext()->GetSubsystem<PluginsRegistry>(); }

template <typename T> static CScriptArray* ScanPlugins(T* _ptr)
{
	const StringVector& result = _ptr->ScanPlugins();
	return VectorToArray<String>(result, "Array<String>");
}

template <typename T> static CScriptArray* GetPluginsList(T* _ptr)
{
	const StringVector& result = _ptr->GetAllNames();
	return VectorToArray<String>(result, "Array<String>");
}

void RegisterPluginsRegistryAPI(asIScriptEngine* engine)
{
	engine->RegisterObjectType("PluginsRegistry", 0, asOBJ_REF);

	engine->RegisterObjectBehaviour("PluginsRegistry",
									asBEHAVE_FACTORY,
									"PluginsRegistry@+ f()",
									AS_FUNCTION(CreatePluginsRegistry),
									AS_CALL_CDECL);

	engine->RegisterGlobalFunction("PluginsRegistry@+ get_plugins()", AS_FUNCTION(GetPluginsRegistry), AS_CALL_CDECL);

	RegisterSubclass<Object, PluginsRegistry>(engine, "Object", "PluginsRegistry");
	RegisterSubclass<RefCounted, PluginsRegistry>(engine, "RefCounted", "PluginsRegistry");

	RegisterMembers_Object<PluginsRegistry>(engine, "PluginsRegistry");

	engine->RegisterObjectMethod("PluginsRegistry",
								 "bool LoadPlugin(const String&in)",
								 AS_METHOD(PluginsRegistry, LoadPlugin),
								 AS_CALL_THISCALL);
	engine->RegisterObjectMethod("PluginsRegistry",
								 "Array<String>@+ ScanPlugins() const",
								 AS_FUNCTION_OBJFIRST(ScanPlugins<PluginsRegistry>),
								 AS_CALL_CDECL_OBJFIRST);

	engine->RegisterObjectMethod("PluginsRegistry",
								 "bool Load(const String&in)",
								 AS_METHOD(PluginsRegistry, Load),
								 AS_CALL_THISCALL);
	engine->RegisterObjectMethod("PluginsRegistry",
								 "void Close(StringHash)",
								 AS_METHOD(PluginsRegistry, Close),
								 AS_CALL_THISCALL);
	engine->RegisterObjectMethod("PluginsRegistry",
								 "Array<String>@+ get_list() const",
								 AS_FUNCTION_OBJFIRST(GetPluginsList<PluginsRegistry>),
								 AS_CALL_CDECL_OBJFIRST);
	engine->RegisterObjectMethod("PluginsRegistry",
								 "void CloseAll()",
								 AS_METHOD(PluginsRegistry, CloseAll),
								 AS_CALL_THISCALL);
}
