#include <ppasskeeper/definitions.h>

#include <libintl.h>

/// Error messages matching ppk_error (see definitions.h)
#ifdef __cplusplus
extern "C"
{
#endif

const char *ppk_error_get_string(ppk_error error_code)
{
	const char *msg;

	switch(error_code)
	{
		case PPK_OK:
			msg="Everything went fine";
			break;
		case PPK_UNKNOWN_ERROR:
			msg="Unknown error";
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
	}

	return msg ? gettext(msg) : "<unknown error>";
}

#ifdef __cplusplus
}
#endif
