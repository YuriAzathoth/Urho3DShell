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

#ifndef INPUTREGISTRY_H
#define INPUTREGISTRY_H

#include <Urho3D/Core/Object.h>
#include "Urho3DShellAPI.h"

class URHO3DSHELLAPI_EXPORT InputRegistry : public Urho3D::Object
{
	URHO3D_OBJECT(InputRegistry, Urho3D::Object)

public:
	explicit InputRegistry(Urho3D::Context* context);

	void RegisterActionLocal(const Urho3D::String& actionName);
	unsigned RegisterActionRemote(const Urho3D::String& actionName);
	void RemoveAction(Urho3D::StringHash action);
	void RemoveAllActions();

	unsigned GetActionFlag(Urho3D::StringHash action) const;
	const Urho3D::String& GetActionName(Urho3D::StringHash action) const;
	Urho3D::StringVector GetActions() const;

	Urho3D::String GetDebugString() const;

private:
	Urho3D::PODVector<Urho3D::StringHash> ordered_;
	Urho3D::HashMap<Urho3D::StringHash, unsigned> remoteFlags_;
	Urho3D::StringMap names_;
	unsigned lastRemoteFlag_;
};

#endif // INPUTREGISTRY_H
