#pragma once

#include "dependencies.h"
#include "inline.h"
#include "print_macro.h"
#include "macro.h"

/* ---- Common API ---- */
void handleError(otError error, char* desc);
void printMeshLocalEid(otInstance *aInstance);

/** ---- CoAP Common API ---- */
uint16_t getPayloadLength(const otMessage *aMessage);
void getPayload(const otMessage *aMessage, void* buffer);
otSockAddr createSocket(const char *recvAddrString);
void coapStart(void);

/* ---- CoAP Client API ---- */
void request(otSockAddr *socket,
             void *payload,
             size_t payloadSize,
             const char *uri,
             otCoapResponseHandler responseCallback,
             otCoapType type);

void addPayload(otMessage *aRequest, void *payload, size_t payloadSize);

/* ---- CoAP Server API ---- */
otError createResource(otCoapResource *resource,
                       const char *resourceName,
                       const char *uri,
                       otCoapRequestHandler requestHandler);

void resourceDestructor(otCoapResource *resource);
void sendCoapResponse(otMessage *aRequest, const otMessageInfo *aRequestInfo);
void printRequest(otMessage *aMessage, const otMessageInfo *aMessageInfo);

void startCoapServer(uint16_t port);

/* ---- CoAP Response Handler ---- */
void defaultResponseCallback(void *aContext,
                             otMessage *aMessage,
                             const otMessageInfo *aMessageInfo,
                             otError aResult);