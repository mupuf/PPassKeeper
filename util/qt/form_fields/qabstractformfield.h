#ifndef QABSTRACTFORMFIELD_H
#define QABSTRACTFORMFIELD_H

#include <QWidget>
#include <QString>
#include <QVariant>
//#include <QValidator>

class QAbstractFormField : public QObject
{
	Q_OBJECT
private:
	QVariant default_value;
	QString name, help_text;
	//QValidator::State state;
	void* user_data;

public:
	QAbstractFormField();

	void setFieldName(QString name);
	virtual void setDefaultValue(QVariant default_value);
	virtual void setValue(QVariant value)=0;
	//virtual bool setValidator(QValidator* validator)=0;
	virtual void setHelpText(QString help_text);
	void setUserData(void* data);

	QString fieldName();
	virtual QVariant defaultValue();
	virtual QVariant value()=0;
	//virtual QValidator::State validationState()=0;
	virtual QString helpText();
	void* userData();

	virtual QLayout* layout();
	virtual QWidget* widget()=0;
	virtual bool setWidget(QWidget* widget);

public slots:
	void reset();
	void showHelpText();

protected slots:
	//check validation
	void onInputChanged(QVariant inputValue);

signals:
	//void validationStateChanged(QValidator::State state);
};

#endif // QABSTRACTFORMFIELD_H
