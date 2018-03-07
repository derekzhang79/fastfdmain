#include "w_scr_dish_select_operid.h"
#include "lds_query.h"

#include <QtDebug>
#include "lds_messagebox.h"
#include "public_sql.h"
#include "backheader.h"

w_scr_dish_select_operid::w_scr_dish_select_operid(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::w_scr_dish_special_cook)
{
    ui->setupUi(this);
    //!---------------------------------1begin
    ui->frame_cook->is_like_tab=false;
    int horizontal_count;
    int vertical_count;
    ui->frame_cook->tool_size=QSize(96,80);
    horizontal_count=5;
    vertical_count=3;
    ui->frame_cook->horizontal_count=horizontal_count;
    ui->frame_cook->vertical_count=vertical_count;
    ui->frame_cook->is_like_tab = true;
    //!---------------------------------2end
    lds_query query;
    query.execSelect("select vch_operID, concat(vch_operator,'\n',vch_operID) vch_operator from ("+backheader::SELECT_OPERID_NAME_FLAG+")t");
    while(query.next()){
        ui->frame_cook->add_item(query.recordValue("vch_operID").toString(), query.recordValue("vch_operator").toString(), false);
    }
    ui->frame_cook->generat_from_items();

    ui->widget->setup(QStringList() << tr("确定") << tr("取消"), Qt::AlignRight);
    connect(ui->widget->index_widget(tr("确定")),SIGNAL(clicked()),this,SLOT(took()));
    connect(ui->widget->index_widget(tr("取消")),SIGNAL(clicked()),this,SLOT(toexit()));

    ui->label_pre->show();
    ui->label_state->clear();
    ui->label_return->hide();
}

w_scr_dish_select_operid::~w_scr_dish_select_operid()
{
    delete ui;
}

void w_scr_dish_select_operid::setOperid(const QString &id)
{
    ui->frame_cook->setCurrentIndex(id);
}

QStringList w_scr_dish_select_operid::hasSelected()
{
    return ui->frame_cook->hasSelected();
}

QString w_scr_dish_select_operid::selectOperid()
{
    return hasSelected().value(0);
}

void w_scr_dish_select_operid::took()
{
    if(ui->frame_cook->hasSelected().isEmpty()){
        switch(lds_messagebox::warning(this, MESSAGE_TITLE_VOID, tr("没有行被选中"), tr("继续退出"), tr("返回"))){
        case 0:
            break;
        case 1:
            return;
        }
    }
    this->accept();
}

void w_scr_dish_select_operid::toexit()
{
    this->reject();
}
