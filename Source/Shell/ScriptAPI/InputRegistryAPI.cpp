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
#include "Input/InputRegistry.h"

using namespace Urho3D;

static InputRegistry* CreateInputRegistry() { return new InputRegistry(GetScriptContext()); }

static InputRegistry* GetInputRegistry() { return GetScriptContext()->GetSubsystem<InputRegistry>(); }

template <typename T> static CScriptArray* GetActions(T* _ptr)
{
	const StringVector& result = _ptr->GetActions();
	return VectorToArray<String>(result, "Array<String>");
}

void RegisterInputRegistryAPI(asIScriptEngine* engine)
{
	engine->RegisterObjectType("InputRegistry", 0, asOBJ_REF);

	engine->RegisterObjectBehaviour("InputRegistry",
									asBEHAVE_FACTORY,
									"InputRegistry@+ f()",
									AS_FUNCTION(CreateInputRegistry),
									AS_CALL_CDECL);

	engine->RegisterGlobalFunction("InputRegistry@+ get_inputRegistry()", AS_FUNCTION(GetInputRegistry), AS_CALL_CDECL);

	RegisterSubclass<Object, InputRegistry>(engine, "Object", "InputRegistry");
	RegisterSubclass<RefCounted, InputRegistry>(engine, "RefCounted", "InputRegistry");

	RegisterMembers_Object<InputRegistry>(engine, "InputRegistry");

	engine->RegisterObjectMethod("InputRegistry",
								 "void RegisterActionLocal(const String&in)",
								 AS_METHOD(InputRegistry, RegisterActionLocal),
								 AS_CALL_THISCALL);
	engine->RegisterObjectMethod("InputRegistry",
								 "uint RegisterActionRemote(const String&in)",
								 AS_METHOD(InputRegistry, RegisterActionRemote),
								 AS_CALL_THISCALL);
	engine->RegisterObjectMethod("InputRegistry",
								 "void RemoveAction(StringHash)",
								 AS_METHOD(InputRegistry, RemoveAction),
								 AS_CALL_THISCALL);
	engine->RegisterObjectMethod("InputRegistry",
								 "void RemoveAllActions()",
								 AS_METHOD(InputRegistry, RemoveAllActions),
								 AS_CALL_THISCALL);

	engine->RegisterObjectMethod("InputRegistry",
								 "uint GetActionFlag(StringHash) const",
								 AS_METHOD(InputRegistry, GetActionFlag),
								 AS_CALL_THISCALL);
	engine->RegisterObjectMethod("InputRegistry",
								 "const String& GetActionName(StringHash) const",
								 AS_METHOD(InputRegistry, GetActionName),
								 AS_CALL_THISCALL);
	engine->RegisterObjectMethod("InputRegistry",
								 "Array<String>@ get_actions() const",
								 AS_FUNCTION_OBJFIRST(GetActions<InputRegistry>),
								 AS_CALL_CDECL_OBJFIRST);

	engine->RegisterObjectMethod("InputRegistry",
								 "String get_debugString() const",
								 AS_METHOD(InputRegistry, GetDebugString),
								 AS_CALL_THISCALL);
}
