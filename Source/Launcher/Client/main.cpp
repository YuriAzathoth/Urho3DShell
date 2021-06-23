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

#include <boost/dll/import.hpp>
#include <boost/function.hpp>

#define LIBRARY_FILENAME "Urho3DShell"
#define LAUNCH_FUNCTION "LaunchShell"

int main(int argc, char** argv)
{
	setlocale(LC_ALL, "");
	using LaunchFun = int(int, char**);
	boost::function<LaunchFun> launchFun;
	try
	{
		launchFun =
			boost::dll::import<LaunchFun>(LIBRARY_FILENAME, LAUNCH_FUNCTION, boost::dll::load_mode::append_decorations);
	}
	catch (boost::system::system_error& e)
	{
		fprintf(stderr, "An exception has occured: %s\n", e.what());
		return 1;
	}
	return launchFun(argc, argv);
}

#ifdef _WIN32
extern "C"
{
	__declspec(dllexport) unsigned NvOptimusEnablement = 1;
	__declspec(dllexport) int AmdPowerXpressRequestHighPerformance = 1;
}
#endif // _WIN32
