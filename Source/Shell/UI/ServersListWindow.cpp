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
#include <Urho3D/Network/NetworkEvents.h>
#include "Core/Shell.h"
#include "Network/ServerDefs.h"
#include "ServersListWindow.h"
#include "ShellState/ShellState.h"

using namespace Urho3D;

ServersListWindow::ServersListWindow(Urho3D::Context* context)
	: StartGameWindow(context)
{
	SetServerSettingsVisible(false);
	SetInteractive(true);
	SetTitle("ConnectToServer");

	SubscribeToEvent(E_NETWORKHOSTDISCOVERED, URHO3D_HANDLER(ServersListWindow, OnNetworkHostDiscovered));

	GetSubsystem<Network>()->DiscoverHosts(GetSubsystem<Shell>()->GetPort());
}

void ServersListWindow::Start(const Urho3D::String& gameName)
{
	GetSubsystem<ShellState>()->StartClient(gameName);
}

void ServersListWindow::OnNetworkHostDiscovered(Urho3D::StringHash, Urho3D::VariantMap& eventData)
{
	using namespace NetworkHostDiscovered;
	const VariantMap& hostBeacon = eventData[P_BEACON].GetVariantMap();
	if (!hostBeacon.Empty())
	{
		const String& address = eventData[P_ADDRESS].GetString();
		const String& serverName = hostBeacon[SV_NAME]->GetString();
		AddGame(serverName, address);
	}
}
