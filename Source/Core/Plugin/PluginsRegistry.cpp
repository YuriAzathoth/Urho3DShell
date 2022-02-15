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

#include <Urho3D/Engine/Engine.h>
#include <Urho3D/IO/FileSystem.h>
#include <Urho3D/IO/Log.h>
#include "Core/ShellConfigurator.h"
#include "PluginsRegistry.h"

static constexpr char CLIENT_PREFIX[] = "Client";
static constexpr char CORE_PREFIX[] = "Core";

using namespace Urho3D;

bool PluginsRegistry::Initialize(const Urho3D::String& pluginName)
{
	StringVector paths;
	const bool found = FindPlugin(paths, GetSubsystem<FileSystem>()->GetProgramDir(), pluginName);
	if (found)
	{
		for (const String& path : paths)
			if (!Load(path))
			{
				URHO3D_LOGERRORF("Could not load game library: %s.", pluginName.CString());
				GetSubsystem<Engine>()->Exit();
				return false;
			}
		return true;
	}
	else
	{
		URHO3D_LOGERRORF("Could not find game library: %s.", pluginName.CString());
		GetSubsystem<Engine>()->Exit();
		return false;
	}
}

bool PluginsRegistry::LoadPlugin(const Urho3D::String& pluginName)
{
	StringVector paths;
	const String pluginPath = GetSubsystem<ShellConfigurator>()->GetPluginsPath() + pluginName + "/";
	unsigned notLoaded;
	if (FindPlugin(paths, pluginPath, pluginPath))
	{
		notLoaded = paths.Size();
		for (const String& path : paths)
			if (Load(path))
				--notLoaded;
			else
				URHO3D_LOGWARNINGF("Failed to load plugin '%s'.", pluginName.CString());
		if (notLoaded)
			for (const String& path : paths)
				Close(path);
		return notLoaded == 0;
	}
	else
	{
		URHO3D_LOGWARNINGF("Failed to find plugin '%s'.", pluginName.CString());
		return false;
	}
}

Urho3D::StringVector PluginsRegistry::ScanPlugins() const
{
	FileSystem* fileSystem = GetSubsystem<FileSystem>();
	StringVector paths, ret;
	const String pluginsPath = GetSubsystem<ShellConfigurator>()->GetPluginsPath();
	fileSystem->ScanDir(paths, pluginsPath, "*", SCAN_DIRS, false);
	for (const String& path : paths)
		FindPlugin(paths, pluginsPath + path, path);
	return ret;
}

bool PluginsRegistry::Load(const Urho3D::String& fileName)
{
	const unsigned dotPos = fileName.FindLast('.');
	if (dotPos == String::NPOS)
	{
		URHO3D_LOGERROR("Failed to determine plugin filename extension.");
		return false;
	}
	const String fileExt = fileName.Substring(dotPos);
	const auto it = factories_.Find(fileExt);
	if (it != factories_.End())
	{
		SharedPtr<Plugin> plugin = it->second_->CreatePlugin(context_);
		if (plugin->Load(fileName))
		{
			plugins_[fileName] = plugin;
			return true;
		}
	}
	else if (dotPos == String::NPOS)
		URHO3D_LOGERRORF("Unsupported plugin type '%s'.", fileExt.CString());
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

void PluginsRegistry::RegisterPluginFactory(Urho3D::SharedPtr<PluginFactory> factory)
{
	factories_[factory->GetExtension()] = factory;
}

void PluginsRegistry::RemovePluginFactory(Urho3D::StringHash extension) { factories_.Erase(extension); }

bool PluginsRegistry::FindPlugin(Urho3D::StringVector& paths,
								 const Urho3D::String& scanPath,
								 const Urho3D::String& pluginName) const
{
#ifndef NDEBUG
	constexpr bool debug = true;
#else
	constexpr bool debug = false;
#endif // NDEBUG

	bool found = false;

	String filePrefix = scanPath + pluginName;
	String fileName;
	if (GetSubsystem<ShellConfigurator>()->IsClient())
	{
		fileName = filePrefix + CLIENT_PREFIX;
		if (!debug || (debug && !FindPlugin(paths, fileName + "_d")))
		{
			if (FindPlugin(paths, fileName))
				found = true;
		}
		else
			found = true;
	}
	fileName = filePrefix + CORE_PREFIX;
	if (!debug || (debug && !FindPlugin(paths, fileName + "_d")))
	{
		if (FindPlugin(paths, fileName))
			found = true;
	}
	else
		found = true;

	if (!found)
	{
		if (!debug || (debug && !FindPlugin(paths, filePrefix + "_d")))
		{
			if (FindPlugin(paths, filePrefix))
				found = true;
		}
		else
			found = true;
	}

	return found;
}

inline bool PluginsRegistry::FindPlugin(Urho3D::StringVector& paths, const Urho3D::String& filePathWithoutExt) const
{
	FileSystem* fileSystem = GetSubsystem<FileSystem>();
	String fileName;
	bool found = false;
	for (const auto& factoryPair : factories_)
	{
		fileName = filePathWithoutExt + factoryPair.second_->GetExtension();
		if (fileSystem->FileExists(fileName))
		{
			paths.Push(fileName);
			found = true;
		}
	}
	return found;
}
