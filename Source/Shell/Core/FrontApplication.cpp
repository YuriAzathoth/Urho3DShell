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
#include <Urho3D/Input/InputEvents.h>
#include <Urho3D/LuaScript/LuaScript.h>
#include <Urho3D/Resource/ResourceCache.h>
#include <Urho3D/UI/UI.h>
#include "Config/Config.h"
#include "FrontApplication.h"
#include "Input/ControllersRegistry.h"
#include "ScriptAPI/AngelScript/ScriptAPI.h"
#include "ScriptAPI/Lua/LuaScriptAPI.h"
#include "Shell.h"
#include "ShellEvents.h"
#include "UI/UIController.h"

using namespace Urho3D;

FrontApplication::FrontApplication(Urho3D::Context* context, Urho3D::VariantMap&& shellParameters)
	: CoreApplication(context, std::move(shellParameters))
{
	GetSubsystem<Config>()->RegisterClientParameters();
}

void FrontApplication::Setup()
{
	CoreApplication::Setup();
	context_->RegisterSubsystem<Shell>();
	RegisterClientAPI(GetSubsystem<Script>()->GetScriptEngine());
	RegisterClientLuaAPI(GetSubsystem<LuaScript>()->GetState());
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

	context_->RegisterSubsystem<UIController>();

	GetSubsystem<Shell>()->StartMainMenu();

	ControllersRegistry* controllers = context_->RegisterSubsystem<ControllersRegistry>();

	SendEvent(E_SHELLCLIENTSTARTED);

	controllers->EnableController("KeyboardController");

	SubscribeToEvent(E_KEYDOWN, URHO3D_HANDLER(FrontApplication, OnKeyDown));
}

void FrontApplication::Stop()
{
	context_->RemoveSubsystem<Shell>();
	context_->RemoveSubsystem<ControllersRegistry>();
	CoreApplication::Stop();
}

void FrontApplication::OnKeyDown(Urho3D::StringHash, Urho3D::VariantMap& eventData)
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
