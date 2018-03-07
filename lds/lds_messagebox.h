#ifndef LDS_MESSAGEBOX_H
#define LDS_MESSAGEBOX_H

#include <QObject>
#include <QMessageBox>
#include <QDialogButtonBox>
#include <QPushButton>
#include "lds_roundeddialog_rect_align.h"
#include <QAbstractButton>
#include "lds.h"

#define _TITLE_WIN(object)		( (qobject_cast<QWidget *>(object))?(qobject_cast<QWidget *>(object)->windowTitle()):("")  )
#define _TEXT_SEND(object)		 ( qobject_cast<QAbstractButton *>(object)?  qobject_cast<QAbstractButton *>(object)->text() : "" )
#define _TEXT_SLOT(object)		( object == 0 ? "" :  _TEXT_SEND(object->sender() ) )

class lds_msgbox:public QMessageBox{
    Q_OBJECT
public:
    lds_msgbox(Icon icon, const QString &title, const QString &text,
               StandardButtons buttons = NoButton, QWidget *parent = 0,
               Qt::WindowFlags flags = Qt::Dialog | Qt::MSWindowsFixedSizeDialogHint):QMessageBox(icon, title, text,
                                                                                                  buttons, parent,
                                                                                                  flags){}
    QPushButton *btnok;

public slots:
    void updateText(const QString &text);
    void updateTimeframe(int index);
signals:
    void btnok_finished(int index);
protected:
    void closeEvent(QCloseEvent *e);
private:
    QMovie *_movie;

};

class lds_messagebox : public QObject
{
    Q_OBJECT
public:
    explicit lds_messagebox(QObject *parent = 0);

    ////////////////////////////////////////////////////1
    static int warning(QWidget *parent, const QString &title,
                       const QString& text,
                       const QString& button0Text,
                       const QString& button1Text = QString(),
                       const QString& button2Text = QString(),
                       bool delayclose=false
            );
    static int warning(QWidget *parent, const QString &title,
                       const QString& text,
                       bool delayclose=false);
    ////////////////////////////////////////////////////2
    static int information(QWidget *parent, const QString &title,
                           const QString& text,
                           const QString& button0Text,
                           const QString& button1Text = QString(),
                           const QString& button2Text = QString(),
                           bool delayclose=false
            );
    static int information(QWidget *parent, const QString &title,
                           const QString& text,
                           bool delayclose=false);
    ////////////////////////////////////////////////////3
    static int question(QWidget *parent, const QString &title,
                        const QString& text,
                        const QString& button0Text,
                        const QString& button1Text = QString(),
                        const QString& button2Text = QString(),
                        const QString& button3Text = QString(),
                        bool delayclose=false
            );
    static int question(QWidget *parent, const QString &title,
                        const QString& text,
                        bool delayclose=false);
    ////////////////////////////////////////////////////4
    static int critical(QWidget *parent, const QString &title,
                        const QString& text,
                        const QString& button0Text,
                        const QString& button1Text = QString(),
                        const QString& button2Text = QString(),
                        bool delayclose=false);

    static int critical(QWidget *parent, const QString &title,
                        const QString& text,
                        bool delayclose=false);
    ////////////////////////////////////////////////////5
private:
    static int showOldMessageBox(QWidget *parent, QMessageBox::Icon icon,
                                 const QString &title, const QString &text,
                                 QString button0Text,
                                 const QString &button1Text,
                                 const QString &button2Text,
                                 const QString &button3Text,
                                 bool delayclose=false);
};

class lds_messagebox_loading : public QDialog{
    Q_OBJECT
public:
    lds_messagebox_loading(QWidget *parent, const QString &title,
                           const QString& text="");
    ~lds_messagebox_loading();
    void image_dynamic_run();
    QString text();
public slots:
    void setText(const QString &text);
    void update_image();
private:
    QMovie *mv;
    QLabel *label_image;
    QLabel *label_text;
};

class lds_messagebox_loading_show : public QObject{
    Q_OBJECT
public:
    lds_messagebox_loading_show(QWidget *parent, const QString &title,
                                const QString& text="");
    ~lds_messagebox_loading_show();
    void show();
    void hide();
    lds_roundeddialog_rect_align *widget();
    void show_delay();
    void image_dynamic_run();

    QString text();
public slots:
    void updateProgress(qint64 bytesStep, qint64 bytesTotal);
    void setText(const QString &text, int delay);
    void setText(const QString &text);
private:
    lds_roundeddialog_rect_align *dialog;
    lds_messagebox_loading *loading;

};

class lds_messagebox_textedit : public QDialog{
    Q_OBJECT
public:
    lds_messagebox_textedit(QWidget *parent, const QString &title,
                           const QString& text,
                           QStringList buttonTexts = QStringList());
    ~lds_messagebox_textedit();
    QList<QObject *> btnlist;
public slots:
    void todone();

};
#endif // LDS_MESSAGEBOX_H
