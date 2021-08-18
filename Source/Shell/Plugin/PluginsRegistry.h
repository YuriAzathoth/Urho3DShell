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

#ifndef PLUGINSREGISTRY_H
#define PLUGINSREGISTRY_H

#include <Urho3D/Core/Object.h>
#include "Plugin.h"
#include "Urho3DShellAPI.h"

class URHO3DSHELLAPI_EXPORT PluginsRegistry : public Urho3D::Object
{
	URHO3D_OBJECT(PluginsRegistry, Urho3D::Object)

public:
	explicit PluginsRegistry(Urho3D::Context* context);

	bool RegisterPlugin(Urho3D::String pluginName);
	void RemovePlugin(Urho3D::StringHash plugin);
	Urho3D::StringVector GetPlugins() const;
	void RemoveAllPlugins();

private:
	Urho3D::HashMap<Urho3D::StringHash, Urho3D::SharedPtr<Plugin>> plugins_;
	Urho3D::SharedPtr<Plugin> mainPlugin_;
};

#endif // PLUGINSREGISTRY_H
