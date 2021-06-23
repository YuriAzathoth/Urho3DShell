#ifndef PLUGININTERFACE_H
#define PLUGININTERFACE_H

#include <Urho3D/Core/Object.h>
#include "Urho3DShellAPI.h"

class URHO3DSHELLAPI_EXPORT PluginInterface : public Urho3D::Object
{
	URHO3D_OBJECT(PluginInterface, Urho3D::Object)

public:
	using Urho3D::Object::Object;
	virtual ~PluginInterface() {}

	virtual const Urho3D::String& GetName() const = 0;
};

#endif // PLUGININTERFACE_H
