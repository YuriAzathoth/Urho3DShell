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

#ifndef ENUMVARIANT_H
#define ENUMVARIANT_H

#include <Urho3D/Container/Str.h>
#include <Urho3D/Container/Vector.h>
#include <Urho3D/Core/Variant.h>
#include <functional>

struct EnumVariant
{
	Urho3D::String caption_;
	Urho3D::Variant value_;
	EnumVariant() = default;
	EnumVariant(const char* caption, Urho3D::Variant value)
		: caption_(caption)
		, value_(value)
	{
	}
	EnumVariant(const Urho3D::String& caption, Urho3D::Variant value)
		: caption_(caption)
		, value_(value)
	{
	}
	EnumVariant(Urho3D::String&& caption, Urho3D::Variant value)
		: caption_(std::move(caption))
		, value_(value)
	{
	}
	EnumVariant(const EnumVariant& src)
		: caption_(src.caption_)
		, value_(src.value_)
	{
	}
	EnumVariant(EnumVariant&& src) noexcept
		: caption_(std::move(src.caption_))
		, value_(src.value_)
	{
	}
	EnumVariant& operator=(const EnumVariant& src)
	{
		caption_ = src.caption_;
		value_ = src.value_;
		return *this;
	}
	EnumVariant& operator=(EnumVariant&& src)
	{
		caption_.Swap(src.caption_);
		value_ = src.value_;
		return *this;
	}
};

using EnumVector = Urho3D::Vector<EnumVariant>;

class EnumConstructor : public Urho3D::RefCounted
{
public:
	EnumConstructor(bool localized)
		: localized_(localized)
	{
	}

	virtual ~EnumConstructor() {}
	virtual EnumVector Create() = 0;

	bool IsLocalized() const noexcept { return localized_; }

private:
	bool localized_;
};

class BinaryEnumConstructor : public EnumConstructor
{
public:
	using ConstructorFunc = std::function<EnumVector()>;

	BinaryEnumConstructor(ConstructorFunc&& constructor, bool localized)
		: EnumConstructor(localized)
		, constructor_(std::move(constructor))
	{
	}

	EnumVector Create() override { return constructor_(); }

private:
	const ConstructorFunc constructor_;
};

#endif // ENUMVARIANT_H
