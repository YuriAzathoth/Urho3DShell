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

#ifndef UICONTROLLER_H
#define UICONTROLLER_H

#include <Urho3D/Core/Object.h>
#include "UI/Widget.h"
#include "Urho3DShellAPI.h"

class URHO3DSHELLAPI_EXPORT UIController : public Urho3D::Object
{
	URHO3D_OBJECT(UIController, Urho3D::Object)

public:
	explicit UIController(Urho3D::Context* context);

	void CreateDialog(Urho3D::StringHash type);
	void RemoveDialog(Urho3D::StringHash type);
	void RemoveAllDialogs();

	unsigned GetCloseablesCount() const noexcept { return closeables_; }
	unsigned GetInteractivesCount() const noexcept { return interactives_; }

	template <typename T> T* GetDialog(Urho3D::StringHash type);

private:
	void PostWidgetAdd(Widget* widget);
	void PreWidgetRemove(Widget* widget);
	void SetMouseVisible(bool visible) const;
	void CloseFrontDialog();

	void OnKeyDown(Urho3D::StringHash, Urho3D::VariantMap& eventData);

	Urho3D::HashMap<Urho3D::StringHash, Urho3D::SharedPtr<Widget>> widgets_;
	unsigned char closeables_;
	unsigned char interactives_;
};

template <typename T> T* UIController::GetDialog(Urho3D::StringHash type)
{
	const auto it = widgets_.Find(type);
	return it != widgets_.End() ? it->second_.Get()->Cast<T>() : nullptr;
}

#endif // UICONTROLLER_H
