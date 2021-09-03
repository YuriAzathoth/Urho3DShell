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
#include "Input/ActionsRegistry.h"
#include "Input/InputReceiver.h"
#include "Plugin/PluginsRegistry.h"
#include "ShellApplication.h"
#include "ShellConfigurator.h"
#include "ShellDefs.h"
#include "Urho3DShellConfig.h"

#define MAIN_PLUGIN_LIB "Game"

extern void RegisterServerAPI(asIScriptEngine* engine);
#if defined(URHO3DSHELL_EXPERIMENTAL) && defined(URHO3D_LUA)
extern void RegisterServerLuaAPI(lua_State* state);
#endif

using namespace Urho3D;

ShellApplication::ShellApplication(Urho3D::Context* context, Urho3D::VariantMap&& shellParameters)
	: Application(context)
	, shellParameters_(std::move(shellParameters))
{
	context_->RegisterSubsystem<Config>()->RegisterServerParameters();
}

void ShellApplication::Setup()
{
	context_->RegisterSubsystem<LuaScript>();
	context_->RegisterSubsystem<Script>();
	context_->RegisterSubsystem<ActionsRegistry>();
	context_->RegisterSubsystem<PluginsRegistry>();
	context_->RegisterSubsystem<ShellConfigurator>();

	GetSubsystem<PluginsRegistry>()->Initialize(MAIN_PLUGIN_LIB);
	GetSubsystem<ShellConfigurator>()->Initialize(engineParameters_, shellParameters_);

	RegisterServerAPI(GetSubsystem<Script>()->GetScriptEngine());

#if defined(URHO3DSHELL_EXPERIMENTAL) && defined(URHO3D_LUA)
	RegisterServerLuaAPI(GetSubsystem<LuaScript>()->GetState());
#endif
}

void ShellApplication::Start()
{
	InputReceiver::RegisterObject(context_);

	PluginsRegistry* plugins = GetSubsystem<PluginsRegistry>();
	const auto itScript = shellParameters_.Find(SP_SCRIPT);
	if (itScript != shellParameters_.End())
		plugins->Load(itScript->second_.GetString());

	GetSubsystem<Config>()->Apply(shellParameters_);
}

void ShellApplication::Stop()
{
	context_->RemoveSubsystem<ShellConfigurator>();
	context_->RemoveSubsystem<PluginsRegistry>();
}

Urho3D::Variant ShellApplication::GetParameter(Urho3D::StringHash parameter, const Urho3D::Variant& defaultValue) const
{
	const auto it = shellParameters_.Find(parameter);
	return it != shellParameters_.End() ? it->second_ : defaultValue;
}
