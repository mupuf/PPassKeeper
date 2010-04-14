#ifndef _LIBSAFELOCKER_H_
#define _LIBSAFELOCKER_H_

#include <QString>
#include <QList>

//Parameters
QString _safelockDefaultPath();
ppk_error _setPPKModuleForPassphrase(const QString& module);
ppk_error _setSafeLockerPath(const QString& path);
ppk_error _setClosingDelay(int delay);

//Accessors
QList<QString> _listEntries();
ppk_error _getEntry(const ppk_entry* entry, ppk_data **edata, unsigned int flags);
ppk_error _resetEntry(const ppk_entry* entry, const ppk_data* data, unsigned int flags);
ppk_error _removeEntry(const ppk_entry* entry, unsigned int flags);

#endif