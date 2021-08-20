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

#ifndef SERVER_H
#define SERVER_H

#include <Urho3D/Core/Object.h>
#include <Urho3D/Scene/Scene.h>
#include "Urho3DShellAPI.h"

class URHO3DSHELLAPI_EXPORT Server : public Urho3D::Object
{
	URHO3D_OBJECT(Server, Urho3D::Object)

public:
	explicit Server(Urho3D::Context* context);
	~Server();

	void LoadScene(const Urho3D::String& sceneName);
	void ClearScene();

	void Start(unsigned short port);
	void Stop();

	void MakeVisible(const Urho3D::String& serverName);

	void SetPausable(bool pausable) noexcept { pausable_ = pausable; }
	void SetUpdate(bool update);

	bool IsPausable() const noexcept { return pausable_; }
	bool IsRemote() const noexcept { return remote_; }
	bool IsUpdate() const { return scene_.IsUpdateEnabled(); }

private:
	void OnClientConnected(Urho3D::StringHash, Urho3D::VariantMap& eventData);
	void OnClientDisconnected(Urho3D::StringHash, Urho3D::VariantMap& eventData);
	void OnClientIdentity(Urho3D::StringHash, Urho3D::VariantMap& eventData);
	void OnClientSceneLoaded(Urho3D::StringHash, Urho3D::VariantMap& eventData);
	void OnServerSceneLoaded(Urho3D::StringHash, Urho3D::VariantMap&);

	Urho3D::Scene scene_;
	bool pausable_;
	bool remote_;
};

#endif // SERVER_H
