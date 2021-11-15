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
#include <Urho3D/Engine/Console.h>
#include <Urho3D/Engine/DebugHud.h>
#include <Urho3D/Engine/Engine.h>
#include <Urho3D/LuaScript/LuaScript.h>
#include <Urho3D/Resource/ResourceCache.h>
#include <Urho3D/UI/UI.h>
#include <Urho3D/Urho3DConfig.h>
#include "Config/Config.h"
#include "FrontApplication.h"
#include "FrontState/ClientState.h"
#include "FrontState/FrontStateMachine.h"
#include "FrontState/LocalServerState.h"
#include "FrontState/MainMenuState.h"
#include "FrontState/RemoteServerState.h"
#include "Input/ControllersRegistry.h"
#include "ShellConfigurator.h"
#include "ShellDefs.h"
#include "ShellEvents.h"
#include "UI/LoadGameDialog.h"
#include "UI/MainMenuDialog.h"
#include "UI/NewGameDialog.h"
#include "UI/PauseDialog.h"
#include "UI/ServersListDialog.h"
#include "UI/SettingsDialog.h"
#include "Urho3DShellConfig.h"

extern void RegisterClientParameters(Config* config);
#ifdef URHO3D_ANGELSCRIPT
extern void RegisterClientAPI(asIScriptEngine* engine);
#endif // URHO3D_ANGELSCRIPT
#ifdef URHO3D_LUA
extern void RegisterClientLuaAPI(lua_State* state);
#endif // URHO3D_LUA

using namespace Urho3D;

FrontApplication::FrontApplication(Urho3D::Context* context, Urho3D::VariantMap&& shellParameters)
	: CoreApplication(context, std::move(shellParameters))
{
	RegisterClientParameters(GetSubsystem<Config>());
}

void FrontApplication::Setup()
{
	CoreApplication::Setup();

	context_->RegisterFactory<LoadGameDialog>();
	context_->RegisterFactory<MainMenuDialog>();
	context_->RegisterFactory<NewGameDialog>();
	context_->RegisterFactory<PauseDialog>();
	context_->RegisterFactory<ServersListDialog>();
	context_->RegisterFactory<SettingsDialog>();
	context_->RegisterSubsystem<FrontStateMachine>();

#ifdef URHO3D_ANGELSCRIPT
	RegisterClientAPI(GetSubsystem<Script>()->GetScriptEngine());
#endif // URHO3D_ANGELSCRIPT
#ifdef URHO3D_LUA
	RegisterClientLuaAPI(GetSubsystem<LuaScript>()->GetState());
#endif // URHO3D_LUA
}

void FrontApplication::Start()
{
	CoreApplication::Start();

	ResourceCache* cache = GetSubsystem<ResourceCache>();
	XMLFile* styleFile = cache->GetResource<XMLFile>("UI/DefaultStyle.xml");
	GetSubsystem<UI>()->GetRoot()->SetDefaultStyle(styleFile);

	Engine* engine = GetSubsystem<Engine>();
	engine->CreateConsole()->SetDefaultStyle(styleFile);
	engine->CreateDebugHud()->SetDefaultStyle(styleFile);

	ControllersRegistry* controllers = context_->RegisterSubsystem<ControllersRegistry>();

	SendEvent(E_SHELLCLIENTSTARTED);

	controllers->Enable("KeyboardController");

	FrontStateMachine* ssm = GetSubsystem<FrontStateMachine>();
	auto it = shellParameters_.Find(SP_SCENE);
	if (it != shellParameters_.End())
	{
		const String& sceneName = it->second_.GetString();
		it = shellParameters_.Find(SP_SERVER);
		if (it == shellParameters_.End())
			ssm->Initialize<LocalServerState>(sceneName);
		else
		{
			const String& serverName = it->second_.IsZero()
										   ? GetSubsystem<ShellConfigurator>()->GetGameName() + " Server"
										   : it->second_.GetString();
			const unsigned short port = GetSubsystem<ShellConfigurator>()->GetPort();
			ssm->Initialize<RemoteServerState>(sceneName, serverName, port);
		}
		return;
	}
	it = shellParameters_.Find(SP_CLIENT);
	if (it != shellParameters_.End())
	{
		const String& address = it->second_.GetString();
		const unsigned short port = GetSubsystem<ShellConfigurator>()->GetPort();
		ssm->Initialize<ClientState>(address, port);
		return;
	}
	ssm->Initialize<MainMenuState>();
}

void FrontApplication::Stop()
{
	context_->RemoveSubsystem<FrontStateMachine>();
	context_->RemoveSubsystem<ControllersRegistry>();
	CoreApplication::Stop();
}
