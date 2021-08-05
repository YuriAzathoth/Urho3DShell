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

// IMPLEMENTATION ATTENTION!
// Do not remove all UI widgets before any usage of strings' references!
// Removing all widgets will cause references to null objects.

#include <Urho3D/Core/Context.h>
#include "Core/Shell.h"
#include "ShellState.h"
#include "UI/UIController.h"

using namespace Urho3D;

ShellState::ShellState(Urho3D::Context* context)
	: Urho3D::Object(context)
{
	StartMainMenu();
}

void ShellState::StartMainMenu()
{
	client_.Reset();
	server_.Reset();

	UIController* uiController = GetSubsystem<UIController>();
	uiController->RemoveAllDialogs();
	uiController->CreateDialog("MainMenuWindow");
}

void ShellState::StartLocalServer(const Urho3D::String& sceneName)
{
	server_ = MakeUnique<Server>(context_);
	server_->Start(GetSubsystem<Shell>()->GetPort());
	server_->LoadScene(sceneName);
	server_->SetPausable(true);

	client_ = MakeUnique<Client>(context_);
	client_->Connect();

	UIController* uiController = GetSubsystem<UIController>();
	uiController->RemoveAllDialogs();
}

void ShellState::StartRemoteServer(const Urho3D::String& serverName, const Urho3D::String& sceneName)
{
	server_ = MakeUnique<Server>(context_);
	server_->Start(GetSubsystem<Shell>()->GetPort());
	server_->MakeVisible(serverName);
	server_->LoadScene(sceneName);
	server_->SetPausable(false);

	client_ = MakeUnique<Client>(context_);
	client_->Connect();

	UIController* uiController = GetSubsystem<UIController>();
	uiController->RemoveAllDialogs();
}

void ShellState::StartClient(const Urho3D::String& address)
{
	server_.Reset();

	client_ = MakeUnique<Client>(context_);
	client_->Connect(address);

	UIController* uiController = GetSubsystem<UIController>();
	uiController->RemoveAllDialogs();
}
