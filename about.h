#ifndef ABOUT_H
#define ABOUT_H

class QLabel;
class QTabWidget;
class QTextEdit;
class QToolButton;
class QIcon;

#include <QtGui/QDialog>
#include <QIcon>
#include <QLabel>

class About : public QDialog
{
        Q_OBJECT

private:
        QLabel *iconLabel;
        QLabel *aboutLabel;
        QLabel *photoLabel;
        QToolButton *closeButton;

        QTabWidget *tabWidget;

        QTextEdit *copyrightView;
        QTextEdit *thanksView;
        QTextEdit *licenseView;

        QString m_author;
        //QIcon   *m_author_photo;
        QString m_mail;
        QString m_phone;
        QString m_license;

        QList<QStringList> m_thanks;
public:
        About(QWidget *parent = 0, Qt::WFlags f = Qt::WindowSystemMenuHint);
        virtual ~About()
        {}

        void setLicenseText(const QString& text);

        void setAuthor(const QString& author)
        {m_author = author; printCopyright();}

        void setAuthorPhoto(QIcon icon)
        {photoLabel->setPixmap(icon.pixmap(64, 64)); }

        void setMail(const QString& mail)
        {m_mail = mail; printCopyright();}

        void setPhone(const QString& phone)
        {m_phone = phone; printCopyright();}

        void setLicense(const QString& license)
        {m_license = license; printCopyright();}

        void addThanks (const QString& name, const QString& email, const QString& typeWork)
        {
                QStringList l;
                l << name << email << typeWork;
                m_thanks << l;
                printThanks ();
        }

private:
        void printCopyright();
        void printThanks ();
};

#endif // ABOUT_H
