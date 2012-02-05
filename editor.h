#ifndef EDITOR_H
#define EDITOR_H

//EDITOR of templates for price lists in XML format

#include <QMainWindow>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QGraphicsRectItem>
#include <QGraphicsTextItem>
#include <QGraphicsItem>
#include <QGroupBox>
#include <QSpinBox>
#include <QPushButton>
#include <QLabel>
#include <QLineEdit>
#include <QGridLayout>
#include <QtXml>
#include <QFontDialog>
#include <QFileDialog>
#include <QTextCodec>

#include "addlinemaster.h"

#include "cen_viewer.h"


#include <QDebug>

QT_BEGIN_NAMESPACE
class QGraphicsView;
class QGraphicsScene;
class QSpinBox;
class QPushButton;
QT_END_NAMESPACE

class editor : public QDialog
{
    Q_OBJECT

public:
    explicit editor(QWidget *parent = 0, Qt::WFlags f = Qt::WindowSystemMenuHint);
//    About(QWidget *parent = 0, Qt::WFlags f = Qt::WindowSystemMenuHint);

    ~editor();

    void load_xml_data_into_editor(QDomElement* domElement);
    void set_file_name(QString name);
    QDomDocument get_new_shablon();
    QString get_new_fileName();


private slots:
    void on_addRectButton_clicked();

    void on_exitButton_clicked();

    void on_clearButton_clicked();

    void on_saveButton_clicked();

    void generate_preview();

    void on_widthSpin_valueChanged(int );

    void on_heithSpin_valueChanged(int );

    void on_nameButton_clicked();

    void on_scene_selected();

    void on_propert_spin_changed();

    void c_items_initialize();

    void on_priceButton_clicked();

    void switch_cennic_element(QGraphicsItem* item);   //add or remove a cennic element

    void on_barcodeButton_clicked();

    void on_oldpriceButton_clicked();

    void on_dateButton_clicked();

    void on_nomerButton_clicked();

    void on_fontSelectButton_clicked();

    void on_textEdit_textChanged(QString );

    void on_delButton_clicked();

    void add_element_to_scene(QString type,
                              float startX, float startY,
                              float width,  float heith,
                              QBrush brush = QBrush(Qt::green),
                              QFont font = QFont("Arial", 10),
                              QString text = "some text",
                              bool to_removal = true);
    void add_element_to_scene(QString type,
                              float startX, float startY,
                              float width, float heith,
                              float linethick);


    void on_loadButton_clicked();

    void set_spin_value(QSpinBox* widget, float value);


    void on_setLeftButton_clicked();

    void on_setTopButton_clicked();

    void on_setRightButton_clicked();

    void on_setButtomButton_clicked();

    void on_barTextButton_clicked();

    void on_zoomOutButton_clicked();

    void on_zoomInButton_clicked();

    void on_lineButton_clicked();

    void delete_item(QGraphicsItem* item);


private:
    //элементы формы
    QWidget *mainWidget;
    QGridLayout *mainLayout;
    QLabel *label5;
    QLineEdit* ui_shalon_nameEdit;
    QLabel *label, *label2;
    QSpinBox* ui_widthSpin, *ui_heithSpin;
    QGraphicsScene* ui_scene;
    QGraphicsView*  ui_view;

    QPushButton *loadButton, *saveButton, *exitButton, *clearButton;

    QPushButton *barcodeButton, *barTextButton,  *nomerButton,
                *priceButton,   *oldpriceButton, *lineButton,
                *dateButton,    *nameButton,     *addRectButton;

    QGraphicsScene *ui_preScene;
    QGraphicsView  *ui_preView;

    //элементы бокса выбора свойств элемента
    QGroupBox *ui_propertBox;
    QGridLayout *propertLay;
    QLabel *label3, *label4;
    QPushButton *fontSelectbutton;
    QPushButton *delButton;
    QSpinBox *ui_fontSizeSpin;
    QSpinBox* ui_pwidthSpin, *ui_pheithSpin;
    QLineEdit* ui_textEdit;

    //элементы бокса позиционирования элемента
    QGroupBox *posBox;
    QGridLayout *posLay;
    QPushButton *setTopButton,  *setButtomButton,
                *setLeftButton, *setRightButton,
                *setCenterButton;

    //элементы группы управления масштабом поля редактора
    QGroupBox *zoomBox;
    QGridLayout *zoomLay;
    QPushButton *zoomInButton, *zoomOutButton;





    QDomDocument doc;
    QString fileName;
    QTextCodec *codec_utf8;

    int line_number;

    QGraphicsRectItem* baseRect;


    QMap<QString, QGraphicsItem* > c_items;
    struct cennicTextItmes {
        QString text;
        QFont font;
    };
    QMap<QString, cennicTextItmes> c_text_items;

    bool isSaved;


//signals:
//    void shablon_is_ready(QDomDocument );

};

#endif // EDITOR_H
