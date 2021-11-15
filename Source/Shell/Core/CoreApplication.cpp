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
#include <Urho3D/LuaScript/LuaScript.h>
#include <Urho3D/Urho3DConfig.h>
#include "Config/Config.h"
#include "CoreApplication.h"
#include "Input/ActionsRegistry.h"
#include "Input/InputReceiver.h"
#include "Plugin/PluginsRegistry.h"
#include "ShellConfigurator.h"
#include "ShellDefs.h"
#include "Urho3DShellConfig.h"

extern void RegisterServerParameters(Config* config);
#ifdef URHO3DSHELL_EXPERIMENTAL
#ifdef URHO3D_ANGELSCRIPT
extern void RegisterServerAPI(asIScriptEngine* engine);
#endif // URHO3D_ANGELSCRIPT
#ifdef URHO3D_LUA
extern void RegisterServerLuaAPI(lua_State* state);
#endif // URHO3D_LUA
#endif // URHO3DSHELL_EXPERIMENTAL

using namespace Urho3D;

CoreApplication::CoreApplication(Urho3D::Context* context, Urho3D::VariantMap&& shellParameters)
	: Application(context)
	, shellParameters_(std::move(shellParameters))
{
	context_->RegisterSubsystem<Config>();
	RegisterServerParameters(GetSubsystem<Config>());
}

void CoreApplication::Setup()
{
	context_->RegisterSubsystem<ActionsRegistry>();
	context_->RegisterSubsystem<PluginsRegistry>();
	context_->RegisterSubsystem<ShellConfigurator>();

	const String& gameLib = GetParameter(SP_GAME_LIB, "Game").GetString();
	if (!GetSubsystem<PluginsRegistry>()->Load(gameLib))
		GetSubsystem<Engine>()->Exit();

	GetSubsystem<ShellConfigurator>()->Initialize(engineParameters_, shellParameters_);

#ifdef URHO3D_ANGELSCRIPT
	context_->RegisterSubsystem<Script>();
#endif // URHO3DSHELL_EXPERIMENTAL

#ifdef URHO3DSHELL_EXPERIMENTAL
#ifdef URHO3D_ANGELSCRIPT
	RegisterServerAPI(GetSubsystem<Script>()->GetScriptEngine());
#endif // URHO3D_ANGELSCRIPT
#ifdef URHO3D_LUA
	context_->RegisterSubsystem<LuaScript>();
	RegisterServerLuaAPI(GetSubsystem<LuaScript>()->GetState());
#endif // URHO3D_LUA
#endif // URHO3DSHELL_EXPERIMENTAL
}

void CoreApplication::Start()
{
	InputReceiver::RegisterObject(context_);

	PluginsRegistry* plugins = GetSubsystem<PluginsRegistry>();
	const auto itScript = shellParameters_.Find(SP_SCRIPT);
	if (itScript != shellParameters_.End())
		plugins->Load(itScript->second_.GetString());

	GetSubsystem<Config>()->Apply(shellParameters_);
}

void CoreApplication::Stop()
{
	context_->RemoveSubsystem<ShellConfigurator>();
	context_->RemoveSubsystem<PluginsRegistry>();
}

const Urho3D::Variant& CoreApplication::GetParameter(Urho3D::StringHash parameter,
													 const Urho3D::Variant& defaultValue) const
{
	const auto it = shellParameters_.Find(parameter);
	return it != shellParameters_.End() ? it->second_ : defaultValue;
}
