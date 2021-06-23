#ifndef PLUGINSREGISTRY_H
#define PLUGINSREGISTRY_H

#include <Urho3D/Core/Object.h>
#include "Plugin.h"
#include "Urho3DShellAPI.h"

class URHO3DSHELLAPI_EXPORT PluginsRegistry : public Urho3D::Object
{
	URHO3D_OBJECT(PluginsRegistry, Urho3D::Object)

public:
	using Urho3D::Object::Object;

	bool RegisterPlugin(const Urho3D::String& pluginName);
	void RemovePlugin(Urho3D::StringHash plugin);
	Urho3D::StringVector GetPlugins() const;
	void ClearAllPlugins();

	void SetPluginsPath(const Urho3D::String& pluginsPath) { pluginsPath_ = pluginsPath; }
	const Urho3D::String& GetPluginsPath() const noexcept { return pluginsPath_; }

private:
	Urho3D::HashMap<Urho3D::StringHash, Urho3D::SharedPtr<Plugin>> plugins_;
	Urho3D::String pluginsPath_;
};

#endif // PLUGINSREGISTRY_H
