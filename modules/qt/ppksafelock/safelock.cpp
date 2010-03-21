#include "safelock.h"
#include "crypt.h"

#include <QStringList>
#include <QMapIterator>

QString SafeLock::createFile()
{
	QString file;

	file=QString::fromUtf8("#PPK_NETWORK\n");
	file+=QString::fromUtf8("Version: 1.0\n");
	file+=QString::fromUtf8("Revision: %1\n").arg(this->revision+1);
	file+=QString::fromUtf8("Data:\n");

	QMapIterator<QString, Entry> i(entries);
	while (i.hasNext()) {
		i.next();
		file+=i.value().toString();
	}

	file+=QString::fromUtf8("\n");

	qDebug("File = '%s'", qPrintable(file));

	return file;
}

SafeLock::SafeLock(QString safelockPath) : safelockPath(safelockPath), _isOpen(false), revision(0)
{

}

bool SafeLock::open(const char* passphrase)
{
	if(isOpen())
		return false; //it is already open

	//copy values
	this->passphrase=passphrase;
	this->_isOpen=true;

	char* c_data;
	if(decryptFromFile(qPrintable(safelockPath), &c_data, passphrase)==1)
	{
		QString data=QString::fromUtf8(c_data);
		QStringList lines=data.split('\n');

		qDebug("lines[0]='%s'", qPrintable(lines[0]));
		if(lines[0]!="#PPK_NETWORK")
		{
			//The passphrase was right, get the information

			/*QRegExp getVersion(QString::fromUtf8("Version: (\\d+\\.\\d+)\n")); //match 1.0
			if(getVersion.exactMatch(lines[1]))
			{
				QStringList matchedVars=getVersion.capturedTexts();
				QString version=matchedVars[1];
			}*/

			//get the revision number
			QRegExp getRevision(QString::fromUtf8("Revision: (\\d+)\n"));
			if(getRevision.exactMatch(lines[2]))
			{
				QStringList capTexts=getRevision.capturedTexts();
				this->revision=capTexts[1].toInt();
			}

			//Add all the data
			for(int i=4; i<lines.size(); i++)
			{
				Entry e(lines[i]);
				if(e!=Entry())
					entries[e.entry()]=e;
			}
		}
		else
		{
			close();
			return false; //invalid passphrase	
		}
	}

	return true;
}

bool SafeLock::flushChanges()
{
	QString data=createFile();
	return cryptToFile(qPrintable(data), qPrintable(safelockPath), qPrintable(passphrase))==1;
}

bool SafeLock::close()
{
	if(isOpen())
	{
		passphrase=QString();

		flushChanges();
		entries.clear();

		this->_isOpen=false;

		return true;
	}

	return false;
}

bool SafeLock::isOpen() const
{
	return _isOpen;
}

bool SafeLock::add(Entry e)
{
	if(entries.contains(e.entry()))
		return false;

	entries[e.entry()]=e;

	return true;
}

bool SafeLock::remove(QString entry)
{
	return entries.remove(entry)>0;
}

QList<QString> SafeLock::list() const
{
	return entries.keys();
}
