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
#include <Urho3D/IO/File.h>
#include <Urho3D/IO/FileSystem.h>
#include <Urho3D/IO/Log.h>
#include <Urho3D/Resource/XMLFile.h>
#include "Config/Config.h"
#include "Plugin/PluginsRegistry.h"
#include "Shell.h"
#include "ShellState/MainMenu.h"

#define CONFIG_FILENAME "Config.xml"
#define CONFIG_PATH "Config"
#define CONFIG_ROOT "config"
#define DEFAULT_GAME_NAME "SampleGame"
#define DEFAULT_PROFILE "Default"
#define GP_GAME_NAME "GameName"
#define INPUT_PATH "Input"
#define PLUGINS_PATH "Plugins"
#define PROFILE_FILENAME "Profile.txt"

using namespace Urho3D;

Shell::Shell(Urho3D::Context* context)
	: Object(context)
	, profileName_(DEFAULT_PROFILE)
{
	context_->RegisterSubsystem<Config>();
	context_->RegisterSubsystem<PluginsRegistry>();
}

Shell::~Shell()
{
	SaveProfile();
	SaveProfileName();
	context_->RemoveSubsystem<PluginsRegistry>();
}

void Shell::Setup(Urho3D::VariantMap& engineParameters)
{
	Variant gameName;
	if (engineParameters.TryGetValue(GP_GAME_NAME, gameName))
		gameName_ = gameName.GetString();
	else
	{
		gameName_ = DEFAULT_GAME_NAME;
		URHO3D_LOGWARNING("Using default game name because game name is not set.");
	}

	FileSystem* fileSystem = GetSubsystem<FileSystem>();

	const String& gameDataPath = GetGameDataPath();
	if (!fileSystem->DirExists(gameDataPath))
		fileSystem->CreateDir(gameDataPath);

	LoadProfileName();
	LoadProfile();

	GetSubsystem<Config>()->ExtractEngineParameters(engineParameters);
}

void Shell::Initialize()
{
	GetSubsystem<Config>()->Apply(false);
	shellState_ = new ShellState(context_);
}

void Shell::LoadProfile(const Urho3D::String& profileName)
{
	profileName_ = profileName;
	LoadProfile();
}

void Shell::LoadProfile()
{
	userDataPath_ = GetGameDataPath() + profileName_ + "/";

	FileSystem* fileSystem = GetSubsystem<FileSystem>();
	if (!fileSystem->DirExists(userDataPath_))
		fileSystem->CreateDir(userDataPath_);

	String path = userDataPath_ + CONFIG_PATH + "/";
	if (!fileSystem->DirExists(path))
		fileSystem->CreateDir(path);

	String filePath = path + CONFIG_FILENAME;
	Config* config = GetSubsystem<Config>();
	XMLFile file(context_);
	if (file.LoadFile(filePath))
		config->LoadXML(file.GetRoot(CONFIG_ROOT));

	path = userDataPath_ + INPUT_PATH + "/";
	if (!fileSystem->DirExists(path))
		fileSystem->CreateDir(path);

	path = userDataPath_ + PLUGINS_PATH + "/";
	if (!fileSystem->DirExists(path))
		fileSystem->CreateDir(path);

	GetSubsystem<PluginsRegistry>()->SetPluginsPath(path);
}

void Shell::SaveProfile() const
{
	XMLFile file(context_);
	XMLElement root = file.CreateRoot(CONFIG_ROOT);
	GetSubsystem<Config>()->SaveXML(root);
	file.SaveFile(userDataPath_ + CONFIG_PATH + "/" + CONFIG_FILENAME);
}

void Shell::CreateProfile(const Urho3D::String& profileName)
{
	GetSubsystem<FileSystem>()->CreateDir(GetGameDataPath() + profileName);
	LoadProfile(profileName);
}

void Shell::RemoveProfile(const Urho3D::String& profileName)
{
	// TODO: Recursive profile path removing
}

void Shell::LoadProfileName()
{
	File file(context_);
	if (file.Open(GetGameDataPath() + PROFILE_FILENAME, FileMode::FILE_READ))
		profileName_ = file.ReadString();
}

void Shell::SaveProfileName() const
{
	bool replaceFile = false;

	const String profileFile = GetGameDataPath() + PROFILE_FILENAME;
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

#ifdef _WIN32
Urho3D::String Shell::GetGameDataPath() const
{
	return GetSubsystem<FileSystem>()->GetUserDocumentsDir() + "My Games/" + gameName_ + "/";
}
#else
Urho3D::String Shell::GetGameDataPath() const
{
	return GetSubsystem<FileSystem>()->GetUserDocumentsDir() + gameName_ + "/";
}
#endif // _WIN32
