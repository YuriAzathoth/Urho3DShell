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

#ifndef GAMESTATE_H
#define GAMESTATE_H

#include <Urho3D/Core/Object.h>
#include "Network/Client.h"
#include "Network/Server.h"
#include "Urho3DShellAPI.h"

class URHO3DSHELLAPI_EXPORT ShellState : public Urho3D::Object
{
	URHO3D_OBJECT(ShellState, Urho3D::Object)

public:
	explicit ShellState(Urho3D::Context* context);

	void StartMainMenu();
	void StartLocalServer(const Urho3D::String& sceneName);
	void StartRemoteServer(const Urho3D::String& serverName, const Urho3D::String& sceneName);
	void StartClient(const Urho3D::String& address);

	Client* GetClient() const { return client_.Get(); }
	Server* GetServer() const { return server_.Get(); }

private:
	Urho3D::UniquePtr<Server> server_;
	Urho3D::UniquePtr<Client> client_;
};

#endif // GAMESTATE_H
