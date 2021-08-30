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

#include <Urho3D/AngelScript/Generated_Members.h>
#include "ShellState/ClientState.h"
#include "ShellState/GameState.h"
#include "ShellState/LocalServerState.h"
#include "ShellState/MainMenuState.h"
#include "ShellState/RemoteServerState.h"
#include "ShellState/ServerState.h"

using namespace Urho3D;

#if defined(__GNUC__) || defined(__GNUG__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wconversion"
#endif // defined (__GNUC__) || defined(__GNUG__)

#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wconversion"
#endif // defined(__clang__)

template <typename T> void RegisterMembers_ShellState(asIScriptEngine* engine, const char* className)
{
	RegisterMembers_Object<T>(engine, className);
	engine->RegisterObjectMethod(className, "void Enter()", AS_METHOD(T, Enter), AS_CALL_THISCALL);
	engine->RegisterObjectMethod(className, "void Exit()", AS_METHOD(T, Exit), AS_CALL_THISCALL);
	engine->RegisterObjectMethod(className,
								 "void CreateDialog(StringHash)",
								 asMETHOD(T, CreateDialog),
								 AS_CALL_THISCALL);
	engine->RegisterObjectMethod(className,
								 "void RemoveDialog(StringHash)",
								 asMETHOD(T, RemoveDialog),
								 AS_CALL_THISCALL);
	engine->RegisterObjectMethod(className, "void RemoveAllDialogs()", asMETHOD(T, RemoveAllDialogs), AS_CALL_THISCALL);
	engine->RegisterObjectMethod(className, "uint32 get_closeables() const", asMETHOD(T, GetCloseables), AS_CALL_THISCALL);
	engine->RegisterObjectMethod(className, "uint32 get_interactives() const", asMETHOD(T, GetInteractives), AS_CALL_THISCALL);
}

#if defined(__GNUC__) || defined(__GNUG__)
#pragma GCC diagnostic pop
#endif // defined (__GNUC__) || defined(__GNUG__)

#if defined(__clang__)
#pragma clang diagnostic pop
#endif // defined(__clang__)

static ClientState* Create_ClientState(const String& address, unsigned short port)
{
	return new ClientState(GetScriptContext(), address, port);
}

static LocalServerState* Create_LocalServerState(const String& sceneName, unsigned short port)
{
	return new LocalServerState(GetScriptContext(), sceneName, port);
}

static MainMenuState* Create_MainMenuState() { return new MainMenuState(GetScriptContext()); }

static RemoteServerState*
Create_RemoteServerState(const String& sceneName, const String& serverName, unsigned short port)
{
	return new RemoteServerState(GetScriptContext(), sceneName, serverName, port);
}

static ServerState* Create_ServerState(const String& sceneName, unsigned short port)
{
	return new ServerState(GetScriptContext(), sceneName, port);
}

void RegisterShellStateAPI(asIScriptEngine* engine)
{
	engine->RegisterObjectType("ClientState", 0, asOBJ_REF);
	engine->RegisterObjectType("GameState", 0, asOBJ_REF);
	engine->RegisterObjectType("LocalServerState", 0, asOBJ_REF);
	engine->RegisterObjectType("MainMenuState", 0, asOBJ_REF);
	engine->RegisterObjectType("RemoteServerState", 0, asOBJ_REF);
	engine->RegisterObjectType("ServerState", 0, asOBJ_REF);
	engine->RegisterObjectType("ShellState", 0, asOBJ_REF);

	engine->RegisterObjectBehaviour("MainMenuState",
									asBEHAVE_FACTORY,
									"MainMenuState@+ f()",
									AS_FUNCTION(Create_MainMenuState),
									AS_CALL_CDECL);
	engine->RegisterObjectBehaviour("ClientState",
									asBEHAVE_FACTORY,
									"ClientState@+ f(const String&in, uint16)",
									AS_FUNCTION(Create_ClientState),
									AS_CALL_CDECL);
	engine->RegisterObjectBehaviour("LocalServerState",
									asBEHAVE_FACTORY,
									"LocalServerState@+ f(const String&in, uint16)",
									AS_FUNCTION(Create_LocalServerState),
									AS_CALL_CDECL);
	engine->RegisterObjectBehaviour("RemoteServerState",
									asBEHAVE_FACTORY,
									"RemoteServerState@+ f(const String&in, const String&in, uint16)",
									AS_FUNCTION(Create_RemoteServerState),
									AS_CALL_CDECL);
	engine->RegisterObjectBehaviour("ServerState",
									asBEHAVE_FACTORY,
									"ServerState@+ f(const String&in, uint16)",
									AS_FUNCTION(Create_ServerState),
									AS_CALL_CDECL);

	RegisterSubclass<Object, ShellState>(engine, "Object", "ShellState");
	RegisterSubclass<RefCounted, ShellState>(engine, "RefCounted", "ShellState");

	RegisterSubclass<ShellState, MainMenuState>(engine, "ShellState", "MainMenuState");
	RegisterSubclass<Object, MainMenuState>(engine, "Object", "MainMenuState");
	RegisterSubclass<RefCounted, MainMenuState>(engine, "RefCounted", "MainMenuState");

	RegisterSubclass<ShellState, GameState>(engine, "ShellState", "GameState");
	RegisterSubclass<Object, GameState>(engine, "Object", "GameState");
	RegisterSubclass<RefCounted, GameState>(engine, "RefCounted", "GameState");

	RegisterSubclass<GameState, ClientState>(engine, "GameState", "ClientState");
	RegisterSubclass<ShellState, ClientState>(engine, "ShellState", "ClientState");
	RegisterSubclass<Object, ClientState>(engine, "Object", "ClientState");
	RegisterSubclass<RefCounted, ClientState>(engine, "RefCounted", "ClientState");

	RegisterSubclass<GameState, ServerState>(engine, "GameState", "ServerState");
	RegisterSubclass<ShellState, ServerState>(engine, "ShellState", "ServerState");
	RegisterSubclass<Object, ServerState>(engine, "Object", "ServerState");
	RegisterSubclass<RefCounted, ServerState>(engine, "RefCounted", "ServerState");

	RegisterSubclass<ServerState, LocalServerState>(engine, "ServerState", "LocalServerState");
	RegisterSubclass<GameState, LocalServerState>(engine, "GameState", "LocalServerState");
	RegisterSubclass<ShellState, LocalServerState>(engine, "ShellState", "LocalServerState");
	RegisterSubclass<Object, LocalServerState>(engine, "Object", "LocalServerState");
	RegisterSubclass<RefCounted, LocalServerState>(engine, "RefCounted", "LocalServerState");

	RegisterSubclass<ServerState, RemoteServerState>(engine, "ServerState", "RemoteServerState");
	RegisterSubclass<GameState, RemoteServerState>(engine, "GameState", "RemoteServerState");
	RegisterSubclass<ShellState, RemoteServerState>(engine, "ShellState", "RemoteServerState");
	RegisterSubclass<Object, RemoteServerState>(engine, "Object", "RemoteServerState");
	RegisterSubclass<RefCounted, RemoteServerState>(engine, "RefCounted", "RemoteServerState");

	RegisterMembers_ShellState<ClientState>(engine, "ClientState");
	RegisterMembers_ShellState<GameState>(engine, "GameState");
	RegisterMembers_ShellState<LocalServerState>(engine, "LocalServerState");
	RegisterMembers_ShellState<MainMenuState>(engine, "MainMenuState");
	RegisterMembers_ShellState<RemoteServerState>(engine, "RemoteServerState");
	RegisterMembers_ShellState<ServerState>(engine, "ServerState");
	RegisterMembers_ShellState<ShellState>(engine, "ShellState");
}
