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

#ifndef SAMPLEPLUGINCLIENT_H
#define SAMPLEPLUGINCLIENT_H

#include "Plugin/PluginInterfaceClient.h"

class SamplePluginClient : public PluginInterfaceClient
{
	URHO3D_OBJECT(SamplePluginClient, PluginInterfaceClient)

public:
	using PluginInterfaceClient::PluginInterfaceClient;

	void Setup();
	void Spawn(Urho3D::Scene* scene, unsigned nodeId) override;

	const Urho3D::String& GetName() const override;

private:
	static const Urho3D::String PLUGIN_NAME;
};

#endif // SAMPLEPLUGINCLIENT_H
