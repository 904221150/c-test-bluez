#include <stdio.h>

#include "agent-server.h"


char* request_pin_code(AgentAgent1* interface,
    GDBusMethodInvocation* invocation, const char* device)
{ return NULL; }

void display_pin_code(AgentAgent1* interface,
    GDBusMethodInvocation* invocation, const char* device,
    const gchar* pincode)
{}

uint32_t request_passkey(AgentAgent1* interface,
    GDBusMethodInvocation* invocation, const char* device)
{ return 0; }

void display_passkey(AgentAgent1* interface,
    GDBusMethodInvocation* invocation, const char* device,
    uint32_t passkey, uint16_t entered)
{}

void request_confirmation(AgentAgent1* interface,
    GDBusMethodInvocation* invocation, const char* device,
    uint32_t passkey)
{}

void request_authorization(AgentAgent1* interface,
    GDBusMethodInvocation* invocation, const char* device)
{
    g_info("%s called", __FUNCTION__);
    agent_agent1_complete_request_authorization(interface, invocation);
}

void authorize_service(AgentAgent1* interface,
    GDBusMethodInvocation* invocation, const char* path,
    const gchar* uuid)
{
    g_info("%s called", __FUNCTION__);
    agent_agent1_complete_authorize_service(interface, invocation);
}

void cancel(AgentAgent1* interface,
    GDBusMethodInvocation* invocation)
{
    g_info("%s called", __FUNCTION__);
    agent_agent1_complete_cancel(interface, invocation);
}



