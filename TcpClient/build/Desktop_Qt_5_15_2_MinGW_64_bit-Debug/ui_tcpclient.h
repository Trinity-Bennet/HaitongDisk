/********************************************************************************
** Form generated from reading UI file 'tcpclient.ui'
**
** Created by: Qt User Interface Compiler version 5.15.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_TCPCLIENT_H
#define UI_TCPCLIENT_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_TcpClient
{
public:
    QWidget *layoutWidget;
    QVBoxLayout *verticalLayout_2;
    QHBoxLayout *horizontalLayout;
    QLabel *usr_name;
    QLineEdit *usr_name_text;
    QHBoxLayout *horizontalLayout_2;
    QLabel *usr_passwd;
    QLineEdit *usr_passwd_text;
    QHBoxLayout *horizontalLayout_3;
    QPushButton *login_btn;
    QPushButton *register_btn;
    QPushButton *logoff_btn;

    void setupUi(QWidget *TcpClient)
    {
        if (TcpClient->objectName().isEmpty())
            TcpClient->setObjectName(QString::fromUtf8("TcpClient"));
        TcpClient->resize(390, 278);
        layoutWidget = new QWidget(TcpClient);
        layoutWidget->setObjectName(QString::fromUtf8("layoutWidget"));
        layoutWidget->setGeometry(QRect(10, 30, 361, 211));
        verticalLayout_2 = new QVBoxLayout(layoutWidget);
        verticalLayout_2->setObjectName(QString::fromUtf8("verticalLayout_2"));
        verticalLayout_2->setContentsMargins(0, 0, 0, 0);
        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        usr_name = new QLabel(layoutWidget);
        usr_name->setObjectName(QString::fromUtf8("usr_name"));
        usr_name->setMinimumSize(QSize(100, 40));
        usr_name->setMaximumSize(QSize(100, 40));
        QFont font;
        font.setFamily(QString::fromUtf8("\346\226\271\346\255\243\345\256\213\345\210\273\346\234\254\347\247\200\346\245\267\347\256\200\344\275\223"));
        font.setPointSize(15);
        usr_name->setFont(font);

        horizontalLayout->addWidget(usr_name);

        usr_name_text = new QLineEdit(layoutWidget);
        usr_name_text->setObjectName(QString::fromUtf8("usr_name_text"));
        usr_name_text->setMinimumSize(QSize(200, 40));
        usr_name_text->setMaximumSize(QSize(200, 40));
        QFont font1;
        font1.setPointSize(15);
        usr_name_text->setFont(font1);

        horizontalLayout->addWidget(usr_name_text);


        verticalLayout_2->addLayout(horizontalLayout);

        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
        usr_passwd = new QLabel(layoutWidget);
        usr_passwd->setObjectName(QString::fromUtf8("usr_passwd"));
        usr_passwd->setMinimumSize(QSize(100, 40));
        usr_passwd->setMaximumSize(QSize(100, 40));
        usr_passwd->setFont(font);

        horizontalLayout_2->addWidget(usr_passwd);

        usr_passwd_text = new QLineEdit(layoutWidget);
        usr_passwd_text->setObjectName(QString::fromUtf8("usr_passwd_text"));
        usr_passwd_text->setMinimumSize(QSize(200, 40));
        usr_passwd_text->setMaximumSize(QSize(200, 40));
        usr_passwd_text->setFont(font1);

        horizontalLayout_2->addWidget(usr_passwd_text);


        verticalLayout_2->addLayout(horizontalLayout_2);

        horizontalLayout_3 = new QHBoxLayout();
        horizontalLayout_3->setObjectName(QString::fromUtf8("horizontalLayout_3"));
        login_btn = new QPushButton(layoutWidget);
        login_btn->setObjectName(QString::fromUtf8("login_btn"));
        login_btn->setMinimumSize(QSize(80, 40));
        login_btn->setMaximumSize(QSize(80, 40));

        horizontalLayout_3->addWidget(login_btn);

        register_btn = new QPushButton(layoutWidget);
        register_btn->setObjectName(QString::fromUtf8("register_btn"));
        register_btn->setMinimumSize(QSize(80, 40));
        register_btn->setMaximumSize(QSize(80, 40));

        horizontalLayout_3->addWidget(register_btn);

        logoff_btn = new QPushButton(layoutWidget);
        logoff_btn->setObjectName(QString::fromUtf8("logoff_btn"));
        logoff_btn->setMinimumSize(QSize(80, 40));
        logoff_btn->setMaximumSize(QSize(80, 40));

        horizontalLayout_3->addWidget(logoff_btn);


        verticalLayout_2->addLayout(horizontalLayout_3);


        retranslateUi(TcpClient);

        QMetaObject::connectSlotsByName(TcpClient);
    } // setupUi

    void retranslateUi(QWidget *TcpClient)
    {
        TcpClient->setWindowTitle(QCoreApplication::translate("TcpClient", "TcpClient", nullptr));
        usr_name->setText(QCoreApplication::translate("TcpClient", "\347\224\250\346\210\267\345\220\215\357\274\232", nullptr));
        usr_passwd->setText(QCoreApplication::translate("TcpClient", " \345\257\206\347\240\201\357\274\232", nullptr));
        login_btn->setText(QCoreApplication::translate("TcpClient", "\347\231\273\345\275\225", nullptr));
        register_btn->setText(QCoreApplication::translate("TcpClient", "\346\263\250\345\206\214", nullptr));
        logoff_btn->setText(QCoreApplication::translate("TcpClient", "\346\263\250\351\224\200", nullptr));
    } // retranslateUi

};

namespace Ui {
    class TcpClient: public Ui_TcpClient {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_TCPCLIENT_H
