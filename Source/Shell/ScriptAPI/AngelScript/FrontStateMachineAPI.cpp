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
#include "FrontState/FrontStateMachine.h"

using namespace Urho3D;

extern void RegisterFrontStateAPI(asIScriptEngine* engine);

static FrontStateMachine* CreateFrontStateMachine() { return new FrontStateMachine(GetScriptContext()); }

static FrontStateMachine* GetFrontStateMachine() { return GetScriptContext()->GetSubsystem<FrontStateMachine>(); }

void RegisterFrontStateMachineAPI(asIScriptEngine* engine)
{
	RegisterFrontStateAPI(engine);

	engine->RegisterObjectType("FrontStateMachine", 0, asOBJ_REF);

	engine->RegisterGlobalFunction("FrontStateMachine@+ get_ssm()", AS_FUNCTION(GetFrontStateMachine), AS_CALL_CDECL);

	engine->RegisterObjectBehaviour("FrontStateMachine",
									asBEHAVE_FACTORY,
									"FrontStateMachine@+ f()",
									AS_FUNCTION(CreateFrontStateMachine),
									AS_CALL_CDECL);

	RegisterSubclass<Object, FrontStateMachine>(engine, "Object", "FrontStateMachine");
	RegisterSubclass<RefCounted, FrontStateMachine>(engine, "RefCounted", "FrontStateMachine");

	RegisterMembers_Object<FrontStateMachine>(engine, "FrontStateMachine");

	engine->RegisterObjectMethod("FrontStateMachine",
								 "void Push(FrontState@+)",
								 AS_METHODPR(FrontStateMachine, Push, (FrontState*), void),
								 AS_CALL_THISCALL);
	engine->RegisterObjectMethod("FrontStateMachine",
								 "FrontState& Get() const",
								 AS_METHOD(FrontStateMachine, Get),
								 AS_CALL_THISCALL);
}
