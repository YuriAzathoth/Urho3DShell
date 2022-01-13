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
#include "Core/ShellConfigurator.h"

using namespace Urho3D;

static ShellConfigurator* CreateShellConfigurator() { return new ShellConfigurator(GetScriptContext()); }

static ShellConfigurator* GetShellConfigurator() { return GetScriptContext()->GetSubsystem<ShellConfigurator>(); }

void RegisterShellConfiguratorAPI(asIScriptEngine* engine)
{
	engine->RegisterObjectType("ShellConfigurator", 0, asOBJ_REF);

	engine->RegisterGlobalFunction("ShellConfigurator@+ get_configurator()",
								   AS_FUNCTION(GetShellConfigurator),
								   AS_CALL_CDECL);

	engine->RegisterObjectBehaviour("ShellConfigurator",
									asBEHAVE_FACTORY,
									"ShellConfigurator@+ f()",
									AS_FUNCTION(CreateShellConfigurator),
									AS_CALL_CDECL);

	RegisterSubclass<Object, ShellConfigurator>(engine, "Object", "ShellConfigurator");
	RegisterSubclass<RefCounted, ShellConfigurator>(engine, "RefCounted", "ShellConfigurator");

	RegisterMembers_Object<ShellConfigurator>(engine, "ShellConfigurator");

	engine->RegisterObjectMethod("ShellConfigurator",
								 "void LoadProfile(const String&in)",
								 AS_METHOD(ShellConfigurator, LoadProfile),
								 AS_CALL_THISCALL);
	engine->RegisterObjectMethod("ShellConfigurator",
								 "void SaveProfile() const",
								 AS_METHOD(ShellConfigurator, SaveProfile),
								 AS_CALL_THISCALL);
	engine->RegisterObjectMethod("ShellConfigurator",
								 "void CreateProfile(const String&in)",
								 AS_METHOD(ShellConfigurator, CreateProfile),
								 AS_CALL_THISCALL);
	engine->RegisterObjectMethod("ShellConfigurator",
								 "void RemoveProfile(const String&in)",
								 AS_METHOD(ShellConfigurator, RemoveProfile),
								 AS_CALL_THISCALL);

	engine->RegisterObjectMethod("ShellConfigurator",
								 "String get_configFilename() const",
								 AS_METHOD(ShellConfigurator, GetConfigFilename),
								 AS_CALL_THISCALL);
	engine->RegisterObjectMethod("ShellConfigurator",
								 "String get_configPath() const",
								 AS_METHOD(ShellConfigurator, GetConfigPath),
								 AS_CALL_THISCALL);
	engine->RegisterObjectMethod("ShellConfigurator",
								 "String get_inputPath() const",
								 AS_METHOD(ShellConfigurator, GetInputPath),
								 AS_CALL_THISCALL);
	engine->RegisterObjectMethod("ShellConfigurator",
								 "String get_logsFilename() const",
								 AS_METHOD(ShellConfigurator, GetLogsFilename),
								 AS_CALL_THISCALL);
	engine->RegisterObjectMethod("ShellConfigurator",
								 "String get_logsPath() const",
								 AS_METHOD(ShellConfigurator, GetLogsPath),
								 AS_CALL_THISCALL);
	engine->RegisterObjectMethod("ShellConfigurator",
								 "String get_pluginsPath() const",
								 AS_METHOD(ShellConfigurator, GetPluginsPath),
								 AS_CALL_THISCALL);
	engine->RegisterObjectMethod("ShellConfigurator",
								 "String get_profileFilename() const",
								 AS_METHOD(ShellConfigurator, GetProfileFilename),
								 AS_CALL_THISCALL);
	engine->RegisterObjectMethod("ShellConfigurator",
								 "String get_savesPath() const",
								 AS_METHOD(ShellConfigurator, GetSavesPath),
								 AS_CALL_THISCALL);

	engine->RegisterObjectMethod("ShellConfigurator",
								 "const String& get_appName() const",
								 AS_METHOD(ShellConfigurator, GetAppName),
								 AS_CALL_THISCALL);
	engine->RegisterObjectMethod("ShellConfigurator",
								 "const String& get_gameName() const",
								 AS_METHOD(ShellConfigurator, GetGameName),
								 AS_CALL_THISCALL);
	engine->RegisterObjectMethod("ShellConfigurator",
								 "const String& get_profile() const",
								 AS_METHOD(ShellConfigurator, GetProfileName),
								 AS_CALL_THISCALL);
	engine->RegisterObjectMethod("ShellConfigurator",
								 "uint16 get_port() const",
								 AS_METHOD(ShellConfigurator, GetPort),
								 AS_CALL_THISCALL);
}
