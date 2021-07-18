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
#include "ConfigAPI.h"

using namespace Urho3D;

static Config* CreateConfig() { return new Config(GetScriptContext()); }

void RegisterConfigAPI(asIScriptEngine* engine)
{
    engine->RegisterObjectBehaviour("Config", asBEHAVE_FACTORY, "Config@+ f()", AS_FUNCTION(CreateConfig) , AS_CALL_CDECL);

	RegisterSubclass<Object, Config>(engine, "Object", "Config");
	RegisterSubclass<RefCounted, Config>(engine, "RefCounted", "Config");

	RegisterMembers_Object<Config>(engine, "Config");

	engine->RegisterObjectMethod("Config", "void LoadXML(const XMLElement&)", AS_METHOD(Config, LoadXML), AS_CALL_THISCALL);
	engine->RegisterObjectMethod("Config", "void SaveXML(XMLElement&) const", AS_METHOD(Config, SaveXML), AS_CALL_THISCALL);

	engine->RegisterObjectMethod("Config", "void Apply(bool)", AS_METHOD(Config, Apply), AS_CALL_THISCALL);
	engine->RegisterObjectMethod("Config", "void Clear()", AS_METHOD(Config, Clear), AS_CALL_THISCALL);
	engine->RegisterObjectMethod("Config", "void ExtractEngineParameters(VariantMap&) const", AS_METHOD(Config, ExtractEngineParameters), AS_CALL_THISCALL);

	engine->RegisterObjectMethod("Config", "void RegisterSettingsTab(const String&in)", AS_METHOD(Config, RegisterSettingsTab), AS_CALL_THISCALL);
	engine->RegisterObjectMethod("Config", "void RemoveSettingsTab(StringHash)", AS_METHOD(Config, RemoveSettingsTab), AS_CALL_THISCALL);
	engine->RegisterObjectMethod("Config", "Array<String>@+ GetSettingsTabs(StringHash)", AS_METHOD(Config, GetSettingsTabs), AS_CALL_THISCALL);
	engine->RegisterObjectMethod("Config", "Array<String>@+ GetSettings(StringHash)", AS_METHOD(Config, GetSettings), AS_CALL_THISCALL);

	engine->RegisterObjectMethod("Config", "bool RegisterParameter(const String&in, VariantType, StringHash, bool, bool)", AS_METHOD(Config, RegisterParameter), AS_CALL_THISCALL);
	engine->RegisterObjectMethod("Config", "void RemoveParameter(StringHash)", AS_METHOD(Config, RemoveParameter), AS_CALL_THISCALL);

	engine->RegisterObjectMethod("Config", "String GetDebugString()", AS_METHOD(Config, GetDebugString), AS_CALL_THISCALL);
}
