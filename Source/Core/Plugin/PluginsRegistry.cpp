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

#include <Urho3D/IO/FileSystem.h>
#include <Urho3D/IO/Log.h>
#include "BinaryPlugin.h"
#include "Core/ShellConfigurator.h"
#include "PluginsRegistry.h"

#ifdef URHO3D_ANGELSCRIPT
#include "ScriptPlugin.h"
#endif // URHO3D_ANGELSCRIPT

using namespace Urho3D;

// TODO: Refactoring
// TODO: Plugins in "plugins" path
Urho3D::StringVector PluginsRegistry::FindPlugins(const Urho3D::String& pluginName) const
{
	const bool client = GetSubsystem("FrontShell") != NULL;

	FileSystem* fileSystem = GetSubsystem<FileSystem>();
	StringVector ret;

#ifdef NDEBUG
	String path = fileSystem->GetCurrentDir();
	String fileName = path + pluginName + BinaryPlugin::GetExtension();
	if (fileSystem->FileExists(fileName))
		ret.Push(fileName);
	fileName = path + pluginName + "Core" + BinaryPlugin::GetExtension();
	if (fileSystem->FileExists(fileName))
		ret.Push(fileName);
	if (client)
	{
		fileName = path + pluginName + "Client" + BinaryPlugin::GetExtension();
		if (fileSystem->FileExists(fileName))
			ret.Push(fileName);
	}
#else
	String path = fileSystem->GetCurrentDir();
	String fileName = path + pluginName + "_d" + BinaryPlugin::GetExtension();
	if (fileSystem->FileExists(fileName))
		ret.Push(fileName);
	else
	{
		fileName = path + pluginName + BinaryPlugin::GetExtension();
		if (fileSystem->FileExists(fileName))
			ret.Push(fileName);
	}
	fileName = path + pluginName + "Core" + BinaryPlugin::GetExtension();
	if (fileSystem->FileExists(fileName))
		ret.Push(fileName);
	else
	{
		fileName = path + pluginName + "Core_d" + BinaryPlugin::GetExtension();
		if (fileSystem->FileExists(fileName))
			ret.Push(fileName);
	}
	if (client)
	{
		fileName = path + pluginName + "Client" + BinaryPlugin::GetExtension();
		if (fileSystem->FileExists(fileName))
			ret.Push(fileName);
		else
		{
			fileName = path + pluginName + "Client_d" + BinaryPlugin::GetExtension();
			if (fileSystem->FileExists(fileName))
				ret.Push(fileName);
		}
	}
#endif // NDEBUG

	return ret;
}

bool PluginsRegistry::Load(const Urho3D::String& fileName)
{
	SharedPtr<Plugin> plugin;
	if (fileName.EndsWith(ScriptPlugin::GetExtension(), false))
#ifdef URHO3D_ANGELSCRIPT
		plugin.StaticCast(MakeShared<ScriptPlugin>(context_));
#else
		URHO3D_LOGERROR("Loading AngelScript scripts is not supported.");
#endif // URHO3D_ANGELSCRIPT
	else if (fileName.EndsWith(BinaryPlugin::GetExtension(), false))
		plugin.StaticCast(MakeShared<BinaryPlugin>(context_));
	else
		URHO3D_LOGERRORF("Unsupported plugin extension: %s.", fileName.Substring(fileName.FindLast('.')).CString());

	const String pluginsPath = GetSubsystem<ShellConfigurator>()->GetPluginsPath();
	if (plugin.NotNull() && plugin->Load(fileName))
	{
		plugins_[fileName] = plugin;
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
