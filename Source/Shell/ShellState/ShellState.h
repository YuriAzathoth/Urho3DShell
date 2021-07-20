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

#ifndef GAMESTATE_H
#define GAMESTATE_H

#include <Urho3D/Core/Object.h>
#include <Urho3D/Scene/Scene.h>
#include "UI/Widget.h"
#include "Urho3DShellAPI.h"

class URHO3DSHELLAPI_EXPORT ShellState : public Urho3D::Object
{
	URHO3D_OBJECT(ShellState, Urho3D::Object)

public:
	explicit ShellState(Urho3D::Context* context);
	virtual ~ShellState();

	void CreateDialog(Urho3D::StringHash type);
	void RemoveDialog(Urho3D::StringHash type);
	void Clear();

	void SetUpdate(bool update);
	void SetPausable(bool pausable) noexcept { pausable_ = pausable; }

	unsigned GetCloseablesCount() const noexcept { return closeables_; }
	unsigned GetInteractivesCount() const noexcept { return interactives_; }
	Urho3D::Scene& GetScene() noexcept { return scene_; }
	const Urho3D::Scene& GetScene() const noexcept { return scene_; }
	bool IsUpdate() const { return scene_.IsUpdateEnabled(); }
	bool IsPausable() const { return pausable_; }

	template <typename T> T* GetDialog(Urho3D::StringHash type);

private:
	void PostWidgetAdd(Widget* widget);
	void PreWidgetRemove(Widget* widget);
	void SetMouseVisible(bool visible) const;
	void CloseFrontDialog();

	void OnActionUp(Urho3D::StringHash, Urho3D::VariantMap& eventData);
	void OnActionDown(Urho3D::StringHash, Urho3D::VariantMap& eventData);
	void OnKeyDown(Urho3D::StringHash, Urho3D::VariantMap& eventData);

	Urho3D::Scene scene_;
	Urho3D::HashMap<Urho3D::StringHash, Urho3D::SharedPtr<Widget>> widgets_;
	unsigned char closeables_;
	unsigned char interactives_;
	bool pausable_;
};

template <typename T> T* ShellState::GetDialog(Urho3D::StringHash type)
{
	auto it = widgets_.Find(type);
	return it != widgets_.End() ? it->second_.Get()->Cast<T>() : nullptr;
}

#endif // GAMESTATE_H
