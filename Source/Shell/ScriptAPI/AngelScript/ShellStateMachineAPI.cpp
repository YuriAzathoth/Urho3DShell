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
#include "ShellState/ShellStateMachine.h"

using namespace Urho3D;

extern void RegisterShellStateAPI(asIScriptEngine* engine);

static ShellStateMachine* CreateShellStateMachine() { return new ShellStateMachine(GetScriptContext()); }

static ShellStateMachine* GetShellStateMachine() { return GetScriptContext()->GetSubsystem<ShellStateMachine>(); }

void RegisterShellStateMachineAPI(asIScriptEngine* engine)
{
	RegisterShellStateAPI(engine);

	engine->RegisterObjectType("ShellStateMachine", 0, asOBJ_REF);

	engine->RegisterGlobalFunction("ShellStateMachine@+ get_ssm()", AS_FUNCTION(GetShellStateMachine), AS_CALL_CDECL);

	engine->RegisterObjectBehaviour("ShellStateMachine",
									asBEHAVE_FACTORY,
									"ShellStateMachine@+ f()",
									AS_FUNCTION(CreateShellStateMachine),
									AS_CALL_CDECL);

	RegisterSubclass<Object, ShellStateMachine>(engine, "Object", "ShellStateMachine");
	RegisterSubclass<RefCounted, ShellStateMachine>(engine, "RefCounted", "ShellStateMachine");

	RegisterMembers_Object<ShellStateMachine>(engine, "ShellStateMachine");

	engine->RegisterObjectMethod("ShellStateMachine",
								 "void Push(ShellState@+)",
								 AS_METHODPR(ShellStateMachine, NewShellState, (ShellState*), void),
								 AS_CALL_THISCALL);
	engine->RegisterObjectMethod("ShellStateMachine",
								 "ShellState& Get() const",
								 AS_METHOD(ShellStateMachine, GetShellState),
								 AS_CALL_THISCALL);
}
