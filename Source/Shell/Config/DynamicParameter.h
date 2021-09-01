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

#ifndef DYNAMICPARAMETER_H
#define DYNAMICPARAMETER_H

#include <Urho3D/Container/FlagSet.h>
#include <Urho3D/Container/RefCounted.h>
#include <Urho3D/Core/Variant.h>
#include <Urho3D/Math/StringHash.h>

class DynamicParameter : public Urho3D::RefCounted
{
public:
	DynamicParameter(Urho3D::VariantType type, Urho3D::StringHash settingsTab, bool isEngine, bool isLocalized);
	virtual ~DynamicParameter() {}

	virtual Urho3D::Variant Read() = 0;
	virtual void Write(const Urho3D::Variant& value) = 0;

	void SetEnum() noexcept { flags_ |= Flags::ENUM; }

	Urho3D::StringHash GetSettingsTab() const noexcept { return settingsTab_; }
	Urho3D::VariantType GetType() const noexcept { return type_; }
	bool IsEngine() const noexcept { return flags_ & Flags::ENGINE; }
	bool IsEnum() const noexcept { return flags_ & Flags::ENUM; }
	bool IsLocalized() const noexcept { return flags_ & Flags::LOCALIZED; }

protected:
	enum class Flags : unsigned char
	{
		ENGINE = 0x1,
		ENUM = 0x2,
		LOCALIZED = 0x4
	};

	Urho3D::StringHash settingsTab_;
	Urho3D::VariantType type_;
	Urho3D::FlagSet<Flags> flags_;
};

#endif // DYNAMICPARAMETER_H
