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

#include <Urho3D/Graphics/Camera.h>
#include <Urho3D/Graphics/Renderer.h>
#include <Urho3D/Graphics/Viewport.h>
#include <Urho3D/IO/Log.h>
#include <Urho3D/Input/InputConstants.h>
#include <Urho3D/Network/Network.h>
#include <Urho3D/Network/NetworkEvents.h>
#include <Urho3D/Scene/Scene.h>
#include "Core/ShellEvents.h"
#include "GameEvents.h"
#include "Input/InputController.h"
#include "Input/InputRegistry.h"
#include "Network/NetworkEvents.h"
#include "SampleGamePlugin.h"

using namespace Urho3D;

static const String PLUGIN_NAME = "Sample game";

static const String MOVE_FORWARD = "MoveForward";
static const String MOVE_BACK = "MoveBack";
static const String MOVE_LEFT = "MoveLeft";
static const String MOVE_RIGHT = "MoveRight";

SampleGamePlugin::SampleGamePlugin(Urho3D::Context* context)
	: PluginInterface(context)
{
	SubscribeToEvent(E_SHELLCLIENTSTARTED, URHO3D_HANDLER(SampleGamePlugin, OnShellClientStarted));
	SubscribeToEvent(E_REMOTECLIENTSTARTED, URHO3D_HANDLER(SampleGamePlugin, OnRemoteClientStarted));
	SubscribeToEvent(E_REMOTECLIENTSTOPPED, URHO3D_HANDLER(SampleGamePlugin, OnRemoteClientStopped));
	SubscribeToEvent(E_REMOTESERVERSTARTED, URHO3D_HANDLER(SampleGamePlugin, OnRemoteServerStarted));
	SubscribeToEvent(E_REMOTESERVERSTOPPED, URHO3D_HANDLER(SampleGamePlugin, OnRemoteServerStopped));
	SubscribeToEvent(E_CLIENTSCENELOADED, URHO3D_HANDLER(SampleGamePlugin, OnClientSceneLoaded));
	SubscribeToEvent(E_SERVERSIDESPAWNED, URHO3D_HANDLER(SampleGamePlugin, OnServerSideSpawned));

	InputRegistry* inputRegistry = GetSubsystem<InputRegistry>();
	inputRegistry->RegisterActionRemote(MOVE_FORWARD);
	inputRegistry->RegisterActionRemote(MOVE_BACK);
	inputRegistry->RegisterActionRemote(MOVE_LEFT);
	inputRegistry->RegisterActionRemote(MOVE_RIGHT);
}

const Urho3D::String& SampleGamePlugin::GetName() const { return PLUGIN_NAME; }

void SampleGamePlugin::OnShellClientStarted(Urho3D::StringHash, Urho3D::VariantMap&)
{
	InputRegistry* inputRegistry = GetSubsystem<InputRegistry>();
	InputController* keyboard = inputRegistry->GetController("KeyboardController");
	if (keyboard)
	{
		keyboard->SetDefaultBinding(MOVE_FORWARD, KEY_W);
		keyboard->SetDefaultBinding(MOVE_BACK, KEY_S);
		keyboard->SetDefaultBinding(MOVE_LEFT, KEY_A);
		keyboard->SetDefaultBinding(MOVE_RIGHT, KEY_D);
	}
}
void SampleGamePlugin::OnRemoteClientStarted(Urho3D::StringHash, Urho3D::VariantMap&)
{
	Network* network = GetSubsystem<Network>();
	network->RegisterRemoteEvent(E_SERVERSIDESPAWNED);
}

void SampleGamePlugin::OnRemoteClientStopped(Urho3D::StringHash, Urho3D::VariantMap&)
{
	Network* network = GetSubsystem<Network>();
	network->UnregisterAllRemoteEvents();
}

void SampleGamePlugin::OnRemoteServerStarted(Urho3D::StringHash, Urho3D::VariantMap&) {}

void SampleGamePlugin::OnRemoteServerStopped(Urho3D::StringHash, Urho3D::VariantMap&) {}

void SampleGamePlugin::OnClientSceneLoaded(Urho3D::StringHash, Urho3D::VariantMap& eventData)
{
	URHO3D_LOGTRACE("SampleGamePlugin::OnClientSceneLoaded");

	using namespace ClientSceneLoaded;
	Connection* connection = static_cast<Connection*>(eventData[P_CONNECTION].GetPtr());
	Scene* scene = connection->GetScene();
	Node* node = scene->CreateChild();
	node->SetPosition({0.0f, 4.0f, 0.0f});
	node->Pitch(30.0f);

	using namespace ServerSideSpawned;
	eventData[P_NODE] = node->GetID();
	connection->SendRemoteEvent(E_SERVERSIDESPAWNED, true, eventData);
}

void SampleGamePlugin::OnServerSideSpawned(Urho3D::StringHash, Urho3D::VariantMap& eventData)
{
	URHO3D_LOGTRACE("SampleGamePlugin::OnServerSideSpawned");

	using namespace ServerSideSpawned;
	const Connection* connection = static_cast<Connection*>(eventData[P_CONNECTION].GetPtr());
	const unsigned nodeId = eventData[P_NODE].GetInt();
	Scene* scene = connection->GetScene();
	Node* node = scene->GetNode(nodeId);

	Camera* camera = node->CreateComponent<Camera>();
	SharedPtr<Viewport> viewport = MakeShared<Viewport>(context_, scene, camera);
	Renderer* renderer = GetSubsystem<Renderer>();
	renderer->SetViewport(0, viewport);
}

URHO3DSHELL_PLUGIN_REGISTER(SampleGamePlugin)
