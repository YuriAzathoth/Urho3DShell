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

#ifndef BINARYPARAMETER_H
#define BINARYPARAMETER_H

#include "ComplexParameterStorage.h"
#include "ConfigTypes.h"
#include "DynamicParameter.h"

class BinaryParameter : public DynamicParameter
{
public:
	BinaryParameter(SimpleReaderFunc&& reader,
					Urho3D::VariantType type,
					Urho3D::StringHash settingsTab,
					bool isEngine,
					bool isLocalized);

	Urho3D::Variant Read() override;

private:
	const SimpleReaderFunc reader_;
};

class SimpleBinaryParameter : public BinaryParameter
{
public:
	SimpleBinaryParameter(SimpleReaderFunc&& reader,
						  SimpleWriterFunc&& writer,
						  Urho3D::VariantType type,
						  Urho3D::StringHash settingsTab,
						  bool isEngine,
						  bool isLocalized);

	void Write(const Urho3D::Variant& value) override;

private:
	const SimpleWriterFunc writer_;
};

class ComplexBinaryParameter : public BinaryParameter
{
public:
	ComplexBinaryParameter(Urho3D::WeakPtr<ComplexParameterStorage> storage,
						   SimpleReaderFunc&& reader,
						   Urho3D::StringHash name,
						   Urho3D::VariantType type,
						   Urho3D::StringHash settingsTab,
						   bool isEngine,
						   bool isLocalized);

	void Write(const Urho3D::Variant& value) override;

private:
	Urho3D::WeakPtr<ComplexParameterStorage> storage_;
	const Urho3D::StringHash name_;
};

#endif // BINARYPARAMETER_H
