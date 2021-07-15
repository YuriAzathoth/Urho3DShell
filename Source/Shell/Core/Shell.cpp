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

#include <Urho3D/Core/Context.h>
#include <Urho3D/Engine/EngineDefs.h>
#include <Urho3D/IO/Log.h>
#include <Urho3D/Resource/XMLFile.h>
#include "Config/Config.h"
#include "Core/UserProfile.h"
#include "Plugin/PluginsRegistry.h"
#include "Shell.h"

#define CONFIG_FILENAME "Config.xml"
#define CONFIG_PATH "Config"
#define CONFIG_ROOT "config"
#define DEFAULT_PROFILE "Default"
#define PLUGINS_PATH "Plugins"
#define PROFILE_FILENAME "Profile.txt"

using namespace Urho3D;

Shell::Shell(Urho3D::Context* context)
	: Object(context)
{
	context_->RegisterSubsystem<UserProfile>();
	context_->RegisterSubsystem<Config>();
	context_->RegisterSubsystem<PluginsRegistry>();
}

Shell::~Shell()
{
	context_->RemoveSubsystem<Config>();
	context_->RemoveSubsystem<PluginsRegistry>();
	context_->RemoveSubsystem<UserProfile>();
}

void Shell::Setup(Urho3D::VariantMap& engineParameters)
{
	Variant gameName;
	if (engineParameters.TryGetValue(EP_WINDOW_TITLE, gameName))
		GetSubsystem<UserProfile>()->Initialize(gameName.GetString());
	else
	{
		GetSubsystem<UserProfile>()->Initialize("SampleGame");
		URHO3D_LOGWARNING("Game name is not set. Game name is now \"SampleGame\".");
	}

	GetSubsystem<Config>()->ExtractEngineParameters(engineParameters);
}

void Shell::Initialize() { GetSubsystem<Config>()->Apply(false); }
