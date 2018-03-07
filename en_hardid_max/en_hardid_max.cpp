#include "en_hardid_max.h"
#include "ui_en_hardid_max.h"
#include "lds.h"
#include <QMessageBox>
#include <QFile>
#include <QClipboard>

en_hardid_max::en_hardid_max(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::en_hardid_max)
{
    ui->setupUi(this);
}

en_hardid_max::~en_hardid_max()
{
    delete ui;
}

void en_hardid_max::on_pushButton_ok_clicked()
{
    togenerate2();
}

void en_hardid_max::on_pushButton_exit_clicked()
{
    this->reject();
}

void en_hardid_max::togenerate()
{
    QString value_num = QString("%1(%2)")
            .arg(ui->lineEdit_hardid->text())
            .arg(ui->lineEdit_connect_max_id->text());
    if(ui->lineEdit_hardid->text().trimmed().isEmpty()){
        QMessageBox::warning(this, tr("警告"), tr("机器码为空"));
        return;
    }
    if(ui->lineEdit_connect_max_id->text().trimmed().isEmpty()){
        QMessageBox::warning(this, tr("警告"), tr("最大连接数为空"));
        return;
    }

    value_num = lds::of_str_encrypt(value_num);
    QFile file("registration_code.txt");
    file.open(QFile::WriteOnly);
    file.write(value_num.toLocal8Bit());
    file.close();

    QMessageBox::information(this, MESSAGE_TITLE_VOID, tr("当前注册码")+":"+value_num);
}

void en_hardid_max::togenerate2()
{
    QString value_num = QString("%1(%2)")
            .arg(ui->lineEdit_hardid->text())
            .arg(ui->lineEdit_connect_max_id->text());
    if(ui->lineEdit_hardid->text().trimmed().isEmpty()){
        QMessageBox::warning(this, tr("警告"), tr("机器码为空"));
        return;
    }
    if(ui->lineEdit_connect_max_id->text().trimmed().isEmpty()){
        QMessageBox::warning(this, tr("警告"), tr("最大连接数为空"));
        return;
    }

    value_num = lds::of_str_encrypt(value_num);
    value_num = value_num.toLocal8Bit().toHex().toUpper();
    QFile file("registration_code.txt");
    file.open(QFile::WriteOnly);
    file.write(value_num.toLocal8Bit());
    file.close();

    QApplication::clipboard()->setText(value_num);
    QMessageBox::information(this, MESSAGE_TITLE_VOID, tr("当前注册码")+":"+value_num + "\n"
                             + tr("已复制到剪切板"));
}
