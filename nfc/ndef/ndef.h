/*
 * ZentriOS SDK LICENSE AGREEMENT | Zentri.com, 2015.
 *
 * Use of source code and/or libraries contained in the ZentriOS SDK is
 * subject to the Zentri Operating System SDK license agreement and
 * applicable open source license agreements.
 *
 */

#pragma once



#define NDEF_TLV_TYPE           0x03
#define NDEF_TLV_MAX_PAYLOAD    254
#define NDEF_MAX_PAYLOAD        (NDEF_TLV_MAX_PAYLOAD-4)  // this library currently only support 'short' records
#define NDEF_BLOCK_SIZE         16

#define TLV_HEADER_LEN 2
#define TLV_TERMINATOR_LEN 1
#define NDEF_HEADER_LEN 4

typedef enum
{
    TLV_TYPE_NULL               = 0x00,
    TLV_TYPE_NDEF               = 0x03,
    TLV_TYPE_PROPRIETARY        = 0xFD,
    TLV_TYPE_TERMINATOR         = 0xFE
} tlv_block_type_t;



typedef enum
{
    NDEF_TNF_EMPTY      = 0x00,         //!<  Empty Record
                                        //!< Indicates no type, id, or payload is associated with this NDEF Record.
                                        //!< This record type is useful on newly formatted cards since every NDEF tag
                                        //!< must have at least one NDEF Record.

    NDEF_TNF_WELL_KNOWN = 0x01,         //!< Well-Known Record
                                        //!< Indicates the type field uses the RTD type name format.  This type name is used
                                        //!< to stored any record defined by a Record Type Definition (RTD), such as storing
                                        //!< RTD Text, RTD URIs, etc., and is one of the mostly frequently used and useful
                                        //!< record types.

    NDEF_TNF_MIME       = 0x02,         //!< MIME Media Record
                                        //!< Indicates the payload is an intermediate or final chunk of a chunked NDEF Record

    NDEF_TNF_URI        = 0x03,         //!< Absolute URI Record
                                        //!< Indicates the type field contains a value that follows the absolute-URI BNF
                                        //!< construct defined by RFC 3986

    NDEF_TNF_EXTERNAL   = 0x04,         //!< External Record
                                        //!< Indicates the type field contains a value that follows the RTD external
                                        //!< name specification

    NDEF_TNF_UNKNOWN    = 0x05,         //!< Unknown Record
                                        //!< Indicates the payload type is unknown

    NDEF_TNF_UNCHANGED  = 0x06         //!< Unchanged Record
                                       //!< Indicates the payload is an intermediate or final chunk of a chunked NDEF Record
} ndef_tnf_type_t;


/**
 *
 */
typedef enum
{
    NDEF_RECORD_TYPE_TEXT       = 0x54,//!< NDEF_RECORD_TYPE_TEXT
    NDEF_RECORD_TYPE_URI        = 0x55 //!< NDEF_RECORD_TYPE_URI
} ndef_record_type_t;


typedef enum
{
    NDEF_URI_CODE_NONE          = 0x00,     // No prepending is done ... the entire URI is contained in the URI Field
    NDEF_URI_CODE_HTTP          = 0x01,     // http://www.
    NDEF_URI_CODE_HTTPS         = 0x02,     // https://www.
    NDEF_URI_CODE_HTTP2         = 0x03,     // http://
    NDEF_URI_CODE_HTTPS2        = 0x04,     // https://
    NDEF_URI_CODE_TEL           = 0x05,     // tel:
    NDEF_URI_CODE_MAILTO        = 0x06,     // mailto:
    NDEF_URI_CODE_FTP_ANON      = 0x07,     // ftp://anonymous:anonymous@
    NDEF_URI_CODE_FTP           = 0x08,     // ftp://ftp.
    NDEF_URI_CODE_FTPS          = 0x09,     // ftps://
    NDEF_URI_CODE_SFTP          = 0x0A,     // sftp://
    NDEF_URI_CODE_SMB           = 0x0B,     // smb://
    NDEF_URI_CODE_NFS           = 0x0C,     // nfs://
    NDEF_URI_CODE_FTP2          = 0x0D,     // ftp://
    NDEF_URI_CODE_DAV           = 0x0E,     // dav://
    NDEF_URI_CODE_NEWS          = 0x0F,     // news:
    NDEF_URI_CODE_TELNET        = 0x10,     // telnet://
    NDEF_URI_CODE_IMAP          = 0x11,     // imap:
    NDEF_URI_CODE_RTSP          = 0x12,     // rtsp://
    NDEF_URI_CODE_URN           = 0x13,     // urn:
    NDEF_URI_CODE_POP           = 0x14,     // pop:
    NDEF_URI_CODE_SIP           = 0x15,     // sip:
    NDEF_URI_CODE_SIPS          = 0x16,     // sips:
    NDEF_URI_CODE_TFTP          = 0x17,     // tftp:
    NDEF_URI_CODE_BTSPP         = 0x18,     // btspp://
    NDEF_URI_CODE_BTL2CAP       = 0x19,     // btl2cap://
    NDEF_URI_CODE_BTGOEP        = 0x1A,     // btgoep://
    NDEF_URI_CODE_TCPOBEX       = 0x1B,     // tcpobex://
    NDEF_URI_CODE_IRDAOBEX      = 0x1C,     // irdaobex://
    NDEF_URI_CODE_FILE          = 0x1D      // file://
} ndef_uri_code_t;

typedef enum
{
    NDEF_FLAG_TYPE_MASK         = (7 << 0),      // Type-name field mask
                                                    // (currently only 0x01 = 'Well Known Record' is supported)
    NDEF_FLAG_HAS_ID_LENGTH     = (1 << 3),
    NDEF_FLAG_SHORT_RECORD      = (1 << 4),
    NDEF_FLAG_CHUNKED_RECORD    = (1 << 5),
    NDEF_FLAG_END_MSG           = (1 << 6),
    NDEF_FLAG_BEGIN_MSG         = (1 << 7)
} ndef_header_flag_t;


typedef struct
{
    uint8_t flags;
    uint8_t type_length;
    uint8_t length;
    uint8_t type;
} ndef_record_header_t;


typedef struct ndef_record
{
    struct ndef_record *next;
    ndef_record_header_t header;
    uint8_t payload[1];
} ndef_record_t;



typedef struct ndef_uri_record
{
    struct ndef_record *next;
    ndef_record_header_t header;
    uint8_t code;
    uint8_t uri[1];
} ndef_uri_record_t;

typedef struct ndef_text_record
{
    struct ndef_record *next;
    ndef_record_header_t header;
    uint8_t status;
#define NDEF_TEXT_FLAG_UTF8     (0 << 7)
#define NDEF_TEXT_FLAG_UTF16    (1 << 7)
#define NDEF_TEXT_LANG_LEN_MASK (0x1F)
    uint8_t *lang;
    char *text;
    uint8_t payload[1];
} ndef_text_record_t;



zos_result_t nfc_create_record(ndef_record_t **record, ndef_header_flag_t flags, ndef_record_type_t type, const uint8_t *buffer, uint8_t length);

zos_result_t nfc_create_text_record(ndef_text_record_t **record, const char *text, const char *language);

zos_result_t nfc_create_uri_record(ndef_uri_record_t **record, const char *uri, ndef_uri_code_t code);


