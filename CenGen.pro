# -------------------------------------------------
# Project created by QtCreator 2011-05-08T22:12:48
# -------------------------------------------------
QT += sql
QT += xml
QT += network
#INCLUDEPATH += D:\\mysql\\include
#LIBS+=-LD:\\mysql\\lib\\opt
#LIBS+=-lmysqlclient

QMAKE_CXXFLAGS += -fno-show-column


TARGET = CenGen
TEMPLATE = app
SOURCES += main.cpp \
    cengen.cpp \
    tinformer.cpp \
    cen_viewer.cpp \
    barcode.cpp \
    dbf_informer.cpp \
    dialog.cpp \
    editor.cpp \
    graph.cpp \
    imagebox.cpp \
    about.cpp \
    mainTableWidget.cpp \
    autozakaz.cpp \
    sellFilterWidget.cpp \
    models/textBox.cpp \
    models/line.cpp \
    models/image.cpp \
    models/good.cpp \
    models/barcodeBox.cpp \
    models/abstract_item.cpp \
    SyntaxHighlighter.cpp \
    xmlTextEditor.cpp
HEADERS += cengen.h \
    tinformer.h \
    tovar.h \
    bvalidator.h \
    barcode.h \
    cen_viewer.h \
    sql_config.h \
    dbf_config.h \
    db_translator.h \
    sql_validator.h \
    dbf.h \
    dbf_informer.h \
    dialog.h \
    editor.h \
    graph.h \
    imagebox.h \
    about.h \
    mainTableWidget.h \
    tnvalidator.h \
    file_worker.h \
    autozakaz.h \
    sellFilterWidget.h \
    models/textBox.h \
    models/line.h \
    models/image.h \
    models/good.h \
    models/barcodeBox.h \
    models/abstract_item.h \
    SyntaxHighlighter.h \
    xmlTextEditor.h
FORMS += \
    autozakaz.ui

OTHER_FILES += \
    todo.txt \
    tmp.txt

TRANSLATIONS = cengen_ru.ts\
    cengen_en.ts

RESOURCES += \
    resource.qrc




