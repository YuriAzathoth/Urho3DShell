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

#ifndef PLUGINSREGISTRY_H
#define PLUGINSREGISTRY_H

#include <Urho3D/Core/Object.h>
#include "Plugin.h"
#include "U3SCoreAPI.h"

#define REGISTER_PLUGIN_TYPE(CLASS, EXTENSION)

class U3SCOREAPI_EXPORT PluginsRegistry : public Urho3D::Object
{
	URHO3D_OBJECT(PluginsRegistry, Urho3D::Object)

public:
	using Urho3D::Object::Object;

	bool Initialize(const Urho3D::String& pluginName);
	bool LoadPlugin(const Urho3D::String& pluginName);
	Urho3D::StringVector ScanPlugins() const;

	bool Load(const Urho3D::String& fileName);
	void Close(Urho3D::StringHash plugin);
	Urho3D::StringVector GetAllNames() const;
	void CloseAll();

	void RegisterPluginFactory(Urho3D::SharedPtr<PluginFactory> factory);
	void RemovePluginFactory(Urho3D::StringHash extension);

	template <typename T> void RegisterPluginFactory();

private:
	bool
	FindPlugin(Urho3D::StringVector& paths, const Urho3D::String& scanPath, const Urho3D::String& pluginName) const;
	bool FindPlugin(Urho3D::StringVector& paths, const Urho3D::String& filePathWithoutExt) const;

	Urho3D::HashMap<Urho3D::StringHash, Urho3D::SharedPtr<Plugin>> plugins_;
	Urho3D::HashMap<Urho3D::StringHash, Urho3D::SharedPtr<PluginFactory>> factories_;
	Urho3D::SharedPtr<Plugin> mainPlugin_;
};

template <typename T> void PluginsRegistry::RegisterPluginFactory()
{
	RegisterPluginFactory(Urho3D::MakeShared<PluginFactoryImpl<T>>(T::GetExtension()));
}

#endif // PLUGINSREGISTRY_H
