#ifndef EDITOR_H
#define EDITOR_H

#include <QDialog>
//#include <QMainWindow>
#include <QTextCodec>
#include <QFileInfo>
//#include <QMenuBar>
//#include <QMenu>
#include <QToolBar>
//#include <QStatusBar>
#include <QLabel>
#include <QAction>
#include <QtXml>
#include <QDomDocument>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QGraphicsRectItem>
#include <QFont>
#include <QGridLayout>
#include <QWidget>
#include <QTextStream>
#include <QTextCodec>

#include <QDebug>

#include "models/barcodeBox.h"
#include "models/textBox.h"
#include "models/good.h"
#include "models/image.h"
#include "models/line.h"
#include "cen_viewer.h"

class editor : public QDialog
{
    Q_OBJECT
    
public:
    explicit editor(QWidget *parent = 0, Qt::WFlags f = Qt::WindowSystemMenuHint);
    ~editor();

    bool set_file_name(QString name);
    void load_xml_data_into_editor(QDomElement* domElement);
    QDomDocument get_new_shablon();
    QString get_new_fileName();

private:
    //QMainWindow *wnd_;

    ///имя файла шаблона
    QString fileName;
    ///признак, изменялось ли имя файла шаблона
    bool fileNameChanged;

    //QMenuBar *mMenu;
    //QMenu    *fileMenu;
    QToolBar *mainToolBar;
    //QStatusBar *mainStatusBar;
    QLabel *statusFileNameLabel;
    QGraphicsScene *scene, *pre_scene;
    QGraphicsView *view, *pre_view;
    QGridLayout *layout;

    QGraphicsRectItem *baseRect;
    QSpinBox *lineWidth, *lineHeith;
    QLineEdit *labelName;
    QToolBar *secToolBar;
    QToolBar *brToolBar, *txToolBar, *gToolBar, *iToolBar, *lToolBar;
    bool brIs, txIs, gIs, iIs, lIs; //указатели, отображен ли тулбар

    QAction *action_new;
    QAction *action_save;
    QAction *action_open;

    QAction *action_del;
    QAction *action_zoom_in;
    QAction *action_zoom_out;

    QAction *buttonRight, *buttonLeft;
    QAction *buttonTop,   *buttonButtom;
    QAction *buttonCenter;
    bool onXchanged, onYchanged;

    QAction *action_add_barcode;
    QAction *action_add_barcodeText;
    QAction *action_add_nomer;
    QAction *action_add_good;
    QAction *action_add_date;
    QAction *action_add_price;
    QAction *action_add_priceOld;
    QAction *action_add_image;
    QAction *action_add_line;
    QAction *action_add_textInBox;

    QDomDocument doc;
    Cennic cen;

    bool isSaved;
    bool isLoaded;

    QTextCodec *codec_utf8;


private slots:
    void makeToolBars();
    //void makeMenu();
    void makeActions();
    //void makeStatusBar();

    void generate_preview();
    void create_font_xml_element(QDomElement elem, AbstractItem *itm);

    void on_action_new();
    void on_action_save();
    void on_action_open();
    void on_action_del();

    void on_action_add_barcode();
    void on_action_add_barcodeText();
    void on_action_add_nomer();
    void on_action_add_good();
    void on_action_add_date();
    void on_action_add_price();
    void on_action_add_priceOld();
    void on_action_add_image();
    void on_action_add_line();
    void on_action_add_textInBox();

    void on_zoom_in();
    void on_zoom_out();

    void on_button_right();
    void on_button_left();
    void on_button_top();
    void on_button_buttom();
    void on_button_center();


    void on_baseSize_changed();

    void new_shablon(int w, int h);
    void add_barcode(float startX, float startY, float w, float h,
                     int font_size, QString font_family, bool Bold, bool Italic,
                     int brAddition, int brOtstup,
                     QString text);
    void add_text(QString method, float startX, float strartY, float w, float h,
                  int font_size, QString font_family, bool Bold, bool Italic,
                  QString text);
    void add_good(float startX, float startY, float w, float h,
                  int font_size, QString font_family, bool Bold, bool Italic,
                  QString text);
    void add_image(float startX, float startY, float w, float h,
                   QString fileName);
    void add_line(float startX, float startY, float w, float h,
                  float lineThick);

    void on_scene_selected();

};

#endif // EDITOR_H
