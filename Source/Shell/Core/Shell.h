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
#include "ShellState/ShellState.h"
#include "Urho3DShellAPI.h"

class URHO3DSHELLAPI_EXPORT Shell : public Urho3D::Object
{
	URHO3D_OBJECT(Shell, Urho3D::Object)

public:
	explicit Shell(Urho3D::Context* context);
	~Shell();

	void Setup(Urho3D::VariantMap& engineParameters);
	void Initialize();

	void LoadProfile(const Urho3D::String& profileName);
	void SaveProfile() const;
	void CreateProfile(const Urho3D::String& profileName);
	void RemoveProfile(const Urho3D::String& profileName);

	Urho3D::String GetConfigFilename() const;
	Urho3D::String GetConfigPath() const;
	Urho3D::String GetInputPath() const;
	Urho3D::String GetLogsFilename() const;
	Urho3D::String GetLogsPath() const;
	Urho3D::String GetPluginsPath() const;
	Urho3D::String GetProfileFilename() const;

	const Urho3D::String& GetProfileName() const noexcept { return profileName_; }

private:
	bool PreconfigureEngine();
	void LoadProfile();
	void LoadProfileName();
	void SaveProfileName();

	Urho3D::String GetGameDataPath() const;

	void ParseParameters(const Urho3D::StringVector& arguments);
	Urho3D::Variant GetParameter(Urho3D::StringHash parameter, const Urho3D::Variant& defaultValue);

	Urho3D::SharedPtr<ShellState> shellState_;
	Urho3D::VariantMap shellParameters_;
	Urho3D::String appName_;
	Urho3D::String profileName_;
	Urho3D::String userDataPath_;
	bool client_;
};

#endif // SHELL_H
