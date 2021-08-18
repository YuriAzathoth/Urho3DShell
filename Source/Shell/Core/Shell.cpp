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
#include <Urho3D/Network/NetworkEvents.h>
#include <Urho3D/Resource/JSONFile.h>
#include <Urho3D/Resource/ResourceCache.h>
#include <Urho3D/Scene/SceneEvents.h>
#include <Urho3D/UI/UI.h>
#include "Config/Config.h"
#include "Input/ActionsRegistry.h"
#include "Input/ControllersRegistry.h"
#include "Input/InputReceiver.h"
#include "Input/InputSender.h"
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
	, isClient_(true)
{
	context_->RegisterSubsystem<Script>();
	context_->RegisterSubsystem<Config>();
	context_->RegisterSubsystem<ActionsRegistry>();
	context_->RegisterSubsystem<PluginsRegistry>();
	context_->RegisterSubsystem<ShellConfigurator>();
}

Shell::~Shell()
{
	context_->RemoveSubsystem<ShellConfigurator>();
	context_->RemoveSubsystem<PluginsRegistry>();
	if (isClient_)
		context_->RemoveSubsystem<ControllersRegistry>();
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
	isClient_ = !(GetParameter(LP_NO_CLIENT, false).GetBool() || GetParameter(EP_HEADLESS, false).GetBool());

	Config* config = GetSubsystem<Config>();
	config->RegisterServerParameters();
	if (isClient_)
		config->RegisterClientParameters();

	ShellConfigurator* configurator = GetSubsystem<ShellConfigurator>();
	configurator->Initialize(shellParameters_);
	config->ExtractEngineParameters(engineParameters, shellParameters_);

	engineParameters[EP_LOG_NAME] = configurator->GetLogsFilename();

	asIScriptEngine* engine = GetSubsystem<Script>()->GetScriptEngine();
	RegisterServerAPI(engine);
	if (isClient_)
		RegisterClientAPI(engine);
}

void Shell::Initialize()
{
	PluginsRegistry* pluginsRegistry = GetSubsystem<PluginsRegistry>();
	pluginsRegistry->RegisterPlugin(gameLibrary_);
	const auto itScript = shellParameters_.Find(LP_SCRIPT);
	if (itScript != shellParameters_.End())
		pluginsRegistry->RegisterPlugin(itScript->second_.GetString());

	InputReceiver::RegisterObject(context_);

	if (isClient_)
	{
		InputSender::RegisterObject(context_);

		ResourceCache* cache = GetSubsystem<ResourceCache>();
		XMLFile* styleFile = cache->GetResource<XMLFile>("UI/DefaultStyle.xml");
		GetSubsystem<UI>()->GetRoot()->SetDefaultStyle(styleFile);

		Engine* engine = GetSubsystem<Engine>();
		engine->CreateConsole()->SetDefaultStyle(styleFile);
		engine->CreateDebugHud()->SetDefaultStyle(styleFile);

		context_->RegisterSubsystem<UIController>();

		StartMainMenu();

		ControllersRegistry* controllers = context_->RegisterSubsystem<ControllersRegistry>();
		controllers->SetConfigPath(GetSubsystem<ShellConfigurator>()->GetInputPath());

		SendEvent(E_SHELLCLIENTSTARTED);

		controllers->EnableController("KeyboardController");

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
	client_.Reset();
	server_.Reset();

	UIController* uiController = GetSubsystem<UIController>();
	uiController->RemoveAllDialogs();
	uiController->CreateDialog("MainMenuDialog");
}

void Shell::StartLocalServer(Urho3D::String sceneName)
{
	auto InitServer = [this](StringHash, VariantMap&)
	{
		server_->Start(GetPort());
		server_->SetPausable(true);
		client_ = MakeUnique<Client>(context_);
		client_->Connect(GetPort());
		UIController* uiController = GetSubsystem<UIController>();
		uiController->RemoveAllDialogs();
		UnsubscribeFromEvent(E_ASYNCLOADFINISHED);
	};

	auto LoadScene =
		[this, sceneName = std::move(sceneName), InitServer = std::move(InitServer)](StringHash, VariantMap&)
	{
		server_ = MakeUnique<Server>(context_);
		server_->LoadScene(sceneName);
		SubscribeToEvent(E_ASYNCLOADFINISHED, InitServer);
		UnsubscribeFromEvent(E_SERVERDISCONNECTED);
	};

	const bool disconnect = client_.NotNull();
	client_.Reset();
	server_.Reset();
	if (disconnect)
		SubscribeToEvent(E_SERVERDISCONNECTED, LoadScene);
	else
		LoadScene(StringHash::ZERO, GetEventDataMap());
}

void Shell::StartRemoteServer(Urho3D::String serverName, Urho3D::String sceneName)
{
	auto InitServer = [this, serverName = std::move(serverName)](StringHash, VariantMap&)
	{
		server_->Start(GetPort());
		server_->MakeVisible(serverName);
		server_->SetPausable(false);
		client_ = MakeUnique<Client>(context_);
		client_->Connect(GetPort());
		UIController* uiController = GetSubsystem<UIController>();
		uiController->RemoveAllDialogs();
		UnsubscribeFromEvent(E_ASYNCLOADFINISHED);
	};

	auto LoadScene =
		[this, sceneName = std::move(sceneName), InitServer = std::move(InitServer)](StringHash, VariantMap&)
	{
		server_ = MakeUnique<Server>(context_);
		server_->LoadScene(sceneName);
		SubscribeToEvent(E_ASYNCLOADFINISHED, InitServer);
		UnsubscribeFromEvent(E_SERVERDISCONNECTED);
	};

	const bool disconnect = client_.NotNull();
	client_.Reset();
	server_.Reset();
	if (disconnect)
		SubscribeToEvent(E_SERVERDISCONNECTED, LoadScene);
	else
		LoadScene(StringHash::ZERO, GetEventDataMap());
}

void Shell::StartClient(Urho3D::String address)
{
	auto InitClient = [this, address = std::move(address)](StringHash, VariantMap&)
	{
		client_ = MakeUnique<Client>(context_);
		client_->Connect(GetPort(), address);
		UIController* uiController = GetSubsystem<UIController>();
		uiController->RemoveAllDialogs();
		UnsubscribeFromEvent(E_SERVERDISCONNECTED);
	};

	const bool disconnect = client_.NotNull();
	client_.Reset();
	server_.Reset();
	if (disconnect)
		SubscribeToEvent(E_SERVERDISCONNECTED, InitClient);
	else
		InitClient(StringHash::ZERO, GetEventDataMap());
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
