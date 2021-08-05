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
#include "UI/UIController.h"

using namespace Urho3D;

static UIController* CreateUIController() { return new UIController(GetScriptContext()); }

static UIController* GetUIController() { return GetScriptContext()->GetSubsystem<UIController>(); }

void RegisterUIControllerAPI(asIScriptEngine* engine)
{
	engine->RegisterObjectType("UIController", 0, asOBJ_REF);

	engine->RegisterGlobalFunction("UIController@+ get_uiController()", AS_FUNCTION(GetUIController), AS_CALL_CDECL);

	engine->RegisterObjectBehaviour("UIController",
									asBEHAVE_FACTORY,
									"UIController@+ f()",
									AS_FUNCTION(CreateUIController),
									AS_CALL_CDECL);

	RegisterSubclass<Object, UIController>(engine, "Object", "UIController");
	RegisterSubclass<RefCounted, UIController>(engine, "RefCounted", "UIController");

	RegisterMembers_Object<UIController>(engine, "UIController");

	engine->RegisterObjectMethod("UIController",
								 "void CreateDialog(StringHash)",
								 AS_METHOD(UIController, CreateDialog),
								 AS_CALL_THISCALL);
	engine->RegisterObjectMethod("UIController",
								 "void RemoveDialog(StringHash)",
								 AS_METHOD(UIController, RemoveDialog),
								 AS_CALL_THISCALL);
	engine->RegisterObjectMethod("UIController",
								 "void RemoveAllDialogs()",
								 AS_METHOD(UIController, RemoveAllDialogs),
								 AS_CALL_THISCALL);

	engine->RegisterObjectMethod("UIController",
								 "uint get_closeables() const",
								 AS_METHOD(UIController, GetCloseablesCount),
								 AS_CALL_THISCALL);
	engine->RegisterObjectMethod("UIController",
								 "uint get_interactives() const",
								 AS_METHOD(UIController, GetInteractivesCount),
								 AS_CALL_THISCALL);
}
