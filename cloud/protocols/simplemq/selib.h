/*
 *     ____             _________                __                _
 *    / __ \___  ____ _/ /_  __(_)___ ___  ___  / /   ____  ____ _(_)____
 *   / /_/ / _ \/ __ `/ / / / / / __ `__ \/ _ \/ /   / __ \/ __ `/ / ___/
 *  / _, _/  __/ /_/ / / / / / / / / / / /  __/ /___/ /_/ / /_/ / / /__
 * /_/ |_|\___/\__,_/_/ /_/ /_/_/ /_/ /_/\___/_____/\____/\__, /_/\___/
 *                                                       /____/
 *
 *                 SharkSSL Embedded SSL/TLS Stack
 ****************************************************************************
 *   PROGRAM MODULE
 *
 *   $Id: selib.h 3638 2015-01-05 18:21:33Z wini $
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

#ifndef _selib_h
#define _selib_h
#include <selibplat.h>

#ifndef SE_CTX
#define SeCtx void
#endif

/** @addtogroup lib_cloud_smq
@{
*/

#ifndef XTYPES
#ifndef NO_C99
#include <stdint.h>
typedef uint8_t            U8;
typedef int8_t             S8;
typedef uint16_t           U16;
typedef int16_t            S16;
typedef uint32_t           U32;
typedef int32_t            S32;
typedef uint64_t           U64;
typedef int64_t            S64;
#else
typedef unsigned char      U8;
typedef signed   char      S8;
typedef unsigned short     U16;
typedef signed   short     S16;
typedef unsigned long      U32;
typedef signed   long      S32;
typedef unsigned long long U64;
typedef signed   long long S64;
#endif
#endif

#ifndef XPRINTF
#define XPRINTF 0
#endif

#ifndef TRUE
#define TRUE  1
#endif

#ifndef FALSE
#define FALSE 0
#endif

#ifndef baAssert
#include <assert.h>
#define baAssert assert
#endif

#include <string.h>
#include <stdarg.h>


/** Infinite wait time option for socket read functions.
 */
#define INFINITE_TMO (~((U32)0))

#ifndef NO_BSD_SOCK
/** The SOCKET object/handle is an 'int' when using a BSD compatible
    TCP/IP stack. Non BSD compatible TCP IP stacks must set the macro
    NO_BSD_SOCK and define the SOCKET object. See the header file
    selib.h for details.
*/
#define SOCKET int
#endif

#ifndef SE_CTX
#define SeCtx void
#endif

#ifndef SOCKET_constructor
#define SOCKET_constructor(o, ctx) (void)ctx,memset(o,0,sizeof(SOCKET))
#endif


#ifdef __cplusplus
extern "C" {
#endif

/** Initializes a SOCKET object connected to a remote host/address at
 * a given port.
 \return  Zero on success.
   Error codes returned:
   \li \c -1 Cannot create socket: Fatal
   \li \c -2 Cannot resolve 'address'
   \li \c -3 Cannot connect
*/
int se_connect(SOCKET* sock, const char* address, U16 port);

/** Initializes a SOCKET object bound to a local port, ready to accept
    client connections.
 \return Zero on success.
   Error codes returned:
   \li \c -1 Cannot create socket: Fatal
   \li \c -2 Cannot listen: Fatal
   \li \c -3 Cannot bind: socket in use
 */
int se_bind(SOCKET* sock, U16 port);

/** Waits for remote connections on the server SOCKET object
   'listenSock', initialized by function se_bind, and initializes
   socket object 'outSock' to represent the new connection.

   \return
   \li \c 1 Success
   \li \c 0 timeout
   \li \c -1 error
*/
int se_accept(SOCKET** listenSock, U32 timeout, SOCKET** outSock);

/** Close a connected socket connection.
 */
void se_close(SOCKET* sock);

/** Returns TRUE if socket is valid (connected).
 */
int se_sockValid(SOCKET* sock);

/** Sends data to the connected peer.
 */
S32 se_send(SOCKET* sock, const void* buf, U32 len);

/** Waits for data sent by peer.

    \param sock the SOCKET object.
    \param buf is the data to send.
    \param len is the 'buf' length.
    \param timeout in milliseconds. The timeout can be set to #INFINITE_TMO.
    \returns the length of the data read, zero on timeout, or a
    negative value on error.
 */
S32 se_recv(SOCKET* sock, void* buf, U32 len, U32 timeout);

#if XPRINTF == 1
/** The macro xprintf expands to function _xprintf if the code is
    compiled with XPRINTF set to 1.
    \param data is standard printf arguments enclosed in parenthesis;
    thus you must use double parenthesis when using macro xprintf.
*/
#define xprintf(data) _xprintf data
/** The example code and macro xprintf requires this function when the
    code is compiled with macro XPRINTF set to 1.
    \param fmt the format string.
    \param ... variable arguments.
*/
#ifndef _xprintf  /* to handle #define _xprintf printf */
void _xprintf(const char* fmt, ...);
#endif
#else
#ifndef xprintf
#define xprintf(data)
#endif
#endif

/** Main entry for all example programs */
void mainTask(SeCtx* ctx);

#ifdef __cplusplus
}
#endif


/** @} */ /* end group selib */

#endif
