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

#define URHO3D_WIN32_CONSOLE
#include <stdio.h>
#include <Urho3D/Container/Str.h>
#include <Urho3D/Engine/EngineDefs.h>
#include "Core/Launch.h"
#include "Plugin/PluginsRegistry.h"
#include "ServerApplication.h"

#define APP_NAME "Server"
#define GAME_LIB "SampleGame"
#define SCRIPT_FILE "Scripts/Editor.as"
#define UI_STYLE "UI/DefaultStyle.xml"

using namespace Urho3D;

void ServerApplication::Setup()
{
	core_ = MakeUnique<CoreShell>(context_);
	core_->LoadGameLibrary(GAME_LIB);
	core_->LoadConfig(engineParameters_, APP_NAME);

	engineParameters_[EP_HEADLESS] = true;
}

void ServerApplication::Start() { core_->ApplyConfig(); }

void ServerApplication::Stop() { core_.Reset(); }

URHO3D_DEFINE_APPLICATION_MAIN(ServerApplication)
