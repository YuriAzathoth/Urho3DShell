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

#include <Urho3D/AngelScript/Generated_Members.h>
#include "FrontState/ClientState.h"
#include "FrontState/GameState.h"
#include "FrontState/LocalServerState.h"
#include "FrontState/MainMenuState.h"
#include "FrontState/RemoteServerState.h"
#include "FrontState/ServerState.h"

using namespace Urho3D;

template <typename T> void RegisterMembers_FrontState(asIScriptEngine* engine, const char* className)
{
	RegisterMembers_Object<T>(engine, className);
	engine->RegisterObjectMethod(className, "void Enter()", AS_METHOD(T, Enter), AS_CALL_THISCALL);
	engine->RegisterObjectMethod(className, "void Exit()", AS_METHOD(T, Exit), AS_CALL_THISCALL);
	engine->RegisterObjectMethod(className,
								 "Dialog@+ GetDialog(StringHash) const",
								 AS_METHODPR(T, GetDialog, (StringHash) const, Dialog*),
								 AS_CALL_THISCALL);
	engine->RegisterObjectMethod(className,
								 "Dialog@+ CreateDialog(StringHash)",
								 AS_METHODPR(T, CreateDialog, (StringHash), Dialog*),
								 AS_CALL_THISCALL);
	engine->RegisterObjectMethod(className,
								 "void RemoveDialog(StringHash)",
								 AS_METHODPR(T, RemoveDialog, (StringHash), void),
								 AS_CALL_THISCALL);
	engine->RegisterObjectMethod(className, "void RemoveAllDialogs()", asMETHOD(T, RemoveAllDialogs), AS_CALL_THISCALL);
	engine->RegisterObjectMethod(className,
								 "uint32 get_closeables() const",
								 asMETHOD(T, GetCloseables),
								 AS_CALL_THISCALL);
	engine->RegisterObjectMethod(className,
								 "uint32 get_interactives() const",
								 asMETHOD(T, GetInteractives),
								 AS_CALL_THISCALL);
}

extern void RegisterDialogAPI(asIScriptEngine* engine);

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

void RegisterFrontStateAPI(asIScriptEngine* engine)
{
	RegisterDialogAPI(engine);

	engine->RegisterObjectType("ClientState", 0, asOBJ_REF);
	engine->RegisterObjectType("GameState", 0, asOBJ_REF);
	engine->RegisterObjectType("LocalServerState", 0, asOBJ_REF);
	engine->RegisterObjectType("MainMenuState", 0, asOBJ_REF);
	engine->RegisterObjectType("RemoteServerState", 0, asOBJ_REF);
	engine->RegisterObjectType("ServerState", 0, asOBJ_REF);
	engine->RegisterObjectType("FrontState", 0, asOBJ_REF);

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

	RegisterSubclass<Object, FrontState>(engine, "Object", "FrontState");
	RegisterSubclass<RefCounted, FrontState>(engine, "RefCounted", "FrontState");

	RegisterSubclass<FrontState, MainMenuState>(engine, "FrontState", "MainMenuState");
	RegisterSubclass<Object, MainMenuState>(engine, "Object", "MainMenuState");
	RegisterSubclass<RefCounted, MainMenuState>(engine, "RefCounted", "MainMenuState");

	RegisterSubclass<FrontState, GameState>(engine, "FrontState", "GameState");
	RegisterSubclass<Object, GameState>(engine, "Object", "GameState");
	RegisterSubclass<RefCounted, GameState>(engine, "RefCounted", "GameState");

	RegisterSubclass<GameState, ClientState>(engine, "GameState", "ClientState");
	RegisterSubclass<FrontState, ClientState>(engine, "FrontState", "ClientState");
	RegisterSubclass<Object, ClientState>(engine, "Object", "ClientState");
	RegisterSubclass<RefCounted, ClientState>(engine, "RefCounted", "ClientState");

	RegisterSubclass<GameState, ServerState>(engine, "GameState", "ServerState");
	RegisterSubclass<FrontState, ServerState>(engine, "FrontState", "ServerState");
	RegisterSubclass<Object, ServerState>(engine, "Object", "ServerState");
	RegisterSubclass<RefCounted, ServerState>(engine, "RefCounted", "ServerState");

	RegisterSubclass<ServerState, LocalServerState>(engine, "ServerState", "LocalServerState");
	RegisterSubclass<GameState, LocalServerState>(engine, "GameState", "LocalServerState");
	RegisterSubclass<FrontState, LocalServerState>(engine, "FrontState", "LocalServerState");
	RegisterSubclass<Object, LocalServerState>(engine, "Object", "LocalServerState");
	RegisterSubclass<RefCounted, LocalServerState>(engine, "RefCounted", "LocalServerState");

	RegisterSubclass<ServerState, RemoteServerState>(engine, "ServerState", "RemoteServerState");
	RegisterSubclass<GameState, RemoteServerState>(engine, "GameState", "RemoteServerState");
	RegisterSubclass<FrontState, RemoteServerState>(engine, "FrontState", "RemoteServerState");
	RegisterSubclass<Object, RemoteServerState>(engine, "Object", "RemoteServerState");
	RegisterSubclass<RefCounted, RemoteServerState>(engine, "RefCounted", "RemoteServerState");

	RegisterMembers_FrontState<ClientState>(engine, "ClientState");
	RegisterMembers_FrontState<FrontState>(engine, "FrontState");
	RegisterMembers_FrontState<GameState>(engine, "GameState");
	RegisterMembers_FrontState<LocalServerState>(engine, "LocalServerState");
	RegisterMembers_FrontState<MainMenuState>(engine, "MainMenuState");
	RegisterMembers_FrontState<RemoteServerState>(engine, "RemoteServerState");
	RegisterMembers_FrontState<ServerState>(engine, "ServerState");
}
