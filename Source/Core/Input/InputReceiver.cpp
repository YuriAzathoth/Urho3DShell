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

#include <Urho3D/Core/Context.h>
#include <Urho3D/Input/Controls.h>
#include <Urho3D/Network/Connection.h>
#include <Urho3D/Physics/PhysicsEvents.h>
#include "InputReceiver.h"

using namespace Urho3D;

InputReceiver::InputReceiver(Urho3D::Context* context)
	: Component(context)
	, previous_(0)
	, connection_(nullptr)
{
}

void InputReceiver::SetConnection(const Urho3D::Connection* connection)
{
	connection_ = connection;
	SubscribeToEvent(E_PHYSICSPRESTEP, URHO3D_HANDLER(InputReceiver, OnPhysicsPreStep));
}

void InputReceiver::OnPhysicsPreStep(Urho3D::StringHash, Urho3D::VariantMap&)
{
	previous_ = current_.buttons_;
	current_ = connection_->GetControls();
}

void InputReceiver::RegisterObject(Urho3D::Context* context)
{
	context->RegisterFactory<InputReceiver>("Input");
	URHO3D_ATTRIBUTE("Controls", unsigned, current_.buttons_, 0, AM_NOEDIT | AM_LATESTDATA);
	URHO3D_ATTRIBUTE("Pitch", float, current_.pitch_, 0, AM_NOEDIT | AM_LATESTDATA);
	URHO3D_ATTRIBUTE("Yaw", float, current_.yaw_, 0, AM_NOEDIT | AM_LATESTDATA);
}
