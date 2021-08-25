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
#include <Urho3D/Core/Context.h>
#include <Urho3D/Core/ProcessUtils.h>
#include <Urho3D/Engine/Console.h>
#include <Urho3D/Engine/DebugHud.h>
#include <Urho3D/Engine/Engine.h>
#include <Urho3D/IO/FileSystem.h>
#include <Urho3D/IO/Log.h>
#include <Urho3D/Input/InputEvents.h>
#include <Urho3D/LuaScript/LuaScript.h>
#include <Urho3D/Network/NetworkEvents.h>
#include <Urho3D/Resource/ResourceCache.h>
#include <Urho3D/Scene/SceneEvents.h>
#include <Urho3D/UI/UI.h>
#include "Config/Config.h"
#include "CoreApplication.h"
#include "Input/ActionsRegistry.h"
#include "Input/ControllersRegistry.h"
#include "Input/InputReceiver.h"
#include "Plugin/PluginsRegistry.h"
#include "ScriptAPI/AngelScript/ScriptAPI.h"
#include "ScriptAPI/Lua/LuaScriptAPI.h"
#include "Shell.h"
#include "ShellConfigurator.h"
#include "ShellDefs.h"
#include "ShellEvents.h"
#include "UI/UIController.h"

#define MAIN_PLUGIN_LIB "Game"

using namespace Urho3D;

CoreApplication::CoreApplication(Urho3D::Context* context, Urho3D::VariantMap&& shellParameters)
	: Urho3D::Application(context)
	, shellParameters_(std::move(shellParameters))
{
	context_->RegisterSubsystem<Config>()->RegisterServerParameters();
}

void CoreApplication::Setup()
{
	context_->RegisterSubsystem<LuaScript>();
	context_->RegisterSubsystem<Script>();
	context_->RegisterSubsystem<ActionsRegistry>();
	context_->RegisterSubsystem<PluginsRegistry>();
	context_->RegisterSubsystem<ShellConfigurator>();

	GetSubsystem<PluginsRegistry>()->RegisterMainPlugin(MAIN_PLUGIN_LIB);
	GetSubsystem<ShellConfigurator>()->Initialize(engineParameters_, shellParameters_);

	RegisterServerAPI(GetSubsystem<Script>()->GetScriptEngine());
	RegisterServerLuaAPI(GetSubsystem<LuaScript>()->GetState());
}

void CoreApplication::Start()
{
	InputReceiver::RegisterObject(context_);

	PluginsRegistry* pluginsRegistry = GetSubsystem<PluginsRegistry>();
	const auto itScript = shellParameters_.Find(SP_SCRIPT);
	if (itScript != shellParameters_.End())
		pluginsRegistry->RegisterPlugin(itScript->second_.GetString());

	GetSubsystem<Config>()->Apply(shellParameters_);
}

void CoreApplication::Stop()
{
	context_->RemoveSubsystem<ShellConfigurator>();
	context_->RemoveSubsystem<PluginsRegistry>();
}

Urho3D::Variant CoreApplication::GetParameter(Urho3D::StringHash parameter, const Urho3D::Variant& defaultValue) const
{
	const auto it = shellParameters_.Find(parameter);
	return it != shellParameters_.End() ? it->second_ : defaultValue;
}