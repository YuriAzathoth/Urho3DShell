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

#ifndef INPUTBINDINGS_H
#define INPUTBINDINGS_H

#include <Urho3D/Core/Object.h>
#include "InputController.h"
#include "Urho3DShellAPI.h"

namespace Urho3D
{
class XMLElement;
}

class URHO3DSHELLAPI_EXPORT InputClient : public Urho3D::Object
{
	URHO3D_OBJECT(InputClient, Urho3D::Object)

public:
	explicit InputClient(Urho3D::Context* context);
	~InputClient();

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

template <typename T> T* InputClient::RegisterController()
{
	Urho3D::SharedPtr<InputController> controller(new T(context_));
	RegisterController(controller);
	return static_cast<T*>(controller.Get());
}

template <typename T> void InputClient::RemoveController() { RemoveController(T::GetTypeInfoStatic()->GetType()); }
template <typename T> void InputClient::EnableController() { EnableController(T::GetTypeInfoStatic()->GetType()); }
template <typename T> void InputClient::DisableController() { DisableController(T::GetTypeInfoStatic()->GetType()); }

template <typename T> T* InputClient::GetController() const
{
	return static_cast<T*>(GetController(T::GetTypeInfoStatic()->GetType()));
}

#endif // INPUTBINDINGS_H
