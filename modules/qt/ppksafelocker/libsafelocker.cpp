#include "libsafelocker.h"

//Parameters
QString _safelockDefaultPath()
{
	return QString::fromUtf8(ppk_settings_directory())+QString::fromUtf8("ppk_safelock.ppksl");
}

ppk_error _setPPKModuleForPassphrase(const QString& module)
{
	return PPK_OK;
}

ppk_error _setSafeLockerPath(const QString& path)
{
	return PPK_OK;
}

ppk_error _setClosingDelay(int delay)
{
	return PPK_OK;
}

//Accessors
QList<QString> _listEntries()
{
	return QList<QString>();
}

ppk_error _getEntry(const ppk_entry* entry, ppk_data **edata, unsigned int flags)
{
	return PPK_OK;
}

ppk_error _resetEntry(const ppk_entry* entry, const ppk_data* data, unsigned int flags)
{
	return PPK_OK;
}

ppk_error _removeEntry(const ppk_entry* entry, unsigned int flags)
{
	return PPK_OK;
}