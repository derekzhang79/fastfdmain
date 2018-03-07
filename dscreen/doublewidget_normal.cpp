#include "doublewidget_normal.h"
#include "ui_doublewidget.h"
#include <QLabel>

doublewidget_normal::doublewidget_normal(QSize dsize, QWidget *parent)
    : doublewidget(dsize, parent)
{
    doublewidget_layout();
}

void doublewidget_normal::doublewidget_layout()
{
    QVBoxLayout *vlayout = new QVBoxLayout;
    vlayout->setSpacing(0);
    vlayout->setMargin(0);
    vlayout->addWidget(ui->frame_deskinfo);
    vlayout->addWidget(ui->tableView ,1);
    ui->dishlist->setLayout(vlayout);
    ui->dishlist->setStyleSheet(
                "QFrame#dishlist{"
                "border-radius: 20px;"
                "background:transparent;"
                "}");
    ui->label_num->setProperty("outer_stylesheet", "line_doubleview");
    ui->label_total->setProperty("outer_stylesheet", "line_doubleview");
    //
    QLabel *vline = new QLabel;
    vline->setProperty("outer_stylesheet", "label_dish_div");
    QBoxLayout *layout;
    switch(lds::sysconf->value("system_setting/bofang_layout_dir", 0).toInt()){
    case 1://垂直
        layout = new QVBoxLayout;
        vline->setFixedHeight(2);
        setStyleSheet(QString("font-size:%1pt").arg(30));
        //        ui->tableView_m->setFixedHeight(qMin(768, this->height()) / 3);
        ui->tableView_m->setFixedHeight( qMin(1024, this->height()) / 3) ;
        break;
    case 0://水平
    default://默认水平
        layout = new QHBoxLayout;
        vline->setFixedWidth(2);
        setStyleSheet(QString("font-size:%1pt").arg(16));
        ui->dishlist->setFixedWidth( qMin(1024, this->width()) / 3) ;
        break;
    }
    layout->setSpacing(0);
    layout->setMargin(0);
    layout->addWidget(ui->dishlist,1);
    layout->addWidget(vline);
    layout->addWidget(ui->tableView_m,0);
    this->setLayout(layout);

    ///
    ui->tableView->setProperty("outer_stylesheet", "fexpandmain_tableview");
    ui->tableView->horizontalHeader()->hide();
    ui->tableView->verticalHeader()->hide();
    ui->tableView->horizontalHeader()->setStretchLastSection(true);
    ui->tableView->setEditTriggers(QTableView::NoEditTriggers);
    ui->tableView->setStyleSheet("border:0;background:transparent;selection-background-color: transparent;");
    ui->tableView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->tableView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->tableView->setShowGrid(false);
    delegate *d = new delegate;
    ui->tableView->setItemDelegate(d);
    d->keepConnect(ui->tableView);

    start();
}

