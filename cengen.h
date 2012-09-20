#ifndef CENGEN_H
#define CENGEN_H

#include <QMainWindow>
#include <QPrinter>
#include <QPrintDialog>
#include <QTabWidget>
#include <QScrollArea>
#include <QVBoxLayout>
#include <QFile>
#include <QRectF>
#include <QStringList>
#include <QComboBox>
#include <QCheckBox>
#include <QProgressBar>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QSettings>
#include <QGroupBox>
#include <QSpinBox>
#include <QRadioButton>
#include <QLineEdit>
#include <QTextEdit>
#include <QCalendarWidget>
#include <QToolBar>
#include <QStatusBar>
#include <QMenuBar>
#include <QAction>
#include <QMap>
#include <QApplication>
#include <QClipboard>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include "tinformer.h"
#include "barcode.h"
#include "cen_viewer.h"
#include "bvalidator.h"
#include "sql_validator.h"
#include "tnvalidator.h"
#include "dialog.h"
#include "editor.h"
#include "about.h"
#include "mainTableWidget.h"
#include "autozakaz.h"
#include "sellFilterWidget.h"
#include "SyntaxHighlighter.h"

QT_BEGIN_NAMESPACE
class QPushButton;
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
//class SellFilterWidget;
QT_END_NAMESPACE

//namespace Ui {
//    class cengen;
//}

class cengen : public QMainWindow, public ImageBox
{
    Q_OBJECT
public:
    cengen(QWidget *parent = 0);
    ~cengen();
    void writeSettings();
    void readSettings();
    void make_actions();
    void make_toolBar();
    void make_mainMenu();
    void make_search_tab();
    void make_shablon_tab();
    void make_preview_tab();
    void make_source_tab();
    void make_filter_tab();
    void make_fieldList_tab();
    void make_sellSettings_tab();
    void make_ext_app_tab();
    void make_status_bar();
    void set_org_name(QString org, QString prog);
    void trigger_source_selection(bool state);
    bool load_source_settings_file(QString fileName);
    void load_filter_settings_file(QString fileName);
    void save_source_settings_file(QString fileName);
    void turn_filter_ON();
    void open_tovar_list(QString fileName);
    void load_all_records();
    void set_ext_shablon_name(QString fileName);
    void execute_macro_file(QString fileName);
    void edit_macro_file(QString fileName);

    //порядок вкладок
    struct TabsOrder
    {
        static int const Search = 0;
        static int const Shablon = 1;
        static int const Preview = 2;
        static int const Source = 3;
        static int const fList = 4;
        static int const Filter = 5;
        static int const Sell = 6;
        static int const App = 7;
    };

protected:
    void changeEvent(QEvent *e);

private:
    QString org_name, app_name;

    TabsOrder order;

    //статусбар
    QStatusBar* statusBar;
    QLabel *ui_statusLabel;
    QProgressBar *progressBar;

    //виджеты вкладки поиска
    QWidget *tab1;
    QGridLayout *layTab1,
                *layBox1, *layBox5, *layBox6;
    QLineEdit* ui_lineEdit;
    MainTableWidget *tableWidget;


    //виджеты вкладки предварительного просмотра
    QWidget *tab3;
    QGridLayout *layTab3;
    QGraphicsScene* currentScene;
    QGraphicsView* view;    //используется для отображения превью


    //виджеты вкладки выбора шаблона и параметров страницы
    QWidget *tab2;
    QBoxLayout *layTab2;
    QGroupBox *ui_groupBox_2;
    QGroupBox *ui_groupBox_3;
    QBoxLayout *layBoxT2B2;
    QPushButton *ui_selecctShablonButton;
    QLabel *ui_label;
    QPushButton *ui_show_editor_button;
    QLabel *label24;
    QComboBox *shablonBox;
    bool previewed;


    QGridLayout *layBoxT2B3;
    QComboBox *comboBox;
    QLineEdit *ui_lineH, *ui_lineW, *ui_lineH_2, *ui_lineW_2;
    QLabel *label2, *label3, *label4, *label5, *labelOrient, *labelZero;
    QLabel *totalCennicOnPage;
    QRadioButton *ui_radioButton_6;
    QRadioButton *ui_radioButton_7;

    QLabel *zoomLabel;
    QSpinBox *zoomBox;

    QLabel *otstupLabel1;
    QLineEdit *otstupTopLine;

    //QLabel *label32;
    QCheckBox *expandBox;


    //виджеты вкладки источника данных
    QWidget *tab4;
    QGroupBox *ui_groupDBF, *ui_groupSQL, *ui_groupOpisateli;
    QGridLayout *layTab4;
    QGridLayout *laySQL, *layDBF, *layOpisateli;
    //поля для параметров MySQL-базы данных
    QLineEdit *ui_lineHost, *ui_lineName, *ui_lineUser, *ui_linePort, *ui_linePass;
    QLabel *label6, *label10, *label9, *label7, *label8, *label11;
    QPushButton *ui_connectMySQLButton;
    QComboBox *ui_mysql_table_selector_comboBox;

    QLabel *label16, *labelDBFname;
    QPushButton *ui_selectSourceDBF_file_button;

    //а эти для определения, где в таблице какое поле
    QComboBox *ui_comboTnomer, *ui_comboTname, *ui_comboTbarcode, *ui_comboTprice;
    QLabel *label12, *label13, *label14, *label15;
    QPushButton *saveSourceButton, *loadSourceButton;


    //виджеты вкладки фильтра
    QWidget *tab5;
    QBoxLayout *layTab5;
    QGroupBox* ui_filterBox;
    QGridLayout *laytab5g1;
    QPushButton *ui_filterFileSelectButton;
    QComboBox   *ui_filterWhatBox, *ui_filterWhereBox,
                *ui_filterWhatToFoundBox, *ui_filterMethodBox;
    QLineEdit* ui_filterLineText;
    QLabel *label18, *label19, *label20, *label21, *label22, *label23, *label23a, *label18a;

    QGroupBox *filBoxCheck;
    QGridLayout *filCheckLay;
    QLabel *label30, *label31;
    QComboBox *filterCheckOutBox, *filterCheckInBox;
    QPushButton *saveFilterSettings, *loadFilterSettings;
    QCheckBox *delete_filtered_box;
    QCheckBox *fill_by_zero_box;



    //виджеты вкладки выбора списка полей для основной таблицы
    QWidget *tab6;
    QGridLayout *layTab6;
    QGridLayout *layFields;
    QLabel *label25, *label26, *label27, *label28;
    QMap<QString, bool> fullFieldsList; //список вообще всех полей


    //виджеты вкладки для управления файлом продаж
    SellFilterWidget *sellFilter;

    autozakaz_config acfg;


    //виджеты вкладки настроек внешней программы
    QWidget *tab8;
    QGridLayout *layTab8;
    QLineEdit *ext_prog_name_edit, *ext_conf_edit, *ext_shablon_name_edit;
    QPushButton *select_ext_app_button, *select_ext_conf_file, *select_ext_shablon_button;



    QSettings m_settings;
    int mainTableWidth, mainTableHeith;

    editor* shablon_editor;

    //валидаторы для штрих-кода, номера и текста
    BarcodeValidator* ui_bvalidator;
    TNValidator* ui_nvalidator;
    SqlValidator* ui_svalidator;



    //контролы SQL и DBF

    QRadioButton  *ui_radioButton_1,
                  *ui_radioButton_2,
                  *ui_radioButton_3,
                    *ui_radioButton_5;
    QGroupBox *ui_groupBox, *ui_groupBox_6;
    //QTableWidget* tableWidget;
    int sizeDeltaX;  //разница в ширине между основным окном и таблицей
    int sizeDeltaY;
    QLabel *ui_countLabel;
    QTabWidget* ui_tabWidget;
    //QFile file; //файл шаблона
    QString currShablonFileName;
    QStringList shablonList;    //список шаблонов в текущей дирректории
    //QString shablonPath;
    QDomDocument domDoc;
    QDomElement shablonElement;
    QPushButton *ui_pushButton, *ui_maxButton;

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


    //переменные для работы фильтра
    bool filter_is_on;
    DbfConfig filterDbf;
    Tinformer* filterInformer;

    QString pageSize;   //формат бумаги - А4, А5
    QString paperOrientation;   // книжная или альбомная
    int Crows, Ccols;   //количество строк и столбцов в шаблоне ценника
    float pageW, pageH; //ширина и высота бумаги выбранного формата (в мкм)
    float zoomedPageW, zoomedPageH; //размеры немасштабированного поля
    float otstupTop;    //отступ от верхней границы листа
    float zoom;         //масштаб предпросмотра
    QRectF rectCen;     //размер одного ценника из шаблона
    QList<QRectF> pages;    //области для постраничной печати
    QStringList opisateli;


    QSpinBox* ui_spinLimit;

    //переменные для скачивания файлов обновлений
    QNetworkAccessManager *manager, *manager2;
    int update_counter, update_count;
    QString currentUpdatingFile;
    QString updatingPath;
    QMap<QNetworkReply*, QString> up_files;
    ListFoundedItemsDialog* update_dlg;

    bool unfinishedHttp;


    QToolBar* ui_mainToolBar;
    QMenuBar* mainMenu;
    QMenu *menuFile, *menuEdit, *menuSell, *menuHelp;
    QMenu *cenMenu;
    QMenu *macroMenu;
    QMenu *selectFoundMethodMenu;

    QAction* action_new; //новый файл
    QAction* action_exit; //выход
    QAction* action_save; //сохранить
    QAction* action_open; //загрузить список
    QAction* action_make; //сформировать ценники
    QAction* action_scale_up;
    QAction* action_scale_down;    //масштаб - меньше и больше
    QAction* action_print;
    QAction* action_about;
    QAction* action_minus;
    QAction* apply_filter_on_current_list;
    QAction* interchange_prices_in_table;
    QAction* intellect_interchange_prices_in_table;
    QAction* action_verify_barcode;
    QAction* action_update_prices;
    QAction* action_update_prices_in_minus;
    QAction* action_update_names;
    QAction* action_search_by_tnomer_in_clipboard;
    QAction* action_load_tovar_list_from_clipboard;
    QAction* action_export_tovar_list_to_clipboard;
    QAction* action_on_off_filter;
    QAction* action_filter_not_delete;
    QAction* action_filter_fill_zero_if_no_contains;
    QAction* action_set_special_shablon_to_zero_price2;
    QAction* action_sell_filter;
    QAction* action_render_in_external_app;
    QAction* action_open_in_external_app;
    QAction* action_expand_list;
    QAction* action_select_method_barcode;
    QAction* action_select_method_tnomer;
    QAction* action_select_method_tname;
    QAction* action_get_sum_of_tovar;
    QAction* action_program_update;
    QAction* action_zakaz10;
    QAction* action_remove_zero_items;
    QAction* action_left_items_with_zero_price2;
    QAction* action_group_same_shablons;
    QAction* action_check_line_prices;
    QAction* action_start_macro;
    QAction* action_edit_macro;
    QAction* action_collaps_same_items;
    QAction* action_get_sold_items;


private slots:
    void httpRequestFinished(QNetworkReply* rpl);
    void httpOneFileFinished(QNetworkReply* rpl);

    void get_method_from_ui();

    void on_source_changed_toDBF();
    void on_source_changed_toMySQL();
    void on_opisateli_updated();
    void on_mysql_table_selector_comboBox_activated(QString );
    void on_connectMySQLButton_clicked();
    void on_action_print_triggered();
    void on_action_scaleDown_triggered();
    void on_action_scaleUp_triggered();
    void on_comboBox_activated(QString );
    void on_tabWidget_currentChanged(int index);
    void on_selectShablonButton_clicked();
    void on_action_make_triggered();
    void on_apply_filter_on_current_list_triggered();
    void on_interchange_prices_in_table_triggered();
    void on_intellect_interchange_prices_in_table_triggered();
    void on_action_search_by_tnomer_in_clipboard_triggered();
    void on_action_load_tovar_list_in_clipboard_triggered();
    void on_action_export_tovar_list_to_clipboard();
    void on_action_verify_barcode();
    void on_action_update_prices();
    void update_prices(bool AskUserToSave = true);
    void on_action_update_prices_in_minus();
    void on_action_update_names();
    void update_names(bool ask);
    void on_action_set_special_shablon_to_zero_price2();
    void on_action_sell_filter_triggered();
    void on_action_render_in_external_app();
    void on_action_open_in_external_app();
    void on_action_expand_triggered(bool);
    void on_action_select_method_barcode();
    void on_action_select_method_tnomer();
    void on_action_select_method_tname();
    void on_action_get_sum_of_tovar();
    void on_action_program_update();
    void on_action_zakaz10_triggered();
    void on_action_remove_zero_items();
    void on_action_left_itms_zero_price2();
    void on_action_check_line_prices();
    void on_action_start_macro();
    void on_action_edit_macro();
    void on_collaps_same_items();
    QVector<Tovar> collaps_same_items(QVector<Tovar> inputList);
    void on_action_get_sold_items();
    void on_action_filter_not_delete(bool statu);
    void on_action_filter_fill_zero_if_no_contains(bool state);
    void on_fill_by_zero_box_checked(bool state);
    void get_sold_items(bool toGroup = true);

    void on_lineEdit_returnPressed();
    void tovar_search();
    void new_line_ready();
    void update_values();   //обновить значеня шаблона в UI-форме
    QDomDocument read_file_shablon(QString str);    //прочитать шаблон
    void describe_shablon(QDomDocument shablon);
    void generate_preview();
    void update_ui_tb_fields(QStringList list);
    void update_ui_db_controls();
    void update_ui_connection_established();
    void on_saveSourceButton();
    void on_loadSourceButton();
    void on_saveFilterSettings();
    void on_loadFilterSettings();
    //void on_selectSellFileButtonClicked();
    //void check_sell_file();
    //void updateSellTab();
    void select_ext_conf_button_clicked();
    void select_ext_app_button_clicked();
    void select_ext_shablon_button_clicked();
    void on_data_changed();


    QList<Tovar> show_found_items(QList<Tovar> inputList);
    QList<Tovar> apply_filter(QList<Tovar> inputList, bool startProgressBar = true);
    QRectF get_shablon_rect(const QDomNode& node);
    QString get_shablon_name(const QDomNode& node);
    bool read_opisateli_from_settings();
    void set_opisateli_from_settings();

    void on_selectSourceDBF_file_button_clicked();
    void on_sourceDBFfile_changed(QString fileName);
    void on_spinLimit_valueChanged(int );
    void on_maxButton_clicked();


    void on_show_editor_button_clicked();
    void on_action_save_triggered();
    void on_action_open_triggered();
    void on_action_new_triggered();

    QList<Tovar> convert_xml_into_tovar_list(QDomDocument doc);
    QDomDocument convert_tovar_list_into_xml(QList<Tovar> tovarList);
    QList<Tovar> minus(QList<Tovar> oldList, QList<Tovar> newList);
    QList<Tovar> expand(QList<Tovar> list);
    QList<Tovar> group(QList<Tovar> list);

    void on_action_minus_triggered();
    void on_filterBox_toggled(bool arg1);
    void on_filterFileSelectButton_clicked();
    void on_filterFileName_changed();
    void read_filter_settings();

    void on_radioButton_6_clicked();
    void on_radioButton_7_clicked();

    void on_action_about_triggered();

    void on_shablon_name_changed(QString name);

    void on_shablonList_combo_changed(int index);

    void on_fieldListBox_checked(bool status);

    void save_tovar_list_into_file(QString name, QList<Tovar> spisok, bool ask_for_fileName = true);

    void ask_user_to_save_wrong_tovar_list(QList<Tovar> spisok);

    bool save_tovar_list_into_dbf(QString fileName, QList<Tovar> spisok);

    void debug_tovar(QList<Tovar> list);

    void comboBoxSetText(QComboBox *bx, QString txt);

};

#endif // CENGEN_H
