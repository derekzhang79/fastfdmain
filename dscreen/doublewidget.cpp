#include "doublewidget.h"
#include "ui_doublewidget.h"
#include <QSettings>
#include <QApplication>
#include <QtDebug>
#include <QFile>
#include <QProcess>
#include <QTimer>
#include <QLabel>
#include <QGraphicsProxyWidget>
#include <QDir>
#include "lds_graphicsview.h"
#include "lds.h"
#include "ui_fexpandmain_payover.h"
#include "erweima.h"
#include <QScrollBar>
#include "lds_picshow_item.h"
#include "tableview_item.h"
#include "testwx_help.h"
#include "lds_thread.h"

const QString doublewidget::parentPID;

void doublewidget::showPayinfo(QWidget *tableview, QFrame *&pay, const lds::DOUBLE_DATA &d)
{
    if(pay){delete pay; pay = 0;}
    //1
    Ui_fexpandmain_payover *pui = new Ui_fexpandmain_payover;
    pay = new QFrame(tableview);
    pui->setupUi(pay);
    int width = qMin(tableview->height(), tableview->width())*5/6;
    pay->resize(width, width*3/4);
    pay->move(lds::getParentCenterPopup(tableview, pay));

    //2
    pui->label_num_payamount->setText(d.p_pay);
    pui->label_num_realamount->setText(d.p_receive);
    pui->label_num_change->setText(d.p_change);
    pui->label_desc->setText(d.p_desc);
    lds::hideWidget(pui->frame_explain);

    qDebug() << "label_num_payamount:" << pui->label_num_payamount->font();
    qDebug() << "label_num_realamount:" << pui->label_num_realamount->font();
    qDebug() << "label_num_change:" << pui->label_num_change->font();
    qDebug() << "label_desc:" << pui->label_desc->font();
    pay->raise();
    pay->show();
}

doublewidget::doublewidget(QSize dsize, QWidget *parent) :
    QDialog(parent),
    ui(new Ui_doublewidget),
    paywidget(0)
{
    ui->setupUi(this);
    lds::setwflagFrameless(this);
    dishimagepath = lds::sysconf->value("system_setting/bofanglujing").toString();

    QString advimagepath = lds::localdataPath +"/image_double_screen_dir";
    resize(dsize);

    standmodel = new QStandardItemModel(this);
    ui->tableView->setModel(standmodel);

    //1 监听
    udpSocket= new QUdpSocket(this);
    udpSocket->bind(QHostAddress::LocalHost, lds::SYS_DOUBLE_SCREEN_PORT);
    connect(udpSocket,SIGNAL(readyRead()),this,SLOT(processPendingDatagrams()));
    //2
    //    expand=new tableview_item(this);
    ui->tableView_m->initScene(advimagepath,
                               tableview_item::textData(
                                   lds::sysconf->value("system_setting/bofangg_runningtext").toString(),
                                   lds::sysconf->value("system_setting/bofangg_runningtext_size", 80).toInt(),
                                   lds::sysconf->value("system_setting/bofangg_runningtext_color").toString()
                                   )
                               );
    //3
    dishtimeouttimer = new QTimer(this);
    connect(dishtimeouttimer,SIGNAL(timeout()), this, SLOT(adc_timer_out()));
    //4
    //    trytoclosetimer = new QTimer(this);
    //    trytoclosetimer->setInterval(1*1000);
    //    connect(trytoclosetimer,SIGNAL(timeout()),this,SLOT(trytoclose()));

#ifdef QT_DEBUG
    dishtimeouttimer->setInterval(10*1000);
    ui->tableView_m->showPixmap("C:/Users/cz/Desktop/餐饮window版/fastfd_localdata/image_dish_dir_2/11102.png");
#else
    dishtimeouttimer->setInterval(60*1000);
#endif
}

doublewidget::~doublewidget()
{
    delete ui;
    if(paywidget) delete paywidget;
}

void doublewidget::start()
{
    this->show();
    adc_timer_start();
    adc_timer_out();
}

void doublewidget::toquit()
{
    qApp->quit();
}

void doublewidget::resizeEvent(QResizeEvent *e)
{
    QDialog::resizeEvent(e);
    ui->tableView_m->text_rect_rasie();
}

void doublewidget::moveEvent(QMoveEvent *e)
{
    QDialog::moveEvent(e);
    ui->tableView_m->text_rect_rasie();
}

void doublewidget::adc_timer_start()
{
    lds::showWidget(ui->dishlist);
    dishtimeouttimer->start();
    switch(lds::sysconf->value("system_setting/bofang_layout_dir", 0).toInt()){
    case 0://水平
        break;
    case 1://垂直
        //        ui->tableView_item->setMinimumHeight(ui->tableView_item->idealh());
        break;
    default://默认水平
        break;
    }
}

void doublewidget::adc_timer_out()
{
    ui->tableView_m->start();
    switch(lds::sysconf->value("system_setting/bofang_layout_dir", 0).toInt()){
    case 0://水平
        lds::hideWidget(ui->dishlist);
        break;
    case 1://垂直
        lds::showWidget(ui->dishlist);
        //        ui->graphicsView->setMinimumHeight(ui->graphicsView->idealh());
        break;
    default://默认水平
        lds::hideWidget(ui->dishlist);
        break;
    }
}

void doublewidget::standmodel_setdata(int row, const QString &p0, const QString &p1, const QString &p2, const QString &p3, const QString &p4)
{
    standmodel->item(row, 0)->setText(p0 + "\t" + p1 + "\t" + p2 + "\t" + p3 + "\t" + p4);
}

void doublewidget::standmodel_insetdata(int row, const QString &p0, const QString &p1, const QString &p2, const QString &p3, const QString &p4)
{
    standmodel->insertRow(row, QList<QStandardItem *> ()
                          << new QStandardItem
                          );
    standmodel_setdata(row, p0, p1, p2, p3, p4);
}

void doublewidget::total_settext(const QString &text)
{
    ui->label_total->setText(text);
}

void doublewidget::num_settext(const QString &text)
{
    ui->label_num->setText(text);
}

void doublewidget::adc_show()
{

}

QString doublewidget::class_name()
{
    return "doublewidget";
}

void doublewidget::trytoclose()
{
}

void doublewidget::processPendingDatagrams()
{
    //不显示菜品
    if(false == lds::sysconf->value("system_setting/dd_show_dish_list", true).toBool())  return;

    QString picdishno;
    QString sum_total_desc;
    QString sum_num_desc;
    QPixmap pixmap_rbcode_begin;
    QPixmap pixmap_rbcode_end;
    int currow = -1;

    while(udpSocket->hasPendingDatagrams()){
        QByteArray datagram;
        datagram.resize(udpSocket->pendingDatagramSize());
        udpSocket->readDatagram(datagram.data(), datagram.size());
#ifdef Q_OS_WIN
        lds_thread::msecSleep(5);//window下解决办法
#endif
        lds::DOUBLE_DATA data;
        lds::DOUBLE_DATA_output(datagram, data);

        //开始处理
        if(data.udpKey == lds::udp_close_request){//关闭请求
            qApp->quit();
            return;
        }
        if(data.udpKey == lds::udp_insert_request) {
            standmodel_insetdata(data.row, data.items.vch_dishname, data.items.dish_price, data.items.dish_num, data.items.dish_total, data.items.vch_other);
            currow = data.row;
            if(!data.items.ch_suitflag.isEmpty() && data.items.ch_suitflag != "Y") {//P Y N
                picdishno =data.items.ch_dishno;
            }
            if(paywidget) { delete paywidget; paywidget = 0;  }
        }
        if(data.udpKey == lds::udp_update_request) {
            standmodel_setdata(data.row,  data.items.vch_dishname, data.items.dish_price, data.items.dish_num, data.items.dish_total, data.items.vch_other);
            currow = data.row;
            if(!data.items.ch_suitflag.isEmpty() && data.items.ch_suitflag != "Y") {//P Y N
                picdishno =data.items.ch_dishno;
            }
        }
        if(data.udpKey == lds::udp_delete_all_request) {
            standmodel->removeRows(0, standmodel->rowCount());
        }
        if(data.udpKey == lds::udp_delete_request) {
            standmodel->removeRows(data.row, data.count);
            currow = data.row;
        }

        if(data.udpKey == lds::udp_rb_code_request_begin){
            pixmap_rbcode_begin = testwx_help::get_rbcode(data.rb_title,
                                                          data.rb_wechat.toLocal8Bit().data(), data.rb_title_wechat,
                                                          data.rb_alipay.toLocal8Bit().data(), data.rb_title_alipay,
                                                          data.rb_ch_paymodeno);
//            data.rb_code.toLocal8Bit().data(), 250, data.rb_title, data.rb_desc, QColor("#ff650b"), 50);
        }
        if(data.udpKey == lds::udp_rb_code_request_end) {

            pixmap_rbcode_end = QPixmap(250, 250);
            QPainter p(&pixmap_rbcode_end);
            p.setPen(QColor("#ff650b"));
            p.fillRect(QRect(0, 0, pixmap_rbcode_end.width(), pixmap_rbcode_end.height()), QColor("white"));
            p.drawText(QRect(0, 0, pixmap_rbcode_end.width(), pixmap_rbcode_end.height()), Qt::AlignCenter, data.rb_title);
        }
        if(data.udpKey == lds::udp_title_request) {
            sum_num_desc = data.sum_num_desc;
            sum_total_desc = data.sum_total_desc;
        }

        if(data.udpKey == lds::udp_pay_request) {
            showPayinfo(ui->dishlist, paywidget, data);
        }

        if(data.udpKey == lds::udp_m_hlabels_request) {
            if(class_name() == "doublewidget_1024x600") {
                standmodel->setHorizontalHeaderLabels(data.m_hlabels.split(","));
            }
            *(QString*)&parentPID = QString::number(data.parentPID);
        }
    }



    if(!sum_num_desc.isEmpty())  num_settext(sum_num_desc);
    if(!sum_total_desc.isEmpty())  total_settext(sum_total_desc);

    if(!sum_num_desc.isEmpty() || !sum_total_desc.isEmpty()) {
        adc_timer_start();
    }
    if(currow >= 0)
        ui->tableView->selectRow(currow);

    if(!picdishno.isEmpty()){//菜品图片
        QPixmap m(dishimagepath+"/" + picdishno);
        if(m.isNull()) {
            ui->tableView_m->start();
        } else {
            ui->tableView_m->showPixmap(m);
        }
    }

    //显示二维码，则不会在启动广告
    if(!pixmap_rbcode_begin.isNull()) {//二维码
        dishtimeouttimer->stop();
        ui->tableView_m->showPixmap(pixmap_rbcode_begin);

        adc_show();
    }

    //显示二维码，则不会在启动广告
    if(!pixmap_rbcode_end.isNull()) {//二维码
        dishtimeouttimer->start();
        ui->tableView_m->showPixmap(pixmap_rbcode_end);
        adc_show();
    }
}


void delegate::keepConnect(fexpandmain_table *tableview)
{
    divColor = &tableview->divColor;
    nameColor = &tableview->nameColor;
    nameSubColor = &tableview->nameSubColor;
    border = &tableview->border;
    defColor = &tableview->defColor;
    itemBgColor = &tableview->itemBgColor;
}

void delegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    if(index.column() == 0) {
        const QRect option_rect = option.rect;
        const QStandardItemModel *model = static_cast< const QStandardItemModel *>(index.model());
        int row = index.row();
        QFont font = painter->font();

        QRect rect1 = QRect(option_rect.x(), option_rect.y(), option_rect.width()/7.0, option_rect.height());
        QRect rect2 = QRect(option_rect.x()+option_rect.width()*1/7.0, option_rect.y()+0, option_rect.width()*4/7.0, option_rect.height()*3/5.0);
        QRect rect3 = QRect(option_rect.x()+option_rect.width()*5/7.0, option_rect.y()+0, option_rect.width()*2/7.0, option_rect.height()*3/5.0);
        QRect rect4 = QRect(option_rect.x()+option_rect.width()*1/7.0, option_rect.y()+option_rect.height()*3/5.0, option_rect.width()*5/7.0, option_rect.height()*2/5.0);

        QStringList list = model->item(row, 0)->text().split("\t");
        QString vch_dishname = list.value(0);
        QString dish_price = list.value(1);
        QString dish_num = list.value(2);
        QString dish_total = list.value(3);
        QString vch_other = list.value(4);

        //开始绘制
        if(option.state & QStyle::State_Selected) {
            painter->fillRect(option_rect, QColor("#ff650b"));
        } else {
            painter->fillRect(option_rect, QColor("transparent"));
        }
        //下边黑线
        painter->setPen(Qt::NoPen);
        painter->setBrush(*divColor);
        painter->drawRect(option_rect.adjusted(0, option_rect.height()-*border, 0, 0));
        //文本
        painter->setPen(*defColor);
        font.setPixelSize(rect1.height() * 2 / 5.0);painter->setFont(font);
        painter->drawText(rect1, int(Qt::AlignCenter), QString::number(row + 1));

        font.setPixelSize(rect2.height() * 2 / 4.0);painter->setFont(font);
        painter->drawText(rect2, int(Qt::AlignLeft|Qt::AlignVCenter), vch_dishname);
        painter->drawText(rect3, int(Qt::AlignRight|Qt::AlignVCenter), dish_total);

        font.setPixelSize(rect4.height() * 2 / 3.0);painter->setFont(font);
        painter->drawText(rect4, int(Qt::AlignLeft | Qt::AlignTop), QString("%1x%2 %3").arg(dish_price).arg(dish_num).arg(vch_other));
    } else {
        QStyledItemDelegate::paint(painter, option, index);
    }
}
