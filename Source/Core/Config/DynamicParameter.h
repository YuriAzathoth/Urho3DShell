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

#ifndef DYNAMICPARAMETER_H
#define DYNAMICPARAMETER_H

#include <Urho3D/Core/Variant.h>

class DynamicParameter : public Urho3D::RefCounted
{
public:
	DynamicParameter(Urho3D::VariantType type, Urho3D::StringHash settingsTab, bool engine)
		: settingsTab_(settingsTab)
		, type_(type)
		, engine_(engine)
	{
	}
	virtual ~DynamicParameter() {}

	virtual Urho3D::Variant Read() = 0;
	virtual void Write(const Urho3D::Variant& value) = 0;

	Urho3D::StringHash GetSettingsTab() const { return settingsTab_; }
	Urho3D::VariantType GetType() const { return type_; }
	bool IsEngine() const { return engine_; }

protected:
	Urho3D::StringHash settingsTab_;
	Urho3D::VariantType type_;
	bool engine_;
};

class ComplexParameter : public Urho3D::RefCounted
{
public:
	ComplexParameter(bool engine)
		: changed_(false)
		, engine_(engine)
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

	bool IsEngine() const { return engine_; }

protected:
	Urho3D::VariantMap parameters_;

private:
	virtual void ApplyImpl() = 0;

	bool changed_;
	bool engine_;
};

#endif // DYNAMICPARAMETER_H
