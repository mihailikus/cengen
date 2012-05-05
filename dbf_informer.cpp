#include <QStringList>
#include <QDebug>
#include "dbf_informer.h"

dbf_informer::dbf_informer() {
    codec = QTextCodec::codecForName("IBM 866");
    decoder = new QTextEncoder(codec);
    decodec = new QTextDecoder(codec);
    first_time = true;
    first_tnomer_search = true;
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
    first_tnomer_search = true;

    QStringList list;
    if (this->describe_dbf()) {
        for (int i = 0; i<fields.count(); i++) {
            list << fields.at(i).name;
        }
    }
    return list;
}

QString dbf_informer::get_one_cell(int offset, int lenth) {
    one_cell.clear();
    one_cell.resize(lenth);
    for (int j = 0; j<lenth; j++) {
        one_cell[j] = this->all_records[j+offset];
    }
//    QString tmp;
//    tmp = one_cell;

    //qDebug() << "tmp value is " << tmp;
    //return tmp.trimmed();
    return codec->toUnicode(one_cell).trimmed();
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

    if (!first_time && file_read_start > startPos) {
        file_read_start = startPos;
        first_time = true;
    }
    if (!first_time && file_read_end < maximum) {
        file_read_end = maximum;
        first_time = true;
    }

    if (first_time) {
        this->read_file(startPos, maximum);
        //прочитаем необходему часть файла в буфер
    }


    Tovar tovar;
    QList<Tovar> tovarList;
    tovar.price2 = 0;
    tovar.quantity = 0;
    tovar.shablon = 0;

    if (method == "tnomer" || method == "tbarcode") {
        if (limit != -1) {
                limit = 1;
        } else {
            limit = maximum-startPos;
        }
    }



    int offset;
    int i;
    int method_offset = dbf_fields[method].offset;
    if (FromStartToEnd) {
        offset = 1 + method_offset + (-file_read_start+startPos)*length_of_each_record;
        i = startPos;
    } else {
        offset = 1 + method_offset + length_of_each_record*(maximum-startPos-1);
        i = maximum-1;
    }

    int count = 0;
    int curLength;
    QString value;
    bool found = false;

    int j = startPos;

    QByteArray arr;
    while (j<maximum && count < limit)
    {
        value = this->get_one_cell(offset, dbf_fields[method].length);
        if ((method == "tbarcode"
             || method == "tnomer") &&
                (value == searchText) ) found = true;
        if (method == "tname") {
//            arr.clear();
//            arr.insert(0, value);
//            value = codec->toUnicode(arr);
            //tmp = decoder->fromUnicode(searchText);
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
            //curLength = (i-startPos)*this->length_of_each_record +1;
            curLength = offset - method_offset;

            arr.clear();
            arr.insert(0, get_one_cell(dbf_fields["tname"].offset + curLength,
                                       dbf_fields["tname"].length));
            //tovar.name_of_tovar = codec->toUnicode(arr);
            tovar.name_of_tovar= get_one_cell(dbf_fields["tname"].offset + curLength,
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

QList<Tovar> dbf_informer::found_by_tnomer(int tnomer) {
    Tovar tovar;
    tovar.nomer_of_tovar = tnomer;
    tovar.quantity = 0;
    tovar.price1 = 0;
    tovar.price2 = 0;
    tovar.name_of_tovar = "NOT implemented yet";
    tovar.barcode = "0";

    if (first_time) {
        first_time = false;
        read_file(0, number_of_records);
    }

    if (first_tnomer_search) {
        //qDebug() << "First tnomer search";
        first_tnomer_search = false;
        int i = 1;
        int curLen =file_read_start * length_of_each_record + 1;
        int nomer;

        for (int j = file_read_start; j<=file_read_end; j++) {
            nomer = get_one_cell(dbf_fields["tnomer"].offset + curLen,
                                     dbf_fields["tnomer"].length).toInt();
            if (nomer>i) i = nomer;
            curLen += length_of_each_record;
        }
        offsets=(int*) malloc((i+1)*sizeof(int));
        maximum_tnomer = i;

        for (int j = 0; j<=i; j++) {
            offsets[j] = -1;
        }
        curLen =file_read_start * length_of_each_record + 1;

        for (int j = file_read_start; j<=file_read_end; j++) {
            nomer = get_one_cell(dbf_fields["tnomer"].offset + curLen,
                                     dbf_fields["tnomer"].length).toInt();
            offsets[nomer] = curLen;

            curLen += length_of_each_record;
        }
    }

    QList<Tovar> spisok;
    if (tnomer && tnomer <= maximum_tnomer) {
        int offset = offsets[tnomer];
        if (offset >=0) {
            tovar.price1 = get_one_cell(dbf_fields["tprice"].offset + offset,
                                        dbf_fields["tprice"].length).toFloat();

            tovar.name_of_tovar = get_one_cell(dbf_fields["tname"].offset + offset,
                                                       dbf_fields["tname"].length);

            tovar.barcode = get_one_cell(dbf_fields["tbarcode"].offset + offset,
                                         dbf_fields["tbarcode"].length);

            spisok << tovar;
        }
    }
    last_record = tnomer;
    return spisok;
}

void dbf_informer::read_file(int startPos, int maximum) {
    file.seek(this->length_of_header_structure + startPos*length_of_each_record);
    int fileSize = (1+maximum - startPos) * this->length_of_each_record;
    all_records = new char[fileSize];
//    all_records = (char*) malloc (fileSize)*sizeof(char);

    file.read(this->all_records, fileSize);
    first_time = false;
    file_read_start = startPos;
    file_read_end = maximum;
}
