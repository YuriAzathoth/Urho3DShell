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

#ifndef PLUGININTERFACE_H
#define PLUGININTERFACE_H

#include <Urho3D/Core/Object.h>
#include "Urho3DShellAPI.h"

#define REGISTER_OBJECT(CLASS)                                                                                         \
	CLASS::RegisterObject(context_);                                                                                   \
	RegisterObject(CLASS::GetTypeInfoStatic()->GetType())

class URHO3DSHELLAPI_EXPORT PluginInterface : public Urho3D::Object
{
	URHO3D_OBJECT(PluginInterface, Urho3D::Object)

public:
	using Urho3D::Object::Object;
	virtual ~PluginInterface();

	virtual const Urho3D::String& GetName() const = 0;

	void RegisterObject(Urho3D::StringHash objectType);

private:
	Urho3D::PODVector<Urho3D::StringHash> factories_;
};

#endif // PLUGININTERFACE_H
