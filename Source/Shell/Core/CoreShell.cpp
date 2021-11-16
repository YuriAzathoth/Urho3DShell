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

#include <Urho3D/AngelScript/Script.h>
#include <Urho3D/Engine/Engine.h>
#include <Urho3D/LuaScript/LuaScript.h>
#include <Urho3D/Urho3DConfig.h>
#include "Config/Config.h"
#include "CoreShell.h"
#include "Input/ActionsRegistry.h"
#include "Input/InputReceiver.h"
#include "Plugin/PluginsRegistry.h"
#include "ShellConfigurator.h"
#include "ShellDefs.h"
#include "Urho3DShellConfig.h"

extern void RegisterServerParameters(Config* config);
#ifdef URHO3D_ANGELSCRIPT
extern void RegisterServerAPI(asIScriptEngine* engine);
#endif // URHO3D_ANGELSCRIPT
#ifdef URHO3D_LUA
extern void RegisterServerLuaAPI(lua_State* state);
#endif // URHO3D_LUA

using namespace Urho3D;

CoreShell::CoreShell(Urho3D::Context* context)
	: Object(context)
{
	RegisterServerParameters(context_->RegisterSubsystem<Config>());

	InputReceiver::RegisterObject(context_);

#ifdef URHO3D_ANGELSCRIPT
	RegisterServerAPI(context_->RegisterSubsystem<Script>()->GetScriptEngine());
#endif // URHO3D_ANGELSCRIPT
#ifdef URHO3D_LUA
	RegisterServerLuaAPI(context_->RegisterSubsystem<LuaScript>()->GetState());
#endif // URHO3D_LUA

	context_->RegisterSubsystem<ActionsRegistry>();
	context_->RegisterSubsystem<PluginsRegistry>();
	context_->RegisterSubsystem<ShellConfigurator>();
}

CoreShell::~CoreShell()
{
	context_->RemoveSubsystem<ShellConfigurator>();
	context_->RemoveSubsystem<PluginsRegistry>();
}

void CoreShell::LoadGameLibrary(const Urho3D::String& gameLib)
{
	if (!GetSubsystem<PluginsRegistry>()->Load(gameLib))
		GetSubsystem<Engine>()->Exit();
}

void CoreShell::LoadConfig(Urho3D::VariantMap& engineParameters, const Urho3D::String& appName)
{
	GetSubsystem<ShellConfigurator>()->Initialize(engineParameters, shellParameters_, appName);
}

void CoreShell::LoadPlugin(const Urho3D::String& plugin) { GetSubsystem<PluginsRegistry>()->Load(plugin); }

void CoreShell::ApplyConfig() { GetSubsystem<Config>()->Apply(shellParameters_); }

const Variant& CoreShell::GetShellParameter(Urho3D::StringHash parameter, const Urho3D::Variant& defaultValue) const
{
	const auto it = shellParameters_.Find(parameter);
	return it != shellParameters_.End() ? it->second_ : defaultValue;
}
