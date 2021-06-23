#include <Urho3D/Core/Context.h>
#include "Core/UserProfile.h"
#include "Plugin/PluginsRegistry.h"
#include "Shell.h"

#define PLUGINS_PATH "Plugins"

using namespace Urho3D;

Shell::Shell(Urho3D::Context* context)
	: Object(context)
{
	context_->RegisterSubsystem<UserProfile>();
	context_->RegisterSubsystem<PluginsRegistry>();
}

Shell::~Shell()
{
	context_->RemoveSubsystem<PluginsRegistry>();
	context_->RemoveSubsystem<UserProfile>();
}

void Shell::Initialize()
{
	UserProfile* userProfile = GetSubsystem<UserProfile>();
	userProfile->Initialize("SampleGame");

	PluginsRegistry* pluginsRegistry = GetSubsystem<PluginsRegistry>();
	pluginsRegistry->SetPluginsPath(userProfile->GetUserDataPath() + PLUGINS_PATH + "/");
}
