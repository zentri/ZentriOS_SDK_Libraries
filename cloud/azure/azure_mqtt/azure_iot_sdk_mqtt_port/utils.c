/*
 * ZentriOS SDK LICENSE AGREEMENT | Zentri.com, 2016.
 *
 * Use of source code and/or libraries contained in the ZentriOS SDK is
 * subject to the Zentri Operating System SDK license agreement and
 * applicable open source license agreements.
 *
 */


#include "zos.h"
#include "azure_c_shared_utility/strings.h"
#include "azure_c_shared_utility/base64.h"
#include "azure_c_shared_utility/hmacsha256.h"



/*************************************************************************************************/
BUFFER_HANDLE Base64_Decoder(const char* source)
{
    BUFFER_HANDLE result = NULL;
    int retval ;
    const int slen = strlen(source);
    uint8_t *buffer = NULL;

    uint32_t dlen = (slen / 4)*3;

    if((buffer = malloc(dlen)) == NULL)
    {
    }
    else if((retval = base64_decode(buffer, &dlen, (void*)source, slen)) != 0)
    {

    }
    else if((result = BUFFER_create(buffer, dlen)) == NULL)
    {
    }

    if(buffer != NULL)
    {
        free(buffer);
    }

    return result;
}

/*************************************************************************************************/
STRING_HANDLE Base64_Encoder(BUFFER_HANDLE input)
{
    STRING_HANDLE result = NULL;
    size_t inputSize;
    const unsigned char* inputBinary;
    uint32_t neededSize;
    uint8_t* encoded;
    int retval;

    BUFFER_content(input, &inputBinary);
    BUFFER_size(input, &inputSize);

    neededSize = ((((inputSize - 1) / 3) + 1) * 4);
    neededSize += 1;

    encoded = (void*)malloc(neededSize);
    if (encoded != NULL)
    {
        if((retval = base64_encode(encoded, &neededSize, inputBinary, inputSize)) == 0)
        {
            encoded[neededSize] = 0;
            result = STRING_new_with_memory((char*)encoded);
        }
        else
        {
            free(encoded);
        }
    }

    return result;
}

/*************************************************************************************************/
HMACSHA256_RESULT HMACSHA256_ComputeHash(const unsigned char* key,
                                         size_t keyLen,
                                         const unsigned char* payload,
                                         size_t payloadLen,
                                         BUFFER_HANDLE hash)
{
    HMACSHA256_RESULT result;

    if (BUFFER_enlarge(hash, 32) != 0)
    {
        result = HMACSHA256_ERROR;
    }
    else
    {
        const zos_buffer_t key_buffer =
        {
                .data = (void*)key,
                .size = keyLen
        };
        const zos_buffer_t payload_buffer =
        {
                .data = (void*)payload,
                .size = payloadLen
        };
        sha2_hmac(&key_buffer, &payload_buffer, BUFFER_u_char(hash), 0);
        result = HMACSHA256_OK;
    }

    return result;
}

/*************************************************************************************************/
void *realloc( void *ptr, size_t new_size )
{
    void *new_ptr = malloc(new_size);
    if(new_ptr != NULL)
    {
        memcpy(new_ptr, ptr, new_size);
    }
    free(ptr);

    return new_ptr;
}
