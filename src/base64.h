/** @file util/crypt/base64.h
  * @brief Base64 encoding
  * @author Apache Software Foundation
  * $Revision: 1.1 $
  */

#ifndef __BASE64_H__
#define __BASE64_H__

    #ifdef __cplusplus 
    extern "C"
    {
    #endif
        /* base64 functions */
        int ap_base64decode_len(const char *bufcoded, int buflen);
        int ap_base64decode(char *bufplain, const char *bufcoded, int buflen);
        int ap_base64decode_binary(unsigned char *bufplain, const char *bufcoded, int buflen);
        int ap_base64encode_len(int len);
        int ap_base64encode(char *encoded, const char *string, int len);
        int ap_base64encode_binary(char *encoded, const unsigned char *string, int len);

        /* convenience, result string must be free()'d by caller */
        char *b64_encode(char *buf, int len);
        char *b64_decode(char *buf);
    #ifdef __cplusplus 
    }
    #endif

#endif
