/*
 * ZentriOS SDK LICENSE AGREEMENT | Zentri.com, 2016.
 *
 * Use of source code and/or libraries contained in the ZentriOS SDK is
 * subject to the Zentri Operating System SDK license agreement and
 * applicable open source license agreements.
 *
 */

#include "arducam_drivers.h"



#include "ov2640_driver.c"




static const adrucam_driver_t supported_cameras[ARDUCAM_TYPE_MAX] =
{
        OV2640_DRIVER_CONFIG
};



/*************************************************************************************************/
zos_result_t adrucam_get_driver(arducam_type_t type, const adrucam_driver_t **driver_ptr)
{
    if(type >= ARDUCAM_TYPE_MAX)
    {
        return ZOS_NOT_FOUND;
    }

    *driver_ptr = &supported_cameras[type];

    return ZOS_SUCCESS;
}
