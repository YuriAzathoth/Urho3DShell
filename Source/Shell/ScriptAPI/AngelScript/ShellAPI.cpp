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
#include "Core/Shell.h"

using namespace Urho3D;

static Shell* CreateShell() { return new Shell(GetScriptContext()); }

static Shell* GetShell() { return GetScriptContext()->GetSubsystem<Shell>(); }

void RegisterShellAPI(asIScriptEngine* engine)
{
	engine->RegisterObjectType("Shell", 0, asOBJ_REF);

	engine->RegisterGlobalFunction("Shell@+ get_shell()", AS_FUNCTION(GetShell), AS_CALL_CDECL);

	engine->RegisterObjectBehaviour("Shell", asBEHAVE_FACTORY, "Shell@+ f()", AS_FUNCTION(CreateShell), AS_CALL_CDECL);

	RegisterSubclass<Object, Shell>(engine, "Object", "Shell");
	RegisterSubclass<RefCounted, Shell>(engine, "RefCounted", "Shell");

	RegisterMembers_Object<Shell>(engine, "Shell");

	engine->RegisterObjectMethod("Shell", "void StartMainMenu()", AS_METHOD(Shell, StartMainMenu), AS_CALL_THISCALL);
	engine->RegisterObjectMethod("Shell",
								 "void StartLocalServer(String)",
								 AS_METHOD(Shell, StartLocalServer),
								 AS_CALL_THISCALL);
	engine->RegisterObjectMethod("Shell",
								 "void StartRemoteServer(String, String)",
								 AS_METHOD(Shell, StartRemoteServer),
								 AS_CALL_THISCALL);
	engine->RegisterObjectMethod("Shell", "void StartClient(String)", AS_METHOD(Shell, StartClient), AS_CALL_THISCALL);
}