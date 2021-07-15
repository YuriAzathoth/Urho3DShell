#include <Urho3D/IO/File.h>
#include <Urho3D/IO/FileSystem.h>
#include <Urho3D/Resource/XMLFile.h>
#include "Config/Config.h"
#include "Plugin/PluginsRegistry.h"
#include "UserProfile.h"

using namespace Urho3D;

UserProfile::~UserProfile()
{
	if (!profileName_.Empty())
		SaveProfile();

	const String profileFile = GetGameDataPath() + PROFILE_FILENAME;
	bool replaceFile = false;
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
		File file(context_, profileFile, FILE_WRITE);
		file.WriteString(profileName_);
	}
}

void UserProfile::Initialize(const Urho3D::String& gameName)
{
	gameName_ = gameName;

	const String& gameDataPath = GetGameDataPath();
	FileSystem* fileSystem = GetSubsystem<FileSystem>();
	if (!fileSystem->DirExists(gameDataPath))
		fileSystem->CreateDir(gameDataPath);

	const String profileFile = gameDataPath + PROFILE_FILENAME;
	if (fileSystem->FileExists(profileFile))
	{
		File file(context_);
		file.Open(profileFile, FILE_READ);
		LoadProfile(file.ReadString());
	}
	else
		LoadProfile(DEFAULT_PROFILE);
}

void UserProfile::LoadProfile(const Urho3D::String& profileName)
{
	profileName_ = profileName;
	userDataPath_ = GetGameDataPath() + profileName_ + "/";

	FileSystem* fileSystem = GetSubsystem<FileSystem>();
	const String userDataPath = GetUserDataPath();
	if (!fileSystem->DirExists(userDataPath))
		fileSystem->CreateDir(userDataPath);

	String filePath = userDataPath + CONFIG_PATH + "/" + CONFIG_FILENAME;
	Config* config = GetSubsystem<Config>();
	XMLFile file(context_);
	if (file.LoadFile(filePath))
		config->LoadXML(file.GetRoot(CONFIG_ROOT));

	GetSubsystem<PluginsRegistry>()->SetPluginsPath(userDataPath + PLUGINS_PATH + "/");
}

void UserProfile::SaveProfile()
{
	XMLFile file(context_);
	XMLElement root = file.CreateRoot(CONFIG_ROOT);
	GetSubsystem<Config>()->SaveXML(root);
	file.SaveFile(GetUserDataPath() + CONFIG_PATH + "/" + CONFIG_FILENAME);
}

#ifdef _WIN32
Urho3D::String UserProfile::GetGameDataPath() const
{
	return GetSubsystem<FileSystem>()->GetUserDocumentsDir() + "My Games/" + gameName_ + "/";
}
#else
Urho3D::String UserProfile::GetGameDataPath() const
{
	return GetSubsystem<FileSystem>()->GetUserDocumentsDir() + gameName_ + "/";
}
#endif // _WIN32
