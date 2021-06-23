#include <Urho3D/IO/File.h>
#include <Urho3D/IO/FileSystem.h>
#include "UserProfile.h"

#define PROFILE_FILENAME "Profile.txt"

using namespace Urho3D;

UserProfile::~UserProfile()
{
	const String profileFile = gameDataPath_ + PROFILE_FILENAME;
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
#ifdef _WIN32
	gameDataPath_ = GetSubsystem<FileSystem>()->GetUserDocumentsDir() + "My Games/" + gameName + "/";
#else
	gameDataPath_ = GetSubsystem<FileSystem>()->GetUserDocumentsDir() + gameName + "/";
#endif // _WIN32

	FileSystem* fileSystem = GetSubsystem<FileSystem>();
	if (!fileSystem->DirExists(gameDataPath_))
		fileSystem->CreateDir(gameDataPath_);

	const String profileFile = gameDataPath_ + PROFILE_FILENAME;
	if (fileSystem->FileExists(profileFile))
	{
		File file(context_);
		file.Open(profileFile, FILE_READ);
		profileName_ = file.ReadString();
	}
	else
		profileName_ = "Default";

	const String userDataPath = GetUserDataPath();
	if (!fileSystem->DirExists(userDataPath))
		fileSystem->CreateDir(userDataPath);
}

Urho3D::String UserProfile::GetUserDataPath() const { return gameDataPath_ + profileName_ + "/"; }
