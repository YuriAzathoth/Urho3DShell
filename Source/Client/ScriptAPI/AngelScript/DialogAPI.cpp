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
#include "UI/Dialog.h"

using namespace Urho3D;

template <typename T> void RegisterMembers_Dialog(asIScriptEngine* engine, const char* className)
{
	RegisterMembers_Object<T>(engine, className);
	engine->RegisterObjectMethod(className,
								 "void LoadLayout(const String&in)",
								 AS_METHOD(T, LoadLayout),
								 AS_CALL_THISCALL);
	engine->RegisterObjectMethod(className, "void ShrinkSize()", AS_METHOD(T, ShrinkSize), AS_CALL_THISCALL);
	engine->RegisterObjectMethod(className, "void set_closeable(bool)", AS_METHOD(T, SetCloseable), AS_CALL_THISCALL);
	engine->RegisterObjectMethod(className,
								 "void set_interactive(bool)",
								 AS_METHOD(T, SetInteractive),
								 AS_CALL_THISCALL);
	engine->RegisterObjectMethod(className, "bool get_closeable() const", AS_METHOD(T, IsCloseable), AS_CALL_THISCALL);
	engine->RegisterObjectMethod(className,
								 "bool get_interactive() const",
								 AS_METHOD(T, IsInteractive),
								 AS_CALL_THISCALL);
	engine->RegisterObjectMethod(className, "bool get_isFront() const", AS_METHOD(T, IsFrontElement), AS_CALL_THISCALL);
	engine->RegisterObjectMethod(className, "UIElement@+ get_root() const", AS_METHOD(T, GetRoot), AS_CALL_THISCALL);
}

static Dialog* Create_Dialog() { return new Dialog(GetScriptContext()); }

void RegisterDialogAPI(asIScriptEngine* engine)
{
	engine->RegisterObjectType("Dialog", 0, asOBJ_REF);

	engine->RegisterObjectBehaviour("Dialog",
									asBEHAVE_FACTORY,
									"Dialog@+ f()",
									AS_FUNCTION(Create_Dialog),
									AS_CALL_CDECL);

	RegisterSubclass<Object, Dialog>(engine, "Object", "Dialog");
	RegisterSubclass<RefCounted, Dialog>(engine, "RefCounted", "Dialog");

	RegisterMembers_Dialog<Dialog>(engine, "Dialog");
}
