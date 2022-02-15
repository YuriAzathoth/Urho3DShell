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

#include <Urho3D/Network/Network.h>
#include <Urho3D/Network/NetworkEvents.h>
#include <Urho3D/Scene/Scene.h>
#include "Input/InputReceiver.h"
#include "Network/NetworkEvents.h"
#include "PluginInterfaceCore.h"

using namespace Urho3D;

PluginInterfaceCore::PluginInterfaceCore(Urho3D::Context* context)
	: PluginInterface(context)
{
	SubscribeToEvent(E_CLIENTSCENELOADED, URHO3D_HANDLER(PluginInterfaceCore, OnClientSceneLoaded));
	SubscribeToEvent(E_REMOTECLIENTSTARTED, URHO3D_HANDLER(PluginInterfaceCore, OnRemoteClientStarted));
	SubscribeToEvent(E_REMOTECLIENTSTOPPED, URHO3D_HANDLER(PluginInterfaceCore, OnRemoteClientStopped));
	Setup();
}

void PluginInterfaceCore::OnClientSceneLoaded(Urho3D::StringHash, Urho3D::VariantMap& eventData)
{
	using namespace ClientSceneLoaded;
	Connection* connection = static_cast<Connection*>(eventData[P_CONNECTION].GetPtr());
	Scene* scene = connection->GetScene();

	const unsigned nodeId = Spawn(scene);
	if (nodeId)
	{
		Node* clientNode = scene->GetNode(nodeId);

		InputReceiver* receiver = clientNode->CreateComponent<InputReceiver>(REPLICATED);
		receiver->SetConnection(connection);
		receiver->SetTemporary(true);

		using namespace ServerSideSpawned;
		eventData[P_NODE] = nodeId;
		connection->SendRemoteEvent(E_SERVERSIDESPAWNED, true, eventData);
	}
}

void PluginInterfaceCore::OnRemoteClientStarted(Urho3D::StringHash, Urho3D::VariantMap&)
{
	Network* network = GetSubsystem<Network>();
	network->RegisterRemoteEvent(E_SERVERSIDESPAWNED);
	Start();
}

void PluginInterfaceCore::OnRemoteClientStopped(Urho3D::StringHash, Urho3D::VariantMap&)
{
	Network* network = GetSubsystem<Network>();
	network->UnregisterRemoteEvent(E_SERVERSIDESPAWNED);
	Stop();
}
