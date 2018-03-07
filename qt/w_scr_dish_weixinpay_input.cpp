#include "w_scr_dish_weixinpay_input.h"
#include "public_sql.h"
#include "lds.h"

void w_scr_dish_weixinpay_input::to_qr_ali()
{
    this->done(qr_ALIED);
}

void w_scr_dish_weixinpay_input::to_qr_wx()
{
    this->done(qr_WXED);
}

w_scr_dish_weixinpay_input::w_scr_dish_weixinpay_input(QWidget *parent, const QString &ch_paymodeno) :
    QDialog(parent),
    ui(new Ui::w_scr_dish_weixinpay_input)
{
    ui->setupUi(this);
    this->ch_paymodeno = ch_paymodeno;

    ui->lineEdit_payno->hide();
    ui->label_payno->hide();

    if(ch_paymodeno == "BB"//快捷支付
            || ch_paymodeno == "AA") {//移动支付
        ui->frame_bar->setup(QStringList() << tr("确定") << tr("支付宝") << tr("微信") << tr("取消"), Qt::AlignRight);
        connect(ui->frame_bar->index_widget(tr("微信")), SIGNAL(clicked()), this,SLOT(to_qr_wx()));
        connect(ui->frame_bar->index_widget(tr("支付宝")), SIGNAL(clicked()), this,SLOT(to_qr_ali()));
    }
    if(ch_paymodeno == "05") {//微信
        ui->frame_bar->setup(QStringList() << tr("确定") << tr("取消"), Qt::AlignRight);
    }
    if(ch_paymodeno == "06") {//支付宝
        ui->frame_bar->setup(QStringList() << tr("确定") << tr("取消"), Qt::AlignRight);
    }

    connect(ui->frame_bar->index_widget(tr("确定")), SIGNAL(clicked()), this, SLOT(accept()));
    connect(ui->frame_bar->index_widget(tr("取消")), SIGNAL(clicked()), this, SLOT(reject()));

}

w_scr_dish_weixinpay_input::~w_scr_dish_weixinpay_input()
{
    delete ui;
}

void w_scr_dish_weixinpay_input::updatetype(int index, const QString &title)
{
    ui->lineEdit_payno->setVisible(index==0);
    ui->label_payno->setVisible(index==0);
    this->setWindowTitle(title);
    if(index == 0){
        ui->lineEdit_payno->setFocus();
    }

    if(ch_paymodeno == "AA"
            ||ch_paymodeno == "BB") {
        if(index == 0){//扫手机条码
            ui->frame_bar->index_widget(0/*tr("确定")*/)->show();
            ui->frame_bar->index_widget(1/*tr("微信")*/)->hide();
            ui->frame_bar->index_widget(2/*tr("支付宝")*/)->hide();
        } else {
            ui->frame_bar->index_widget(0/*tr("确定")*/)->hide();
            ui->frame_bar->index_widget(1/*tr("微信")*/)->show();
            ui->frame_bar->index_widget(2/*tr("支付宝")*/)->show();
        }
    }
}

void w_scr_dish_weixinpay_input::toalipaytype(int index)
{
    lds::sysconf->setValue("w_bt_dish_paymode_alipay_procedure_set/"+ui->comboBox->itemData(index).toString(), index);
    updatetype(index,index==0?tr("条码支付"):tr("扫码支付"));
}

