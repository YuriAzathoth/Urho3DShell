#ifndef USERPROFILE_H
#define USERPROFILE_H

#include <Urho3D/Core/Object.h>
#include "Urho3DShellAPI.h"

class URHO3DSHELLAPI_EXPORT UserProfile : public Urho3D::Object
{
	URHO3D_OBJECT(UserProfile, Urho3D::Object)

public:
	using Urho3D::Object::Object;
	~UserProfile();

	void Initialize(const Urho3D::String& gameName);

	void LoadProfile(const Urho3D::String& profileName);
	void SaveProfile();

	const Urho3D::String& GetUserDataPath() const noexcept { return userDataPath_; }

private:
	Urho3D::String GetGameDataPath() const;

	Urho3D::String profileName_;
	Urho3D::String gameName_;
	Urho3D::String userDataPath_;
};

#endif // USERPROFILE_H
