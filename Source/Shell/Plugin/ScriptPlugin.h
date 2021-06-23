#ifndef SCRIPTPLUGIN_H
#define SCRIPTPLUGIN_H

#include <Urho3D/AngelScript/ScriptFile.h>
#include "Plugin.h"

class ScriptPlugin : public Plugin
{
	URHO3D_OBJECT(ScriptPlugin, Plugin)

public:
	explicit ScriptPlugin(Urho3D::Context* context);
	~ScriptPlugin();

	bool Load(const Urho3D::String& fileName) override;

	const Urho3D::String& GetName() const override;

private:
	bool Execute(const Urho3D::String& functionName,
				 const Urho3D::VariantVector& parameters = Urho3D::Variant::emptyVariantVector,
				 Urho3D::Variant* returnVar = nullptr);

	Urho3D::ScriptFile* script_;
};

#endif // SCRIPTPLUGIN_H
