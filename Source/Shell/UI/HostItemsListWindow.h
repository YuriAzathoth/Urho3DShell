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

#ifndef HOSTITEMSLISTWINDOW_H
#define HOSTITEMSLISTWINDOW_H

#include "ItemsListWindow.h"

class HostItemsListWindow : public ItemsListWindow
{
	URHO3D_OBJECT(HostItemsListWindow, ItemsListWindow)

public:
	using ItemsListWindow::ItemsListWindow;

private:
	void Start(const Urho3D::String& itemName) override;
	void
	Start(const Urho3D::String& itemName, const Urho3D::String& serverName, const Urho3D::String& serverPass) override;
};

#endif // HOSTITEMSLISTWINDOW_H
