/******************************************************************************
  Copyright (C), 2001-2016, Allwinner Tech. Co., Ltd.
 ******************************************************************************
  File Name     : aw_cipher.h
  Version       : Initial Draft
  Author        : Allwinner BU3-PD2 Team
  Created       : 2016/03/20
  Last Modified :
  Description   : common parameter definition for cipher module
  Function List :
  History       :
******************************************************************************/

#ifndef _AW_CIPHER_COMMON_H_
#define _AW_CIPHER_COMMON_H_

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <openssl/engine.h>
#include <openssl/aes.h>

#define ENGINE_load_builtin_engines ENGINE_load_af_alg
extern void ENGINE_load_af_alg(void);


#define HW_OFFSET (30)
#define HW_MASK (0x3ul<<HW_OFFSET)

#define TP_OFFSET (26)
#define TP1_MASK (0xful<<TP_OFFSET)

#define ALG_OFFSET (22)
#define ALG_MASK (0xful<<ALG_OFFSET)
#define MODE_OFFSET (16)
#define MODE_MASK (0x3ful<<MODE_OFFSET)
#define KEY_LEN_OFFSET (8)
#define KEY_LEN_MASK (0xfful<<KEY_LEN_OFFSET)

#define BW_OFFSET (4)
#define BW_MASK (0xful<<BW_OFFSET)

typedef enum
{
    AW_CIPHER_SW           = 0x0,  /**< software */
    AW_CIPHER_HW           = 0x1,  /**< hardware */
    AW_CIPHER_TYPE_BUTT
}AW_CIPHER_TYPE_E;

typedef enum
{
    AW_CIPHER_SUBTYPE1_CRYPTO         = 0x0,  
    AW_CIPHER_SUBTYPE1_HASH           = 0x1, 
    AW_CIPHER_SUBTYPE1_BUTT
}AW_CIPHER_SUBTYPE1_E;


typedef enum
{
    AW_CIPHER_MODE_ECB,        /**<Electronic codebook (ECB) mode*/
    AW_CIPHER_MODE_CBC,        /**<Cipher block chaining (CBC) mode*/
    AW_CIPHER_MODE_CFB,        /**<Cipher feedback (CFB) mode*/
    AW_CIPHER_MODE_OFB,        /**<Output feedback (OFB) mode*/
    AW_CIPHER_MODE_CTR,        /**<Counter (CTR) mode*/
    AW_CIPHER_MODE_CBC_CTS,    /**<Cipher block chaining CipherStealing mode*/
    AW_CIPHER_MODE_CCM,
    AW_CIPHER_MODE_GCM,  
    AW_CIPHER_MODE_3DES,		/*aw virtual mode*/
    AW_CIPHER_MODE_BUTT
}AW_CIPHER_WORK_MODE_E;

typedef enum
{
    AW_HASH_MD5 = 0,        
    AW_HASH_SHA1,       
    AW_HASH_SHA256,        
    AW_HASH_SHA384,       
    AW_HASH_SHA512,
    AW_HMAC_BEGIN,
    AW_HASH_HMAC_SHA1,
    AW_HASH_HMAC_SHA256,
    AW_HMAC_END,
    AW_HASH_BUTT
}AW_HASH_WORK_MODE_E; /*AW private definition*/


/** Cipher algorithm */
typedef enum
{
    AW_CIPHER_ALG_DES           = 0x0,  
    AW_CIPHER_ALG_3DES          = 0x1, 
    AW_CIPHER_ALG_AES           = 0x2,   
    AW_CIPHER_ALG_SHA           = 0x3, 
    AW_CIPHER_ALG_TRNG          = 0x4,
    AW_CIPHER_ALG_PRNG          = 0x5,
    AW_CIPHER_ALG_BUTT
}AW_CIPHER_ALG_E;


/** Key info */
typedef enum
{
    AW_CIPHER_KEY_AES_128BIT    = 0x0,  /**< 128-bit key for the AES algorithm */
    AW_CIPHER_KEY_AES_192BIT    = 0x1,  /**< 192-bit key for the AES algorithm */
    AW_CIPHER_KEY_AES_256BIT    = 0x2,  /**< 256-bit key for the AES algorithm */
    AW_CIPHER_KEY_DES_3KEY      = 0x3,  /**< Three keys for the 3DES algorithm */
    AW_CIPHER_KEY_DES_2KEY      = 0x4,  /**< Two keys for the 3DES algorithm */
    AW_CIPHER_KEY_DES = 0x7,  /*aw virtual key*/
    AW_CIPHER_KEY_LEN_BUTT
}AW_CIPHER_KEY_LENGTH_E;


/** Cipher block bit width */
typedef enum
{
    AW_CIPHER_BIT_WIDTH_64BIT   = 0x0,  /**< 64-bit width */
    AW_CIPHER_BIT_WIDTH_8BIT    = 0x1,  /**< 8-bit width */
    AW_CIPHER_BIT_WIDTH_1BIT    = 0x2,  /**< 1-bit width */
    AW_CIPHER_BIT_WIDTH_128BIT  = 0x3,  /**< 128-bit width */
    AW_CIPHER_BIT_WIDTH_BUTT
}AW_CIPHER_BIT_WIDTH_E;

enum
{
  AW_ERR_CIPHER_OK = 0,
  AW_ERR_CIPHER_OP_FAILED = -1,
  AW_ERR_CIPHER_NOT_INIT = -2,
  AW_ERR_CIPHER_INVALID_ARG = -3,
  AW_ERR_CIPHER_HEAP = -4,
  AW_ERR_CIPHER_RANDBYTES_TOO_LARGE = -5,
  AW_ERR_CIPHER_NOT_SUPPORTED = -6,
};

typedef struct
{
	unsigned int  hw:2;
	unsigned int  tp_sub1:4;
	unsigned int  alg:4;
	unsigned int  mode:6;
	unsigned int  key_len:8;
	unsigned int  bw:4 ; /*only valid for block mode : indicate the bit width*/
	unsigned int  rsv:4;
}AW_CIPHER_TYPE_S;

/** Encryption/Decryption type selecting */
typedef enum
{
    AW_CIPHER_KEY_SRC_RAM       = 0x0,  /**<  Key from ram*/
    AW_CIPHER_KEY_SRC_SECHW,          /**< Key from efuse or CE internal ram*/
    AW_CIPHER_KEY_SRC_BUTT,
}AW_CIPHER_KEY_SRC_E;

#define CE_KS_SSK "KEY_SEL_SSK"
#define CE_KS_HUK "KEY_SEL_HUK"
#define CE_KE_RSSK "KEY_SEL_RSSK"
#define CE_KS_INTERNAL_0 "KEY_SEL_INTRA_0"
#define CE_KS_INTERNAL_1 "KEY_SEL_INTRA_1"
#define CE_KS_INTERNAL_2 "KEY_SEL_INTRA_2"
#define CE_KS_INTERNAL_3 "KEY_SEL_INTRA_3"
#define CE_KS_INTERNAL_4 "KEY_SEL_INTRA_4"
#define CE_KS_INTERNAL_5 "KEY_SEL_INTRA_5"
#define CE_KS_INTERNAL_6 "KEY_SEL_INTRA_6"
#define CE_KS_INTERNAL_7 "KEY_SEL_INTRA_7"

typedef struct
{
    unsigned char *u8pKey;
    unsigned char *u8pIv;
	int  klen;
	int  ivlen;/*Bytes*/
    unsigned int  enc;    /*1 == encrypt, 0 == decrypt */  
    AW_CIPHER_KEY_SRC_E enKeySrc;  /**Key source,only valid for hw handle */
    char hw_key[16]; /*reference API doc to get detail information*/
} AW_CIPHER_CFG_S;

typedef struct
{
	ENGINE    *engine;
	const EVP_CIPHER       *e_cipher; 
	EVP_CIPHER_CTX   ctx; 
	AW_CIPHER_CFG_S cip_cfg;
}AW_CIPHER_CRYPTO_S;

typedef int (*AW_PACK_FUNC)(EVP_MD * evp_md,void * key,\
int key_len,const unsigned char * d,size_t n,\
unsigned char * md,unsigned int * md_len);
#define HMAC_MAX_KLEN 128
typedef struct 
{
	EVP_MD       *e_md; /*only valid for hmac*/
	char  key[HMAC_MAX_KLEN];
	int key_len;
} HMAC_PARAM_S;

typedef struct
{
	ENGINE    *engine;
	const EVP_MD       *e_md; 
	AW_PACK_FUNC aw_func;
	HMAC_PARAM_S hm_para;
	EVP_MD_CTX   ctx; 
}AW_CIPHER_HASH_S;

typedef union 
{
	AW_CIPHER_HASH_S hash;
	AW_CIPHER_CRYPTO_S crypto;
}AW_CIPHER_SUB1_U;

#define aw_construct_type(type,bhw,btp_sub1,balg,bmode,bkey_len,bbw) \
do{\
type.hw = (bhw); \
type.tp_sub1 = (btp_sub1);\
type.alg = (balg);\
type.mode = (bmode);\
type.key_len = (bkey_len);\
type.bw = (bbw);\
}while (0); 


typedef enum
{
	AW_HASH_CMD_DIGEST = 0,
	AW_HASH_CMD_BUTT,
}AW_HASH_CMD_E;


typedef enum
{
	/*for stream like submit*/
	AW_CIPHER_CMD_STRM_UPDATE = 0,
	AW_CIPHER_CMD_STRM_FINAL,

	/*for single packet submit*/
	AW_CIPHER_CMD_PACKET_SINGLE,
	AW_CIPHER_CMD_BUTT,
}AW_CIPHER_CMD_E;

typedef enum
{
	AW_CIPHER_HANDLE_STATUS_INITED = 0,
	AW_CIPHER_HANDLE_STATUS_CFGED,
	AW_CIPHER_HANDLE_STATUS_IOING,
	AW_CIPHER_HANDLE_STATUS_IOED,
	AW_CIPHER_HANDLE_STATUS_BUTT,
}AW_CIPHER_HANDLE_STATUS_E;

typedef struct
{
	unsigned char * md;
	unsigned int *p_mdlen;
	unsigned char * in;
	int ilen;
} AW_HASH_DESC_S;

typedef struct
{
	unsigned char * out;
	int *olen ;
	unsigned char * in;
	int ilen;
} AW_PACKET_DESC_S;

typedef struct
{
	AW_CIPHER_CMD_E cmd;
	AW_PACKET_DESC_S packet_desc;
} AW_CRYPTO_OPS_S;

typedef struct
{
	AW_HASH_CMD_E cmd;
	AW_HASH_DESC_S hash_desc;	
} AW_HASH_OPS_S;

typedef struct
{
	union 
	{
		AW_CRYPTO_OPS_S crypto_ops;
		AW_HASH_OPS_S hash_ops;
	}ops;
	
} AW_CIPHER_OPS_S;


typedef enum
{
    AW_CIPHER_RSA_ENC_SCHEME_NO_PADDING,           
    AW_CIPHER_RSA_ENC_SCHEME_BLOCK_TYPE_0,         
    AW_CIPHER_RSA_ENC_SCHEME_BLOCK_TYPE_1,         
    AW_CIPHER_RSA_ENC_SCHEME_BLOCK_TYPE_2,         
    AW_CIPHER_RSA_ENC_SCHEME_RSAES_OAEP_SHA1, 
    AW_CIPHER_RSA_ENC_SCHEME_RSAES_OAEP_SHA256,
    AW_CIPHER_RSA_ENC_SCHEME_RSAES_PKCS1_V1_5, 
    AW_CIPHER_RSA_SCHEME_BUTT,
}AW_CIPHER_RSA_ENC_SCHEME_E;

/** RSA signature scheme*/
typedef enum
{
    AW_CIPHER_RSA_SIGN_SCHEME_RSASSA_PKCS1_V15_SHA1 = 0x100, 
    AW_CIPHER_RSA_SIGN_SCHEME_RSASSA_PKCS1_V15_SHA256,      
    AW_CIPHER_RSA_SIGN_SCHEME_RSASSA_PKCS1_V15_SHA512,
    AW_CIPHER_RSA_SIGN_SCHEME_RSASSA_PKCS1_PSS_SHA1,      
    AW_CIPHER_RSA_SIGN_SCHEME_RSASSA_PKCS1_PSS_SHA256, 
    AW_CIPHER_RSA_SIGN_SCHEME_BUTT,
}AW_CIPHER_RSA_SIGN_SCHEME_E;

/** RSA public key encryption struct input */
typedef struct
{
    AW_CIPHER_RSA_ENC_SCHEME_E enScheme;   /** RSA encryption scheme*/
    unsigned char *stPubKey;
}AW_CIPHER_RSA_PUB_ENC_S;

/** RSA private key decryption struct input */
typedef struct
{
    AW_CIPHER_RSA_ENC_SCHEME_E enScheme; /** RSA encryption scheme */
    unsigned char  *stPriKey;
}AW_CIPHER_RSA_PRI_ENC_S;

/** RSA signature struct input */
typedef struct
{
    AW_CIPHER_RSA_SIGN_SCHEME_E enScheme;  /** RSA signature scheme*/
    unsigned char *stPriKey;
 }AW_CIPHER_RSA_SIGN_S;

/** RSA signature verify struct input */
typedef struct
{
    AW_CIPHER_RSA_SIGN_SCHEME_E enScheme; /** RSA signature scheme*/
    unsigned char *stPubKey;
 }AW_CIPHER_RSA_VERIFY_S;

int AW_CIPHER_RsaGenPubKey(const char *path_key,
	unsigned char *PubKey, unsigned int* p_klen);
	

int AW_CIPHER_RsaGenPriKey(const char *path_key,
				unsigned char *PriKey, unsigned int* p_klen);

/**
* @brief   RSA encrypt a plaintext with a RSA public key.
* @param pstRsaEnc:  encryption attribute.
* @param pu8Input:    input data to be encryption.
* @param u32InLen:   length of input data.
* @param pu8Output: output data.
* @param pu32OutLen: length of output data.
* @return error code
* @note
*/
int AW_CIPHER_RsaPublicEnc(AW_CIPHER_RSA_PUB_ENC_S *pstRsaEnc,
	              unsigned char *pu8Input, unsigned int u32InLen,
	              unsigned char *pu8Output, unsigned int *pu32OutLen);


/**
* @brief   RSA decrypt a ciphertext with a RSA private key.
* @param pstRsaDec:   decryption attribute.
* @param pu8Input:    input data to be decryption.
* @param u32InLen:   length of input data.
* @param pu8Output: output data.
* @param pu32OutLen: length of output data.
* @return error code
* @note
*/
int AW_CIPHER_RsaPrivateDec(AW_CIPHER_RSA_PRI_ENC_S *pstRsaDec,
                   unsigned char *pu8Input, unsigned int u32InLen,
                   unsigned char *pu8Output, unsigned int *pu32OutLen);

/**
* @brief   RSA encrypt a plaintext with a RSA private key.
* @param pstRsaEnc:  encryption attribute.
* @param pu8Input:    input data to be encryption.
* @param u32InLen:   length of input data.
* @param pu8Output:output data.
* @param pu32OutLen: length of output data.
* @return error code
* @note
*/
int AW_CIPHER_RsaPrivateEnc(AW_CIPHER_RSA_PRI_ENC_S *pstRsaEnc,
                   unsigned char *pu8Input, unsigned int u32InLen,
                   unsigned char *pu8Output, unsigned int *pu32OutLen);



/**
* @brief   RSA decrypt a ciphertext with a RSA public key.
* @param pstRsaDec:   decryption attribute.
* @param pu8Input:    input data to be decryption.
* @param u32InLen:   length of input data.
* @param pu8Output:output data.
* @param pu32OutLen: length of output data.
* @return error code
* @note
*/
int AW_CIPHER_RsaPublicDec(AW_CIPHER_RSA_PUB_ENC_S *pstRsaDec,
               unsigned char *pu8Input, unsigned int u32InLen,
               unsigned char *pu8Output, unsigned int *pu32OutLen);



int AW_CIPHER_RsaSign(AW_CIPHER_RSA_SIGN_S *pstRsaSign,
             unsigned char *pu8InData, unsigned int u32InDataLen,
             unsigned char *pu8OutSign, unsigned int *pu32OutSignLen);


int AW_CIPHER_RsaVerify(AW_CIPHER_RSA_VERIFY_S *pstRsaVerify,
                   unsigned char *pu8InData, unsigned int u32InDataLen,
                   unsigned char *pu8InSign, unsigned int u32InSignLen);

int AW_CIPHER_Rand(unsigned char* buf,unsigned int num);                             


int AW_CIPHER_Init();

int AW_CIPHER_DeInit();

int AW_CIPHER_CreateHandle(void** pphandle,
AW_CIPHER_TYPE_S type);

int AW_CIPHER_DestroyHandle(void** pphdl);

int AW_CIPHER_ConfigHandle(void* phandle,
AW_CIPHER_CFG_S* cfg);

int AW_CIPHER_SendCmd(void* phdl, AW_CIPHER_OPS_S* cmd);

                          
#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* _AW_CIPHER_COMMON_H_*/

