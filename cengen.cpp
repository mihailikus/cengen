#include "cengen.h"
#include "tinformer.h"
#include "barcode.h"
#include <QDebug>
#include <QFileDialog>
#include <QDesktopWidget>
#include "cen_viewer.h"


cengen::cengen(QWidget *parent) : QMainWindow(parent)
{
    shablonList.clear();

    //создаем основные рюшечки приложения
    this->make_actions();
    this->make_toolBar();
    this->make_mainMenu();
    this->make_status_bar();

    ui_tabWidget = new QTabWidget;
    connect (ui_tabWidget, SIGNAL(currentChanged(int)), SLOT(on_tabWidget_currentChanged(int)));

    this->setCentralWidget(ui_tabWidget);

    //создаем вкладки, составляющие суть API программы
    //      (мне показалось, что удобней сделать несколько вкладок с настройками
    //          чем кучу открывающихся окон
    this->make_search_tab();
    this->make_shablon_tab();
    this->make_preview_tab();
    this->make_source_tab();
    this->make_filter_tab();
    this->make_fieldList_tab();
    this->make_sellSettings_tab();
    this->make_ext_app_tab();

    tableWidget->set_editing_price2(false);
    tableWidget->set_tovar_searched(false);


    my_informer = new Tinformer();
    zoom = 1.0;

    method = "tbarcode";
    currShablonFileName = "";

    this->paperOrientation = "portrate";

    //конфигурим фильтр
    filter_is_on = false;
    filterInformer = new Tinformer();

    sell_file_is_checked = false;
    previewed = false;

    //После создания всех форм - читаем настройки из конфига
    this->readSettings();

    //и в самом конце - форма готова к новому поиску
    this->new_line_ready();
}

cengen::~cengen()
{
    writeSettings();
}

void cengen::make_actions() {

    //создание нового списка товаров
    action_new = new QAction(QIcon(":/share/images/resources/window_new.png"),
                             tr("New tovar list"), this);
    action_new->setShortcut(QKeySequence("Ctrl+N"));
    action_new->setToolTip(tr("Create new tovar list"));
    connect(action_new, SIGNAL(triggered()), SLOT(on_action_new_triggered()));

    //загрузить
    action_open = new QAction(QIcon(":/share/images/resources/folder_blue_open.png"),
                             tr("Load new list"), this);
    action_open->setShortcut(QKeySequence("Ctrl+L"));
    action_open->setToolTip(tr("Load and append tovar list"));
    connect(action_open, SIGNAL(triggered()), SLOT(on_action_open_triggered()));

    //сохранить
    action_save = new QAction(QIcon(":/share/images/resources/save.png"),
                            tr("Save current list"), this);
    action_save->setShortcut(QKeySequence("Ctrl+S"));
    action_save->setToolTip(tr("Save current tovar list"));
    connect(action_save, SIGNAL(triggered()), SLOT(on_action_save_triggered()));

    //сформировать ценники
    action_make = new QAction(QIcon(":/share/images/resources/apply.png"),
                                tr("MakeUp cennic"), this);
    action_make->setToolTip(tr("Make up cennic for tovar list"));
    action_make->setShortcut(QKeySequence("F12"));
    connect(action_make, SIGNAL(triggered()), SLOT(on_action_make_triggered()));


    //вывод на печать
    action_print = new QAction(QIcon(":/share/images/resources/print.png"),
                               tr("Print"), this);
    action_print->setToolTip(tr("Send previewed to printer"));
    action_print->setShortcut(QKeySequence("Ctrl+P"));
    connect(action_print, SIGNAL(triggered()), SLOT(on_action_print_triggered()));
    //action_print->setEnabled(false);

    //масштаб - больше и меньше
    action_scale_up = new QAction(QIcon(":/share/images/resources/up.png"),
                                  tr("Scale UP"), this);
    connect(action_scale_up, SIGNAL(triggered()), SLOT(on_action_scaleUp_triggered()));

    action_scale_down = new QAction(QIcon(":/share/images/resources/down.png"),
                                  tr("Scale DOWN"), this);
    connect(action_scale_down, SIGNAL(triggered()), SLOT(on_action_scaleDown_triggered()));

    action_scale_up->setEnabled(false);
    action_scale_down->setEnabled(false);


    //выход из программы
    action_exit = new QAction(QIcon(":/share/images/resources/button_cancel.png"),
                             tr("Exit"), this);
    action_exit->setShortcut(QKeySequence("Ctrl+Q"));
    connect(action_exit, SIGNAL(triggered()), SLOT(close()));

    //о программе
    action_about = new QAction(tr("About program"), this);
    connect(action_about, SIGNAL(triggered()), SLOT(on_action_about_triggered()));

    //вычитание списка товаров
    action_minus = new QAction(tr("Minus tovar list"), this);
    connect(action_minus, SIGNAL(triggered()), SLOT(on_action_minus_triggered()));

    //применить фильтр к текущему списку товаров из главной таблицы
    apply_filter_on_current_list = new QAction(QIcon(":/share/images/resources/filter.png"),
                              tr("Apply filter"), this);
    apply_filter_on_current_list->setToolTip(tr("Apply filter on current tovar list"));
    connect(apply_filter_on_current_list, SIGNAL(triggered()),
            SLOT(on_apply_filter_on_current_list_triggered()));

    //поменять местами старую и новую цену
    interchange_prices_in_table = new QAction(tr("Interchange prices"), this);
    interchange_prices_in_table->setToolTip(tr("Intercange OLD and NEW prices"));
    connect(interchange_prices_in_table, SIGNAL(triggered()),
            this, SLOT(on_interchange_prices_in_table_triggered()));

    action_verify_barcode = new QAction(tr("Verify or correct barcode"), this);
    connect(action_verify_barcode, SIGNAL(triggered()),
            SLOT(on_action_verify_barcode()));

    action_update_prices = new QAction(tr("Update price1 for current tovar list"), this);
    connect(action_update_prices, SIGNAL(triggered()),
            SLOT(on_action_update_prices()));

    action_update_prices_in_minus = new QAction(tr("Update price1 for current tovar list after re-pricing"), this);
    connect(action_update_prices_in_minus, SIGNAL(triggered()),
            SLOT(on_action_update_prices_in_minus()));

    action_update_names = new QAction(tr("Update names for current tovar list"), this);
    connect(action_update_names, SIGNAL(triggered()),
            SLOT(on_action_update_names()));

    action_search_by_tnomer_in_clipboard = new QAction(tr("Search by tnomer in clip-board"), this);
    connect(action_search_by_tnomer_in_clipboard, SIGNAL(triggered()),
            SLOT(on_action_search_by_tnomer_in_clipboard_triggered()));

    action_on_off_filter = new QAction(tr("Filter ON"), this);
    action_on_off_filter->setShortcut(QKeySequence("Ctrl+F"));
    action_on_off_filter->setCheckable(true);
    connect(action_on_off_filter, SIGNAL(triggered(bool)),
            SLOT(on_filterBox_toggled(bool)));

    action_load_tovar_list_from_clipboard = new QAction(tr("Load tovar list from clipboard"), this);
    connect (action_load_tovar_list_from_clipboard, SIGNAL(triggered()),
             SLOT(on_action_load_tovar_list_in_clipboard_triggered()));

    action_export_tovar_list_to_clipboard = new QAction(tr("Export tovar list to clipboard"), this);
    connect (action_export_tovar_list_to_clipboard, SIGNAL(triggered()),
             SLOT(on_action_export_tovar_list_to_clipboard()));

    intellect_interchange_prices_in_table = new QAction(tr("Intellect exchange prices"), this);
    connect (intellect_interchange_prices_in_table, SIGNAL(triggered()),
             SLOT(on_intellect_interchange_prices_in_table_triggered()));

    action_set_special_shablon_to_zero_price2 = new QAction(tr("Set special shablon for zero price2"), this);
    connect (action_set_special_shablon_to_zero_price2, SIGNAL(triggered()),
             SLOT(on_action_set_special_shablon_to_zero_price2()));

    action_sell_filter = new QAction(tr("Apply filter by sold tovars"), this);
    action_sell_filter->setShortcut(QKeySequence("F10"));
    connect (action_sell_filter, SIGNAL(triggered()),
             SLOT(on_action_sell_filter_triggered()));

    action_render_in_external_app = new QAction(tr("Render in external prog"), this);
    action_render_in_external_app->setShortcut(QKeySequence("F8"));
    connect(action_render_in_external_app, SIGNAL(triggered()),
            SLOT(on_action_render_in_external_app()));

    action_expand_list = new QAction(tr("Expand tovar list for each item prices"), this);
    action_expand_list->setCheckable(true);
    action_expand_list->setChecked(false);
    connect(action_expand_list, SIGNAL(triggered(bool)),
            SLOT(on_action_expand_triggered(bool)));

    action_select_method_barcode = new QAction(tr("Use barcode for seeaking"), this);
    action_select_method_barcode->setShortcut(QKeySequence("Ctrl+1"));
    connect(action_select_method_barcode, SIGNAL(triggered()),
            SLOT(on_action_select_method_barcode()));

    action_select_method_tnomer = new QAction(tr("Use tnomer for seeaking"), this);
    action_select_method_tnomer->setShortcut(QKeySequence("Ctrl+2"));
    connect(action_select_method_tnomer, SIGNAL(triggered()),
            SLOT(on_action_select_method_tnomer()));

    action_select_method_tname = new QAction(tr("Use tname for seeaking"), this);
    action_select_method_tname->setShortcut(QKeySequence("Ctrl+3"));
    connect(action_select_method_tname, SIGNAL(triggered()),
            SLOT(on_action_select_method_tname()));

    action_get_sum_of_tovar = new QAction(tr("Get sum of tovar"), this);
    action_get_sum_of_tovar->setShortcut(QKeySequence("F2"));
    connect(action_get_sum_of_tovar, SIGNAL(triggered()),
            SLOT(on_action_get_sum_of_tovar()));

    action_program_update = new QAction(tr("Check new version"), this);
    connect(action_program_update, SIGNAL(triggered()),
            SLOT(on_action_program_update()));

    action_zakaz10 = new QAction(tr("Make zakaz for 10 days"), this);
    action_zakaz10->setShortcut(QKeySequence("F3"));
    connect(action_zakaz10, SIGNAL(triggered()),
            SLOT(on_action_zakaz10_triggered()));
    action_remove_zero_items = new QAction(tr("Remove items with zero quantity"), this);
    connect(action_remove_zero_items, SIGNAL(triggered()),
            SLOT(on_action_remove_zero_items()));

}

void cengen::make_toolBar() {
    ui_mainToolBar = new QToolBar(tr("Main toolbar"));

    ui_mainToolBar->addAction(action_new);
    ui_mainToolBar->addAction(action_open);
    ui_mainToolBar->addAction(action_save);
    ui_mainToolBar->addSeparator();
    ui_mainToolBar->addAction(apply_filter_on_current_list);
    ui_mainToolBar->addAction(action_make);
    ui_mainToolBar->addSeparator();
    ui_mainToolBar->addAction(action_print);
    ui_mainToolBar->addAction(action_scale_up);
    ui_mainToolBar->addAction(action_scale_down);
    ui_mainToolBar->addSeparator();
    ui_mainToolBar->addAction(action_exit);

    addToolBar(ui_mainToolBar);
}

void cengen::make_mainMenu() {
    mainMenu = new QMenuBar;
    menuFile = mainMenu->addMenu(tr("File"));
    menuFile->addAction(action_new);
    menuFile->addAction(action_open);
    menuFile->addAction(action_minus);
    menuFile->addAction(action_save);
    menuFile->addSeparator();
    menuFile->addAction(action_print);
    menuFile->addSeparator();
    menuFile->addAction(action_exit);

    menuEdit = mainMenu->addMenu(tr("Edit"));
    menuEdit->addAction(action_on_off_filter);
    menuEdit->addSeparator();
    menuEdit->addAction(interchange_prices_in_table);
    menuEdit->addAction(intellect_interchange_prices_in_table);
    menuEdit->addAction(action_update_prices);
    menuEdit->addAction(action_update_prices_in_minus);
    menuEdit->addAction(action_update_names);
    menuEdit->addSeparator();
    menuEdit->addAction(action_set_special_shablon_to_zero_price2);
    menuEdit->addSeparator();
    menuEdit->addAction(action_search_by_tnomer_in_clipboard);
    menuEdit->addAction(action_load_tovar_list_from_clipboard);
    menuEdit->addSeparator();
    menuEdit->addAction(action_export_tovar_list_to_clipboard);
    menuEdit->addSeparator();
    menuEdit->addAction(action_verify_barcode);
    menuEdit->addSeparator();
    menuEdit->addAction(action_remove_zero_items);

    menuSell = mainMenu->addMenu((tr("Sell control")));
    menuSell->addAction(action_sell_filter);
    menuSell->addSeparator();
    menuSell->addAction(action_get_sum_of_tovar);
    menuSell->addSeparator();
    menuSell->addAction(action_zakaz10);

    cenMenu = mainMenu->addMenu(tr("Cennic's"));
    selectFoundMethodMenu = cenMenu->addMenu(tr("Method for tovar search"));
    cenMenu->addSeparator();
    cenMenu->addAction(action_expand_list);
    cenMenu->addSeparator();
    cenMenu->addAction(action_make);
    cenMenu->addAction(action_render_in_external_app);

    selectFoundMethodMenu->addAction(action_select_method_barcode);
    selectFoundMethodMenu->addAction(action_select_method_tnomer);
    selectFoundMethodMenu->addAction(action_select_method_tname);


    menuHelp = mainMenu->addMenu(tr("About"));
    menuHelp->addAction(action_program_update);
    menuHelp->addSeparator();
    menuHelp->addAction(action_about);

    this->setMenuBar(mainMenu);
}

void cengen::make_search_tab() {

    tab1 = new QWidget;
    layTab1 = new QGridLayout;

    //готовим lineEdit - строку для ввода поисковой фразы
    ui_lineEdit = new QLineEdit;
    ui_lineEdit->setAlignment(Qt::AlignCenter);
    connect (ui_lineEdit, SIGNAL(returnPressed()), SLOT(on_lineEdit_returnPressed()));
    //устанавливаем валидатор (первый вход в программу - поиск через баркод)
    ui_bvalidator = new BarcodeValidator(this);
    ui_nvalidator = new TNValidator(this);
    ui_svalidator = new SqlValidator(this);
    ui_lineEdit->setValidator(ui_bvalidator);
    layTab1->addWidget(ui_lineEdit, 0, 0, 1, 3);

    ui_pushButton = new QPushButton(tr("Search", "Button for search"));
    connect (ui_pushButton, SIGNAL(clicked()), SLOT(on_lineEdit_returnPressed()));
    layTab1->addWidget(ui_pushButton, 0, 3);

    //готовим групбокс - выбор метода поиска (штрих-код, название и т.п.)
    ui_radioButton_1 = new QRadioButton(tr("Barcode"));
    ui_radioButton_2 = new QRadioButton(tr("Tnomer"));
    ui_radioButton_3 = new QRadioButton(tr("Name"));
    ui_radioButton_5 = new QRadioButton(tr("FREE", "uses for NULL search"));
    connect (ui_radioButton_1, SIGNAL(clicked()), SLOT(get_method_from_ui()));
    connect (ui_radioButton_2, SIGNAL(clicked()), SLOT(get_method_from_ui()));
    connect (ui_radioButton_3, SIGNAL(clicked()), SLOT(get_method_from_ui()));
    connect (ui_radioButton_5, SIGNAL(clicked()), SLOT(get_method_from_ui()));
    ui_radioButton_1->setChecked(true);
    layBox1 = new QGridLayout;
    ui_groupBox = new QGroupBox(tr("Method for tovar search"));
    connect (ui_groupBox, SIGNAL(toggled(bool)), SLOT(get_method_from_ui()));
    layBox1->addWidget(ui_radioButton_1, 0, 0);
    layBox1->addWidget(ui_radioButton_2, 0, 1);
    layBox1->addWidget(ui_radioButton_3, 0, 2);
    layBox1->addWidget(ui_radioButton_5, 0, 3);
    ui_groupBox->setLayout(layBox1);

    layBox6 = new QGridLayout;
    ui_spinLimit = new QSpinBox(this);
    ui_spinLimit->setMinimum(0);
    ui_spinLimit->setMaximum(1000);
    ui_spinLimit->setValue(10);
    ui_maxButton = new QPushButton(tr("max"), this);
    connect(ui_maxButton, SIGNAL(clicked()), SLOT(on_maxButton_clicked()));
    connect(ui_spinLimit, SIGNAL(valueChanged(int)), SLOT(on_spinLimit_valueChanged(int)));
    layBox6->addWidget(ui_spinLimit, 0, 0, 2, 2);
    layBox6->addWidget(ui_maxButton, 0, 2, 2, 1);
    ui_groupBox_6 = new QGroupBox(tr("Search limit"));
    ui_groupBox_6->setLayout(layBox6);

    layTab1->addWidget(ui_groupBox, 1, 0, 1, 4);
    layTab1->addWidget(ui_groupBox_6, 0, 4, 2, 1);

    tableWidget = new MainTableWidget();
    tableWidget->set_method_view(0);    //0 - удаление
    connect (tableWidget, SIGNAL(row_count_changed()), SLOT(new_line_ready()));
    connect (tableWidget, SIGNAL(data_changed()), SLOT(on_data_changed()));
    layTab1->addWidget(tableWidget, 2, 0, 1, 6);

    tab1->setLayout(layTab1);
    ui_tabWidget->insertTab(TabsOrder::Search, tab1, tr("SEARCH"));

}

void cengen::make_preview_tab() {
    tab3 = new QWidget;
    layTab3 = new QGridLayout;

    //подготовка сцены для рендеринга ценников
    currentScene = new QGraphicsScene;
    view = new QGraphicsView();
    view->setScene(currentScene);
    layTab3->addWidget(view, 0, 0);
    tab3->setLayout(layTab3);
    ui_tabWidget->insertTab(TabsOrder::Preview, tab3, tr("PREVIEW"));
}

void cengen::make_fieldList_tab() {
    tab6 = new QWidget;
    layTab6 = new QGridLayout;

    label25 = new QLabel(tr("Used fileds:"));
    label25->setAlignment(Qt::AlignCenter);
    layTab6->addWidget(label25, 0, 0, 1, 2);

    label27 = new QLabel(tr("Describer"));
    layTab6->addWidget(label27, 1, 0);

    label28 = new QLabel(tr("Default value"));
    layTab6->addWidget(label28, 1, 1, 1, 2);

    fullFieldsList.clear();
    int i = 0;
    //если будет больше 9 - переделать чтоб шли по порядку
    fullFieldsList.insert(QString::number(++i) + " " + tr("Tnomer"), true);
    fullFieldsList.insert(QString::number(++i) + " " + tr("Name"), true);
    fullFieldsList.insert(QString::number(++i) + " " + tr("Barcode"), true);
    fullFieldsList.insert(QString::number(++i) + " " + tr("Price"), true);
    fullFieldsList.insert(QString::number(++i) + " " + tr("Price2"), true);
    fullFieldsList.insert(QString::number(++i) + " " + tr("Quantity"), true);
    fullFieldsList.insert(QString::number(++i) + " " + tr("Shablon"), true);
    fullFieldsList.insert(QString::number(++i) + " " + tr("DELETE"), true);

    layFields = new QGridLayout;

    QMultiMap<QString, bool>::iterator it = fullFieldsList.begin();
    i = 0;
    for (; it!=fullFieldsList.end(); ++it) {
        QCheckBox *box = new QCheckBox(it.key());
        box->setChecked(it.value());
        layFields->addWidget(box, i, 0);
        QLineEdit *defValue = new QLineEdit;
        layFields->addWidget(defValue, i, 1, 1, 3);
        connect (box, SIGNAL(clicked(bool)), SLOT(on_fieldListBox_checked(bool)));
        i++;
    }
    layTab6->addLayout(layFields, 2, 0, 1, 4);

    label26 = new QLabel(tr(" "));
    layTab6->addWidget(label26);

    tab6->setLayout(layTab6);

    ui_tabWidget->insertTab(TabsOrder::fList, tab6, tr("Fields"));

    tableWidget->set_tableFields(fullFieldsList);
}

void cengen::make_source_tab() {
    tab4 = new QWidget;
    layTab4 = new QGridLayout;

    laySQL = new QGridLayout;
    ui_groupSQL = new QGroupBox(tr("MySQL config"));
    ui_groupSQL->setCheckable(true);
    ui_groupSQL->setChecked(false);
    connect (ui_groupSQL, SIGNAL(toggled(bool)), SLOT(on_source_changed_toMySQL()));

    ui_lineHost = new QLineEdit;
    ui_lineName = new QLineEdit;
    ui_linePort = new QLineEdit(tr("Not used yet"));
    ui_lineUser = new QLineEdit;
    ui_linePass = new QLineEdit;
    ui_linePort->setEnabled(false);
    label6 = new QLabel(tr("DB Host"));
    label10= new QLabel(tr("DB port"));
    label9 = new QLabel(tr("DB name"));
    label7 = new QLabel(tr("User name"));
    label8 = new QLabel(tr("User password"));
    label11 = new QLabel(tr("Select table from MySQL"));
    laySQL->addWidget(ui_lineHost, 0, 1);
    laySQL->addWidget(label6, 0, 0);
    laySQL->addWidget(ui_linePort, 0, 3);
    laySQL->addWidget(label10, 0, 2);
    laySQL->addWidget(label9, 1, 0);
    laySQL->addWidget(ui_lineName, 1, 1, 1, 3);
    laySQL->addWidget(label7, 2, 0);
    laySQL->addWidget(ui_lineUser, 2, 1);
    laySQL->addWidget(label8, 2, 2);
    laySQL->addWidget(ui_linePass, 2, 3);

    ui_connectMySQLButton = new QPushButton (tr("Connect to MySQL"));
    connect(ui_connectMySQLButton, SIGNAL(clicked()), SLOT(on_connectMySQLButton_clicked()));
    laySQL->addWidget(ui_connectMySQLButton, 3, 2, 1, 2);

    laySQL->addWidget(label11, 4, 0, 1, 2);

    ui_mysql_table_selector_comboBox = new QComboBox;
    connect(ui_mysql_table_selector_comboBox, SIGNAL(activated(QString)), SLOT(on_mysql_table_selector_comboBox_activated(QString)));
    this->ui_mysql_table_selector_comboBox->setEnabled(false);
    laySQL->addWidget(ui_mysql_table_selector_comboBox, 4, 2, 1, 2);

    ui_groupSQL->setLayout(laySQL);
    layTab4->addWidget(ui_groupSQL, 0, 0, 1, 2);

    ui_groupDBF = new QGroupBox(tr("DBF file config"));
    ui_groupDBF->setCheckable(true);
    ui_groupDBF->setChecked(true);
    connect(ui_groupDBF, SIGNAL(toggled(bool)), SLOT(on_source_changed_toDBF()));
    layDBF = new QGridLayout;
    label16 = new QLabel(tr("DBF file selected is:"));
    labelDBFname = new QLabel;
    ui_selectSourceDBF_file_button = new QPushButton(tr("Select DBF file"));
    connect(ui_selectSourceDBF_file_button, SIGNAL(clicked()), SLOT(on_selectSourceDBF_file_button_clicked()));
    layDBF->addWidget(ui_selectSourceDBF_file_button, 0, 0);
    layDBF->addWidget(label16, 1, 0, 1, 3);
    layDBF->addWidget(labelDBFname, 2, 0, 1, 3);
    ui_groupDBF->setLayout(layDBF);
    layTab4->addWidget(ui_groupDBF, 1, 0, 1, 2);


    ui_groupOpisateli = new QGroupBox(tr("Placement of DBF fields"));
    layOpisateli = new QGridLayout;

    label12 = new QLabel(tr("Tnomer"));
    label13 = new QLabel(tr("Name of tovar"));
    label14 = new QLabel(tr("Barcode"));
    label15 = new QLabel(tr("Price"));
    layOpisateli->addWidget(label12, 0, 0);
    layOpisateli->addWidget(label13, 1, 0);
    layOpisateli->addWidget(label14, 2, 0);
    layOpisateli->addWidget(label15, 3, 0);

    ui_comboTnomer = new QComboBox;
    ui_comboTname = new QComboBox;
    ui_comboTbarcode = new QComboBox;
    ui_comboTprice = new QComboBox;
    connect(ui_comboTnomer, SIGNAL(activated(int)), SLOT(on_opisateli_updated()));
    connect(ui_comboTname, SIGNAL(activated(int)), SLOT(on_opisateli_updated()));
    connect(ui_comboTbarcode, SIGNAL(activated(int)), SLOT(on_opisateli_updated()));
    connect(ui_comboTprice, SIGNAL(activated(int)), SLOT(on_opisateli_updated()));
    layOpisateli->addWidget(ui_comboTnomer, 0, 1);
    layOpisateli->addWidget(ui_comboTname, 1, 1);
    layOpisateli->addWidget(ui_comboTbarcode, 2, 1);
    layOpisateli->addWidget(ui_comboTprice, 3, 1);

    ui_groupOpisateli->setLayout(layOpisateli);

    layTab4->addWidget(ui_groupOpisateli, 0, 2, 2, 1);

    saveSourceButton = new QPushButton (tr("Save settings to file"));
    loadSourceButton = new QPushButton (tr("Load settings from file"));
    connect (saveSourceButton, SIGNAL(clicked()), SLOT(on_saveSourceButton()));
    connect (loadSourceButton, SIGNAL(clicked()), SLOT(on_loadSourceButton()));
    layTab4->addWidget(saveSourceButton, 2, 0);
    layTab4->addWidget(loadSourceButton, 2, 1);


    tab4->setLayout(layTab4);
    ui_tabWidget->insertTab(TabsOrder::Source, tab4, tr("Data source"));
}

void cengen::make_shablon_tab() {
    tab2 = new QWidget;
    layTab2 = new QBoxLayout(QBoxLayout::LeftToRight);

    ui_groupBox_2 = new QGroupBox(tr("Shablon"));
    layBoxT2B2 = new QBoxLayout(QBoxLayout::TopToBottom);
    ui_selecctShablonButton = new QPushButton (tr("Select shablon file"));
    connect(ui_selecctShablonButton, SIGNAL(clicked()), SLOT(on_selectShablonButton_clicked()));
    layBoxT2B2->addWidget(ui_selecctShablonButton);
    ui_label = new QLabel(tr("Shablon"));
    layBoxT2B2->addWidget(ui_label);
    ui_show_editor_button = new QPushButton(tr("Open shablon in built-in editor"));
    connect(ui_show_editor_button, SIGNAL(clicked()), SLOT(on_show_editor_button_clicked()));
    layBoxT2B2->addWidget(ui_show_editor_button);
    label24 = new QLabel(tr("List of other shablons in current directory:"));
    shablonBox = new QComboBox;
    shablonBox->blockSignals(true);
    shablonBox->addItems(shablonList);
    connect(shablonBox, SIGNAL(currentIndexChanged(int)), SLOT(on_shablonList_combo_changed(int)));
    layBoxT2B2->addWidget(label24);
    layBoxT2B2->addWidget(shablonBox);



    ui_groupBox_2->setLayout(layBoxT2B2);
    layBoxT2B2->addStretch(1);

    layTab2->addWidget(ui_groupBox_2);


    ui_groupBox_3 = new QGroupBox(tr("Page setup"));
    layBoxT2B3 = new QGridLayout();
    comboBox = new QComboBox();
    comboBox->addItem(tr("Paper A4"));
    comboBox->addItem(tr("Paper A5"));
    connect (comboBox, SIGNAL(activated(QString)), SLOT(on_comboBox_activated(QString)));
    layBoxT2B3->addWidget(comboBox, 0, 0);
    ui_lineW = new QLineEdit;
    ui_lineH = new QLineEdit;
    ui_lineW_2 = new QLineEdit;
    ui_lineH_2 = new QLineEdit;
    ui_lineW->setReadOnly(true);
    ui_lineH->setReadOnly(true);
    ui_lineW_2->setReadOnly(true);
    ui_lineH_2->setReadOnly(true);
    layBoxT2B3->addWidget(ui_lineW, 1, 0);
    layBoxT2B3->addWidget(ui_lineH, 1, 2);
    layBoxT2B3->addWidget(ui_lineW_2, 2, 0);
    layBoxT2B3->addWidget(ui_lineH_2, 2, 2);
    label2 = new QLabel(tr("x"));
    label3 = new QLabel(tr("x"));
    label4 = new QLabel(tr("in mkm", "Size in mikro-millimeters"));
    label5 = new QLabel(tr("in cennic's"));
    labelOrient = new QLabel(tr("Paper orientation:"));
    labelZero = new QLabel(tr(" ", "Zero label"));
    layBoxT2B3->addWidget(label2, 1, 1);
    layBoxT2B3->addWidget(label3, 2, 1);
    layBoxT2B3->addWidget(label4, 1, 3);
    layBoxT2B3->addWidget(label5, 2, 3);
    layBoxT2B3->addWidget(labelOrient, 3, 0, 1, 4);
    layBoxT2B3->addWidget(labelZero, 6, 0, 2, 4);

    ui_radioButton_6 = new QRadioButton(tr("orientation: portrate"));
    ui_radioButton_7 = new QRadioButton(tr("orientation: landscape"));
    ui_radioButton_6->setChecked(true);
    layBoxT2B3->addWidget(ui_radioButton_6, 4, 0, 1, 3);
    layBoxT2B3->addWidget(ui_radioButton_7, 5, 0, 1, 3);
    connect(ui_radioButton_6, SIGNAL(clicked()), SLOT(on_radioButton_6_clicked()));
    connect(ui_radioButton_7, SIGNAL(clicked()), SLOT(on_radioButton_7_clicked()));
    totalCennicOnPage = new QLabel(" ");
    layBoxT2B3->addWidget(totalCennicOnPage, 9, 0, 1, 4);

    zoomLabel = new QLabel (tr("Zoom of cennic's"));
    zoomBox = new QSpinBox;
    zoomBox->setMaximum(1000);
    zoomBox->setMinimum(30);
    zoomBox->setValue(100);
    connect(zoomBox, SIGNAL(valueChanged(int)), SLOT(update_values()));

    layBoxT2B3->addWidget(zoomLabel, 6, 0, 1, 3);
    layBoxT2B3->addWidget(zoomBox, 6, 3);

    otstupLabel1 = new QLabel(tr("Otstup from Top of the page"));
    otstupTopLine = new QLineEdit("10");
    otstupTopLine->setValidator(ui_nvalidator);
    connect(otstupTopLine, SIGNAL(textChanged(QString)), SLOT(update_values()));

    layBoxT2B3->addWidget(otstupLabel1, 7, 0, 1, 3);
    layBoxT2B3->addWidget(otstupTopLine, 7, 3);

    //label32 = new QLabel(tr(""));
    expandBox = new QCheckBox(tr("Expand tovar list for each item prices"));
    layBoxT2B3->addWidget(expandBox, 8, 0, 1, 3);

    ui_groupBox_3->setLayout(layBoxT2B3);
    layTab2->addWidget(ui_groupBox_3);

    layTab2->addStretch(1);

    tab2->setLayout(layTab2);
    ui_tabWidget->insertTab(TabsOrder::Shablon, tab2, tr("Shablon"));


}

void cengen::make_filter_tab() {
    tab5 = new QWidget;
    layTab5 = new QBoxLayout(QBoxLayout::TopToBottom);


    laytab5g1 = new QGridLayout;
    label18 = new QLabel(tr("File"));
    label18a =new QLabel(tr("File not selected yet"));
    label19 = new QLabel(tr("What to find2"));
    label20 = new QLabel(tr("Where is it"));
    label21 = new QLabel(tr("Column for compare"));
    label22 = new QLabel(tr("Method of compare"));
    label23 = new QLabel(tr("value"));
    label23a = new QLabel(" ");

    saveFilterSettings = new QPushButton (tr("Save filter settings"));
    loadFilterSettings = new QPushButton (tr("Load filter settings"));
    connect (saveFilterSettings, SIGNAL(clicked()), SLOT(on_saveFilterSettings()));
    connect (loadFilterSettings, SIGNAL(clicked()), SLOT(on_loadFilterSettings()));


    ui_filterFileSelectButton = new QPushButton (tr("Select file"));
    connect (ui_filterFileSelectButton, SIGNAL(clicked()), SLOT(on_filterFileSelectButton_clicked()));

    ui_filterWhatBox = new QComboBox();
    ui_filterWhereBox = new QComboBox();
    ui_filterWhatToFoundBox = new QComboBox();
    ui_filterMethodBox = new QComboBox();

    ui_filterWhatToFoundBox->insertItems(0, QStringList()
                                         <<tr("Tnomer")
                                         <<tr("barcode")
                                         <<tr("name")
                                         <<tr("Price"));
    ui_filterWhatToFoundBox->setEnabled(false);

    ui_filterMethodBox->insertItems(0, QStringList()
                                    << tr("==")
                                    << tr("<>")
                                    << tr(">")
                                    << tr("<")
                                    << tr("LIKE"));

    laytab5g1->addWidget(label18, 0, 0);
    laytab5g1->addWidget(ui_filterFileSelectButton, 0, 1);
    laytab5g1->addWidget(label18a, 0, 2, 1, 2);
    laytab5g1->addWidget(label21, 1, 0);
    laytab5g1->addWidget(ui_filterWhatBox, 1, 1);
    laytab5g1->addWidget(label19, 1, 2);
    laytab5g1->addWidget(ui_filterWhatToFoundBox, 1, 3);
    laytab5g1->addWidget(label20, 2, 0);
    laytab5g1->addWidget(ui_filterWhereBox, 2, 1);
    laytab5g1->addWidget(label22, 2, 2);
    laytab5g1->addWidget(ui_filterMethodBox, 2, 3);
    laytab5g1->addWidget(label23, 3, 0);
    laytab5g1->addWidget(label23a, 4, 0);

    ui_filterLineText = new QLineEdit;
    laytab5g1->addWidget(ui_filterLineText, 3, 1, 1, 3);


    filBoxCheck = new QGroupBox(tr("Use filter for update fields"));
    filBoxCheck->setCheckable(true);
    filBoxCheck->setChecked(true);

    filCheckLay = new QGridLayout;

    label30 = new QLabel(tr("Use found field: "));
    label31 = new QLabel(tr("for update field in main table:"));
    filCheckLay->addWidget(label30, 0, 0);
    filCheckLay->addWidget(label31, 1, 0);

    filterCheckInBox = new QComboBox;
    filterCheckOutBox = new QComboBox;
    filCheckLay->addWidget(filterCheckInBox, 0, 1);
    filCheckLay->addWidget(filterCheckOutBox, 1, 1);




    filBoxCheck->setLayout(filCheckLay);
    laytab5g1->addWidget(filBoxCheck, 4, 0, 1, 4);

    laytab5g1->addWidget(saveFilterSettings, 7, 0);
    laytab5g1->addWidget(loadFilterSettings, 7, 1);

    delete_filtered_box = new QCheckBox(tr("Delete items not found by filter"));
    delete_filtered_box->setChecked(true);
    laytab5g1->addWidget(delete_filtered_box, 5, 0, 1, 2);


    ui_filterBox = new QGroupBox(tr("Filter") + tr(" - version alpha, just DBF"));
    ui_filterBox->setCheckable(true);
    ui_filterBox->setChecked(false);
    connect(ui_filterBox, SIGNAL(toggled(bool)), SLOT(on_filterBox_toggled(bool)));
    ui_filterBox->setLayout(laytab5g1);

    layTab5->addWidget(ui_filterBox);
    layTab5->addSpacing(100);

    tab5->setLayout(layTab5);
    ui_tabWidget->insertTab(TabsOrder::Filter, tab5, tr("Filter"));



}

void cengen::make_sellSettings_tab() {
    tab7 = new QWidget;
    layTab7 = new QGridLayout;

    selectSellFileButton = new QPushButton (tr("Select file with sell records"));
    layTab7->addWidget(selectSellFileButton, 0, 0);
    connect(selectSellFileButton, SIGNAL(clicked()), SLOT(on_selectSellFileButtonClicked()));

    lb1 = new QLabel(tr("Selected file: "));
    lb2 = new QLabel("");

    layTab7->addWidget(lb1, 0, 1);
    layTab7->addWidget(lb2, 0, 2, 1, 3);

    sellNomerBox = new QComboBox;
    sellDateBox = new QComboBox;
    sellTimeBox = new QComboBox;
    sellKolBox = new QComboBox;

    lb3 = new QLabel(tr("Tnomer"));
    lb4 = new QLabel(tr("Date of sell"));
    lb5 = new QLabel(tr("Time of sell"));
    lb6 = new QLabel(tr("Quantity"));

    layTab7->addWidget(lb3, 1, 0);
    layTab7->addWidget(sellNomerBox, 1, 1);
    layTab7->addWidget(lb4, 2, 0);
    layTab7->addWidget(sellDateBox, 2, 1);
    layTab7->addWidget(lb5, 3, 0);
    layTab7->addWidget(sellTimeBox, 3, 1);
    layTab7->addWidget(lb6, 4, 0);
    layTab7->addWidget(sellKolBox, 4, 1);

    lb10 = new QLabel(tr("Method of get sell value: " ));
    methodSellBox = new QComboBox;
    methodSellBox->insertItems(0, QStringList()
                               << tr("Replace")
                               << tr("Minus")
                               << tr("Per day"));
    //methodSellValue = new QLineEdit("0");

    layTab7->addWidget(lb10, 5, 0);
    layTab7->addWidget(methodSellBox, 5, 1);
    //layTab7->addWidget(methodSellValue, 5, 2);

    dateStart = new QCalendarWidget;
    dateStop = new QCalendarWidget;

    dateStart->setFirstDayOfWeek ( Qt::Monday );
    dateStop->setFirstDayOfWeek ( Qt::Monday );

    dateStart->setGridVisible(true);
    dateStop->setGridVisible(false);


    lb7 = new QLabel(tr("From date: "));
    lb8 = new QLabel(tr("To date: "));

    layTab7->addWidget(lb7, 6, 1);
    layTab7->addWidget(dateStart, 7, 1);
    layTab7->addWidget(lb8, 6, 3);
    layTab7->addWidget(dateStop, 7, 3);


    tab7->setLayout(layTab7);
    ui_tabWidget->insertTab(TabsOrder::Sell, tab7, tr("Sell settings"));

}

void cengen::make_ext_app_tab() {
    tab8 = new QWidget;
    layTab8 = new QGridLayout;

    ext_prog_name_edit = new QLineEdit();
    select_ext_app_button = new QPushButton(tr("Select ext app"));

    layTab8->addWidget(ext_prog_name_edit, 0, 0);
    layTab8->addWidget(select_ext_app_button, 0, 1);

    ext_conf_edit = new QLineEdit();
    select_ext_conf_file = new QPushButton(tr("Select conf settings file"));

    layTab8->addWidget(ext_conf_edit, 1, 0);
    layTab8->addWidget(select_ext_conf_file, 1, 1);

    ext_shablon_name_edit = new QLineEdit();
    select_ext_shablon_button = new QPushButton(tr("Select external shablon"));

    layTab8->addWidget(ext_shablon_name_edit, 2, 0);
    layTab8->addWidget(select_ext_shablon_button, 2, 1);

    tab8->setLayout(layTab8);

    ui_tabWidget->insertTab(TabsOrder::App, tab8, tr("External application"));

    connect(select_ext_conf_file, SIGNAL(clicked()), SLOT(select_ext_conf_button_clicked()));
    connect(select_ext_app_button, SIGNAL(clicked()), SLOT(select_ext_app_button_clicked()));
    connect(select_ext_shablon_button, SIGNAL(clicked()), SLOT(select_ext_shablon_button_clicked()));
}

void cengen::make_status_bar() {
    //создаем строку состояния
    statusBar = new QStatusBar(this);
    ui_countLabel = new QLabel(tr("COUNT: ", "ITOGO") +"0");
    statusBar->addWidget(ui_countLabel);
    ui_statusLabel = new QLabel(tr("OK"));
    statusBar->addWidget(ui_statusLabel);

    progressBar = new QProgressBar;

    setStatusBar(statusBar);
}

void cengen::changeEvent(QEvent *e)
{
    QMainWindow::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        //ui->retranslateUi(this);
        qDebug() << "EVENT language change";
        break;
    default:
        break;
    }
}

void cengen::load_all_records() {
    QList<Tovar> tovarAll;
    my_informer->set_limit_search(my_informer->get_maximum());
    tovarAll = my_informer->info("", "tname");
    tableWidget->load_tovar_list_into_table(tovarAll);
    tableWidget->repaint();
    tableWidget->scrollToTop();
    ui_tabWidget->setCurrentIndex(TabsOrder::Search);
}

void cengen::tovar_search() {
    QList<Tovar> tovarListFull;
    tovarListFull = my_informer->info(ui_lineEdit->text(), this->method);
    if (filter_is_on) {
        tovarListFull = apply_filter(tovarListFull);
    }

    QList<Tovar> tovarList;

    if (tovarListFull.count() != 1) {
        tovarList = this->show_found_items(tovarListFull);
    } else {
        tovarList = tovarListFull;
    }
    tableWidget->load_tovar_list_into_table(tovarList);
}

QList<Tovar> cengen::show_found_items(QList<Tovar> inputList) {
    QList<Tovar> tovarList;
    MainTableWidget *table = new MainTableWidget;
    table->set_method_view(1);  //будет выбор с галочками
    ListFoundedItemsDialog* dlg = new ListFoundedItemsDialog(this);

    if (!inputList.count()) {
        //если пусто - показать ошибку
        dlg->setMessage(tr("NOT FOUND"));

    } else {
        table->set_tableFields(fullFieldsList);
        for (int i =0; i< table->columnCount(); i++) {
            table->set_tableTab_width(i, tableWidget->get_tableTab_width(i));
        }
        table->load_tovar_list_into_table(inputList);

        dlg->setMessage(&table);
    }
    if (dlg->exec()) {
        tovarList = table->get_tovar_list("V");
    }

    return tovarList;
}

void cengen::on_lineEdit_returnPressed()
{
    if (ui_lineEdit->text() != "" || !tableWidget->is_tovar_searched()) {
        this->tovar_search();
        this->new_line_ready();
    } else {
        //нажата Энтер с пустым полем - значит, хотим отредактировать
        //    цену только что введенного товара
        tableWidget->set_focus_on_price2();
    }

}

void cengen::new_line_ready() {
    ui_countLabel->setText(tr("COUNT: ", "ITOGO")
                           + QString::number(tableWidget->rowCount()));
    ui_lineEdit->clear();
    ui_lineEdit->setFocus();

}

void cengen::on_data_changed()
{
    previewed = false;
}

void cengen::on_action_make_triggered()
{
    //функция для формирования ценников
    ui_tabWidget->setCurrentIndex(TabsOrder::Preview);
    this->generate_preview();
    //action_print->setEnabled(true);
    previewed = true;
}

void cengen::on_selectShablonButton_clicked()
{
    QString str = QFileDialog::getOpenFileName(0, tr("Select shablon file"), currShablonFileName, tr("CEN-files (*.cen)"));

    if (file_is_ready && str == "") return;

    on_shablon_name_changed(str);

}

void cengen::on_shablonList_combo_changed(int index) {
    QString name = shablonBox->itemText(index);
    QString fullName;
    QFileInfo fi;
    for (int i = 0; i< shablonList.count(); i++) {
        fi.setFile(shablonList.at(i));
        if (fi.fileName() == name) {
            fullName = shablonList.at(i);
        }
    }
    on_shablon_name_changed(fullName);

    tableWidget->set_shablon_current(index);

}

void cengen::on_shablon_name_changed(QString str) {
    currShablonFileName = str;
    if (str == "") {
        ui_label->setText(tr("Please select file", "If no file selected"));
    } else {
        domDoc = this->read_file_shablon(str);
        this->describe_shablon(domDoc);
        file_is_ready = true;
        ui_statusLabel->setText(tr("Shablon OK"));
    }

    //получение списка остальных шаблонов из текущей дирретории
    QFileInfo fi(str);
    QDir * dir = new QDir;
    dir->setPath(fi.path());
    QFileInfoList shabList = dir->entryInfoList(QStringList() << tr("*.cen"), QDir::Files);
    shablonBox->blockSignals(true);
    shablonBox->clear();
    shablonList.clear();

    int index = 0;
    QStringList names;
    for (int i = 0; i< shabList.count(); i++) {
        names << shabList.at(i).fileName();
        shablonList << shabList.at(i).filePath();
        shablonBox->addItem(names.last());
        if (str == shabList.at(i).filePath()) index=i;
    }
    shablonBox->setCurrentIndex(index);
    shablonBox->blockSignals(false);

    tableWidget->set_shablon_list(names);
    tableWidget->set_shablon_current(index);

}

void cengen::on_tabWidget_currentChanged(int index)
{
    if (index == TabsOrder::Preview) {
        action_scale_up->setEnabled(true);
        action_scale_down->setEnabled(true);
    } else {
        action_scale_up->setEnabled(false);
        action_scale_down->setEnabled(false);
    }

    if (index == TabsOrder::Search) {
        this->new_line_ready();
        return;
    }
    update_values();
}

QRectF cengen::get_shablon_rect(const QDomNode &node) {
    QRectF rect;// = new QRectF();
    rect.setWidth(10);
    rect.setHeight(10);

    if (!file_is_ready) {
            return rect;
    }
    QDomNode domNode = node.firstChild();
    while(!domNode.isNull()) {
        if(domNode.isElement()) {
           QDomElement domElement = domNode.toElement();
           if(!domElement.isNull()) {
               if (domElement.tagName() == "base") {
                   //описание размера шаблона
                   //qDebug() << "We read rect of shablon";
                   QString width = domElement.attribute("width", "");
                   rect.setWidth(width.toFloat());
                   QString heith = domElement.attribute("heith", "");
                   rect.setHeight(heith.toFloat());
                   return rect;
               }

           }
        }
        //get_shablon_rect(domNode);
        domNode = domNode.nextSibling();
     }


    return rect;
}

QString cengen::get_shablon_name(const QDomNode &node) {
    QString name = tr("Select proper file");

    if (!file_is_ready) {
        qDebug() << "Not ready";
            return name;
    }
    QDomNode domNode = node.firstChild();
    while(!domNode.isNull()) {
        //qDebug() << "Not null";
        if(domNode.isElement()) {
            //qDebug() << "is element";
           QDomElement domElement = domNode.toElement();
           if(!domElement.isNull()) {
               if (domElement.tagName() == "name") {
                   //название шаблона
                   name = domElement.text();
                   return name;
               }
               qDebug() << "element " << domElement.tagName();

           }
        }
        //get_shablon_name(domNode);
        domNode = domNode.nextSibling();
     }

    return name;

}

void cengen::on_comboBox_activated(QString paper)
{
    qDebug() << "paper" << paper;
    update_values();
}

void cengen::update_values() {
    switch (comboBox->currentIndex()) {
    case 0:
        //qDebug() << "paperA4";
        this->pageW = 2100;
        this->pageH = 2970;
        this->pageSize = "A4";
        break;
    case 1:
        //qDebug() << "paperA5";
        this->pageW = 1480;
        this->pageH = 2100;
        this->pageSize = "A5";
        break;
    default:
        this->pageW = 2100;
        this->pageH = 2970;
        this->pageSize = "A4";
        break;
    }

    if (paperOrientation == "landscape") {
        float tmp = this->pageW;
        this->pageW = this->pageH;
        this->pageH = tmp;
    }

    //управление масштабом
    float zoomed = zoomBox->value();
    zoomedPageW = pageW / zoomed * 100;
    zoomedPageH = pageH / zoomed * 100;
    //qDebug() << "Zoomed page" << zoomedPageW << zoomedPageH;

    //управление отступом от границы страницы
    this->otstupTop = 10 * otstupTopLine->text().toInt();

    if (file_is_ready) {
        this->Crows = (zoomedPageH - otstupTop) / rectCen.height();
        this->Ccols = zoomedPageW / rectCen.width();
    } else {
        Crows = 1;
        Ccols = 1;
    }

    ui_lineW->setText(QString::number(pageW));
    ui_lineH->setText(QString::number(pageH));
    ui_lineW_2->setText(QString::number(Ccols));
    ui_lineH_2->setText(QString::number(Crows));

    totalCennicOnPage->setText(tr("Total: ")
                               +QString::number(Ccols*Crows)
                               +tr(" cennic's on one page"));
}

void cengen::generate_preview() {
    if (!file_is_ready) {
        qDebug() << "File is not ready";
        ui_statusLabel->setText(tr("File is not ready", "If file shablon is not ready"));
        return;
    }

    ui_statusLabel->setText(tr("Rendering cennic's"));
    currentScene->clear();

    pages.clear();

    QList<Tovar> spisok =  tableWidget->get_tovar_list("x");
    if (expandBox->isChecked()) spisok = expand(spisok);

    Tovar currentTovar;

    //высчитываем левый верхний угол первой страницы
    float bXpos, bYpos, bXstart, bYstart, maxYadd, oldYadd;
    bXstart = (zoomedPageW - Ccols*rectCen.width())/2;
    bYstart = (zoomedPageH - Crows*rectCen.height())/2;
    bXpos = bXstart;
    bYpos = bYstart;
    maxYadd = 0;
    int maxYaddPos = 0;
    oldYadd = 0;
    float oldX, oldY;

    int zazor = bXstart;

    //заносим этот угол в список страниц
    QRectF page(0, 0, zoomedPageW, zoomedPageH);
    pages << page;

    int cenCount = spisok.count();

    progressBar->setMaximum(cenCount);
    statusBar->addWidget(progressBar);
    progressBar->show();

    Cennic *cennics = new Cennic();

    for (int i = 0; i<spisok.count(); i++) {
        progressBar->setValue(i);

        //получаем из списка очередной товар и рисуем на него ценник
        currentTovar = spisok.at(i);
        currentTovar.name_of_tovar = currentTovar.name_of_tovar.toUtf8();

        int sName = currentTovar.shablon;
        QString shbl =  shablonList.at(sName);
        QDomElement sbl;
        if (currShablonFileName != shbl) {
             sbl = read_file_shablon(shbl).documentElement();
        } else {
            sbl = domDoc.documentElement();
        }

        cennics->create(&currentTovar, sbl);
        QGraphicsItemGroup *items = cennics->render(currentScene, bXpos, bYpos);
        oldX = bXpos;
        oldY = bYpos;

        if (cennics->lastCorner().y() > maxYadd) {
            oldYadd = maxYadd;
            maxYadd = cennics->lastCorner().y();
            maxYaddPos = i;
        }

            //проверим - не начать ли новую строчку
        if (cennics->lastCorner().x() + bXpos <= zoomedPageW*(pages.count())) {
                bXpos  += cennics->lastCorner().x();
            } else {
            if (maxYaddPos == i) {
                maxYadd = oldYadd;
            }
            //if (maxYadd < oldYadd) maxYadd = oldYadd;

            items->setPos(bXstart-bXpos, max(maxYadd, oldYadd));
            bXpos = bXstart + cennics->lastCorner().x();
            bYpos += max(maxYadd, oldYadd);
            oldYadd = 0;

            if (bYpos >= zoomedPageH - otstupTop - maxYadd) {
                //начали новую строчку - а вдруг надо новую страницу?

                //начинаем новую страницу
                if (pages.count() == 1) {
                    view->fitInView(page, Qt::KeepAspectRatio);
                }
                QRectF page(zoomedPageW*pages.count()-1, 0,
                            zoomedPageW, zoomedPageH);
                pages << page;
                bXstart += zoomedPageW;
                bXpos = bXstart;
                //bYstart не меняется - делаем все странички в строчку
                items->setPos(zoomedPageW*(pages.count()-1)-oldX + zazor, -oldY+bYstart);

                bYpos = bYstart;


                bXpos += cennics->lastCorner().x();
                }
                maxYadd = cennics->lastCorner().y();

        }
    }

    delete cennics;
    statusBar->removeWidget(progressBar);
    previewed =  true;
    ui_statusLabel->setText(tr("Result: ") + QString::number(pages.count()) + tr(" pages"));
}

void cengen::on_action_scaleUp_triggered()
{
    view->scale(2, 2);
    zoom = zoom*2.0;
}

void cengen::on_action_scaleDown_triggered()
{
    view->scale(0.5, 0.5);
    zoom = zoom/2.0;
}

void cengen::on_action_print_triggered()
{
    if (!previewed) generate_preview();

    QPrinter printer(QPrinter::HighResolution);

    if (pageSize == "A4") {
        printer.setPageSize(QPrinter::A4);
    }
    if (pageSize == "A5") {
        printer.setPageSize(QPrinter::A5);
    }

    if (paperOrientation == "landscape") {
        printer.setOrientation(QPrinter::Landscape);
    } else {
        printer.setOrientation(QPrinter::Portrait);
    }

    printer.setFullPage(true);

    printer.setNumCopies(1);

    printer.setColorMode(QPrinter::GrayScale);

    QPrintDialog dlg (&printer, this);
    dlg.setMinMax(1, pages.count());
    dlg.setFromTo(1, pages.count());

    if (dlg.exec() == QDialog::Accepted)
    {
        printer.setColorMode(QPrinter::GrayScale);
        QPainter painter(&printer);
        QRectF region = printer.pageRect();

        int min, max;
        min = dlg.fromPage();
        max = dlg.toPage();

        if (!max) max = pages.count();

        if (min > pages.count()) {
            min--;
        }

        if (min>0) min--;
        if (max <= pages.count()) max--;

        for (int i = min; i<= max; i++) {
            QRect current = pages.at(i).toRect();
            currentScene->render(&painter, region, current, Qt::KeepAspectRatio);
            if (i<max) {
                printer.newPage();
            }

        }

    }
}

void cengen::writeSettings()
{
    m_settings.beginGroup("/Settings");
    if (this->db_source) {
        m_settings.setValue("/source", 1);
        //если 1 - ищем в мускуле, если 0 - в DBF
        if (db_is_ready) {
            m_settings.beginGroup("/sql");
            m_settings.setValue("/host", sql->dbHost);
            m_settings.setValue("/dbName", sql->dbName);
            m_settings.setValue("/pass", sql->dbPass);
            m_settings.setValue("/user", sql->dbUser);
            m_settings.setValue("/tbName", sql->tbName);

            //qDebug() << "saving TABLE " << sql->tbName;
            m_settings.endGroup();
            qDebug() << "sql config written";
        }
    } else {
        m_settings.setValue("/source", 0);
        m_settings.beginGroup("/dbf");
        m_settings.setValue("/filename", this->dbf->fileName);
        m_settings.endGroup();
    }

    m_settings.setValue("/shablon", currShablonFileName);

    m_settings.setValue("/SearchLimit", ui_spinLimit->value());

    if (describer_is_ready) {
        m_settings.beginGroup("/describer");
        m_settings.setValue("/tnomer", config_index[0]);
        m_settings.setValue("/tname", config_index[1]);
        m_settings.setValue("/tbarcode", config_index[2]);
        m_settings.setValue("/tprice", config_index[3]);
        //qDebug() << "writing index of 3 = " << config_index[3];
        m_settings.endGroup();
        //qDebug() << "desc written";
    }

    //записываем настройки фильтра
    m_settings.setValue("/filterIsOn", filter_is_on);
    if (filter_is_on) {

        m_settings.beginGroup("/filter");

        //записываем название файла
        m_settings.setValue("/FileName", filterDbf.fileName);

        //следующие два поля - это название полей из предыдушего файла
        m_settings.setValue("/WhatBox", ui_filterWhatBox->currentIndex());
        m_settings.setValue("/WhereBox", ui_filterWhereBox->currentIndex());

        //строка поиска
        m_settings.setValue("/LineText", ui_filterLineText->text());

        //ключ поиска - товарный номер, штрихкод и т.п. (в виде индекса)
        m_settings.setValue("/WhatToFoundBox", ui_filterWhatToFoundBox->currentIndex());

        //метод фильтрации - равно, больше, меньше и т.п. (в виде индекса)
        m_settings.setValue("/Method", ui_filterMethodBox->currentIndex());

        m_settings.setValue("/CheckIn", filterCheckInBox->currentIndex());

        m_settings.endGroup();

    }

    //сохраняем настройки о файле продаж
    m_settings.beginGroup("/Sell");
    m_settings.setValue("sellFileName", sellFileName);
    m_settings.setValue("sellFileFields", sellOpisateli);
    m_settings.setValue("nomer", sellNomerBox->currentIndex());
    m_settings.setValue("date", sellDateBox->currentIndex());
    m_settings.setValue("time", sellTimeBox->currentIndex());
    m_settings.setValue("kol", sellKolBox->currentIndex());
    m_settings.setValue("lastDate", last_known_date);
    m_settings.setValue("lastPos", last_known_pos);
    m_settings.setValue("lastFile", last_known_file);
    m_settings.endGroup();

    //указываем текущую вкладку
    m_settings.setValue("tabIndex", ui_tabWidget->currentIndex());

    //сохранение параметров отображения окна
    m_settings.beginGroup("/Window");
    m_settings.setValue("geometry", geometry());
    m_settings.endGroup();

    int mainTableCount = tableWidget->horizontalHeader()->count();
    m_settings.setValue("mainTableCount", mainTableCount);
    m_settings.beginGroup("/mainTable");
    mainTableWidth = tableWidget->width();
    m_settings.setValue("tabWidth", mainTableWidth);
    for (int i=0; i<mainTableCount; i++) {
        m_settings.setValue("tab"+QString::number(i), tableWidget->get_tableTab_width(i));
    }
    m_settings.endGroup();

    m_settings.setValue("extAppConf", ext_conf_edit->text());
    m_settings.setValue("extAppName", ext_prog_name_edit->text());
    m_settings.setValue("extShablon", ext_shablon_name_edit->text());

    //закрываем вообще settings
    m_settings.endGroup();

}

void cengen::readSettings() {
    //устанавливаем геометрию окна
    QRect rect = m_settings.value("/Settings/Window/geometry", QRect(0, 20, 950, 500)).toRect();
    resize(rect.size());
    QRect r = this->geometry();
    r.moveCenter(QApplication::desktop()->availableGeometry().center());
    setGeometry(r);

    int mainTableCount = m_settings.value("/Settings/mainTableCount", 0).toInt();

    //mainTableTabs.clear();
    for (int i = 0; i<mainTableCount; i++) {
        tableWidget->set_tableTab_width(i, m_settings.value("/Settings/mainTable/tab"+QString::number(i), 10).toInt());
    }
    //set_tableWidget_header(tableWidget);

    //добавить, если там нет названия шаблона
    on_shablon_name_changed(m_settings.value("/Settings/shablon", "").toString());

    //создаем новые переменные для работы
    this->dbf = new DbfConfig;
    this->sql = new SqlConfig;

    //читаем источник данных (по умолчанию - DBF)
    this->db_source = m_settings.value("/Settings/source", "0").toInt();

    if (db_source) {
        //если 1 - ищем в мускуле
        //ui_radioButton->setChecked(true);
        //ui_radioButton_4->setChecked(true);

        //блокируем блок выбора файла DBF и включаем блок настроек MySQL
        trigger_source_selection(false);

        //читаем настройки базы данных
        //m_settings.beginGroup("/Settings/sql");
        sql = new SqlConfig;
        sql->dbHost = m_settings.value("/Settings/sql/host", "").toString();
        sql->dbName = m_settings.value("/Settings/sql/dbName", "").toString();
        sql->dbUser = m_settings.value("/Settings/sql/user", "").toString();
        sql->dbPass = m_settings.value("/Settings/sql/pass", "").toString();
        sql->tbName = m_settings.value("/Settings/sql/tbName", "").toString();
        //m_settings.endGroup();

        //передаем эти настройки в информер
        this->db_is_ready = my_informer->prepare(sql);

        if (db_is_ready) {
            //если база данных открывается
            //читаем список таблиц из базы данных
            update_ui_connection_established();

            if (sql->tbName != "") {
                //если в настройках - не пустое имя таблицы
                my_informer->set_tb_name(sql->tbName);
                this->opisateli.clear();
                opisateli = my_informer->tb_describe(sql->tbName);


            } else {
                //устанавливаем фокус на выборе таблицы
            }

        } else {
            //если база данных не открывается -
            //  блокируем остальные контролы,
            //  устанавливаем фокус ввода на настройках базы данных

            return;
        }


    } else {
        //значит, db_source = 0
        //и мы используем DBF-файлы
        //qDebug() << "we read DBF settings";
        my_informer->prepare(dbf);
        trigger_source_selection(true);

        //читаем имя файла DBF-базы данных
        //m_settings.beginGroup("/Settings/dbf");
        this->dbf->fileName = m_settings.value("/Settings/dbf/filename", "").toString();

        if (dbf->fileName != "") {
            //если имя файла не пусто - продолжаем
            my_informer->set_tb_name(dbf->fileName);
            labelDBFname->setText(dbf->fileName);
            this->opisateli.clear();
            opisateli = my_informer->tb_describe("DBF");

            //вызываем функцию выбора и установки описателей
            this->update_ui_db_controls();

        } else {
            //если получено пустое имя файла -
            //заблокировать дальнейшие контролы
            //и установить фокус ввода на выборе имени файла
            //ui_gridLayout_2;
            qDebug() << "we read DBF settings. File is empty";

            return;
        }


    }

    this->set_opisateli_from_settings();
    this->update_ui_db_controls();

    //Читаем настройки фильтра
    filter_is_on = m_settings.value("/Settings/filterIsOn", "false").toBool();
    if (filter_is_on) {
        //если фильтр включен - включаем контрол в UI и читаем настройки
        //ui_filterBox->setChecked(true);
        on_filterBox_toggled(true);
        //this->read_filter_settings();
    } else {
        //если в конфиге фильтр выключен - выключем его в UI на всякий случай
        //ui_filterBox->setChecked(false);
        on_filterBox_toggled(false);
    }

    //читаем информацию о файле продаж
    sellFileName = m_settings.value("/Settings/Sell/sellFileName", "").toString();
    updateSellTab();
    sellOpisateli = m_settings.value("/Settings/Sell/sellFileFields", "").toStringList();

    sellNomerBox->addItems(sellOpisateli);
    sellDateBox->addItems(sellOpisateli);
    sellTimeBox->addItems(sellOpisateli);
    sellKolBox->addItems(sellOpisateli);

    sellNomerBox->setCurrentIndex(m_settings.value("/Settings/Sell/nomer", "0").toInt());
    sellDateBox->setCurrentIndex(m_settings.value("/Settings/Sell/date", "0").toInt());
    sellTimeBox->setCurrentIndex(m_settings.value("/Settings/Sell/time", "0").toInt());
    sellKolBox->setCurrentIndex(m_settings.value("/Settings/Sell/kol", "0").toInt());
//    m_settings.setValue("lastDate", last_known_date);
//    m_settings.setValue("lastPos", last_known_pos);
//    m_settings.setValue("lastFile", last_known_file);
    last_known_file = m_settings.value("/Settings/Sell/lastFile", "").toString();
    if (last_known_file == sellFileName) {
        last_known_date = m_settings.value("/Settings/Sell/lastDate", "").toDate();
        last_known_pos = m_settings.value("/Settings/Sell/lastPos", "").toInt();
    } else {
        last_known_date = QDate::currentDate().addYears(-50);
        last_known_pos = 0;
    }

    //читаем номер текущей вкладки (по умолчанию - вкладка с источником данных)
    int ind = m_settings.value("/Settings/tabIndex", "3").toInt();
    if (ind > ui_tabWidget->count()-1) {
        ind = 0;
    }
    if (ind == TabsOrder::Preview) ind = TabsOrder::Search;
    ui_tabWidget->setCurrentIndex(ind);

    ui_spinLimit->setMaximum(my_informer->get_maximum());
    ui_spinLimit->setValue(m_settings.value("/Settings/SearchLimit", "10").toInt());

    m_settings.setValue("extAppConf", select_ext_conf_file->text());

    ext_conf_edit->setText(m_settings.value("/Settings/extAppConf", "").toString());
    ext_prog_name_edit->setText(m_settings.value("/Settings/extAppName", "").toString());
    ext_shablon_name_edit->setText(m_settings.value("/Settings/extShablon", "").toString());


}

void cengen::read_filter_settings() {
    //читаем название файла фильтра и обновляем UI по необходимости
    filterDbf.fileName = m_settings.value("/Settings/filter/FileName", "").toString();
    if (filterDbf.fileName == "") {
        this->file_is_ready = false;
        ui_statusLabel->setText(tr("Wrong shablon"));
        return;
    }

    on_filterFileName_changed();

    ui_filterWhatBox->setCurrentIndex(m_settings.value("/Settings/filter/WhatBox", "0").toInt());
    ui_filterWhereBox->setCurrentIndex(m_settings.value("/Settings/filter/WhereBox", "0").toInt());

    ui_filterMethodBox->setCurrentIndex(m_settings.value("/Settings/filter/Method", "0").toInt());

    ui_filterLineText->setText(m_settings.value("/Settings/filter/LineText", "").toString());

    ui_filterWhatToFoundBox->setCurrentIndex(m_settings.value("/Settings/filter/WhatToFoundBox", "0").toInt());

    filterCheckInBox->setCurrentIndex(m_settings.value("/Settings/filter/CheckIn", "0").toInt());

}

void cengen::on_connectMySQLButton_clicked()
{
    //подготавливаем соединение с базой данных
    sql = new SqlConfig;
    sql->dbHost = ui_lineHost->text();
    sql->dbName = ui_lineName->text();
    sql->dbUser = ui_lineUser->text();
    sql->dbPass = ui_linePass->text();
    qDebug() << "prepared";

    if (my_informer->prepare(sql)) {
        update_ui_connection_established();
        //my_informer->tb_describe("test");
    }
    ui_spinLimit->setMaximum(my_informer->get_maximum());
}

void cengen::on_mysql_table_selector_comboBox_activated(QString tbName)
{

    QStringList list = my_informer->tb_describe(tbName);
    this->update_ui_tb_fields(list);

    my_informer->set_tb_name(tbName);
    //ui_save_db_config_button->setEnabled(true);
    this->sql->tbName = tbName;
    this->db_is_ready = true;


}

void cengen::update_ui_tb_fields(QStringList list) {
    //обновляем пользовательские контролы для выбора в таблице цены, названия и т.п.
    ui_comboTnomer->clear();
    ui_comboTname->clear();
    ui_comboTbarcode->clear();
    ui_comboTprice->clear();

    ui_comboTnomer->addItems(list);
    ui_comboTname->addItems(list);
    ui_comboTbarcode->addItems(list);
    ui_comboTprice->addItems(list);

}

void cengen::on_opisateli_updated()
{
    //сохраняем, если возможно, полученные настройки для подключения к базе

    config.tnomer = ui_comboTnomer->currentText();
    config.tname = ui_comboTname->currentText();
    config.tbarcode = ui_comboTbarcode->currentText();
    config.tprice = ui_comboTprice->currentText();

    config_index[0] = ui_comboTnomer->currentIndex();
    config_index[1] = ui_comboTname->currentIndex();
    config_index[2] = ui_comboTbarcode->currentIndex();
    config_index[3] = ui_comboTprice->currentIndex();

    my_informer->set_fields(&config);
    this->describer_is_ready = true;

    qDebug() << "Save db headers. 3 is " << config_index[3];
}

void cengen::update_ui_db_controls() {


    if (db_is_ready && this->db_source) {
        ui_lineHost->setText(sql->dbHost);
        ui_lineName->setText(sql->dbName);
        ui_lineUser->setText(sql->dbUser);
        ui_linePass->setText(sql->dbPass);
        this->update_ui_connection_established();

    }

    if (this->describer_is_ready) {
        if (this->db_source) {
            this->on_mysql_table_selector_comboBox_activated(sql->tbName);
        }
        ui_comboTnomer->blockSignals(true);
        ui_comboTname->blockSignals(true);
        ui_comboTbarcode->blockSignals(true);
        ui_comboTprice->blockSignals(true);

        this->ui_comboTnomer->setCurrentIndex(config_index[0]);
        this->ui_comboTname->setCurrentIndex(config_index[1]);
        this->ui_comboTbarcode->setCurrentIndex(config_index[2]);
        this->ui_comboTprice->setCurrentIndex(config_index[3]);

        ui_comboTnomer->blockSignals(false);
        ui_comboTname->blockSignals(false);
        ui_comboTbarcode->blockSignals(false);
        ui_comboTprice->blockSignals(false);

    }
}

void cengen::update_ui_connection_established() {
    QStringList tables = my_informer->db_describe();
    ui_mysql_table_selector_comboBox->clear();
    ui_mysql_table_selector_comboBox->addItems(tables);
    ui_mysql_table_selector_comboBox->setEnabled(true);

}

void cengen::get_method_from_ui()
{
    if (ui_radioButton_1->isChecked()) {
        method= "tbarcode";
        ui_lineEdit->setValidator(ui_bvalidator);
    }
    if (ui_radioButton_2->isChecked()) {
        method= "tnomer";
        ui_lineEdit->setValidator(ui_nvalidator);
        //ui_lineEdit->setValidator(ui_bvalidator);
    }
    if (ui_radioButton_3->isChecked()) {
        method= "tname";
        ui_lineEdit->setValidator(ui_svalidator);
    }
    if (ui_radioButton_5->isChecked()) {
        method= "any";
        ui_lineEdit->setValidator(ui_svalidator);
    }
    tableWidget->set_tovar_searched(false);
    new_line_ready();
}

void cengen::on_source_changed_toMySQL()
{
    //устанавливаем метод поиска через сетевую MySQL-базу данных
    this->db_source = 1;
    m_settings.sync();
    m_settings.beginGroup("/Settings");
    m_settings.setValue("/source", 1);
    m_settings.endGroup();
    m_settings.sync();
    this->readSettings();

    trigger_source_selection(false);
}

void cengen::on_source_changed_toDBF()
{
    //устанавливаем метод поиска товара через чтение локального DBF-файла
    this->db_source = 0;
    m_settings.beginGroup("/Settings");
    m_settings.setValue("/source", 0);
    m_settings.endGroup();
    this->readSettings();

    trigger_source_selection(true);
}

void cengen::trigger_source_selection(bool state) {
    ui_groupDBF->blockSignals(true);
    ui_groupSQL->blockSignals(true);
    ui_groupDBF->setChecked(state);
    ui_groupSQL->setChecked(!state);
    ui_groupDBF->blockSignals(false);
    ui_groupSQL->blockSignals(false);

}

void cengen::on_selectSourceDBF_file_button_clicked()
{
    //выбор DBF-файла
    QString str = QFileDialog::getOpenFileName(0, "Open Dialog", "", "DBF-files (*.dbf)");
    if (str == "") return;
    labelDBFname->setText(str);
    //this->dbf = new DbfConfig;
    dbf->fileName = str;
    my_informer->set_tb_name(str);
    QStringList list = my_informer->tb_describe(str);
    this->update_ui_tb_fields(list);
    ui_spinLimit->setMaximum(my_informer->get_maximum());

}

bool cengen::read_opisateli_from_settings() {
    //m_settings.beginGroup("/Settings/describer");
    config_index[0] = m_settings.value("/Settings/describer/tnomer", -1).toInt();
    config_index[1] = m_settings.value("/Settings/describer/tname", -1).toInt();
    config_index[2] = m_settings.value("/Settings/describer/tbarcode", -1).toInt();
    config_index[3] = m_settings.value("/Settings/describer/tprice", -1).toInt();
    //m_settings.endGroup();
    if (config_index[0] == -1 ||
            config_index[1] == -1 ||
            config_index[2] == -1 ||
            config_index[3] == -1
            ) {
        return false;
    }
    return true;


}

void cengen::set_opisateli_from_settings()
{
    if (!this->db_source && !opisateli.count()) {
        //если в списке ноль полей - выбираем файл
        qDebug() << "we read DBF settings. Zero file";

        return;
    }
    this->db_is_ready = true;
    //вызываем функцию установки описателей на пользователький интерфейс
    update_ui_tb_fields(opisateli);

    //читаем настройки, что где в этой базе данных
    describer_is_ready = read_opisateli_from_settings();
    if (describer_is_ready) {
        //qDebug() << "we read DBF settings. Set fields";

        config.tnomer = opisateli.at(config_index[0]);
        config.tname = opisateli.at(config_index[1]);
        config.tbarcode = opisateli.at(config_index[2]);
        config.tprice = opisateli.at(config_index[3]);
        my_informer->set_fields(&config);
        return;

    }
}

void cengen::on_spinLimit_valueChanged(int number)
{
    //qDebug() << "limit = " << number;
    my_informer->set_limit_search(number);
}

void cengen::on_maxButton_clicked()
{
    //нажата кнопка выбора лимита поиска без ограничений
    ui_spinLimit->setValue(ui_spinLimit->maximum());
}

void cengen::on_show_editor_button_clicked()
{
    if (file_is_ready) {
        //подготовка редактора шаблонов
        shablon_editor = new editor(this);

        shablonElement = domDoc.documentElement();
        shablon_editor->load_xml_data_into_editor(&shablonElement);
        shablon_editor->set_file_name(currShablonFileName);

        shablon_editor->resize(this->size());

        shablon_editor->exec();

        domDoc.clear();
        domDoc = shablon_editor->get_new_shablon();
        currShablonFileName = shablon_editor->get_new_fileName();
        //file.setFileName(str);


        this->describe_shablon(domDoc);

        delete(shablon_editor);
    }
}

void cengen::describe_shablon(QDomDocument shablon) {

    domDoc = shablon;
    this->shablonElement = domDoc.documentElement();
    //qDebug() << "SHABLON\n" << domDoc.toString();
    rectCen = this->get_shablon_rect(shablonElement);
    ui_label->setText(get_shablon_name(shablonElement));
    this->update_values();
    //this->shablonElement = shablon;
}

QDomDocument cengen::read_file_shablon(QString str) {
    //QDomElement domElement;
    QDomDocument doc;
    QFile * fileSh = new QFile;
    fileSh->setFileName(str);
    if (fileSh->open(QIODevice::ReadOnly)) {
        if (doc.setContent(fileSh)) {
            file_is_ready = true;
            fileSh->close();
        }
    } else {
        file_is_ready = false;
        ui_statusLabel->setText(tr("Cannot open shablon"));

    }
    return doc;
}

void cengen::on_action_save_triggered() {
    QList<Tovar> spisok= tableWidget->get_tovar_list("x");
    this->save_tovar_list_into_file("", spisok);
}

void cengen::save_tovar_list_into_file(QString preferName, QList<Tovar> spisok)
{
    //Сохранение списка товаров
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save tovar list"), preferName , tr("Tovar lists (*.tov)"));

    if (fileName != "") {
        bool ckeck = false;
        int ind = fileName.lastIndexOf(".tov", -1, Qt::CaseInsensitive);
        if (ind + 4 != fileName.length() || ind == -1) {
            fileName += ".tov";
            ckeck = true;
        }

        QFile file(fileName);
        if (ckeck && file.exists()) {
            ListFoundedItemsDialog* dlg = new ListFoundedItemsDialog(this);
            dlg->setMessage(tr("File exist. Rewrite?"));
            if (dlg->exec()) {
                ckeck = false;
            } else {
                //приделать повторный выбор имени файла
                return;
            }
        } else {
            ckeck = false;
        }

        if(!ckeck && file.open(QIODevice::WriteOnly)) {
            QDomDocument doc;
            doc = this->convert_tovar_list_into_xml(spisok);

            QString shablon_array = doc.toString();
            QTextStream out(&file);
            out.setCodec("UTF-8");
            out << shablon_array;
            file.close();
        }
    }
}

void cengen::on_action_open_triggered()
{
    qDebug() << "Load tovar list selected";
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open tovar list"), "", tr("Tovar lists (*.tov)"));
    if (fileName == "") {
        qDebug() << "Please select file name";
        return;
    }
    open_tovar_list(fileName);
}
void cengen::open_tovar_list(QString fileName) {
    QDomDocument doc;
    QFile file;
    file.setFileName(fileName);

    if (file.open(QIODevice::ReadOnly)) {
        if (doc.setContent(&file)) {
            QList<Tovar> tovarList;
            tovarList = convert_xml_into_tovar_list(doc);
            tableWidget->load_tovar_list_into_table(tovarList, false);

        }
    } else {
        //error что файл не открывается
        qDebug() << "cant open file: " << fileName;
        return;
    }

}

QList<Tovar> cengen::convert_xml_into_tovar_list(QDomDocument doc) {

    QDomElement domElement = doc.documentElement();
    QList<Tovar> tovarList;

    //раз файл открылся - переводим xml в TovarList

    QDomNode node = domElement.firstChild();
    while (!node.isNull()){
        if (node.isElement()) {
            QDomElement item = node.toElement();
            Tovar tovarItem;
            tovarItem.quantity = 0;
            if (item.tagName() == "tovarItem") {
                QDomNode nodeItem = node.firstChild();
                QString itemName;
                QString itemValue;
                while (!nodeItem.isNull()) {
                    if (nodeItem.isElement()) {
                        QDomElement elementItem = nodeItem.toElement();
                        if (!elementItem.isNull()) {
                            itemName = elementItem.tagName();
                            itemValue = elementItem.text();
                            if (itemName == "name") {
                                tovarItem.name_of_tovar = itemValue;
                            }
                            if (itemName == "barcode") {
                                tovarItem.barcode = itemValue;
                            }
                            if (itemName == "price1") {
                                tovarItem.price1 = itemValue.toFloat();
                            }
                            if (itemName == "price2") {
                                tovarItem.price2 = itemValue.toFloat();
                            }
                            if (itemName == "tnomer") {
                                tovarItem.nomer_of_tovar = itemValue.toInt();
                            }
                            if (itemName == "quantity") {
                                tovarItem.quantity = itemValue.toInt();
                            }

                        }
                    }
                    nodeItem = nodeItem.nextSibling();
                }

                tovarList << tovarItem;
            }

        }
        node = node.nextSibling();
    }
    return tovarList;

}

QDomDocument cengen::convert_tovar_list_into_xml(QList<Tovar> spisok) {
    QDomDocument doc;

    QDomNode xmlNode = doc.createProcessingInstruction("xml",
                                    "version=\"1.0\" encoding=\"UTF-8\"");
    doc.insertBefore(xmlNode, doc.firstChild());

    QDomElement domList = doc.createElement("tovarList");


    for (int i = 0; i<spisok.count(); i++) {
        QDomElement tovarElement = doc.createElement("tovarItem");

        QDomElement nameElement = doc.createElement("name");
        QDomText dm1 = doc.createTextNode(spisok.at(i).name_of_tovar);
        nameElement.appendChild(dm1);
        tovarElement.appendChild(nameElement);

        QDomElement priceElement = doc.createElement("price1");
        QDomText dm2 = doc.createTextNode(QString::number(spisok.at(i).price1));
        priceElement.appendChild(dm2);
        tovarElement.appendChild(priceElement);

        QDomElement price2Element = doc.createElement("price2");
        QDomText dm3 = doc.createTextNode(QString::number(spisok.at(i).price2));
        price2Element.appendChild(dm3);
        tovarElement.appendChild(price2Element);

        QDomElement tnomerElement = doc.createElement("tnomer");
        QDomText dm4 = doc.createTextNode(QString::number(spisok.at(i).nomer_of_tovar));
        tnomerElement.appendChild(dm4);
        tovarElement.appendChild(tnomerElement);

        QDomElement quantityElement = doc.createElement("quantity");
        QDomText dm6 = doc.createTextNode(QString::number(spisok.at(i).quantity));
        quantityElement.appendChild(dm6);
        tovarElement.appendChild(quantityElement);

        QDomElement barcodeElement = doc.createElement("barcode");
        QDomText dm5 = doc.createTextNode(spisok.at(i).barcode);
        barcodeElement.appendChild(dm5);
        tovarElement.appendChild(barcodeElement);

        domList.appendChild(tovarElement);
    }

    doc.appendChild(domList);
    return doc;
}

void cengen::on_action_new_triggered()
{
    ui_countLabel->setText(tr("COUNT: ") + "0");
    tableWidget->setRowCount(0);
    //action_print->setEnabled(false);
    currentScene->clear();
    ui_tabWidget->setCurrentIndex(TabsOrder::Search);

}

void cengen::on_action_minus_triggered()
{
    //функция вычитания списка
    //из существующего списка убираем позиции, которые есть в загружаемом списке
    //сверку ведем по товарному номеру

    qDebug() << "Tovar list minus";

    QString fileName = QFileDialog::getOpenFileName(this, tr("Select tovar list for minus"), "", tr("Tovar lists (*.tov)"));
    if (fileName == "") {
        qDebug() << "Please select file name";
        return;
    }

    QDomDocument doc;

    QFile file;
    file.setFileName(fileName);

    if (file.open(QIODevice::ReadOnly)) {
        if (doc.setContent(&file)) {
            QList<Tovar> newTovarList, oldTovarList, finalList;
            newTovarList = convert_xml_into_tovar_list(doc);
            oldTovarList = tableWidget->get_tovar_list("x");

            finalList = minus(oldTovarList, newTovarList);

            tableWidget->setRowCount(0);
            tableWidget->load_tovar_list_into_table(finalList);

        }
    } else {
        //error что файл не открывается
        qDebug() << "cant open file";
        return;
    }
}

QList<Tovar> cengen::minus(QList<Tovar> oldList, QList<Tovar> newList) {
    QList<Tovar> final;

    final = oldList;
    int k = 0;

    for (int i = 0; i<newList.count(); i++) {

        for (int j = 0; j<oldList.count(); j++) {
            if (newList.at(i).nomer_of_tovar == oldList.at(j).nomer_of_tovar) {
                final.removeAt(j-k);
                qDebug() << "j and k = " << j << k;
                k++;
            }
        }

    }

    return final;
}

void cengen::on_filterBox_toggled(bool arg1)
{
    action_on_off_filter->blockSignals(true);
    ui_filterBox->blockSignals(true);
    action_on_off_filter->setChecked(arg1);
    ui_filterBox->setChecked(arg1);
    action_on_off_filter->blockSignals(false);
    ui_filterBox->blockSignals(false);

    filter_is_on = arg1;
    apply_filter_on_current_list->setEnabled(arg1);
    if (arg1) {
        this->read_filter_settings();
    }
}

void cengen::turn_filter_ON() {
    on_filterBox_toggled(true);
}

void cengen::on_filterFileSelectButton_clicked()
{
    //выбор DBF-файла
    QString str = QFileDialog::getOpenFileName(0, "Select FILTER file", "", "DBF-files (*.dbf)");
    if (str == "") return;

    filterDbf.fileName = str;

    on_filterFileName_changed();

}

void cengen::on_filterFileName_changed() {
    //изменилось имя файла для фильтра - подготовливаем Tinformer и обновляем UI
    filterInformer->prepare(&filterDbf);
    filterInformer->set_tb_name(filterDbf.fileName);
    QStringList list = filterInformer->tb_describe(filterDbf.fileName);

    ui_filterWhatBox->clear();
    ui_filterWhatBox->addItems(list);

    ui_filterWhereBox->clear();
    ui_filterWhereBox->addItems(list);

    filterCheckInBox->clear();
    filterCheckInBox->addItems(list);

    label18a->setText(filterDbf.fileName);
}

QList<Tovar> cengen::apply_filter(QList<Tovar> inputList) {
    QList<Tovar> filteredList;

    bool delete_filtered = delete_filtered_box->isChecked();

    filterConfig.tnomer = ui_filterWhereBox->currentText();
    filterConfig.tprice = ui_filterWhereBox->currentText();
    filterConfig.tname = ui_filterWhereBox->currentText();
    //filterConfig.tbarcode = ui_filterWhereBox->currentText();
    filterConfig.tbarcode = filterCheckInBox->currentText();

    int WhatToFound = ui_filterWhatToFoundBox->currentIndex();
    //qDebug() << "What to found is " << WhatToFound;
    switch (WhatToFound) {
    case 0:
        filterConfig.tnomer = ui_filterWhatBox->currentText();
        break;
    case 1:
        filterConfig.tbarcode = ui_filterWhatBox->currentText();
        break;
    case 2:
        filterConfig.tname = ui_filterWhatBox->currentText();
        break;
    case 3:
        filterConfig.tprice = ui_filterWhatBox->currentText();
        break;

    }

    filterInformer->set_fields(&filterConfig);

    filterInformer->set_limit_search(1);

    int method = ui_filterMethodBox->currentIndex();
    QString search = ui_filterLineText->text();
    bool itemfound;

    Tovar tovarItem, tovarFound;

    bool ckeckOut = filBoxCheck->isChecked();

    //поиск еще не оптимален - иногда он бывает долгим
    progressBar->setMaximum(inputList.count());
    statusBar->addWidget(progressBar);
    progressBar->show();

    for (int i = 0; i<inputList.count(); i++) {
        progressBar->setValue(i);
        tovarItem = inputList.at(i);

        QString searchText, searchMethod;
        bool FastSearch = false;
        //в зависимости от выбранного ищем по номеру, цене и т.п.
        switch (WhatToFound) {
        case 0:
            searchText = QString::number(tovarItem.nomer_of_tovar);
            searchMethod = "tnomer";
            FastSearch = true;
            break;
        case 1:
            searchText = tovarItem.barcode;
            searchMethod = "tbarcode";
            break;
        case 2:
            searchText = tovarItem.name_of_tovar;
            searchMethod = "tname";
            break;
        case 3:
            searchText = QString::number(tovarItem.price1);
            searchMethod = "tprice";
        default:
            searchText = tovarItem.nomer_of_tovar;
            searchMethod = "tnomer";
            break;
        }
        //qDebug() << "searching " << searchText << searchMethod;
        QList<Tovar> foundList = filterInformer->info(searchText, searchMethod, 0, -1, 1, true, FastSearch);
        itemfound = false;
        if (foundList.count()) {
            tovarFound = foundList.at(0);
            //проверяем, используя выбранный метод сравнения
            switch (method) {
            case 0:
                if (tovarFound.price1 == search.toFloat()) itemfound = true;
                break;
            case 1:
                if (tovarFound.price1 != search.toFloat()) itemfound = true;
                break;
            case 2:
                if (tovarFound.price1 > search.toFloat()) itemfound = true;
                break;
            case 3:
                if (tovarFound.price1 < search.toFloat()) itemfound = true;
                break;
            case 4:
                if (tovarFound.name_of_tovar.contains(searchText, Qt::CaseInsensitive)) itemfound = true;
            default:
                break;
            }
            if (ckeckOut) {
                //qDebug() << "count << " << tovarFound.barcode;
                tovarItem.quantity = tovarFound.barcode.toFloat();
                //qDebug() << "count << " << tovarItem.quantity;

            }
            if (!delete_filtered && !itemfound) {
                filteredList << tovarItem;
            }
            if (itemfound) filteredList <<tovarItem;
        } else {
            if (!delete_filtered) {
                filteredList << tovarItem;
            }
        }
    }

    statusBar->removeWidget(progressBar);

    return filteredList;
}

void cengen::on_radioButton_6_clicked()
{
    this->paperOrientation = "portrate";
    update_values();
}

void cengen::on_radioButton_7_clicked()
{
    this->paperOrientation = "landscape";
    update_values();
}

void cengen::on_action_about_triggered()
{
    About d (this);

    d.setAuthor(tr("Michael Volkov"));
    d.setMail("miha@mihol.ru");
    d.setSite("http://mihol.ru/cengen");
    d.setAuthorPhoto(QIcon(":/share/images/resources/my.png"));
    d.setPhone("+7-960-88-44-777");
    d.setLicense("GNU GPL v3");

    {
        QFileInfo fi(QApplication::applicationFilePath());
        fi.setFile(fi.absoluteDir(), "LICENSE.GPL3");
        QFile file(fi.absoluteFilePath());
        if (file.open(QFile::ReadOnly)) {
                d.setLicenseText(QString(file.readAll()));
                file.close();
        }
    }
    d.addThanks(tr("UG_Dvor_34"), "", tr ("Encouragement"));
    d.exec();
}

void cengen::on_apply_filter_on_current_list_triggered() {
    QList<Tovar> oldList = tableWidget->get_tovar_list("x");
    QList<Tovar> newList = apply_filter(oldList);
    this->on_action_new_triggered();
    tableWidget->load_tovar_list_into_table(newList);
}

void cengen::on_fieldListBox_checked(bool status) {
    QString str = ((QCheckBox*)sender())->text();
    //qDebug() << "Text checked " << str << status;
    //qDebug() << "Last val " << fullFieldsList[str];
    fullFieldsList[str] = status;
    //qDebug() << "Next val " << fullFieldsList[str];
    tableWidget->set_tableFields(fullFieldsList);


}

QList<Tovar> cengen::expand(QList<Tovar> list) {
    QList<Tovar> newList;
    Tovar tovar;
    int quant;
    for (int i = 0; i<list.count(); i++) {
        tovar = list.at(i);
        quant = tovar.quantity;
        tovar.quantity = 1;
        newList << tovar;
        if (quant>1) {
            for (int j=0; j<quant-1; j++) newList<<tovar;
        }
    }
    return newList;
}

void cengen::set_org_name(QString org, QString prog) {
    org_name = org;
    app_name = prog;
}

void cengen::on_interchange_prices_in_table_triggered() {
    tableWidget->interchange_prices_in_table();
    if (ui_tabWidget->currentIndex() == TabsOrder::Preview) {
        on_action_make_triggered();
    }
}

void cengen::on_intellect_interchange_prices_in_table_triggered() {
    tableWidget->intellect_interchange_prices_in_table();
    if (ui_tabWidget->currentIndex() == TabsOrder::Preview) {
        on_action_make_triggered();
    }
}

void cengen::on_action_verify_barcode() {
    Barcode *barc = new Barcode;
    QString text = ui_lineEdit->text();
    QString br = barc->found_lost_digit(text);
    ListFoundedItemsDialog* dlg = new ListFoundedItemsDialog(this);
    QStringList strs = br.split("\n");
    dlg->setMessage(tr("FOUND ") + QString::number(strs.count()-1)  + " items.\n" + tr("Do you want to search them in database?"));
    if (dlg->exec()) {
        QList<Tovar> tmp;

        progressBar->setMaximum(strs.count());
        statusBar->addWidget(progressBar);
        progressBar->show();

        for (int i = 0; i<strs.count()-1; i++) {
            progressBar->setValue(i);
            tmp = my_informer->info(strs.at(i), "tbarcode");
            if (tmp.count()) {
                tableWidget->load_tovar_list_into_table(tmp);
            }
        }
        statusBar->removeWidget(progressBar);
    }

}

void cengen::on_action_update_prices() {
    QList<Tovar> spisokCur, spisokNew, spisokWrong, spTmp;
    Tovar tovar;
    spisokCur = tableWidget->get_tovar_list("x");

    int count = spisokCur.count();
    float tmpPrice;

    progressBar->setMaximum(count);
    statusBar->addWidget(progressBar);
    progressBar->show();

    for (int i = 0; i<count; i++){
        progressBar->setValue(i);
        tovar = spisokCur.at(i);
        spTmp = my_informer->info(QString::number(tovar.nomer_of_tovar), "tnomer");
        if (!spTmp.count()) {
            tovar.name_of_tovar = tr("NOT FOUND ")
                    + tovar.name_of_tovar;
            spisokWrong << tovar;
        } else {
            tmpPrice = tovar.price1;
            tovar.price1 = spTmp.at(0).price1;
            spisokNew << tovar;

            if (tmpPrice != spTmp.at(0).price1) {
                tovar.name_of_tovar = tr("Price changed ") + tovar.name_of_tovar;
                tovar.price2 = tmpPrice;
                spisokWrong << tovar;
            }
        }


    }
    statusBar->removeWidget(progressBar);

    this->on_action_new_triggered();
    tableWidget->load_tovar_list_into_table(spisokNew);

    ask_user_to_save_wrong_tovar_list(spisokWrong);
}

void cengen::on_action_update_prices_in_minus() {
    QList<Tovar> spisokCur, spisokWrong, spTmp;
    Tovar tovar;
    spisokCur = tableWidget->get_tovar_list("x");

    int count = spisokCur.count();
    float tmpPrice;

    progressBar->setMaximum(count);
    statusBar->addWidget(progressBar);
    progressBar->show();

    for (int i = 0; i<count; i++){
        progressBar->setValue(i);
        tovar = spisokCur.at(i);
        spTmp = my_informer->info(QString::number(tovar.nomer_of_tovar), "tnomer");
        if (!spTmp.count()) {
            tovar.name_of_tovar = tr("NOT FOUND ")
                    + tovar.name_of_tovar;
            spisokWrong << tovar;
        } else {
            tmpPrice = tovar.price1;
            tovar.price1 = spTmp.at(0).price1;

            if (tmpPrice > spTmp.at(0).price1) {
                tovar.price2 = tmpPrice;
                spisokWrong << tovar;
            }
        }
    }
    statusBar->removeWidget(progressBar);

    this->on_action_new_triggered();
    tableWidget->load_tovar_list_into_table(spisokWrong);
}

void cengen::on_action_search_by_tnomer_in_clipboard_triggered() {
    QClipboard *pcb = QApplication::clipboard();
    QString str = pcb->text();
    if (!str.isNull()) {
        QStringList strs = str.split("\n");
        QString tmp;
        Tovar tovar;
        tovar.price1 = 0;
        tovar.price2 = 0;
        tovar.quantity = 0;
        QList<Tovar> tmpList, newList, spisokWrong;

        progressBar->setMaximum(strs.count());
        statusBar->addWidget(progressBar);
        progressBar->show();

        for (int i = 0; i<strs.count(); i++) {
            progressBar->setValue(i);
            tmp = QString::number(strs.at(i).toFloat());    //убрать все лишнее
            tmpList = my_informer->info(tmp, "tnomer");
            if (tmpList.count()) {
                newList << tmpList;
            } else {
                tovar.name_of_tovar = tr("NOT FOUND ") + strs.at(i);
                tovar.nomer_of_tovar = strs.at(i).toInt();
                spisokWrong << tovar;
            }
        }
        tableWidget->load_tovar_list_into_table(newList);
        statusBar->removeWidget(progressBar);

        ask_user_to_save_wrong_tovar_list(spisokWrong);
    }
}

void cengen::ask_user_to_save_wrong_tovar_list(QList<Tovar> spisokWrong) {
    if (spisokWrong.count()) {
        ListFoundedItemsDialog* dlg = new ListFoundedItemsDialog(this);
        dlg->setMessage(tr("FOUND ") + QString::number(spisokWrong.count())
                        + tr(" wrong tovars.") + "\n"
                        + tr("If you want to save them into file, click OK button"));
        if (dlg->exec()) {
            //сохраняем список
            QString name;
            QDate date = QDate::currentDate();
            name = date.toString("yyyy MMMM dddd (dd)") + ".tov";
            save_tovar_list_into_file(name, spisokWrong);
        }
    }
}

void cengen::on_action_load_tovar_list_in_clipboard_triggered(){
    QClipboard *pcb = QApplication::clipboard();
    QString str = pcb->text();
    if (str.isNull()) return;

    QStringList strs = str.split("\n");
    QString tmp;
    QStringList tmps;
    Tovar tovar;
    tovar.quantity = 0;
    QList<Tovar> newList;
    int i;

    progressBar->setMaximum(strs.count());
    statusBar->addWidget(progressBar);
    progressBar->show();

    for (int j = 0; j<strs.count(); j++) {
        progressBar->setValue(j);

        tmps =  strs.at(j).split("\t");
        i = tmps.count();
        tovar.nomer_of_tovar = tmps.at(0).toInt();
        if (i>1) {
            tovar.name_of_tovar = tmps.at(1);
        } else {
            tovar.name_of_tovar = "";
        }
        if (i>2) {
            tmp = tmps.at(2);
            if (tmp.contains(",")) tmp.replace(",", ".");
            tovar.price1 = tmp.toFloat();
        } else {
            tovar.price2 = 0;
        }
        if (i>3) {
            tmp = tmps.at(3);
            if (tmp.contains(",")) tmp.replace(",", ".");
            tovar.price2 = tmp.toFloat();
        } else {
            tovar.price2 = 0;
        }
        newList << tovar;
    }
    tableWidget->load_tovar_list_into_table(newList);
    statusBar->removeWidget(progressBar);
}

void cengen::on_action_update_names() {
    QList<Tovar> spisokCur, spisokNew, spTmp, spisokWrong;
    Tovar tovar;
    spisokCur = tableWidget->get_tovar_list("x");

    int count = spisokCur.count();

    progressBar->setMaximum(count);
    statusBar->addWidget(progressBar);
    progressBar->show();

    for (int i = 0; i<count; i++){
        progressBar->setValue(i);
        tovar = spisokCur.at(i);
        spTmp = my_informer->info(QString::number(tovar.nomer_of_tovar), "tnomer");
        if (!spTmp.count()) {
            tovar.name_of_tovar = tr("NOT FOUND ")
                    + tovar.name_of_tovar;
            spisokWrong << tovar;
        } else {
            tovar.name_of_tovar = spTmp.at(0).name_of_tovar;
            tovar.barcode = spTmp.at(0).barcode;
            spisokNew << tovar;
        }


    }
    statusBar->removeWidget(progressBar);

    this->on_action_new_triggered();
    tableWidget->load_tovar_list_into_table(spisokNew);

    ask_user_to_save_wrong_tovar_list(spisokWrong);
}

void cengen::on_action_export_tovar_list_to_clipboard() {
    QString str, tmp;
    str = tr("# ") + "\t" + tr("Tnomer") +" \t"
            + tr("Name") +" \t" + tr("Barcode") + " \t"
            + tr("Quantity") + "\t" + tr("Price1") + " \t"
            + tr("Price2") + "\n";
    QList<Tovar> tovar = tableWidget->get_tovar_list("x");
    Tovar tvr;
    for (int i = 0; i<tovar.count(); i++) {
        tvr = tovar.at(i);
        tmp = QString::number(i+1) + "\t";
        tmp+= QString::number(tvr.nomer_of_tovar) + "\t";
        tmp+= tvr.name_of_tovar + "\t";
        tmp+= tvr.barcode + "\t";
        tmp+= QString::number(tvr.quantity) + "\t";
        tmp+= QString::number(tvr.price1) + "\t";
        tmp+= QString::number(tvr.price2) + "\t";
        tmp+= "\n";
        str += tmp;
    }
    QClipboard *pcb = QApplication::clipboard();
    pcb->setText(str);
}

void cengen::on_action_set_special_shablon_to_zero_price2() {
    QDialog *dlg = new QDialog(this);
    QGridLayout *lay = new QGridLayout;
    QPushButton *butOK = new QPushButton(tr("OK"));
    QPushButton *butNO = new QPushButton(tr("Cancel"));
    QLabel *lb1 = new QLabel(tr("Set special shablon for zero price2"));
    QLabel *lb2 = new QLabel(tr("Select shablon: "));
    QComboBox *bx = new QComboBox;
    bx->addItems(tableWidget->get_shablon_list());
    bx->setCurrentIndex(shablonBox->currentIndex());

    lay->addWidget(lb1, 0, 0, 1, 2);
    lay->addWidget(lb2, 1, 0);
    lay->addWidget(bx, 1, 1);
    lay->addWidget(butOK, 2, 1);
    lay->addWidget(butNO, 2, 0);

    connect(butOK, SIGNAL(clicked()), dlg, SLOT(accept()));
    connect(butNO, SIGNAL(clicked()), dlg, SLOT(reject()));

    dlg->setLayout(lay);

    if (dlg->exec()) {
        tableWidget->set_special_shablon_for_zero_price2(bx->currentIndex());
    }

}

void cengen::on_saveSourceButton() {
    qDebug() << "Saving source settings";
    //Сохранение настроек источника данных
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save source data"), QApplication::applicationDirPath() , tr("Source settings (*.das)"));

    if (fileName != "") {
        this->save_source_settings_file(fileName);
    }
}

void cengen::save_source_settings_file(QString fileName) {
    bool ckeck = false;
    int ind = fileName.lastIndexOf(".das", -1, Qt::CaseInsensitive);
    if (ind + 4 != fileName.length() || ind == -1) {
        fileName += ".das";
        ckeck = true;
    }

    QFile file(fileName);
    if (ckeck && file.exists()) {
        ListFoundedItemsDialog* dlg = new ListFoundedItemsDialog(this);
        dlg->setMessage(tr("File exist. Rewrite?"));
        if (dlg->exec()) {
            ckeck = false;
        } else {
            //приделать повторный выбор имени файла
            return;
        }
    } else {
        ckeck = false;
    }

    if(!ckeck && file.open(QIODevice::WriteOnly)) {
        QString settings;
        settings = this->dbf->fileName + "\n";
        settings += QString::number(config_index[0]) + "\n";
        settings += QString::number(config_index[1]) + "\n";
        settings += QString::number(config_index[2]) + "\n";
        settings += QString::number(config_index[3]) + "\n";

        QTextStream out(&file);
        out.setCodec("UTF-8");
        out << settings;
        file.close();
    }
}

void cengen::on_loadSourceButton() {
    qDebug() << "Load source settings";
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open source data"), QApplication::applicationDirPath(), tr("Source settings (*.das)"));
    if (fileName == "") {
        qDebug() << "Please select file name";
        return;
    }
    load_source_settings_file(fileName);
}

bool cengen::load_source_settings_file(QString fileName) {

    if (fileName == "_$dbf") {
        fileName = ext_conf_edit->text();
    }
    QFile file;
    file.setFileName(fileName);

    if (file.open(QIODevice::ReadOnly)) {
        qDebug() << "Loading souce file: " << fileName;
        QTextStream fstream(&file);
        fstream.setCodec("UTF-8");

        QString line;

        //первая строка - название файла
        line = fstream.readLine();
        this->dbf->fileName = line;

        db_source = 0;

        if (dbf->fileName != "") {
            //если имя файла не пусто - продолжаем
            my_informer->set_tb_name(dbf->fileName);
            labelDBFname->setText(dbf->fileName);
            this->opisateli.clear();
            opisateli = my_informer->tb_describe("DBF");

            //вызываем функцию выбора и установки описателей
            this->update_ui_db_controls();

            ui_spinLimit->setMaximum(my_informer->get_maximum());

        }

        //последующие четыре строки - описатели полей предыдущего файла
        line = fstream.readLine();
        config_index[0] = line.toInt();
        line = fstream.readLine();
        config_index[1] = line.toInt();
        line = fstream.readLine();
        config_index[2] = line.toInt();
        line = fstream.readLine();
        config_index[3] = line.toInt();

        describer_is_ready = true;
        config.tnomer = opisateli.at(config_index[0]);
        config.tname = opisateli.at(config_index[1]);
        config.tbarcode = opisateli.at(config_index[2]);
        config.tprice = opisateli.at(config_index[3]);
        my_informer->set_fields(&config);

        update_ui_tb_fields(opisateli);

        update_ui_db_controls();
    } else {
        qDebug() << "Cannot open this file: " << fileName;
        return false;
    }
    return true;
}

void cengen::on_saveFilterSettings() {
    qDebug() << "Saving filter ";
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save filter data"), QApplication::applicationDirPath() , tr("Filter settings (*.fli)"));

    if (fileName != "") {
    bool ckeck = false;
    int ind = fileName.lastIndexOf(".fli", -1, Qt::CaseInsensitive);
    if (ind + 4 != fileName.length() || ind == -1) {
        fileName += ".fli";
        ckeck = true;
    }

    QFile file(fileName);
    if (ckeck && file.exists()) {
        ListFoundedItemsDialog* dlg = new ListFoundedItemsDialog(this);
        dlg->setMessage(tr("File exist. Rewrite?"));
        if (dlg->exec()) {
            ckeck = false;
        } else {
            //приделать повторный выбор имени файла
            return;
        }
    } else {
        ckeck = false;
    }

    if(!ckeck && file.open(QIODevice::WriteOnly)) {
        QString settings;
        //записываем название файла
        settings = filterDbf.fileName + "\n";
        settings += QString::number(ui_filterWhatBox->currentIndex()) + "\n";
        settings += QString::number(ui_filterWhereBox->currentIndex()) + "\n";
        //строка поиска
        settings += ui_filterLineText->text() + "\n";
        //ключ поиска - товарный номер, штрихкод и т.п. (в виде индекса)
        settings += QString::number(ui_filterWhatToFoundBox->currentIndex()) + "\n";
        //метод фильтрации - равно, больше, меньше и т.п. (в виде индекса)
        settings += QString::number(ui_filterMethodBox->currentIndex()) + "\n";
        settings += QString::number(filterCheckInBox->currentIndex()) + "\n";

        //удалять ли ненайденные позиции
        settings += QString::number(delete_filtered_box->isChecked()) + "\n";

        //использовать ли найденное в фильтре поле для обновления значений
        settings += QString::number(filBoxCheck->isChecked()) + "\n";

        QTextStream out(&file);
        out.setCodec("UTF-8");
        out << settings;
        file.close();
    }
}
}

void cengen::on_loadFilterSettings() {
    qDebug() << "load filter";
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open filter settings"), QApplication::applicationDirPath(), tr("Filter settings (*.fli)"));
    if (fileName == "") {
        qDebug() << "Please select file name";
        return;
    }
    load_filter_settings_file(fileName);
}

void cengen::load_filter_settings_file(QString fileName) {
    QFile file;
    file.setFileName(fileName);

    if (file.open(QIODevice::ReadOnly)) {
        qDebug() << "Load file: " << fileName;
        QTextStream fstream(&file);
        fstream.setCodec("UTF-8");

        QString line;

        //первая строка - название файла
        line = fstream.readLine();
        filterDbf.fileName = line;
        if (filterDbf.fileName == "") {
            this->file_is_ready = false;
            ui_statusLabel->setText(tr("Wrong shablon"));
            return;
        }

        on_filterFileName_changed();

        line = fstream.readLine();
        ui_filterWhatBox->setCurrentIndex(line.toInt());
        line = fstream.readLine();
        ui_filterWhereBox->setCurrentIndex(line.toInt());

        line = fstream.readLine();
        ui_filterLineText->setText(line);

        line = fstream.readLine();
        ui_filterWhatToFoundBox->setCurrentIndex(line.toInt());

        line = fstream.readLine();
        ui_filterMethodBox->setCurrentIndex(line.toInt());

        line = fstream.readLine();
        filterCheckInBox->setCurrentIndex(line.toInt());

        //удалять ли ненайденные позиции
        //settings += QString::number(delete_filtered_box->isChecked()) + "\n";
        line = fstream.readLine();
        delete_filtered_box->setChecked(line.toInt());

        //использовать ли найденное в фильтре поле для обновления значений
        //settings += QString::number(filBoxCheck->isChecked()) + "\n";
        line = fstream.readLine();
        filBoxCheck->setChecked(line.toInt());

    } else {
        qDebug() << "Cannot open file: " << fileName;
    }


}

void cengen::on_action_sell_filter_triggered() {

    if (!sell_file_is_checked) check_sell_file();


    //methodSellBox->setCurrentIndex(1);
    int methodSell = methodSellBox->currentIndex();
    //0 - заменить количество товара количеством проданных единиц
    //1 - вычесть из старого количества количество проданных единиц

    //float methodVal = methodSellValue->text().toFloat();
    qDebug() << "Sell action method" << methodSell;


    dbTranslator sellConfig;
    sellConfig.tbarcode = sellDateBox->currentText();
    sellConfig.tnomer =   sellNomerBox->currentText();
    sellConfig.tname =    sellTimeBox->currentText();
    sellConfig.tprice =   sellKolBox->currentText();

    sell_informer->set_fields(&sellConfig);

    sell_informer->set_limit_search(1);
    QList<Tovar> tb1, tb2, tb3, curTb, newTb;

    QString curDate;
    QDate dt1, dt2;
    int init_pos;
    dt1 = dateStart->selectedDate();
    //qDebug() << "Dates is " << dt1 << last_known_date;
    if (dt1 >= last_known_date) {
        qDebug() << "Using last known position";
        init_pos = last_known_pos;
    } else {
        qDebug() << "Found last date";
        init_pos = 0;
    }
    int startPos;
    //dt1 = dt1.addDays(-1);
    curDate =  dt1.toString("yyyyMMdd");
    //qDebug() << "cur date " << curDate;
    tb1 = sell_informer->info(curDate, "tbarcode", init_pos, -1, 1, true);

    //dt1 = dt1.addDays(1);

    if (!tb1.count()) {
        startPos = 0;
    } else {
        startPos = sell_informer->last_found_record_number();
        startPos--;
        //startPos++;
        if (dt1 > last_known_date) {
            last_known_pos = startPos;
            QString dt = tb1.at(0).barcode;
            last_known_date = QDate::fromString(dt, "yyyyMMdd").addDays(1);
            last_known_file = sellFileName;
            qDebug() << "We know now: " << startPos << last_known_date;
        }
    }
    //qDebug() << "Start pos will be: " << startPos;
    //qDebug() << "Date delta is " << last_known_date.daysTo(dt1);


    dt2 = dateStop->selectedDate();
    curDate = dt2.toString("yyyyMMdd");
//    curDate = "20120311";
    qDebug() << curDate;

    tb2 = sell_informer->info(curDate, "tbarcode", init_pos, -1, 0, false);
    int lastPos;
    //lastPos = 374904;
    int max = sell_informer->get_maximum();
    if (tb2.count()) {
        lastPos = sell_informer->last_found_record_number();
        lastPos++;
        lastPos++;
        if (lastPos > max) lastPos = max;

    } else {
        lastPos = max;
    }


    //qDebug() << "Last position " << lastPos;

    curTb = tableWidget->get_tovar_list("x");

    int count, prod;
    Tovar tovar;
    QList<QDateTime> lastSellDates;
    QDateTime lastSellDate;
    QString lh, lm ;
    QDateTime zeroDate = QDateTime(QDate::fromString("19000101", "yyyyMMdd"));


    progressBar->setMaximum(curTb.count());
    statusBar->addWidget(progressBar);
    progressBar->show();

    for (int i = 0; i<curTb.count(); i++) {
        progressBar->setValue(i);
        tovar = curTb.at(i);
        tb3 = sell_informer->info
                (QString::number(tovar.nomer_of_tovar),
                 "tnomer",
                 startPos,
                 lastPos,
                 -1,
                 true,
                 false);
        //qDebug() << i << " Last found number" << sell_informer->last_found_record_number();
        QApplication::processEvents();
        count = 0;
        prod = tb3.count();
        for (int j = 0; j<prod; j++) {
            count += tb3.at(j).price1;
        }
        if (prod) {
            lh = tb3.at(prod-1).name_of_tovar.split(":").at(0);
            lm = tb3.at(prod-1).name_of_tovar.split(":").at(1);
            //qDebug() << "Time is " <<lh << lm;

            lastSellDate = QDateTime(QDate::fromString(tb3.at(prod-1).barcode, "yyyyMMdd"),
                                     QTime::fromString(tb3.at(prod-1).name_of_tovar, "hh:mm"));
            //qDebug() << "Last date " << lastSellDate;
        } else {
            lastSellDate = zeroDate;
        }
        lastSellDates << lastSellDate;
        //в зависимости от метода обновления - разные действия
        int days;
        switch (methodSell) {
        case 0:
            tovar.quantity = count;
            break;
        case 1:
            tovar.quantity = tovar.quantity - count;
            break;
        case 2:
            days = dt1.daysTo(dt2) + 1;
            tovar.price2 = (float)count / days;
            tovar.quantity = count;
            break;
        default:
            tovar.quantity = count;
            break;
        }

        //tovar.quantity = count;
        newTb << tovar;        
    }
    if (curTb.count() == 1 && tb3.count()) {
        //был поиск продаж только одного товара и он продавался -
        //  покажем информацию о дате и времени продажи
        int prod = tb3.count();
        QString dt = tb3.at(prod-1).barcode;
        QDate ldt  = QDate::fromString(dt, "yyyyMMdd");
        dt = ldt.toString("d MMMM yyyy, dddd");
        QString tm = tb3.at(prod-1).name_of_tovar;
        QString message = tr("Total sold %1 goods of %2 tnomer%3Last date is %4 at %5")
                .arg(QString::number(prod))
                .arg(QString::number(curTb.at(0).nomer_of_tovar))
                .arg("\n")
                .arg(dt)
                .arg(tm);
        ListFoundedItemsDialog *dlg = new ListFoundedItemsDialog(this);
        dlg->setMessage(message);
        dlg->exec();
    }
    statusBar->removeWidget(progressBar);



    on_action_new_triggered();
    tableWidget->load_tovar_list_into_table(newTb);
    on_action_get_sum_of_tovar();

}

void cengen::on_selectSellFileButtonClicked() {
    //выбор DBF-файла
    QString str = QFileDialog::getOpenFileName(0, "Select SELL file", sellFileName, "DBF-files (*.dbf)");
    sellFileName = str;
    updateSellTab();
    check_sell_file();
}

void cengen::check_sell_file() {
    qDebug() << "Checking sell file";
    sell_informer = new Tinformer;
    DbfConfig sellDb;
    sellDb.fileName = sellFileName;

    QFileInfo fi;
    fi.setFile(sellFileName);
    if (!fi.exists()) {
        qDebug() << "File not exists";
        return;
    }


    sell_informer->prepare(&sellDb);

    sell_informer->set_tb_name(sellDb.fileName);

    int j1 = sellNomerBox->currentIndex();
    int j2 = sellDateBox->currentIndex();
    int j3 = sellTimeBox->currentIndex();
    int j4 = sellKolBox->currentIndex();

    sellNomerBox->clear();
    sellDateBox->clear();
    sellTimeBox->clear();
    sellKolBox->clear();

    sellOpisateli = sell_informer->tb_describe("DBF");

    sellNomerBox->addItems(sellOpisateli);
    sellDateBox->addItems(sellOpisateli);
    sellTimeBox->addItems(sellOpisateli);
    sellKolBox->addItems(sellOpisateli);

    sellNomerBox->setCurrentIndex(j1);
    sellDateBox->setCurrentIndex(j2);
    sellTimeBox->setCurrentIndex(j3);
    sellKolBox->setCurrentIndex(j4);

    //delete sell_informer;

    sell_file_is_checked = true;
}

void cengen::on_saveSellSettingsButtonClicked() {
    qDebug() << "Saving sell settings";
}

void cengen::updateSellTab() {
    lb2->setText(sellFileName);
}

void cengen::on_action_render_in_external_app() {
    qDebug() << "Going to use external application";

    QString ext_shablon_name = ext_shablon_name_edit->text();

    QString dbf_tmp_name;
    dbf_tmp_name = QApplication::applicationDirPath();
    #ifdef Q_WS_WIN
    dbf_tmp_name += "\\";
    #else
    dbf_tmp_name += "/";
    #endif
    dbf_tmp_name += "result.dbf";
    qDebug() << "tmp DBF file is: " << dbf_tmp_name;

    QString extName = ext_prog_name_edit->text();

    QStringList args;
    args << dbf_tmp_name;
    args << ext_shablon_name;

    qDebug() << "app name " << extName;

    QList<Tovar> spisok =  tableWidget->get_tovar_list("x");
    if (expandBox->isChecked()) {
        qDebug() << "Expanding ";
        spisok = expand(spisok);

    }

    if (save_tovar_list_into_dbf(dbf_tmp_name, spisok)) {
        QProcess proc;

        proc.execute(extName, args);

    }


}

void cengen::set_ext_shablon_name(QString fileName) {
    ext_shablon_name_edit->setText(fileName);
}

void cengen::select_ext_conf_button_clicked() {
    QFileInfo fi(ext_conf_edit->text());
    QString fileName = QFileDialog::getOpenFileName(this, tr("Choose config location"), fi.path() , tr("CenGen config (tmp.das)"));
    ext_conf_edit->setText(fileName);

}

void cengen::select_ext_app_button_clicked() {
    QFileInfo fi(ext_prog_name_edit->text());
    QString fileName = QFileDialog::getOpenFileName(this, tr("Choose external application location"), fi.path() , tr("Executable files (*.exe)"));
    ext_prog_name_edit->setText(fileName);
}

void cengen::select_ext_shablon_button_clicked() {
    QFileInfo fi(ext_shablon_name_edit->text());
    QString fileName = QFileDialog::getOpenFileName(this, tr("Choose external shablon location"), fi.path() , tr("Fast report shablons (*.frf)"));
    ext_shablon_name_edit->setText(fileName);
}

bool cengen::save_tovar_list_into_dbf(QString fileName, QList<Tovar> spisok) {
    QFile file;
    file.setFileName(fileName);
    if (!file.open(QIODevice::WriteOnly)) {
        qDebug() << "Cannot open dbf file: " << fileName;
        return false;
    }
    //значит файл открылся
    int max_name_length=0;
    for (int i = 0; i<spisok.count(); i++) {
        if (max_name_length < spisok.at(i).name_of_tovar.length()) {
            max_name_length = spisok.at(i).name_of_tovar.length();
        }
    }


    QByteArray header;

    //байт 1 - простая таблица
    header.append(3);

    //байты 2, 3, 4 год, месяц и день редактирования
    QDate curdate = QDate::currentDate();
    header.append(curdate.year()-1900);
    header.append(curdate.month());
    header.append(curdate.day());

    //байты 5 6 7 8 число записей в таблице
    int records_count = spisok.count();
    qDebug() << "count " << records_count;

    int tmp = records_count / 256;
    int bit1 = records_count - tmp*256;
    int tmp2 = tmp / 256;
    int bit2 = tmp - tmp2*256;
    int tmp3 = tmp2 / 256;
    int bit3 = tmp2 - tmp3*256;
    int tmp4 = tmp3 / 256;
    int bit4 = tmp3 - tmp4*256;

    header.append(bit1);
    header.append(bit2);
    header.append(bit3);
    header.append(bit4);

    char zeroh = 0;

    //байты 8 9 размер заголовка в байтах, заранее неизвестен
    header.append(zeroh);
    header.append(zeroh);

    //байт 10 и 11 - размер записи в байтах, тоже пока неизвестен
    header.append(zeroh);
    header.append(zeroh);

    //два байта 12 и 13 - зарезервировано (будет использоваться в 2050 году, когда Россия создаст поселение на Марсе)
    header.append(zeroh);
    header.append(zeroh);

    //байт 14 - признак незваршенной транзацкии, у нас 0
    header.append(zeroh);

    //байт 15 - а не закодирована ли таблица
    header.append(zeroh);

    //байт 16-27 включительно многопользовательский режим
    for (int i = 0; i<12; i++) header.append(zeroh);

    //байт 28 - содержание поля МЕМО. У нас нет
    header.append(zeroh);

    //байт 29 - код драйвера языка. У нас ОЕМ, т.е. 0
    header.append(zeroh);

    //байт 30 и 31 - зарезервировано. И нафига? :)
    header.append(zeroh);
    header.append(zeroh);

    file.write(header);

    QStringList fields;
    fields << "N_TOV"
           << "NAME_T"
           << "KOL"
           << "CHENA"
           << "SHKOD"
           << "DATE_VV";

    int length_of_header_structure = 0;
    int record_length = 0;
    for (int i = 0; i<fields.count(); i++) {
        QString field;
        field = fields.at(i);
        QByteArray head;

        //байты 0-11 название колонки, завершающеся нулями
        for (int j = 0; j<field.toAscii().length(); j++) {
            head.append(field[j].toAscii());
        }
        for (int j = field.toAscii().length(); j<11; j++) {
            head.append('\0');
        }

        //байт 12 - тип записи (ну там число, текст)
        char type = 'C';
        if (field == "NAME_T" || field == "SHKOD")
            type = 'C';
        if (field == "N_TOV" || field == "KOL" || field == "CHENA")
            type = 'N';
        if (field == "DATE_VV")
            type = 'D';
        head.append(type);

        //байты 12 13 14 15 - адрес поля в памяти. не исп.
        head.append(zeroh);
        head.append(zeroh);
        head.append(zeroh);
        head.append(zeroh);

        //байт 16 - размер (ширина) колонки
        int len = 1;
        if (field == "NAME_T")
            len = max_name_length;
        if (field == "N_TOV" || field == "KOL" || field == "CHENA" )
            len = 8;
        if (field == "SHKOD")
            len = 13;
        if (field == "DATE_VV")
            len = 8;
        record_length += len;
        head.append(len);

        //байт 17 - количество знаков после запятой
        int zap = 0;
        if (field == "CHENA")
            zap = 2;
        head.append(zap);

        //байты 18 и 19 зарезервированы
        head.append(zeroh);
        head.append(zeroh);

        //байт 20 - идентификатор рабочей области, не используется
        head.append(zeroh);

        //байты 21 и 22 - многопользовательский байт, у нас однопользов.
        head.append(zeroh);
        head.append(zeroh);

        //байт 23 - устаноленные поля. ХЗ.
        head.append(zeroh);

        //байты 24-30 зарезервированы
        for (int i = 24; i<=30; i++)
            head.append(zeroh);

        //байт 31 а не включено ли поле в .mdx индекс
        head.append(zeroh);

        length_of_header_structure += head.count();
        file.write(head);

    }

    file.putChar(0x0D);

    QTextCodec *codec = QTextCodec::codecForName("IBM 866");
    QTextEncoder *decoder = new QTextEncoder(codec);


    zeroh = ' ';
    for (int i = 0; i<records_count; i++) {
        //теперь записываем строчки, одну за одной
        Tovar tovar;
        tovar = spisok.at(i);

        tovar.name_of_tovar = decoder->fromUnicode(tovar.name_of_tovar);

        //tovar.price1 = tovar.price1*100;    //это сразу чтоб дробные записать
        QByteArray head;

        //байт 0 - признак, не удалена ли строчка. Таких не держим
        head.append(zeroh);

        for (int j = 0; j<fields.count(); j++) {
            QString field = fields.at(j);
            //и теперь в каждой строчке - по ячейке
            int len = 1;
            if (field == "NAME_T")
                len = max_name_length;
            if (field == "N_TOV" || field == "KOL" || field == "CHENA" )
                len = 8;
            if (field == "SHKOD")
                len = 13;
            if (field == "DATE_VV")
                len = 8;

            QString zap = "";
            if (field == "N_TOV")
                zap = QString::number(tovar.nomer_of_tovar);
            if (field == "KOL")
                zap = QString::number(tovar.quantity);
            if (field == "CHENA")
                zap = QString::number(tovar.price1);
            if (field == "NAME_T"){
                //zap = superSplit(tovar.name_of_tovar);
                zap = tovar.name_of_tovar;
            }
            if (field == "SHKOD")
                zap = tovar.barcode;
            if (field == "DATE_VV")
                zap = QDate::currentDate().toString("yyyyMMdd");

            if (field == "NAME_T") {
                for (int k = 0; k<zap.length(); k++)
                    head.append(zap[k].toAscii());
                for (int k = 0; k<len-zap.length(); k++)
                    head.append(zeroh);
            } else {
                for (int k = 0; k<len-zap.length(); k++)
                    head.append(zeroh);
                for (int k = 0; k<zap.length(); k++)
                    head.append(zap[k].toAscii());
            }

        }
    file.write(head);
    }

    //запишем длину структуры заголовка
    length_of_header_structure += 33;
    file.seek(8);
    tmp = length_of_header_structure / 256;
    bit1 = length_of_header_structure - tmp*256;
    tmp2 = tmp / 256;
    bit2 = tmp - tmp2*256;
    file.putChar(bit1);
    file.putChar(bit2);

    //запишем длину каждой записи
    record_length++;
    file.seek(10);
    tmp = record_length / 256;
    bit1 = record_length - tmp*256;
    tmp2 = tmp / 256;
    bit2 = tmp - tmp2*256;
    file.putChar(bit1);
    file.putChar(bit2);

    file.close();
    return true;
}

void cengen::on_action_expand_triggered(bool value) {
    expandBox->setChecked(value);
    previewed = false;
}

void cengen::on_action_select_method_barcode() {
    ui_radioButton_1->click();
}
void cengen::on_action_select_method_tnomer() {
    ui_radioButton_2->click();
}
void cengen::on_action_select_method_tname() {
    ui_radioButton_3->click();
}

void cengen::on_action_get_sum_of_tovar() {
    double sum = tableWidget->sum_of_tovar();
    int kop = sum*100 - (int)sum*100;
    //qDebug() << "kop" << kop << "sum=" << sum;
    int rub = (int)sum;
    float delta = sum*100 - (kop + rub*100);
    //qDebug() << "Delta " << delta;
    kop += delta;

    QString sm = "";
    int tmp;
    int dg;
    while (rub) {
        tmp = rub/10;
        dg = rub - tmp*10;
        rub = tmp;
        sm = QString::number(dg) + sm;
    }
    if (sm == "") sm = "0";
    QString kp = QString::number(kop);
    if (kop<10) kp = "0" + kp;

    sm += "." + kp;
    ui_statusLabel->setText(tr("Sum of tovar: " ) + sm);
}

void cengen::on_action_program_update() {
    qDebug() << "Going to update program";
    update_dlg = new ListFoundedItemsDialog(this);
    update_dlg->setMessage(tr("Updating. Please wait."));
    update_dlg->show();

    update_counter = 0;
    updatingPath = "http://mihol.ru/cengen/bin/" + QApplication::organizationName() + "/";
    QString file = updatingPath + "list.txt";


    manager = new QNetworkAccessManager(this);
    connect(manager, SIGNAL(finished(QNetworkReply*)),
            this, SLOT(httpRequestFinished(QNetworkReply*)));

    manager->get(QNetworkRequest(QUrl(file)));

}

void cengen::httpRequestFinished(QNetworkReply* rpl) {
    qDebug() <<"Ready ";
    //qDebug() << rpl->size();
    QStringList files;
    QString line;
    while (!rpl->atEnd()) {
        line = rpl->readLine();
        line = line.split("\n").at(0).trimmed();
            qDebug() << "File is: " << line;


            files << line;
    }
    update_count = files.count();
    rpl->close();
    up_files.clear();
    for (int i = 0; i<files.count(); i++) {
        manager2 = new QNetworkAccessManager(this);
        connect(manager2, SIGNAL(finished(QNetworkReply*)), SLOT(httpOneFileFinished(QNetworkReply*)));

        QString file = updatingPath + files.at(i);
        currentUpdatingFile = files.at(i);
        qDebug() << "File: " << file;
        QNetworkReply *rpl2 = manager2->get(QNetworkRequest(QUrl(file)));
        up_files[rpl2] = currentUpdatingFile;


    }

}
void cengen::httpOneFileFinished(QNetworkReply *rpl) {
    int size1, size2;
    size1 = rpl->size();
    qDebug() << size1;
    QString fele = up_files[rpl];
    QFileInfo fi(fele);
    size2 = fi.size();
    if (size1 != size2) {
        QFile fl(fele);
        if (fi.exists()) {
        #ifdef Q_WS_WIN
            qDebug() << "WIN section";
            fl.rename(fele+".bak.exe");
        #else
            qDebug() << "Lin section";
            fl.rename(fele+".bak");
        #endif
        }

        fl.setFileName(fele);
        QByteArray arr = rpl->readAll();
        fl.open(QIODevice::WriteOnly);
        fl.write(arr);
        fl.close();
    } else {
        qDebug() << "File " << fele << " don't need updating: size equal";
    }
    update_counter++;
    if (update_counter >= update_count) {
        //обновились все файлы - выведем сообщение
        update_dlg->setMessage(tr("Updated successful"));
        //update_dlg->show();
    }
    //unfinishedHttp = false;
}

void cengen::on_action_zakaz10_triggered() {
    qDebug() << "Going to make zakaz on 10 days";
    QList<Tovar> tbOld, tbNew;
    Tovar tovar;
    int quant;
    tbOld = tableWidget->get_tovar_list("x");
    tbOld = apply_filter(tbOld);
    for (int i = 0; i<tbOld.count(); i++) {
        tovar = tbOld.at(i);

        quant = ceil(tovar.price2 * 10);
        //qDebug() << "Count on skald " << tovar.quantity << " Need: " << quant;
        if (quant < tovar.quantity) {
            quant = 0;
        } else {
            quant = quant - tovar.quantity +1;
        }
        tovar.quantity = quant;
        tbNew << tovar;
    }
    on_action_new_triggered();
    tableWidget->load_tovar_list_into_table(tbNew);
}

void cengen::on_action_remove_zero_items() {
    tableWidget->remove_zero_quantity();
}
