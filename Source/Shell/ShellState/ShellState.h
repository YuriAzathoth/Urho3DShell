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

#ifndef GAMESTATE_H
#define GAMESTATE_H

#include <Urho3D/Core/Object.h>
#include <Urho3D/Scene/Scene.h>
#include "Urho3DShellAPI.h"

class URHO3DSHELLAPI_EXPORT ShellState : public Urho3D::Object
{
	URHO3D_OBJECT(ShellState, Urho3D::Object)

public:
	explicit ShellState(Urho3D::Context* context);
	virtual ~ShellState();

	void ActivateMainMenu();

	//	void Clear();
	//
	//	void SetUpdate(bool update);
	//	void SetPausable(bool pausable) noexcept { pausable_ = pausable; }
	//
	//	Urho3D::Scene& GetScene() noexcept { return scene_; }
	//	const Urho3D::Scene& GetScene() const noexcept { return scene_; }
	//
	//	bool IsUpdate() const { return scene_.IsUpdateEnabled(); }
	//	bool IsPausable() const { return pausable_; }
	//
	// private:
	//	void OnActionUp(Urho3D::StringHash, Urho3D::VariantMap& eventData);
	//	void OnActionDown(Urho3D::StringHash, Urho3D::VariantMap& eventData);
	//
	//	Urho3D::Scene scene_;
	//	bool pausable_;
};

#endif // GAMESTATE_H
