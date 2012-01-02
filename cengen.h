#ifndef CENGEN_H
#define CENGEN_H

#include <QMainWindow>
#include <QPrinter>
#include <QPrintDialog>
#include <QTableWidget>
#include <QTabWidget>
#include <QScrollArea>
#include <QVBoxLayout>
#include <QFile>
#include <QRectF>
#include <QStringList>
#include <QComboBox>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QTabWidget>
#include <QSettings>
#include <QGroupBox>
#include <QSpinBox>
#include <QToolBar>
#include <QAction>
#include "tinformer.h"
#include "barcode.h"
#include "cen_viewer.h"
#include "bvalidator.h"
#include "sql_validator.h"
#include "dialog.h"
#include "editor.h"

QT_BEGIN_NAMESPACE
class QLineEdit;
class QPushButton;
class QRadioButton;
class QTableWidget;
class QLabel;
class QFile;
class QScrollArea;
class QVBoxLayout;
class QSpinBox;
class QPrinter;
class QComboBox;
class QGraphicsScene;
class QTabWidget;
class QValidator;
class QStringList;
class QToolBar;

class BarcodeValidator;
class Tinformer;
class Barcode;
class Cennic;

namespace Ui {
    class cengen;
}

class cengen : public QMainWindow {
    Q_OBJECT
public:
    cengen(QWidget *parent = 0);
    ~cengen();
    void writeSettings();
    void readSettings();
   // void translate();

protected:
    void changeEvent(QEvent *e);

private:
    Ui::cengen *ui;
    QWidget *tab1;
    QGridLayout *layTab1,
                *layBox1, *layBox5, *layBox6;

    QSettings m_settings;
    QList<int> mainTableTabs;
    int mainTableWidth, mainTableHeith;

    editor* shablon_editor;

    //поля для размера шаблона:
    QLineEdit* ui_lineEdit, *ui_lineH, *ui_lineW, *ui_lineH_2, *ui_lineW_2;

    //валидаторы для штрих-кода, номера и текста
    BarcodeValidator* ui_bvalidator;
    QIntValidator* ui_nvalidator;
    SqlValidator* ui_svalidator;


    //поля для параметров MySQL-базы данных
    QLineEdit *ui_lineHost, *ui_lineName, *ui_lineUser, *ui_linePort, *ui_linePass;

    //а эти для определения, где в таблице какое поле
    QComboBox *ui_comboTnomer, *ui_comboTname, *ui_comboTbarcode, *ui_comboTprice;

    //контролы SQL и DBF
    QGroupBox *ui_groupDBF, *ui_groupSQL;

    QRadioButton  *ui_radioButton_1,
                  *ui_radioButton_2,
                  *ui_radioButton_3, *ui_radioButton, *ui_radioButton_4,
                    *ui_radioButton_5;
    QGroupBox *ui_groupBox, *ui_groupBox_5, *ui_groupBox_6;
    QTableWidget* ui_tableWidget;
    int sizeDeltaX;  //разница в ширине между основным окном и таблицей
    int sizeDeltaY;
    QLabel* ui_label, *ui_statusLabel, *ui_countLabel, *ui_labelDBFname;
    QScrollArea* ui_scrollArea;
    QTabWidget* ui_tabWidget;
    QVBoxLayout* previewLayout;
    QFile file;
    QDomDocument domDoc;
    QDomElement shablonElement;
    //QPrinter *printer;
    QComboBox* ui_comboBox, *ui_comboTbList;
    QGraphicsView* view;    //используется для отображения превью
    QPushButton *ui_zoomInButton, *ui_zoomOutButton, *ui_save_db_config_button,
                *ui_pushButton, *ui_maxButton;

    QString method; //метод поиска товара - номер, штрихкод или название
    int db_source;  //источник данных: MySQL(1) или DBF(0)


    Tinformer* my_informer;
    dbTranslator config, filterConfig;    //что есть что в базе данных
    int config_index[4];    //индекс для комбобоксов
    SqlConfig* sql;
    DbfConfig* dbf;
    Barcode* my_barcode;
    bool file_is_ready;
    bool db_is_ready;
    bool describer_is_ready;

    bool editing_price2;

    //переменные для работы фильтра
    QGroupBox* ui_filterBox;
    bool filter_is_on;
    DbfConfig filterDbf;
    Tinformer* filterInformer;
    QComboBox* ui_filterWhatBox, *ui_filterWhereBox, *ui_filterWhatToFoundBox,
                *ui_filterMethodBox;
    QLineEdit* ui_filterLineText;

    QString pageSize;   //формат бумаги - А4, А5
    QString paperOrientation;   // книжная или альбомная
    int Crows, Ccols;   //количество строк и столбцов в шаблоне ценника
    float pageW, pageH; //ширина и высота бумаги выбранного формата (в мкм)
    float zoom;         //масштаб предпросмотра
    QRectF rectCen;     //размер одного ценника из шаблона
    QList<QRectF> pages;    //области для постраничной печати
    QGraphicsScene* currentScene;
    QStringList opisateli;

    //QGraphicsLineItem* point;

    QSpinBox* ui_spinLimit;

    bool add_flag;

    QToolBar* ui_mainToolBar;
    QAction* ui_action3; //новый файл
    QAction* ui_action4; //выход
    QAction* ui_action; //сохранить
    QAction* ui_action2; //загрузить список
    QAction* ui_actionMake; //сформировать ценники

private slots:
    //void on_tabWidget_selected(QString );
    void on_tableWidget_itemClicked(QTableWidgetItem* item);
    void on_tableWidget_cellClicked(int row, int column);
    void on_radioButton_4_clicked();
    void on_radioButton_clicked();
    void on_radioButton_3_clicked();
    void on_radioButton_2_clicked();
    void on_radioButton_1_clicked();
    void on_radioButton_5_clicked();
    void on_save_db_config_button_clicked();
    void on_comboTbList_activated(QString );
    void on_connectMySQLButton_clicked();
    void on_printButton_clicked();
    void on_zoomOutButton_clicked();
    void on_zoomInButton_clicked();
    void on_comboBox_activated(QString );
    void on_pushButton_4_clicked();
    void on_tabWidget_currentChanged(int index);
    void on_pushButton_3_clicked();
    void on_pushButton_2_clicked();
    void on_lineEdit_returnPressed();
    void on_pushButton_clicked();
    void tovar_search();
    void new_line_ready();
    void update_values();   //обновить значеня шаблона в UI-форме
    void read_file_shablon();    //прочитать шаблон
    void describe_shablon(QDomDocument shablon);
    void generate_preview();
    void switch_zoom_buttons_enabled(bool state);
    void update_ui_tb_fields(QStringList list);
    void update_ui_db_controls();
    void update_ui_connection_established();
    void delete_line_from_table(int pos);
    QList<Tovar> show_found_items(QList<Tovar> inputList, QString method);
    QList<Tovar> get_tovar_list(QTableWidget* table, QString priznak);
    QList<Tovar> apply_filter(QList<Tovar> inputList);
    QRectF get_shablon_rect(const QDomNode& node);
    QString get_shablon_name(const QDomNode& node);
    bool read_opisateli_from_settings();
    void set_opisateli_from_settings();

    void set_tableWidget_header(QTableWidget* table);
    void add_table_item(QTableWidget* table, int position, Tovar tovar);

    void on_pushButton_5_clicked();
    void on_spinLimit_valueChanged(int );
    void on_maxButton_clicked();
    //void on_tableWidget_cellEntered(int row, int column);
    void on_tableWidget_cellChanged(int row, int column);
    void on_tableWidget_itemEntered(QTableWidgetItem* item);
    void update_mainTableTabs(QTableWidget * ui_tableWidget);


    void on_show_editor_button_clicked();
    void on_action_activated();
    void on_action_2_triggered();
    void on_action_3_activated();

    void load_tovar_list_into_cengen(QList<Tovar> tovarList);
    QList<Tovar> convert_xml_into_tovar_list(QDomDocument doc);
    QDomDocument convert_tovar_list_into_xml(QList<Tovar> tovarList);
    QList<Tovar> minus(QList<Tovar> oldList, QList<Tovar> newList);


    void on_action_8_activated();
    void on_filterBox_toggled(bool arg1);
    void on_filterFileSelectButton_clicked();
    void on_filterFileName_changed();
    void read_filter_settings();

    void on_radioButton_6_clicked();
    void on_radioButton_7_clicked();

    void on_action_4_activated();
    void on_comboTnomer_currentIndexChanged(int index);
    void on_comboTname_currentIndexChanged(int index);
    void on_comboTbarcode_currentIndexChanged(int index);
    void on_comboTprice_currentIndexChanged(int index);
};

#endif // CENGEN_H
