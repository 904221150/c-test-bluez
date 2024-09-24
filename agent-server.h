#ifndef AGENT_SERVER_H
#define AGENT_SERVER_H

#include <stdint.h>
#include <glib.h>
#include <gio/gio.h>
#include "bluez-agent.h"

char* request_pin_code(AgentAgent1* interface,
    GDBusMethodInvocation* invocation, const char* device);

void display_pin_code(AgentAgent1* interface,
    GDBusMethodInvocation* invocation, const char* device,
    const gchar* pincode);

uint32_t request_passkey(AgentAgent1* interface,
    GDBusMethodInvocation* invocation, const char* device);

void display_passkey(AgentAgent1* interface,
    GDBusMethodInvocation* invocation, const char* device,
    uint32_t passkey, uint16_t entered);

void request_confirmation(AgentAgent1* interface,
    GDBusMethodInvocation* invocation, const char* device,
    uint32_t passkey);

void request_authorization(AgentAgent1* interface,
    GDBusMethodInvocation* invocation, const char* device);

void authorize_service(AgentAgent1* interface,
    GDBusMethodInvocation* invocation, const char* path,
    const gchar* uuid);

void cancel(AgentAgent1* interface,
    GDBusMethodInvocation* invocation);


#endif // AGENT_SERVER_H