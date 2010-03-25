#include "modulecreation.h"

#include <QDialogButtonBox>
#include <QPushButton>

#include <math.h>

int ModuleCreation::secLevel(const QString& pwd, QString& tips)
{
	int score=0;
	
	bool containsLetter=false, containsInteger=false;
	bool containsSpecialChars=false, containsUpperCase=false;
	bool containsLowerCase=false;
	
	for(int i=0; i<pwd.size(); i++)
	{
		QChar c(pwd[i]);
		
		//Check the kind of characters
		if(c.isDigit())
			containsInteger=true;
		else if(c.isLetter())
		{
			containsLetter=true;
			
			if(c.isLower())
				containsLowerCase=true;
			else
				containsUpperCase=true;
		}
		else
			containsSpecialChars=true;
	}
	
	//Get the number of possibilities for a single character
	int numberPossibilities=0;
	if(containsInteger)
		numberPossibilities+=10;
	if(containsLetter)
	{
		if(containsUpperCase && containsLowerCase)
			numberPossibilities+=52;
		else
			numberPossibilities+=26;
	}
	if(containsSpecialChars)
		numberPossibilities+=65;
	
	//Get the number of possibilities
	double poss_count=pow(numberPossibilities, pwd.size());
	
	//Set this as the maxium (20 ascii characters)
	double max_poss=pow(127, 20);
	
	//get the score
	score=log(poss_count)*100/log(max_poss);
	
	//get the tips
	if(pwd.size()<8)
		tips=QString::fromUtf8("You password is too short. It should be at least 8 characters long.");
	
	//numberPossibilities=10
	else if(!containsLetter && containsInteger && !containsSpecialChars)
		tips=QString::fromUtf8("Digit-based passwords are not secure at all. Please at least add some letters.");
	
	//numberPossibilities=26
	else if(containsLetter && !containsInteger && !containsSpecialChars && !(containsUpperCase && containsLowerCase))
		tips=QString::fromUtf8("Letter-based passwords are not really secure. Please at least add some digits.");
	
	//numberPossibilities=36
	else if(containsLetter && containsInteger && !containsSpecialChars && !(containsUpperCase && containsLowerCase))
		tips=QString::fromUtf8("Letter-based passwords are not really secure. Please at least add lower and uppercase letter characters");
	
	//numberPossibilities=52
	else if(containsLetter && !containsInteger && !containsSpecialChars && containsUpperCase && containsLowerCase)
		tips=QString::fromUtf8("Letter-based passwords are not really secure. Please at least add some digits.");
	
	//numberPossibilities=62
	else if(containsLetter && containsInteger && !containsSpecialChars && containsUpperCase && containsLowerCase)
		tips=QString::fromUtf8("Letter-based passwords are not really secure. Please at least add some digits.");
	
	//numberPossibilities=65
	else if(!containsLetter && !containsInteger && containsSpecialChars)
		tips=QString::fromUtf8("SpecialChars-based passwords are secure. But you can add letters and digits.");
	
	//numberPossibilities=75
	else if(!containsLetter && containsInteger && containsSpecialChars)
		tips=QString::fromUtf8("SpecialChars and digit-based passwords are secure. But you can still add letters.");
	
	//numberPossibilities=91
	else if(containsLetter && !containsInteger && containsSpecialChars)
		tips=QString::fromUtf8("SpecialChars and letter-based passwords are secure. But you can still add digits.");
	
	//numberPossibilities=101
	else if(containsLetter && containsInteger && containsSpecialChars && !(containsUpperCase && containsLowerCase))
		tips=QString::fromUtf8("You can strengthen your password by using lower and uppercase letter characters");
	
	//numberPossibilities=127
	else if(containsLetter && containsInteger && containsSpecialChars && containsUpperCase && containsLowerCase)
		tips=QString::fromUtf8("Great security! You can strengthen your password by using a longer password");
	
	//If I forgot anything
	else
		tips=QString::fromUtf8("Nothing to say.");
	
	return score;
}

ModuleCreation::ModuleCreation()
{
	setupUi(this);
	
	onPassphraseUpdated();

	connect(passphraseEdit, SIGNAL(textChanged(QString)), this, SLOT(onPassphraseUpdated()));
	connect(passphraseEdit2, SIGNAL(textChanged(QString)), this, SLOT(onPassphraseUpdated()));
}

QString ModuleCreation::passphrase()
{
	return passphraseEdit->text();
}

void ModuleCreation::onPassphraseUpdated()
{
	//Enable/disable the OK button
	bool pwdMatch=passphraseEdit->text().size()>0 && passphraseEdit->text()==passphraseEdit2->text();
	buttonBox->button(QDialogButtonBox::Ok)->setEnabled(pwdMatch);
	
	//Check the current security
	QString tips;
	int score=secLevel(passphraseEdit->text(), tips);
	tips_lbl->setText(QString::fromUtf8("Tips: %1").arg(tips));
	securityProgressBar->setValue(score);
}
