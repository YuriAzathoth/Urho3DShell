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
#include <Urho3D/Engine/EngineDefs.h>
#include <Urho3D/IO/File.h>
#include <Urho3D/IO/FileSystem.h>
#include <Urho3D/IO/Log.h>
#include <Urho3D/Resource/ResourceCache.h>
#include <Urho3D/Resource/XMLFile.h>
#include <Urho3D/UI/UI.h>
#include "Config/Config.h"
#include "Config/Configurator.h"
#include "Plugin/PluginsRegistry.h"
#include "ScriptAPI/ScriptAPI.h"
#include "Shell.h"
#include "ShellState/MainMenu.h"

#define LP_NO_CLIENT "NoClient"
#define LP_SCRIPT "Script"

void RegisterMainMenuWindow(Urho3D::Context* context);

using namespace Urho3D;

Shell::Shell(Urho3D::Context* context)
	: Object(context)
	, client_(true)
{
	context_->RegisterSubsystem<Script>();
	context_->RegisterSubsystem<Configurator>();
	context_->RegisterSubsystem<Config>();
	context_->RegisterSubsystem<PluginsRegistry>();
}

Shell::~Shell()
{
	context_->RemoveSubsystem<PluginsRegistry>();
	context_->RemoveSubsystem<Configurator>();
}

void Shell::Setup(Urho3D::VariantMap& engineParameters)
{
	ParseParameters(GetArguments());
	client_ = !(GetParameter(LP_NO_CLIENT, false).GetBool() || GetParameter(EP_HEADLESS, false).GetBool());

	Config* config = GetSubsystem<Config>();
	config->RegisterServerParameters();
	if (client_)
		config->RegisterClientParameters();

	Configurator* configurator = GetSubsystem<Configurator>();
	configurator->Initialize();

	asIScriptEngine* engine = GetSubsystem<Script>()->GetScriptEngine();
	RegisterServerAPI(engine);
	if (client_)
		RegisterClientAPI(engine);

	engineParameters[EP_LOG_NAME] = configurator->GetLogsFilename();

	config->ExtractEngineParameters(engineParameters);
}

void Shell::Initialize()
{
	if (client_)
	{
		ResourceCache* cache = GetSubsystem<ResourceCache>();

		XMLFile* styleFile = cache->GetResource<XMLFile>("UI/DefaultStyle.xml");
		GetSubsystem<UI>()->GetRoot()->SetDefaultStyle(styleFile);

		Engine* engine = GetSubsystem<Engine>();
		Console* console = engine->CreateConsole();
		console->SetDefaultStyle(styleFile);
		DebugHud* debugHud = engine->CreateDebugHud();
		debugHud->SetDefaultStyle(styleFile);

		RegisterMainMenuWindow(context_);

		shellState_ = new MainMenu(context_);
	}

	const auto itScript = shellParameters_.Find(LP_SCRIPT);
	if (itScript != shellParameters_.End())
		GetSubsystem<PluginsRegistry>()->RegisterPlugin(itScript->second_.GetString());

	GetSubsystem<Config>()->Apply(false);
}

void Shell::ParseParameters(const Urho3D::StringVector& arguments)
{
	String argument, value;
	for (unsigned i = 0; i < arguments.Size(); ++i)
		if (arguments[i].Length() > 1 && arguments[i][0] == '-')
		{
			argument = arguments[i].Substring(1).ToLower();
			value = i + 1 < arguments.Size() ? arguments[i + 1] : String::EMPTY;
			if (argument == "noclient")
				shellParameters_[LP_NO_CLIENT] = true;
			else if (argument == "script")
			{
				shellParameters_[LP_SCRIPT] = value;
				++i;
			}
		}
}

Urho3D::Variant Shell::GetParameter(Urho3D::StringHash parameter, const Urho3D::Variant& defaultValue)
{
	const auto it = shellParameters_.Find(parameter);
	return it != shellParameters_.End() ? it->second_ : defaultValue;
}
