/*
PPassKeeper
Copyright (C) 2008-2010 MùPùF.org

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/

#include "editparams.h"
#include "ui_editparams.h"

#include <QSpinBox>
#include <QLineEdit>
#include <QDoubleSpinBox>
#include <QMessageBox>
#include <QPushButton>

EditParams::EditParams(QWidget *parent) :
	QDialog(parent),
	m_ui(new Ui::EditParams),
	catTab(NULL),
	module(NULL)
{
	m_ui->setupUi(this);

	connect(m_ui->buttonBox, SIGNAL(clicked(QAbstractButton*)), this, SLOT(buttonBoxClicked(QAbstractButton*)));
}

EditParams::~EditParams()
{
    delete m_ui;
}

void EditParams::setModule(const char* module)
{
	//Set up the tab widget before rendering into it
	if(catTab!=NULL)
		delete catTab;
	catTab = new QTabWidget(this);
	m_ui->mainLayout->insertWidget(1, catTab);

	//Change the module name
	this->module = module;
	m_ui->lbl_modulename->setText(QString::fromUtf8(ppk_module_display_name(module)));
	m_ui->lbl_textintro->setText(tr("Edit settings of the \nmodule %0").arg(QString::fromUtf8(ppk_module_display_name(module))));

	//Show the available parameters
	const ppk_proto_param** list=ppk_module_available_parameters(module);
	if(list)
	{
		int i=0;
		while(list[i]!=NULL && list[i]->group!=NULL)
		{
			if(!categories.contains(QString::fromUtf8(list[i]->group->display_name)))
				addCategory(catTab, list[i]->group);

			addParam(catTab, categories[QString::fromUtf8(list[i]->group->display_name)], list[i]);

			i++;
		}
	}
}

void EditParams::changeEvent(QEvent *e)
{
    switch (e->type()) {
    case QEvent::LanguageChange:
	   m_ui->retranslateUi(this);
	   break;
    default:
	   break;
    }
}

void EditParams::buttonBoxClicked(QAbstractButton* bbox)
{
	QDialogButtonBox::StandardButton button=m_ui->buttonBox->standardButton(bbox);

	if(button==QDialogButtonBox::RestoreDefaults)
	{
		for(int i=0;i<fields.size();i++)
			fields[i]->reset();
	}
	else if(button==QDialogButtonBox::Cancel)
		close();
	else if(button==QDialogButtonBox::Save)
	{
		saveParam();
		accept();
	}
}





/*
  Parameter listing functions
  */

QString EditParams::createNameString(const ppk_proto_param* pparam)
{
	QVariant default_value;

	cvariant cvariant_default=ppk_param_proto_default_value(pparam);

	switch(cvariant_get_type(cvariant_default))
	{
		case cvariant_string:
			default_value=QString::fromUtf8(cvariant_get_string(cvariant_default));
			break;

		case cvariant_int:
			default_value=cvariant_get_int(cvariant_default);
			break;

		case cvariant_float:
			default_value=cvariant_get_float(cvariant_default);
			break;

		case cvariant_boolean:
			default_value=(cvariant_get_bool(cvariant_default)==cvariant_true);
			break;

		default:
			return tr("Unknown");
	}

	return QString::fromUtf8("<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n"
"<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\n"
"p, li { white-space: pre-wrap; }\n"
"</style></head><body style=\" font-family:'Helvetica'; font-size:8pt; font-weight:400; font-style:normal;\">\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-weight:600;\">%1</span> (Default: '%2')</p></body></html>").arg(QString::fromUtf8(pparam->name)).arg(default_value.toString());
}

#include "form_fields/qtextfield.h"
#include "form_fields/qfilefield.h"
#include "form_fields/qdirfield.h"
#include "form_fields/qspinfield.h"
#include "form_fields/qdoublespinfield.h"
#include "form_fields/qcombofield.h"
#include "form_fields/qmodulefield.h"
#include "form_fields/qcheckfield.h"

#include <stdio.h>
QAbstractFormField* EditParams::abstractFormFieldFromParamProto(QWidget* parent, const ppk_proto_param* pparam)
{
	//Get Param's value
	cvariant c_value=ppk_module_get_param(module, ppk_param_proto_name(pparam));

	//Create the widget
	switch(ppk_param_proto_expected_type(pparam))
	{
		case cvariant_none:
		{
			break;
		}

		case cvariant_string:
		{
			QString default_value=QString::fromUtf8(cvariant_get_string(ppk_param_proto_default_value(pparam)));
			QString value;
			if(cvariant_not_null(c_value))
				value=QString::fromUtf8(cvariant_get_string(c_value));
			else
				value=default_value;

			QAbstractFormField* lineEdit;
			if(ppk_param_proto_user_type(pparam)==ppk_proto_file_param)
				lineEdit = new QFileField(parent, 1000, QString::fromUtf8(ppk_param_proto_file_params(pparam)->file_filter));
			else if(ppk_param_proto_user_type(pparam)==ppk_proto_directory_param)
				lineEdit = new QDirField(parent, 1000);
			else if(ppk_param_proto_user_type(pparam)==ppk_proto_module_param)
				lineEdit = new QModuleField(parent, QString::fromUtf8(module), ppk_param_proto_module_params(pparam));
			else if(ppk_param_proto_user_type(pparam)==ppk_proto_list_param)
				lineEdit = new QComboField(parent, ppk_param_proto_list_params(pparam)->list);
			else if(ppk_param_proto_user_type(pparam)==ppk_proto_validated_string_param)
				lineEdit = new QTextField(parent, QRegExp(QString::fromUtf8(ppk_param_proto_validated_string_params(pparam)->validation_regexp)));
			else
				lineEdit = new QTextField(parent);
			
			lineEdit->setDefaultValue(default_value);
			lineEdit->setValue(value);
			
			return lineEdit;
		}

		case cvariant_int:
		{
			int default_value=cvariant_get_int(ppk_param_proto_default_value(pparam));
			int value;
			
			if(cvariant_not_null(c_value))
				value=cvariant_get_int(c_value);
			else
				value=default_value;

			QSpinField* spinBox;
			const ppk_proto_param_ranged_int* ranged_int_parameters=ppk_param_proto_ranged_int_params(pparam);
			if(ppk_param_proto_user_type(pparam)==ppk_proto_ranged_int_param)
				spinBox = new QSpinField(parent, ranged_int_parameters->lowest, ranged_int_parameters->greatest);
			else
				spinBox = new QSpinField(parent);
			spinBox->setDefaultValue(default_value);
			spinBox->setValue(value);
			return (QAbstractFormField*)spinBox;
		}

		case cvariant_float:
		{
			double default_value=cvariant_get_float(ppk_param_proto_default_value(pparam));
			double value;
			
			if(cvariant_not_null(c_value))
				value=cvariant_get_float(c_value);
			else
				value=default_value;

			QDoubleSpinField* doubleSpinBox;
			const ppk_proto_param_ranged_float* ranged_float_parameters=ppk_param_proto_ranged_float_params(pparam);
			if(ppk_param_proto_user_type(pparam)==ppk_proto_ranged_int_param)
				doubleSpinBox = new QDoubleSpinField(parent, ranged_float_parameters->lowest, ranged_float_parameters->greatest);
			else
				doubleSpinBox = new QDoubleSpinField(parent);
			doubleSpinBox->setDefaultValue(default_value);
			doubleSpinBox->setValue(value);
			return (QAbstractFormField*)doubleSpinBox;
		}
		
		case cvariant_boolean:
		{
			bool default_value=cvariant_get_bool(ppk_param_proto_default_value(pparam))==cvariant_true;
			bool value;
			
			if(cvariant_not_null(c_value))
				value=cvariant_get_bool(c_value)==cvariant_true;
			else
				value=default_value;
			
			QCheckField* checkbox=new QCheckField(parent, QString::fromUtf8(ppk_param_proto_help_text(pparam)));
			checkbox->setDefaultValue(default_value);
			checkbox->setValue(value);

			return (QAbstractFormField*)checkbox;
		}
	}

	return NULL;
}

void EditParams::addParam(QWidget* parent, QGridLayout* layout, const ppk_proto_param* pparam)
{
	if(!layout)
	{
		qDebug("EditParams::addParam: Tried to add a param to a NULL tab widget !\n");
		return;
	}

	QAbstractFormField* field=abstractFormFieldFromParamProto(parent, pparam);
	if(field)
	{
		field->setFieldName(QString::fromUtf8(pparam->name));
		field->setHelpText(QString::fromUtf8(pparam->help_text));

		int line=layout->rowCount();

		QSizePolicy sizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
		sizePolicy.setHorizontalStretch(0);
		sizePolicy.setVerticalStretch(0);
		field->widget()->setSizePolicy(sizePolicy);
		field->widget()->setToolTip(QString::fromUtf8(pparam->help_text));
		if(field->layout()!=NULL)
		{
			layout->addLayout(field->layout(), line, 1, 1, 1);
		}
		else
		{
			sizePolicy.setHeightForWidth(field->widget()->sizePolicy().hasHeightForWidth());
			layout->addWidget(field->widget(), line, 1, 1, 1);
		}

		QLabel* nameParamLbl = new QLabel(createNameString(pparam), parent);
		nameParamLbl->setToolTip(QString::fromUtf8(pparam->help_text));
		nameParamLbl->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Fixed);
		layout->addWidget(nameParamLbl, line, 0, 1, 1);

		QPushButton* helpParamBtn = new QPushButton(tr("?"), parent);
		helpParamBtn->setObjectName(QString::fromUtf8("helpParamBtn"));
		QSizePolicy sizePolicy1(QSizePolicy::Maximum, QSizePolicy::Fixed);
		sizePolicy1.setHorizontalStretch(0);
		sizePolicy1.setVerticalStretch(0);
		sizePolicy1.setHeightForWidth(helpParamBtn->sizePolicy().hasHeightForWidth());
		helpParamBtn->setSizePolicy(sizePolicy1);
		helpParamBtn->setMaximumSize(QSize(40, 16777215));
		helpParamBtn->setToolTip(QString::fromUtf8(pparam->help_text));
		layout->addWidget(helpParamBtn, line, 2, 1, 1);

		QDialogButtonBox* rstParamBtn = new QDialogButtonBox(parent);
		rstParamBtn->setObjectName(QString::fromUtf8("rstParamBtn"));
		sizePolicy1.setHeightForWidth(rstParamBtn->sizePolicy().hasHeightForWidth());
		rstParamBtn->setSizePolicy(sizePolicy1);
		rstParamBtn->setStandardButtons(QDialogButtonBox::Reset);
		rstParamBtn->setToolTip(tr("Reset to the default value"));
		layout->addWidget(rstParamBtn, line, 3, 1, 1);

		//Add the field to the field vector
		fields.push_back(field);

		connect(helpParamBtn, SIGNAL(clicked()), field, SLOT(showHelpText()));
		connect(rstParamBtn, SIGNAL(clicked(QAbstractButton*)), field, SLOT(reset()));
	}
}

QWidget* EditParams::addCategory(QTabWidget* catTab, const ppk_settings_group* categ)
{
	if(categ)
	{
		if(!categories.contains(QString::fromUtf8(categ->display_name)))
		{
			QWidget* tab = new QWidget(catTab);
			/*int pos=*/catTab->addTab(tab, QString::fromUtf8(categ->display_name));

			categories[QString::fromUtf8(categ->display_name)]=new QGridLayout(tab);

			return tab;
		}
		else
		{
			qDebug("EditParams::addCategory: Trying to add an already existing category(%s) !\n", categ->display_name);
			return NULL;
		}
	}
	else
	{
		qDebug("EditParams::addCategory: categ==NULL !\n");
		return NULL;
	}
}

void EditParams::saveParam()
{
	for(int i=0;i<fields.size();i++)
	{
		QString name=fields[i]->fieldName();
		QVariant fieldValue=fields[i]->value();
		QVariant fieldDefaultValue=fields[i]->defaultValue();

		cvariant new_value=cvariant_null();
		switch(fields[i]->value().type())
		{
			case QVariant::Double:
			{
				new_value=cvariant_from_float(fieldValue.toDouble());
				break;
			}
			case QVariant::Int:
			{
				new_value=cvariant_from_int(fieldValue.toInt());
				break;
			}
			case QVariant::String:
			{
				new_value=cvariant_from_string_copy(fieldValue.toString().toUtf8().data(), fieldValue.toString().size()+1);
				break;
			}
			default:
				continue;
		}

		//Compare the current value to the currently stored
		cvariant cur_value=ppk_module_get_param(module, qPrintable(name));

		//If there is an already existing key, replace it. or if we changed the default value
		if(cvariant_not_null(cur_value) || fieldValue!=fieldDefaultValue)
			ppk_module_save_param(module, qPrintable(name), new_value);
	}
}
