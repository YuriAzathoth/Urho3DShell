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

#ifndef ACTIONSREGISTRY_H
#define ACTIONSREGISTRY_H

#include <Urho3D/Core/Object.h>
#include "U3SCoreAPI.h"

class U3SCOREAPI_EXPORT ActionsRegistry : public Urho3D::Object
{
	URHO3D_OBJECT(ActionsRegistry, Urho3D::Object)

public:
	using ActionsVector = Urho3D::PODVector<Urho3D::StringHash>;

	explicit ActionsRegistry(Urho3D::Context* context);

	void RegisterLocal(const Urho3D::String& actionName);
	unsigned RegisterRemote(const Urho3D::String& actionName);
	void Remove(Urho3D::StringHash action);
	void RemoveAll();

	unsigned GetFlag(Urho3D::StringHash action) const;
	const Urho3D::String& GetName(Urho3D::StringHash action) const;
	bool IsRemote(Urho3D::StringHash action) const { return remoteFlags_.Contains(action); }
	const ActionsVector& GetAll() const noexcept { return ordered_; }

	Urho3D::String GetDebugString() const;

private:
	ActionsVector ordered_;
	Urho3D::HashMap<Urho3D::StringHash, unsigned> remoteFlags_;
	Urho3D::StringMap names_;
	unsigned lastRemoteFlag_;
};

#endif // ACTIONSREGISTRY_H
