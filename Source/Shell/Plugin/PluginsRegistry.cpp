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

#include <Urho3D/IO/Log.h>
#include "BinaryPlugin.h"
#include "Core/ShellConfigurator.h"
#include "PluginsRegistry.h"
#include "ScriptPlugin.h"
#include "Urho3DShellConfig.h"

#if defined(URHO3DSHELL_EXPERIMENTAL) && defined(URHO3D_LUA)
#include "LuaScriptPlugin.h"
#endif // defined(URHO3DSHELL_EXPERIMENTAL) && defined(URHO3D_LUA)

using namespace Urho3D;

void PluginsRegistry::Initialize(const Urho3D::String& coreLibraryName)
{
	mainPlugin_.StaticCast(MakeShared<BinaryPlugin>(context_));
	mainPlugin_->Load(coreLibraryName);
}

bool PluginsRegistry::Load(const Urho3D::String& pluginName)
{
	SharedPtr<Plugin> plugin;
	if (pluginName.EndsWith(".as", false))
#ifdef URHO3D_ANGELSCRIPT
		plugin.StaticCast(MakeShared<ScriptPlugin>(context_));
#endif // URHO3D_ANGELSCRIPT
	else if (pluginName.EndsWith(".lua", false))
#if defined(URHO3DSHELL_EXPERIMENTAL) && defined(URHO3D_LUA)
		plugin.StaticCast(MakeShared<LuaScriptPlugin>(context_));
#else
			URHO3D_LOGERROR("Loading Lua scripts is only supported in experimental mode.");
#endif // defined(URHO3DSHELL_EXPERIMENTAL) && defined(URHO3D_LUA)
	else
		plugin.StaticCast(MakeShared<BinaryPlugin>(context_));

	const String pluginsPath = GetSubsystem<ShellConfigurator>()->GetPluginsPath();
	if (plugin.NotNull() && plugin->Load(/*pluginsPath + */ pluginName))
	{
		plugins_[pluginName] = plugin;
		return true;
	}
	return false;
}

void PluginsRegistry::Close(Urho3D::StringHash plugin) { plugins_.Erase(plugin); }

Urho3D::StringVector PluginsRegistry::GetAllNames() const
{
	StringVector ret;
	ret.Reserve(plugins_.Size());
	for (const auto& pluginPair : plugins_)
		ret.Push(pluginPair.second_->GetName());
	return ret;
}

void PluginsRegistry::CloseAll() { plugins_.Clear(); }
