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

#ifndef SHELLSTATEMACHINE_H
#define SHELLSTATEMACHINE_H

#include <Urho3D/Core/Object.h>
#include "ShellState/ShellState.h"
#include "Urho3DShellAPI.h"

class URHO3DSHELLAPI_EXPORT ShellStateMachine : public Urho3D::Object
{
	URHO3D_OBJECT(ShellStateMachine, Urho3D::Object)

public:
	using Urho3D::Object::Object;

	void Initialize(ShellState* newState);
	void NewShellState(ShellState* newState);
	ShellState* GetShellState() const { return currState_.Get(); }

	template <typename T, typename... TArgs> void Initialize(TArgs&&... args);
	template <typename T, typename... TArgs> void NewShellState(TArgs&&... args);

private:
	Urho3D::SharedPtr<ShellState> currState_;
	Urho3D::SharedPtr<ShellState> nextState_;

	bool ProcessStateChanging(); // May be called only from Shell
	friend class ShellState;	 // Allow to call ProcessStateChanging only from ShellState
};

template <typename T, typename... TArgs> void ShellStateMachine::Initialize(TArgs&&... args)
{
	Initialize(new T(context_, std::forward<TArgs>(args)...));
}

template <typename T, typename... TArgs> void ShellStateMachine::NewShellState(TArgs&&... args)
{
	NewShellState(new T(context_, std::forward<TArgs>(args)...));
}

#endif // SHELLSTATEMACHINE_H
