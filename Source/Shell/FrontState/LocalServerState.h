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

#ifndef LOCALSERVERSTATE_H
#define LOCALSERVERSTATE_H

#include "Network/Client.h"
#include "ServerState.h"

#define LOCAL_SERVER_PORT 10

class URHO3DSHELLAPI_EXPORT LocalServerState : public ServerState
{
	URHO3D_OBJECT(LocalServerState, ServerState)

public:
	explicit LocalServerState(Urho3D::Context* context,
							  const Urho3D::String& sceneName,
							  unsigned short port = LOCAL_SERVER_PORT);

	void Exit() override;

protected:
	void OnSceneLoaded() override;

	Client client_;
	unsigned short port_;

private:
	// On Shutdown
	void OnServerDisconnected(Urho3D::StringHash, Urho3D::VariantMap&);
};

#endif // LOCALSERVERSTATE_H
