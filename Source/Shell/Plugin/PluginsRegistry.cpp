#include "BinaryPlugin.h"
#include "PluginsRegistry.h"
#include "ScriptPlugin.h"

using namespace Urho3D;

bool PluginsRegistry::RegisterPlugin(const Urho3D::String& pluginName)
{
	const String fileName = pluginsPath_ + "/" + pluginName;

	SharedPtr<Plugin> plugin;
	if (fileName.EndsWith(".as", false))
		plugin.StaticCast(MakeShared<ScriptPlugin>(context_));
	else
		plugin.StaticCast(MakeShared<BinaryPlugin>(context_));

	if (plugin.NotNull() && plugin->Load(fileName))
	{
		plugins_[pluginName] = plugin;
		return true;
	}
	else
		return false;
}

void PluginsRegistry::RemovePlugin(Urho3D::StringHash plugin) { plugins_.Erase(plugin); }

Urho3D::StringVector PluginsRegistry::GetPlugins() const
{
	StringVector ret;
	ret.Reserve(plugins_.Size());
	for (const auto& pluginPair : plugins_)
		ret.Push(pluginPair.second_->GetName());
	return ret;
}

void PluginsRegistry::ClearAllPlugins() { plugins_.Clear(); }
