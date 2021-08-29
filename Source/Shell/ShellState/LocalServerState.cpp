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
#include "LocalServerState.h"

#define LOCAL_SERVER_PORT 0

using namespace Urho3D;

LocalServerState::LocalServerState(Urho3D::Context* context, Urho3D::String&& sceneName)
	: ShellState(context)
	, server_(context)
	, client_(context)
	, sceneName_(std::move(sceneName))
{
}

void LocalServerState::Enter()
{
	server_.LoadScene(sceneName_);
	SubscribeToEvent(E_ASYNCLOADFINISHED, URHO3D_HANDLER(LocalServerState, OnAsyncLoadFinished));
}

void LocalServerState::Exit()
{
	client_.Disconnect();
	server_.Stop();
	SubscribeToEvent(E_SERVERDISCONNECTED, URHO3D_HANDLER(LocalServerState, OnServerDisconnected));
}

void LocalServerState::BackState() {}

void LocalServerState::SetSceneUpdate(bool update) { server_.SetUpdate(update); }

void LocalServerState::OnAsyncLoadFinished(Urho3D::StringHash, Urho3D::VariantMap&)
{
	server_.Start(LOCAL_SERVER_PORT);
	server_.SetPausable(true);
	client_.Connect(LOCAL_SERVER_PORT);
	RemoveAllDialogs();
	UnsubscribeFromEvent(E_ASYNCLOADFINISHED);
}

void LocalServerState::OnServerDisconnected(Urho3D::StringHash, Urho3D::VariantMap&) { ReleaseSelf(); }
