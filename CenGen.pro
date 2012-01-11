# -------------------------------------------------
# Project created by QtCreator 2011-05-08T22:12:48
# -------------------------------------------------
QT += sql
QT += xml
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
    addlinemaster.cpp \
    imagebox.cpp \
    about.cpp
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
    addlinemaster.h \
    imagebox.h \
    about.h
FORMS += \
    editor.ui \
    addlinemaster.ui

OTHER_FILES += \
    todo.txt \
    tmp.txt

TRANSLATIONS = cengen_ru.ts\
    cengen_en.ts

RESOURCES += \
    resource.qrc
