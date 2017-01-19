/*
 * ZentriOS SDK LICENSE AGREEMENT | Zentri.com, 2016.
 *
 * Use of source code and/or libraries contained in the ZentriOS SDK is
 * subject to the Zentri Operating System SDK license agreement and
 * applicable open source license agreements.
 *
 */

#pragma once



#include "arducam_types.h"


zos_result_t arducam_init(const arducam_config_t *config, arducam_type_t type);

zos_result_t arducam_deinit(void);

zos_result_t adrucam_start_capture(void);

zos_bool_t adrucam_is_image_ready(void);

zos_result_t adrucam_start_reading_image(void);

zos_result_t adrucam_abort_capture(void);

zos_result_t adrucam_set_setting(arducam_setting_type_t setting, uint32_t value);

zos_result_t adrucam_get_setting(arducam_setting_type_t setting, uint32_t *value_ptr);

