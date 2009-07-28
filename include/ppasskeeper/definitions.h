#ifndef PPK_DEFINITIONS_H
#define PPK_DEFINITIONS_H

///Boolean 
typedef enum
{
	///FALSE
	PPK_FALSE=0,
	///TRUE
	PPK_TRUE=1
} ppk_boolean;

///Error codes
typedef enum
{
        PPK_OK = 0,
	PPK_UNKNOWN_ERROR=1,
	PPK_LOCKED_NO_ACCESS=100,
        PPK_ALREADY_LOCKED=101,
        PPK_CANNOT_LOCK=102,
        PPK_CANNOT_UNLOCK=103,
        PPK_NOT_LOCKED=104,
        PPK_MODULE_UNAVAILABLE=200,
	PPK_CANNOT_SAVE_PARAM=300,
	PPK_CANNOT_GET_PARAM=301,
} ppk_error;

///Security Level
typedef enum
{
	///Lowest (eg Stored in a plain-text file)
	ppk_sec_lowest=0,
	///Scrambled (eg Stored in a poorly-encrypted file)
	ppk_sec_scrambled=1,
	///Safe (eg Keyrings/Wallets)	
	ppk_sec_safe=2,		
	///Perfect (eg Prompt users to key-in their password)	
	ppk_sec_perfect=3
} ppk_security_level;

///Reading flags
typedef enum {
	///Retrieve the password by the default way
	ppk_rf_none=0,
	///Will retrieve the password silently or abort, but won't prompt anything to users
	ppk_rf_silent=1
} ppk_readFlag;

///Writing flags
typedef enum {
	///Retrieve the password by the default way
	ppk_wf_none=0,
	///Will retrieve the password silently or abort, but won't prompt anything to users
	ppk_wf_silent=1
} ppk_writeFlag;

///Listing flags
typedef enum {
	///Retrieve the password by the default way
	ppk_lf_none=0,
	///Will retrieve the password silently or abort, but won't prompt anything to users
	ppk_lf_silent=1
} ppk_listingFlag;

#endif
