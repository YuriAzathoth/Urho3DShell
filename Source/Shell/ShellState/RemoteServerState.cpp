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

#include <Urho3D/Network/NetworkEvents.h>
#include <Urho3D/Scene/SceneEvents.h>
#include "RemoteServerState.h"

using namespace Urho3D;

RemoteServerState::RemoteServerState(Urho3D::Context* context,
									 Urho3D::String&& sceneName,
									 Urho3D::String&& serverName,
									 unsigned short port)
	: ShellState(context)
	, server_(context)
	, client_(context)
	, sceneName_(std::move(sceneName))
	, serverName_(std::move(serverName))
	, port_(port)
{
}

void RemoteServerState::Enter()
{
	server_.LoadScene(sceneName_);
	SubscribeToEvent(E_ASYNCLOADFINISHED, URHO3D_HANDLER(RemoteServerState, OnAsyncLoadFinished));
}

void RemoteServerState::Exit()
{
	client_.Disconnect();
	server_.Stop();
	SubscribeToEvent(E_SERVERDISCONNECTED, URHO3D_HANDLER(RemoteServerState, OnServerDisconnected));
}

void RemoteServerState::BackState() {}

void RemoteServerState::SetSceneUpdate(bool update) { server_.SetUpdate(update); }

void RemoteServerState::OnAsyncLoadFinished(Urho3D::StringHash, Urho3D::VariantMap&)
{
	server_.Start(port_);
	client_.Connect(port_);
	server_.MakeVisible(serverName_);
	RemoveAllDialogs();
	UnsubscribeFromEvent(E_ASYNCLOADFINISHED);
}

void RemoteServerState::OnServerDisconnected(Urho3D::StringHash, Urho3D::VariantMap&) { ReleaseSelf(); }
