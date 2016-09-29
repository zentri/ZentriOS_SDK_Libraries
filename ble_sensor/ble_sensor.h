/*
 * ZentriOS SDK LICENSE AGREEMENT | Zentri.com, 2016.
 *
 * Use of source code and/or libraries contained in the ZentriOS SDK is
 * subject to the Zentri Operating System SDK license agreement and
 * applicable open source license agreements.
 *
 */
#pragma once
#include "zos.h"

/**
 * @brief Instantiate a BLE sensor descriptor with the given name and UUID
 * @def BLE_SENSOR_DESCRIPTOR( name, descriptor_uuid, write_handler, data_handler
 */
#define BLE_SENSOR_DESCRIPTOR( name, descriptor_uuid, write_handler, data_handler ) \
static ble_sensor_descriptor_data_t descriptor_data_ ## name; \
const ble_sensor_descriptor_t descriptor_ ## name = \
{ \
    .uuid = descriptor_uuid, \
    .write_callback = write_handler, \
    .data_callback = data_handler, \
    .data = &descriptor_data_ ## name \
}

/**
 * @brief Instantiate a BLE sensor characteristic with the given name and UUID
 * @def BLE_SENSOR_CHARACTERISTIC( name, characteristic_uuid, write_handler, data_handler, notification_handler )
 */
#define BLE_SENSOR_CHARACTERISTIC( name, characteristic_uuid, write_handler, data_handler, notification_handler ) \
static ble_sensor_characteristic_data_t characteristic_data_ ## name; \
const ble_sensor_characteristic_t characteristic_ ## name = \
{ \
    .uuid = characteristic_uuid, \
    .write_callback = write_handler, \
    .data_callback = data_handler, \
    .notification_callback = notification_handler, \
    .data = &characteristic_data_ ## name, \
    .descriptors = \
    { \
        NULL \
    } \
}

/**
 * @brief Open instantiation of a BLE Sensor Characteristic that may include descriptor(s).
 * @def BLE_SENSOR_CHARACTERISTIC_START( name, characteristic_uuid, write_handler, data_handler, notification_handler )
 */
#define BLE_SENSOR_CHARACTERISTIC_START( name, characteristic_uuid, write_handler, data_handler, notification_handler ) \
static ble_sensor_characteristic_data_t characteristic_data_ ## name; \
const ble_sensor_characteristic_t characteristic_ ## name = \
{ \
    .uuid = characteristic_uuid, \
    .write_callback = write_handler, \
    .data_callback = data_handler, \
    .notification_callback = notification_handler, \
    .data = &characteristic_data_ ## name, \
    .descriptors = \
    {

/**
 * @brief Add a descriptor instance to the characteristic with the given name
 * @def BLE_SENSOR_ADD_DESCRIPTOR( name )
 */
#define BLE_SENSOR_ADD_DESCRIPTOR( name )  (ble_sensor_descriptor_t*)&descriptor_ ## name,

/**
 * @brief Close an instantiation of a BLE sensor characteristic
 * @def BLE_SENSOR_CHARACTERISTIC_END
 */
#define BLE_SENSOR_CHARACTERISTIC_END  NULL } }

/**
 * @brief Start instantiation of a BLE sensor service with the given name and UUID
 * @note Use @ref BLE_SENSOR_ADD_CHARACTERISTIC to add characteristics
 * @def BLE_SENSOR_SERVICE_START( name, service_uuid )
 */
#define BLE_SENSOR_SERVICE_START( name, service_uuid ) \
static ble_sensor_service_data_t service_data_ ## name; \
const ble_sensor_service_t service_ ## name = \
{ \
    .uuid = service_uuid, \
    .data = &service_data_ ## name, \
    .characteristics = \
    {

/**
 * @brief Add a characteristic instance to the service identified by the given name
 * @def BLE_SENSOR_ADD_CHARACTERISTIC( name )
 */
#define BLE_SENSOR_ADD_CHARACTERISTIC( name )  (ble_sensor_characteristic_t*)&characteristic_ ## name,

/**
 * @brief Close the definition of a BLE sensor service
 * @def BLE_SENSOR_SERVICE_END
 */
#define BLE_SENSOR_SERVICE_END  NULL } }

/**
 * @brief Instantiate a BLE sensor identified by the given name
 * @note Use @ref BLE_SENSOR_ADD_SERVICE to add services to the sensor
 * @def BLE_SENSOR_START( name, ready_handler, disconnect_handler )
 */
#define BLE_SENSOR_START( name, ready_handler, disconnect_handler ) \
ble_sensor_t ble_sensor_ ## name = \
{ \
    .connection = 0, \
    .address.type = 0, \
    .address.address.octet = { 0 }, \
    .ready_callback = ready_handler, \
    .disconnect_callback = disconnect_handler, \
    .service_index = 0, \
    .characteristic_index = 0, \
    .descriptor_index = 0, \
    .services = \
    {

/**
 * @brief Add a BLE sensor service to the sensor identified by name
 * @def BLE_SENSOR_ADD_SERVICE( name )
 */
#define BLE_SENSOR_ADD_SERVICE( name )  (ble_sensor_service_t*)&service_ ## name,

/**
 * @brief Close a BLE sensor instantiation
 * @def BLE_SENSOR_END
 */
#define BLE_SENSOR_END  NULL } }

/**
 * @brief Publicly declare a sensor instance
 * @def BLE_SENSOR_DECLARE( name )
 */
#define BLE_SENSOR_DECLARE( name )  extern ble_sensor_t ble_sensor_ ## name;

/**
 * @brief Publicly declare a BLE sensor service instance
 * @def BLE_SENSOR_DECLARE_SERVICE( name )
 */
#define BLE_SENSOR_DECLARE_SERVICE( name )  extern const ble_sensor_service_t service_ ## name;

/**
 * @brief Publicly declare a BLE sensor characteristic instance
 * @def BLE_SENSOR_DECLARE_CHARACTERISTIC( name )
 */
#define BLE_SENSOR_DECLARE_CHARACTERISTIC( name )  extern const ble_sensor_characteristic_t characteristic_ ## name;

/**
 * @brief Publicly declare a BLE sensor descriptor instance
 * @def BLE_SENSOR_DECLARE_DESCRIPTOR( name )
 */
#define BLE_SENSOR_DECLARE_DESCRIPTOR( name )  extern const ble_sensor_descriptor_t descriptor_ ## name;

/**
 * @brief Reference a BLE sensor instance with the given name
 * @def BLE_SENSOR_GET( name )
 */
#define BLE_SENSOR_GET( name )  ((ble_sensor_t*)(&ble_sensor_ ## name))

/**
 * @brief Reference a BLE sensor service instance with the given name
 * @def BLE_SENSOR_GET_SERVICE( name )
 */
#define BLE_SENSOR_GET_SERVICE( name )  ((ble_sensor_service_t*)(&service_ ## name))

/**
 * @brief Reference a BLE sensor characteristic instance with the given name
 * @def BLE_SENSOR_GET_CHARACTERISTIC( name )
 */
#define BLE_SENSOR_GET_CHARACTERISTIC( name )  ((ble_sensor_characteristic_t*)(&characteristic_ ## name))

/**
 * @brief Reference a BLE sensor descriptor instance with the given name
 * @def BLE_SENSOR_GET_DESCRIPTOR( name )
 */
#define BLE_SENSOR_GET_DESCRIPTOR( name )  ((ble_sensor_descriptor_t*)(&descriptor_ ## name))

/**
 * BLE sensor notification callback result type
 */
typedef enum
{
    BLE_SENSOR_DISCARD_NOTIFICATION, //<<< Discard notification
    BLE_SENSOR_PROCESS_NOTIFICATION, //<<< Process and defer notification data to data receive handler
} ble_sensor_notification_result_t;

#pragma pack(1)
/**
 * BLE sensor descriptor data
 */
typedef struct
{
    uint16_t handle; //<<< descriptor handle
    uint16_t length; //<<< data length in bytes
    uint8_t* value;  //<<< pointer to data
} ble_sensor_descriptor_data_t;

/**
 * BLE sensor characteristic data
 */
typedef struct
{
    zos_gatt_group_handle_t group_handle;
    uint16_t                value_handle;
    uint8_t                 properties;
    uint16_t                length;
    uint8_t*                value;
} ble_sensor_characteristic_data_t;

/**
 * BLE sensor service data
 */
typedef struct
{
    zos_gatt_group_handle_t group_handle;
} ble_sensor_service_data_t;

/**
 * BLE sensor descriptor
 */
typedef struct ble_sensor_descriptor
{
    zos_bt_uuid_t                 uuid;
    void                          (*data_callback) ( struct ble_sensor_descriptor* descriptor, ble_sensor_descriptor_data_t* data );
    void                          (*write_callback)( struct ble_sensor_descriptor* descriptor, zos_gatt_status_t status );
    ble_sensor_descriptor_data_t* data;
} ble_sensor_descriptor_t;

typedef void (*ble_sensor_descriptor_data_callback_t)( ble_sensor_descriptor_t* descriptor, ble_sensor_descriptor_data_t* data );
typedef void (*ble_sensor_descriptor_write_callback_t)( ble_sensor_descriptor_t* descriptor, zos_gatt_status_t status );

/**
 * BLE sensor characteristic
 */
typedef struct ble_sensor_characteristic
{
    zos_bt_uuid_t                     uuid;
    void                              (*data_callback) ( struct ble_sensor_characteristic* characteristic, ble_sensor_characteristic_data_t* data );
    void                              (*write_callback)( struct ble_sensor_characteristic* characteristic, zos_gatt_status_t status );
    ble_sensor_notification_result_t  (*notification_callback)( struct ble_sensor_characteristic* characteristic, uint16_t length, const uint8_t* data );
    ble_sensor_characteristic_data_t* data;
    ble_sensor_descriptor_t*          descriptors[];
} ble_sensor_characteristic_t;

typedef void (*ble_sensor_characteristic_data_callback_t)( ble_sensor_characteristic_t* characteristic, ble_sensor_characteristic_data_t* data );
typedef void (*ble_sensor_characteristic_write_callback_t)( ble_sensor_characteristic_t* characteristic, zos_gatt_status_t status );
typedef ble_sensor_notification_result_t (*ble_sensor_characteristic_notification_callback_t)( ble_sensor_characteristic_t* characteristic, uint16_t length, const uint8_t* data );

/**
 * BLE sensor service
 */
typedef struct
{
    zos_bt_uuid_t                uuid;
    ble_sensor_service_data_t*   data;
    ble_sensor_characteristic_t* characteristics[];
} ble_sensor_service_t;

/**
 * BLE sensor
 */
typedef struct ble_sensor
{
    zos_ble_connection_t  connection;
    zos_ble_address_t     address;
    void                  (*ready_callback)( struct ble_sensor* sensor );
    void                  (*disconnect_callback)( struct ble_sensor* sensor );
    int32_t               service_index;
    int32_t               characteristic_index;
    int32_t               descriptor_index;
    ble_sensor_service_t* services[];
} ble_sensor_t;
#pragma pack()

typedef void (*ble_sensor_ready_callback_t)( ble_sensor_t* sensor );
typedef void (*ble_sensor_disconnected_callback_t)( ble_sensor_t* sensor );

/**
 * Connect to a BLE sensor
 *
 * @param[in] sensor  BLE sensor instance
 * @param[in] address BLE address of sensor to connect to
 * @return @ref zos_result_t
 */
zos_result_t ble_sensor_connect( ble_sensor_t* sensor, const zos_ble_address_t* address );

/**
 * Disconnect from a BLE sensor
 *
 * @param[in] sensor  BLE sensor instance to disconnect from
 * @return @ref zos_result_t
 */
zos_result_t ble_sensor_disconnect( ble_sensor_t* sensor );

/**
 * Read data from a BLE sensor characteristic. Result will be received via the characteristic data callback.
 *
 * @param[in] sensor         BLE sensor instance
 * @param[in] characteristic BLE sensor characteristic instance to read
 * @return @ref zos_result_t
 */
zos_result_t ble_sensor_read_characteristic( ble_sensor_t* sensor, ble_sensor_characteristic_t* characteristic );

/**
 * Write data to a BLE sensor characteristic. Result will be reported via the characteristic write callback.
 *
 * @param[in] sensor         BLE sensor instance
 * @param[in] characteristic BLE sensor characteristic instance to write data to
 * @param[in] data           data to write
 * @return @ref zos_result_t
 */
zos_result_t ble_sensor_write_characteristic( ble_sensor_t* sensor, ble_sensor_characteristic_t* characteristic, ble_sensor_characteristic_data_t* data );

/**
 * Read data from a BLE sensor descriptor. Result will be received via the descriptor data callback.
 *
 * @param[in] sensor         BLE sensor instance
 * @param[in] characteristic BLE sensor descriptor instance to read from
 * @return @ref zos_result_t
 */
zos_result_t ble_sensor_read_descriptor( ble_sensor_t* sensor, ble_sensor_descriptor_t* descriptor );

/**
 * Write data to a BLE sensor descriptor. Result will be reported via the descriptor write callback.
 *
 * @param[in] sensor         BLE sensor instance
 * @param[in] characteristic BLE sensor descriptor instance to write data to
 * @param[in] data           data to write
 * @return @ref zos_result_t
 */
zos_result_t ble_sensor_write_descriptor( ble_sensor_t* sensor, ble_sensor_descriptor_t* descriptor, ble_sensor_descriptor_data_t* data );

/**
 * Enable notification on the characteristic that includes the given Client Characteristic Configuration (CCC) descriptor
 *
 * @param[in] sensor BLE sensor instance
 * @param[in] ccc    CCC descriptor instance
 * @return @ref zos_result_t
 */
zos_result_t ble_sensor_enable_notification( ble_sensor_t* sensor, ble_sensor_descriptor_t* ccc );

/**
 * Disable notification on the characteristic that includes the given CCC descriptor
 *
 * @param[in] sensor BLE sensor instance
 * @param[in] ccc    CCC descriptor instance
 * @return @ref zos_result_t
 */
zos_result_t ble_sensor_disable_notification( ble_sensor_t* sensor, ble_sensor_descriptor_t* ccc );

