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

#ifndef INPUTCONTROLLER_H
#define INPUTCONTROLLER_H

#include <Urho3D/Core/Object.h>
#include "Urho3DShellAPI.h"

namespace Urho3D
{
class Controls;
class XMLElement;
} // namespace Urho3D

class URHO3DSHELLAPI_EXPORT InputController : public Urho3D::Object
{
	URHO3D_OBJECT(InputController, Urho3D::Object)

public:
	using BindingsMap = Urho3D::HashMap<unsigned, Urho3D::StringHash>;
	using DefaultBindingsMap = Urho3D::HashMap<Urho3D::StringHash, unsigned>;

	explicit InputController(Urho3D::Context* context);
	virtual ~InputController() {}

	virtual void ReadControls(Urho3D::Controls& controls) const = 0;
	virtual Urho3D::String GetKeyName(unsigned keyCode) const = 0;
	virtual unsigned GetKeyCode(const Urho3D::String& keyName) const = 0;
	virtual void StartBinding(Urho3D::StringHash action) = 0;
	virtual void EndBinding() = 0;

	bool EnableController();
	bool DisableController();

	bool LoadXML(const Urho3D::XMLElement& source);
	void SaveXML(Urho3D::XMLElement& dst) const;

	void SetBinding(Urho3D::StringHash action, unsigned keyCode);
	unsigned GetBindingKey(Urho3D::StringHash action) const;
	const BindingsMap& GetBindings() const noexcept { return bindings_; }
	void RemoveAllBindings();

	void SetDefaultBinding(Urho3D::StringHash action, unsigned keyCode) { defaultBindings_[action] = keyCode; }
	void RemoveDefaultBinding(Urho3D::StringHash action) { defaultBindings_.Erase(action); }
	unsigned GetDefaultBinding(Urho3D::StringHash action) const;
	const DefaultBindingsMap& GetDefaultBindings() const noexcept { return defaultBindings_; }
	void RemoveAllDefaultBindings() { defaultBindings_.Clear(); }

	void SetSensitivity(float sensitivity) noexcept { sensitivity_ = sensitivity; }
	float GetSensitivity() const noexcept { return sensitivity_; }

	bool IsEnabled() const noexcept { return enabled_; }

	Urho3D::String GetDebugString() const;

protected:
	using RemoteBindingsVector = Urho3D::PODVector<Urho3D::Pair<unsigned, unsigned>>;

	void SendAction(unsigned keyCode, bool isDown);

	const RemoteBindingsVector& GetRemoteBindings() const noexcept { return remoteBindings_; }

private:
	virtual bool Enable() = 0;
	virtual bool Disable() = 0;

	BindingsMap bindings_;											// Key -> Action
	RemoteBindingsVector remoteBindings_;							// Key -> Flag
	Urho3D::HashMap<Urho3D::StringHash, unsigned> defaultBindings_; // Action -> Key
	float sensitivity_;
	bool enabled_;
};

inline void InputController::RemoveAllBindings()
{
	bindings_.Clear();
	remoteBindings_.Clear();
}

#endif // INPUTCONTROLLER_H
