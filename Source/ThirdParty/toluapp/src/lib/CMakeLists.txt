#
# Copyright (c) 2008-2021 the Urho3D project.
# Copyright (c) 2021 Yuriy Zinchenko.
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

# Define target name
set (TARGET_NAME toluapp)

# Define source files
define_source_files (GLOB_CPP_PATTERNS *.c)

# Setup target
setup_library ()

# Install headers for building and using the Urho3D library (no direct dependencies but library user may need them)
install (DIRECTORY ../../include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/ThirdParty/toluapp FILES_MATCHING PATTERN *.h)  # Note: the trailing slash is significant

# Install and export package
target_include_directories (${TARGET_NAME} PUBLIC
	 "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../../include>"
	 "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../../../Lua${JIT}/src>"
	 "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/ThirdParty/toluapp>"
)
install (TARGETS ${TARGET_NAME} EXPORT ${TARGET_NAME}Targets)
export (TARGETS ${TARGET_NAME} FILE ${TARGET_NAME}Targets.cmake)
install (EXPORT ${TARGET_NAME}Targets FILE ${TARGET_NAME}Targets.cmake DESTINATION ${CMAKE_CONFIG_OUTPUT_DIRECTORY})
