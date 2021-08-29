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
#include "Shell.h"
#include "ShellState/ClientState.h"
#include "ShellState/LocalServerState.h"
#include "ShellState/MainMenuState.h"
#include "ShellState/RemoteServerState.h"

using namespace Urho3D;

Shell::Shell(Urho3D::Context* context)
	: Object(context)
	, port_(27500)
{
}

void Shell::Initialize()
{
	currState_ = new MainMenuState(context_);
	currState_->Enter();
}

void Shell::StartMainMenu() { PushNewState(new MainMenuState(context_)); }

void Shell::StartLocalServer(Urho3D::String sceneName)
{
	PushNewState(new LocalServerState(context_, std::move(sceneName)));
}

void Shell::StartRemoteServer(Urho3D::String serverName, Urho3D::String sceneName)
{
	PushNewState(new RemoteServerState(context_, std::move(sceneName), std::move(serverName), port_));
}

void Shell::StartClient(Urho3D::String address)
{
	PushNewState(new ClientState(context_, std::move(address), port_));
}

void Shell::PushNewState(ShellState* newState)
{
	nextState_ = newState;
	currState_->Exit();
}

bool Shell::ProcessStateChanging()
{
	if (nextState_.NotNull())
	{
		currState_ = std::move(nextState_);
		currState_->Enter();
		return true;
	}
	else
	{
		URHO3D_LOGERRORF("Failed to change shell state: command queue is empty.");
		return false;
	}
}
