#ifndef BINARYPLUGIN_H
#define BINARYPLUGIN_H

#include <boost/dll/shared_library.hpp>
#include "Plugin.h"

class BinaryPlugin : public Plugin
{
	URHO3D_OBJECT(BinaryPlugin, Plugin)

public:
	using Plugin::Plugin;

	bool Load(const Urho3D::String& fileName) override;

	const Urho3D::String& GetName() const override;

private:
	boost::dll::shared_library plugin_;
	Urho3D::UniquePtr<PluginInterface> interface_;
};

#endif // BINARYPLUGIN_H
