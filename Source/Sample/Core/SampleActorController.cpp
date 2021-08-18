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
#include <Urho3D/Input/Controls.h>
#include <Urho3D/Scene/Node.h>
#include <Urho3D/Scene/SmoothedTransform.h>
#include "ActionsDefs.h"
#include "Input/ActionsRegistry.h"
#include "Input/InputReceiver.h"
#include "SampleActorController.h"

#define MOVE_SPEED 20.0f
#define LOOK_SPEED 1.0f
#define PITCH_MIN -80.0f
#define PITCH_MAX 80.0f

using namespace Urho3D;

SampleActorController::SampleActorController(Urho3D::Context* context)
	: LogicComponent(context)
{
	SetUpdateEventMask(USE_FIXEDUPDATE);
}

void SampleActorController::FixedUpdate(float timeStep)
{
	const ActionsRegistry* actions = GetSubsystem<ActionsRegistry>();
	const InputReceiver* receiver = GetComponent<InputReceiver>();
	const Controls& controls = receiver->GetControls();
	Vector3 translation;
	translation.x_ = static_cast<float>(controls.IsDown(actions->GetActionFlag(MOVE_RIGHT)) -
										controls.IsDown(actions->GetActionFlag(MOVE_LEFT)));
	translation.z_ = static_cast<float>(controls.IsDown(actions->GetActionFlag(MOVE_FORWARD)) -
										controls.IsDown(actions->GetActionFlag(MOVE_BACK)));
	translation *= timeStep * MOVE_SPEED;

	const float pitch = Clamp(node_->GetRotation().PitchAngle() + controls.pitch_, PITCH_MIN, PITCH_MAX);
	const float yaw = node_->GetRotation().YawAngle() + controls.yaw_;
	const Quaternion rotation(pitch, yaw, 0.0f);
	const Vector3 position = node_->GetPosition() + rotation * translation;

	node_->SetPosition(position);
	node_->SetRotation(rotation);
}

void SampleActorController::RegisterObject(Urho3D::Context* context)
{
	context->RegisterFactory<SampleActorController>("Actor");
}
