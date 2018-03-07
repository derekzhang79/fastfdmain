#include "doublewidget_1024x600.h"
#include "ui_doublewidget.h"
#include "lds_picshow_item.h"
#include "tableview_item.h"

doublewidget_1024x600::doublewidget_1024x600(QSize dsize, QWidget *parent)
    : doublewidget(dsize, parent)
{
    blank = "    ";
    doublewidget_layout();

#ifdef QT_DEBUG
    QTimer::singleShot(1000, this, SLOT(delayShowPayTest()));
#endif
}

void doublewidget_1024x600::doublewidget_layout()
{
    QString fontpt = "40";
    QString totalpt = "50";
    QString vheaderh = "50";
    QVBoxLayout *vlayout = new QVBoxLayout;
    vlayout->setSpacing(0);
    vlayout->setMargin(0);
    vlayout->addWidget(ui->tableView ,1);
    vlayout->addWidget(ui->frame_deskinfo);
    ui->dishlist->setLayout(vlayout);
    ui->dishlist->setStyleSheet(
                "QFrame#dishlist{"
                "border-radius: 20px;"
                "background:black;"
                "}");
    ui->frame_deskinfo->setStyleSheet("border:0px;");
    ui->frame_deskinfo->layout()->setSpacing(0);
    ui->frame_deskinfo->layout()->setMargin(0);
    setStyleSheet(
                "font-size:" +fontpt+ "pt; background-color:black; color:white;"
                );
    ui->tableView->setStyleSheet(
                "QTableView::item{"
                "border-bottom:2px solid  white;"
                "}"
                "QTableView::item:selected{"
                "color: white;"
                "}"
                "QHeaderView::section{ "
                "min-height:"+vheaderh+"px;"
                "font-size:"+fontpt+"pt;"
                "border:0px; background-color: qlineargradient(spread:reflect, x1:0, y1:0, x2:0, y2:1, stop:0 #2b2b2b, stop:1 black);"
                "border-bottom:2px solid #ff650b;"
                "}");

    //
    QHBoxLayout *layout = new QHBoxLayout;
    layout->setSpacing(0);
    layout->setMargin(0);
    layout->addWidget(ui->dishlist,1);
    layout->addWidget(ui->tableView_m,0);
    this->setLayout(layout);
    //
    ui->dishlist->show();
    ui->tableView_m->hide();
    ui->label_num->setStyleSheet("font-size:"+totalpt+"pt;");
    ui->label_total->setStyleSheet("font-size:"+totalpt+"pt;");
    //

    standmodel->setHorizontalHeaderLabels(QStringList() << "" << "" << "" << "");//tr("名称") << tr("价格") << tr("数量") << tr("金额"));
    ui->tableView->setColumnWidth(0, 400);
    int w1 = (this->width() - 400) / 3;
    ui->tableView->setColumnWidth(1, w1);
    ui->tableView->setColumnWidth(2, w1);
    ui->tableView->setColumnWidth(3, w1);
    ui->tableView->horizontalHeader()->show();
    ui->tableView->verticalHeader()->hide();
    ui->tableView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->tableView->setSelectionMode(QTableView::SingleSelection);
    ui->tableView->setSelectionBehavior(QTableView::SelectRows);
        ui->tableView->setShowGrid(false);
    ui->tableView->setFocusPolicy(Qt::NoFocus);

    int vdefaultw = (this->height()-vheaderh.toInt() ) / 5;
    ui->tableView->verticalHeader()->setDefaultSectionSize(vdefaultw);
    ui->frame_deskinfo->setFixedHeight(vdefaultw);
    //    ui->label_total->setFixedWidth(w1);
    ui->label_total->setAlignment(Qt::AlignRight);
    ui->label_num->setAlignment(Qt::AlignRight);


    standmodel_insetdata(0, "hello1", "12.00", "11.00", "23.00", "");
    standmodel_insetdata(0, "hello2", "12.00", "11.00", "23.00", "");
    standmodel_insetdata(0, "hello3", "12.00", "11.00", "23.00", "");
    standmodel_insetdata(0, "hello4", "12.00", "11.00", "23.00", "");
    standmodel_insetdata(0, "hello5", "12.00", "11.00", "23.00", "");
    standmodel_insetdata(0, "hello6", "12.00", "11.00", "23.00", "");
    num_settext(tr("数量100.12"));
    total_settext(tr("总计100.12"));

    //开始广告，并显示
    this->start();
}

void doublewidget_1024x600::adc_timer_start()
{
    ui->dishlist->show();
    ui->tableView_m->hide();
    dishtimeouttimer->start();/*
    switch(lds::sysconf->value("system_setting/bofang_layout_dir", 0).toInt()){
    case 0://水平
        break;
    case 1://垂直
        ui->graphicsView->setMinimumHeight(ui->graphicsView->idealh());
        break;
    default://默认水平
        break;
    }*/
}

void doublewidget_1024x600::adc_timer_out()
{
    ui->tableView_m->start();
    ui->tableView_m->show();
    ui->dishlist->hide();
}

void doublewidget_1024x600::standmodel_setdata(int row, const QString &p0, const QString &p1, const QString &p2, const QString &p3, const QString &p4)
{
    standmodel->item(row, 0)->setText(QString::number(row+1) + "." +p0);
    standmodel->item(row, 1)->setText(p1+blank);//单价
    standmodel->item(row, 2)->setText(p2+blank);//数量
    standmodel->item(row, 3)->setText(p3+blank);//金额
    //    standmodel->item(row, 4)->setText(p4);

    standmodel->item(row, 1)->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
    standmodel->item(row, 2)->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
    standmodel->item(row, 3)->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
}

void doublewidget_1024x600::standmodel_insetdata(int row, const QString &p0, const QString &p1, const QString &p2, const QString &p3, const QString &p4)
{
    standmodel->insertRow(row, QList<QStandardItem *> ()
                          << new QStandardItem
                          << new QStandardItem
                          << new QStandardItem
                          << new QStandardItem
                          );
    standmodel_setdata(row, p0, p1, p2, p3, p4);
}

void doublewidget_1024x600::total_settext(const QString &text)
{
    ui->label_total->setText(text+blank);
}

void doublewidget_1024x600::num_settext(const QString &text)
{
    ui->label_num->setText(text + blank);
}

void doublewidget_1024x600::adc_show()
{
    ui->dishlist->hide();
    ui->tableView_m->show();
}

QString doublewidget_1024x600::class_name()
{
    return "doublewidget_1024x600";
}

void doublewidget_1024x600::delayShowPayTest()
{
    dishtimeouttimer->stop();
    qDebug() << __FUNCTION__;
    ui->dishlist->show();
    ui->tableView_m->hide();

    lds::DOUBLE_DATA d;
    d.p_pay = "p_pay";
    d.p_receive = "p_receive";
    d.p_change = "p_change";
    d.p_desc = "p_desc";

    showPayinfo(ui->dishlist, paywidget, d);
}
