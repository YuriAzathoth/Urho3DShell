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
#include <Urho3D/Engine/Console.h>
#include <Urho3D/Engine/DebugHud.h>
#include <Urho3D/Engine/Engine.h>
#include <Urho3D/LuaScript/LuaScript.h>
#include <Urho3D/Resource/ResourceCache.h>
#include <Urho3D/UI/UI.h>
#include <Urho3D/Urho3DConfig.h>
#include "Config/Config.h"
#include "Core/ShellConfigurator.h"
#include "Core/ShellDefs.h"
#include "Core/ShellEvents.h"
#include "FrontShell.h"
#include "FrontState/ClientState.h"
#include "FrontState/FrontStateMachine.h"
#include "FrontState/LocalServerState.h"
#include "FrontState/MainMenuState.h"
#include "FrontState/RemoteServerState.h"
#include "Input/ControllersRegistry.h"
#include "UI/LoadGameDialog.h"
#include "UI/MainMenuDialog.h"
#include "UI/NewGameDialog.h"
#include "UI/PauseDialog.h"
#include "UI/ServersListDialog.h"
#include "UI/SettingsDialog.h"

extern void RegisterClientParameters(Config* config);
#ifdef URHO3D_ANGELSCRIPT
extern void RegisterClientAPI(asIScriptEngine* engine);
#endif // URHO3D_ANGELSCRIPT

using namespace Urho3D;

FrontShell::FrontShell(Urho3D::Context* context)
	: Object(context)
{
	RegisterClientParameters(GetSubsystem<Config>());

#ifdef URHO3D_ANGELSCRIPT
	RegisterClientAPI(GetSubsystem<Script>()->GetScriptEngine());
#endif // URHO3D_ANGELSCRIPT

	context_->RegisterFactory<LoadGameDialog>();
	context_->RegisterFactory<MainMenuDialog>();
	context_->RegisterFactory<NewGameDialog>();
	context_->RegisterFactory<PauseDialog>();
	context_->RegisterFactory<ServersListDialog>();
	context_->RegisterFactory<SettingsDialog>();
	context_->RegisterSubsystem<FrontStateMachine>();
}

FrontShell::~FrontShell()
{
	context_->RemoveSubsystem<FrontStateMachine>();
	context_->RemoveSubsystem<ControllersRegistry>();
}

void FrontShell::SetUIStyle(const Urho3D::String& filename)
{
	ResourceCache* cache = GetSubsystem<ResourceCache>();
	XMLFile* styleFile = cache->GetResource<XMLFile>(filename);
	GetSubsystem<UI>()->GetRoot()->SetDefaultStyle(styleFile);

	Engine* engine = GetSubsystem<Engine>();
	engine->CreateConsole()->SetDefaultStyle(styleFile);
	engine->CreateDebugHud()->SetDefaultStyle(styleFile);
}

void FrontShell::InitInput()
{
	ControllersRegistry* controllers = context_->RegisterSubsystem<ControllersRegistry>();

	SendEvent(E_SHELLCLIENTSTARTED);

	controllers->Enable("KeyboardController");
}

void FrontShell::StartMainMenu() { GetSubsystem<FrontStateMachine>()->Initialize<MainMenuState>(); }

void FrontShell::StartLocalServer(const Urho3D::String& sceneName)
{
	GetSubsystem<FrontStateMachine>()->Initialize<LocalServerState>(sceneName);
}

void FrontShell::StartRemoteServer(const Urho3D::String& sceneName, const Urho3D::String& serverName)
{
	const unsigned short port = GetSubsystem<ShellConfigurator>()->GetPort();
	GetSubsystem<FrontStateMachine>()->Initialize<RemoteServerState>(sceneName, serverName, port);
}

void FrontShell::StartClient(const Urho3D::String& address)
{
	const unsigned short port = GetSubsystem<ShellConfigurator>()->GetPort();
	GetSubsystem<FrontStateMachine>()->Initialize<ClientState>(address, port);
}
