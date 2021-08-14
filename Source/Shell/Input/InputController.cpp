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

#include <Urho3D/Container/HashSet.h>
#include <Urho3D/Core/StringUtils.h>
#include <Urho3D/Resource/XMLElement.h>
#include "InputClient.h"
#include "InputController.h"
#include "InputEvents.h"
#include "InputRegistry.h"

using namespace Urho3D;

InputController::InputController(Urho3D::Context* context)
	: Object(context)
	, sensitivity_(0.25f)
{
}

bool InputController::LoadXML(const Urho3D::XMLElement& source)
{
	if (source.IsNull())
		return false;

	if (source.GetAttribute("controller") != GetTypeName())
		return false;

	XMLElement sensitivity = source.GetChild("sensitivity");
	if (!sensitivity.IsNull())
		SetSensitivity(sensitivity.GetFloat("value"));

	RemoveAllBindings();

	HashSet<StringHash> unboundActions_;
	for (const auto& p : defaultBindings_)
		unboundActions_.Insert(p.first_);

	InputRegistry* inputRegistry = GetSubsystem<InputRegistry>();
	StringHash action;
	unsigned keyCode;
	for (XMLElement binding = source.GetChild("bind"); !binding.IsNull(); binding = binding.GetNext())
	{
		keyCode = GetKeyCode(binding.GetAttribute("key"));
		action = binding.GetAttribute("action");
		bindings_[keyCode] = action;
		if (inputRegistry->IsRemote(action))
			remoteBindings_.Push(MakePair(keyCode, inputRegistry->GetActionFlag(action)));
		unboundActions_.Erase(action);
	}

	decltype(defaultBindings_)::ConstIterator it;
	for (StringHash p : unboundActions_)
	{
		it = defaultBindings_.Find(p);
		if (it != defaultBindings_.End())
			bindings_[it->second_] = it->first_;
	}

	return true;
}

void InputController::SaveXML(Urho3D::XMLElement& dst) const
{
	dst.SetAttribute("controller", GetTypeName());

	dst.CreateChild("sensitivity").SetFloat("value", GetSensitivity());

	InputRegistry* inputRegistry = GetSubsystem<InputRegistry>();
	const String* action;
	String keyName;
	XMLElement binding;
	for (const auto& p : bindings_)
	{
		action = &inputRegistry->GetActionName(p.second_);
		keyName = GetKeyName(p.first_);
		binding = dst.CreateChild("binding");
		binding.SetAttribute("key", keyName);
		binding.SetAttribute("action", *action);
	}
}

void InputController::SetBinding(Urho3D::StringHash action, unsigned keyCode)
{
	InputRegistry* inputRegistry = GetSubsystem<InputRegistry>();
	if (inputRegistry->IsRemote(action))
	{
		const unsigned actionFlag = inputRegistry->GetActionFlag(action);
		for (auto it = remoteBindings_.Begin(); it != remoteBindings_.End();)
			if (it->first_ == keyCode || it->second_ == actionFlag)
				it = remoteBindings_.Erase(it);
			else
				++it;
		remoteBindings_.Push(MakePair(keyCode, actionFlag));
	}
	for (auto it = bindings_.Begin(); it != bindings_.End(); ++it)
		if (it->second_ == action)
		{
			bindings_.Erase(it);
			break;
		}
	bindings_[keyCode] = action;
}

unsigned InputController::GetBindingKey(Urho3D::StringHash action) const
{
	for (auto it = bindings_.Begin(); it != bindings_.End(); ++it)
		if (it->second_ == action)
			return it->first_;
	return 0;
}

unsigned InputController::GetDefaultBinding(Urho3D::StringHash action) const
{
	const auto it = defaultBindings_.Find(action);
	return it != defaultBindings_.End() ? it->second_ : 0;
}

void InputController::EnableSelf() { GetSubsystem<InputClient>()->EnableController(GetType()); }
void InputController::DisableSelf() { GetSubsystem<InputClient>()->EnableController(GetType()); }

void InputController::SendActionDown(unsigned keyCode)
{
	const auto it = bindings_.Find(keyCode);
	if (it != bindings_.End())
	{
		using namespace ActionDown;
		VariantMap& eventData = GetEventDataMap();
		eventData[P_ACTION] = it->second_;
		SendEvent(E_ACTIONDOWN, eventData);
	}
}

void InputController::SendActionUp(unsigned keyCode)
{
	const auto it = bindings_.Find(keyCode);
	if (it != bindings_.End())
	{
		using namespace ActionDown;
		VariantMap& eventData = GetEventDataMap();
		eventData[P_ACTION] = it->second_;
		SendEvent(E_ACTIONUP, eventData);
	}
}

Urho3D::String InputController::GetDebugString() const
{
	InputRegistry* inputRegistry = GetSubsystem<InputRegistry>();
	String ret;
	ret.Append("Name: ").Append(GetTypeName()).Append("\n");
	ret.Append("\tSensitivity = ").Append(ToString("%f", GetSensitivity())).Append("\n");
	ret.Append("\tDefault bindings:\n");
	for (const auto& p : GetDefaultBindings())
	{
		ret.Append("\t\tAction: ").Append(inputRegistry->GetActionName(p.first_)).Append("\n");
		ret.Append("\t\tKey:    ").Append(GetKeyName(p.second_)).Append("\n");
	}
	ret.Append("\tBindings:\n");
	for (const auto& p : GetBindings())
	{
		ret.Append("\t\tAction: ").Append(inputRegistry->GetActionName(p.second_)).Append("\n");
		ret.Append("\t\tKey:    ").Append(GetKeyName(p.first_)).Append("\n");
	}
	return ret;
}
