#ifndef SHELL_H
#define SHELL_H

#include <Urho3D/Core/Object.h>
#include "Urho3DShellAPI.h"

class URHO3DSHELLAPI_EXPORT Shell : public Urho3D::Object
{
	URHO3D_OBJECT(Shell, Urho3D::Object)

public:
	explicit Shell(Urho3D::Context* context);
	~Shell();

	void Initialize();
};

#endif // SHELL_H
