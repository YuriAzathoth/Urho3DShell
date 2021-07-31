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

#ifndef StartGameWindow_H
#define StartGameWindow_H

#include "Widget.h"

namespace Urho3D
{
class LineEdit;
class ListView;
}

class StartGameWindow : public Widget
{
	URHO3D_OBJECT(StartGameWindow, Widget)

public:
	explicit StartGameWindow(Urho3D::Context* context);

private:
	void ShowNewGameTab();
	void ShowLoadGameTab();
	void FillScenesList(const Urho3D::StringVector& scenes);
	void ClearScenesList();
	void SetServerPanelVisible(bool visible);
	Urho3D::StringVector ScanScenesInPackages() const;
	Urho3D::StringVector ScanScenesInPath(const Urho3D::String& path) const;

	void OnNewGame(Urho3D::StringHash, Urho3D::VariantMap&);
	void OnLoadGame(Urho3D::StringHash, Urho3D::VariantMap&);
	void OnItemSelected(Urho3D::StringHash, Urho3D::VariantMap& eventData);
	void OnServerToggled(Urho3D::StringHash, Urho3D::VariantMap& eventData);

	Urho3D::ListView* levelsList_;
	Urho3D::UIElement* serverPanel_;
	Urho3D::UIElement* spawnedControls_;
	Urho3D::LineEdit* serverName_;
	Urho3D::LineEdit* serverPass_;
};

#endif // StartGameWindow_H
