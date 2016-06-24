/*
 * ZentriOS SDK LICENSE AGREEMENT | Zentri.com, 2016.
 *
 * Use of source code and/or libraries contained in the ZentriOS SDK is
 * subject to the Zentri Operating System SDK license agreement and
 * applicable open source license agreements.
 *
 */

#pragma once



zos_result_t s2c_commands_init(uint32_t setting_magic_number);

void s2c_file_commands_init(void);

zos_bool_t s2c_is_admin_command(const char *cmd);
