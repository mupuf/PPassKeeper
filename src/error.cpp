#include <ppasskeeper/definitions.h>

#include <libintl.h>

/// Error messages matching ppk_error (see definitions.h)
static const char *error_messages[] = {
	"",																			// PPK_OK
	"PPassKeeper is locked, access denied",										// PPK_LOCKED_NO_ACCESS
	"PPassKeeper is already locked",											// PPK_ALREADY_LOCKED
	"PPassKeeper could not be locked (check your permissions)"					// PPK_CANNOT_LOCK
	"PPassKeeper could not be unlocked (check your password and permissions)",	// PPK_CANNOT_UNLOCK
	"PPassKeeper could not be unlocked (it is not locked)",						// PPK_NOT_LOCKED
	"No such module is available"												// PPK_MODULE_UNAVAILABLE
	"Unknown error",															// PPK_UNKNOWN_ERROR
};

const char *ppk_error_get_string(ppk_error error_code)
{
	const char *msg = error_messages[error_code];
	return msg ? gettext(msg) : "";
}
