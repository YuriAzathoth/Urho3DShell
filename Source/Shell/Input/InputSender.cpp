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

#include <Urho3D/Core/Context.h>
#include <Urho3D/Network/Network.h>
#include <Urho3D/Scene/Node.h>
#include "InputController.h"
#include "InputSender.h"

using namespace Urho3D;

InputSender::InputSender(Urho3D::Context* context)
	: LogicComponent(context)
	, controller_(nullptr)
{
	SetUpdateEventMask(USE_UPDATE | USE_FIXEDUPDATE);
}

void InputSender::Update(float) { controller_->ReadControls(controls_); }

void InputSender::FixedUpdate(float)
{
	Connection* connection = GetSubsystem<Network>()->GetServerConnection();
	connection->SetControls(controls_);
	connection->SetPosition(node_->GetPosition());
	connection->SetRotation(node_->GetRotation());
	controls_.Reset();
}

void InputSender::SetController(const InputController* controller) { controller_ = controller; }

void InputSender::RegisterObject(Urho3D::Context* context) { context->RegisterFactory<InputSender>("Input"); }