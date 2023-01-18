/* *******************************************************************************
* Copyright (C), 2001-2018, xiaoyi. Co., Ltd.
* *******************************************************************************/
/**
* @file dd_encrypt.h
* @brief dd_encrypt
*
*
* @version v0.1
* @date 2018-05-28
*/
#ifndef __DD_ENCRYPT_H__
#define __DD_ENCRYPT_H__

#include <stdio.h>
#include <memory.h>
#include <pthread.h>
#include <string>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <openssl/engine.h>
#include <openssl/pkcs12.h>
#include <openssl/conf.h>
#include <openssl/opensslconf.h>
#include <openssl/err.h>
#include <openssl/pem.h>
#include <openssl/rsa.h>
#include <openssl/md5.h>
#include "dd_debug.h"

# define RSA_SIGN        1
# define RSA_VERIFY      2
# define RSA_ENCRYPT     3
# define RSA_DECRYPT     4

# define KEY_PRIVKEY     1
# define KEY_PUBKEY      2
# define KEY_CERT        3

# define FORMAT_UNDEF    0
# define FORMAT_ASN1     1
# define FORMAT_TEXT     2
# define FORMAT_PEM      3
# define FORMAT_NETSCAPE 4
# define FORMAT_PKCS12   5
# define FORMAT_SMIME    6
# define FORMAT_ENGINE   7
# define FORMAT_IISSGC   8      /* XXX this stupid macro helps us to avoid
                                   360                                  * adding yet another param to load_*key() */
# define FORMAT_PEMRSA   9      /* PEM RSAPubicKey format */
# define FORMAT_ASN1RSA  10     /* DER RSAPubicKey format */
# define FORMAT_MSBLOB   11     /* MS Key blob format */
# define FORMAT_PVK      12     /* MS PVK file format */
# define FORMAT_HTTP     13     /* Download using HTTP */

# define EXT_COPY_NONE   0
# define EXT_COPY_ADD    1
# define EXT_COPY_ALL    2



#define RSA_PUB_KEY         "-----BEGIN PUBLIC KEY-----\nMIGfMA0GCSqGSIb3DQEBAQUAA4GNADCBiQKBgQC7ZGyulWgOTi5W7bL67tM6sdCo\nrKQ/tsiUVHO1bt9hzJQAjT8E96/dIb0R3ChQ2cafs2kjZCkCOqisXz7JqoaqVS91\nFvC2/z0oF0am3TbtN9LZalhqNhiMnzct5PSziICI7lWbzTmgjKiTHsdGIbs1HTcZ\nClb/WEt3joM0E8I0fQIDAQAB\n-----END PUBLIC KEY-----"

# define APP_PASS_LEN    1024
 # define PW_MIN_LENGTH 4

static UI_METHOD *ui_method = NULL;

static int seeded = 0;
static int egdsocket = 0;

typedef struct pw_cb_data {
    const void *password;
    const char *prompt_info;
} PW_CB_DATA;




class DD_ENCRYPT
{
public:
	DD_ENCRYPT();
	virtual ~DD_ENCRYPT();
	static DD_ENCRYPT* GetInstance();

    /*
     *  Description :   get RSA public key.
     *  Parameter   :
     *      rsa_pub_key:    buffer to load RSA public key.
     *  Return      :   >0 : success, return RSA public key length
     *                  -1 : failed
     */
    int GetRSAPubKey(char* rsa_pub_key);

    /*
     *  Description :   encrypt endata with rsa_pub_key; endata maybe : key1 or iv2.
     *  Parameter   :
     *      in_data  :   the data to be encrypted.
     *      rsa_pub_key:    rsa public key.
     *      out_endata:     the encrypted data.
     *  Return      :   0  : success
     *                  -1 : failed
     */
    int EncryptRSAPub(const char *in_data, char* rsa_pub_key, char* out_endata);

    /*
     *  Description :   get file md5.
     *  Parameter   :
     *      file_path:  /filepath/name
     *      md5_str :   the md5 result.
     *  Return      :   =0 : success
     *                  -1 : failed
     */
    int GenFileMD5(const char *file_path, char *md5_str);

    /*
     *  Description :   get data content md5.
     *  Parameter   :
     *      data    :   data content.
     *      md5_str :   the md5 result.
     *  Return      :   =0 : success
     *                  -1 : failed
     */
    int GenDataMD5(const char *data, int datalen, char *md5_str);

private:
	pthread_mutex_t encrypt_lock;

    int app_RAND_load_file(const char *file, BIO *bio_e, int dont_warn);

    int load_config(BIO *err, CONF *cnf);

    ENGINE* try_load_engine(BIO *err, const char *engine, int debug);

    ENGINE* setup_engine(BIO *err, const char *engine, int debug);

    //int password_callback(char *buf, int bufsiz, int verify, PW_CB_DATA *cb_tmp);

    char* app_get_pass(BIO *err, char *arg, int keepbio);

    int app_passwd(BIO *err, char *arg1, char *arg2, char **pass1, char **pass2);

    EVP_PKEY *load_netscape_key(BIO *err, BIO *key, const char *file,
            const char *key_descrip, int format);

    int load_pkcs12(BIO *err, BIO *in, const char *desc,
             pem_password_cb *pem_cb, void *cb_data,
             EVP_PKEY **pkey, X509 **cert, STACK_OF(X509) **ca);

    EVP_PKEY* load_pubkey(BIO *err, const char *file, int format, int maybe_stdin,
            const char *pass, ENGINE *e, const char *key_descrip);

    EVP_PKEY* load_key(BIO *err, const char *file, int format, int maybe_stdin,
            const char *pass, ENGINE *e, const char *key_descrip);
};


#endif

