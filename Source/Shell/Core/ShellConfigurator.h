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

#ifndef SHELLCONFIGURATOR_H
#define SHELLCONFIGURATOR_H

#include <Urho3D/Core/Object.h>
#include "Urho3DShellAPI.h"

class URHO3DSHELLAPI_EXPORT ShellConfigurator : public Urho3D::Object
{
	URHO3D_OBJECT(ShellConfigurator, Urho3D::Object)

public:
	explicit ShellConfigurator(Urho3D::Context* context);
	~ShellConfigurator();

	void Initialize(Urho3D::VariantMap& engineParameters, Urho3D::VariantMap& shellParameters);
	void LoadProfile(const Urho3D::String& profileName);
	void SaveProfile() const;
	void CreateProfile(const Urho3D::String& profileName);
	void RemoveProfile(const Urho3D::String& profileName);

	Urho3D::String GetConfigFilename() const;
	Urho3D::String GetConfigPath() const;
	Urho3D::String GetInputPath() const;
	Urho3D::String GetLogsFilename() const;
	Urho3D::String GetLogsPath() const;
	Urho3D::String GetPluginsFilename() const;
	Urho3D::String GetPluginsPath() const;
	Urho3D::String GetProfileFilename() const;
	Urho3D::String GetSavesPath() const;

	void SetGameName(const Urho3D::String& gameName) { gameName_ = gameName; }

	const Urho3D::String& GetAppName() const noexcept { return appName_; }
	const Urho3D::String& GetGameName() const noexcept { return gameName_; }
	const Urho3D::String& GetProfileName() const noexcept { return profileName_; }

private:
	void CreatePath(const Urho3D::String& path) const;
	Urho3D::String GetGameDataPath() const;
	bool InitProfile();

	Urho3D::String appName_;
	Urho3D::String gameName_;
	Urho3D::String profileName_;
	Urho3D::String userDataPath_;
};

#endif // SHELLCONFIGURATOR_H
