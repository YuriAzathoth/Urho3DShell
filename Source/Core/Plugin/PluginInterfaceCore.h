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

#ifndef PLUGININTERFACECORE_H
#define PLUGININTERFACECORE_H

#include "PluginInterface.h"
#include "U3SCoreAPI.h"

namespace Urho3D
{
class Connection;
}

class U3SCOREAPI_EXPORT PluginInterfaceCore : public PluginInterface
{
	URHO3D_OBJECT(PluginInterfaceCore, PluginInterface)

public:
	explicit PluginInterfaceCore(Urho3D::Context* context);

	virtual void Setup() {}
	virtual void Start() {}
	virtual void Stop() {}
	virtual unsigned Spawn(Urho3D::Connection*) { return 0; }

	const Urho3D::String& GetName() const override = 0;

private:
	void OnClientSceneLoaded(Urho3D::StringHash, Urho3D::VariantMap& eventData);
	void OnRemoteClientStarted(Urho3D::StringHash, Urho3D::VariantMap&);
	void OnRemoteClientStopped(Urho3D::StringHash, Urho3D::VariantMap&);

	Urho3D::PODVector<Urho3D::StringHash> factories_;
};

#endif // PLUGININTERFACECORE_H
