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

#include <Urho3D/IO/Log.h>
#include <Urho3D/Resource/ResourceCache.h>
#include "ScriptPlugin.h"

using namespace Urho3D;

ScriptPlugin::ScriptPlugin(Urho3D::Context* context)
	: Plugin(context)
	, script_(nullptr)
{
}

ScriptPlugin::~ScriptPlugin() { Execute("void Stop()"); }

bool ScriptPlugin::Load(const Urho3D::String& fileName)
{
	script_ = GetSubsystem<ResourceCache>()->GetResource<ScriptFile>(fileName);
	if (!script_)
		return false;

	if (!Execute("void Setup()"))
		Execute("void Start()");

	script_->SetName(fileName);
	URHO3D_LOGINFOF("Script plugin \"%s\" loaded.", fileName.CString());

	return true;
}

const Urho3D::String& ScriptPlugin::GetName() const { return script_->GetName(); }

bool ScriptPlugin::Execute(const Urho3D::String& functionName,
						   const Urho3D::VariantVector& parameters,
						   Urho3D::Variant* returnVar)
{
	if (script_)
	{
		asIScriptFunction* functionPtr = script_->GetFunction(functionName);
		return functionPtr ? script_->Execute(functionPtr) : false;
	}
	else
		return false;
}
