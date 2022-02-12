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

#include <Urho3D/IO/Log.h>
#include <Urho3D/Network/Network.h>
#include <Urho3D/Network/NetworkEvents.h>
#include <Urho3D/Scene/Scene.h>
#include "ActionsDefs.h"
#include "Core/ShellConfigurator.h"
#include "Input/ActionsRegistry.h"
#include "Input/InputReceiver.h"
#include "Network/NetworkEvents.h"
#include "SampleActorController.h"
#include "SamplePluginCore.h"

using namespace Urho3D;

const String SamplePluginCore::PLUGIN_NAME = "Sample game";

SamplePluginCore::SamplePluginCore(Urho3D::Context* context)
	: PluginInterface(context)
{
	GetSubsystem<ShellConfigurator>()->SetGameName("SampleGame");

	SubscribeToEvent(E_REMOTECLIENTSTARTED, URHO3D_HANDLER(SamplePluginCore, OnRemoteClientStarted));
	SubscribeToEvent(E_REMOTECLIENTSTOPPED, URHO3D_HANDLER(SamplePluginCore, OnRemoteClientStopped));
	SubscribeToEvent(E_CLIENTSCENELOADED, URHO3D_HANDLER(SamplePluginCore, OnClientSceneLoaded));

	REGISTER_OBJECT(SampleActorController);

	ActionsRegistry* actions = GetSubsystem<ActionsRegistry>();
	actions->RegisterRemote(MOVE_FORWARD);
	actions->RegisterRemote(MOVE_BACK);
	actions->RegisterRemote(MOVE_LEFT);
	actions->RegisterRemote(MOVE_RIGHT);
	actions->RegisterRemote(MOVE_UP);
	actions->RegisterRemote(MOVE_DOWN);
	actions->RegisterRemote(WALK);
}

const Urho3D::String& SamplePluginCore::GetName() const { return PLUGIN_NAME; }

void SamplePluginCore::OnRemoteClientStarted(Urho3D::StringHash, Urho3D::VariantMap&)
{
	Network* network = GetSubsystem<Network>();
	network->RegisterRemoteEvent(E_SERVERSIDESPAWNED);
}

void SamplePluginCore::OnRemoteClientStopped(Urho3D::StringHash, Urho3D::VariantMap&)
{
	Network* network = GetSubsystem<Network>();
	network->UnregisterAllRemoteEvents();
}

void SamplePluginCore::OnClientSceneLoaded(Urho3D::StringHash, Urho3D::VariantMap& eventData)
{
	URHO3D_LOGTRACE("SamplePluginCore::OnClientSceneLoaded");

	using namespace ClientSceneLoaded;
	Connection* connection = static_cast<Connection*>(eventData[P_CONNECTION].GetPtr());
	Scene* scene = connection->GetScene();
	Node* node = scene->CreateChild();
	node->SetPosition({0.0f, 4.0f, 0.0f});
	node->Pitch(30.0f);

	node->CreateComponent<SampleActorController>(REPLICATED);

	InputReceiver* receiver = node->CreateComponent<InputReceiver>(REPLICATED);
	receiver->SetConnection(connection);
	receiver->SetTemporary(true);

	using namespace ServerSideSpawned;
	eventData[P_NODE] = node->GetID();
	connection->SendRemoteEvent(E_SERVERSIDESPAWNED, true, eventData);
}

URHO3DSHELL_PLUGIN(SamplePluginCore)
