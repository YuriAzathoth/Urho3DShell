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

#ifndef ITEMSLISTWINDOW_H
#define ITEMSLISTWINDOW_H

#include "Dialog.h"

namespace Urho3D
{
class CheckBox;
class LineEdit;
class ListView;
} // namespace Urho3D

class ItemsListWindow : public Dialog
{
	URHO3D_OBJECT(ItemsListWindow, Dialog)

public:
	explicit ItemsListWindow(Urho3D::Context* context);

	void AddGame(const Urho3D::String& caption, const Urho3D::String& gameName);
	void ClearGamesList();

	void SetTitle(const Urho3D::String& title);
	void SetServerSettingsVisible(bool visible);

private:
	virtual void Start(const Urho3D::String& gameName) = 0;
	virtual void
	Start(const Urho3D::String& gameName, const Urho3D::String& serverName, const Urho3D::String& serverPass) = 0;

	void SetServerPanelVisible(bool visible);

	void OnItemSelected(Urho3D::StringHash, Urho3D::VariantMap&);
	void OnServerToggled(Urho3D::StringHash, Urho3D::VariantMap& eventData);
	void OnStart(Urho3D::StringHash, Urho3D::VariantMap&);

	Urho3D::ListView* gamesList_;
	Urho3D::UIElement* serverPanel_;
	Urho3D::UIElement* spawnedButton_;
	Urho3D::CheckBox* server_;
	Urho3D::LineEdit* serverName_;
	Urho3D::LineEdit* serverPass_;
};

#endif // ITEMSLISTWINDOW_H
