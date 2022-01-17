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

#include <Urho3D/AngelScript/ScriptFile.h>
#include <Urho3D/IO/Log.h>
#include "ScriptParameter.h"

using namespace Urho3D;

ScriptParameter::ScriptParameter(Urho3D::VariantType type,
								 Urho3D::StringHash settingsTab,
								 bool isEngine,
								 const Urho3D::String& reader,
								 const Urho3D::String& writer)
	: DynamicParameter(type, settingsTab, isEngine)
	, file_(GetScriptContextFile())
	, reader_(nullptr)
	, success_(false)
{
	if (!file_)
	{
		URHO3D_LOGERROR("Failed to assign script parameter: file not loaded or called from console.");
		return;
	}

	reader_ = file_->GetFunction(ToString("%s %s()", GetReaderType(type).CString(), reader.CString()));
	writer_ = file_->GetFunction(ToString("void %s(%s)", writer.CString(), GetWriterType(type).CString()));
	if (!reader_)
		URHO3D_LOGERRORF("Could not assign config parameter reader: function not found.");
	else if (!writer_)
		URHO3D_LOGERRORF("Could not assign config parameter writer: function not found.");
	else
		success_ = true;
}

Urho3D::Variant ScriptParameter::Read()
{
	Variant ret;
	file_->Execute(reader_, Variant::emptyVariantVector, &ret);
	return ret;
}

void ScriptParameter::Write(const Urho3D::Variant& value) { file_->Execute(writer_, {{value}}); }

Urho3D::String ScriptParameter::GetReaderType(Urho3D::VariantType type)
{
	const String typeName = Variant::GetTypeName(type);
	if (type != VAR_STRING)
		return typeName.ToLower();
	else
		return typeName;
}

Urho3D::String ScriptParameter::GetWriterType(Urho3D::VariantType type)
{
	const String typeName = Variant::GetTypeName(type);
	if (type != VAR_STRING)
		return typeName.ToLower();
	else
		return ToString("const %s&in", typeName.CString());
}
