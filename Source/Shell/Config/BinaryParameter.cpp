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

#include "BinaryParameter.h"

BinaryParameter::BinaryParameter(SimpleReaderFunc&& reader,
								 Urho3D::VariantType type,
								 Urho3D::StringHash settingsTab,
								 bool isEngine)
	: DynamicParameter(type, settingsTab, isEngine)
	, reader_(std::move(reader))
{
}

Urho3D::Variant BinaryParameter::Read() { return reader_(); }

SimpleBinaryParameter::SimpleBinaryParameter(SimpleReaderFunc&& reader,
											 SimpleWriterFunc&& writer,
											 Urho3D::VariantType type,
											 Urho3D::StringHash settingsTab,
											 bool isEngine)
	: BinaryParameter(std::move(reader), type, settingsTab, isEngine)
	, writer_(std::move(writer))
{
}

void SimpleBinaryParameter::Write(const Urho3D::Variant& value) { writer_(value); }

ComplexBinaryParameter::ComplexBinaryParameter(Urho3D::WeakPtr<ComplexParameter> storage,
											   SimpleReaderFunc&& reader,
											   Urho3D::StringHash name,
											   Urho3D::VariantType type,
											   Urho3D::StringHash settingsTab,
											   bool isEngine)
	: BinaryParameter(std::move(reader), type, settingsTab, isEngine)
	, storage_(storage)
	, name_(name)
{
}

void ComplexBinaryParameter::Write(const Urho3D::Variant& value) { storage_->Set(name_, value); }
