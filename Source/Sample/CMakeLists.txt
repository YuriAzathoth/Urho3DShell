#
# Copyright (c) 2021-2022 Yuriy Zinchenko.
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
# THE SOFTWARE.
#

IF (NOT CMAKE_PROJECT_NAME STREQUAL Urho3DShell)
	CMAKE_MINIMUM_REQUIRED (VERSION 3.2.3)
	PROJECT (SampleGame VERSION 0.0.0)
	FIND_PACKAGE (U3S REQUIRED)
	IF (COMMAND CMAKE_POLICY)
		# Honor visibility properties for all target types
		CMAKE_POLICY (SET CMP0063 NEW)
		# cmake_dependent_option() supports full Condition Syntax
		CMAKE_POLICY (SET CMP0127 NEW)
	ENDIF ()
ENDIF ()

SET (DEPENDENCY U3S)
SET (SDK_NAME SampleGame)

ADD_SUBDIRECTORY (Core)
ADD_SUBDIRECTORY (Client)

SETUP_LAUNCHERS ()
