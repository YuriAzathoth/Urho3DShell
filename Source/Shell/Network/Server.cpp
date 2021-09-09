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

#include <Urho3D/IO/Log.h>
#include <Urho3D/Network/Network.h>
#include <Urho3D/Network/NetworkEvents.h>
#include <Urho3D/Resource/ResourceCache.h>
#include <Urho3D/Resource/XMLFile.h>
#include <Urho3D/Scene/SceneEvents.h>
#include "NetworkEvents.h"
#include "Server.h"
#include "ServerDefs.h"

using namespace Urho3D;

Server::Server(Urho3D::Context* context)
	: Object(context)
	, scene_(context)
	, pausable_(false)
	, remote_(false)
{
	URHO3D_LOGTRACE("Server::Server");
	SubscribeToEvent(E_CLIENTCONNECTED, URHO3D_HANDLER(Server, OnClientConnected));
	SubscribeToEvent(E_CLIENTDISCONNECTED, URHO3D_HANDLER(Server, OnClientDisconnected));
	SubscribeToEvent(E_CLIENTIDENTITY, URHO3D_HANDLER(Server, OnClientIdentity));
	SubscribeToEvent(E_CLIENTSCENELOADED, URHO3D_HANDLER(Server, OnClientSceneLoaded));
	SubscribeToEvent(E_SERVERSIDERESPAWNED, URHO3D_HANDLER(Server, OnServerSideRespawned));
	SubscribeToEvent(E_SERVERSIDESPAWNED, URHO3D_HANDLER(Server, OnServerSideSpawned));
}

Server::~Server()
{
	URHO3D_LOGTRACE("Server::~Server");
	Stop();
}

void Server::LoadScene(const Urho3D::String& sceneName)
{
	URHO3D_LOGTRACEF("Server::LoadScene(%s)", sceneName.CString());
	SharedPtr<File> file = GetSubsystem<ResourceCache>()->GetFile(sceneName);
	if (sceneName.EndsWith(".xml", false))
		scene_.LoadAsyncXML(file);
	else if (sceneName.EndsWith(".bin", false))
		scene_.LoadAsync(file);
	else if (sceneName.EndsWith(".json", false))
		scene_.LoadAsyncJSON(file);
	else
		URHO3D_LOGERRORF("Failed to load scene %s: unsupported file format.", sceneName.CString());
}

void Server::ClearScene()
{
	URHO3D_LOGTRACE("Server::ClearScene");
	SetUpdate(true);
	scene_.Clear();
}

void Server::Start(unsigned short port)
{
	URHO3D_LOGTRACEF("Server::Start(%u)", port);
	GetSubsystem<Network>()->StartServer(port);
	remote_ = false;
}

void Server::Stop()
{
	URHO3D_LOGTRACE("Server::Stop");
	Network* network = GetSubsystem<Network>();
	if (network->IsServerRunning())
	{
		network->StopServer();
		SendEvent(E_REMOTESERVERSTOPPED);
	}
	if (remote_)
		network->SetDiscoveryBeacon(Variant::emptyVariantMap);
	nodes_.Clear();
	scene_.Clear();
	remote_ = false;
}

void Server::MakeVisible(const Urho3D::String& serverName)
{
	URHO3D_LOGTRACEF("Server::MakeVisible(%s)", serverName.CString());
	VariantMap hostBeacon;
	hostBeacon[SV_NAME] = serverName;
	hostBeacon[SV_SCENE] = scene_.GetFileName();
	hostBeacon[SV_PLAYERS] = 0;
	hostBeacon[SV_PLAYERS_MAX] = 128;
	GetSubsystem<Network>()->SetDiscoveryBeacon(hostBeacon);
	remote_ = true;
	SendEvent(E_REMOTESERVERSTARTED);
}

void Server::SetUpdate(bool update)
{
	if (pausable_)
		scene_.SetUpdateEnabled(update);
}

void Server::OnClientConnected(Urho3D::StringHash, Urho3D::VariantMap& eventData)
{
	using namespace ClientConnected;
	const Connection* connection = static_cast<Connection*>(eventData[P_CONNECTION].GetPtr());
	URHO3D_LOGTRACEF("Server::OnClientConnected %s", connection->ToString().CString());
}

void Server::OnClientDisconnected(Urho3D::StringHash, Urho3D::VariantMap& eventData)
{
	using namespace ClientConnected;
	const Connection* connection = static_cast<Connection*>(eventData[P_CONNECTION].GetPtr());
	const String address = connection->ToString();
	auto it = nodes_.Find(address);
	if (it != nodes_.End())
	{
		Node* node = scene_.GetNode(it->second_);
		if (node)
			node->Remove();
		nodes_.Erase(it);
	}
	URHO3D_LOGTRACEF("Server::OnClientDisconnected %s", address);
}

void Server::OnClientIdentity(Urho3D::StringHash, Urho3D::VariantMap& eventData)
{
	using namespace ClientIdentity;
	Connection* connection = static_cast<Connection*>(eventData[P_CONNECTION].GetPtr());
	connection->SetScene(&scene_);
	const String& clientName = eventData[CL_NAME].GetString();
	URHO3D_LOGTRACEF("Server::OnServerIdentity %s name %s", connection->ToString().CString(), clientName.CString());
}

void Server::OnClientSceneLoaded(Urho3D::StringHash, Urho3D::VariantMap& eventData)
{
	using namespace ClientSceneLoaded;
	const Connection* connection = static_cast<Connection*>(eventData[P_CONNECTION].GetPtr());
	URHO3D_LOGTRACEF("Server::OnClientSceneLoaded %s", connection->ToString().CString());
}

void Server::OnServerSideRespawned(Urho3D::StringHash, Urho3D::VariantMap& eventData)
{
	using namespace ServerSideRespawned;
	const Connection* connection = static_cast<Connection*>(eventData[P_CONNECTION].GetPtr());
	const String address = connection->ToString();
	const unsigned nodeId = eventData[P_NODE].GetInt();
	auto it = nodes_.Find(address);
	if (it != nodes_.End())
	{
		Node* node = scene_.GetNode(it->second_);
		if (node)
			node->Remove();
		it->second_ = nodeId;
	}
	URHO3D_LOGTRACEF("Server::OnServerSideDespawned %s", address.CString());
}

void Server::OnServerSideSpawned(Urho3D::StringHash, Urho3D::VariantMap& eventData)
{
	using namespace ServerSideSpawned;
	const Connection* connection = static_cast<Connection*>(eventData[P_CONNECTION].GetPtr());
	const String address = connection->ToString();
	const unsigned nodeId = eventData[P_NODE].GetInt();
	nodes_[address] = nodeId;
	URHO3D_LOGTRACEF("Server::OnClientSceneLoaded %s node %u", address.CString(), nodeId);
}
