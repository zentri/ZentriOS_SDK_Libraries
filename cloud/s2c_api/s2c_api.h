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
#include "s2c_streams.h"
#include "s2c_network.h"
#include "s2c_commands.h"
#include "s2c_mobile.h"
#include "s2c_cloud.h"




typedef enum
{
    S2C_NETWORK_STATE_SETUP_MODE,
    S2C_NETWORK_STATE_JOINING,
    S2C_NETWORK_STATE_LOCAL_MODE,
    S2C_NETWORK_STATE_CLOUD_CONNECTING,
    S2C_NETWORK_STATE_CLOUD_MODE
} s2c_network_state_t;

typedef enum
{
    S2C_INTERNET_STATE_PENDING,
    S2C_INTERNET_STATE_VALID,
    S2C_INTERNET_STATE_FAILED
} s2c_internet_state_t;

#ifndef S2C_CUSTOM_APP_SETTINGS_PATH
typedef struct
{
    uint32_t magic_number;
    struct
    {
        char url[32];
        char token[36];
    } cloud;
    struct
    {
        uint8_t data[S2C_DEVICE_KEY_SIZE];
        zos_bool_t valid;
    } device_key;

    char device_title[S2C_DEVICE_TITLE_SIZE];
    char device_code[S2C_DEVICE_CODE_SIZE];
    char caps_filename[24];
    s2c_flag_t flags;
} s2c_app_settings_t;
#else
#include S2C_CUSTOM_APP_SETTINGS_PATH
#endif

typedef struct
{
    s2c_app_settings_t *settings;
    s2c_network_state_t state;
    zos_bool_t initialized;
    zos_bool_t streams_started;
    s2c_internet_state_t internet_state;
    zos_bool_t forced_setup_mode;
    zos_bool_t forced_cloud_disconnect;
    uint8_t network_retry_count;
    uint8_t cloud_retry_count;
    uint8_t softap_retry_count;
    char cloud_domain[32];
} s2c_app_context_t;




extern s2c_app_context_t s2c_app_context;

zos_result_t s2c_api_init(const s2c_config_t config);
