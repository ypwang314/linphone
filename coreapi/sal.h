/*
linphone
Copyright (C) 2010  Simon MORLAT (simon.morlat@free.fr)

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*/

/** 
 This header files defines the Signaling Abstraction Layer.
 The purpose of this layer is too allow experiment different call signaling 
 protocols and implementations under linphone, for example SIP, JINGLE...
**/

#ifndef sal_h
#define sal_h

#include "mediastreamer2/mscommon.h"

struct Sal;

typedef struct Sal Sal;

struct SalOp;

typedef struct SalOp SalOp;

struct SalAddress;

typedef struct SalAddress SalAddress;

/* Address manipulation API*/
SalAddress * sal_address_new(const char *uri);
SalAddress * sal_address_clone(const SalAddress *addr);
const char *sal_address_get_scheme(const SalAddress *addr);
const char *sal_address_get_display_name(const SalAddress* addr);
const char *sal_address_get_username(const SalAddress *addr);
const char *sal_address_get_domain(const SalAddress *addr);
void sal_address_set_display_name(SalAddress *addr, const char *display_name);
void sal_address_set_username(SalAddress *addr, const char *username);
void sal_address_set_domain(SalAddress *addr, const char *host);
void sal_address_set_port(SalAddress *addr, const char *port);
void sal_address_set_port_int(SalAddress *uri, int port);
void sal_address_clean(SalAddress *addr);
char *sal_address_as_string(const SalAddress *u);
char *sal_address_as_string_uri_only(const SalAddress *u);
void sal_address_destroy(SalAddress *u);




Sal * sal_init();
void sal_uninit(Sal* sal);

typedef enum {
	SAL_TRANSPORT_DATAGRAM,
	SAL_TRANSPORT_STREAM
}SalTransport;

typedef enum {
	SAL_AUDIO,
	SAL_VIDEO,
	SAL_OTHER
} SalStreamType;

typedef enum{
	SAL_PROTO_UNKNOWN,
	SAL_PROTO_RTP_AVP,
	SAL_PROTO_RTP_SAVP
}SalMediaProto;

typedef struct SalStreamDescription{
	SalMediaProto proto;
	SalStreamType type;
	char addr[64];
	int port;
	MSList *payloads; //<list of PayloadType
	int bandwidth;
	int ptime;
} SalStreamDescription;

#define SAL_MEDIA_DESCRIPTION_MAX_STREAMS 4

typedef struct SalMediaDescription{
	char addr[64];
	char username[64];
	int nstreams;
	SalStreamDescription streams[SAL_MEDIA_DESCRIPTION_MAX_STREAMS];
} SalMediaDescription;

SalMediaDescription *sal_media_description_new();
void sal_media_description_destroy(SalMediaDescription *md);

/*this structure must be at the first byte of the SalOp structure defined by implementors*/
typedef struct SalOpBase{
	Sal *root;
	char *route;
	char *contact;
	char *from;
	char *to;
	SalMediaDescription *local_media;
	SalMediaDescription *remote_media;
} SalOpBase;


typedef enum SalError{
	SalErrorNoResponse,
	SalErrorMedia,
	SalErrorFailure, /* see SalReason for more details */
	SalErrorUnknown
} SalError;

typedef enum SalReason{
	SalReasonDeclined,
	SalReasonBusy,
	SalReasonRedirect,
	SalReasonTemporarilyUnavailable,
	SalReasonNotFound,
	SalReasonDoNotDisturb,
	SalReasonForbidden,
	SalReasonUnknown
}SalReason;

typedef enum SalPresenceStatus{
	SalPresenceOffline,
	SalPresenceOnline,
	SalPresenceBusy,
	SalPresenceBerightback,
	SalPresenceAway,
	SalPresenceOnthephone,
	SalPresenceOuttolunch,
	SalPresenceDonotdisturb,
	SalPresenceMoved,
	SalPresenceAltService,
}SalPresenceStatus;

typedef void (*SalOnCallReceived)(SalOp *op);
typedef void (*SalOnCallRinging)(SalOp *op);
typedef void (*SalOnCallAccepted)(SalOp *op);
typedef void (*SalOnCallAck)(SalOp *op);
typedef void (*SalOnCallUpdated)(SalOp *op);
typedef void (*SalOnCallTerminated)(SalOp *op);
typedef void (*SalOnCallFailure)(SalOp *op, SalError error, SalReason reason, const char *details);
typedef void (*SalOnAuthRequested)(SalOp *op, const char *realm, const char *username);
typedef void (*SalOnAuthSuccess)(SalOp *op, const char *realm, const char *username);
typedef void (*SalOnRegisterSuccess)(SalOp *op, bool_t registered);
typedef void (*SalOnRegisterFailure)(SalOp *op, SalError error, SalReason reason, const char *details);
typedef void (*SalOnVfuRequest)(SalOp *op);
typedef void (*SalOnDtmfReceived)(SalOp *op, char dtmf);
typedef void (*SalOnRefer)(Sal *sal, SalOp *op, const char *referto);
typedef void (*SalOnTextReceived)(Sal *sal, const char *from, const char *msg);
typedef void (*SalOnPresenceChanged)(Sal *sal, const char *from, SalPresenceStatus status);
typedef void (*SalOnInternalMsg)(Sal *sal, const char *msg);

typedef struct SalCallbacks{
	SalOnCallReceived call_received;
	SalOnCallRinging call_ringing;
	SalOnCallAccepted call_accepted;
	SalOnCallAck call_ack;
	SalOnCallUpdated call_updated;
	SalOnCallTerminated call_terminated;
	SalOnCallFailure call_failure;
	SalOnAuthRequested auth_requested;
	SalOnAuthSuccess auth_success;
	SalOnRegisterSuccess register_success;
	SalOnRegisterFailure register_failure;
	SalOnVfuRequest vfu_request;
	SalOnDtmfReceived dtmf_received;
	SalOnRefer refer_received;
	SalOnTextReceived text_received;
	SalOnPresenceChanged presence_changed;
	SalOnInternalMsg internal_message;
}SalCallbacks;

typedef struct SalAuthInfo{
	char *username;
	char *userid;
	char *password;
	char *realm;
}SalAuthInfo;

void sal_set_callbacks(Sal *ctx, const SalCallbacks *cbs);
int sal_listen_port(Sal *ctx, const char *addr, int port, SalTransport tr, int is_secure);
void sal_set_user_agent(Sal *ctx, const char *user_agent);
void sal_use_session_timers(Sal *ctx, int expires);
int sal_iterate(Sal *sal);

/*create an operation */
SalOp * sal_op_new(Sal *sal);

/*generic SalOp API, working for all operations */
void sal_op_set_contact(SalOp *op, const char *contact);
void sal_op_set_route(SalOp *op, const char *route);
void sal_op_set_from(SalOp *op, const char *from);
void sal_op_set_to(SalOp *op, const char *to);
void sal_op_release(SalOp *h);
void sal_op_authenticate(SalOp *h, const SalAuthInfo *info);

const char *sal_op_get_from(const SalOp *op);
const char *sal_op_get_to(const SalOp *op);
const char *sal_op_get_contact(const SalOp *op);

/*Call API*/
int sal_call_set_local_media_description(SalOp *h, SalMediaDescription *desc);
int sal_call(SalOp *h, const char *from, const char *to);
int sal_call_accept(SalOp*h);
int sal_call_decline(SalOp *h, SalReason reason, const char *redirection /*optional*/);
const SalMediaDescription * sal_call_get_final_media_description(SalOp *h);
int sal_call_terminate(SalOp *h);

int sal_register(SalOp *op, const char *proxy, const char *from, int expires);


#define payload_type_set_number(pt,n)	(pt)->user_data=(void*)((long)n);
#define payload_type_get_number(pt)		((int)(long)(pt)->user_data)


/*internal API */
void __sal_op_init(SalOp *b, Sal *sal);
void __sal_op_free(SalOp *b);


#endif
