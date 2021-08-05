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

#include "BinaryPlugin.h"
#include "PluginsRegistry.h"
#include "ScriptPlugin.h"

using namespace Urho3D;

bool PluginsRegistry::RegisterPlugin(Urho3D::String pluginName)
{
	SharedPtr<Plugin> plugin;
	if (pluginName.EndsWith(".as", false))
		plugin.StaticCast(MakeShared<ScriptPlugin>(context_));
	else
		plugin.StaticCast(MakeShared<BinaryPlugin>(context_));

	if (plugin.Null())
		return false;

	if (!plugin->Load(pluginName))
		if (!plugin->Load(pluginsPath_ + "/" + pluginName))
			return false;

	plugins_[pluginName] = plugin;
	return true;
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

void PluginsRegistry::RemoveAllPlugins() { plugins_.Clear(); }
