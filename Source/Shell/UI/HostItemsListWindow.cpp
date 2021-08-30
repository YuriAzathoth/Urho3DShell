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

#include "Core/ShellConfigurator.h"
#include "HostItemsListWindow.h"
#include "ShellState/LocalServerState.h"
#include "ShellState/RemoteServerState.h"
#include "ShellState/ShellStateMachine.h"

using namespace Urho3D;

void HostItemsListWindow::Start(const Urho3D::String& sceneName)
{
	GetSubsystem<ShellStateMachine>()->NewShellState<LocalServerState>(sceneName);
}

void HostItemsListWindow::Start(const Urho3D::String& sceneName,
								const Urho3D::String& serverName,
								[[maybe_unused]] const Urho3D::String& serverPass)
{
	const unsigned short port = GetSubsystem<ShellConfigurator>()->GetPort();
	GetSubsystem<ShellStateMachine>()->NewShellState<RemoteServerState>(sceneName, serverName, port);
}
