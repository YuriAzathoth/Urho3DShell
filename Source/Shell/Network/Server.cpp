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
#include "Server.h"
#include "ServerDefs.h"

#define DEFAULT_PORT 27500

using namespace Urho3D;

Server::Server(Urho3D::Context* context)
	: Object(context)
	, scene_(context)
	, serverPort_(27500)
	, pausable_(false)
	, remote_(false)
{
	SubscribeToEvent(E_CLIENTCONNECTED, URHO3D_HANDLER(Server, OnClientConnected));
	SubscribeToEvent(E_CLIENTIDENTITY, URHO3D_HANDLER(Server, OnClientIdentity));
	SubscribeToEvent(E_CLIENTSCENELOADED, URHO3D_HANDLER(Server, OnClientSceneLoaded));
	SubscribeToEvent(E_ASYNCLOADFINISHED, URHO3D_HANDLER(Server, OnServerSceneLoaded));
}

Server::~Server() { Stop(); }

void Server::LoadScene(const Urho3D::String& sceneName)
{
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
	SetUpdate(true);
	scene_.Clear();
}

void Server::Start()
{
	GetSubsystem<Network>()->StartServer(serverPort_);
	remote_ = false;
}

void Server::Stop()
{
	Network* network = GetSubsystem<Network>();
	if (network->IsServerRunning())
		network->StopServer();
	remote_ = false;
}

void Server::OnClientConnected(Urho3D::StringHash, Urho3D::VariantMap& /*eventData*/)
{
	URHO3D_LOGTRACE("Server::OnClientConnected");
	using namespace ClientConnected;
	//	Connection* connection = static_cast<Connection*>(eventData[P_CONNECTION].GetPtr());
}

void Server::OnClientIdentity(Urho3D::StringHash, Urho3D::VariantMap& eventData)
{
	URHO3D_LOGTRACE("Server::OnClientIdentity");
	using namespace ClientIdentity;
	Connection* connection = static_cast<Connection*>(eventData[P_CONNECTION].GetPtr());
	//	const String& clientName = eventData[CL_NAME].GetString();
	//	const String& clientPass = eventData[CL_PASSWORD].GetString();
	connection->SetScene(&scene_);
}

void Server::OnClientSceneLoaded(Urho3D::StringHash, Urho3D::VariantMap& /*eventData*/)
{
	URHO3D_LOGTRACE("Server::OnClientSceneLoaded");
}

void Server::OnServerSceneLoaded(Urho3D::StringHash, Urho3D::VariantMap&)
{
	URHO3D_LOGTRACE("Server::OnServerSceneLoaded");
}
