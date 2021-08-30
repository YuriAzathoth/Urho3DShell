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

#ifndef SHELLSTATE_H
#define SHELLSTATE_H

#include <Urho3D/Core/Object.h>
#include "UI/Dialog.h"
#include "Urho3DShellAPI.h"

class URHO3DSHELLAPI_EXPORT ShellState : public Urho3D::Object
{
	URHO3D_OBJECT(ShellState, Urho3D::Object)

public:
	explicit ShellState(Urho3D::Context* context);
	virtual ~ShellState() {}

	virtual void Enter() = 0;
	virtual void Exit();

	Dialog* CreateDialog(Urho3D::StringHash type);
	void RemoveDialog(Urho3D::StringHash type);
	void RemoveAllDialogs();

	unsigned GetCloseables() const noexcept { return closeables_; }
	unsigned GetInteractives() const noexcept { return interactives_; }

	template <typename T> T* GetDialog(Urho3D::StringHash type);

protected:
	bool ReleaseSelf();

private:
	virtual void BackState() = 0;
	virtual void SetSceneUpdate(bool) {}

	void OnDialogAdd(Dialog* widget);
	void OnDialogRemove(Dialog* widget);
	void IncInteractives();
	void DecInteractives();
	void SetMouseVisible(bool visible) const;

	void CloseFrontDialog();
	void OnEscapePressed();
	void ToggleConsole();

	void OnKeyDown(Urho3D::StringHash, Urho3D::VariantMap& eventData);

	Urho3D::HashMap<Urho3D::StringHash, Urho3D::SharedPtr<Dialog>> dialogs_;
	unsigned char closeables_;
	unsigned char interactives_;
};

template <typename T> inline T* ShellState::GetDialog(Urho3D::StringHash type)
{
	const auto it = dialogs_.Find(type);
	return it != dialogs_.End() ? it->second_.Get()->Cast<T>() : nullptr;
}

#endif // SHELLSTATE_H
