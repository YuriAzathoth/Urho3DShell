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

#include <Urho3D/IO/FileSystem.h>
#include <Urho3D/IO/Log.h>
#include <functional>
#include "BinaryPlugin.h"

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#define LIBRARY_HANDLER_NULL nullptr
#else
#include <dlfcn.h>
#define LIBRARY_HANDLER_NULL 0
#endif // _WIN32

using namespace Urho3D;

BinaryPlugin::BinaryPlugin(Urho3D::Context* context)
	: Plugin(context)
	, library_(LIBRARY_HANDLER_NULL)
{
}

BinaryPlugin::~BinaryPlugin()
{
	interface_.Reset();
#ifdef _WIN32
	FreeLibrary(library_);
#else
	dlclose(library_);
#endif // _WIN32
}

bool BinaryPlugin::Load(const Urho3D::String& fileName)
{
	String dllName = fileName;
#ifndef NDEBUG
	dllName += "_d";
#endif // NDEBUG
#if defined (_WIN32)
	dllName += ".dll";
#elif defined(EMSCRIPTEN)
	dllName += ".wasm";
#else
	dllName += ".so";
#endif

	FileSystem* fs = GetSubsystem<FileSystem>();
	if (!fs->FileExists(fs->GetProgramDir() + dllName))
		return true;

	using PluginFactory = Urho3D::UniquePtr<PluginInterface>(Urho3D::Context*);
	std::function<PluginFactory> CreatePlugin;

#ifdef _WIN32
	library_ = LoadLibrary(dllName.CString());
	if (!library_)
	{
		char* message;
		FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
					  NULL,
					  GetLastError(),
					  MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
					  (LPTSTR)&message,
					  0,
					  nullptr);
		URHO3D_LOGERRORF("Failed to load binary plugin \"%s\": %s.", dllName.CString(), message);
		LocalFree(message);
		return false;
	}

	// Potential risky casting to suppress function type casting errors
	CreatePlugin = reinterpret_cast<PluginFactory*>(reinterpret_cast<void*>(GetProcAddress(library_, "CreatePlugin")));
	if (!CreatePlugin)
	{
		char* message;
		FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
					  NULL,
					  GetLastError(),
					  MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
					  (LPTSTR)&message,
					  0,
					  nullptr);
		URHO3D_LOGERRORF("Failed to get function \"CreatePlugin\" address in plugin \"%s\": %s.",
						 dllName.CString(),
						 message);
		LocalFree(message);
		return false;
	}
#else
	library_ = dlopen(dllName.CString(), RTLD_LAZY);
	if (!library_)
	{
		URHO3D_LOGERRORF("Failed to load binary plugin \"%s\": %s.", dllName.CString(), dlerror());
		return false;
	}
	dlerror();

	CreatePlugin = reinterpret_cast<PluginFactory*>(dlsym(library_, "CreatePlugin"));
	const char* error = dlerror();
	if (error != nullptr)
	{
		URHO3D_LOGERRORF("Failed to get function \"CreatePlugin\" address in plugin \"%s\": %s.",
						 dllName.CString(),
						 error);
		return false;
	}
#endif // _WIN32

	interface_ = CreatePlugin(context_);
	if (interface_.NotNull())
		return true;
	else
	{
		URHO3D_LOGERRORF("Failed to load binary plugin \"%s\": could not create plugin interface.", dllName.CString());
		return false;
	}
}

const Urho3D::String& BinaryPlugin::GetName() const { return interface_->GetName(); }
