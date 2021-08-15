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
#include <Urho3D/IO/FileSystem.h>
#include <Urho3D/IO/Log.h>
#include <Urho3D/Input/InputEvents.h>
#include <Urho3D/Resource/JSONFile.h>
#include <Urho3D/Resource/ResourceCache.h>
#include <Urho3D/UI/UI.h>
#include "Config/Config.h"
#include "Input/InputClient.h"
#include "Input/InputRegistry.h"
#include "Network/Client.h"
#include "Network/Server.h"
#include "Plugin/PluginsRegistry.h"
#include "ScriptAPI/ScriptAPI.h"
#include "Shell.h"
#include "ShellConfigurator.h"
#include "ShellEvents.h"
#include "UI/UIController.h"

#define LP_GAME_LIB "GameLib"
#define LP_NO_CLIENT "NoClient"
#define LP_SCRIPT "Script"

using namespace Urho3D;

Shell::Shell(Urho3D::Context* context)
	: Object(context)
	, port_(27500)
	, client_(true)
{
	context_->RegisterSubsystem<Script>();
	context_->RegisterSubsystem<Config>();
	context_->RegisterSubsystem<InputRegistry>();
	context_->RegisterSubsystem<PluginsRegistry>();
	context_->RegisterSubsystem<ShellConfigurator>();
}

Shell::~Shell()
{
	context_->RemoveSubsystem<ShellConfigurator>();
	context_->RemoveSubsystem<PluginsRegistry>();
	if (client_)
		context_->RemoveSubsystem<InputClient>();
}

void Shell::Setup(Urho3D::VariantMap& engineParameters)
{
	if (!PreconfigureEngine())
	{
		URHO3D_LOGERRORF("Failed to load and configure game library.");
		GetSubsystem<Engine>()->Exit();
		return;
	}

	ParseParameters(GetArguments());
	client_ = !(GetParameter(LP_NO_CLIENT, false).GetBool() || GetParameter(EP_HEADLESS, false).GetBool());

	Config* config = GetSubsystem<Config>();
	config->RegisterServerParameters();
	if (client_)
		config->RegisterClientParameters();

	ShellConfigurator* configurator = GetSubsystem<ShellConfigurator>();
	configurator->Initialize(shellParameters_);
	config->ExtractEngineParameters(engineParameters, shellParameters_);

	engineParameters[EP_LOG_NAME] = configurator->GetLogsFilename();

	asIScriptEngine* engine = GetSubsystem<Script>()->GetScriptEngine();
	RegisterServerAPI(engine);
	if (client_)
		RegisterClientAPI(engine);
}

void Shell::Initialize()
{
	PluginsRegistry* pluginsRegistry = GetSubsystem<PluginsRegistry>();
	pluginsRegistry->RegisterPlugin(gameLibrary_);
	const auto itScript = shellParameters_.Find(LP_SCRIPT);
	if (itScript != shellParameters_.End())
		pluginsRegistry->RegisterPlugin(itScript->second_.GetString());

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

		context_->RegisterSubsystem<UIController>();
		StartMainMenu();

		InputClient* inputClient = context_->RegisterSubsystem<InputClient>();
		inputClient->SetConfigPath(GetSubsystem<ShellConfigurator>()->GetInputPath());

		SendEvent(E_SHELLCLIENTSTARTED);

		inputClient->EnableController("KeyboardController");

		SubscribeToEvent(E_KEYDOWN, URHO3D_HANDLER(Shell, OnKeyDown));
	}

	GetSubsystem<Config>()->Apply(shellParameters_);
	shellParameters_.Clear();
}

bool Shell::PreconfigureEngine()
{
	if (shellParameters_.Contains(LP_GAME_LIB))
		return true;

	JSONFile file(context_);
	if (!file.LoadFile(GetSubsystem<FileSystem>()->GetProgramDir() + "/Engine.dat"))
		return false;

	const JSONValue& root = file.GetRoot();
	if (root.IsNull())
		return false;

	const JSONValue& libraryNameValue = root.Get("lib");
	if (libraryNameValue.IsNull())
		return false;

	gameLibrary_ = libraryNameValue.GetString();
	if (gameLibrary_.Empty())
		return false;

	const JSONValue& dirNameValue = root.Get("dir");
	if (dirNameValue.IsNull())
		return false;

	const String& gameName = dirNameValue.GetString();
	if (!gameName.Empty())
		GetSubsystem<ShellConfigurator>()->SetGameName(gameName);
	else
		return false;

	return true;
}

void Shell::StartMainMenu()
{
	context_->RemoveSubsystem<Client>();
	context_->RemoveSubsystem<Server>();

	UIController* uiController = GetSubsystem<UIController>();
	uiController->RemoveAllDialogs();
	uiController->CreateDialog("MainMenuDialog");
}

void Shell::StartLocalServer(Urho3D::String sceneName)
{
	Server* server = context_->RegisterSubsystem<Server>();
	server->Start(GetSubsystem<Shell>()->GetPort());
	server->LoadScene(sceneName);
	server->SetPausable(true);

	Client* client = context_->RegisterSubsystem<Client>();
	client->Connect();

	UIController* uiController = GetSubsystem<UIController>();
	uiController->RemoveAllDialogs();
}

void Shell::StartRemoteServer(Urho3D::String serverName, Urho3D::String sceneName)
{
	Server* server = context_->RegisterSubsystem<Server>();
	server->Start(GetPort());
	server->MakeVisible(serverName);
	server->LoadScene(sceneName);
	server->SetPausable(false);

	Client* client = context_->RegisterSubsystem<Client>();
	client->Connect();

	UIController* uiController = GetSubsystem<UIController>();
	uiController->RemoveAllDialogs();
}

void Shell::StartClient(Urho3D::String address)
{
	context_->RemoveSubsystem<Server>();

	Client* client = context_->RegisterSubsystem<Client>();
	client->Connect(address);

	UIController* uiController = GetSubsystem<UIController>();
	uiController->RemoveAllDialogs();
}

void Shell::ParseParameters(const Urho3D::StringVector& arguments)
{
	ShellConfigurator* configurator = GetSubsystem<ShellConfigurator>();
	String argument, value;
	for (unsigned i = 0; i < arguments.Size(); ++i)
		if (arguments[i].Length() > 1 && arguments[i][0] == '-')
		{
			argument = arguments[i].Substring(1).ToLower();
			value = i + 1 < arguments.Size() ? arguments[i + 1] : String::EMPTY;
			if (argument == "appname")
			{
				configurator->SetAppName(value);
				++i;
			}
			else if (argument == "gamelib")
				shellParameters_[LP_GAME_LIB] = value;
			else if (argument == "noclient")
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

void Shell::OnKeyDown(Urho3D::StringHash, Urho3D::VariantMap& eventData)
{
	using namespace KeyDown;
	const int key = eventData[P_KEY].GetInt();
	switch (key)
	{
	case KEY_F1:
		GetSubsystem<Console>()->Toggle();
		break;
	}
}
