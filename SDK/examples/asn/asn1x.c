/** @file
@copyright
(C) Commsignia Ltd. - All Rights Reserved.
Unauthorised copying of this file, via any medium is strictly prohibited.
Proprietary and confidential.
@date 2020
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <unistd.h>
#include <ctype.h>
#include <getopt.h>
#include <assert.h>

#include <v2x/asn.1/v2x_eu_asn.h>
#include <v2x/asn.1/v2x_us_asn.h>
#include <v2x/asn.1/v2x_cn_asn.h>

#include <v2x/asn.1/asn1defs.h>

/** @file
@brief ASN.1 UPER encode and decode tool.
This example is a command line utility to encode/decode ASN.1 UPER messages to/from XML.
@ingroup ex
*/

/* App options */
#define OPT_HELP          'H'
#define OPT_UPER          'U'
#define OPT_XML           'X'
#define OPT_TYPE          'T'

enum { INPUT_BUFFER_LENGTH = (1024U * 1024U) };
enum { ASN_NAME_LENGTH = 64U };

typedef struct {
    char name[ASN_NAME_LENGTH];
    const ASN1CType* asn_type;
} l_type_t;

/** Supported types to translate */
static const l_type_t l_asn_types[] = {
    {
        .name = "CAM",
        .asn_type = asn1_type_EU_CAM
    },
    {
        .name = "CPM",
        .asn_type = asn1_type_EU_CPM
    },
    {
        .name = "DENM",
        .asn_type = asn1_type_EU_DENM
    },
    {
        .name = "MAP",
        .asn_type = asn1_type_EU_MAP
    },
    {
        .name = "SPAT",
        .asn_type = asn1_type_EU_SPAT
    },
    {
        .name = "BSM",
        .asn_type = asn1_type_US_BasicSafetyMessage
    },
    {
        .name = "IVI",
        .asn_type = asn1_type_EU_IVI
    },
    {
        .name = "CN_MSG_FRAME",
        .asn_type = asn1_type_CN_MessageFrame
    },
    {
        .name = "MSG_FRAME",
        .asn_type = asn1_type_US_MessageFrame
    },
    {
        .name = "WSA",
        .asn_type = asn1_type_US_SrvAdvMsg
    },
    {
        .name = "CoopAwareness",
        .asn_type = asn1_type_EU_CoopAwareness
    },
    {
        .name = "DecentralizedEnvironmentalNotificationMessage",
        .asn_type = asn1_type_EU_DecentralizedEnvironmentalNotificationMessage
    },
    {
        .name = "IviStructure",
        .asn_type = asn1_type_EU_IviStructure
    },
    {
        .name = "CollectivePerceptionMessage",
        .asn_type = asn1_type_EU_CollectivePerceptionMessage
    },
    {
        .name = "TravelerDataFrame",
        .asn_type = asn1_type_US_TravelerDataFrame
    },
    {
        .name = "RTCMcorrections",
        .asn_type = asn1_type_US_RTCMcorrections
    },
    /* Terminating type */
    {
        .name = "",
        .asn_type = NULL
    }
};


/** App parameters */
typedef struct l_params_t {
    bool help;
    bool xml;
    bool uper;
    const ASN1CType* asn_type;
} l_params_t;


static const ASN1CType* get_type(const char* in_type);
static uint32_t read_hex_from_stdin(uint8_t* buff, uint32_t size);
static uint32_t read_text_from_stdin(char* buff, uint32_t size);
static bool xml_to_uper(const ASN1CType* asn_type);
static bool uper_to_xml(const ASN1CType* asn_type);
static void print_usage(const char* bin);
static bool fetch_params(l_params_t* params, int argc, char* argv[]);


static uint32_t read_hex_from_stdin(uint8_t* buff, uint32_t size)
{
    uint32_t read_bytes = 0;

    bool finished = false;
    while(!finished) {

        char byte[3] = {0};
        int res = fread(byte, 2, 1, stdin);
        if(res == 1) {

            byte[2] = '\0';
            char* err = NULL;
            buff[read_bytes] = (uint8_t)strtoul(byte, &err, 16);

            if(err && (*err)) {
                fprintf(stderr, "%s is not a hex number [%lu]",
                        byte,
                        (unsigned long)read_bytes);
                read_bytes = 0;
                finished = true;
            } else {
                ++read_bytes;
            }

        } else if(res == 0) {
            finished = true;
        } else {

            if(ferror(stdin)) {
                fprintf(stderr, "Cannot read from stdin");
            }
            read_bytes = 0;
            finished = true;
        }

        if(read_bytes >= size) {
            finished = true;
        }
    }

    return read_bytes;
}


static uint32_t read_text_from_stdin(char* buff, uint32_t size)
{
    char* char_ptr = buff;
    uint32_t read_bytes = 0;

    bool finished = false;
    while(!finished) {
        int len = read(STDIN_FILENO, char_ptr, (size - read_bytes));
        if(len > 0) {

            char_ptr += len;
            read_bytes += len;
            if(read_bytes >= size) {
                read_bytes = 0;
                finished = true;
            }

        } else {
            finished = true;
        }
    }

    return read_bytes;
}


static bool xml_to_uper(const ASN1CType* asn_type)
{
    bool error = false;
    void* c_struct = NULL;
    char xml_buf[INPUT_BUFFER_LENGTH] = {0};

    uint32_t xml_len = read_text_from_stdin(xml_buf, sizeof(xml_buf));
    if(xml_len == 0) {
        fprintf(stderr, "Unable to read xml from stdin\n");
        error = true;
    } else {
        fprintf(stderr,
                "XML input:\n"
                "%s\n"
                "--- End of XML input\n\n",
                xml_buf);
    }

    if(!error) {
        ASN1Error err;
        int ret = asn1_xer_decode((void**)&c_struct,
                                  asn_type,
                                  (uint8_t*)xml_buf,
                                  xml_len,
                                  &err);
        if((ret < 0) || (c_struct == NULL)) {
            fprintf(stderr, "Decoding error: %s\n", err.msg);
            error = true;
        }
    }

    if(!error) {
        uint8_t* uper_ptr = NULL;
        ASN1Error err;
        int ret = asn1_uper_encode2(&uper_ptr, asn_type, c_struct, &err);
        if((ret < 0) || (uper_ptr == NULL)) {
            fprintf(stderr, "Encoding error: %s\n", err.msg);
            error = true;
        } else {
            fprintf(stderr, "Encoded data:\n");
            for(int i = 0; i < ret; i++) {
                printf("%02X", uper_ptr[i]);
            }
            printf("\n");
            fprintf(stderr, "--- End of encoded data\n\n");
        }

        if(uper_ptr != NULL) {
            asn1_free(uper_ptr);
        }
    }

    if(c_struct != NULL) {
        asn1_free_value(asn_type, c_struct);
    }

    return error;
}


static bool uper_to_xml(const ASN1CType* asn_type)
{
    bool error = false;
    void* c_struct = NULL;
    uint8_t uper_buf[INPUT_BUFFER_LENGTH] = {0};

    uint32_t uper_len = read_hex_from_stdin(uper_buf, sizeof(uper_buf));
    if(uper_len <= 0) {
        fprintf(stderr, "Unable to read stdin\n");
        error = true;
    }

    if(!error) {
        fprintf(stderr, "Decoding %d bytes\n", uper_len);
        ASN1Error err;
        int ret = asn1_uper_decode((void**)&c_struct,
                                   asn_type,
                                   uper_buf,
                                   uper_len,
                                   &err);
        if((ret < 0) || (c_struct == NULL)) {
            fprintf(stderr, "Decoding error: %s\n", err.msg);
            error = true;
        }
    }

    if(!error) {
        uint8_t* xer_ptr = NULL;
        int ret = asn1_xer_encode((uint8_t**)&xer_ptr, asn_type, c_struct);
        if((ret < 0) || (xer_ptr == NULL)) {
            fprintf(stderr, "Encoding error\n");
            error = true;
        } else {
            fprintf(stderr, "Encoded XML data:\n");
            printf("%s", xer_ptr);
            fprintf(stderr, "---- End of encoded XML data\n\n");
        }

        if(xer_ptr != NULL) {
            asn1_free(xer_ptr);
        }
    }

    if(c_struct != NULL) {
        asn1_free_value(asn_type, c_struct);
    }

    return error;;
}


static const ASN1CType* get_type(const char* in_type)
{
    bool error = (NULL == in_type);
    const ASN1CType* asn_type = NULL;

    if(!error) {
        for(int i = 0; l_asn_types[i].asn_type != NULL; i++) {
            if(strncmp(in_type, l_asn_types[i].name, strlen(l_asn_types[i].name)) == 0) {
                asn_type = l_asn_types[i].asn_type;
                break;
            }
        }
    }

    return asn_type;
}


int main(int argc, char* argv[])
{
    l_params_t params = {0};

    bool error = fetch_params(&params, argc, argv);

    if(error || params.help) {
        print_usage(argv[0]);
    } else {

        if(params.uper) {
            error = uper_to_xml(params.asn_type);
        }
        if(params.xml) {
            error = xml_to_uper(params.asn_type);
        }
    }

    return error;
}


static bool fetch_params(l_params_t* params, int argc, char* argv[])
{

    assert(params != NULL);
    assert(argv != NULL);

    bool error = false;
    int option = 0;
    char getopt_fmt[8U] = {0};

    snprintf(getopt_fmt, sizeof(getopt_fmt), "%c%c%c%c:",
             OPT_HELP,
             OPT_UPER,
             OPT_XML,
             OPT_TYPE);

    while((!error) && (0 < (option = getopt(argc, argv, (const char*)getopt_fmt)))) {
        switch(option) {
        case OPT_UPER:
            params->uper = true;
            break;
        case OPT_XML:
            params->xml = true;
            break;
        case OPT_HELP:
            params->help = true;
            break;
        case OPT_TYPE:
            if(NULL == optarg) {
                error = true;
            } else {
                params->asn_type = get_type(optarg);
                if(NULL == params->asn_type) {
                    fprintf(stderr, "unkown type: %s", optarg);
                    error = true;
                }
            }
            break;
        default:
            error = true;
        }
    }

    if(NULL == params->asn_type) {
        fprintf(stderr, "please specify ASN.1 type with -T\n");
        error = true;
    }

    if(!error) {
        if(!(params->uper ^ params->xml)) {
            fprintf(stderr, "please specify input -%c or -%c\n", OPT_UPER, OPT_XML);
            error = true;
        }
    }

    return error;
}


static void print_usage(const char* bin)
{
    fprintf(stderr, "Usage: %s <-%c|-%c> <-%c type> [-%c]\n",
            bin,
            OPT_UPER,
            OPT_XML,
            OPT_TYPE,
            OPT_HELP);
    fprintf(stderr, "  Conversion direction:\n");
    fprintf(stderr, "    -%c    UPER to XML\n", OPT_UPER);
    fprintf(stderr, "    -%c    XML to UPER\n", OPT_XML);
    fprintf(stderr, "  type is one of:");

    for(int i = 0; l_asn_types[i].asn_type != NULL; i++) {
        fprintf(stderr, " %s", l_asn_types[i].name);
    }

    fprintf(stderr, "\n");
    fprintf(stderr, "    -%c    help, print this usage\n", OPT_HELP);
}
