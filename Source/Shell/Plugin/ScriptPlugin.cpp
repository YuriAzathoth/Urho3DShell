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
