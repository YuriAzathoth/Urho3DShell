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

#include <Urho3D/IO/FileSystem.h>
#include <Urho3D/IO/PackageFile.h>
#include <Urho3D/Resource/ResourceCache.h>
#include "NewGameDialog.h"

#define SCENES_PATH "Scenes/"

using namespace Urho3D;

NewGameDialog::NewGameDialog(Urho3D::Context* context)
	: HostItemsListWindow(context)
{
	SetTitle("NewGame");

	StringVector row(1);
	row[0] = "Map";
	SetCaptions(row);

	StringVector resources;
	for (const SharedPtr<PackageFile>& package : GetSubsystem<ResourceCache>()->GetPackageFiles())
	{
		resources = package->GetEntryNames();
		for (const String& filename : resources)
			if (filename.StartsWith(SCENES_PATH) &&
				(filename.EndsWith(".xml") || filename.EndsWith(".bin") || filename.EndsWith(".json")))
			{
				row[0] = filename.Replaced(SCENES_PATH, "");
				AddItem(filename, row);
			}
	}

	StringVector files;
	GetSubsystem<FileSystem>()->ScanDir(files, "Scenes/", "*.xml|*.bin|*.json", SCAN_FILES, true);
	for (const String& filename : files)
	{
		row[0] = filename.Replaced(SCENES_PATH, "");
		AddItem(filename, row);
	}
}
