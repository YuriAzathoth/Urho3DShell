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
#include <Urho3D/UI/UI.h>
#include <boost/filesystem.hpp>
#include "Config/Config.h"
#include "Plugin/PluginsRegistry.h"
#include "ScriptAPI/ScriptAPI.h"
#include "Shell.h"
#include "ShellState/MainMenu.h"

#define CONFIG_ROOT "config"
#define DEFAULT_APP_NAME "Unknown"
#define DEFAULT_PROFILE "Default"

#define LP_GAME_LIB "GameLib"
#define LP_NO_CLIENT "NoClient"
#define LP_SCRIPT "Script"

void RegisterMainMenuWindow(Urho3D::Context* context);
void RegisterSettingsWindow(Urho3D::Context* context);

using namespace Urho3D;

Shell::Shell(Urho3D::Context* context)
	: Object(context)
	, appName_(DEFAULT_APP_NAME)
	, profileName_(DEFAULT_PROFILE)
	, userDataPath_("./")
	, client_(true)
{
	context_->RegisterSubsystem<Script>();
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
	ParseParameters(GetArguments());

	if (!PreconfigureEngine())
	{
		URHO3D_LOGERRORF("Failed to load game library.");
		GetSubsystem<Engine>()->Exit();
		return;
	}

	client_ = !(GetParameter(LP_NO_CLIENT, false).GetBool() || GetParameter(EP_HEADLESS, false).GetBool());

	Config* config = GetSubsystem<Config>();
	config->RegisterServerParameters();
	if (client_)
		config->RegisterClientParameters();
	config->ExtractEngineParameters(engineParameters);

	asIScriptEngine* engine = GetSubsystem<Script>()->GetScriptEngine();
	RegisterServerAPI(engine);
	if (client_)
		RegisterClientAPI(engine);

	FileSystem* fileSystem = GetSubsystem<FileSystem>();
	const String path = GetGameDataPath();
	if (!fileSystem->DirExists(path))
		fileSystem->CreateDir(path);

	LoadProfileName();
	LoadProfile();

	engineParameters[EP_LOG_NAME] = GetLogsFilename();
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
		RegisterSettingsWindow(context_);

		shellState_ = new MainMenu(context_);
	}

	const auto itScript = shellParameters_.Find(LP_SCRIPT);
	if (itScript != shellParameters_.End())
		GetSubsystem<PluginsRegistry>()->RegisterPlugin(itScript->second_.GetString());

	shellParameters_.Clear();

	GetSubsystem<Config>()->Apply(false);
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

	const String& libraryName = libraryNameValue.GetString();
	return GetSubsystem<PluginsRegistry>()->RegisterPlugin(libraryName);
}

void Shell::LoadProfile(const Urho3D::String& profileName)
{
	profileName_ = profileName;
	LoadProfile();
}

void Shell::LoadProfile()
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

void Shell::SaveProfile() const
{
	XMLFile file(context_);
	XMLElement root = file.CreateRoot(CONFIG_ROOT);
	GetSubsystem<Config>()->SaveXML(root);
	file.SaveFile(GetConfigFilename());
}

void Shell::CreateProfile(const Urho3D::String& profileName)
{
	GetSubsystem<FileSystem>()->CreateDir(GetGameDataPath() + profileName);
	LoadProfile(profileName);
}

void Shell::RemoveProfile(const Urho3D::String& profileName)
{
	const String path = GetGameDataPath() + profileName;
	boost::filesystem::remove_all(path.CString());
}

void Shell::LoadProfileName()
{
	File file(context_);
	if (file.Open(GetProfileFilename(), FileMode::FILE_READ))
		profileName_ = file.ReadString();
}

void Shell::SaveProfileName()
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

Urho3D::String Shell::GetConfigFilename() const { return GetConfigPath() + appName_ + ".xml"; }
Urho3D::String Shell::GetConfigPath() const { return userDataPath_ + "Config/"; }
Urho3D::String Shell::GetInputPath() const { return userDataPath_ + "Input/"; }
Urho3D::String Shell::GetLogsFilename() const { return GetLogsPath() + appName_ + ".log"; }
Urho3D::String Shell::GetLogsPath() const { return userDataPath_ + "Errorlogs/"; }
Urho3D::String Shell::GetPluginsPath() const { return userDataPath_ + "Plugins/"; }
Urho3D::String Shell::GetProfileFilename() const { return GetGameDataPath() + "Profile.txt"; }

Urho3D::String Shell::GetGameDataPath() const
{
	String ret = GetSubsystem<FileSystem>()->GetUserDocumentsDir();
#ifdef _WIN32
	ret.Append("My Games/");
#endif // _WIN32
	ret.Append("SampleGame").Append('/');
	return ret;
}

void Shell::ParseParameters(const Urho3D::StringVector& arguments)
{
	String argument, value;
	for (unsigned i = 0; i < arguments.Size(); ++i)
		if (arguments[i].Length() > 1 && arguments[i][0] == '-')
		{
			argument = arguments[i].Substring(1).ToLower();
			value = i + 1 < arguments.Size() ? arguments[i + 1] : String::EMPTY;
			if (argument == "appname")
			{
				appName_ = value;
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
