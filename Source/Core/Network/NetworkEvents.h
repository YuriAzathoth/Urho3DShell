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

#ifndef NETWORKEVENTS_H
#define NETWORKEVENTS_H

#include <Urho3D/Core/Object.h>

URHO3D_EVENT(E_REMOTECLIENTSTARTED, RemoteClientStarted) {}
URHO3D_EVENT(E_REMOTECLIENTSTOPPED, RemoteClientStopped) {}
URHO3D_EVENT(E_REMOTESERVERSTARTED, RemoteServerStarted) {}
URHO3D_EVENT(E_REMOTESERVERSTOPPED, RemoteServerStopped) {}

URHO3D_EVENT(E_SERVERSIDERESPAWNED, ServerSideRespawned)
{
	URHO3D_PARAM(P_CONNECTION, Connection); // Connection ptr
	URHO3D_PARAM(P_NODE, Node);				// int
}

URHO3D_EVENT(E_SERVERSIDESPAWNED, ServerSideSpawned)
{
	URHO3D_PARAM(P_CONNECTION, Connection); // Connection ptr
	URHO3D_PARAM(P_NODE, Node);				// int
}

#endif // NETWORKEVENTS_H
