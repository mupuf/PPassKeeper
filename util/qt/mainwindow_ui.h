/********************************************************************************
** Form generated from reading ui file 'mainwindow.ui'
**
** Created: Sun Mar 29 14:50:24 2009
**      by: Qt User Interface Compiler version 4.5.0
**
** WARNING! All changes made in this file will be lost when recompiling ui file!
********************************************************************************/

#ifndef MAINWINDOW_UI_H
#define MAINWINDOW_UI_H

#include <QtCore/QLocale>
#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QCheckBox>
#include <QtGui/QComboBox>
#include <QtGui/QDockWidget>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QMainWindow>
#include <QtGui/QMenu>
#include <QtGui/QMenuBar>
#include <QtGui/QPlainTextEdit>
#include <QtGui/QProgressBar>
#include <QtGui/QPushButton>
#include <QtGui/QSpacerItem>
#include <QtGui/QStatusBar>
#include <QtGui/QToolBar>
#include <QtGui/QTreeView>
#include <QtGui/QVBoxLayout>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QAction *action_Quit;
    QAction *action_Add;
    QAction *action_Del;
    QAction *action_Import;
    QAction *action_Export;
    QAction *actionLock_ppasskeeper;
    QWidget *centralwidget;
    QVBoxLayout *verticalLayout;
    QHBoxLayout *horizontalLayout;
    QLabel *label_2;
    QComboBox *modulesBox;
    QCheckBox *checkBox;
    QLineEdit *lineEdit;
    QTreeView *pwdlistView;
    QStatusBar *statusbar;
    QMenuBar *menubar;
    QMenu *menu_File;
    QMenu *menu_Help;
    QMenu *menuActions;
    QToolBar *toolBar;
    QDockWidget *dockWidget;
    QWidget *dockWidgetContents_2;
    QVBoxLayout *verticalLayout_3;
    QVBoxLayout *verticalLayout_2;
    QSpacerItem *verticalSpacer;
    QLabel *infoLabel;
    QSpacerItem *verticalSpacer_2;
    QLabel *label_5;
    QPlainTextEdit *passwordEdit;
    QHBoxLayout *horizontalLayout_3;
    QProgressBar *progressBar;
    QPushButton *showButton;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QString::fromUtf8("MainWindow"));
        MainWindow->resize(731, 621);
        action_Quit = new QAction(MainWindow);
        action_Quit->setObjectName(QString::fromUtf8("action_Quit"));
        action_Add = new QAction(MainWindow);
        action_Add->setObjectName(QString::fromUtf8("action_Add"));
        action_Del = new QAction(MainWindow);
        action_Del->setObjectName(QString::fromUtf8("action_Del"));
        action_Import = new QAction(MainWindow);
        action_Import->setObjectName(QString::fromUtf8("action_Import"));
        action_Export = new QAction(MainWindow);
        action_Export->setObjectName(QString::fromUtf8("action_Export"));
        actionLock_ppasskeeper = new QAction(MainWindow);
        actionLock_ppasskeeper->setObjectName(QString::fromUtf8("actionLock_ppasskeeper"));
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName(QString::fromUtf8("centralwidget"));
        verticalLayout = new QVBoxLayout(centralwidget);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        label_2 = new QLabel(centralwidget);
        label_2->setObjectName(QString::fromUtf8("label_2"));

        horizontalLayout->addWidget(label_2);

        modulesBox = new QComboBox(centralwidget);
        modulesBox->setObjectName(QString::fromUtf8("modulesBox"));
        modulesBox->setSizeAdjustPolicy(QComboBox::AdjustToMinimumContentsLength);
        modulesBox->setMinimumContentsLength(32);
        modulesBox->setFrame(true);

        horizontalLayout->addWidget(modulesBox);

        checkBox = new QCheckBox(centralwidget);
        checkBox->setObjectName(QString::fromUtf8("checkBox"));

        horizontalLayout->addWidget(checkBox);

        lineEdit = new QLineEdit(centralwidget);
        lineEdit->setObjectName(QString::fromUtf8("lineEdit"));

        horizontalLayout->addWidget(lineEdit);


        verticalLayout->addLayout(horizontalLayout);

        pwdlistView = new QTreeView(centralwidget);
        pwdlistView->setObjectName(QString::fromUtf8("pwdlistView"));

        verticalLayout->addWidget(pwdlistView);

        MainWindow->setCentralWidget(centralwidget);
        statusbar = new QStatusBar(MainWindow);
        statusbar->setObjectName(QString::fromUtf8("statusbar"));
        MainWindow->setStatusBar(statusbar);
        menubar = new QMenuBar(MainWindow);
        menubar->setObjectName(QString::fromUtf8("menubar"));
        menubar->setGeometry(QRect(0, 0, 731, 24));
        menu_File = new QMenu(menubar);
        menu_File->setObjectName(QString::fromUtf8("menu_File"));
        menu_Help = new QMenu(menubar);
        menu_Help->setObjectName(QString::fromUtf8("menu_Help"));
        menuActions = new QMenu(menubar);
        menuActions->setObjectName(QString::fromUtf8("menuActions"));
        MainWindow->setMenuBar(menubar);
        toolBar = new QToolBar(MainWindow);
        toolBar->setObjectName(QString::fromUtf8("toolBar"));
        MainWindow->addToolBar(Qt::TopToolBarArea, toolBar);
        dockWidget = new QDockWidget(MainWindow);
        dockWidget->setObjectName(QString::fromUtf8("dockWidget"));
        dockWidget->setFeatures(QDockWidget::NoDockWidgetFeatures);
        dockWidget->setAllowedAreas(Qt::RightDockWidgetArea);
        dockWidgetContents_2 = new QWidget();
        dockWidgetContents_2->setObjectName(QString::fromUtf8("dockWidgetContents_2"));
        verticalLayout_3 = new QVBoxLayout(dockWidgetContents_2);
        verticalLayout_3->setObjectName(QString::fromUtf8("verticalLayout_3"));
        verticalLayout_2 = new QVBoxLayout();
        verticalLayout_2->setObjectName(QString::fromUtf8("verticalLayout_2"));
        verticalSpacer = new QSpacerItem(174, 58, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout_2->addItem(verticalSpacer);

        infoLabel = new QLabel(dockWidgetContents_2);
        infoLabel->setObjectName(QString::fromUtf8("infoLabel"));
        infoLabel->setAlignment(Qt::AlignCenter);

        verticalLayout_2->addWidget(infoLabel);

        verticalSpacer_2 = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout_2->addItem(verticalSpacer_2);

        label_5 = new QLabel(dockWidgetContents_2);
        label_5->setObjectName(QString::fromUtf8("label_5"));
        label_5->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignVCenter);

        verticalLayout_2->addWidget(label_5);

        passwordEdit = new QPlainTextEdit(dockWidgetContents_2);
        passwordEdit->setObjectName(QString::fromUtf8("passwordEdit"));
        passwordEdit->setLocale(QLocale(QLocale::English, QLocale::UnitedKingdom));

        verticalLayout_2->addWidget(passwordEdit);


        verticalLayout_3->addLayout(verticalLayout_2);

        horizontalLayout_3 = new QHBoxLayout();
        horizontalLayout_3->setObjectName(QString::fromUtf8("horizontalLayout_3"));
        progressBar = new QProgressBar(dockWidgetContents_2);
        progressBar->setObjectName(QString::fromUtf8("progressBar"));
        progressBar->setValue(0);
        progressBar->setTextVisible(false);
        progressBar->setOrientation(Qt::Horizontal);
        progressBar->setInvertedAppearance(true);
        progressBar->setTextDirection(QProgressBar::TopToBottom);

        horizontalLayout_3->addWidget(progressBar);

        showButton = new QPushButton(dockWidgetContents_2);
        showButton->setObjectName(QString::fromUtf8("showButton"));
        showButton->setCheckable(true);
        showButton->setFlat(false);

        horizontalLayout_3->addWidget(showButton);


        verticalLayout_3->addLayout(horizontalLayout_3);

        dockWidget->setWidget(dockWidgetContents_2);
        MainWindow->addDockWidget(static_cast<Qt::DockWidgetArea>(2), dockWidget);

        menubar->addAction(menu_File->menuAction());
        menubar->addAction(menuActions->menuAction());
        menubar->addAction(menu_Help->menuAction());
        menu_File->addAction(action_Quit);
        menuActions->addAction(actionLock_ppasskeeper);
        toolBar->addAction(action_Add);
        toolBar->addAction(action_Del);
        toolBar->addSeparator();
        toolBar->addAction(action_Import);
        toolBar->addAction(action_Export);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QApplication::translate("MainWindow", "MainWindow", 0, QApplication::UnicodeUTF8));
        action_Quit->setText(QApplication::translate("MainWindow", "&Quit", 0, QApplication::UnicodeUTF8));
        action_Quit->setShortcut(QApplication::translate("MainWindow", "Ctrl+Q", 0, QApplication::UnicodeUTF8));
        action_Add->setText(QApplication::translate("MainWindow", "&Add", 0, QApplication::UnicodeUTF8));
        action_Del->setText(QApplication::translate("MainWindow", "&Del", 0, QApplication::UnicodeUTF8));
        action_Import->setText(QApplication::translate("MainWindow", "&Import", 0, QApplication::UnicodeUTF8));
        action_Export->setText(QApplication::translate("MainWindow", "&Export", 0, QApplication::UnicodeUTF8));
        actionLock_ppasskeeper->setText(QApplication::translate("MainWindow", "Set a master password", 0, QApplication::UnicodeUTF8));
        label_2->setText(QApplication::translate("MainWindow", "Module:", 0, QApplication::UnicodeUTF8));
        checkBox->setText(QApplication::translate("MainWindow", "Filter by name:", 0, QApplication::UnicodeUTF8));
        menu_File->setTitle(QApplication::translate("MainWindow", "&File", 0, QApplication::UnicodeUTF8));
        menu_Help->setTitle(QApplication::translate("MainWindow", "&Help", 0, QApplication::UnicodeUTF8));
        menuActions->setTitle(QApplication::translate("MainWindow", "Actions", 0, QApplication::UnicodeUTF8));
        toolBar->setWindowTitle(QApplication::translate("MainWindow", "toolBar", 0, QApplication::UnicodeUTF8));
        dockWidget->setWindowTitle(QApplication::translate("MainWindow", "Password information", 0, QApplication::UnicodeUTF8));
        infoLabel->setText(QApplication::translate("MainWindow", "(none selected)", 0, QApplication::UnicodeUTF8));
        label_5->setText(QApplication::translate("MainWindow", "Value:", 0, QApplication::UnicodeUTF8));
        passwordEdit->setPlainText(QApplication::translate("MainWindow", "(hidden)", 0, QApplication::UnicodeUTF8));
        showButton->setText(QApplication::translate("MainWindow", "Show value", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // MAINWINDOW_UI_H
