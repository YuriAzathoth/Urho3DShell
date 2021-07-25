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
#include <Urho3D/Resource/JSONFile.h>
#include <Urho3D/Resource/ResourceCache.h>
#include <Urho3D/Resource/XMLFile.h>
#include "Config.h"
#include "Configurator.h"
#include "Plugin/PluginsRegistry.h"

#define CONFIG_ROOT "config"
#define DEFAULT_PROFILE "Default"
#define GAME_FILE "Game.json"

using namespace Urho3D;

Configurator::Configurator(Urho3D::Context* context)
	: Object(context)
	, appName_("SampleGame")
	, profileName_(DEFAULT_PROFILE)
	, userDataPath_("./")
{
	if (!ConfigureGame())
	{
		GetSubsystem<Engine>()->Exit();
		return;
	}

	FileSystem* fileSystem = GetSubsystem<FileSystem>();
	String path = GetGameDataPath();
	if (!fileSystem->DirExists(path))
		fileSystem->CreateDir(path);
}

Configurator::~Configurator()
{
	SaveProfile();
	SaveProfileName();
}

void Configurator::Initialize()
{
	LoadProfileName();
	LoadProfile();
}

void Configurator::LoadProfile(const Urho3D::String& profileName)
{
	profileName_ = profileName;
	LoadProfile();
}

void Configurator::LoadProfile()
{
	FileSystem* fileSystem = GetSubsystem<FileSystem>();

	userDataPath_ = GetGameDataPath() + profileName_ + "/";
	if (!fileSystem->DirExists(userDataPath_))
		fileSystem->CreateDir(userDataPath_);

	String path = GetConfigPath();
	if (fileSystem->DirExists(path))
	{
		path = GetConfigFilename();
		Config* config = GetSubsystem<Config>();
		XMLFile file(context_);
		if (fileSystem->FileExists(path) && file.LoadFile(path))
			config->LoadXML(file.GetRoot(CONFIG_ROOT));
	}
	else
		fileSystem->CreateDir(path);

	path = GetInputPath();
	if (!fileSystem->DirExists(path))
		fileSystem->CreateDir(path);

	path = GetLogsPath();
	if (!fileSystem->DirExists(path))
		fileSystem->CreateDir(path);

	path = GetPluginsPath();
	if (!fileSystem->DirExists(path))
		fileSystem->CreateDir(path);
	GetSubsystem<PluginsRegistry>()->SetPluginsPath(path);
}

void Configurator::SaveProfile() const
{
	XMLFile file(context_);
	XMLElement root = file.CreateRoot(CONFIG_ROOT);
	GetSubsystem<Config>()->SaveXML(root);
	file.SaveFile(GetConfigFilename());
}

void Configurator::CreateProfile(const Urho3D::String& profileName)
{
	GetSubsystem<FileSystem>()->CreateDir(GetGameDataPath() + profileName);
	LoadProfile(profileName);
}

void Configurator::RemoveProfile(const Urho3D::String& profileName)
{
	// TODO: Recursive profile path removing
}

bool Configurator::ConfigureGame()
{
	JSONFile file(context_);
	if (!file.LoadFile(GetSubsystem<FileSystem>()->GetProgramDir() + GAME_FILE))
		return false;
	const JSONValue& root = file.GetRoot();
	if (root.IsNull())
		return false;
	gameName_ = root.Get("name").GetString("");
	const String gameDll = root.Get("lib").GetString("");
	if (!gameName_.Empty())
		return true;
	else
		return false;
}

void Configurator::LoadProfileName()
{
	File file(context_);
	if (file.Open(GetProfileFilename(), FileMode::FILE_READ))
		profileName_ = file.ReadString();
}

void Configurator::SaveProfileName()
{
	bool replaceFile = false;

	const String profileFile = GetProfileFilename();
	if (GetSubsystem<FileSystem>()->FileExists(profileFile))
	{
		File file(context_, profileFile, FILE_READ);
		if (file.ReadString() != profileName_)
			replaceFile = true;
	}
	else
		replaceFile = true;

	if (replaceFile)
	{
		File file(context_, profileFile, FileMode::FILE_WRITE);
		file.WriteString(profileName_);
	}
}

Urho3D::String Configurator::GetConfigFilename() const { return GetConfigPath() + appName_ + ".xml"; }
Urho3D::String Configurator::GetConfigPath() const { return userDataPath_ + "Config/"; }
Urho3D::String Configurator::GetInputPath() const { return userDataPath_ + "Input/"; }
Urho3D::String Configurator::GetLogsFilename() const { return GetLogsPath() + appName_ + ".log"; }
Urho3D::String Configurator::GetLogsPath() const { return userDataPath_ + "Errorlogs/"; }
Urho3D::String Configurator::GetPluginsPath() const { return userDataPath_ + "Plugins/"; }
Urho3D::String Configurator::GetProfileFilename() const { return GetGameDataPath() + "Profile.txt"; }

// TODO: Move to preferences dir
#ifdef _WIN32
Urho3D::String Configurator::GetGameDataPath() const
{
	return GetSubsystem<FileSystem>()->GetUserDocumentsDir() + "My Games/" + gameName_ + "/";
}
#else
Urho3D::String Configurator::GetGameDataPath() const
{
	return GetSubsystem<FileSystem>()->GetUserDocumentsDir() + gameName_ + "/";
}
#endif // _WIN32
