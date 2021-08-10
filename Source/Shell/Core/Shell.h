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

#ifndef SHELL_H
#define SHELL_H

#include <Urho3D/Core/Object.h>
#include "Urho3DShellAPI.h"

class URHO3DSHELLAPI_EXPORT Shell : public Urho3D::Object
{
	URHO3D_OBJECT(Shell, Urho3D::Object)

public:
	explicit Shell(Urho3D::Context* context);
	~Shell();

	void Setup(Urho3D::VariantMap& engineParameters);
	void Initialize();

	void StartMainMenu();
	void StartLocalServer(Urho3D::String sceneName);
	void StartRemoteServer(Urho3D::String serverName, Urho3D::String sceneName);
	void StartClient(Urho3D::String address);

	void LoadProfile(const Urho3D::String& profileName);
	void SaveProfile() const;
	void CreateProfile(const Urho3D::String& profileName);
	void RemoveProfile(const Urho3D::String& profileName);

	void SetPort(unsigned short port) noexcept { port_ = port; }

	Urho3D::String GetConfigFilename() const;
	Urho3D::String GetConfigPath() const;
	Urho3D::String GetInputPath() const;
	Urho3D::String GetLogsFilename() const;
	Urho3D::String GetLogsPath() const;
	Urho3D::String GetPluginsPath() const;
	Urho3D::String GetProfileFilename() const;
	Urho3D::String GetSavesPath() const;

	unsigned short GetPort() const noexcept { return port_; }
	const Urho3D::String& GetProfileName() const noexcept { return profileName_; }
	bool IsClientLaunched() const noexcept { return client_; }

private:
	bool PreconfigureEngine();
	void LoadProfile();
	void LoadProfileName();
	void SaveProfileName();

	Urho3D::String GetGameDataPath() const;

	void ParseParameters(const Urho3D::StringVector& arguments);
	Urho3D::Variant GetParameter(Urho3D::StringHash parameter, const Urho3D::Variant& defaultValue);

	void OnKeyDown(Urho3D::StringHash, Urho3D::VariantMap& eventData);

	Urho3D::VariantMap shellParameters_;
	Urho3D::String appName_;
	Urho3D::String gameName_;
	Urho3D::String gameLibrary_;
	Urho3D::String profileName_;
	Urho3D::String userDataPath_;
	unsigned short port_; // TODO: Move to something else location
	bool client_;
};

#endif // SHELL_H
