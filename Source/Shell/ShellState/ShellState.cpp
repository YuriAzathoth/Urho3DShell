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

#include <Urho3D/Core/Context.h>
#include <Urho3D/IO/Log.h>
#include "ShellState.h"

using namespace Urho3D;

ShellState::ShellState(Urho3D::Context* context)
	: Urho3D::Object(context)
	, scene_(context)
{
}

ShellState::~ShellState() {}

void ShellState::Clear()
{
	dialogs_.Clear();
	scene_.Clear();
}

void ShellState::CreateDialog(Urho3D::StringHash type)
{
	SharedPtr<Object> object = context_->CreateObject(type);
	if (object.Null())
	{
		URHO3D_LOGERROR("Failed to create unregistered dialog.");
		return;
	}

	SharedPtr<Dialog> dialog;
	dialog.DynamicCast(object);
	if (dialog.NotNull())
		dialogs_[type] = dialog;
	else
		URHO3D_LOGERROR("Failed to create dialog: given type is not a dialog.");
}

void ShellState::RemoveDialog(Urho3D::StringHash type) { dialogs_.Erase(type); }

void ShellState::SetUpdate(bool update) { scene_.SetUpdateEnabled(update); }

void ShellState::OnActionUp(Urho3D::StringHash, Urho3D::VariantMap& eventData) {}

void ShellState::OnActionDown(Urho3D::StringHash, Urho3D::VariantMap& eventData) {}
