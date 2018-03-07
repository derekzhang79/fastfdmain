#include "lds_dialog_input_expand.h"
#include "ui_lds_dialog_input_expand.h"
#include <QFile>
#include "public_sql.h"
#include <QStringList>
#include "lds_dialog_input.h"
#include "backheader.h"
#include "lds_messagebox.h"
#include "lds.h"


lds_dialog_input_expand::lds_dialog_input_expand(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::lds_dialog_input_expand)
{
    ui->setupUi(this);

    //!---------------------------------1begin
    ui->frame->is_like_tab=false;
    int horizontal_count;
    int vertical_count;
    ui->frame->tool_size=QSize(96,80);
    horizontal_count=5;
    vertical_count=3;
    ui->frame->horizontal_count=horizontal_count;
    ui->frame->vertical_count=vertical_count;
    //!---------------------------------2end

    //1
    defreasons << tr("菜品售罄") << tr("外卖超区域") << tr("未达到外卖启送价格");

    file = new QFile(lds::localdataPath + "/quitreason.txt");
    file->open(QFile::ReadOnly);
    QByteArray readall = file->readAll();
    file->close();
    //2
    if(readall.isEmpty()){
        QByteArray txt;
        QDataStream in(&txt, QIODevice::WriteOnly);
        foreach(const QString &r, defreasons){
            in << r;
        }
        file->open(QFile::WriteOnly);
        file->write(txt);
        file->close();

        readall = txt;
    }
    //3
    QDataStream in(&readall,QIODevice::ReadOnly);
    int sqr = 0;
    while(!in.atEnd()){
        sqr ++;
        QString piecedata;
        in >> piecedata;
        ui->frame->add_item(QString::number(sqr), piecedata,false);
    }
    ui->frame->generat_from_items();
    //4
    ui->widget->setup(QStringList()<<"-"<<"+"<<tr("确定")<<tr("取消"), Qt::AlignRight);
    connect(ui->widget->index_widget("-"), SIGNAL(clicked()),this,SLOT(tosub()));
    connect(ui->widget->index_widget("+"), SIGNAL(clicked()),this,SLOT(toadd()));
    connect(ui->widget->index_widget(tr("确定")), SIGNAL(clicked()),this,SLOT(took()));
    connect(ui->widget->index_widget(tr("取消")), SIGNAL(clicked()),this,SLOT(tocancel()));
    connect(ui->frame,SIGNAL(signal_text(QString)),this, SLOT(updatereason()));
}


lds_dialog_input_expand::~lds_dialog_input_expand()
{
    delete file;
    delete ui;
}

void lds_dialog_input_expand::tosub()
{
    //1
    QStringList texts = ui->frame->hasSelectedText();
    QStringList alltext = ui->frame->curallText();
    int textscurindex;
    if(!texts.isEmpty()){
        foreach (const QString &subr, texts) {
            if((textscurindex=alltext.indexOf(subr)) != -1){
                alltext.removeAt(textscurindex);
            }
        }

        //2
        ui->frame->clearall_item();
        int sqr = 0;
        foreach(const QString &piecedata, alltext){
            sqr ++;
            ui->frame->add_item(QString::number(sqr), piecedata,false);
        }
        ui->frame->generat_from_items();
        ui->frame->setallitemcheck(false);

        updatereason();
        savereason();
    }
}

void lds_dialog_input_expand::toadd()
{
    //1
    lds_dialog_input inputdialog(this);
    inputdialog.ui->label->setText(tr("拒绝理由:"));
    inputdialog.hideKey();
    inputdialog.setWindowTitle(tr("新增拒绝理由"));
    if(QDialog::Accepted == lds_roundeddialog_rect_align(&inputdialog).exec()
            &&!inputdialog.ui->lineEdit->text().isEmpty()){
        QStringList alltext = ui->frame->curallText();
        if(alltext.indexOf(inputdialog.ui->lineEdit->text()) == -1){
            alltext.append(inputdialog.ui->lineEdit->text());
        }

        //2
        ui->frame->clearall_item();
        int sqr = 0;
        foreach(const QString &piecedata, alltext){
            sqr ++;
            ui->frame->add_item(QString::number(sqr), piecedata,false);
        }
        ui->frame->generat_from_items();
        ui->frame->setallitemcheck(false);

        updatereason();
        savereason();
    }
}

void lds_dialog_input_expand::took()
{
    retreason = ui->lineEdit->text();
    this->accept();
}

void lds_dialog_input_expand::tocancel()
{
    this->reject();
}

void lds_dialog_input_expand::updatereason()
{
    ui->lineEdit->clear();
    QStringList reasons = ui->frame->hasSelectedText();
    foreach(const QString &r, reasons){
        ui->lineEdit->insert(r+";");
    }
}

void lds_dialog_input_expand::savereason()
{
    QStringList reasons = ui->frame->curallText();
    QByteArray txt;
    QDataStream in(&txt, QIODevice::WriteOnly);
    foreach(const QString &r, reasons){
        in << r;
    }
    file->open(QFile::WriteOnly);
    file->write(txt);
    file->close();
}

