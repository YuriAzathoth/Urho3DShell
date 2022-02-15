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
#include <Urho3D/IO/PackageFile.h>
#include <Urho3D/Resource/ResourceCache.h>
#include "NewGameDialog.h"

#define SCENES_PATH "Scenes/"

using namespace Urho3D;

struct SceneItem
{
	String name_;
	String path_;
	SceneItem(const String& name, const String& path)
		: name_(name)
		, path_(path)
	{
	}
	bool operator<(const SceneItem& rhs) { return name_ < rhs.name_; }
};

NewGameDialog::NewGameDialog(Urho3D::Context* context)
	: HostItemsListWindow(context)
{
	SetTitle("NewGame");

	StringVector row(1);
	row[0] = "Map";
	SetCaptions(row);

	ResourceCache* cache = GetSubsystem<ResourceCache>();
	Vector<SceneItem> scenes;
	StringVector names;
	for (const SharedPtr<PackageFile>& package : cache->GetPackageFiles())
	{
		names = package->GetEntryNames();
		for (const String& filename : names)
			if (filename.StartsWith(SCENES_PATH) &&
				(filename.EndsWith(".xml") || filename.EndsWith(".bin") || filename.EndsWith(".json")))
				scenes.EmplaceBack(filename.Substring(filename.FindLast('/')), filename);
	}
	names.Clear();

	FileSystem* fileSystem = GetSubsystem<FileSystem>();
	String path;
	for (const String& dir : cache->GetResourceDirs())
	{
		path = dir + "/Scenes/";
		fileSystem->ScanDir(names, path, "*.*", SCAN_FILES, true);
		for (const String& filename : names)
			if (filename.EndsWith(".xml", false) || filename.EndsWith(".json", false) ||
				filename.EndsWith(".bin", false))
				scenes.EmplaceBack(filename, path + filename);
	}
	Sort(scenes.Begin(), scenes.End());
	for (const auto& item : scenes)
	{
		row[0] = item.name_;
		AddItem(item.path_, row);
	}
}
