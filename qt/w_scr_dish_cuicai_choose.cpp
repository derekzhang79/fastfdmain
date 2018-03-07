#include "w_scr_dish_cuicai_choose.h"
#include "lds_query.h"

#include <QtDebug>
#include "lds_messagebox.h"
#include "public_sql.h"

w_scr_dish_cuicai_choose::w_scr_dish_cuicai_choose(QWidget *parent) :
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
    //!---------------------------------2end

    ui->widget->setup(QStringList() << tr("确定") << tr("取消"), Qt::AlignRight);
    connect(ui->widget->index_widget(tr("确定")),SIGNAL(clicked()),this,SLOT(took()));
    connect(ui->widget->index_widget(tr("取消")),SIGNAL(clicked()),this,SLOT(toexit()));

    ui->label_pre->show();
    ui->label_pre->setText(tr("可多选"));
    ui->label_state->clear();
}

w_scr_dish_cuicai_choose::~w_scr_dish_cuicai_choose()
{
    delete ui;
}

void w_scr_dish_cuicai_choose::reloaddata(const QList< QPair<QString, QString> > &data)
{
    for(int index = 0,indexcount = data.count(); index < indexcount; index++){
        ui->frame_cook->add_item(data[index].first, data[index].second, false);
    }
    ui->frame_cook->generat_from_items();
}

QStringList w_scr_dish_cuicai_choose::hasSelected()
{
    return ui->frame_cook->hasSelected();
}

void w_scr_dish_cuicai_choose::took()
{
    if(ui->frame_cook->hasSelected().isEmpty()){
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, tr("没有类别选中"));
        return;
    }
    this->accept();
}

void w_scr_dish_cuicai_choose::toexit()
{
    this->reject();
}
