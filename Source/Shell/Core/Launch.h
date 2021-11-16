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

#ifndef LAUNCH_H
#define LAUNCH_H

#include <Urho3D/Core/Context.h>
#include <Urho3D/Core/ProcessUtils.h>
#include <Urho3D/Engine/Application.h>
#include "Urho3DShellAPI.h"

#define URHO3DSHELL_APPLICATION(CLASS)                                                                                 \
	int main(int argc, char** argv)                                                                                    \
	{                                                                                                                  \
		VariantMap shellParameters = ParseParameters(GetArguments());                                                  \
		SharedPtr<Context> context = MakeShared<Context>();                                                            \
		UniquePtr<CLASS> app = new CLASS(context);                                                                     \
		return app->Run();                                                                                             \
	}

#ifdef _WIN32
#define URHO3DSHELL_GPU_PARAMETERS                                                                                     \
	extern "C"                                                                                                         \
	{                                                                                                                  \
		__declspec(dllexport) unsigned NvOptimusEnablement = 1;                                                        \
		__declspec(dllexport) int AmdPowerXpressRequestHighPerformance = 1;                                            \
	}
#else
#define URHO3DSHELL_GPU_PARAMETERS
#endif // _WIN32

URHO3DSHELLAPI_EXPORT Urho3D::VariantMap ParseParameters(const Urho3D::StringVector& arguments);

// extern "C" int URHO3DSHELLAPI_EXPORT LaunchShell(int argc1, char** argv1, int argc2, char** argv2);

#endif // LAUNCH_H
