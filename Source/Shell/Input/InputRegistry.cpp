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

#include <Urho3D/IO/Log.h>
#include "InputRegistry.h"

using namespace Urho3D;

static constexpr unsigned LAST_REMOTE_ACTION = 1 << 31;

InputRegistry::InputRegistry(Urho3D::Context* context)
	: Object(context)
	, lastRemoteFlag_(1)
{
}

void InputRegistry::RegisterActionLocal(const Urho3D::String& actionName)
{
	ordered_.Push(actionName);
	names_[actionName] = actionName;
}

unsigned InputRegistry::RegisterActionRemote(const Urho3D::String& actionName)
{
	if (lastRemoteFlag_ < LAST_REMOTE_ACTION)
	{
		const StringHash action = actionName;
		const unsigned flag = lastRemoteFlag_;
		ordered_.Push(action);
		names_[action] = actionName;
		remoteFlags_[action] = flag;
		lastRemoteFlag_ <<= 1;
		return flag;
	}
	else
	{
		URHO3D_LOGERRORF("Failed to register remote action %s: maximum allowed remote actions is 32.",
						 actionName.CString());
		return 0;
	}
}

void InputRegistry::RemoveAction(Urho3D::StringHash action)
{
	auto it = ordered_.Find(action);
	if (it != ordered_.End())
		ordered_.Erase(it);
	names_.Erase(action);
	remoteFlags_.Erase(action);
}

void InputRegistry::RemoveAllActions()
{
	ordered_.Clear();
	names_.Clear();
	remoteFlags_.Clear();
}

unsigned InputRegistry::GetActionFlag(Urho3D::StringHash action) const
{
	const auto it = remoteFlags_.Find(action);
	return it != remoteFlags_.End() ? it->second_ : 0;
}

const Urho3D::String& InputRegistry::GetActionName(Urho3D::StringHash action) const
{
	const auto it = names_.Find(action);
	return it != names_.End() ? it->second_ : String::EMPTY;
}

Urho3D::String InputRegistry::GetDebugString() const
{
	String ret;
	Urho3D::HashMap<Urho3D::StringHash, unsigned>::ConstIterator it;
	for (const StringHash action : ordered_)
	{
		ret.Append("Name: ").Append(*names_[action]).Append("\n");
		it = remoteFlags_.Find(action);
		if (it == remoteFlags_.End())
			ret.Append("\tType: Local\n");
		else
		{
			ret.Append("\tType: Remote\n");
			ret.Append(ToString("\tFlag: %u\n", it->second_));
		}
	}
	return ret;
}
