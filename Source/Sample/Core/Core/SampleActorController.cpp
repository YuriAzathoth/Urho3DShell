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
#include <Urho3D/Scene/Component.h>
#include <Urho3D/Scene/Node.h>
#include <Urho3D/Scene/SmoothedTransform.h>
#include "ActionsDefs.h"
#include "Input/ActionsRegistry.h"
#include "Input/InputReceiver.h"
#include "SampleActorController.h"

#define WALK_SPEED 5.0f
#define MOVE_SPEED 20.0f
#define LOOK_SPEED 1.0f
#define PITCH_MIN -80.0f
#define PITCH_MAX 80.0f

using namespace Urho3D;

SampleActorController::SampleActorController(Urho3D::Context* context)
	: LogicComponent(context)
{
	SetUpdateEventMask(USE_UPDATE | USE_FIXEDUPDATE);
}

void SampleActorController::Update(float)
{	
	const InputReceiver* input = GetComponent<InputReceiver>();
	if (input->IsServerSide()) // Server-side only code
	{
		const float pitch = Clamp(input->GetPitch(), PITCH_MIN, PITCH_MAX);
		const float yaw = input->GetYaw();
		const Quaternion rotation(pitch, yaw, 0.0f);
		node_->SetRotation(rotation);
	}
}

void SampleActorController::FixedUpdate(float timeStep)
{
	const InputReceiver* input = GetComponent<InputReceiver>();
	if (input->IsServerSide()) // Server-side only code
	{
		const ActionsRegistry* actions = GetSubsystem<ActionsRegistry>();
		Vector3 translation;
		translation.x_ = static_cast<float>(input->IsDown(actions->GetFlag(MOVE_RIGHT)) -
											input->IsDown(actions->GetFlag(MOVE_LEFT)));
		translation.y_ =
			static_cast<float>(input->IsDown(actions->GetFlag(MOVE_UP)) - input->IsDown(actions->GetFlag(MOVE_DOWN)));
		translation.z_ = static_cast<float>(input->IsDown(actions->GetFlag(MOVE_FORWARD)) -
											input->IsDown(actions->GetFlag(MOVE_BACK)));
		translation.Normalize();

		const bool walk = input->IsDown(actions->GetFlag(WALK));
		translation *= timeStep;
		translation *= walk ? WALK_SPEED : MOVE_SPEED;
		node_->Translate(translation);
	}
}

void SampleActorController::RegisterObject(Urho3D::Context* context)
{
	context->RegisterFactory<SampleActorController>("Actor");
}
