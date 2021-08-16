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

#ifndef INPUTREGISTRY_H
#define INPUTREGISTRY_H

#include <Urho3D/Core/Object.h>
#include "InputController.h"
#include "Urho3DShellAPI.h"

class URHO3DSHELLAPI_EXPORT InputRegistry : public Urho3D::Object
{
	URHO3D_OBJECT(InputRegistry, Urho3D::Object)

public:
	using ActionsVector = Urho3D::PODVector<Urho3D::StringHash>;

	explicit InputRegistry(Urho3D::Context* context);
	~InputRegistry();

	void RegisterActionLocal(const Urho3D::String& actionName);
	unsigned RegisterActionRemote(const Urho3D::String& actionName);
	void RemoveAction(Urho3D::StringHash action);
	void RemoveAllActions();

	unsigned GetActionFlag(Urho3D::StringHash action) const;
	const Urho3D::String& GetActionName(Urho3D::StringHash action) const;
	bool IsActionRemote(Urho3D::StringHash action) const { return remoteFlags_.Contains(action); }
	const ActionsVector& GetActions() const noexcept { return ordered_; }

	void RegisterController(Urho3D::SharedPtr<InputController> controller);
	void RemoveController(Urho3D::StringHash controllerType);
	void EnableController(Urho3D::StringHash controllerType);
	void DisableController(Urho3D::StringHash controllerType);
	InputController* GetController(Urho3D::StringHash controllerType) const;
	Urho3D::StringVector GetEnabledControllers() const;
	void RemoveAllControllers();

	Urho3D::String GetDebugString() const;

	template <typename T> T* RegisterController();
	template <typename T> void RemoveController();
	template <typename T> void EnableController();
	template <typename T> void DisableController();
	template <typename T> T* GetController() const;

private:
	void EnableController(InputController* inputController);
	void DisableController(InputController* inputController);

	using ControllersMap = Urho3D::HashMap<Urho3D::StringHash, Urho3D::SharedPtr<InputController>>;

	ActionsVector ordered_;
	Urho3D::HashMap<Urho3D::StringHash, unsigned> remoteFlags_;
	ControllersMap enabledControllers_;
	ControllersMap disabledControllers_;
	Urho3D::StringMap names_;
	unsigned lastRemoteFlag_;
};

template <typename T> T* InputRegistry::RegisterController()
{
	Urho3D::SharedPtr<InputController> controller(new T(context_));
	RegisterController(controller);
	return static_cast<T*>(controller.Get());
}

template <typename T> void InputRegistry::RemoveController() { RemoveController(T::GetTypeInfoStatic()->GetType()); }
template <typename T> void InputRegistry::EnableController() { EnableController(T::GetTypeInfoStatic()->GetType()); }
template <typename T> void InputRegistry::DisableController() { DisableController(T::GetTypeInfoStatic()->GetType()); }

template <typename T> T* InputRegistry::GetController() const
{
	return static_cast<T*>(GetController(T::GetTypeInfoStatic()->GetType()));
}

#endif // INPUTREGISTRY_H
