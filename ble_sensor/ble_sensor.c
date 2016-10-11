/*
 * ZentriOS SDK LICENSE AGREEMENT | Zentri.com, 2015.
 *
 * Use of source code and/or libraries contained in the ZentriOS SDK is
 * subject to the Zentri Operating System SDK license agreement and
 * applicable open source license agreements.
 *
 */

#include "zos.h"
#include "ble_sensor.h"

#define CURRENT_SERVICE( sensor )        sensor->services[sensor->service_index]
#define CURRENT_CHARACTERISTIC( sensor ) sensor->services[sensor->service_index]->characteristics[sensor->characteristic_index]
#define CURRENT_DESCRIPTOR( sensor )     sensor->services[sensor->service_index]->characteristics[sensor->characteristic_index]->descriptors[sensor->descriptor_index]

typedef enum
{
    CHARACTERISTIC_WITH_DESCRIPTORS = 1,
    CHARACTERISTIC_READABLE = 2,
} characteristic_flag_t;

typedef struct
{
    ble_sensor_characteristic_t* characteristic;
    uint16_t length;
    uint8_t  data[0];
} ble_sensor_notification_data_t;

/* Bluetooth stack callbacks */
static void connect_callback                     ( zos_ble_connection_t connection, zos_result_t result, void* arg );
static void disconnect_callback                  ( zos_ble_connection_t connection, void* arg );
static void discover_service_callback            ( zos_ble_connection_t connection, zos_gatt_status_t status, const zos_gatt_service_t* service, void* arg );
static void discover_characteristic_callback     ( zos_ble_connection_t connection, zos_gatt_status_t status, const zos_gatt_characteristic_t* characteristic, void* arg );
static void discover_descriptor_callback         ( zos_ble_connection_t connection, zos_gatt_status_t status, const zos_gatt_descriptor_t* descriptor, void* arg );
static void discover_read_characteristic_callback( zos_ble_connection_t connection, zos_gatt_status_t status, const zos_gatt_attribute_t* attribute, void* arg );
static void discover_read_descriptor_callback    ( zos_ble_connection_t connection, zos_gatt_status_t status, const zos_gatt_attribute_t* attribute, void* arg );
static void notification_callback                ( zos_ble_connection_t connection, const zos_gatt_attribute_t* attribute, void* arg );

/* Internal discovery functions */
static void discover_characteristics( ble_sensor_t* sensor, ble_sensor_service_t* service );
static void discover_descriptors    ( ble_sensor_t* sensor, ble_sensor_characteristic_t* characteristic );

/* Internal attribute helper functions */
static void                         reset_discovery_indices( ble_sensor_t* sensor );
static ble_sensor_service_t*        get_next_service       ( ble_sensor_t* sensor );
static ble_sensor_characteristic_t* get_next_characteristic( ble_sensor_t* sensor, characteristic_flag_t flag );
static ble_sensor_descriptor_t*     get_next_descriptor    ( ble_sensor_t* sensor );

static const zos_ble_connection_callbacks_t connection_callbacks =
{
    .connect_callback    = connect_callback,
    .disconnect_callback = disconnect_callback,
};

zos_result_t ble_sensor_connect( ble_sensor_t* sensor, const zos_ble_address_t* address )
{
    zn_ble_register_connection_callbacks( &connection_callbacks );
    memcpy( &sensor->address, address, sizeof(zos_ble_address_t) );
    return zn_ble_connect( address, &sensor->connection, (void*)sensor );
}

zos_result_t ble_sensor_disconnect( ble_sensor_t* sensor )
{
    return zn_ble_disconnect( sensor->connection );
}

static void read_characteristic_callback( zos_ble_connection_t connection, zos_gatt_status_t status, const zos_gatt_attribute_t* attribute, void* arg )
{
    ble_sensor_characteristic_t* characteristic = (ble_sensor_characteristic_t*)arg;

    if ( status == ZOS_GATT_SUCCESS )
    {
        memcpy( characteristic->data->value, attribute->value, characteristic->data->length );
    }
    if ( characteristic->data_callback != NULL )
    {
        characteristic->data_callback( characteristic, characteristic->data );
    }
}

zos_result_t ble_sensor_read_characteristic( ble_sensor_t* sensor, ble_sensor_characteristic_t* characteristic )
{
    return zn_gatt_read( sensor->connection, characteristic->data->value_handle, read_characteristic_callback, (void*)characteristic );
}

static void write_characteristic_callback( zos_ble_connection_t connection, zos_gatt_status_t status, uint16_t value_handle, void* arg )
{
    ble_sensor_characteristic_t* characteristic = (ble_sensor_characteristic_t*)arg;

    if ( characteristic->write_callback != NULL )
    {
        characteristic->write_callback( characteristic, status );
    }
}

zos_result_t ble_sensor_write_characteristic( ble_sensor_t* sensor, ble_sensor_characteristic_t* characteristic, ble_sensor_characteristic_data_t* data )
{
    const zos_gatt_write_parameters_t parameters =
    {
        .handle = data->value_handle,
        .value  = data->value,
        .length = data->length
    };
    return zn_gatt_write( sensor->connection, &parameters, write_characteristic_callback, (void*)characteristic );
}

static void read_descriptor_callback( zos_ble_connection_t connection, zos_gatt_status_t status, const zos_gatt_attribute_t* attribute, void* arg )
{
    ble_sensor_descriptor_t* descriptor = (ble_sensor_descriptor_t*)arg;

    if ( status == ZOS_GATT_SUCCESS )
    {
        memcpy( descriptor->data->value, attribute->value, descriptor->data->length );
    }
    if ( descriptor->data_callback != NULL )
    {
        descriptor->data_callback( descriptor, descriptor->data );
    }
}

zos_result_t ble_sensor_read_descriptor( ble_sensor_t* sensor, ble_sensor_descriptor_t* descriptor )
{
    return zn_gatt_read( sensor->connection, descriptor->data->handle, read_descriptor_callback, (void*)descriptor );
}

static void write_descriptor_callback( zos_ble_connection_t connection, zos_gatt_status_t status, uint16_t value_handle, void* arg )
{
    ble_sensor_descriptor_t* descriptor = (ble_sensor_descriptor_t*)arg;

    if ( descriptor->write_callback != NULL )
    {
        descriptor->write_callback( descriptor, status );
    }
}

zos_result_t ble_sensor_write_descriptor( ble_sensor_t* sensor, ble_sensor_descriptor_t* descriptor, ble_sensor_descriptor_data_t* data )
{
    const zos_gatt_write_parameters_t parameters =
    {
        .handle = data->handle,
        .value  = data->value,
        .length = data->length
    };
    return zn_gatt_write( sensor->connection, &parameters, write_descriptor_callback, (void*)descriptor );

}

zos_result_t ble_sensor_enable_notification( ble_sensor_t* sensor, ble_sensor_descriptor_t* ccc )
{
    ccc->data->value[0] |= 1;
    return ble_sensor_write_descriptor( sensor, ccc, ccc->data );
}

zos_result_t ble_sensor_disable_notification( ble_sensor_t* sensor, ble_sensor_descriptor_t* ccc )
{
    ccc->data->value[0] &= 0xfe;
    return ble_sensor_write_descriptor( sensor, ccc, ccc->data );
}

static void connect_callback( zos_ble_connection_t connection, zos_result_t result, void* arg )
{
    zn_gatt_register_notification_callback( connection, notification_callback, arg );
    if ( zn_gatt_discover_all_primary_services( connection, discover_service_callback, arg ) != ZOS_SUCCESS )
    {
        ZOS_LOG( "Discovery failed" );
    }
}

static void disconnect_callback( zos_ble_connection_t connection, void* arg )
{
    ble_sensor_t* sensor = (ble_sensor_t*)arg;
    if ( sensor->disconnect_callback != NULL )
    {
        sensor->disconnect_callback( sensor );
    }
    sensor->connection           = 0;
    sensor->service_index        = 0;
    sensor->characteristic_index = 0;
    sensor->descriptor_index     = 0;
    memset( &sensor->address, 0, sizeof( sensor->address ) );
}

static void discover_service_callback( zos_ble_connection_t connection, zos_gatt_status_t status, const zos_gatt_service_t* service, void* arg )
{
    ble_sensor_t* sensor = (ble_sensor_t*)arg;

    if ( service == NULL )
    {
        /* Service discovery completed. Discover characteristics now */
        reset_discovery_indices( sensor );
        discover_characteristics( sensor, get_next_service( sensor ) );
        return;
    }

    for ( uint32_t a = 0; sensor->services[a] != NULL; a++ )
    {
        ble_sensor_service_t* current = sensor->services[a];
        if ( memcmp( &current->uuid.value, &service->uuid.value, service->uuid.type ) == 0 )
        {
            current->data->group_handle = service->group_handle;
        }
    }
}

static void discover_characteristic_callback( zos_ble_connection_t connection, zos_gatt_status_t status, const zos_gatt_characteristic_t* characteristic, void* arg )
{
    ble_sensor_t* sensor = (ble_sensor_t*)arg;

    if ( characteristic == NULL )
    {
        ble_sensor_service_t* service = get_next_service( sensor );
        if ( service != NULL )
        {
            discover_characteristics( sensor, service );
        }
        else
        {
            reset_discovery_indices( sensor );
            discover_descriptors( sensor, get_next_characteristic( sensor, CHARACTERISTIC_WITH_DESCRIPTORS ) );
        }
        return;
    }

    for ( uint32_t a = 0; sensor->services[sensor->service_index]->characteristics[a] != NULL; a++ )
    {
        ble_sensor_characteristic_t* current = (ble_sensor_characteristic_t*)sensor->services[sensor->service_index]->characteristics[a];
        if ( memcmp( &current->uuid.value, &characteristic->uuid.value, characteristic->uuid.type ) == 0 )
        {
            current->data->group_handle = characteristic->group_handle;
            current->data->properties   = characteristic->properties;
            current->data->value_handle = characteristic->value_handle;
        }
    }
}

static void discover_descriptor_callback( zos_ble_connection_t connection, zos_gatt_status_t status, const zos_gatt_descriptor_t* descriptor, void* arg )
{
    ble_sensor_t* sensor = (ble_sensor_t*)arg;

    if ( descriptor == NULL )
    {
        ble_sensor_characteristic_t* characteristic = get_next_characteristic( sensor, CHARACTERISTIC_WITH_DESCRIPTORS );
        if ( characteristic != NULL )
        {
            discover_descriptors( sensor, characteristic );
        }
        else
        {
            reset_discovery_indices( sensor );
            characteristic = get_next_characteristic( sensor, CHARACTERISTIC_READABLE );
            if ( characteristic != NULL )
            {
                zn_gatt_read( connection, characteristic->data->value_handle, discover_read_characteristic_callback, arg );
            }
        }
        return;
    }

    for ( uint32_t a = 0; sensor->services[sensor->service_index]->characteristics[sensor->characteristic_index]->descriptors[a] != NULL; a++ )
    {
        ble_sensor_descriptor_t* current = (ble_sensor_descriptor_t*)sensor->services[sensor->service_index]->characteristics[sensor->characteristic_index]->descriptors[a];
        if ( memcmp( &current->uuid.value, &descriptor->uuid.value, current->uuid.type ) == 0 )
        {
            current->data->handle = descriptor->handle;
        }
    }
}

static void discover_read_characteristic_callback( zos_ble_connection_t connection, zos_gatt_status_t status, const zos_gatt_attribute_t* attribute, void* arg )
{
    ble_sensor_t* sensor = (ble_sensor_t*)arg;
    ble_sensor_characteristic_t* characteristic = NULL;

    /* Store read characteristic value */
    if ( ( status == ZOS_GATT_SUCCESS ) && ( attribute != NULL ) && ( attribute->length != 0 ) )
    {
        characteristic = (ble_sensor_characteristic_t*)CURRENT_CHARACTERISTIC( sensor );

        zn_malloc( &characteristic->data->value, attribute->length );
        if ( characteristic->data->value != NULL )
        {
            memcpy( characteristic->data->value, attribute->value, attribute->length );
            characteristic->data->length = attribute->length;
            if ( characteristic->data_callback != NULL )
            {
                /* Notify app that new data is available */
                characteristic->data_callback( characteristic, characteristic->data );
            }
        }
    }

    characteristic = get_next_characteristic( sensor, CHARACTERISTIC_READABLE );
    if ( characteristic != NULL )
    {
        /* Read next characteristic */
        zn_gatt_read( connection, characteristic->data->value_handle, discover_read_characteristic_callback, arg );
    }
    else
    {
        /* Characteristic read complete. Start reading descriptor value */
        ble_sensor_descriptor_t* descriptor = NULL;

        reset_discovery_indices( sensor );
        descriptor = get_next_descriptor( sensor );
        if ( descriptor != NULL )
        {
            zn_gatt_read( connection, descriptor->data->handle, discover_read_descriptor_callback, arg );
        }
    }
}

static void discover_read_descriptor_callback( zos_ble_connection_t connection, zos_gatt_status_t status, const zos_gatt_attribute_t* attribute, void* arg )
{
    ble_sensor_t* sensor = (ble_sensor_t*)arg;
    ble_sensor_descriptor_t* descriptor = NULL;

    if ( ( status == ZOS_GATT_SUCCESS ) && ( attribute != NULL ) && ( attribute->length != 0 ) )
    {
        /* Store read descriptor value */
        descriptor = (ble_sensor_descriptor_t*)CURRENT_DESCRIPTOR( sensor );

        zn_malloc( &descriptor->data->value, attribute->length );
        if ( descriptor->data->value != NULL )
        {
            memcpy( descriptor->data->value, attribute->value, attribute->length );
            descriptor->data->length = attribute->length;
            if ( descriptor->data_callback != NULL )
            {
                /* Notify app that new data available */
                descriptor->data_callback( descriptor, descriptor->data );
            }
        }
    }

    descriptor = get_next_descriptor( sensor );
    if ( descriptor != NULL )
    {
        /* Read next descriptor */
        zn_gatt_read( connection, descriptor->data->handle, discover_read_descriptor_callback, arg );
    }
    else if ( sensor->ready_callback != NULL )
    {
        /* Discovery complete */
        sensor->ready_callback( sensor );
    }
}

static void notification_app_callback( void* arg )
{
    ble_sensor_notification_data_t* data = (ble_sensor_notification_data_t*)arg;

    if ( ( data->characteristic->data->value != NULL ) && ( data->characteristic->data->length < data->length ) )
    {
        zn_free( data->characteristic->data->value );
        data->characteristic->data->value  = NULL;
        data->characteristic->data->length = 0;
    }
    if ( ( data->characteristic->data->value == NULL ) && ( data->characteristic->data->length == data->length ) )
    {
        zn_malloc( &data->characteristic->data->value, data->length );
    }
    if ( data->length != 0 )
    {
        memcpy( data->characteristic->data->value, data->data, data->length );
        data->characteristic->data->length = data->length;
    }
    if ( data->characteristic->data_callback != NULL )
    {
        data->characteristic->data_callback( data->characteristic, data->characteristic->data );
    }
    zn_free( data );
}

static void notification_callback( zos_ble_connection_t connection, const zos_gatt_attribute_t* attribute, void* arg )
{
    ble_sensor_t* sensor = (ble_sensor_t*)arg;

    for ( uint32_t a = 0; sensor->services[a] != NULL; a++ )
    {
        ble_sensor_service_t* service = (ble_sensor_service_t*)sensor->services[a];

        for ( uint32_t b = 0; service->characteristics[b]; b++ )
        {
            ble_sensor_characteristic_t* characteristic = (ble_sensor_characteristic_t*)service->characteristics[b];

            if ( characteristic->data->value_handle == attribute->handle )
            {
                if ( ( characteristic->notification_callback != NULL ) && ( characteristic->notification_callback( characteristic, attribute->length, attribute->value ) == BLE_SENSOR_PROCESS_NOTIFICATION ) )
                {
                    ble_sensor_notification_data_t* data = NULL;

                    zn_malloc( (uint8_t**)&data, sizeof( ble_sensor_notification_data_t ) + attribute->length );
                    if ( data != NULL )
                    {
                        memcpy( data->data, attribute->value, attribute->length );
                        data->length         = attribute->length;
                        data->characteristic = characteristic;
                        zn_event_issue( notification_app_callback, (void*)data, ZOS_EVENT_FLAG_ALLOW_DUPLICATE );
                    }
                }
                return;
            }
        }
    }
}

static void discover_characteristics( ble_sensor_t* sensor, ble_sensor_service_t* service )
{
    const zos_gatt_service_t service_internal =
    {
        .group_handle = service->data->group_handle,
        .uuid         = service->uuid,
    };
    zn_gatt_discover_characteristics( sensor->connection, &service_internal, discover_characteristic_callback, (void*)sensor );
}

static void discover_descriptors( ble_sensor_t* sensor, ble_sensor_characteristic_t* characteristic )
{
    const zos_gatt_characteristic_t characteristic_internal =
    {
        .group_handle = characteristic->data->group_handle,
        .uuid         = characteristic->uuid,
        .value_handle = characteristic->data->value_handle,
        .properties   = characteristic->data->properties
    };
    zn_gatt_discover_descriptors( sensor->connection, &characteristic_internal, discover_descriptor_callback, (void*)sensor );
}

static void reset_discovery_indices( ble_sensor_t* sensor )
{
    sensor->service_index        = -1;
    sensor->characteristic_index = -1;
    sensor->descriptor_index     = -1;
}

static ble_sensor_service_t* get_next_service( ble_sensor_t* sensor )
{
    sensor->service_index++;
    return CURRENT_SERVICE( sensor );
}

static ble_sensor_characteristic_t* get_next_characteristic( ble_sensor_t* sensor, characteristic_flag_t flag )
{
    sensor->service_index = ( sensor->service_index == -1 ) ? 0 : sensor->service_index;
    sensor->characteristic_index++;
    for ( ; CURRENT_SERVICE( sensor ) != NULL; sensor->service_index++ )
    {
        for ( ; CURRENT_CHARACTERISTIC( sensor ) != NULL; sensor->characteristic_index++ )
        {
            if ( ( flag == CHARACTERISTIC_WITH_DESCRIPTORS ) && ( CURRENT_CHARACTERISTIC( sensor )->descriptors[0] != NULL ) )
            {
                return CURRENT_CHARACTERISTIC( sensor );
            }
            else if ( ( flag == CHARACTERISTIC_READABLE ) && ( ( CURRENT_CHARACTERISTIC( sensor )->data->properties & 0x02 ) != 0 ) )
            {
                return CURRENT_CHARACTERISTIC( sensor );
            }
        }
        sensor->characteristic_index = 0;
    }

    return NULL;
}

static ble_sensor_descriptor_t* get_next_descriptor( ble_sensor_t* sensor )
{
    sensor->service_index = ( sensor->service_index == -1 ) ? 0 : sensor->service_index;
    sensor->characteristic_index = ( sensor->characteristic_index == -1 ) ? 0 : sensor->characteristic_index;
    sensor->descriptor_index++;

    for ( ; CURRENT_SERVICE( sensor ) != NULL; sensor->service_index++ )
    {
        for ( ; CURRENT_CHARACTERISTIC( sensor ) != NULL; sensor->characteristic_index++ )
        {
            if ( CURRENT_DESCRIPTOR( sensor ) != NULL )
            {
                return CURRENT_DESCRIPTOR( sensor );
            }
            sensor->descriptor_index = 0;
        }
        sensor->characteristic_index = 0;
    }
    return NULL;
}
