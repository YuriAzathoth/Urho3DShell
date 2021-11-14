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
#include <Urho3D/Core/ProcessUtils.h>
#include "CoreApplication.h"
#include "FrontApplication.h"
#include "Launch.h"
#include "ShellDefs.h"

using namespace Urho3D;

static void JoinArgs(char** dst, int argc1, char** argv1, int argc2, char** argv2)
{
	char** src_ptr;
	char** dst_ptr = dst;
	char** src_ptr_end = argv1 + argc1;
	for (src_ptr = argv1; src_ptr < src_ptr_end; ++src_ptr, ++dst_ptr)
		*dst_ptr = *src_ptr;
	src_ptr_end = argv2 + argc2;
	for (src_ptr = argv2; src_ptr < src_ptr_end; ++src_ptr, ++dst_ptr)
		*dst_ptr = *src_ptr;
}

static Urho3D::VariantMap ParseParameters(const Urho3D::StringVector& arguments)
{
	VariantMap ret;
	String argument, value;
	for (unsigned i = 0; i < arguments.Size(); ++i)
		if (arguments[i].Length() > 1 && arguments[i][0] == '-')
		{
			argument = arguments[i].Substring(1).ToLower();
			value = i + 1 < arguments.Size() ? arguments[i + 1] : String::EMPTY;
			if (argument == "appname")
			{
				ret[SP_APP_NAME] = value;
				++i;
			}
			else if (argument == "client")
			{
				ret[SP_CLIENT] = value;
				++i;
			}
			else if (argument == "headless" || argument == "noclient")
				ret[SP_NO_CLIENT] = true;
			else if (argument == "scene")
			{
				ret[SP_SCENE] = value;
				++i;
			}
			else if (argument == "script")
			{
				ret[SP_SCRIPT] = value;
				++i;
			}
			else if (argument == "server")
			{
				if (value.Empty() || value[0] == '-')
					ret[SP_SERVER] = String::EMPTY;
				else
				{
					ret[SP_SERVER] = value;
					++i;
				}
			}
		}
	return ret;
}

extern "C" int LaunchShell(int argc1, char** argv1, int argc2, char** argv2)
{
	{
		const int argc = argc1 + argc2;
		char** argv = new char*[argc];
		JoinArgs(argv, argc1, argv1, argc2, argv2);
		ParseArguments(argc, argv);
		delete[] argv;
	}

	VariantMap shellParameters = ParseParameters(GetArguments());
	SharedPtr<Context> context = MakeShared<Context>();
	UniquePtr<CoreApplication> app;
	if (shellParameters.Contains(SP_NO_CLIENT))
		app = new CoreApplication(context, std::move(shellParameters));
	else
		app = new FrontApplication(context, std::move(shellParameters));
	return app->Run();
}
