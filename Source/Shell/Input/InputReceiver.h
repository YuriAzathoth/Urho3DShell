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

#ifndef INPUTRECEIVER_H
#define INPUTRECEIVER_H

#include <Urho3D/Input/Controls.h>
#include <Urho3D/Scene/Component.h>
#include "Urho3DShellAPI.h"

namespace Urho3D
{
class Connection;
}

class URHO3DSHELLAPI_EXPORT InputReceiver : public Urho3D::Component
{
	URHO3D_OBJECT(InputReceiver, Urho3D::Component)

public:
	explicit InputReceiver(Urho3D::Context* context);

	void SetConnection(const Urho3D::Connection* connection);

	const Urho3D::Controls& GetControls() const noexcept { return current_; }

private:
	void OnPhysicsPreStep(Urho3D::StringHash, Urho3D::VariantMap&);

	Urho3D::Controls current_;
	Urho3D::Controls previous_;
	const Urho3D::Connection* connection_;

public:
	static void RegisterObject(Urho3D::Context* context);
};

#endif // INPUTRECEIVER_H
