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
#include "ClientState.h"

using namespace Urho3D;

ClientState::ClientState(Urho3D::Context* context, Urho3D::String&& address, unsigned short port)
	: ShellState(context)
	, client_(context)
	, address_(std::move(address))
	, port_(port)
{
}

void ClientState::Enter() { client_.Connect(port_, address_); }

void ClientState::Exit()
{
	client_.Disconnect();
	SubscribeToEvent(E_SERVERDISCONNECTED, URHO3D_HANDLER(ClientState, OnServerDisconnected));
}

void ClientState::BackState() {}

void ClientState::SetSceneUpdate(bool) {}

void ClientState::OnServerDisconnected(Urho3D::StringHash, Urho3D::VariantMap&) { ReleaseSelf(); }