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

#ifndef CONTROLLERSREGISTRY_H
#define CONTROLLERSREGISTRY_H

#include <Urho3D/Core/Object.h>
#include "InputController.h"
#include "Urho3DShellAPI.h"

namespace Urho3D
{
class XMLElement;
}

class URHO3DSHELLAPI_EXPORT ControllersRegistry : public Urho3D::Object
{
	URHO3D_OBJECT(ControllersRegistry, Urho3D::Object)

public:
	explicit ControllersRegistry(Urho3D::Context* context);
	~ControllersRegistry();

	void ReadControls(Urho3D::Controls& controls) const;

	void RegisterController(Urho3D::SharedPtr<InputController> controller);
	void RemoveController(Urho3D::StringHash controllerType);
	void EnableController(Urho3D::StringHash controllerType);
	void DisableController(Urho3D::StringHash controllerType);
	InputController* GetController(Urho3D::StringHash controllerType) const;
	Urho3D::StringVector GetEnabledControllers() const;
	void RemoveAllControllers();

	template <typename T> T* RegisterController();
	template <typename T> void RemoveController();
	template <typename T> void EnableController();
	template <typename T> void DisableController();
	template <typename T> T* GetController() const;

	void SetConfigPath(const Urho3D::String& configPath) { configPath_ = configPath; }

	Urho3D::String GetDebugString() const;

private:
	void EnableController(InputController* inputController);
	void DisableController(InputController* inputController);

	using ControllersMap = Urho3D::HashMap<Urho3D::StringHash, Urho3D::SharedPtr<InputController>>;
	ControllersMap enabledControllers_;
	ControllersMap disabledControllers_;
	Urho3D::String configPath_;
};

template <typename T> T* ControllersRegistry::RegisterController()
{
	Urho3D::SharedPtr<InputController> controller(new T(context_));
	RegisterController(controller);
	return static_cast<T*>(controller.Get());
}

template <typename T> void ControllersRegistry::RemoveController()
{
	RemoveController(T::GetTypeInfoStatic()->GetType());
}
template <typename T> void ControllersRegistry::EnableController()
{
	EnableController(T::GetTypeInfoStatic()->GetType());
}
template <typename T> void ControllersRegistry::DisableController()
{
	DisableController(T::GetTypeInfoStatic()->GetType());
}

template <typename T> T* ControllersRegistry::GetController() const
{
	return static_cast<T*>(GetController(T::GetTypeInfoStatic()->GetType()));
}

#endif // CONTROLLERSREGISTRY_H
