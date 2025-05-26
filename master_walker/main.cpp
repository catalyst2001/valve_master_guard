#include <platform.h>
#include <cdll_int.h>
#include <cbase.h>
#include <utlvector.h>
#include <threadtools.h>
#include <netadr.h>
#include "networksystem/inetworkmessage.h"
#include "networksystem/inetworksystem.h"

IVEngineClient* g_pengine = nullptr;
INetworkSystem* g_pnetwork = nullptr;

#define CHECK_FACTORY(factory) do {\
	if(!factory) {\
			Error("Factory " #factory " is invalid!");\
			return 1;\
		}\
	} while (0.0)

#define GET_INTERFACE(var, type, factory, ifacename) do {\
	var = reinterpret_cast<type *>(factory(ifacename, nullptr));\
		if(!var) {\
			Error("Failed to get '" ifacename "' interface!");\
			return 1;\
		}\
	} while(0.0)

/**
* handle_network_event
* 
*/
void handle_network_event(NetworkEvent_t *pevent)
{
	char buf[512];
	Assert(pevent && "pevent was nullptr!");
	switch (pevent->m_nType)
	{
	case NETWORK_EVENT_MESSAGE_RECEIVED: {
		NetworkMessageReceivedEvent_t* msgevent = static_cast<NetworkMessageReceivedEvent_t*>(pevent);
		Assert(msgevent->m_pChannel && "msgevent->m_pChannel was nullptr!");
		const netadr_t& remote_adr = msgevent->m_pChannel->GetRemoteAddress();
		remote_adr.ToString(buf, sizeof(buf));
		Msg("Netmsg '%s' received from %s\n", msgevent->m_pNetworkMessage->GetName(), buf);
		break;
	}

	case NETWORK_EVENT_CONNECTED:
		break;
	case NETWORK_EVENT_DISCONNECTED:
	default:
		Msg("unhandled network event %d!\n", pevent->m_nType);
	}
}

int main()
{
	Msg("Client startup\n");
	Msg("Commandline: \"%s\"\n", CommandLine()->GetCmdLine());
	CreateInterfaceFn engine_factory = Sys_GetFactory("engine.dll");
	//CreateInterfaceFn tier0_factory = Sys_GetFactory("tier0.dll");
	CreateInterfaceFn network_factory = Sys_GetFactory("networksystem.dll");

	CHECK_FACTORY(engine_factory);
	//CHECK_FACTORY(tier0_factory);
	CHECK_FACTORY(network_factory);
	GET_INTERFACE(g_pengine, IVEngineClient, engine_factory, VENGINE_CLIENT_INTERFACE_VERSION_13);
	GET_INTERFACE(g_pnetwork, INetworkSystem, network_factory, NETWORKSYSTEM_INTERFACE_VERSION);
	MathLib_Init(2.2f, 2.2f, 0.0f, 2.0f);
	
	/* init network system */
	if (g_pnetwork->Init() != INIT_OK) {
		Error("network system initializing failed!\n");
		return 1;
	}
	
	/* try connect */
	//NOTE: K.D. help me raggerr
	ConnectionStatus_t status = CONNECTION_STATE_DISCONNECTED;
	if (g_pnetwork->StartClient()) {
		int nport = 27015;
		const char* pserver = "";
		do {
			INetChannel* pnetchan = g_pnetwork->ConnectClientToServer(pserver, nport);
			while (pnetchan) {
				status = pnetchan->GetConnectionState();
				if (status != CONNECTION_STATE_CONNECTED &&
					status != CONNECTION_STATE_CONNECTING) {
					Msg("");
					g_pnetwork->DisconnectClientFromServer(pnetchan);
					break;
				}

				constexpr int thread_sleep_interval = 1 / 66.f * 1000;
				g_pnetwork->ClientReceiveMessages();
				NetworkEvent_t* pnetevent = g_pnetwork->FirstNetworkEvent();
				if (pnetevent) {
					handle_network_event(pnetevent);
					while ((pnetevent = g_pnetwork->NextNetworkEvent())) {
						handle_network_event(pnetevent);
					}
				}
				g_pnetwork->ClientSendMessages();
				ThreadSleep(thread_sleep_interval);
			}
			Msg("Client disconnected from server with status: %d\n", status);
		} while (1);
		g_pnetwork->ShutdownClient();
	}
	g_pnetwork->Shutdown();
	return 0;
}