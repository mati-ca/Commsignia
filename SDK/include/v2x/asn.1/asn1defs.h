/** @file
@copyright
(C) Commsignia Ltd. - All Rights Reserved.
Unauthorised copying of this file, via any medium is strictly prohibited.
Proprietary and confidential.
@date 2020-2021
*/

/*
 * ASN1 definitions (application visible)
 * Copyright (C) 2011-2015 Fabrice Bellard
*/

#ifndef _ASN1_DEFS_H
#define _ASN1_DEFS_H

#include <stdlib.h>
#include <stdio.h>
#include <inttypes.h>
#include <math.h>

#ifdef  __cplusplus
extern "C" {
#endif

/**
@defgroup asn ASN.1
Types and conversion functions for V2X messages based on ASN.1 description
*/

/**
@defgroup asn_tools ASN.1 tools
@ingroup asn
Conversion functions for encoding and decoding ASN.1 types
*/

/**
@ingroup asn_tools
@{
*/

#ifndef _BOOL_defined
#define _BOOL_defined

typedef int BOOL;
typedef enum {
    ASN1_FALSE = 0,
    ASN1_TRUE = 1,
} asn1_bool;
#endif

/** Used for:
   OCTET STRING: len is in bytes
   SEQUENCE OF: len is a number of items buf contains len * sizeof(struct) bytes
*/
typedef struct ASN1String {
    uint8_t *buf;
    int len;
} ASN1String;

typedef ASN1String ASN1ObjectIdentifier;

/**
    ASN1BitString
*/
typedef struct ASN1BitString {
    uint8_t *buf;
    int len; /** len is in bits. buf contains ceil(len/8) bytes */
} ASN1BitString;

/**
    Represent big numbers
*/
typedef uint32_t ASN1Limb;

typedef uint8_t ASN1Null;

typedef struct ASN1Integer {
    ASN1Limb *data;
    int len; /** 0 is represented as len = 0 and negative = 0 or 1 */
    int allocated_len; /** length (>= len) allocated for tab */
    int negative; /** 0 = positive, 1 = negative */
} ASN1Integer;

/**
    Compressed ASN1 type definition
*/
typedef uintptr_t ASN1CType;

/**
    Open type, used for extensions
*/
typedef struct ASN1OpenType {
    const ASN1CType *type; /** if the 'type' is NULL, 'octet_string' contains
                               the raw content. Otherwise, 'data' is the
                               corresponding value. */
    union {
        void *data;
        ASN1String octet_string;
    } u;
} ASN1OpenType;

/**
    Error message
*/
typedef struct ASN1Error {
    int line_num; /** used for GSER decoding */
    int bit_pos; /** used for PER decoding */
    char msg[512]; /** message */
} ASN1Error;

/** it uses malloc, do not forget to call asn1_free to avoid memory leaking*/
void *asn1_malloc(int size);
void *asn1_realloc(void *ptr, int size);
void asn1_free(void *ptr);

/** same as malloc but initialise to 0 */
void *asn1_mallocz(int size);

/** Return the size (in bytes) of an ASN1 typed value */
int asn1_get_size(const ASN1CType *p);

/** Allocate a value of the ASN1 type 'p'. All fields are set to zero. */
void *asn1_mallocz_value(const ASN1CType *p);

/** Free a value of the ASN1 type 'p' */
void asn1_free_value(const ASN1CType *p, void *data);

/** Free all the values referenced by the structure 'data' but not data
   iteself. asn1_free_value(p, data) is equivalent to
   asn1_free_value_struct(p, data) followed by asn1_free(data). */
void asn1_free_value_struct(const ASN1CType *p, void *data);

/** Compare two ASN1 values of type 'p'. Return < 0 for less than, ==
   0 for equal, or > 0 for larger than. For composite values, a
   lexicographical ordering is assumed. */
int asn1_cmp_value(const ASN1CType *p, const void *data1,
                   const void *data2);

/** Copy 'data2' to 'data1' assuming 'data1' is allocated. All
   referenced data inside 'data1' is allocated. Return 0 if OK, < 0 if
   error. */
int asn1_copy_value(const ASN1CType *p, void *data1, const void *data2);

/** Clone the value 'data' of type 'p' (equivalent to
   asn1_mallocz_value() followed by asn1_copy_value()). Return NULL
   if error. */
void *asn1_clone_value(const ASN1CType *p, const void *data);


/** unaligned PER encode & decode */
int asn1_uper_encode2(uint8_t **pbuf, const ASN1CType *p, const void *data,
                      ASN1Error *err);
int asn1_uper_decode(void **pdata, const ASN1CType *p,
                     const uint8_t *buf, int buf_len, ASN1Error *err);
int asn1_uper_decode_ng(void **pdata, const ASN1CType *p, uint32_t p_u32_bit_offset,
                        const uint8_t *buf, int buf_len, ASN1Error *err, uint32_t* p_u32p_consumed_bits);

/** XER encode & decode */
int asn1_xer_encode(uint8_t **pbuf, const ASN1CType *p, const void *data);
int asn1_xer_decode(void **pdata, const ASN1CType *p, const uint8_t *buf,
                    int buf_len, ASN1Error *err);

/** Return 1 if the constraints are satisfied. Otherwise return
   0 and put an informative message string in 'msg_buf' of maximum
   size 'msg_buf_size'.  always ese berfore asn1_uper_encode2 */
int asn1_check_constraints(const ASN1CType *p, const void *data,
                            char *msg_buf, int msg_buf_size);

/** Large integer functions, use for  ASN1Integer set and get*/
void asn1_integer_set_ui64(ASN1Integer *r, uint64_t v);
int asn1_integer_get_ui64_ov(const ASN1Integer *r, uint64_t *pval);

/**
    Add p type data to the end of the list
    @param  p               type
    @param[in/out]  count   in: current number of elements with type p in the list, out: increment by 1
    @param[in/out]  list    in: pointer for array of p type elements, out: list is realloced and data is in the end of the list
    @param data             data to add, it has type p
    @return 0 in case of succes
*/
int asn1_seq_add(const ASN1CType *p, int* count, void** list, void* data);

/**
    Delete p type data from index of the list
    @param  p               type
    @param[in/out]  count   in: current number of elements with type p in the list, out: decrement by 1
    @param[in/out]  list    in: pointer for array of p type elements, out: list is realloced and data is removed from index,
    or NULL if list become empty
    @param index            index of the element to remove
    @return 0 in case of succes
*/
int asn1_seq_del(const ASN1CType *p, int* count, void** list, int index);

/**
    Parse an ASN1BitString into uint8_t
    @param p_bitString       pointer to input ASN1BitString
    @param p_u8p_output      pointer to output uint8_t
    @return                  0 if success, else failure
*/
int asn1_conv_bitstring_8(const ASN1BitString* p_bitString,
                                      uint8_t* p_u8p_output);

/**
    Parse an ASN1BitString into uint16_t
    @param p_bitString       pointer to input ASN1BitString
    @param p_u16p_output     pointer to output uint16_t
    @return                  0 if success, else failure
 */
int asn1_conv_bitstring_16(const ASN1BitString* p_bitString,
                                      uint16_t* p_u16p_output);

/**
    Parse an ASN1BitString into uint32_t
    @param p_bitString       pointer to input ASN1BitString
    @param p_u32p_output     pointer to output uint32_t
    @return                  0 if success, else failure
*/
int asn1_conv_bitstring_32(const ASN1BitString* p_bitString,
                                      uint32_t* p_u32p_output);


/**
@}
*/

/**
@defgroup asn_types ASN.1 types
@ingroup asn
Messaging types based on the ASN.1 message descriptors.
*/

/**
@file v2x_asn.h
@ingroup asn_types
@note Auto-generated file.
*/

#ifdef  __cplusplus
}
#endif

#endif /** _ASN1_DEFS_H */
