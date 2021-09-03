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

#include <Urho3D/Core/Variant.h>

class DynamicParameter : public Urho3D::RefCounted
{
public:
	DynamicParameter(Urho3D::VariantType type, Urho3D::StringHash settingsTab, bool isEngine)
		: settingsTab_(settingsTab)
		, type_(type)
		, isEngine_(isEngine)
	{
	}
	virtual ~DynamicParameter() {}

	virtual Urho3D::Variant Read() = 0;
	virtual void Write(const Urho3D::Variant& value) = 0;

	Urho3D::StringHash GetSettingsTab() const noexcept { return settingsTab_; }
	Urho3D::VariantType GetType() const noexcept { return type_; }
	bool IsEngine() const noexcept { return isEngine_; }

protected:
	Urho3D::StringHash settingsTab_;
	Urho3D::VariantType type_;
	bool isEngine_;
};

class ComplexParameter : public Urho3D::RefCounted
{
public:
	ComplexParameter(bool isEngine)
		: changed_(false)
		, isEngine_(isEngine)
	{
	}
	virtual ~ComplexParameter() {}

	void Apply()
	{
		if (changed_)
		{
			ApplyImpl();
			parameters_.Clear();
		}
	}

	void Set(Urho3D::StringHash name, const Urho3D::Variant& value)
	{
		parameters_[name] = value;
		changed_ = true;
	}

	bool IsEngine() const noexcept { return isEngine_; }

protected:
	Urho3D::VariantMap parameters_;

private:
	virtual void ApplyImpl() = 0;

	bool changed_;
	bool isEngine_;
};

#endif // DYNAMICPARAMETER_H
