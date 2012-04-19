#include <QStringList>
#include <QDebug>
#include "dbf_informer.h"

dbf_informer::dbf_informer() {
    codec = QTextCodec::codecForName("IBM 866");
    first_time = true;
}

dbf_informer::~dbf_informer() {
    file.close();
}

bool dbf_informer::file_is_ready() {
    return file.isOpen();
}

bool dbf_informer::describe_dbf() {

    if (!file.isOpen()) {
        return false;
    }

    //считываем первоначальный заголовок DBF-файла
    char header[32];
    file.read(header, sizeof(header));

    //Получаем дату редактирования файла
    int year_of_edit = (unsigned char) header[1]+2000;
    int month_of_edit = (unsigned char) header[2];
    int date_of_edit = (unsigned char) header[3];
    this->date = "File was edited in " + QString::number(date_of_edit) + \
                   "-th of " + QString::number(month_of_edit) + \
                   ", " + QString::number(year_of_edit) + ".";

    //Получаем число записей в файле (байты 4 5 6 7)
    this->number_of_records = (unsigned char) header[4] + (unsigned char) header[5]*256 + \
                            (unsigned char) header[6]*65536 + (unsigned char) header[7]*65536*256;

    this->length_of_header_structure = (unsigned char) header[8] + (unsigned char) header[9]*256;

    this->length_of_each_record = (unsigned char) header[10] + (unsigned char) header[11]*256;

    //байт 14 - незавершенная транзакция

    this->incomplete_transac = (unsigned char) header[14];

    //Language driver - байт 29
    //в рассматриваемых файлах не используется
    //int lang_driver = (unsigned char) header[29];
    //qDebug() << lang_driver;

    //проверка подлинности файла
    if ( (number_of_records*length_of_each_record + length_of_header_structure) > file.size()) {
        number_of_records = 0;
        qDebug() << "Wrong DBF file. Select proper one";
        this->fields.clear();
        return false;
    }

    //--------------------------------------------------------
    //считываем описатели полей (на каждое поле - 32 байта)
    char terminator[1];
    file.read(terminator, 1);
    int field_count = 0;
    this->fields.clear();
    dbf_field_descr descr;
    while ((unsigned int) terminator[0] != 13) {
        field_count++;

        file.seek(file.pos()-1);
        char field_descr[32];
        file.read(field_descr, sizeof(field_descr));
        QChar name[10];
        int j = 0;
        while ( field_descr[j] ) {
            name[j] = field_descr[j];
            j++;
        }
        descr.type = field_descr[11];
        descr.decimal_count = field_descr[17];
        descr.length = field_descr[16];
        descr.index_field_flag = field_descr[31];

        QString field_name(name, j);
        descr.name = field_name;

        //qDebug() << descr->decimal_count;
        this->fields << descr;


        //и так до тех пор, пока не встретится терминатор - код 13
        file.read(terminator, 1);
    }
    return true;

}

QStringList dbf_informer::get_dbf_header(QString filename) {
    file.setFileName(filename);
    file.open(QIODevice::ReadOnly);
    first_time = true;

    QStringList list;
    if (this->describe_dbf()) {
        for (int i = 0; i<fields.count(); i++) {
            list << fields.at(i).name;
        }
    }
    return list;
}

QString dbf_informer::get_one_cell(int offset, int lenth) {
    QByteArray arr;
    arr.resize(lenth);
    for (int j = 0; j<lenth; j++) {
        arr[j] = this->all_records[j+offset];
    }
    return codec->toUnicode(arr).trimmed();
}

QList<Tovar> dbf_informer::found_record_in_dbf(QString searchText, QString method, int limit,
                                               int startPos, int endPos,
                                               bool FromStartToEnd) {
    int maximum;
    if (endPos == -1) {
        maximum = number_of_records;
    } else {
        maximum = endPos;
    }

    if (first_time) {
        //да сразу и прочитаем весь файл от начала до конца в буфер



        file.seek(this->length_of_header_structure + startPos*length_of_each_record);
        int fileSize = (maximum - startPos) * this->length_of_each_record;
        this->all_records = new char[fileSize];
        file.read(this->all_records, fileSize);
        file.seek(this->length_of_header_structure);
        first_time = false;
    }


    Tovar tovar;
    QList<Tovar> tovarList;
    tovar.price2 = 0;
    tovar.quantity = 0;
    tovar.shablon = 0;



    int offset;
    int i;
    if (FromStartToEnd) {
        offset = 1 + dbf_fields[method].offset;
        i = startPos;
    } else {
        offset = 1 + dbf_fields[method].offset + length_of_each_record*(maximum-startPos-1);
        i = maximum-1;
    }

    int count = 0;
    int curLength;
    QString value;
    bool found = false;

    int j = startPos;

    while (j<maximum && count < limit)
    {
        value = this->get_one_cell(offset, dbf_fields[method].length);
        if ((method == "tbarcode"
             || method == "tnomer") &&
                (value == searchText) ) found = true;
        if (method == "tname") {
            QStringList searches = searchText.split(" ");
            found = true;
            for (int j = 0; j<searches.count(); j++) {
                if (!value.contains(searches.at(j), Qt::CaseInsensitive)) {
                    found = false;
                }
            }
        }
        if (found)
        {
            curLength = i*this->length_of_each_record +1;

            tovar.name_of_tovar = get_one_cell(dbf_fields["tname"].offset + curLength,
                             dbf_fields["tname"].length);
            tovar.nomer_of_tovar = get_one_cell(dbf_fields["tnomer"].offset + curLength,
                                                dbf_fields["tnomer"].length).toInt();
            tovar.price1 = get_one_cell(dbf_fields["tprice"].offset + curLength,
                                                dbf_fields["tprice"].length).toFloat();
            tovar.barcode = get_one_cell(dbf_fields["tbarcode"].offset + curLength,
                                         dbf_fields["tbarcode"].length);

            tovarList << tovar;
            count++;
        }
        if (FromStartToEnd) {
            i++;
            offset += this->length_of_each_record;
        } else {
            i--;
            offset -= this->length_of_each_record;
        }
        j++;
        found = false;

    }

    this->last_record = i;
    return tovarList;

}

int dbf_informer::last_found_record() {
    return last_record;
}
