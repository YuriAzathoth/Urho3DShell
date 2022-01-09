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

#ifndef SETTINGSLIST_H
#define SETTINGSLIST_H

#include <Urho3D/Core/Object.h>

namespace Urho3D
{
class ListView;
class UIElement;
} // namespace Urho3D

class SettingsList : public Urho3D::Object
{
	URHO3D_OBJECT(SettingsList, Urho3D::Object)

public:
	SettingsList(Urho3D::Context* context, Urho3D::ListView* settings);

	virtual void Apply() = 0;

protected:
	Urho3D::UIElement* CreateCaption(const Urho3D::String& parameterName);

	Urho3D::ListView* settings_;

private:
	SettingsList(const SettingsList&) = delete;
	SettingsList& operator=(const SettingsList&) = delete;
};

#endif // SETTINGSLIST_H
