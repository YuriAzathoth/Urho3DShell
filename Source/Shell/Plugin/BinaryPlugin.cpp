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
#include <boost/dll/import.hpp>
#include <boost/function.hpp>
#include "BinaryPlugin.h"

using namespace Urho3D;

bool BinaryPlugin::Load(const Urho3D::String& fileName)
{
	const unsigned delimPos = fileName.FindLast('/');
	String dllPath;
	String dllName;
	if (delimPos != String::NPOS)
	{
		dllPath = fileName.Substring(0, delimPos).CString();
		dllName = fileName.Substring(delimPos);
	}
	else
		dllName = fileName;

	try
	{
		boost::dll::fs::path path(dllPath.CString());
#ifndef _NDEBUG
		dllName += "_d";
#endif // _NDEBUG
		path /= dllName.CString();
		plugin_.load(path, boost::dll::load_mode::append_decorations);
	}
	catch (boost::system::system_error& e)
	{
		URHO3D_LOGERRORF("Failed to load binary plugin \"%s\": %s.", dllName.CString(), e.what());
		return false;
	}

	using PluginFactory = Urho3D::UniquePtr<PluginInterface>(Urho3D::Context*);
	boost::function<PluginFactory> CreatePlugin;
	try
	{
		CreatePlugin = plugin_.get_alias<PluginFactory>("CreatePlugin");
	}
	catch (boost::system::system_error& e)
	{
		URHO3D_LOGERRORF("Failed to get function \"CreatePlugin\" address in plugin \"%s\": %s.",
						 fileName.CString(),
						 e.what());
		return false;
	}

	interface_ = CreatePlugin(context_);
	if (interface_.NotNull())
		return true;
	else
	{
		URHO3D_LOGERRORF("Failed to load binary plugin \"%s\": could not create plugin interface.", fileName.CString());
		return false;
	}
}

const Urho3D::String& BinaryPlugin::GetName() const { return interface_->GetName(); }
