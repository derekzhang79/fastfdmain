#include "founding_cook.h"
#include "backheader.h"
#include "w_scr_dish.h"
#include "frontviewdialog_virtual.h"
#include "lds_messagebox.h"
#include <QSqlError>
#include "w_scr_dish_restaurant_dialog.h"
#include "lds_menu.h"
#include "w_scr_dish_select_operid.h"
#include "w_scr_dish_restaurant_div_pop.h"
#include <QDataWidgetMapper>
#include "w_scr_dish_pay_widget.h"
#include "w_bt_dish.h"
#include "pointerautodel.h"
#include "public_printby_ch_billno.h"
#include "lds_model_sqltablemodel_delegate_com.h"
#include "public_sql.h"
#include "w_scr_dish_main_table_pay.h"
#include "w_scr_dish_main_bar_cancel.h"


founding_cook::founding_cook(const QString &ch_billnos, const QString &ch_tableno, const QString &ch_billno, int sqr, QWidget *parent)
    : QDialog(parent)
{
    initData();
    update_9Box(ch_billnos, ch_tableno, ch_billno, sqr);
}

founding_cook::~founding_cook()
{
    delete ui;
}

void founding_cook::initData(QString ch_billno, int sqr)
{
    if(ch_billno.isEmpty()) {//不存在
        ch_billno  = w_scr_dish_restaurant_div_pop::getMaxChbillnoAdd1_cey_u_master("B");
    }
    //    ui->pushButton_div->setFocus();
    //1
    ui->lineEdit_ch_billno->setText(ch_billno);
    //2
    tablemodel->setFilter(QString("ch_billno = '%1' ").arg(ch_billno));
    mapper->setCurrentIndex(0);
    //3
    if(tablemodel->rowCount() > 0){
        lds::hideWidget(ui->frame_stack1);
        lds::showWidget(ui->frame_stack2);
        lds::hideWidget(ui->pushButton_bar_clear);
    } else {
        lds_query query;
        query.execSelect(QString("select int_person from cey_bt_table where ch_tableno = '%1' ")
                         .arg(tablemodel->tablename_kvmap_value("cey_u_table", ch_billno, "ch_billno", "ch_tableno").toString())
                         );
        query.next();
        lds::showWidget(ui->frame_stack1);
        lds::hideWidget(ui->frame_stack2);
        lds::showWidget(ui->pushButton_bar_clear, w_scr_dish_restaurant_dialog::cey_bt_table_bar_existed(ch_tableno, sqr));

        ui->dateTimeEdit_dt_operdate->setDateTime(n_func::f_get_sysdatetime());
        ui->lineEdit_int_person->setValue(query.recordValue("int_person").toDouble());
        ui->lineEdit_vch_memo->setText("");
        ui->lineEdit_vch_waiter->setText("");
    }
}

w_scr_dish_restaurant_div_pop_only_table *founding_cook::get9Box()
{
    if( _9Box == 0) {
        QMap<int, QString> ch_billno_map = restraurantDelegate::get_ch_billno_div_map(ch_billnos);

        _9Box = new w_scr_dish_restaurant_div_pop_only_table(true, this);
        _9Box->enableItemFlagSelectedWithAll();
        _9Box->setWindowFlags(Qt::Widget);
        _9Box->update_by_ch_billnos_ch_tableno(ch_billno_map, ch_tableno, false);
        _9Box->setcurindex_by_sqr(0);

        ui->horizontalLayout_div->addWidget(_9Box);

        connect(_9Box, SIGNAL(signal_div_click(QModelIndex)),this,SLOT(toBillDiv(QModelIndex)));
    }
    return _9Box;
}

void founding_cook::update_9Box(const QString &ch_billnos, const QString &ch_tableno, const QString &ch_billno, int sqr)
{
    this->ch_tableno  = ch_tableno;
    this->ch_billnos = ch_billnos;

    QMap<int, QString> ch_billno_map = restraurantDelegate::get_ch_billno_div_map(ch_billnos);
    if(restraurantDelegate::ch_billno_is_div(ch_billno_map)
            || (sqr != 0)) {
        get9Box()->setcurindex_by_sqr(sqr);
    }

    initData(ch_billno, sqr);
}

void founding_cook::on_pushButton_14_clicked()//取消
{
    this->done(EXIT);
}

void founding_cook::on_pushButton_9_clicked()//选择
{
}

void founding_cook::on_pushButton_13_clicked()//点单
{
    {
        this->done(DISH_ORDER);
    }
}

void founding_cook::on_pushButton_open_clicked()//开台
{
    {
        this->done(TABLE_OPEN);
    }
}

void founding_cook::toed_exit()
{
    //
    //    ui->pushButton_div->setFocus();
    if(tablemodel->model_is_dirty()){
        switch(lds_messagebox::question(this, MESSAGE_TITLE_VOID,tr("有数据未保存,确认保存?"), tr("保存"),tr("不保存"))){
        case 0:
            if(!saveData()){
                return;
            }
            break;
        case 1:
            break;
        }
    }
    this->done(EXIT);
}

void founding_cook::toed_order()
{
    if(saveData(false)){
        this->done(DISH_APPEND);
        return;
    }
}

void founding_cook::toed_pay()
{
    if(payBill(this, ui->lineEdit_ch_billno->text(), false)){
        this->done(PRE_PAYBILL);
        return;
    }
    //    lds_messagebox::warning(this, MESSAGE_TITLE_VOID, tr("结账失败"));
}

void founding_cook::toed_pay_somedish()
{
    if(payBill(this, ui->lineEdit_ch_billno->text(), true)){
        this->done(PRE_PAYBILL);
        return;
    }
}

void founding_cook::toed_save()
{
    if(saveData()) {
        this->reject();
    }
}

void founding_cook::toed_inputonline()
{

}

void founding_cook::toBillDiv(const QModelIndex &index)
{
    //
    ui->pushButton_div->setFocus();
    if(tablemodel->model_is_dirty()){
        switch(lds_messagebox::question(this, MESSAGE_TITLE_VOID,tr("有数据未保存,确认保存?"), tr("保存"),tr("不保存"))){
        case 0:
            if(!saveData()){
                return;
            }
            break;
        case 1:
            break;
        }
    }
    //
    QString ch_billno = get9Box()->get_ch_billno_by_index(index);
    //
    initData(ch_billno, get_sqr_by_curindex());
}

void founding_cook::toIsShowDiv()
{
    if(_9Box == 0) {
        get9Box();
    }
    _9Box->setVisible(!get9Box()->isVisible());
}

void founding_cook::toinlne()
{
}

void founding_cook::toreprint()
{
    pre_print(this, ch_tableno);
}

void founding_cook::tobarclear()
{
    int  sqr = get_sqr_by_curindex();
    w_scr_dish_main_bar_cancel dialog(ch_tableno, sqr, this);
    if(QDialog::Accepted == lds_roundeddialog_rect_align(&dialog).exec()) {
        this->done(EXIT);
    }
}

bool founding_cook::saveData(bool inf)
{
    if(tablemodel->trySubmitAll()){
        if(inf)lds_messagebox::information(this, MESSAGE_TITLE_VOID, tr("修改成功"));
        return true;
    }
    lds_messagebox::warning(this, MESSAGE_TITLE_VOID, tr("修改失败")+QString::number(__LINE__));
    return false;
}

void founding_cook::pre_print(QWidget *parent, const QString &ch_tableno)
{
    QString errstring;
    QString ch_billnos;
    lds_query query;
    query.execSelect("select "
                + restraurantDelegate::sql_ch_billnos + " as ch_billnos "
                + QString(" from cey_bt_table where ch_tableno = '%1' ")
                .arg(ch_tableno));
    query.next();
    ch_billnos = query.recordValue("ch_billnos").toString();

    QMap<int, QString> ch_billno_map = restraurantDelegate::get_ch_billno_div_map(ch_billnos);
    errstring = tr("没有桌台被选中");
    if(!ch_tableno.isEmpty()){
        errstring = tr("桌台未开台");
        if(!ch_billno_map.isEmpty()) {
            //可以分单
            errstring = "";
            if(restraurantDelegate::ch_billno_is_div(ch_billno_map)){
                w_scr_dish_restaurant_div dialog(ch_billno_map, ch_tableno, parent);
                dialog.setFlagWith_KAITAI_YUDA();
                dialog.enableMutilSelected();
                dialog.setWindowTitle(tr("预打单"));
                if(QDialog::Accepted == lds_roundeddialog_rect_align(&dialog).exec()){
                    QModelIndexList index_list = dialog.get_cur_check_index_list();
                    if(index_list.count() > 0) {
                        if(0 == lds_messagebox::question(parent, MESSAGE_TITLE_VOID, tr("是否打印预打单"), tr("是"), tr("否"))){
                            QString ch_billno = index_list[0].data(w_scr_dish_restaurant_div_pop::Model_ch_billno).toString();
                            public_printby_ch_billno printer(ch_billno);//所有单号的前12位都一样，所以对应的桌号也一致
                            printer.toprint_preprint_div_title(printer.defpath(), tr("预打单"));
                            double subtotal = 0;
                            int idvcount = 0;
                            for(int k = 0;k < index_list.count(); k++) {
                                ch_billno = index_list[k].data(w_scr_dish_restaurant_div_pop::Model_ch_billno).toString();
                                public_printby_ch_billno printer(ch_billno);
                                subtotal += printer.toprint_preprint_div(printer.defpath(),
                                                                         index_list[k].data(w_scr_dish_restaurant_div_pop::Model_sqr).toInt());
                                idvcount++;
                                public_sql::preprinttablemap.insert(ch_billno, true);
                            }
                            printer.clear_ins();
                            printer.toprint_preprint_div_tail(printer.defpath(), subtotal, idvcount);
                            return;
                        }
                    }
                }
            } else {//不可以分单
                QString ch_billno = ch_billno_map.value(0);
                if(0 == lds_messagebox::question(parent, MESSAGE_TITLE_VOID, tr("是否打印预打单"), tr("是"), tr("否"))){
                    public_printby_ch_billno printer(ch_billno);
                    printer.toprint_preprint(printer.defpath(), tr("预打单"));
                    public_sql::preprinttablemap.insert(ch_billno, true);
                    return;
                }
            }
        }
        //单号不为空

        if(!errstring.isEmpty()) {
            lds_messagebox::warning(parent, MESSAGE_TITLE_VOID, errstring);
        }
    }
}

bool founding_cook::payBill(QWidget *parent, const QString &ch_billno, bool ispaysomedish)
{
    /** -----------------------------权限-----------------------------begin*/
    QString ls_operid = public_sql::gs_operid;
    if(!n_func::f_check_bus_grant_else_authority(ls_operid, "0214", parent, tr("付款"))){
        return  false;
    }
    /** -----------------------------权限-----------------------------end*/

    QString errstring;
    lds_query query;
    QString ch_tableno = public_sql::getTablenoByChbillno(ch_billno);
    int div_sqr = w_scr_dish_restaurant_div_pop::cey_bt_table_div_sqr(ch_tableno, ch_billno);
    if(w_scr_dish_restaurant_div_pop::cey_bt_table_has_locked(parent, ch_tableno, div_sqr)) {
        return false;
    }
    ///lock

    fexpandmain_model_sqltablemodel_data tablemodel;
    tablemodel.setTable("cey_u_orderdish");
    tablemodel.setEditStrategy(lds_model_sqltablemodel::OnManualSubmit);
    tablemodel.select(ch_billno, parent);

    if(tablemodel.rowCount() <= 0) {
        lds_messagebox::warning(parent ,MESSAGE_TITLE_VOID, tr("没有点菜记录,不能付款"));
        return false;
    }

    //lock
    w_scr_dish_restaurant_div_pop::cey_bt_table_lock(ch_tableno, div_sqr);
    //~lock
    bool payFlag = false;

    //退单，需要新开单子
    if(ispaysomedish) {
        QList<int> rowListAsc;
        if(QDialog::Rejected == w_scr_dish_main::paysomedishGetRows(parent, &tablemodel, rowListAsc, false)) {
            goto unlock;
        }
        if(rowListAsc.count() == 0) {
            lds_messagebox::warning(parent ,MESSAGE_TITLE_VOID, tr("没有点菜记录,不能付款"));
            goto unlock;
        }
        //
        if(false == w_scr_dish_main::separatePaybill(parent, &tablemodel, rowListAsc, true)) {
            goto unlock;
        }
        payFlag = true;
    } else {
        //w_scr_dish_pay
        w_scr_dish_main_table_pay dialog(&tablemodel, parent);
        dialog.move(0, 0);
        if(QDialog::Rejected == dialog.exec()) {
            goto unlock;
        }
        tablemodel.select("1=2", parent);//1=2必定为空
        //~w_scr_dish_pay
        payFlag = true;
    }
    //
    if(tablemodel.rowCount() == 0) {
        w_scr_dish_restaurant_div_pop::cey_bt_table_update(ch_tableno, "", div_sqr, &errstring);
    }
    //更新餐桌金额
    query.execUpdate("cey_u_table","num_cost", tablemodel.getKeepdata().dish_total(),qrtEqual("ch_billno", ch_billno));
unlock:
    ///unlock
    w_scr_dish_restaurant_div_pop::cey_bt_table_unlock(ch_tableno, div_sqr);
    if(payFlag) {
        lds_messagebox::information(parent, MESSAGE_TITLE_VOID, tr("结算成功"), true);
    }
    return payFlag;
}

int founding_cook::get_sqr_by_curindex()
{
    if(_9Box == 0) return 0;

    return get9Box()->get_sqr_by_curindex().toInt();
}

void founding_cook::updateVch_memo(const QString &vch_memo)
{
    ui->lineEdit_vch_memo->setText(vch_memo);
}

void founding_cook::hideOpenTable()
{
    ui->pushButton_open->hide();
}

void founding_cook::initData()
{
    ui = new Ui::founding_cook;
    ui->setupUi(this);
    lds::hideWidget(ui->frame_stack1);
    lds::hideWidget(ui->frame_stack2);
    //2
    ui->lineEdit_ch_billno->setEnabled(false);
    ui->lineEdit_vch_waiter->setEnabled(false);
    ui->dateTimeEdit_dt_operdate->setEnabled(false);
    //九宫格
    _9Box = 0;

    //datamapper
    tablemodel = new lds_model_sqltablemodel(this);
    tablemodel->setEditStrategy(lds_model_sqltablemodel::OnManualSubmit);
    tablemodel->setTable("cey_u_table");
    mapper = new QDataWidgetMapper(this);
    mapper->setSubmitPolicy(QDataWidgetMapper::AutoSubmit);
    mapper->setModel(tablemodel);

    mapper->addMapping(ui->lineEdit_ch_billno, tablemodel->fieldIndex("ch_billno"));
    mapper->addMapping(ui->dateTimeEdit_dt_operdate, tablemodel->fieldIndex("dt_operdate"));
    mapper->addMapping(ui->lineEdit_int_person, tablemodel->fieldIndex("int_person"));
    mapper->addMapping(ui->lineEdit_vch_memo, tablemodel->fieldIndex("vch_memo"));
    mapper->addMapping(ui->lineEdit_vch_waiter, tablemodel->fieldIndex("vch_waiter"));
    mapper->addMapping(ui->lineEdit_vch_operid, tablemodel->fieldIndex("vch_operid"));
    ui->lineEdit_vch_operid->setEnabled(false);

    //!connect
    connect(ui->pushButton_ed_exit,SIGNAL(clicked()),this,SLOT(toed_exit()));
    connect(ui->pushButton_ed_order,SIGNAL(clicked()),this,SLOT(toed_order()));
    connect(ui->pushButton_pay,SIGNAL(clicked()),this,SLOT(toed_pay()));
    connect(ui->pushButton_pay_div,SIGNAL(clicked()),this,SLOT(toed_pay_somedish()));
    connect(ui->pushButton_ed_save,SIGNAL(clicked()),this,SLOT(toed_save()));
    connect(ui->pushButton_div,SIGNAL(clicked()),this,SLOT(toIsShowDiv()));
    connect(ui->pushButton_inline, SIGNAL(clicked()),this,  SLOT(toinlne()));
    connect(ui->pushButton_reprint, SIGNAL(clicked()),this,  SLOT(toreprint()));
    connect(ui->pushButton_bar_clear, SIGNAL(clicked()),this,  SLOT(tobarclear()));
}
