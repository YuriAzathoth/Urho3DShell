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

#include <Urho3D/Network/Network.h>
#include <Urho3D/Scene/SceneEvents.h>
#include "Client.h"
#include "Core/Shell.h"
#include "Core/ShellEvents.h"
#include "NetworkEvents.h"
#include "ServerDefs.h"

using namespace Urho3D;

Client::Client(Urho3D::Context* context)
	: Object(context)
	, scene_(context)
{
}

Client::~Client() { Disconnect(); }

void Client::Connect(const Urho3D::String& address)
{
	VariantMap identity;
	identity[CL_NAME] = "SimpleName";

	GetSubsystem<Network>()->Connect(address, GetSubsystem<Shell>()->GetPort(), &scene_, identity);

	SendEvent(E_REMOTECLIENTSTARTED);
}

void Client::Disconnect()
{
	Network* network = GetSubsystem<Network>();
	if (network->GetServerConnection())
	{
		network->Disconnect();
		SendEvent(E_REMOTECLIENTSTOPPED);
	}
}

void Client::OnSceneLoaded(Urho3D::StringHash, Urho3D::VariantMap&) {}
