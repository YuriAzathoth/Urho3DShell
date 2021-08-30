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
#include <Urho3D/Scene/SceneEvents.h>
#include "HeadlessApplication.h"
#include "ShellConfigurator.h"
#include "ShellDefs.h"

using namespace Urho3D;

HeadlessApplication::HeadlessApplication(Urho3D::Context* context, Urho3D::VariantMap&& shellParameters)
	: ShellApplication(context, std::move(shellParameters))
	, server_(context)
{
}

void HeadlessApplication::Start()
{
	// TODO: Default values
	ShellApplication::Start();
	auto it = shellParameters_.Find(SP_SCENE);
	if (it != shellParameters_.End())
	{
		const String& sceneName = it->second_.GetString();
		it = shellParameters_.Find(SP_SERVER);
		const String& serverName = it != shellParameters_.End()
									   ? it->second_.GetString()
									   : GetSubsystem<ShellConfigurator>()->GetGameName() + " Server";
		const unsigned short port = GetSubsystem<ShellConfigurator>()->GetPort();
		server_.LoadScene(sceneName);
		SubscribeToEvent(E_ASYNCLOADPROGRESS,
						 [](StringHash, VariantMap& eventData)
						 {
							 using namespace AsyncLoadProgress;
							 URHO3D_LOGINFOF("Loading u%%%...",
											 static_cast<unsigned char>(eventData[P_PROGRESS].GetFloat() * 100.0f));
						 });
		SubscribeToEvent(E_ASYNCLOADFINISHED,
						 [this, port, serverName = std::move(serverName)](StringHash, VariantMap&)
						 {
							 server_.Start(port);
							 server_.MakeVisible(serverName);
						 });
	}
}
