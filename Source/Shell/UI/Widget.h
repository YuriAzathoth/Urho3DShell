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

#ifndef WIDGET_H
#define WIDGET_H

#include <Urho3D/Core/Object.h>
#include <Urho3D/Container/FlagSet.h>
#include <Urho3D/UI/UIElement.h>
#include "Urho3DShellAPI.h"

class URHO3DSHELLAPI_EXPORT Widget : public Urho3D::Object
{
	URHO3D_OBJECT(Widget, Urho3D::Object)

public:
	enum class Flags : unsigned char
	{
		NONE = 0x0,
		DIALOG = 0x1,
		MAIN = 0x2
	};

	explicit Widget(Urho3D::Context* context);
	virtual ~Widget();

	void LoadLayout(const Urho3D::String& layoutName);

	bool IsFrontElement() const;

	void SetFlags(Urho3D::FlagSet<Flags> flags) noexcept { flags_ = flags; }
	Urho3D::FlagSet<Flags> GetFlags() const noexcept { return flags_; }
	Urho3D::UIElement* GetRoot() const { return root_.Get(); }
	bool IsDialog() const noexcept { return flags_ & Flags::DIALOG; }
	bool IsCloseable() const noexcept { return IsDialog() && !(flags_ & Flags::MAIN); }

protected:
	void Close();
	void UpdateSize();

	Urho3D::UIElement* GetChild(const Urho3D::String& name, bool recurse) { return root_->GetChild(name, recurse); }

	template <typename T> T* GetChildStaticCast(const Urho3D::String& name, bool recurse)
	{
		return root_->GetChildStaticCast<T>(name, recurse);
	}

	template <typename T> T* GetChildDynamicCast(const Urho3D::String& name, bool recurse)
	{
		return root_->GetChildDynamicCast<T>(name, recurse);
	}

private:
	Urho3D::SharedPtr<Urho3D::UIElement> root_;
	Urho3D::FlagSet<Flags> flags_;
};

#endif // WIDGET_H
