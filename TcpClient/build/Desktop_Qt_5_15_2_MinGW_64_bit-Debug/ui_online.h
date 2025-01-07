/********************************************************************************
** Form generated from reading UI file 'online.ui'
**
** Created by: Qt User Interface Compiler version 5.15.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_ONLINE_H
#define UI_ONLINE_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QListWidget>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_online
{
public:
    QWidget *layoutWidget;
    QHBoxLayout *horizontalLayout;
    QListWidget *listWidget;
    QPushButton *pushButton;

    void setupUi(QWidget *online)
    {
        if (online->objectName().isEmpty())
            online->setObjectName(QString::fromUtf8("online"));
        online->resize(500, 280);
        online->setMinimumSize(QSize(500, 280));
        online->setMaximumSize(QSize(500, 280));
        layoutWidget = new QWidget(online);
        layoutWidget->setObjectName(QString::fromUtf8("layoutWidget"));
        layoutWidget->setGeometry(QRect(20, 20, 471, 241));
        horizontalLayout = new QHBoxLayout(layoutWidget);
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        horizontalLayout->setContentsMargins(0, 0, 0, 0);
        listWidget = new QListWidget(layoutWidget);
        new QListWidgetItem(listWidget);
        new QListWidgetItem(listWidget);
        new QListWidgetItem(listWidget);
        new QListWidgetItem(listWidget);
        new QListWidgetItem(listWidget);
        new QListWidgetItem(listWidget);
        listWidget->setObjectName(QString::fromUtf8("listWidget"));

        horizontalLayout->addWidget(listWidget);

        pushButton = new QPushButton(layoutWidget);
        pushButton->setObjectName(QString::fromUtf8("pushButton"));
        QFont font;
        font.setPointSize(15);
        pushButton->setFont(font);

        horizontalLayout->addWidget(pushButton);


        retranslateUi(online);

        QMetaObject::connectSlotsByName(online);
    } // setupUi

    void retranslateUi(QWidget *online)
    {
        online->setWindowTitle(QCoreApplication::translate("online", "Form", nullptr));

        const bool __sortingEnabled = listWidget->isSortingEnabled();
        listWidget->setSortingEnabled(false);
        QListWidgetItem *___qlistwidgetitem = listWidget->item(0);
        ___qlistwidgetitem->setText(QCoreApplication::translate("online", "111", nullptr));
        QListWidgetItem *___qlistwidgetitem1 = listWidget->item(1);
        ___qlistwidgetitem1->setText(QCoreApplication::translate("online", "222", nullptr));
        QListWidgetItem *___qlistwidgetitem2 = listWidget->item(2);
        ___qlistwidgetitem2->setText(QCoreApplication::translate("online", "333", nullptr));
        QListWidgetItem *___qlistwidgetitem3 = listWidget->item(3);
        ___qlistwidgetitem3->setText(QCoreApplication::translate("online", "444", nullptr));
        QListWidgetItem *___qlistwidgetitem4 = listWidget->item(4);
        ___qlistwidgetitem4->setText(QCoreApplication::translate("online", "555", nullptr));
        QListWidgetItem *___qlistwidgetitem5 = listWidget->item(5);
        ___qlistwidgetitem5->setText(QCoreApplication::translate("online", "666", nullptr));
        listWidget->setSortingEnabled(__sortingEnabled);

        pushButton->setText(QCoreApplication::translate("online", "\346\267\273\345\212\240\345\245\275\345\217\213", nullptr));
    } // retranslateUi

};

namespace Ui {
    class online: public Ui_online {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_ONLINE_H
