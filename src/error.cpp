#include <ppasskeeper/definitions.h>

#include <cstdlib>

#ifdef I18N
#include <libintl.h>
#endif

/// Error messages matching ppk_error (see definitions.h)
#ifdef __cplusplus
extern "C"
{
#endif

const char *ppk_error_get_string(ppk_error error_code)
{
	const char *msg = NULL;

	switch(error_code)
	{
		case PPK_OK:
			msg="Everything went fine";
			break;
		case PPK_UNKNOWN_ERROR:
			msg="Unknown error";
			break;
		case PPK_UNSUPPORTED_METHOD:
			msg="Unsupported method";
			break;
		case PPK_LOCKED_NO_ACCESS:
			msg="PPassKeeper is locked, access denied";
			break;
		case PPK_ALREADY_LOCKED:
			msg="PPassKeeper is already locked";
			break;
		case PPK_CANNOT_LOCK:
			msg="PPassKeeper could not be locked (check your permissions)";
			break;
		case PPK_CANNOT_UNLOCK:
			msg="PPassKeeper could not be unlocked (it is not locked)";
			break;
		case PPK_NOT_LOCKED:
			msg="PPassKeeper could not be unlocked (it is not locked)";
			break;
		case PPK_MODULE_UNAVAILABLE:
			msg="No such module is available";
			break;
		case PPK_CANNOT_SAVE_PARAM:
			msg="The parameter cannot be saved";
			break;
		case PPK_CANNOT_GET_PARAM:
			msg="The requested parameter could not be retrieved";
			break;
		case PPK_ENTRY_UNAVAILABLE:
			msg="The requested entry could not be retrieved";
			break;
		case PPK_UNKNOWN_ENTRY_TYPE:
			msg="The requested entry has un unknown type";
			break;
		case PPK_FILE_CANNOT_BE_ACCESSED:
			msg="A file cannot be accessed. Check your permissions";
			break;
		case PPK_INSUFFICIENT_MEMORY:
			msg="Insufficient memory";
			break;
		case PPK_CANNOT_OPEN_PASSWORD_MANAGER:
			msg="Cannot open the requested password manager";
			break;
		case PPK_USER_CANCELLED:
			msg="User pressed cancel";
			break;
		case PPK_INCOMPATIBLE_FLAGS:
			msg="The requested flags are incompatible with the chosen method";
			break;
	}

	#ifdef I18N
	return msg ? gettext(msg) : gettext("<Undefined error>");
	#else
	return msg?msg:"<Undefined error>";
	#endif
}

#ifdef __cplusplus
}
#endif
