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

#ifndef FRONTSTATE_H
#define FRONTSTATE_H

#include <Urho3D/Core/Object.h>
#include "U3SClientAPI.h"
#include "UI/Dialog.h"

#ifdef _WIN32
#undef CreateDialog
#undef MessageBox
#endif // _WIN32

namespace Urho3D
{
class MessageBox;
}

class U3SCLIENTAPI_EXPORT FrontState : public Urho3D::Object
{
	URHO3D_OBJECT(FrontState, Urho3D::Object)

public:
	explicit FrontState(Urho3D::Context* context);
	virtual ~FrontState() {}

	virtual void Enter() = 0;
	virtual void Exit();

	Dialog* GetDialog(Urho3D::StringHash type) const;
	Dialog* CreateDialog(Urho3D::StringHash type);
	void RemoveDialog(Urho3D::StringHash type);
	void RemoveAllDialogs();

	void ShowErrorMessage(const Urho3D::String& text, const Urho3D::String& title);
	void ShowQuestionMessage(const Urho3D::String& text, const Urho3D::String& title);

	unsigned GetCloseables() const noexcept { return closeables_; }
	unsigned GetInteractives() const noexcept { return interactives_; }

	template <typename T> T* GetDialog() const { return static_cast<T*>(GetDialog(T::GetTypeInfoStatic()->GetType())); }
	template <typename T> T* CreateDialog() { return static_cast<T*>(CreateDialog(T::GetTypeInfoStatic()->GetType())); }
	template <typename T> void RemoveDialog() { RemoveDialog(T::GetTypeInfoStatic()->GetType()); }

protected:
	bool ReleaseSelf();

private:
	virtual void BackState() {}
	virtual void SetSceneUpdate(bool) {}

	void OnDialogAdd(Dialog* widget);
	void OnDialogRemove(Dialog* widget);
	void IncInteractives();
	void DecInteractives();
	void SetMouseVisible(bool visible) const;

	void CloseFrontDialog();
	void OnEscapePressed();
	void ToggleConsole();

	void ShowMessageBox(const Urho3D::String& text, const Urho3D::String& title);
	void EnableCancelButton();

	void OnKeyDown(Urho3D::StringHash, Urho3D::VariantMap& eventData);
	void OnMessageACK(Urho3D::StringHash, Urho3D::VariantMap&);

	Urho3D::HashMap<Urho3D::StringHash, Urho3D::SharedPtr<Dialog>> dialogs_;
	Urho3D::MessageBox* message_;
	unsigned char closeables_;
	unsigned char interactives_;
};

#endif // FRONTSTATE_H
