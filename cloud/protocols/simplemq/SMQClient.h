/*
 *     ____             _________                __                _     
 *    / __ \___  ____ _/ /_  __(_)___ ___  ___  / /   ____  ____ _(_)____
 *   / /_/ / _ \/ __ `/ / / / / / __ `__ \/ _ \/ /   / __ \/ __ `/ / ___/
 *  / _, _/  __/ /_/ / / / / / / / / / / /  __/ /___/ /_/ / /_/ / / /__  
 * /_/ |_|\___/\__,_/_/ /_/ /_/_/ /_/ /_/\___/_____/\____/\__, /_/\___/  
 *                                                       /____/          
 *
 ****************************************************************************
 *            HEADER
 *
 *   $Id: SMQClient.h 3638 2015-01-05 18:21:33Z wini $
 *
 *   COPYRIGHT:  Real Time Logic LLC, 2014
 *
 *   This software is copyrighted by and is the sole property of Real
 *   Time Logic LLC.  All rights, title, ownership, or other interests in
 *   the software remain the property of Real Time Logic LLC.  This
 *   software may only be used in accordance with the terms and
 *   conditions stipulated in the corresponding license agreement under
 *   which the software has been supplied.  Any unauthorized use,
 *   duplication, transmission, distribution, or disclosure of this
 *   software is expressly forbidden.
 *                                                                        
 *   This Copyright notice may not be removed or modified without prior
 *   written consent of Real Time Logic LLC.
 *                                                                         
 *   Real Time Logic LLC. reserves the right to modify this software
 *   without notice.
 *
 *               https://realtimelogic.com
 ****************************************************************************
 *
 */

#ifndef __SMQClient_h
#define __SMQClient_h

#include "selib.h"

#ifndef TRUE
#define TRUE 1
#define FALSE 0
#endif

/** @addtogroup lib_cloud_smq
@{
*/

/** \defgroup SMQClientErrorCodes Error codes returned by function SMQ_getMessage
\ingroup SMQClient
\anchor SMQClientErrorCodes hello
@{
*/

/** The buffer provided in SMQ_constructor is not sufficiently large.
 */
#define SMQE_BUF_OVERFLOW    -10000

/** The URL provided is invalid.
 */
#define SMQE_INVALID_URL     -10002 

/** TCP connection error
 */
#define SMQE_PROTOCOL_ERROR  -10003 

/** Server sent a disconnect message
 */
#define SMQE_DISCONNECT      -10004


/** No PONG response to PING: timeout
 */
#define SMQE_PONGTIMEOUT     -10005


/** The SMQ_getMessage call timed out.
 */
#define SMQE_TIMEOUT         -10100


/** @} */ /* end SMQClientErrorCodes */


/** \defgroup SMQClientRespCodes Response codes returned by function SMQ_getMessage
\ingroup SMQClient
@{
*/

/** #SMQ_subscribe response message received via #SMQ_getMessage.
    \li SMQ::ptid is set to the subscribed Topic ID.
    \li SMQ::status is set to zero (0) if the request was accepted and
    a negative value if the request was denied.
    \li the 'msg' out parameter in #SMQ_getMessage is set to the optional
    server's authentication response message if the request was
    denied.
 */
#define SMQ_SUBACK           -20000

/** #SMQ_create response message received via #SMQ_getMessage.
    \li SMQ::ptid is set to the created Topic ID.
    \li SMQ::status is set to zero (0) if the request was accepted and
    a negative value if the request was denied.
    \li the 'msg' out parameter in #SMQ_getMessage is set to the optional
    server's authentication response message if the request was
    denied.
 */
#define SMQ_CREATEACK        -20001

/** #SMQ_createsub response message received via #SMQ_getMessage.
    \li SMQ::ptid is set to the created Sub Topic ID.
    \li SMQ::status is set to zero (0) if the request was accepted and
    a negative value if the request was denied.
 */
#define SMQ_CREATESUBACK     -20002

/** Change notification event (from observed tid). Observe events are
 * initiated via #SMQ_observe.

    \li SMQ::ptid is set to the observed Topic ID.
    \li SMQ::status is set to the number of clients subscribed to the topic.
 */
#define SMQ_SUBCHANGE        -20003

/** @} */ /* end SMQClientRespCodes */


#define SMQSTR(str) str, (sizeof(str)-1)

/** SimpleMQ structure.
 */
typedef struct
{
   SOCKET sock;

   U8* buf; /**< The buffer set via the constructor. */

   /** Timeout in milliseconds to wait in functions waiting for server
       data */
   U32 timeout;
   S32 pingTmoCounter,pingTmo;
   U32 clientTid; /**< Client's unique topic ID */
   U32 tid;  /**< Topic: set when receiving MSG_PUBLISH from broker */
   U32 ptid; /**< Publisher's tid: Set when receiving MSG_PUBLISH from broker */
   U32 subtid; /**< Sub-tid: set when receiving MSG_PUBLISH from broker */
   int status; /**< Last known error code */
   U16 bufLen;
   U16 bufIx;
   U16 frameLen; /**< The SimpleMQ frame size for the incomming data */
   /** Read frame data using SMQ_getMessage until: frameLen - bytesRead = 0 */
   U16 bytesRead;
   U8 inRecv; /* boolean set to true when thread blocked in SMQ_recv */
} SMQ;


/** Create a SimpleMQ client instance.
    \param o Uninitialized data of size sizeof(SMQ).
    \param buf is used for internal management and must not be less
    than 127 bytes and not smaller than the largest control
    frame. Function SMQ_getMessage will return #SMQE_BUF_OVERFLOW if
    the buffer is not sufficiently large.
    \param bufLen buffer length.
 */
void SMQ_constructor(SMQ* o, U8* buf, U16 bufLen);


/** Bare metal configuration. This macro must be called immediately
    after calling the constructor on bare metal systems.
    \param o the #SMQ instance.
    \param ctx an SeCtx instance.
 */
#define SMQ_setCtx(o, ctx) SOCKET_constructor(&(o)->sock, ctx)


/** Initiate the SMQ server connection. The connection phase is
    divided into two steps: (1) initiating and (2) connecting via
    SMQ_connect.
    \param o the SMQ instance.
    \param url is a URL that starts with http:// and this URL
    must be to a server resource that initiates a SimpleMQ connection.
    \param rnd (out param) a random number created by the server. This
    number can be used for securing hash based password encryption.
    \returns 0 on success, error code from TCP/IP stack, or
    [SimpleMQ error code](\ref SMQClientErrorCodes). SMQ::buf is set
    to the IP address of the client as seen by the broker.
 */
int SMQ_init(SMQ* o, const char* url, U32* rnd);

/** Connect/establish a persistent SimpleMQ connection. The connection
    phase is divided into two steps: (1) initiating via SMQ_init and (2)
    connecting.
    \param o the SMQ instance.
    \param uid a universally unique client identifier (uid) must be
    unique across all clients connecting to the same broker
    instance. The uid is preferably a stringified version of the
    client's Ethernet MAC address.
    \param uidLen the uid length.
    \param credentials provide credentials if required by the broker instance.
    \param credLen credentials length.
    \param info a string that provides information to optional server
    code interacting with the broker. This string is also passed into
    the optional broker's authentication callback function.
    \param infoLen length of info.
    \returns 0 on success, error code from TCP/IP stack,
    [SimpleMQ error code](\ref SMQClientErrorCodes), or one of the
    following error codes from the broker:

    \li 0x01	Connection Refused: unacceptable protocol version
    \li 0x02	Connection Refused: server unavailable
    \li 0x03	Connection Refused: Incorrect credentials
    \li 0x04	Connection Refused: Client certificate required
    \li 0x05	Connection Refused: Client certificate not accepted
    \li 0x06	Connection Refused: Access denied

    The broker may optionally send a human readable string in addition
    to the above broker produced error codes. This string is avaiable
    via SMQ::buf.
 */
int SMQ_connect(SMQ* o, const char* uid, int uidLen, const char* credentials,
                U8 credLen, const char* info, int infoLen);


/** Gracefully close the connection. You cannot publish any messages
    after calling this method.
    \param o the SMQ instance.
 */
void SMQ_disconnect(SMQ* o);


/** Terminate a SimpleMQ instance.
    \param o the SMQ instance.
 */
void SMQ_destructor(SMQ* o);


/** Create a topic an fetch the topic ID (tid). The SMQ protocol is
    optimized and does not directly use a string when publishing, but a
    number. The server randomly a creates 32 bit number and
    persistently stores the topic name and number.

    The response to SMQ_create is asynchronous and returned as status
    #SMQ_CREATEACK via #SMQ_getMessage.

    \param o the SMQ instance.
    \param topic the topic name where you plan on publishing messages.
 */
int SMQ_create(SMQ* o, const char* topic);


/** Create a sub-topic and fetch the subtopic ID.

    The response to SMQ_subscribe is asynchronous and returned as status
    #SMQ_CREATESUBACK via #SMQ_getMessage.

    \param o the SMQ instance.
    \param subtopic the sub-topic name you want registered.
 */
int SMQ_createsub(SMQ* o, const char* subtopic);


/**

    The response to SMQ_subscribe is asynchronous and returned as status
    #SMQ_SUBACK via #SMQ_getMessage.

    \param o the SMQ instance.
    \param topic the topic name to subscribe to.
 */
int SMQ_subscribe(SMQ* o, const char* topic);


/** Requests the broker to unsubscribe the server from a topic.
    \param o the SMQ instance.
    \param tid the topic name's Topic ID.
 */
int SMQ_unsubscribe(SMQ* o, U32 tid);


/** Publish messages to a topic and optionally to a sub-topic. Topic
    name must have previosly been been resolved by #SMQ_create and
    sub-topic should preferably have been created by #SMQ_createsub.
    \param o the SMQ instance.
    \param data message payload.
    \param len payload length.
    \param tid the topic ID (created with SMQ_create).
    \param subtid optional sub-topic ID preferably created with SMQ_createsub.
 */
int SMQ_publish(SMQ* o, const void* data, int len, U32 tid, U32 subtid);


/** Publish a message in chunks and request the broker to assemble the
    message before publishing to the subscriber(s). This method uses
    the internal buffer (SMQ::buf) and sends the message as a chunk
    when the internal buffer is full, thus sending the message as an
    incomplete message to the broker. The message is assembled by the
    broker when you flush the remaining bytes in the buffer by calling
    #SMQ_pubflush.

    \param o the SMQ instance.
    \param str a string.
 */
int SMQ_wrtstr(SMQ* o, const char* str);

/** Publish a message in chunks and request the broker to assemble the
    message before publishing to the subscriber(s). This method uses
    the internal buffer (SMQ::buf) and sends the message as a chunk
    when the internal buffer is full, thus sending the message as an
    incomplete message to the broker. The message is assembled by the
    broker when you flush the remaining bytes in the buffer by calling
    #SMQ_pubflush.


    \param o the SMQ instance.
    \param data message payload.
    \param len payload length.
 */
int SMQ_write(SMQ* o,  const void* data, int len);

/** Flush the internal buffer and request the broker to assemble all
    stored fragments as one message. This message is then published to
    topic 'tid', and sub-topic 'subtid'.

    \param o the SMQ instance.
    \param tid the topic ID (created with SMQ_create).
    \param subtid optional sub-topic ID preferably created with SMQ_createsub.

    Example:
    \code
    SMQ_wrtstr(smq, "Hello");
    SMQ_wrtstr(smq, " ");
    SMQ_wrtstr(smq, "World");
    SMQ_pubflush(smq,tid,subtid);
    \endcode

 */
int SMQ_pubflush(SMQ* o, U32 tid, U32 subtid);


/** Request the broker to provide change notification events when the
    number of subscribers to a specific topic changes. Ephemeral topic
    IDs can also be observed. The number of connected subscribers for
    an ephemeral ID can only be one, which means the client is
    connected. Receiving a change notification for an ephemeral ID
    means the client has disconnected and that you no longer will get
    any change notifications for the observed topic ID.

    Change notification events are received as #SMQ_SUBCHANGE via
    #SMQ_getMessage.

    \param o the SMQ instance.
    \param tid the Topic ID you which to observe.
 */
int SMQ_observe(SMQ* o, U32 tid);


/** Stop receiving change notifications for a topic ID or ephemeral topic ID.
    \param o the SMQ instance.
    \param tid the Topic ID you no longer want to observe.
 */
int SMQ_unobserve(SMQ* o, U32 tid);


/** Wait for messages sent from the broker.
    \param o the SMQ instance.
    \param msg a pointer to the response data (out param)
    \returns
    \li a negative value signals an
    [error code](\ref SMQClientErrorCodes) or an
    [asynchronous response code](\ref SMQClientRespCodes).
    \li zero signals timeout.
    \li a value greater than zero signals the reception of a full
    message or a message fragment. See receiving large frames for details.

    <b>Receiving large frames:</b><br>
    The SimpleMQ protocol is frame based, but the function can return
    a fragment before the complete SimpleMQ frame is received if the
    frame sent by the peer is larger than the provided buffer. The
    frame length is returned in SMQ::frameLen and the data consumed
    thus far is returned in SMQ::bytesRead. The complete frame is
    consumed when frameLen == bytesRead.

    <b>Note:</b> the default timeout value is set to one minute. You
    can set the timeout value by setting SharkMQ::timeout to the
    number of milliseconds you want to wait for incoming messages
    before the timeout triggers. Note: Setting a long timeout may
    interfere with the built in PING timer.
 */
int SMQ_getMessage(SMQ* o, U8** msg);


/** Returns the message size, which is SMQ::frameLen - 15.
    \param o the SMQ instance.
 */
#define SMQ_getMsgSize(o)

/** @} */ /* end group selib */

#endif
