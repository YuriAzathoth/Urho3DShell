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

void Shell::NewShellState(ShellState* newState)
{
	nextState_ = newState;
	currState_->Exit();
}

bool Shell::ProcessStateChanging()
{
	currState_ = std::move(nextState_);
	currState_->Enter();
	return true;
}
