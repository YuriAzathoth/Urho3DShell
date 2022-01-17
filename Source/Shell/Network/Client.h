//
// Copyright (c) 2021-2022 Yuriy Zinchenko.
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

#ifndef CLIENT_H
#define CLIENT_H

#include <Urho3D/Core/Object.h>
#include <Urho3D/Input/Controls.h>
#include <Urho3D/Scene/Scene.h>
#include "Urho3DShellAPI.h"

namespace Urho3D
{
class Connection;
}
class InputController;

class URHO3DSHELLAPI_EXPORT Client : public Urho3D::Object
{
	URHO3D_OBJECT(Client, Urho3D::Object)

public:
	explicit Client(Urho3D::Context* context);
	~Client();

	void Connect(unsigned short port, const Urho3D::String& address = "localhost");
	void Disconnect();

private:
	void OnServerConnected(Urho3D::StringHash, Urho3D::VariantMap&);
	void OnConnectFailed(Urho3D::StringHash, Urho3D::VariantMap&);
	void OnSceneLoaded(Urho3D::StringHash, Urho3D::VariantMap&);
	void OnSceneUpdated(Urho3D::StringHash, Urho3D::VariantMap&);
	void OnPhysicsPreStep(Urho3D::StringHash, Urho3D::VariantMap&);

	Urho3D::Controls controls_;
	Urho3D::Scene scene_;
	Urho3D::Connection* connection_;
};

#endif // CLIENT_H
