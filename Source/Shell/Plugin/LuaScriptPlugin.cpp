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
#include <Urho3D/LuaScript/LuaFile.h>
#include <Urho3D/LuaScript/LuaFunction.h>
#include <Urho3D/LuaScript/LuaScript.h>
#include <Urho3D/Resource/ResourceCache.h>
#include "LuaScriptPlugin.h"

using namespace Urho3D;

LuaScriptPlugin::LuaScriptPlugin(Urho3D::Context* context)
	: Plugin(context)
	, script_(nullptr)
{
}

LuaScriptPlugin::~LuaScriptPlugin()
{
	if (script_)
		Execute("Stop");
}

bool LuaScriptPlugin::Load(const Urho3D::String& fileName)
{
	script_ = GetSubsystem<ResourceCache>()->GetResource<LuaFile>(fileName);
	if (!script_ || !script_->LoadAndExecute(GetSubsystem<LuaScript>()->GetState()))
		return false;

	Execute("Start");
	script_->SetName(fileName);
	URHO3D_LOGINFOF("Lua script plugin \"%s\" loaded.", fileName.CString());

	return true;
}

const Urho3D::String& LuaScriptPlugin::GetName() const { return script_ ? script_->GetName() : String::EMPTY; }

bool LuaScriptPlugin::Execute(const Urho3D::String& functionName,
							  const Urho3D::VariantVector& parameters,
							  [[maybe_unused]] Urho3D::Variant* returnVar)
{
	LuaFunction* function = GetSubsystem<LuaScript>()->GetFunction(functionName);

	if (!function)
		return false;

	if (!function->IsValid())
	{
		URHO3D_LOGERRORF("Invalid function %s in file %s.", functionName.CString(), script_->GetName().CString());
		return false;
	}

	if (!function->BeginCall())
	{
		URHO3D_LOGERRORF("Failed to call function %s in file %s.",
						 functionName.CString(),
						 script_->GetName().CString());
		return false;
	}

	for (const Variant& value : parameters)
		function->PushVariant(value);
	function->EndCall(0);

	return true;
}
