#include "w_scr_dish_num_cook_dialog.h"
#include "ui_w_scr_dish_num_cook_dialog.h"
#include "lds_query.h"
#include "w_bt_dish_cookset.h"
#include <QPointer>
#include <QtDebug>
#include <QKeyEvent>
#include "public_sql.h"
#include "lds_dialog_input.h"
#include "lds.h"
#include "fexpandmain_2.h"
#include "take_weight.h"
#include "fexpandmain_model_sqltablemodel.h"
#include "lds_menu.h"


void w_scr_dish_num_cook_Dialog::static_to_present(QWidget *parent, fexpandmain_model_sqltablemodel *tablemodel, int row, float num_num)
{
    QDateTime dt = n_func::f_get_sysdatetime();
    lds_query query;
    //权限，赠送上限
    if(public_sql::gs_operid != "0000") {
        query.execSelect(QString("select num_present from cey_sys_operator where vch_operID ='%1' and ch_flag = 'Y' ").arg(public_sql::gs_operid));
        query.next();
        float dec_presenttop = query.recordValue("num_present").toFloat();
        if(num_num > dec_presenttop) {
            lds_messagebox::warning(parent, MESSAGE_TITLE_VOID,tr("赠送金额超过赠送上限,请到【操作员信息->送菜金额】调整"));
            return ;
        }
    }
    //赠送
    if(tablemodel->model_data(row, "ch_presentflag").toString() != "Y"){
        if(0==lds_messagebox::question(parent, MESSAGE_TITLE_VOID,tr("是否确定将此菜品赠送?"), tr("确认"), tr("取消"))){
            tablemodel->model_data_set(row, "ch_presentflag", "Y");
            tablemodel->model_data_set(row, "vch_pre_operID", public_sql::gs_operid);
            tablemodel->model_data_set(row, "dt_pre_operdate", dt);
            tablemodel->model_data_set(row, "int_discount", 100);
            return ;
        }
    } else {
        if(0==lds_messagebox::question(parent, MESSAGE_TITLE_VOID,tr("此菜品已经被赠送,是否取消赠送?"), tr("确认"), tr("取消"))) {
            if(tablemodel->model_data(row, "vch_pre_operID").toString() == public_sql::gs_operid){
                tablemodel->model_data_set(row, "ch_presentflag", "N");
                tablemodel->model_data_set(row, "vch_pre_operID", "");
                tablemodel->model_data_set(row, "dt_pre_operdate", lds::dtifnull());
            } else {
                lds_messagebox::warning(parent, MESSAGE_TITLE_VOID, tr("此菜品已经被赠送，取消赠送必须是:") +
                                        tablemodel->model_data(row, "vch_pre_operID").toString());
                return;
            }
        }
    }
}

w_scr_dish_num_cook_Dialog::w_scr_dish_num_cook_Dialog(QWidget *parent) :
    QDialog(parent)
{
    intData();
}

w_scr_dish_num_cook_Dialog::w_scr_dish_num_cook_Dialog(const fexpandmain_model_sqltablemodel_data *model, int row, QWidget *parent) :
    QDialog(parent)
{
    intData();
    refresh(model, row);
}

void w_scr_dish_num_cook_Dialog::enable_cook(bool f) const
{
    ui->pushButton_allcook->setEnabled(f);
    ui->pushButton_clearcook->setEnabled(f);
    ui->pushButton_take_weight->setEnabled(f);
    ui->frame_cook->setEnabled(f);
    ui->label_return_cal->setEnabled(f);
}

w_scr_dish_num_cook_Dialog::~w_scr_dish_num_cook_Dialog()
{
    delete ui;
}

void w_scr_dish_num_cook_Dialog::retranslateView()
{
    ui->retranslateUi(this);
}

void w_scr_dish_num_cook_Dialog::commitModel(fexpandmain_model_sqltablemodel *frommodel, int row)
{
    float num_num_old = frommodel->model_data_NIBOLAN(row, "num_num-num_back");
    for(int c = 0, cc = tablemodel->columnCount(); c < cc; c++) {
        if(c == tablemodel->fieldIndex("int_flowid")) continue;

        frommodel->model_data_set(row, c , tablemodel->model_data(0, c));
    }
    if(frommodel->model_data(row, "ch_suitflag").toString() == "Y") {
        frommodel->update_num_price_add_2_P(row);
    }
    if(frommodel->model_data(row, "ch_suitflag").toString() == "P") {
        frommodel->update_num_num_Y(num_num_old, row);
    }
}

void w_scr_dish_num_cook_Dialog::intData()
{
    ui = new Ui::w_scr_dish_num_cook_Dialog;
    ui->setupUi(this);
    tablemodel = new fexpandmain_model_sqltablemodel(this);
    tablemodel->setEditStrategy(lds_model_sqltablemodel::OnManualSubmit);
    frommodel =  0;
    //!---------------------------------1begin
    ui->frame_cook->is_like_tab=false;
    int horizontal_count;
    int vertical_count;
    ui->frame_cook->tool_size = QSize(100,100);
    horizontal_count=3;
    vertical_count=3;
    ui->frame_cook->horizontal_count=horizontal_count;
    ui->frame_cook->vertical_count=vertical_count;
    //!---------------------------------2end
    //
    ui->lineEdit_vch_dishname->setEnabled(false);
    connect(ui->pushButton_allcook, SIGNAL(clicked()),this,SLOT(toallcook()));
    connect(ui->pushButton_clearcook, SIGNAL(clicked()),this,SLOT(toclearcook()));
    connect(ui->pushButton_ok, SIGNAL(clicked()),this,SLOT(took()));
    connect(ui->pushButton_cancel, SIGNAL(clicked()),this,SLOT(tocancel()));
    connect(ui->pushButton_present, SIGNAL(clicked()),this,SLOT(topresent()));
    connect(ui->pushButton_take_weight, SIGNAL(clicked()),this,SLOT(totakeweight()));
    connect(ui->pushButton_cookset, SIGNAL(clicked()),this,SLOT(tocookset()));

    connect(ui->frame_cook,SIGNAL(signal_text(QString)),this,SLOT(toupdatelabelreturn()));//点击
    connect(ui->frame_cook,SIGNAL(signal_rb_pressed(frontViewDialog_toolbtn *)),this,SLOT(toupdatelabelreturn(frontViewDialog_toolbtn *)));//修改数量时
    connect(ui->label_return_cal, SIGNAL(textChanged(QString)), this,SLOT(togenaratcook(QString)));
    ui->label_err->hide();


    label_info  = new QTextEdit(this);
    label_info->setMinimumSize(300, 300);
    label_info->setStyleSheet("color:red;");
    lds_menu *menu=new lds_menu(this);
    QWidgetAction *widgetaction = new QWidgetAction(ui->pushButton_more);
    widgetaction->setDefaultWidget(label_info);
    menu->addAction(widgetaction);
    ui->pushButton_more->setMenu(menu);

    ui->pushButton_more->hide();
    label_info->hide();
    if(lds::get_soft_curent_language_first() == "CN") {
        label_info->show();
        ui->pushButton_more->show();
    }

    ui->frame_cook->enable_dollar();
    ui->pushButton_present->hide();
    ui->doubleSpinBox_num_num->installEventFilter(this);
    ui->doubleSpinBox_num_price->installEventFilter(this);
    ui->lineEdit_vch_dishname->installEventFilter(this);
    ui->spinBox_int_discount->installEventFilter(this);
    ui->pushButton_allcook->installEventFilter(this);
    ui->pushButton_cancel->installEventFilter(this);
    ui->pushButton_clearcook->installEventFilter(this);
    ui->pushButton_more->installEventFilter(this);
    ui->pushButton_ok->installEventFilter(this);
    ui->pushButton_present->installEventFilter(this);
    ui->pushButton_take_weight->installEventFilter(this);
    ui->label_return_cal->installEventFilter(this);

    setWindowTitle(tr("详细"));
}

void w_scr_dish_num_cook_Dialog::refresh(const fexpandmain_model_sqltablemodel_data *model, int row)
{
    ui->doubleSpinBox_num_num->setDecimals(model->get_cey_sys_parameter_value("bp_fmt_num_dec", 3).toInt());
    ui->doubleSpinBox_num_price->setDecimals(model->get_cey_sys_parameter_value("bp_fmt_price_dec", 2).toInt());

    //init
    if(frommodel == 0) {
        frommodel = model;
        tablemodel->setTable(frommodel->tableName());
    }

    //refresh
    tablemodel->removeRows(0, tablemodel->rowCount());
    //1
    tablemodel->insertRow(0);
    for(int c = 0, cc = frommodel->columnCount(); c < cc; c++) {
        tablemodel->model_data_set(0, c,  frommodel->model_data(row, c));
    }
    refresh_sub(row);
}

double w_scr_dish_num_cook_Dialog::get_cur_dish_num()
{
    return ui->doubleSpinBox_num_num->value();
}

void w_scr_dish_num_cook_Dialog::refresh_sub(int row)
{
    bool flag;
    label_info->clear();
    //2
    QString ch_dishno = frommodel->model_data(row, "ch_dishno").toString();
    ui->doubleSpinBox_num_num->setValue(frommodel->model_data_NIBOLAN(row, "num_num - num_back"));//这边若是已经落单，则无法修改数量，
    ui->doubleSpinBox_num_price->setValue(frommodel->model_data(row, "num_price").toDouble());
    ui->lineEdit_vch_dishname->setText(frommodel->get_vch_dishname_by_map(ch_dishno));
    ui->spinBox_int_discount->setValue(frommodel->model_data(row, "int_discount").toInt());
    if(public_sql::discount_off_mode() == "1") {
        ui->spinBox_int_discount->enableValueMaxMinus();
    }


    //3
    QString vch_print_memo = frommodel->model_data(row, "vch_print_memo").toString();
    QMap<QString, int> map = frommodel->get_map_vch_print_memo(vch_print_memo);
    QStringList ch_no_list = map.keys();
    bool vch_print_memo_not_find_int_cey_bt_cp_dish = false;
    lds_query query;
    //查找当前做法是否在菜品做法里
    foreach(const QString &ch_no, ch_no_list) {
        query.execSelect(QString("SELECT ch_no from cey_bt_cp_dish where ch_dishno = '%1'  and ch_no = '%2' ;  ")
                         .arg(ch_dishno)
                         .arg(ch_no));
        if(!query.next()) {
            vch_print_memo_not_find_int_cey_bt_cp_dish = true;
            break;
        }
    }
    //
    ui->frame_cook->clearall_item();
    if(vch_print_memo_not_find_int_cey_bt_cp_dish) {
        query.execSelect(QString("select a.ch_no,a.vch_cp_memo,a.ch_typeno,a.num_add_price FROM cey_bt_cp_memo a"));
    } else {
        query.execSelect(QString("select a.ch_no,a.vch_cp_memo,a.ch_typeno,a.num_add_price FROM cey_bt_cp_memo a,cey_bt_cp_dish b WHERE a.ch_no = b.ch_no and  b.ch_dishno = '%1' ")
                         .arg(ch_dishno));
    }
    while(query.next()){
        ui->frame_cook->add_item(query.recordValue("ch_no").toString(),
                                 QString("%1\n%2").arg(query.recordValue("vch_cp_memo").toString()).arg(query.recordValue("num_add_price").toString()),
                                 false);
    }
    ui->frame_cook->generat_from_items();

    //4
    QMapIterator<QString, int> mapitr(map);
    while(mapitr.hasNext()) {
        mapitr.next();
        ui->frame_cook->setCurrentIndex(mapitr.key(), mapitr.value());
    }
    ui->label_return_cal->setText(tablemodel->get_value_vch_print_memo(map));
    //5
    QString errstring;
    ui->spinBox_int_discount->setRange(0, 100);

    if(!errstring.isEmpty()) label_info->append(errstring);
    //折扣
    flag = true;
    if(!frommodel->can_discount(row, &errstring)){
        flag = false;
        label_info->append(errstring);
    }
    ui->spinBox_int_discount->setEnabled(flag);
    //做法
    enable_cook(frommodel->can_cook(row));
    toupdatelabelreturn();
    //价格
    flag = true;
    if(!frommodel->can_change_price(row, &errstring)) {
        flag = false;
        label_info->append(errstring);
    }
    ui->doubleSpinBox_num_price->setEnabled(flag);
    //数量
    flag  = true;
    if(!frommodel->can_change_num(row, &errstring)) {
        flag = false;
        label_info->append(errstring);
    }
    ui->doubleSpinBox_num_num->setEnabled(flag);
    ui->pushButton_take_weight->setEnabled(flag);
    //赠送
    flag = true;
    if(!frommodel->can_present(row, &errstring)) {
        flag = false;
        label_info->append(errstring);
    }
    ui->pushButton_present->setEnabled(flag);
    ui->pushButton_present->set_rt_checked(frommodel->model_data(row, "ch_presentflag").toString() == "Y");
}

void w_scr_dish_num_cook_Dialog::toallcook()
{
    ui->frame_cook->clearall_item();
    lds_query query;
    query.execSelect(QString("select a.ch_no,a.vch_cp_memo,a.ch_typeno,a.num_add_price FROM cey_bt_cp_memo a"));
    while(query.next()){
        ui->frame_cook->add_item(query.recordValue("ch_no").toString(),
                                 QString("%1\n%2").arg(query.recordValue("vch_cp_memo").toString()).arg(query.recordValue("num_add_price").toString()),
                                 false);
    }
    ui->frame_cook->generat_from_items();
}

void w_scr_dish_num_cook_Dialog::toclearcook()
{
    ui->frame_cook->clearSelected();

}

void w_scr_dish_num_cook_Dialog::took()
{
    float num_num = ui->doubleSpinBox_num_num->value();
    if(num_num == 0) num_num = 1;
    //判断
    QMap<QString, int> map;
    QList<frontViewDialog_toolbtn *> rblist=ui->frame_cook->hasSelectedRb();
    foreach(frontViewDialog_toolbtn *rb, rblist){
        map.insert(rb->index, (int)rb->getNum());
    }
    if(ui->doubleSpinBox_num_num->value() <= 0) {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, tr("数量不能小于等于0"));
        ui->doubleSpinBox_num_num->setFocus();
        return;
    }
    QString discount_desc;
    if(false == tablemodel->can_discount_limit(ui->spinBox_int_discount->trueValue(), tr("折扣"), discount_desc)) {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, discount_desc);
        return;
    }

    //将数据提交给model
    tablemodel->model_data_set(0, "num_num", ui->doubleSpinBox_num_num->value() + tablemodel->model_data(0, "num_back").toFloat());
    tablemodel->model_data_set(0, "num_price", ui->doubleSpinBox_num_price->value());
    tablemodel->model_data_set(0, "int_discount", ui->spinBox_int_discount->value());

    tablemodel->model_data_set(0, "num_price_add", tablemodel->get_num_price_add_vch_print_memo_by_map(map));
    if(tablemodel->model_data(0, "ch_suitflag").toString() == "Y" ) {
        tablemodel->model_data_set(0, "num_price_add", 0);
    }
    tablemodel->model_data_set(0, "vch_print_memo", tablemodel->get_value_vch_print_memo(map));

    //关闭
    this->accept();
}

void w_scr_dish_num_cook_Dialog::tocancel()
{
    this->reject();
}

void w_scr_dish_num_cook_Dialog::topresent()
{
    w_scr_dish_num_cook_Dialog::static_to_present(this,
                                                  tablemodel,
                                                  0,
                                                  ui->doubleSpinBox_num_num->value());
    ui->pushButton_present->set_rt_checked(tablemodel->model_data(0, "ch_presentflag").toString() == "Y");
}

void w_scr_dish_num_cook_Dialog::totakeweight()
{
    //没有选中行
    take_weight dialog(tablemodel->model_data(0, "ch_dishno").toString(),
                       ui->doubleSpinBox_num_price->value(),
                       ui->doubleSpinBox_num_num->value(),
                       ui->spinBox_int_discount->value(),
                       this);
    dialog.setWindowTitle(tr("取重"));
    if(QDialog::Accepted == lds_roundeddialog_rect_align(&dialog).exec()){
        ui->doubleSpinBox_num_num->setValue(dialog._weight);
    }
}

void w_scr_dish_num_cook_Dialog::tocookset()
{
    w_bt_dish_cookset_typeset dialog(this);
    dialog.setWindowTitle(_TEXT_SLOT(this));
    lds_roundeddialog_rect_align(&dialog).exec();
    if(ui->frame_cook->count() > 0)
        toallcook();
}

void w_scr_dish_num_cook_Dialog::tomore()
{
    static QLabel *label = new QLabel;
    if(ui->pushButton_more->menu() == 0) {
        label->setMinimumSize(300, 300);
        label->setStyleSheet("color:red; font-size:16pt;");
        lds_menu *menu=new lds_menu(this);
        QWidgetAction *widgetaction = new QWidgetAction(ui->pushButton_more);
        widgetaction->setDefaultWidget(label);
        menu->addAction(widgetaction);
        ui->pushButton_more->setMenu(menu);
    }
}

void w_scr_dish_num_cook_Dialog::toupdatelabelreturn()
{
    QMap<QString, int> map;
    QList<frontViewDialog_toolbtn *> rblist=ui->frame_cook->hasSelectedRb();
    foreach(frontViewDialog_toolbtn *rb, rblist){
        map.insert(rb->index, (int)rb->getNum());
    }
    ui->label_return_cal->setText(tablemodel->get_value_vch_print_memo(map));
}

void w_scr_dish_num_cook_Dialog::toupdatelabelreturn(frontViewDialog_toolbtn *rb)
{
    if(rb && rb->isChecked()){
        lds_dialog_input inputdialog(this);
        inputdialog.setWindowTitle(tr("做法(为0时表示没有该做法)"));
        inputdialog.ui->label->setText(tr("做法数量:"));
        inputdialog.ui->lineEdit->setText(QString().sprintf("%0.2f", rb->getNum()));
        inputdialog.setDotEnabled(false);
        switch(lds_roundeddialog_rect_align(&inputdialog).exec()) {
        case 0://取消
            break;
        case 1:{//确定
            float num = inputdialog.ui->lineEdit->text().toFloat();
            if(num == 0){
                rb->setChecked(false);
                num = 1;
            }
            rb->setNum(num);
            toupdatelabelreturn();
        }
            break;
        }
    }
}

void w_scr_dish_num_cook_Dialog::togenaratcook(const QString &no)
{

    ui->frame_cook->clearSelected();
    QMap<QString, int> map = frommodel->get_map_vch_print_memo(no);
    QMapIterator<QString, int> mapitr(map);
    while(mapitr.hasNext()) {
        mapitr.next();
        ui->frame_cook->setCurrentIndex(mapitr.key(), mapitr.value());
    }
}

bool w_scr_dish_num_cook_Dialog::eventFilter(QObject *o, QEvent *e)
{
    if(QEvent::KeyPress == e->type()) {
        QKeyEvent *ke = ((QKeyEvent *)e);
        const int kvalue = ke->key();
        if( kvalue == Qt::Key_Return
                || kvalue == Qt::Key_Enter
                ) {
            took();
            e->ignore();
            return true;
        }
    }
    return QDialog::eventFilter(o, e);
}
