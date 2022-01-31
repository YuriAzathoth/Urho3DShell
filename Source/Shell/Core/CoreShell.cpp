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

#include <Urho3D/AngelScript/Script.h>
#include <Urho3D/Core/ProcessUtils.h>
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

extern void RegisterServerParameters(Config* config);
#ifdef URHO3D_ANGELSCRIPT
extern void RegisterServerAPI(asIScriptEngine* engine);
#endif // URHO3D_ANGELSCRIPT

using namespace Urho3D;

CoreShell::CoreShell(Urho3D::Context* context)
	: Object(context)
{
	ParseParameters();

	RegisterServerParameters(context_->RegisterSubsystem<Config>());

	InputReceiver::RegisterObject(context_);

#ifdef URHO3D_ANGELSCRIPT
	RegisterServerAPI(context_->RegisterSubsystem<Script>()->GetScriptEngine());
#endif // URHO3D_ANGELSCRIPT

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

void CoreShell::ParseParameters()
{
	const StringVector& arguments = GetArguments();
	String argument, value;
	for (unsigned i = 0; i < arguments.Size(); ++i)
		if (arguments[i].Length() > 1 && arguments[i][0] == '-')
		{
			argument = arguments[i].Substring(1).ToLower();
			value = i + 1 < arguments.Size() ? arguments[i + 1] : String::EMPTY;
			if (argument == "appname")
			{
				shellParameters_[SP_APP_NAME] = value;
				++i;
			}
			else if (argument == "client")
			{
				shellParameters_[SP_CLIENT] = value;
				++i;
			}
			else if (argument == "gamelib")
			{
				shellParameters_[SP_GAME_LIB] = value;
				++i;
			}
			else if (argument == "scene")
			{
				shellParameters_[SP_SCENE] = value;
				++i;
			}
			else if (argument == "script")
			{
				shellParameters_[SP_SCRIPT] = value;
				++i;
			}
			else if (argument == "server")
			{
				if (value.Empty() || value[0] == '-')
					shellParameters_[SP_SERVER] = String::EMPTY;
				else
				{
					shellParameters_[SP_SERVER] = value;
					++i;
				}
			}
		}
}
