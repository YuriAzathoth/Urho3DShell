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

#include <Urho3D/IO/File.h>
#include <Urho3D/IO/FileSystem.h>
#include <Urho3D/Resource/XMLFile.h>
#include <boost/filesystem.hpp>
#include "Config/Config.h"
#include "Plugin/PluginsRegistry.h"
#include "ShellConfigurator.h"

#define CONFIG_ROOT "config"
#define DEFAULT_PROFILE "Default"

using namespace Urho3D;

ShellConfigurator::ShellConfigurator(Urho3D::Context* context)
	: Object(context)
	, profileName_(DEFAULT_PROFILE)
	, userDataPath_("./")
{
	FileSystem* fileSystem = GetSubsystem<FileSystem>();
	const String path = GetGameDataPath();
	if (fileSystem->DirExists(path))
	{
		File file(context_);
		if (file.Open(GetProfileFilename(), FileMode::FILE_READ))
			profileName_ = file.ReadString();
	}
	else
		fileSystem->CreateDir(path);
}

ShellConfigurator::~ShellConfigurator()
{
	SaveProfile();
	File file(context_, GetProfileFilename(), FileMode::FILE_WRITE);
	file.WriteString(profileName_);
}

void ShellConfigurator::Initialize(Urho3D::VariantMap& shellParameters)
{
	if (InitProfile())
	{
		const String filename = GetConfigFilename();
		XMLFile file(context_);
		if (GetSubsystem<FileSystem>()->FileExists(filename) && file.LoadFile(filename))
			GetSubsystem<Config>()->LoadXML(shellParameters, file.GetRoot(CONFIG_ROOT));
	}
}

void ShellConfigurator::LoadProfile(const Urho3D::String& profileName)
{
	SaveProfile();
	profileName_ = profileName;
	if (InitProfile())
	{
		const String filename = GetConfigFilename();
		XMLFile file(context_);
		if (GetSubsystem<FileSystem>()->FileExists(filename) && file.LoadFile(filename))
			GetSubsystem<Config>()->LoadXML(file.GetRoot(CONFIG_ROOT));
	}
}

bool ShellConfigurator::InitProfile()
{
	userDataPath_ = GetGameDataPath() + profileName_ + "/";

	FileSystem* fileSystem = GetSubsystem<FileSystem>();
	if (!fileSystem->DirExists(userDataPath_))
		fileSystem->CreateDir(userDataPath_);

	String path = GetInputPath();
	if (!fileSystem->DirExists(path))
		fileSystem->CreateDir(path);

	path = GetLogsPath();
	if (!fileSystem->DirExists(path))
		fileSystem->CreateDir(path);

	path = GetPluginsPath();
	if (!fileSystem->DirExists(path))
		fileSystem->CreateDir(path);
	GetSubsystem<PluginsRegistry>()->SetPluginsPath(path);

	path = GetSavesPath();
	if (!fileSystem->DirExists(path))
		fileSystem->CreateDir(path);

	path = GetConfigPath();
	const bool configFound = fileSystem->DirExists(path);
	if (!configFound)
		fileSystem->CreateDir(path);
	return configFound;
}

void ShellConfigurator::SaveProfile() const
{
	XMLFile file(context_);
	XMLElement root = file.CreateRoot(CONFIG_ROOT);
	GetSubsystem<Config>()->SaveXML(root);
	file.SaveFile(GetConfigFilename());
}

void ShellConfigurator::CreateProfile(const Urho3D::String& profileName)
{
	GetSubsystem<FileSystem>()->CreateDir(GetGameDataPath() + profileName);
	LoadProfile(profileName);
}

void ShellConfigurator::RemoveProfile(const Urho3D::String& profileName)
{
	const String path = GetGameDataPath() + profileName;
	boost::filesystem::remove_all(path.CString());
}

Urho3D::String ShellConfigurator::GetConfigFilename() const { return GetConfigPath() + appName_ + ".xml"; }
Urho3D::String ShellConfigurator::GetConfigPath() const { return userDataPath_ + "Config/"; }
Urho3D::String ShellConfigurator::GetInputPath() const { return userDataPath_ + "Input/"; }
Urho3D::String ShellConfigurator::GetLogsFilename() const { return GetLogsPath() + appName_ + ".log"; }
Urho3D::String ShellConfigurator::GetLogsPath() const { return userDataPath_ + "Errorlogs/"; }
Urho3D::String ShellConfigurator::GetPluginsPath() const { return userDataPath_ + "Plugins/"; }
Urho3D::String ShellConfigurator::GetProfileFilename() const { return GetGameDataPath() + "Profile.txt"; }
Urho3D::String ShellConfigurator::GetSavesPath() const { return userDataPath_ + "Saves/"; }

void ShellConfigurator::CreatePath(const Urho3D::String& path) const
{
	FileSystem* fileSystem = GetSubsystem<FileSystem>();
	if (!fileSystem->DirExists(path))
		fileSystem->CreateDir(path);
}

Urho3D::String ShellConfigurator::GetGameDataPath() const
{
	String ret = GetSubsystem<FileSystem>()->GetUserDocumentsDir();
#ifdef _WIN32
	ret.Append("My Games/");
#endif // _WIN32
	ret.Append(gameName_).Append('/');
	return ret;
}