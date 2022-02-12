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

#ifndef PLUGININTERFACECLIENT_H
#define PLUGININTERFACECLIENT_H

#include "Plugin/PluginInterface.h"
#include "U3SClientAPI.h"

namespace Urho3D
{
class Scene;
}

class U3SCLIENTAPI_EXPORT PluginInterfaceClient : public PluginInterface
{
	URHO3D_OBJECT(PluginInterfaceClient, PluginInterface)

public:
	explicit PluginInterfaceClient(Urho3D::Context* context);

	virtual void Setup() {}
	virtual void Start() {}
	virtual void Stop() {}
	virtual void Spawn(Urho3D::Scene* scene, unsigned nodeId) {}

private:
	void OnServerSideSpawned(Urho3D::StringHash, Urho3D::VariantMap& eventData);
};

#endif // PLUGININTERFACECLIENT_H
