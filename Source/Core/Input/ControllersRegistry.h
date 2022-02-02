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

#ifndef CONTROLLERSREGISTRY_H
#define CONTROLLERSREGISTRY_H

#include <Urho3D/Core/Object.h>
#include "InputController.h"
#include "U3SCoreAPI.h"

namespace Urho3D
{
class XMLElement;
}

class U3SCOREAPI_EXPORT ControllersRegistry : public Urho3D::Object
{
	URHO3D_OBJECT(ControllersRegistry, Urho3D::Object)

public:
	explicit ControllersRegistry(Urho3D::Context* context);
	~ControllersRegistry();

	void ReadControls(Urho3D::Controls& controls) const;

	void Register(Urho3D::SharedPtr<InputController> controller);
	void Remove(Urho3D::StringHash controllerType);
	void Enable(Urho3D::StringHash controllerType);
	void Disable(Urho3D::StringHash controllerType);
	InputController* Get(Urho3D::StringHash controllerType) const;
	Urho3D::StringVector GetEnabledNames() const;
	void RemoveAll();

	template <typename T> T* Register();
	template <typename T> void Remove();
	template <typename T> void Enable();
	template <typename T> void Disable();
	template <typename T> T* Get() const;

	Urho3D::String GetDebugString() const;

private:
	void EnableImpl(InputController* inputController);
	void DisableImpl(InputController* inputController);

	using ControllersMap = Urho3D::HashMap<Urho3D::StringHash, Urho3D::SharedPtr<InputController>>;
	ControllersMap enabledControllers_;
	ControllersMap disabledControllers_;
};

template <typename T> T* ControllersRegistry::Register()
{
	Urho3D::SharedPtr<InputController> controller(new T(context_));
	Register(controller);
	return static_cast<T*>(controller.Get());
}

template <typename T> void ControllersRegistry::Remove() { Remove(T::GetTypeInfoStatic()->GetType()); }
template <typename T> void ControllersRegistry::Enable() { Enable(T::GetTypeInfoStatic()->GetType()); }
template <typename T> void ControllersRegistry::Disable() { Disable(T::GetTypeInfoStatic()->GetType()); }

template <typename T> T* ControllersRegistry::Get() const
{
	return static_cast<T*>(Get(T::GetTypeInfoStatic()->GetType()));
}

#endif // CONTROLLERSREGISTRY_H
