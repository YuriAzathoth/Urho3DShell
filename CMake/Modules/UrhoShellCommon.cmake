#
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

MACRO (SETUP_GAME_PROJECT)
	IF (COMMAND CMAKE_POLICY)
		# Honor visibility properties for all target types
		CMAKE_POLICY (SET CMP0063 NEW)
		# Find_package uses <PackageName>_ROOT warning
		CMAKE_POLICY (SET CMP0074 NEW) 
	ENDIF ()

	SET (CMAKE_CODEBLOCKS_EXCLUDE_EXTERNAL_FILES ON)

	SET (CMAKE_INSTALL_INCLUDEDIR include/${CMAKE_PROJECT_NAME})
	SET (CMAKE_LIBRARY_OUTPUT_DIRECTORY lib)
	SET (CMAKE_RUNTIME_OUTPUT_DIRECTORY bin)
	SET (CMAKE_CONFIG_OUTPUT_DIRECTORY
		${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/cmake/${CMAKE_PROJECT_NAME}
	)
	
	# Dropped lib prefix on MINGW to avoid library names inconsistency
	IF (WIN32)
		SET (CMAKE_SHARED_LIBRARY_PREFIX "")
	ENDIF ()

	OPTION (URHO3DSHELL_LTO "Enable linking-time optimisations" OFF)
	OPTION (URHO3DSHELL_WARNINGS "Enable project warnings" OFF)
	MARK_AS_ADVANCED(URHO3DSHELL_LTO)
	MARK_AS_ADVANCED(URHO3DSHELL_WARNINGS)

	FIND_PACKAGE (Boost REQUIRED COMPONENTS system filesystem)

	INCLUDE (CMakePackageConfigHelpers REQUIRED)
	INCLUDE (GenerateExportHeader REQUIRED)
	INCLUDE (UrhoCommon REQUIRED)
	IF (PROJECT_LTO)
		INCLUDE (CheckIPOSupported REQUIRED)
	ENDIF ()

	ADD_SUBDIRECTORY (Source)

	IF (${URHO3D_PACKAGING})
		FILE (GLOB RESOURCE_PACKAGES "${CMAKE_BINARY_DIR}/bin/*.pak")
		FOREACH (PACKAGE ${RESOURCE_PACKAGES})
			INSTALL (FILES "${PACKAGE}" DESTINATION bin)
		ENDFOREACH ()
	ENDIF ()

	FIND_PROGRAM (CLANG_FORMAT "clang-format")
	IF (CLANG_FORMAT)
		FILE (GLOB_RECURSE ALL_SOURCE_FILES ${PROJECT_SOURCE_DIR}/*.cpp)
		FILE (GLOB_RECURSE ALL_HEADER_FILES ${PROJECT_SOURCE_DIR}/*.h)
		ADD_CUSTOM_TARGET (
			FORMAT
			COMMAND clang-format
			-i
			-style=file
			${ALL_SOURCE_FILES}
			${ALL_HEADER_FILES}
		)
	ENDIF ()
	
	SET (CMAKE_MODULES_PATH "${CMAKE_SOURCE_DIR}/CMake/Modules")
	IF (EXISTS ${CMAKE_MODULES_PATH})
		INSTALL (
			DIRECTORY "${CMAKE_MODULES_PATH}/"
			DESTINATION ${CMAKE_CONFIG_OUTPUT_DIRECTORY}
		)
	ENDIF ()
ENDMACRO ()

MACRO (SETUP_GAME_LAUNCHER)
	CMAKE_PARSE_ARGUMENTS (ARG "HEADLESS" "NAME" "ARGUMENTS;RESOURCES;AUTOLOAD" ${ARGN})
	
	SET (TARGET_NAME ${ARG_NAME})

	SET (QUOTED_ARGUMENTS "\"-appname\";\"${TARGET_NAME}\"")
	FOREACH (ARGUMENT ${ARG_ARGUMENTS})
		LIST (APPEND QUOTED_ARGUMENTS "\"${ARGUMENT}\"")
	ENDFOREACH ()

	IF (ARG_RESOURCES)
		SET (RESOURCES_LIST "")
		FOREACH (RESOURCE ${ARG_RESOURCES})
			LIST (APPEND RESOURCES_LIST ${RESOURCE})
		ENDFOREACH ()
		LIST (JOIN RESOURCES_LIST "|" SEMICOLONED_RESOURCES)
		LIST (APPEND QUOTED_ARGUMENTS "\"-p\"")
		LIST (APPEND QUOTED_ARGUMENTS "\"${SEMICOLONED_RESOURCES}\"")
	ENDIF ()

	IF (ARG_AUTOLOAD)
		SET (RESOURCES_LIST "")
		FOREACH (RESOURCE ${ARG_AUTOLOAD})
			LIST (APPEND RESOURCES_LIST ${RESOURCE})
		ENDFOREACH ()
		LIST (JOIN RESOURCES_LIST "|" SEMICOLONED_RESOURCES)
		LIST (APPEND QUOTED_ARGUMENTS "\"-ap\"")
		LIST (APPEND QUOTED_ARGUMENTS "\"${SEMICOLONED_RESOURCES}\"")
	ENDIF ()

	LIST (JOIN QUOTED_ARGUMENTS ",\n\t\t\t" LAUNCHER_ARGUMENTS_RAW)

	# Unwanted semicolon replacement workaround
	STRING (REPLACE "|" ";" LAUNCHER_ARGUMENTS ${LAUNCHER_ARGUMENTS_RAW})

	IF (WIN32 AND NOT ARG_HEADLESS AND NOT ${CMAKE_BUILD_TYPE} STREQUAL "Debug")
		SET (APP_MODIFIERS WIN32)
	ENDIF ()

	SET (SOURCE_FILE "${CMAKE_CURRENT_BINARY_DIR}/${TARGET_NAME}.cpp")
	CONFIGURE_FILE ("${CMAKE_SOURCE_DIR}/Source/Launcher/Launcher.cpp.in" ${SOURCE_FILE})
	ADD_EXECUTABLE (${TARGET_NAME} ${APP_MODIFIERS} ${SOURCE_FILE})
	TARGET_LINK_LIBRARIES (${TARGET_NAME} PRIVATE Boost::system Boost::filesystem)
	APPLY_TARGET_SETTINGS ()
	ADD_DEPENDENCIES (${TARGET_NAME} Urho3DShell)
	INSTALL (TARGETS ${TARGET_NAME})
ENDMACRO ()

MACRO (SETUP_GAME_PLUGIN)
	LIST (APPEND LIBS ${URHO3DSHELL_LIBRARIES})

	SETUP_GAME_LIBRARY (${ARGN})

	# Copy launchers
	FILE (GLOB LAUNCHERS ${URHO3DSHELL_RUNTIME_DIR}/*.exe)
	FILE (COPY ${LAUNCHERS} DESTINATION bin)
	INSTALL (FILES ${LAUNCHERS} DESTINATION bin)
	
	# Copy resource packages
	SET (PACKAGES "")
	FOREACH (PAK_FILE ${URHO3DSHELL_PACKAGES})
		LIST (APPEND PACKAGES ${URHO3DSHELL_RUNTIME_DIR}${PAK_FILE})
	ENDFOREACH ()
	FILE (COPY ${PACKAGES} DESTINATION bin)
	INSTALL (FILES ${PACKAGES} DESTINATION bin)
	
	# Copy resource autoload paths
	FOREACH (RES_PATH ${URHO3DSHELL_RESOURCES})
		FILE (GLOB_RECURSE RESOURCE_FILES
			  ${URHO3DSHELL_RUNTIME_DIR}${RES_PATH}/*.*
		)
		FILE (COPY ${RESOURCE_FILES} DESTINATION bin/${RES_PATH})
		INSTALL (FILES ${RESOURCE_FILES} DESTINATION bin/${RES_PATH})
	ENDFOREACH ()

	# Copy underlying SO/DLL
	FOREACH (LIB_NAME ${URHO3DSHELL_LIBRARIES})
		GET_TARGET_PROPERTY (LIBRARY_CONFIG ${LIB_NAME}
							 IMPORTED_CONFIGURATIONS)
		GET_TARGET_PROPERTY (LIBRARY_FILENAME ${LIB_NAME}
							 IMPORTED_LOCATION_${LIBRARY_CONFIG})
		FILE (COPY ${LIBRARY_FILENAME} DESTINATION bin)
		INSTALL (FILES ${LIBRARY_FILENAME} DESTINATION bin)
	ENDFOREACH ()
ENDMACRO ()

MACRO (SETUP_GAME_LIBRARY)
	CMAKE_PARSE_ARGUMENTS (ARG "" "NAME" "RESOURCES" ${ARGN})
	
	SET (TARGET_NAME ${ARG_NAME})

	LIST (APPEND LIBS Boost::system Boost::filesystem)
	
	DEFINE_SOURCE_FILES (RECURSE)
	SETUP_LIBRARY (SHARED)
	APPLY_TARGET_SETTINGS ()

	TARGET_INCLUDE_DIRECTORIES (${TARGET_NAME} PUBLIC
		$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
		$<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
		$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
	)

	# Basic library install
	INSTALL	(TARGETS ${TARGET_NAME} EXPORT ${TARGET_NAME})

	# Headers install
	STRING (REGEX REPLACE "[A-Za-z/]*API.h" ""
			PUBLIC_H_FILES "${H_FILES}")
	STRING (REGEX REPLACE "[A-Za-z/]*Impl.h" ""
			PUBLIC_H_FILES "${PUBLIC_H_FILES}")
	FOREACH (PUBLIC_HEADER ${PUBLIC_H_FILES})
		STRING (REGEX MATCH "(.*)[/\\]" HEADER_PATH ${PUBLIC_HEADER})
		INSTALL (FILES ${PUBLIC_HEADER} DESTINATION
				 ${CMAKE_INSTALL_INCLUDEDIR}/${HEADER_PATH})
	ENDFOREACH ()
	
	# Dependencies list building to use in package 
	LIST (APPEND URHO3DSHELL_DEPENDENCIES ${TARGET_NAME})

	# Export header
	SET (TARGET_EXPORT_HEADER "${TARGET_NAME}API.h")

	GENERATE_EXPORT_HEADER (${TARGET_NAME}
		BASE_NAME "${TARGET_NAME}API"
		EXPORT_FILE_NAME ${TARGET_EXPORT_HEADER}
	)

	INSTALL_HEADER_FILES (
		FILES ${CMAKE_CURRENT_BINARY_DIR}/${TARGET_EXPORT_HEADER}
		DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
		ACCUMULATE GENERATED_HEADERS
	)

	# Copy/install game resources
	IF (ARG_RESOURCES)
		_SETUP_GAME_RESOURCES (${ARG_RESOURCES})
	ENDIF ()

	# Package configuration
	SET (TARGET_CONFIG_FILE "${TARGET_NAME}Config.cmake")
	SET (TARGET_EXPORT_FILE "${TARGET_NAME}Targets.cmake")
	SET (TARGET_VERSION_FILE "${TARGET_NAME}ConfigVersion.cmake")

	SET (CONFIG_INSTALL_DIR ${CMAKE_CONFIG_OUTPUT_DIRECTORY}/)
	SET (RUNTIME_INSTALL_DIR bin/)
	
	IF (NOT URHO3DSHELL_CONFIG_DIR)
		SET (TARGET_SHELL_SOURCE ${CMAKE_SOURCE_DIR}/CMake/Modules/UrhoShellConfig.cmake.in)
	ELSE ()
		SET (TARGET_SHELL_SOURCE ${URHO3DSHELL_CONFIG_DIR}/UrhoShellConfig.cmake.in)
	ENDIF ()

	EXPORT (TARGETS ${TARGET_NAME} FILE ${TARGET_EXPORT_FILE})

	INSTALL (EXPORT ${TARGET_NAME}
		FILE ${TARGET_EXPORT_FILE}
		DESTINATION ${CMAKE_CONFIG_OUTPUT_DIRECTORY}
		EXPORT_LINK_INTERFACE_LIBRARIES
	)

	CONFIGURE_PACKAGE_CONFIG_FILE (
		"${TARGET_SHELL_SOURCE}"
		"${CMAKE_CURRENT_BINARY_DIR}/${TARGET_CONFIG_FILE}"
		INSTALL_DESTINATION ${CMAKE_CONFIG_OUTPUT_DIRECTORY}
		PATH_VARS CONFIG_INSTALL_DIR RUNTIME_INSTALL_DIR
	)

	WRITE_BASIC_PACKAGE_VERSION_FILE (
		"${CMAKE_CURRENT_BINARY_DIR}/${TARGET_VERSION_FILE}"
		VERSION ${PROJECT_VERSION}
		COMPATIBILITY SameMajorVersion
	)

	INSTALL (FILES
		"${CMAKE_CURRENT_BINARY_DIR}/${TARGET_CONFIG_FILE}"
		"${CMAKE_CURRENT_BINARY_DIR}/${TARGET_VERSION_FILE}"
		DESTINATION ${CMAKE_CONFIG_OUTPUT_DIRECTORY}
	)

	# Install Urho3D DLL file
	IF (WIN32 AND URHO3D_LIB_TYPE STREQUAL SHARED)
		FOREACH (DLL ${URHO3D_DLL})
			ADD_CUSTOM_COMMAND (
				TARGET ${TARGET_NAME}
				POST_BUILD COMMAND
				${CMAKE_COMMAND} -E copy_if_different
				"${DLL}" "${CMAKE_BINARY_DIR}/bin"
			)
			ADD_MAKE_CLEAN_FILES (${RUNTIME_DIR}/${DLL})
		ENDFOREACH ()
		INSTALL (FILES ${URHO3D_DLL} DESTINATION bin)
	ENDIF ()
ENDMACRO ()

MACRO (APPLY_TARGET_SETTINGS)
	SET_TARGET_PROPERTIES (${TARGET_NAME} PROPERTIES DEBUG_POSTFIX _d)

	# Useful warnings
	IF (URHO3DSHELL_WARNINGS)
		TARGET_COMPILE_OPTIONS (${TARGET_NAME} PRIVATE
			$<$<CXX_COMPILER_ID:MSVC>:/W4 /WX>
			$<$<NOT:$<CXX_COMPILER_ID:MSVC>>:
				-Wall -Wextra -Wpedantic -Werror -Wnon-virtual-dtor
				-Woverloaded-virtual -Wsign-promo
				-Wshadow=compatible-local -Wconversion
				-Wzero-as-null-pointer-constant -Wextra-semi>
		)
	ENDIF ()

	# Link time optimisations
	IF (URHO3DSHELL_LTO)
		CHECK_IPO_SUPPORTED (RESULT LTO_SUPPORTED)
		IF (LTO_SUPPORTED)
			SET_PROPERTY (
				TARGET ${TARGET_NAME} PROPERTY
				INTERPROCEDURAL_OPTIMIZATION TRUE
			)
		ENDIF ()
	ENDIF ()
ENDMACRO ()

# Original code of this macro from Urho3D
MACRO (_SETUP_GAME_RESOURCES)
	IF (NOT TARGET_NAME)
		MESSAGE (FATAL_ERROR "_SETUP_GAME_RESOURCES must be called after building target is declared.")
	ENDIF ()

	FOREACH (DIR ${ARGV})
		IF (EXISTS "${CMAKE_SOURCE_DIR}/Assets/${DIR}")
			LIST (APPEND RESOURCE_DIRS ${CMAKE_SOURCE_DIR}/Assets/${DIR})
		ENDIF ()
	ENDFOREACH ()

	IF (URHO3D_PACKAGING AND RESOURCE_DIRS)
		FIND_URHO3D_TOOL (PACKAGE_TOOL PackageTool
			HINTS ${CMAKE_BINARY_DIR}/bin/tool ${URHO3D_HOME}/bin/tool
			DOC "Path to PackageTool" MSG_MODE WARNING)
	
		FOREACH (DIR ${RESOURCE_DIRS})
			GET_FILENAME_COMPONENT (NAME ${DIR} NAME)
			IF (ANDROID)
				SET (RESOURCE_${DIR}_PATHNAME ${CMAKE_BINARY_DIR}/assets/${NAME}.pak)
			ELSE ()
				SET (RESOURCE_${DIR}_PATHNAME ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${NAME}.pak)
			ENDIF ()
			LIST (APPEND RESOURCE_PAKS ${RESOURCE_${DIR}_PATHNAME})
			LIST (APPEND URHO3DSHELL_PACKAGES ${NAME}.pak)

			STRING (MD5 MD5 ${DIR})
			SET (MD5ALL ${MD5ALL}${MD5})
			IF (WIN32)
				SET (PACKAGING_COMMAND && echo Packaging ${DIR}... && ${PACKAGE_TOOL} ${DIR} ${RESOURCE_${DIR}_PATHNAME}.new -c -q && ${CMAKE_COMMAND} -E copy_if_different ${RESOURCE_${DIR}_PATHNAME}.new ${RESOURCE_${DIR}_PATHNAME} && ${CMAKE_COMMAND} -E remove ${RESOURCE_${DIR}_PATHNAME}.new)
				LIST (APPEND COMMANDS COMMAND ${CMAKE_COMMAND} -E touch ${DIR} ${PACKAGING_COMMAND})
			ELSE ()
				SET (PACKAGING_COMMAND && echo Packaging ${DIR}... && ${PACKAGE_TOOL} ${DIR} ${RESOURCE_${DIR}_PATHNAME} -c -q)
				SET (OUTPUT_COMMAND test -e ${RESOURCE_${DIR}_PATHNAME} || \( true ${PACKAGING_COMMAND} \))
				LIST (APPEND COMMANDS COMMAND echo Checking ${DIR}... && bash -c \"\(\( `find ${DIR} -newer ${DIR} |wc -l` \)\)\" && touch -cm ${DIR} ${PACKAGING_COMMAND} || ${OUTPUT_COMMAND})
			ENDIF ()
			ADD_MAKE_CLEAN_FILES (${RESOURCE_${DIR}_PATHNAME})
			STRING (MD5 MD5ALL ${MD5ALL})
		ENDFOREACH ()
		
		FOREACH (PAK ${RESOURCE_PAKS})
			INSTALL (FILES ${PAK} DESTINATION bin)
		ENDFOREACH ()
		
		IF (NOT RESOURCE_CHECK_${MD5ALL})
			SET (RESOURCE_CHECK RESOURCE_CHECK)
			WHILE (TARGET ${RESOURCE_CHECK})
				STRING (RANDOM RANDOM)
				SET (RESOURCE_CHECK RESOURCE_CHECK_${RANDOM})
			ENDWHILE ()
			SET (RESOURCE_CHECK_${MD5ALL} ${RESOURCE_CHECK} CACHE INTERNAL "Resource check hash map")
		ENDIF ()
		IF (NOT TARGET ${RESOURCE_CHECK_${MD5ALL}})
			ADD_CUSTOM_TARGET (${RESOURCE_CHECK_${MD5ALL}} ALL ${COMMANDS} ${PACKAGING_DEP} COMMENT "Checking${PACKAGING_COMMENT} resource directories")
		ENDIF ()
		ADD_DEPENDENCIES (${TARGET_NAME} ${RESOURCE_CHECK_${MD5ALL}})
	ELSE ()
		FOREACH (DIR ${RESOURCE_DIRS})
			STRING (FIND ${DIR} "/" DELIM_POS REVERSE)
			MATH (EXPR DELIM_POS "${DELIM_POS} + 1")
			STRING (SUBSTRING ${DIR} ${DELIM_POS} -1 NAME)
			FILE (GLOB_RECURSE RESOURCE_FILES "${DIR}/*")
			FILE (COPY ${RESOURCE_FILES} DESTINATION ${CMAKE_BINARY_DIR}/bin/${NAME})
			INSTALL (DIRECTORY ${DIR} DESTINATION bin)
			LIST (APPEND URHO3DSHELL_RESOURCES ${NAME})
		ENDFOREACH ()
	ENDIF ()
ENDMACRO ()
