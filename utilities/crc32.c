/*
 * ZentriOS SDK LICENSE AGREEMENT | Zentri.com, 2016.
 *
 * Use of source code and/or libraries contained in the ZentriOS SDK is
 * subject to the Zentri Operating System SDK license agreement and
 * applicable open source license agreements.
 *
 */

#include "crc32.h"



 static uint32_t *crc32_table;


/*************************************************************************************************/
void crc32_init(void)
{
    /* Set up the table, if necessary. */
    if(crc32_table == NULL)
    {
        uint32_t byte, crc, mask;

        crc32_table = malloc(sizeof(uint32_t)*256);

        for (byte = 0; byte <= 255; byte++)
        {
            crc = byte;
            for (int j = 7; j >= 0; --j)  // Do eight times.
            {
                mask = -(crc & 1);
                crc = (crc >> 1) ^ (0xEDB88320 & mask);
            }
            crc32_table[byte] = crc;
        }
    }
}

/*************************************************************************************************/
void crc32_deinit(void)
{
    if(crc32_table != NULL)
    {
        free(crc32_table);
        crc32_table = NULL;
    }
}

/*************************************************************************************************/
uint32_t crc32(const uint8_t *message, uint32_t length)
{
    return crc32_with_value(message, length, 0);
}


/*************************************************************************************************/
uint32_t crc32_with_value(const uint8_t *message, uint32_t length, uint32_t previous_value)
{
   uint32_t crc = ~previous_value;
   const uint8_t *byte = message;

   crc32_init();


   /* Through with table setup, now calculate the CRC. */
   for(int c = length; c > 0; --c)
   {
      crc = (crc >> 8) ^ crc32_table[(crc ^ *byte++) & 0xFF];
   }
   return ~crc;
}
