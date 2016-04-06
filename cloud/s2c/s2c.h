/*
 * ZentriOS SDK LICENSE AGREEMENT | Zentri.com, 2015.
 *
 * Use of source code and/or libraries contained in the ZentriOS SDK is
 * subject to the Zentri Operating System SDK license agreement and
 * applicable open source license agreements.
 *
 */
#pragma once


#include "zos_common.h"
#include "utilities/zos_json.h"
#include "network/zos_network.h"

/**
 * @addtogroup lib_cloud_s2c_macros
 * @{
 */

/**
 * The size of the device key (to encrypt local server client connections)
 */
#define S2C_DEVICE_KEY_SIZE 16
/**
 * The maximum size of a data packet payload for the cloud or local client
 */
#define S2C_MAX_PACKET_SIZE 1424


/**
 * @}
 * @addtogroup lib_cloud_s2c_types
 * @{
 */

/**
 * @brief S2C library settings
 *
 * These index the @ref s2c_config_t passed to the library
 * on initialization.
 */
typedef enum
{
    S2C_SETTING_FLAGS,                      //!< Settings flags, @ref s2c_flag_t
    S2C_SETTING_CLOUD_TOKEN,                //!< Cloud token supplied by a remote app,
                                            //!< this is used to log the device into the cloud
    S2C_SETTING_DEVICE_KEY,                 //!< Device key supplied by a remote app for encryption local server client connects,
                                            //!< this is used to encrypt connects with local mobile apps
    S2C_SETTING_PRODUCT_TYPE,               //!< The product code of this device (e.g BMC123) this is broadcasted on the local network
    S2C_SETTING_PRODUCT_DESCRIPTION,        //!< A description of the device (e.g. Kitchen Light), this may be set by the mobile app
    S2C_SETTING_MDNS_TXT_CALLBACK,          //!< Optional callback to broadcast custom mDNS TXT records on the local network, @ref mdns_txt_record_callback_t
    S2C_SETTING_UDP_BROADCAST_CALLBACK,     //!< Optional callback to broadcast custom UDP packets on the local network, @ref broadcast_callback_t
    S2C_SETTING_CLOUD_DISCONNECTED_HANDLER, //!< Optional event handler which indicates if the remote cloud disconnects, @ref zos_event_handler_t
    S2C_SETTING_LOCAL_CONNECTED_HANDLER,    //!< Optional event handler which indicates if a client connects to the local server, @ref zos_event_handler_t
    S2C_SETTING_LOCAL_DISCONNECTED_HANDLER, //!< Optional event handler which indicates if a client disconnect from the local server, @ref zos_event_handler_t
    S2C_SETTING_STREAM_READ_REQUEST_HANDLER,//!< Optional event handler which indicates if a client requests the device's current stream values, @ref zos_event_handler_t
    S2C_SETTING_COMMAND_CALLBACK,           //!< Optional, @ref s2c_command_callback_t, callback which is called when a locally connected client issues a command to the device. This callback can stop the command from executing.
    S2C_SETTING_COUNT                       //!< The total number of settings
} s2c_setting_t;

/**
 * @brief S2C Settings flags
 */
typedef enum
{
    S2C_FLAG_MDNS_ENABLED               = (1 << 0),//!< Enable mDNS for advertising device on local network
    S2C_FLAG_BROADCAST_ENABLED          = (1 << 1),//!< Enable UDP broadcast for advertising device on local network
} s2c_flag_t;

/**
 * The S2C libraries status
 *
 * These flags are broadcasts on the local network
 */
typedef enum
{
    S2C_STATUS_HAVE_DEVICE_KEY      = (1 << 0),//!< The device has a key for the local server (i.e. local connections are encrypted)
    S2C_STATUS_HAVE_CLOUD_TOKEN     = (1 << 1),//!< The device has a cloud token (i.e. it's able to connect to the cloud)
    S2C_STATUS_SERVER_ON_WLAN       = (1 << 2),//!< The device's local server is running the the WLAN interface
    S2C_STATUS_SERVER_ON_SOFTAP     = (1 << 3),//!< The device's local server is running on the SoftAP interface
    S2C_STATUS_CLOUD_CONNECTED      = (1 << 4),//!< The device is connected to the cloud
} s2c_status_flag_t;

/**
 * The target system node of a packet
 */
typedef enum
{
    S2C_NODE_DEVICE          = 0,//!< The Device (i.e. the ZentriOS application running a ZentriOS module)
    S2C_NODE_HOST            = 1,//!< The external host MCU connected to the Device
    S2C_NODE_MOBILE          = 2,//!< Mobile application (e.g. iPhone or Android App)
    S2C_NODE_CLOUD           = 3,//!< S2C Cloud connector
    S2C_NODE_DMS             = 4 //!< Device Management Server
} s2c_node_t;




/**
 * Array of pointers to configure the S2C library
 */
typedef const void* s2c_config_t[S2C_SETTING_COUNT];

/**
 * Callback for when data has been read from a stream
 *
 * This callback executes in the context of the TCP server. The callback
 * is supplied with the parsed JSON tokens. The tokens are automatically
 * cleaned up after the callback returns.
 *
 * @param stream The whose data has been read
 * @param arg Argument supplied when the listener was registered, @ref s2c_register_listener
 * @param json_context The parsed JSON context, @ref json_parse_context_t
 * @param msg_tok The 'message' token from the received stream data
 */
typedef void (*s2c_stream_listener_t)(const char *stream, void *arg, json_parse_context_t *json_context, json_tok_t *msg_tok);


/**
 * Callback for when a local client attempt to execute a command
 *
 * This callback can be used as a security feature. It is called before a
 * local client attempts to execute a command. If the callback returns
 * @ref ZOS_FALSE then the command won't be executed and the client will receive and
 * error code.
 *
 * If the callback returns @ref ZOS_TRUE then the command will be executed.
 *
 * @note Some commands are required as part of the setup process. If this callback
 *       always returns ZOS_FALSE then the device will not be able to be setup.
 *
 * @param source The source of the command, see @ref s2c_node_t
 * @param command_string The commmand string the remote client wants to execute
 * @return ZOS_TRUE if the remote client is allowed to execute the command, ZOS_FALSE else
 */
typedef zos_bool_t (*s2c_command_callback_t)(s2c_node_t source, const char *command_string);



/**
 * @}
 * @addtogroup lib_cloud_s2c
 * @{
 */

/**
 * @brief Initialize the S2C library
 *
 * This should be called on ZAP startup. The supplied config
 * should contain pointers/values for the S2C libraries internal settings.
 * If a settings isn't used leave as NULL.
 *
 * @param config Array of pointers/values for library configuration
 * @return @ref zos_result_t The result of the API call.
 * @zos{level=1}
 */
zos_result_t s2c_init(const s2c_config_t config);

/**
 * @brief De-initialize the S2C library
 *
 * This should be called on ZAP shutdown.
 * @zos{level=1}
 */
void s2c_deinit(void);

/**
 * @brief Return the value of a S2C library setting
 *
 * @param setting The key of the corresponding setting
 * @return The value of the setting
 * @zos{level=1}
 */
const void* s2c_get_setting(s2c_setting_t setting);

/**
 * @brief Set the value of a S2C library setting
 *
 * @param setting The key of the corresponding setting
 * @param value The next value of the setting
 * @return @ref zos_result_t The result of the API call.
 * @zos{level=1}
 */
zos_result_t s2c_set_setting(s2c_setting_t setting, const void *value);

/**
 * @brief Start the local server Mobile app's connect
 *
 * This starts the local server on the specified network interface.
 * If the network is up, it will bring the network up first before starting
 * the server.
 *
 * @param interface @ref zos_interface_t, The network interface on which to run the S2C Mobile server
 * @return @ref zos_result_t The result of the API call.
 * @zos{level=1}
 */
zos_result_t s2c_start_server(zos_interface_t interface);

/**
 * @brief Stop the local server
 *
 * This stop the local server. All open Mobile clients connections are closed first.
 *
 * @return @ref zos_result_t The result of the API call
 * @zos{level=1}
 */
zos_result_t s2c_stop_server(void);

/**
 * @brief Open a connection to the cloud
 *
 * This opens a websocket connection to the cloud. The device must first have
 * a valid cloud token before issuing this API call. If cloud token must be set
 * either during s2c_init() or s2c_set_setting() using the @ref S2C_SETTING_CLOUD_TOKEN setting.
 *
 * Upon opening the websocket connection, the cloud validates the token. If the token is
 * invalid then @ref ZOS_INVALID_TOKEN is returned. The device should stop trying to connect
 * and obtain a new token if this result is returned.
 *
 * @param url_str The host URL of cloud (e.g. wss://mycloud.com)
 * @param cert_filename Optional TLS certificate filename used to create a secure connection to the cloud
 * @return @ref zos_result_t The result of the API call
 * @zos{level=1}
 */
zos_result_t s2c_open_cloud_connection(const char *url_str, const char *cert_filename);

/**
 * @brief Close connection to cloud
 *
 * This closed the websocket connection to the cloud.
 *
 * @return @ref zos_result_t The result of the API call
 * @zos{level=1}
 */
zos_result_t s2c_close_cloud_connection(void);

/**
 * @brief Write stream data to all listening clients
 *
 * This will write stream data to:
 * - The cloud if the connection is opened
 * - A local client if the client registered a listener (this is a different listener than s2c_register_listener())
 *
 * This API allows for writing stream data all at once or in chunks.
 * To write stream data all at once, supply:
 * - `stream` - The name of the stream
 * - `data` - Pointer to buffer containing stream data
 * - `length` - The length of the buffer
 * - `last_write` - ZOS_TRUE
 *
 * To write stream data in chunks, this API must be called two or more times. The first time, supply:
 * - `stream` - The name of the stream
 * - `data` - NULL
 * - `length` - The total length of all the data chunks to write
 * - `last_write` - ZOS_FALSE
 *
 * Subsequent calls, supply:
 * - `stream` - The name of the stream (optional)
 * - `data` - Pointer to buffer containing stream data chunk
 * - `length` - The length of the data chunk
 * - `last_write` - ZOS_FALSE if more writes to follow, ZOS_TRUE else
 *
 * @param stream The name of the data stream to write
 * @param data Pointer to data buffer
 * @param length Length of data buffer
 * @param last_write ZOS_TRUE if no more writes to follow, ZOS_FALSE if final write
 * @return @ref zos_result_t The result of the API call
 * @zos{level=1}
 */
zos_result_t s2c_write_stream(const char *stream, const void *data, uint16_t length, zos_bool_t last_write);

/**
 * @brief Write string data to all listening clients
 *
 * This will write string data to:
 * - The cloud if the connection is opened
 * - A local client if the client registered a listener (this is a different listener than s2c_register_listener())
 *
 * @note The string is written all at once (chunking not supported, @ref s2c_write_stream() )
 *
 * @param stream The name of the stream to write
 * @param str Null-terminated string to write
 * @return @ref zos_result_t The result of the API call
 * @zos{level=1}
 */
zos_result_t s2c_write_stream_str(const char *stream, const char *str);

/**
 * @brief Register a listener for incoming stream data
 *
 * This registers a callback which is executed when the corresponding stream receives
 * data from either the cloud or local client. Refer to @ref s2c_stream_listener_t for
 * more information on how the callback works.
 *
 * Use s2c_unregister_listener() to remove the listener.
 *
 * @note The registered listener executes in the TCP server context.
 *
 * @param stream Name of stream to listen for incoming data
 * @param listener Callback to be executed when stream receives data
 * @param arg Optional argument to pass to callback
 * @return @ref zos_result_t The result of the API call
 * @zos{level=1}
 */
zos_result_t s2c_register_listener(const char *stream, s2c_stream_listener_t listener, void *arg);

/**
 * @brief Unregister a listener for incoming stream data
 *
 * This removes a listener registered by s2c_register_listener()
 *
 * @param stream Name of the stream for which to remove the listener
 * @return @ref zos_result_t The result of the API call
 * @zos{level=1}
 */
zos_result_t s2c_unregister_listener(const char *stream);

/**
 * @brief Request a streams current value from the cloud
 *
 * This retrieves the latest value the cloud currently has stored
 * for the corresponding stream. This is typically used after
 * opening a cloud connection to update the device's output
 * components to their correct value.
 *
 * @note A cloud connection must be opened before calling this API
 *
 * @param stream Name of the stream for which to retrieve latest value from cloud
 * @return @ref zos_result_t The result of the API call
 * @zos{level=1}
 */
zos_result_t s2c_request_stream_data(const char *stream);

/**
 * @brief Return the number of connected local server clients
 *
 * @return The number of locally connected clients to the TCP server
 * @zos{level=1}
 */
uint8_t s2c_get_connection_count(void);

/**
 *
 * @brief Return if the connected to the cloud
 *
 * @return ZOS_TRUE if connected to cloud, ZOS_FALSE else
 * @zos{level=1}
 */
zos_bool_t s2c_cloud_connection_is_active(void);



#include "s2c_command.h"
#include "s2c_file.h"
#include "s2c_util.h"

/**
 * @}
 */
