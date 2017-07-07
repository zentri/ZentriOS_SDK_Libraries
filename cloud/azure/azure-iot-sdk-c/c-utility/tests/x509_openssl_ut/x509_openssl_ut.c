// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#ifdef __cplusplus
#include <cstdlib>
#else
#include <stdlib.h>
#endif

static void* my_gballoc_malloc(size_t size)
{
    return malloc(size);
}

static void my_gballoc_free(void* s)
{
    free(s);
}

#ifdef __cplusplus
#include <cstddef>
#else
#include <stddef.h>
#endif

#include "testrunnerswitcher.h"
#include "umock_c.h"

#include "openssl/ssl.h"
#include "openssl/x509.h"
#include "openssl/err.h"
#include "openssl/opensslv.h"
#include "openssl/pem.h"
#include "openssl/bio.h"
#include "openssl/rsa.h"

#include "azure_c_shared_utility/x509_openssl.h"
#include "umocktypes_charptr.h"
#include "umock_c_negative_tests.h"

#define ENABLE_MOCKS
#include "azure_c_shared_utility/gballoc.h"

#include "azure_c_shared_utility/umock_c_prod.h"

/*from openssl/bio.h*/
MOCKABLE_FUNCTION(,int, BIO_free, BIO *,a);
MOCKABLE_FUNCTION(, BIO *, BIO_new, BIO_METHOD *, type);
MOCKABLE_FUNCTION(, BIO *, BIO_mem, BIO_METHOD *, type);
MOCKABLE_FUNCTION(, int, BIO_puts, BIO *, bp, const char *, buf);
MOCKABLE_FUNCTION(, BIO_METHOD *, BIO_s_mem);

/*the below function has different signatures on different versions of OPENSSL*/
/*
|openssl version (number) | openssl string                   | BIO_new_mem_buf prototype                         | Observations                    |
|-------------------------|----------------------------------|---------------------------------------------------|---------------------------------|
| 0x1000106fL             | OpenSSL 1.0.1f 6 Jan 2014        | BIO *BIO_new_mem_buf(void *buf, int len);         | Ubuntu 14.04                    |
| 0x1000204fL             | OpenSSL 1.0.2d 9 Jul 2015        | BIO *BIO_new_mem_buf(void *buf, int len);         | Ubuntu 15.10                    |
| 0x1000207fL             | OpenSSL 1.0.2g-fips  1 Mar 2016  | BIO *BIO_new_mem_buf(const void *buf, int len);   | Ubuntu 16.10                    |

*/

#if OPENSSL_VERSION_NUMBER >= 0x1000207fL
MOCKABLE_FUNCTION(,BIO *,BIO_new_mem_buf, const void *,buf, int, len);
#else
MOCKABLE_FUNCTION(, BIO *, BIO_new_mem_buf, void *, buf, int, len);
#endif

/*from openssl/rsa.h*/
MOCKABLE_FUNCTION(,void, RSA_free, RSA *,rsa);

/*from openssl/x509.h*/
MOCKABLE_FUNCTION(,void, X509_free, X509 *, a);

/*from  openssl/pem.h*/
MOCKABLE_FUNCTION(, X509 *,PEM_read_bio_X509, BIO *, bp, X509 **, x, pem_password_cb *, cb, void *, u);
MOCKABLE_FUNCTION(,RSA *,PEM_read_bio_RSAPrivateKey, BIO *,bp, RSA **,x, pem_password_cb *,cb, void *,u);

/*from openssl/ssl.h*/
MOCKABLE_FUNCTION(,int, SSL_CTX_use_RSAPrivateKey, SSL_CTX *,ctx, RSA *,rsa);
MOCKABLE_FUNCTION(,int, SSL_CTX_use_certificate, SSL_CTX *,ctx, X509*, x);
MOCKABLE_FUNCTION(, X509_STORE *, SSL_CTX_get_cert_store, const SSL_CTX *, ssl_ctx);

/*from openssl/err.h*/
MOCKABLE_FUNCTION(,unsigned long, ERR_get_error);
MOCKABLE_FUNCTION(, char *,ERR_error_string, unsigned long, e, char *,buf);
MOCKABLE_FUNCTION(, unsigned long, ERR_peek_error);

/*from openssl/x509_vfy.h*/
MOCKABLE_FUNCTION(,int, X509_STORE_add_cert, X509_STORE *, ctx, X509 *, x);

typedef void (*x509_FREE_FUNC)(void*);
MOCKABLE_FUNCTION(, void, sk_pop_free, _STACK*, st, x509_FREE_FUNC, free_func);
MOCKABLE_FUNCTION(, void, EVP_PKEY_free, EVP_PKEY*, pkey);
MOCKABLE_FUNCTION(, X509*, PEM_read_bio_X509_AUX, BIO*, bp, X509**, x, pem_password_cb*, cb, void*, u);
MOCKABLE_FUNCTION(, EVP_PKEY*, PEM_read_bio_PrivateKey, BIO*, bp, EVP_PKEY**, x, pem_password_cb*, cb, void*, u);
MOCKABLE_FUNCTION(, int, SSL_CTX_use_PrivateKey, SSL_CTX*, ctx, EVP_PKEY*, pkey);
MOCKABLE_FUNCTION(, long, SSL_CTX_ctrl, SSL_CTX*, ctx, int, cmd, long, larg, void*, parg);
MOCKABLE_FUNCTION(, unsigned long, ERR_peek_last_error);
MOCKABLE_FUNCTION(, void, ERR_clear_error);

#undef ENABLE_MOCKS

/*the below function has different signatures on different versions of OPENSSL*/
#if OPENSSL_VERSION_NUMBER >= 0x1000207fL
static BIO* my_BIO_new_mem_buf(const void * buf, int len)
#else
static BIO* my_BIO_new_mem_buf(void * buf, int len)
#endif
{
    (void)len, (void)buf;
    return (BIO*)my_gballoc_malloc(sizeof(BIO));
}

static int my_BIO_free(BIO * a)
{
    my_gballoc_free(a);
    return 0;
}

static BIO *my_BIO_new(BIO_METHOD *type)
{
    (void)type;
    return (BIO*)my_gballoc_malloc(sizeof(BIO));
}

static void my_RSA_free(RSA * rsa)
{
    my_gballoc_free(rsa);
}

static void my_X509_free(X509 * a)
{
    my_gballoc_free(a);
}

static X509* my_PEM_read_bio_X509_AUX(BIO* bp, X509** x, pem_password_cb* cb, void* u)
{
    (void)u, (void)cb, (void)x, (void)bp;
    return (X509*)my_gballoc_malloc(sizeof(X509));
}

static long my_SSL_CTX_ctrl(SSL_CTX* ctx, int cmd, long larg, void* parg)
{
    (void)ctx;
    (void)cmd;
    (void)larg;
    my_gballoc_free(parg);
    return 1;
}

static X509 * my_PEM_read_bio_X509(BIO * bp, X509 ** x, pem_password_cb * cb, void * u)
{
    (void)u, (void)cb, (void)x, (void)bp;
    return (X509*)my_gballoc_malloc(sizeof(X509));
}

static RSA * my_PEM_read_bio_RSAPrivateKey(BIO * bp, RSA ** x, pem_password_cb * cb, void * u)
{
    (void)u, (void)cb, (void)x, (void)bp;
    return (RSA*)my_gballoc_malloc(sizeof(RSA));
}

static TEST_MUTEX_HANDLE g_testByTest;
static TEST_MUTEX_HANDLE g_dllByDll;

static void on_umock_c_error(UMOCK_C_ERROR_CODE error_code)
{
    (void)error_code;
    ASSERT_FAIL("umock_c reported error");
}


#define TEST_SSL_CTX ((SSL_CTX*)(0x42))
#define TEST_CERTIFICATE_1 "one certificate"
#define TEST_X509_STORE (X509_STORE *)"le store"
#define TEST_BIO_METHOD (BIO_METHOD*)"le method"
#define TEST_BIO (BIO*)"le bio"

static const char* TEST_ECC_CERTIFICATE = "-----BEGIN CERTIFICATE-----";
static const char* TEST_ECC_ALIAS_KEY = "-----BEGIN EC PRIVATE KEY-----";
static EVP_PKEY* TEST_PKEY = (EVP_PKEY*)0x12;
static BIO* TEST_BIO_CERT = (BIO*)0x11;
static X509* TEST_X509 = (X509*)0x13;
static SSL_CTX TEST_SSL_CTX_STRUCTURE;

BEGIN_TEST_SUITE(x509_openssl_unittests)

    TEST_SUITE_INITIALIZE(a)
    {
        TEST_INITIALIZE_MEMORY_DEBUG(g_dllByDll);
        g_testByTest = TEST_MUTEX_CREATE();
        ASSERT_IS_NOT_NULL(g_testByTest);

        (void)umock_c_init(on_umock_c_error);

        (void)umocktypes_charptr_register_types();

        REGISTER_GLOBAL_MOCK_HOOK(gballoc_malloc, my_gballoc_malloc);
        REGISTER_GLOBAL_MOCK_FAIL_RETURN(gballoc_malloc, NULL);
        REGISTER_GLOBAL_MOCK_HOOK(gballoc_free, my_gballoc_free);

        REGISTER_GLOBAL_MOCK_HOOK(BIO_new_mem_buf, my_BIO_new_mem_buf);
        REGISTER_GLOBAL_MOCK_FAIL_RETURN(BIO_new_mem_buf, NULL);

        REGISTER_GLOBAL_MOCK_HOOK(PEM_read_bio_X509, my_PEM_read_bio_X509);
        REGISTER_GLOBAL_MOCK_FAIL_RETURN(PEM_read_bio_X509, NULL);

        REGISTER_GLOBAL_MOCK_HOOK(PEM_read_bio_RSAPrivateKey, my_PEM_read_bio_RSAPrivateKey);
        REGISTER_GLOBAL_MOCK_FAIL_RETURN(PEM_read_bio_X509, NULL);
        
        REGISTER_GLOBAL_MOCK_RETURNS(SSL_CTX_use_certificate, 1, 0);
        REGISTER_GLOBAL_MOCK_RETURNS(BIO_s_mem, TEST_BIO_METHOD, NULL);

        REGISTER_GLOBAL_MOCK_HOOK(BIO_new, my_BIO_new);
        REGISTER_GLOBAL_MOCK_FAIL_RETURN(BIO_new, NULL);
        REGISTER_GLOBAL_MOCK_RETURNS(BIO_puts, strlen(TEST_CERTIFICATE_1), strlen(TEST_CERTIFICATE_1)-1);
        
        REGISTER_GLOBAL_MOCK_RETURNS(SSL_CTX_get_cert_store, TEST_X509_STORE, NULL);
        REGISTER_GLOBAL_MOCK_RETURNS(X509_STORE_add_cert, __LINE__, 0);
        
        REGISTER_GLOBAL_MOCK_RETURNS(SSL_CTX_use_RSAPrivateKey, 1, 0);

        REGISTER_GLOBAL_MOCK_HOOK(BIO_free, my_BIO_free);
        REGISTER_GLOBAL_MOCK_HOOK(RSA_free, my_RSA_free);
        REGISTER_GLOBAL_MOCK_HOOK(X509_free, my_X509_free);

        REGISTER_GLOBAL_MOCK_RETURNS(PEM_read_bio_PrivateKey, TEST_PKEY, NULL);
        REGISTER_GLOBAL_MOCK_RETURNS(BIO_new_mem_buf, TEST_BIO_CERT, NULL);
        REGISTER_GLOBAL_MOCK_HOOK(PEM_read_bio_X509_AUX, my_PEM_read_bio_X509_AUX);
        REGISTER_GLOBAL_MOCK_RETURNS(SSL_CTX_use_PrivateKey, 1, 0);
        REGISTER_GLOBAL_MOCK_HOOK(SSL_CTX_ctrl, my_SSL_CTX_ctrl);
    }

    TEST_SUITE_CLEANUP(TestClassCleanup)
    {
        umock_c_deinit();

        TEST_MUTEX_DESTROY(g_testByTest);
        TEST_DEINITIALIZE_MEMORY_DEBUG(g_dllByDll);
    }

    TEST_FUNCTION_INITIALIZE(initialize)
    {
        umock_c_reset_all_calls();

        memset(&TEST_SSL_CTX_STRUCTURE, 0, sizeof(SSL_CTX) );
    }

    TEST_FUNCTION_CLEANUP(cleans)
    {

    }

    static void setup_load_alias_key_cert_mocks()
    {
        STRICT_EXPECTED_CALL(BIO_new_mem_buf((void*)TEST_ECC_ALIAS_KEY, -1));
        STRICT_EXPECTED_CALL(PEM_read_bio_PrivateKey(IGNORED_PTR_ARG, NULL, NULL, NULL));
        STRICT_EXPECTED_CALL(SSL_CTX_use_PrivateKey(&TEST_SSL_CTX_STRUCTURE, TEST_PKEY));
        STRICT_EXPECTED_CALL(EVP_PKEY_free(TEST_PKEY) );
        STRICT_EXPECTED_CALL(BIO_free(IGNORED_PTR_ARG) );
    }

    static void setup_load_certificate_chain_mocks()
    {
        STRICT_EXPECTED_CALL(BIO_new_mem_buf((void*)TEST_ECC_CERTIFICATE, -1));
        STRICT_EXPECTED_CALL(PEM_read_bio_X509_AUX(IGNORED_PTR_ARG, NULL, NULL, NULL));
        STRICT_EXPECTED_CALL(SSL_CTX_use_certificate(&TEST_SSL_CTX_STRUCTURE, IGNORED_PTR_ARG));
        STRICT_EXPECTED_CALL(PEM_read_bio_X509(IGNORED_PTR_ARG, NULL, NULL, NULL));
        STRICT_EXPECTED_CALL(SSL_CTX_ctrl(&TEST_SSL_CTX_STRUCTURE, SSL_CTRL_EXTRA_CHAIN_CERT, IGNORED_NUM_ARG, IGNORED_PTR_ARG));
        STRICT_EXPECTED_CALL(PEM_read_bio_X509(IGNORED_PTR_ARG, NULL, NULL, NULL))
            .SetReturn(NULL); // Needed because the x509 needs not to be free
        STRICT_EXPECTED_CALL(X509_free(IGNORED_PTR_ARG));
        STRICT_EXPECTED_CALL(BIO_free(IGNORED_PTR_ARG));
    }

    /*Tests_SRS_X509_OPENSSL_02_001: [ If any argument is NULL then x509_openssl_add_credentials shall fail and return a non-zero value. ]*/
    TEST_FUNCTION(x509_openssl_add_credentials_with_NULL_SSL_CTX_fails)
    {
        ///arrange

        ///act
        int result = x509_openssl_add_credentials(NULL, "certificate", "privatekey");

        ///assert
        ASSERT_ARE_NOT_EQUAL(int, 0, result);

        ///cleanup
    }

    /*Tests_SRS_X509_OPENSSL_02_001: [ If any argument is NULL then x509_openssl_add_credentials shall fail and return a non-zero value. ]*/
    TEST_FUNCTION(x509_openssl_add_credentials_with_NULL_certificate_fails)
    {
        ///arrange

        ///act
        int result = x509_openssl_add_credentials(TEST_SSL_CTX, NULL, "privatekey");

        ///assert
        ASSERT_ARE_NOT_EQUAL(int, 0, result);

        ///cleanup
    }

    /*Tests_SRS_X509_OPENSSL_02_001: [ If any argument is NULL then x509_openssl_add_credentials shall fail and return a non-zero value. ]*/
    TEST_FUNCTION(x509_openssl_add_credentials_with_NULL_privatekey_fails)
    {
        ///arrange

        ///act
        int result = x509_openssl_add_credentials(TEST_SSL_CTX, "certificate", NULL);

        ///assert
        ASSERT_ARE_NOT_EQUAL(int, 0, result);

        ///cleanup
    }

    #ifndef VALIDATED_PTR_ARG
    #define VALIDATED_PTR_ARG NULL
    #endif

    #ifndef VALIDATED_NUM_ARG
    #define VALIDATED_NUM_ARG 0
    #endif
    /*Tests_SRS_X509_OPENSSL_02_002: [ x509_openssl_add_credentials shall use BIO_new_mem_buf to create a memory BIO from the x509 certificate. ] */
    /*Tests_SRS_X509_OPENSSL_02_003: [ x509_openssl_add_credentials shall use PEM_read_bio_X509 to read the x509 certificate. ] */
    /*Tests_SRS_X509_OPENSSL_02_004: [ x509_openssl_add_credentials shall use BIO_new_mem_buf to create a memory BIO from the x509 privatekey. ] */
    /*Tests_SRS_X509_OPENSSL_02_005: [ x509_openssl_add_credentials shall use PEM_read_bio_RSAPrivateKey to read the x509 private key. ] */
    /*Tests_SRS_X509_OPENSSL_02_006: [ x509_openssl_add_credentials shall use SSL_CTX_use_certificate to load the certicate into the SSL context. ] */
    /*Tests_SRS_X509_OPENSSL_02_007: [ x509_openssl_add_credentials shall use SSL_CTX_use_RSAPrivateKey to load the private key into the SSL context. ]*/
    /*Tests_SRS_X509_OPENSSL_02_008: [ If no error occurs, then x509_openssl_add_credentials shall succeed and return 0. ] */
    TEST_FUNCTION(x509_openssl_add_credentials_happy_path)
    {
        
        ///arrange
        char* certificateText = (char*)"certificate";
        char* privatekeyText = (char*)"privatekeyText";
        BIO* certificate;
        BIO* privatekey;
        X509* certificateAsX509;
        RSA* privatekeyasRSA;
		int result;
        STRICT_EXPECTED_CALL(BIO_new_mem_buf(certificateText, -1))
            .CaptureReturn(&certificate);
        STRICT_EXPECTED_CALL(PEM_read_bio_X509(VALIDATED_PTR_ARG, NULL, 0, NULL))
            .ValidateArgumentValue_bp(&certificate)
            .CaptureReturn(&certificateAsX509)
            ;
        STRICT_EXPECTED_CALL(BIO_new_mem_buf(privatekeyText, -1))
            .CaptureReturn(&privatekey);
        STRICT_EXPECTED_CALL(PEM_read_bio_RSAPrivateKey(VALIDATED_PTR_ARG, NULL, 0, NULL))
            .ValidateArgumentValue_bp(&privatekey)
            .CaptureReturn(&privatekeyasRSA);

        STRICT_EXPECTED_CALL(SSL_CTX_use_certificate(TEST_SSL_CTX, VALIDATED_PTR_ARG))
            .ValidateArgumentValue_x(&certificateAsX509);
        STRICT_EXPECTED_CALL(SSL_CTX_use_RSAPrivateKey(TEST_SSL_CTX, VALIDATED_PTR_ARG))
            .ValidateArgumentValue_rsa(&privatekeyasRSA);

        STRICT_EXPECTED_CALL(RSA_free(VALIDATED_PTR_ARG))
            .ValidateArgumentValue_rsa(&privatekeyasRSA);

        STRICT_EXPECTED_CALL(BIO_free(VALIDATED_PTR_ARG))
            .ValidateArgumentValue_a(&privatekey);

        STRICT_EXPECTED_CALL(X509_free(VALIDATED_PTR_ARG))
            .ValidateArgumentValue_a(&certificateAsX509);

        STRICT_EXPECTED_CALL(BIO_free(VALIDATED_PTR_ARG))
            .ValidateArgumentValue_a(&certificate);

        ///act
        result = x509_openssl_add_credentials(TEST_SSL_CTX, certificateText, privatekeyText);

        ///assert
        ASSERT_ARE_EQUAL(int, 0, result);
        ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

        ///cleanup
    }

    /*Tests_SRS_X509_OPENSSL_02_009: [ Otherwise x509_openssl_add_credentials shall fail and return a non-zero number. ]*/
    TEST_FUNCTION(x509_openssl_add_credentials_unhappy_paths)
    {
        ///arrange
        size_t calls_that_cannot_fail[] =
        {
            6, /*RSA_free*/
            7, /*BIO_free*/
            8, /*X509_free*/
            9 /*BIO_free*/
        };
        char* certificateText = (char*)"certificate";
        char* privatekeyText = (char*)"privatekeyText";

        BIO* certificate;
        BIO* privatekey;
        X509* certificateAsX509;
        RSA* privatekeyasRSA;
		size_t i;

        (void)umock_c_negative_tests_init();

		STRICT_EXPECTED_CALL(BIO_new_mem_buf(certificateText, -1))
            .CaptureReturn(&certificate);
        STRICT_EXPECTED_CALL(PEM_read_bio_X509(VALIDATED_PTR_ARG, NULL, 0, NULL))
            .ValidateArgumentValue_bp(&certificate)
            .CaptureReturn(&certificateAsX509)
            ;
        STRICT_EXPECTED_CALL(BIO_new_mem_buf(privatekeyText, -1))
            .CaptureReturn(&privatekey);
        STRICT_EXPECTED_CALL(PEM_read_bio_RSAPrivateKey(VALIDATED_PTR_ARG, NULL, 0, NULL))
            .ValidateArgumentValue_bp(&privatekey)
            .CaptureReturn(&privatekeyasRSA);

        STRICT_EXPECTED_CALL(SSL_CTX_use_certificate(TEST_SSL_CTX, VALIDATED_PTR_ARG))
            .ValidateArgumentValue_x(&certificateAsX509);
        STRICT_EXPECTED_CALL(SSL_CTX_use_RSAPrivateKey(TEST_SSL_CTX, VALIDATED_PTR_ARG))
            .ValidateArgumentValue_rsa(&privatekeyasRSA);

        STRICT_EXPECTED_CALL(RSA_free(VALIDATED_PTR_ARG))
            .ValidateArgumentValue_rsa(&privatekeyasRSA);

        STRICT_EXPECTED_CALL(BIO_free(VALIDATED_PTR_ARG))
            .ValidateArgumentValue_a(&privatekey);

        STRICT_EXPECTED_CALL(X509_free(VALIDATED_PTR_ARG))
            .ValidateArgumentValue_a(&certificateAsX509);

        STRICT_EXPECTED_CALL(BIO_free(VALIDATED_PTR_ARG))
            .ValidateArgumentValue_a(&certificate);

        umock_c_negative_tests_snapshot();

        ///act

        for (i = 0; i < umock_c_negative_tests_call_count(); i++)
        {
            size_t j;
            umock_c_negative_tests_reset();

            for (j = 0;j<sizeof(calls_that_cannot_fail) / sizeof(calls_that_cannot_fail[0]);j++) /*not running the tests that have failed that cannot fail*/
            {
                if (calls_that_cannot_fail[j] == i)
                    break;
            }

            if (j == sizeof(calls_that_cannot_fail) / sizeof(calls_that_cannot_fail[0]))
            {

                char temp_str[128];
				int result;
                umock_c_negative_tests_fail_call(i);
                sprintf(temp_str, "On failed call %zu", i);

                ///act
                result = x509_openssl_add_credentials(TEST_SSL_CTX, certificateText, privatekeyText);

                ///assert
                ASSERT_ARE_NOT_EQUAL_WITH_MSG(int, 0, result, temp_str);
            }
        }

        ///cleanup
        umock_c_negative_tests_deinit();
    }

    /*Tests_SRS_X509_OPENSSL_02_010: [ If ssl_ctx is NULL then x509_openssl_add_certificates shall fail and return a non-zero value. ]*/
    TEST_FUNCTION(x509_openssl_add_certificates_with_NULL_ssl_ctx_fails)
    {
        ///arrange

        ///act
        int result = x509_openssl_add_certificates(NULL, "a");

        ///assert
        ASSERT_ARE_NOT_EQUAL(int, 0, result);

        ///clean
    }

    /*Tests_SRS_X509_OPENSSL_02_011: [ If certificates is NULL then x509_openssl_add_certificates shall fail and return a non-zero value. ]*/
    TEST_FUNCTION(x509_openssl_add_certificates_with_NULL_certificates_fails)
    {
        ///arrange

        ///act
        int result = x509_openssl_add_certificates(TEST_SSL_CTX, NULL);

        ///assert
        ASSERT_ARE_NOT_EQUAL(int, 0, result);

        ///clean
    }

    /*Tests_SRS_X509_OPENSSL_02_012: [ x509_openssl_add_certificates shall get the memory BIO method function by calling BIO_s_mem. ]*/
    /*Tests_SRS_X509_OPENSSL_02_013: [ x509_openssl_add_certificates shall create a new memory BIO by calling BIO_new. ]*/
    /*Tests_SRS_X509_OPENSSL_02_014: [ x509_openssl_add_certificates shall load certificates into the memory BIO by a call to BIO_puts. ]*/
    /*Tests_SRS_X509_OPENSSL_02_015: [ x509_openssl_add_certificates shall retrieve each certificate by a call to PEM_read_bio_X509. ]*/
    /*Tests_SRS_X509_OPENSSL_02_016: [ x509_openssl_add_certificates shall add the certificate to the store by a call to X509_STORE_add_cert. ]*/
    /*Tests_SRS_X509_OPENSSL_02_019: [ Otherwise, x509_openssl_add_certificates shall succeed and return 0. ]*/
    TEST_FUNCTION(x509_openssl_add_certificates_1_certificate_happy_path)
    {
        ///arrange
		int result;

        STRICT_EXPECTED_CALL(SSL_CTX_get_cert_store(TEST_SSL_CTX));
        STRICT_EXPECTED_CALL(BIO_s_mem());
        STRICT_EXPECTED_CALL(BIO_new(IGNORED_PTR_ARG));
        STRICT_EXPECTED_CALL(BIO_puts(IGNORED_PTR_ARG, TEST_CERTIFICATE_1));
        STRICT_EXPECTED_CALL(PEM_read_bio_X509(IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG));
        STRICT_EXPECTED_CALL(X509_STORE_add_cert(IGNORED_PTR_ARG, IGNORED_PTR_ARG));
        STRICT_EXPECTED_CALL(X509_free(IGNORED_PTR_ARG));

        STRICT_EXPECTED_CALL(PEM_read_bio_X509(IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG))
            .SetReturn(NULL);

        ///act
        result = x509_openssl_add_certificates(TEST_SSL_CTX, TEST_CERTIFICATE_1);

        ///assert
        ASSERT_ARE_EQUAL(int, 0, result);

        ///clean
    }

    void x509_openssl_add_certificates_1_certificate_which_exists_inert_path(void)
    {
        STRICT_EXPECTED_CALL(SSL_CTX_get_cert_store(TEST_SSL_CTX));
        STRICT_EXPECTED_CALL(BIO_s_mem());
        STRICT_EXPECTED_CALL(BIO_new(IGNORED_PTR_ARG));
        STRICT_EXPECTED_CALL(BIO_puts(IGNORED_PTR_ARG, TEST_CERTIFICATE_1));
        STRICT_EXPECTED_CALL(PEM_read_bio_X509(IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG));
        STRICT_EXPECTED_CALL(X509_STORE_add_cert(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
            .SetReturn(0);
        STRICT_EXPECTED_CALL(ERR_peek_error())
            .SetReturn(X509_R_CERT_ALREADY_IN_HASH_TABLE);
        STRICT_EXPECTED_CALL(X509_free(IGNORED_PTR_ARG));

        STRICT_EXPECTED_CALL(PEM_read_bio_X509(IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG))
            .SetReturn(NULL);
    }

    /*Tests_SRS_X509_OPENSSL_02_017: [ If X509_STORE_add_cert returns with error and that error is X509_R_CERT_ALREADY_IN_HASH_TABLE then x509_openssl_add_certificates shall ignore it as the certificate is already in the store. ]*/
    TEST_FUNCTION(x509_openssl_add_certificates_1_certificate_which_exists_happy_path)
    {
        ///arrange
		int result;

        x509_openssl_add_certificates_1_certificate_which_exists_inert_path();

        ///act
        result = x509_openssl_add_certificates(TEST_SSL_CTX, TEST_CERTIFICATE_1);

        ///assert
        ASSERT_ARE_EQUAL(int, 0, result);

        ///clean
    }

    /*Tests_SRS_X509_OPENSSL_02_018: [ In case of any failure x509_openssl_add_certificates shall fail and return a non-zero value. ]*/
    TEST_FUNCTION(x509_openssl_add_certificates_1_certificate_which_exists_unhappy_paths)
    {
        ///arrange
		size_t i;

        umock_c_negative_tests_init();
        x509_openssl_add_certificates_1_certificate_which_exists_inert_path();
        umock_c_negative_tests_snapshot();

        for (i = 0; i < umock_c_negative_tests_call_count(); i++)
        {
			int result;

            if (
                (i == 4) || /*PEM_read_bio_X509*/
                (i == 5)|| /*X509_STORE_add_cert*/
                (i == 7) || /*X509_free*/
                (i == 8) /*PEM_read_bio_X509*/
                )
            {
                continue; // these lines have functions that do not return anything (void).
            }

            umock_c_negative_tests_reset();
            umock_c_negative_tests_fail_call(i);

            ///act
            result = x509_openssl_add_certificates(TEST_SSL_CTX, TEST_CERTIFICATE_1);

            ///assert
            ASSERT_ARE_NOT_EQUAL(int, 0, result);
        }

        ///clean
        umock_c_negative_tests_deinit();
    }

    /* Tests_SRS_X509_OPENSSL_07_001: [ If ssl_ctx, ecc_alias_cert, or ecc_alias_key are NULL, x509_openssl_add_ecc_credentials shall return a non-zero value. ] */
    TEST_FUNCTION(x509_openssl_add_ecc_credentials_ssl_ctx_NULL_fail)
    {
        //arrange

        //act
        int result = x509_openssl_add_ecc_credentials(NULL, TEST_ECC_CERTIFICATE, TEST_ECC_ALIAS_KEY);

        //assert
        ASSERT_ARE_NOT_EQUAL(int, 0, result);

        //clean
    }

    /* Tests_SRS_X509_OPENSSL_07_001: [ If ssl_ctx, ecc_alias_cert, or ecc_alias_key are NULL, x509_openssl_add_ecc_credentials shall return a non-zero value. ] */
    TEST_FUNCTION(x509_openssl_add_ecc_credentials_cert_NULL_fail)
    {
        //arrange

        //act
        int result = x509_openssl_add_ecc_credentials(TEST_SSL_CTX, NULL, TEST_ECC_ALIAS_KEY);

        //assert
        ASSERT_ARE_NOT_EQUAL(int, 0, result);

        //clean
    }

    /* Tests_SRS_X509_OPENSSL_07_001: [ If ssl_ctx, ecc_alias_cert, or ecc_alias_key are NULL, x509_openssl_add_ecc_credentials shall return a non-zero value. ] */
    TEST_FUNCTION(x509_openssl_add_ecc_credentials_key_NULL_fail)
    {
        //arrange

        //act
        int result = x509_openssl_add_ecc_credentials(TEST_SSL_CTX, TEST_ECC_CERTIFICATE, NULL);

        //assert
        ASSERT_ARE_NOT_EQUAL(int, 0, result);

        //clean
    }

    /*Tests_SRS_X509_OPENSSL_07_002: [ x509_openssl_add_ecc_credentials shall get the memory BIO method function. ] */
    /*Tests_SRS_X509_OPENSSL_07_003: [ x509_openssl_add_ecc_credentials shall generate a EVP_PKEY by calling PEM_read_bio_PrivateKey. ] */
    /*Tests_SRS_X509_OPENSSL_07_004: [ x509_openssl_add_ecc_credentials shall import the certification using by the EVP_PKEY. ] */
    /*Tests_SRS_X509_OPENSSL_07_005: [ x509_openssl_add_ecc_credentials shall load the cert chain by calling PEM_read_bio_X509_AUX and SSL_CTX_use_certification. ] */
    /*Tests_SRS_X509_OPENSSL_07_006: [ If successful x509_openssl_add_ecc_credentials shall to import each certificate in the cert chain. ] */
    TEST_FUNCTION(x509_openssl_add_ecc_credentials_success)
    {
        //arrange
		int result;

        setup_load_alias_key_cert_mocks();
        setup_load_certificate_chain_mocks();

        //act
        result = x509_openssl_add_ecc_credentials(&TEST_SSL_CTX_STRUCTURE, TEST_ECC_CERTIFICATE, TEST_ECC_ALIAS_KEY);

        //assert
        ASSERT_ARE_EQUAL(int, 0, result);

        //clean
    }

END_TEST_SUITE(x509_openssl_unittests)


