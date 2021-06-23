#ifndef PLUGIN_H
#define PLUGIN_H

#include "PluginInterface.h"

class Plugin : public PluginInterface
{
	URHO3D_OBJECT(Plugin, PluginInterface)

public:
	using PluginInterface::PluginInterface;
	virtual ~Plugin() {}
	virtual bool Load(const Urho3D::String& fileName) = 0;
};

#endif // PLUGIN_H
