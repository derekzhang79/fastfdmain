#ifndef PUBLIC_INS_MAKER_H
#define PUBLIC_INS_MAKER_H

#include <QStandardItemModel>
#include <QMap>
#include "commonheader.h"
#include "lds_query.h"
#include "pointerautodel.h"
#include <QTranslator>

class public_ins_maker : public QObject
{
    Q_OBJECT
public:
    public_ins_maker(QObject *parent = 0);
    void clear();
    QByteArray textAlign(const QString &text, Qt::Alignment textflag, int textwidth,  char c = ' ' );//00, 10, 01,11
public:
    void drawText(const QByteArray &hex=QByteArray(""), const QString &text=QString(""), Qt::Alignment textflag=Qt::AlignLeft);//获取文本，打印指令
    void drawText(const QByteArray &hex, const QString &text, int textwidth ,Qt::Alignment textflag);//获取文本，打印指令
    void insertText(int index, const QByteArray &hex=QByteArray(""), const QString &text=QString(""), Qt::Alignment textflag=Qt::AlignLeft);
    void insertText(int index, const QByteArray &hex, const QString &text, int textwidth ,Qt::Alignment textflag);
    int currentIndex();

    void drawN();

    void drawLogo(const QByteArray &hex=QByteArray(""), const QString &logopath=QString(""));//获取logo，打印指令
    void drawQR_Code(const QByteArray &hex/*=QByteArray("1B40 1B6101")*/, const QString &code);//获取二维码，打印指令
    void drawoneBarcode(const QByteArray &hex/*=QByteArray("1B40 1B6101")*/, const QString &code);//获取二维码，打印指令
    void drawTextEnd(const QByteArray &hex);

    void setPrintCount(int printcount=1);//获取打印次数，打印指令
    bool asynchPrint(const QString &path, bool isblock=false);//异步打印
    bool asynchPrintWithoutresolve(const QString &path, bool isblock=false);//异步打印
    bool synchPrint();//同步打印
    bool synchPrintWithoutresolve(const QString &path);//同步打印

    void begin_column_print(int columnCount);
    void end_column_print();
    int column_width();

    int left();
    void resolve();
public:
    static void resolve(const QString &path, lds_query &query, blockIns &ins);
    static QString stringToHex(const QString &str);
    static pieceIns draw_qr_code(const QByteArray &hex/*=QByteArray("1B40 1B6101")*/, const QString &code);//获取二维码，打印指令

public slots:
    void sendError(const QString &err, const QString &title);
//signals:
//    void errorString(const QString &err, const QString &title);
private:
    bool is_column_print;
    int column;
    int row;
    int step;
    int column_count;
public:
    int paperwidth;
    blockIns print_ins;
    QString print_path;


    /////////////////
public:
    void setPrintTranslate(const QString &languageKey, QObject *languageObj);
    QString printTr(const char *key);
    bool languageIsCN();
private:
    pointerAutoDel<QTranslator> printTranslate;
    QTranslator *getPrintTranslate(const QString &languageKey);
    QString languageKey;
    QObject *languageObj;
};

#endif // PUBLIC_INS_MAKER_H
