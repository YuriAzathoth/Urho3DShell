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

#ifndef DIALOG_H
#define DIALOG_H

#include <Urho3D/Core/Object.h>
#include <Urho3D/UI/UIElement.h>
#include "Urho3DShellAPI.h"

class URHO3DSHELLAPI_EXPORT Dialog : public Urho3D::Object
{
	URHO3D_OBJECT(Dialog, Urho3D::Object)

public:
	explicit Dialog(Urho3D::Context* context);
	virtual ~Dialog();

	void LoadLayout(const Urho3D::String& layoutName);
	void ShrinkSize();

	void SetCloseable(bool closeable) noexcept { closeable_ = closeable; }
	void SetInteractive(bool interactive) noexcept { interactive_ = interactive; }

	Urho3D::UIElement* GetRoot() const { return root_.Get(); }
	bool IsCloseable() const noexcept { return closeable_; }
	bool IsFrontElement() const;
	bool IsInteractive() const noexcept { return interactive_; }

	void CreateDialog(Urho3D::StringHash type);
	void RemoveDialog(Urho3D::StringHash type);

protected:
	void Close();
	void BindButtonToClose(Urho3D::UIElement* button);

	Urho3D::SharedPtr<Urho3D::UIElement> root_;
	bool closeable_;
	bool interactive_;
};

#endif // DIALOG_H