/********************************************************************************
** Form generated from reading UI file 'privatechat.ui'
**
** Created by: Qt User Interface Compiler version 5.15.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_PRIVATECHAT_H
#define UI_PRIVATECHAT_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_PrivateChat
{
public:
    QWidget *layoutWidget;
    QVBoxLayout *verticalLayout;
    QTextEdit *privateChatMsg;
    QHBoxLayout *horizontalLayout;
    QLineEdit *privateChatText;
    QPushButton *privateChatSendBtn;

    void setupUi(QWidget *PrivateChat)
    {
        if (PrivateChat->objectName().isEmpty())
            PrivateChat->setObjectName(QString::fromUtf8("PrivateChat"));
        PrivateChat->resize(488, 340);
        layoutWidget = new QWidget(PrivateChat);
        layoutWidget->setObjectName(QString::fromUtf8("layoutWidget"));
        layoutWidget->setGeometry(QRect(20, 10, 451, 321));
        verticalLayout = new QVBoxLayout(layoutWidget);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        verticalLayout->setContentsMargins(0, 0, 0, 0);
        privateChatMsg = new QTextEdit(layoutWidget);
        privateChatMsg->setObjectName(QString::fromUtf8("privateChatMsg"));
        QFont font;
        font.setFamily(QString::fromUtf8("\345\276\256\350\275\257\351\233\205\351\273\221"));
        font.setPointSize(15);
        privateChatMsg->setFont(font);

        verticalLayout->addWidget(privateChatMsg);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        privateChatText = new QLineEdit(layoutWidget);
        privateChatText->setObjectName(QString::fromUtf8("privateChatText"));
        QFont font1;
        font1.setPointSize(15);
        privateChatText->setFont(font1);

        horizontalLayout->addWidget(privateChatText);

        privateChatSendBtn = new QPushButton(layoutWidget);
        privateChatSendBtn->setObjectName(QString::fromUtf8("privateChatSendBtn"));
        QFont font2;
        font2.setFamily(QString::fromUtf8("\346\226\271\346\255\243\345\256\213\345\210\273\346\234\254\347\247\200\346\245\267\347\256\200\344\275\223"));
        font2.setPointSize(15);
        privateChatSendBtn->setFont(font2);

        horizontalLayout->addWidget(privateChatSendBtn);


        verticalLayout->addLayout(horizontalLayout);


        retranslateUi(PrivateChat);

        QMetaObject::connectSlotsByName(PrivateChat);
    } // setupUi

    void retranslateUi(QWidget *PrivateChat)
    {
        PrivateChat->setWindowTitle(QCoreApplication::translate("PrivateChat", "Form", nullptr));
        privateChatSendBtn->setText(QCoreApplication::translate("PrivateChat", "\345\217\221\351\200\201", nullptr));
    } // retranslateUi

};

namespace Ui {
    class PrivateChat: public Ui_PrivateChat {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_PRIVATECHAT_H
