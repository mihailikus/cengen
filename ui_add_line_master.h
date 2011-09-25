/********************************************************************************
** Form generated from reading UI file 'add_line_master.ui'
**
** Created: Thu Aug 11 22:41:13 2011
**      by: Qt User Interface Compiler version 4.7.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_ADD_LINE_MASTER_H
#define UI_ADD_LINE_MASTER_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QDialog>
#include <QtGui/QDialogButtonBox>
#include <QtGui/QGraphicsView>
#include <QtGui/QGridLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QSpinBox>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_Dialog
{
public:
    QDialogButtonBox *buttonBox;
    QLabel *label;
    QWidget *gridLayoutWidget;
    QGridLayout *gridLayout;
    QLabel *label_2;
    QLabel *label_3;
    QLabel *label_4;
    QLabel *label_5;
    QLabel *label_6;
    QSpinBox *startXspin;
    QSpinBox *startYspin;
    QSpinBox *heithSpin;
    QSpinBox *widthSpin;
    QSpinBox *thickSpin;
    QGraphicsView *graphicsView;

    void setupUi(QDialog *Dialog)
    {
        if (Dialog->objectName().isEmpty())
            Dialog->setObjectName(QString::fromUtf8("Dialog"));
        Dialog->resize(527, 300);
        buttonBox = new QDialogButtonBox(Dialog);
        buttonBox->setObjectName(QString::fromUtf8("buttonBox"));
        buttonBox->setGeometry(QRect(30, 240, 341, 32));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);
        label = new QLabel(Dialog);
        label->setObjectName(QString::fromUtf8("label"));
        label->setGeometry(QRect(40, 30, 191, 16));
        gridLayoutWidget = new QWidget(Dialog);
        gridLayoutWidget->setObjectName(QString::fromUtf8("gridLayoutWidget"));
        gridLayoutWidget->setGeometry(QRect(40, 60, 251, 161));
        gridLayout = new QGridLayout(gridLayoutWidget);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        gridLayout->setContentsMargins(0, 0, 0, 0);
        label_2 = new QLabel(gridLayoutWidget);
        label_2->setObjectName(QString::fromUtf8("label_2"));
        label_2->setAlignment(Qt::AlignCenter);

        gridLayout->addWidget(label_2, 0, 0, 1, 1);

        label_3 = new QLabel(gridLayoutWidget);
        label_3->setObjectName(QString::fromUtf8("label_3"));
        label_3->setAlignment(Qt::AlignCenter);

        gridLayout->addWidget(label_3, 1, 0, 1, 1);

        label_4 = new QLabel(gridLayoutWidget);
        label_4->setObjectName(QString::fromUtf8("label_4"));
        label_4->setAlignment(Qt::AlignCenter);

        gridLayout->addWidget(label_4, 0, 2, 1, 1);

        label_5 = new QLabel(gridLayoutWidget);
        label_5->setObjectName(QString::fromUtf8("label_5"));
        label_5->setAlignment(Qt::AlignCenter);

        gridLayout->addWidget(label_5, 1, 2, 1, 1);

        label_6 = new QLabel(gridLayoutWidget);
        label_6->setObjectName(QString::fromUtf8("label_6"));
        label_6->setAlignment(Qt::AlignCenter);

        gridLayout->addWidget(label_6, 2, 0, 1, 2);

        startXspin = new QSpinBox(gridLayoutWidget);
        startXspin->setObjectName(QString::fromUtf8("startXspin"));
        startXspin->setAlignment(Qt::AlignCenter);

        gridLayout->addWidget(startXspin, 0, 1, 1, 1);

        startYspin = new QSpinBox(gridLayoutWidget);
        startYspin->setObjectName(QString::fromUtf8("startYspin"));
        startYspin->setAlignment(Qt::AlignCenter);

        gridLayout->addWidget(startYspin, 1, 1, 1, 1);

        heithSpin = new QSpinBox(gridLayoutWidget);
        heithSpin->setObjectName(QString::fromUtf8("heithSpin"));
        heithSpin->setAlignment(Qt::AlignCenter);

        gridLayout->addWidget(heithSpin, 1, 3, 1, 1);

        widthSpin = new QSpinBox(gridLayoutWidget);
        widthSpin->setObjectName(QString::fromUtf8("widthSpin"));
        widthSpin->setAlignment(Qt::AlignCenter);

        gridLayout->addWidget(widthSpin, 0, 3, 1, 1);

        thickSpin = new QSpinBox(gridLayoutWidget);
        thickSpin->setObjectName(QString::fromUtf8("thickSpin"));
        thickSpin->setAlignment(Qt::AlignCenter);

        gridLayout->addWidget(thickSpin, 2, 2, 1, 1);

        graphicsView = new QGraphicsView(Dialog);
        graphicsView->setObjectName(QString::fromUtf8("graphicsView"));
        graphicsView->setGeometry(QRect(310, 60, 200, 161));

        retranslateUi(Dialog);
        QObject::connect(buttonBox, SIGNAL(accepted()), Dialog, SLOT(accept()));
        QObject::connect(buttonBox, SIGNAL(rejected()), Dialog, SLOT(reject()));

        QMetaObject::connectSlotsByName(Dialog);
    } // setupUi

    void retranslateUi(QDialog *Dialog)
    {
        Dialog->setWindowTitle(QApplication::translate("Dialog", "Dialog", 0, QApplication::UnicodeUTF8));
        label->setText(QApplication::translate("Dialog", "\320\234\320\260\321\201\321\202\320\265\321\200 \320\264\320\276\320\261\320\260\320\262\320\273\320\265\320\275\320\270\321\217 \320\273\320\270\320\275\320\270\320\270", 0, QApplication::UnicodeUTF8));
        label_2->setText(QApplication::translate("Dialog", "startX", 0, QApplication::UnicodeUTF8));
        label_3->setText(QApplication::translate("Dialog", "startY", 0, QApplication::UnicodeUTF8));
        label_4->setText(QApplication::translate("Dialog", "+X", 0, QApplication::UnicodeUTF8));
        label_5->setText(QApplication::translate("Dialog", "+Y", 0, QApplication::UnicodeUTF8));
        label_6->setText(QApplication::translate("Dialog", "\320\242\320\276\320\273\321\211\320\270\320\275\320\260", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class Dialog: public Ui_Dialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_ADD_LINE_MASTER_H
