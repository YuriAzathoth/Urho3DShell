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

#include <Urho3D/Network/Connection.h>
#include "Core/ShellEvents.h"
#include "Network/NetworkEvents.h"
#include "PluginInterfaceClient.h"

using namespace Urho3D;

PluginInterfaceClient::PluginInterfaceClient(Urho3D::Context* context)
	: PluginInterface(context)
{
	SubscribeToEvent(E_REMOTESERVERSTARTED, [this](StringHash, VariantMap) { Start(); });
	SubscribeToEvent(E_REMOTESERVERSTOPPED, [this](StringHash, VariantMap) { Stop(); });
	SubscribeToEvent(E_SHELLCLIENTSTARTED, [this](StringHash, VariantMap) { Setup(); });
	SubscribeToEvent(E_SERVERSIDESPAWNED, URHO3D_HANDLER(PluginInterfaceClient, OnServerSideSpawned));
}

void PluginInterfaceClient::OnServerSideSpawned(Urho3D::StringHash, Urho3D::VariantMap& eventData)
{
	using namespace ServerSideSpawned;
	const Connection* connection = static_cast<Connection*>(eventData[P_CONNECTION].GetPtr());
	const unsigned nodeId = eventData[P_NODE].GetInt();
	Scene* scene = connection->GetScene();
	Spawn(scene, nodeId);
}
