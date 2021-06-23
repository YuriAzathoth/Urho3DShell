#ifndef BINARYPLUGIN_H
#define BINARYPLUGIN_H

#define BOOST_DLL_FORCE_ALIAS_INSTANTIATION
#include <boost/config.hpp>
#include <boost/dll/alias.hpp>

#define PLUGIN_DECLARE() static Urho3D::UniquePtr<PluginInterface> Create(Urho3D::Context* context);
#define PLUGIN_REGISTER(CLASS)                                                                                         \
	Urho3D::UniquePtr<PluginInterface> CLASS::Create(Urho3D::Context* context)                                         \
	{                                                                                                                  \
		return UniquePtr<PluginInterface>(new CLASS(context));                                                         \
	}                                                                                                                  \
	BOOST_DLL_ALIAS(CLASS::Create, CreatePlugin)

#endif // BINARYPLUGIN_H
