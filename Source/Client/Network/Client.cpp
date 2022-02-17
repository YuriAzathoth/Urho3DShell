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
#include <Urho3D/Physics/PhysicsEvents.h>
#include <Urho3D/Scene/SceneEvents.h>
#include "Client.h"
#include "Core/ShellEvents.h"
#include "Input/ControllersRegistry.h"
#include "Network/NetworkEvents.h"
#include "Network/ServerDefs.h"

using namespace Urho3D;

Client::Client(Urho3D::Context* context)
	: Object(context)
	, scene_(context)
	, connection_(nullptr)
{
	URHO3D_LOGTRACE("Client::Client");
	SubscribeToEvent(E_ASYNCLOADFINISHED, URHO3D_HANDLER(Client, OnSceneLoaded));
	SubscribeToEvent(E_CONNECTFAILED, URHO3D_HANDLER(Client, OnConnectFailed));
}

Client::~Client()
{
	URHO3D_LOGTRACE("Client::~Client");
	Disconnect();
}

bool Client::Connect(unsigned short port, const Urho3D::String& address)
{
	URHO3D_LOGTRACEF("Client::Connect(%s)", address.CString());
	VariantMap identity;
	identity[CL_NAME] = "SimpleName";
	if (GetSubsystem<Network>()->Connect(address, port, &scene_, identity))
	{
		SendEvent(E_REMOTECLIENTSTARTED);
		SubscribeToEvent(E_SERVERCONNECTED, URHO3D_HANDLER(Client, OnServerConnected));
		return true;
	}
	else
		return false;
}

void Client::Disconnect()
{
	URHO3D_LOGTRACE("Client::Disconnect");
	Network* network = GetSubsystem<Network>();
	Connection* connection = network->GetServerConnection();
	if (connection && connection->IsConnected())
	{
		network->Disconnect();
		SendEvent(E_REMOTECLIENTSTOPPED);
	}
}

void Client::OnServerConnected(Urho3D::StringHash, Urho3D::VariantMap&)
{
	connection_ = GetSubsystem<Network>()->GetServerConnection();
	SubscribeToEvent(E_SCENEUPDATE, URHO3D_HANDLER(Client, OnSceneUpdated));
	SubscribeToEvent(E_PHYSICSPRESTEP, URHO3D_HANDLER(Client, OnPhysicsPreStep));
}

void Client::OnConnectFailed(Urho3D::StringHash, Urho3D::VariantMap&) { URHO3D_LOGTRACE("Client::OnConnectFailed"); }

void Client::OnSceneLoaded(Urho3D::StringHash, Urho3D::VariantMap&) { URHO3D_LOGTRACE("Client::OnSceneLoaded"); }

void Client::OnSceneUpdated(Urho3D::StringHash, Urho3D::VariantMap&)
{
	GetSubsystem<ControllersRegistry>()->ReadControls(controls_);
}

void Client::OnPhysicsPreStep(Urho3D::StringHash, Urho3D::VariantMap&)
{
	connection_->SetControls(controls_);
	controls_.buttons_ = 0;
}
