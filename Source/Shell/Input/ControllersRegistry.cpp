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
#include <Urho3D/Input/Controls.h>
#include <Urho3D/Resource/XMLFile.h>
#include "ControllersRegistry.h"
#include "InputRegistry.h"
#include "KeyboardController.h"

using namespace Urho3D;

ControllersRegistry::ControllersRegistry(Urho3D::Context* context)
	: Object(context)
{
	context_->RegisterFactory<KeyboardController>();
	RegisterController<KeyboardController>();
}

ControllersRegistry::~ControllersRegistry()
{
	for (const auto& p : enabledControllers_)
		DisableController(p.second_);
}

void ControllersRegistry::ReadControls(Urho3D::Controls& controls) const
{
	controls.Reset();
	for (const auto& p : enabledControllers_)
		p.second_->ReadControls(controls);
}

void ControllersRegistry::RegisterController(Urho3D::SharedPtr<InputController> controller)
{
	const StringHash type = controller->GetType();
	if (!enabledControllers_.Contains(type) && !disabledControllers_.Contains(type))
		disabledControllers_[type] = controller;
	else
		URHO3D_LOGERROR("Failed to register already registered input controller.");
}

void ControllersRegistry::RemoveController(Urho3D::StringHash controllerType)
{
	if (!enabledControllers_.Erase(controllerType))
		URHO3D_LOGERROR("Failed to remove non-registered input controller.");
}

void ControllersRegistry::EnableController(Urho3D::StringHash controllerType)
{
	auto it = disabledControllers_.Find(controllerType);
	if (it != disabledControllers_.End())
	{
		EnableController(it->second_);
		enabledControllers_.Insert(it);
		disabledControllers_.Erase(it);
	}
	else if (enabledControllers_.Contains(controllerType))
		URHO3D_LOGERRORF("Failed to enable already activated input controller.");
	else
		URHO3D_LOGERRORF("Failed to enable not registered input controller.");
}

void ControllersRegistry::EnableController(InputController* inputController)
{
	if (inputController->Enable())
	{
		XMLFile file(context_);
		if (file.LoadFile(configPath_ + inputController->GetTypeName() + ".xml"))
			inputController->LoadXML(file.GetRoot());
	}
	else
		URHO3D_LOGERROR("Failed to enable input controller.");
}

void ControllersRegistry::DisableController(Urho3D::StringHash controllerType)
{
	auto it = enabledControllers_.Find(controllerType);
	if (it != enabledControllers_.End())
	{
		DisableController(it->second_);
		disabledControllers_.Insert(it);
		enabledControllers_.Erase(it);
	}
	else if (disabledControllers_.Contains(controllerType))
		URHO3D_LOGERRORF("Failed to disable not activated input controller.");
	else
		URHO3D_LOGERRORF("Failed to disable not registered input controller.");
}

void ControllersRegistry::DisableController(InputController* inputController)
{
	if (inputController->Disable())
	{
		XMLFile file(context_);
		XMLElement root = file.CreateRoot("input");
		inputController->SaveXML(root);
		file.SaveFile(configPath_ + inputController->GetTypeName() + ".xml");
		inputController->RemoveAllBindings();
	}
	else
		URHO3D_LOGERROR("Failed to disable input controller.");
}

InputController* ControllersRegistry::GetController(Urho3D::StringHash controllerType) const
{
	auto it = enabledControllers_.Find(controllerType);
	if (it != enabledControllers_.End())
		return it->second_;
	it = disabledControllers_.Find(controllerType);
	if (it != disabledControllers_.End())
		return it->second_;
	return nullptr;
}

Urho3D::StringVector ControllersRegistry::GetEnabledControllers() const
{
	StringVector ret;
	ret.Reserve(enabledControllers_.Size());
	for (const auto& p : enabledControllers_)
		ret.Push(p.second_->GetTypeName());
	return ret;
}

void ControllersRegistry::RemoveAllControllers()
{
	enabledControllers_.Clear();
	disabledControllers_.Clear();
}

Urho3D::String ControllersRegistry::GetDebugString() const
{
	String ret;
	ret.Append("Enabled\n");
	for (const auto& p : enabledControllers_)
		ret.Append(p.second_->GetDebugString());
	ret.Append("Disabled\n");
	for (const auto& p : disabledControllers_)
		ret.Append(p.second_->GetDebugString());
	return ret;
}
