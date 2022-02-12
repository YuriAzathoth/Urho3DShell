//
// Copyright (c) 2021-2022 Yuriy Zinchenko.
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

#include <Urho3D/Graphics/Camera.h>
#include <Urho3D/Graphics/Renderer.h>
#include <Urho3D/Graphics/Viewport.h>
#include <Urho3D/IO/Log.h>
#include <Urho3D/Input/InputConstants.h>
#include <Urho3D/Scene/Scene.h>
#include "Core/ActionsDefs.h"
#include "Input/ControllersRegistry.h"
#include "Plugin/BinaryPluginUtils.h"
#include "SamplePluginClient.h"

using namespace Urho3D;

const String SamplePluginClient::PLUGIN_NAME = "Sample game";

void SamplePluginClient::Setup()
{
	ControllersRegistry* controllers = GetSubsystem<ControllersRegistry>();
	InputController* keyboard = controllers->Get("KeyboardController");
	if (keyboard)
	{
		keyboard->SetDefaultBinding(MOVE_FORWARD, KEY_W);
		keyboard->SetDefaultBinding(MOVE_BACK, KEY_S);
		keyboard->SetDefaultBinding(MOVE_LEFT, KEY_A);
		keyboard->SetDefaultBinding(MOVE_RIGHT, KEY_D);
		keyboard->SetDefaultBinding(MOVE_UP, KEY_SPACE);
		keyboard->SetDefaultBinding(MOVE_DOWN, KEY_CTRL);
		keyboard->SetDefaultBinding(WALK, KEY_SHIFT);
	}
}

void SamplePluginClient::Spawn(Urho3D::Scene* scene, unsigned nodeId)
{
	URHO3D_LOGTRACE("SamplePluginClient::Spawn");

	Node* node = scene->GetNode(nodeId);
	Camera* camera = node->CreateComponent<Camera>();
	camera->SetTemporary(true);
	SharedPtr<Viewport> viewport = MakeShared<Viewport>(context_, scene, camera);
	Renderer* renderer = GetSubsystem<Renderer>();
	renderer->SetViewport(0, viewport);
}

const Urho3D::String& SamplePluginClient::GetName() const { return PLUGIN_NAME; }

URHO3DSHELL_PLUGIN(SamplePluginClient)
