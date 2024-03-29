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

#ifndef BINARYPLUGIN_H
#define BINARYPLUGIN_H

#include "Plugin.h"

#ifdef _WIN32
#include <minwindef.h>
#endif // _WIN32

class BinaryPlugin : public Plugin
{
	URHO3D_OBJECT(BinaryPlugin, Plugin)

public:
	explicit BinaryPlugin(Urho3D::Context* context);
	~BinaryPlugin();

	bool Load(const Urho3D::String& fileName) override;

	const Urho3D::String& GetName() const override;

private:
#ifdef _WIN32
	using LibHandle = HMODULE;
#else
	using LibHandle = void*;
#endif // WIN32

	Urho3D::UniquePtr<PluginInterface> interface_;
	LibHandle library_;

public:
	static const char* GetExtension();
};

#endif // BINARYPLUGIN_H
