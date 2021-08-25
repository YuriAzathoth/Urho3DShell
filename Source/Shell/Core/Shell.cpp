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
#include "Shell.h"
#include "UI/UIController.h"

using namespace Urho3D;

Shell::Shell(Urho3D::Context* context)
	: Object(context)
	, port_(27500)
{
}

Shell::~Shell()
{
	client_.Delete();
	server_.Delete();
}

void Shell::StartMainMenu()
{
	client_.Delete();
	server_.Delete();

	UIController* uiController = GetSubsystem<UIController>();
	uiController->RemoveAllDialogs();
	uiController->CreateDialog("MainMenuDialog");
}

void Shell::StartLocalServer(Urho3D::String sceneName)
{
	auto InitServer = [this](StringHash, VariantMap&)
	{
		server_->Start(GetPort());
		server_->SetPausable(true);
		client_.New(context_);
		client_->Connect(GetPort());
		UIController* uiController = GetSubsystem<UIController>();
		uiController->RemoveAllDialogs();
		UnsubscribeFromEvent(E_ASYNCLOADFINISHED);
	};

	auto LoadScene =
		[this, sceneName = std::move(sceneName), InitServer = std::move(InitServer)](StringHash, VariantMap&)
	{
		server_.New(context_);
		server_->LoadScene(sceneName);
		SubscribeToEvent(E_ASYNCLOADFINISHED, InitServer);
		UnsubscribeFromEvent(E_SERVERDISCONNECTED);
	};

	const bool disconnect = client_.NotNull();
	client_.Delete();
	server_.Delete();
	if (disconnect)
		SubscribeToEvent(E_SERVERDISCONNECTED, LoadScene);
	else
		LoadScene(StringHash::ZERO, GetEventDataMap());
}

void Shell::StartRemoteServer(Urho3D::String serverName, Urho3D::String sceneName)
{
	auto InitServer = [this, serverName = std::move(serverName)](StringHash, VariantMap&)
	{
		server_->Start(GetPort());
		server_->MakeVisible(serverName);
		server_->SetPausable(false);
		client_.New(context_);
		client_->Connect(GetPort());
		UIController* uiController = GetSubsystem<UIController>();
		uiController->RemoveAllDialogs();
		UnsubscribeFromEvent(E_ASYNCLOADFINISHED);
	};

	auto LoadScene =
		[this, sceneName = std::move(sceneName), InitServer = std::move(InitServer)](StringHash, VariantMap&)
	{
		server_.New(context_);
		server_->LoadScene(sceneName);
		SubscribeToEvent(E_ASYNCLOADFINISHED, InitServer);
		UnsubscribeFromEvent(E_SERVERDISCONNECTED);
	};

	const bool disconnect = client_.NotNull();
	client_.Delete();
	server_.Delete();
	if (disconnect)
		SubscribeToEvent(E_SERVERDISCONNECTED, LoadScene);
	else
		LoadScene(StringHash::ZERO, GetEventDataMap());
}

void Shell::StartClient(Urho3D::String address)
{
	auto InitClient = [this, address = std::move(address)](StringHash, VariantMap&)
	{
		client_.New(context_);
		client_->Connect(GetPort(), address);
		UIController* uiController = GetSubsystem<UIController>();
		uiController->RemoveAllDialogs();
		UnsubscribeFromEvent(E_SERVERDISCONNECTED);
	};

	const bool disconnect = client_.NotNull();
	client_.Delete();
	server_.Delete();
	if (disconnect)
		SubscribeToEvent(E_SERVERDISCONNECTED, InitClient);
	else
		InitClient(StringHash::ZERO, GetEventDataMap());
}
