#include "w_m_member_point.h"
//会员积分管理
#include "ui_w_m_member_dialog.h"

#include <QSqlError>
#include "lds_query.h"
#include <QSqlRecord>
#include "lds_model_sqltablemodel.h"
#include "lds_model_sqltablemodel_delegate_com.h"
#include "getchinesespell.h"
#include "lds_tableview_delegate_check.h"
#include <QSqlRelation>
#include "lds_tableview_repecify.h"
#include "backheader.h"
#include <QTimer>
#include "w_m_member.h"
#include "lds_messagebox.h"
#include "ui_w_m_member_point_recharge_dialog.h"
#include "w_m_member_list.h"
#include "lds.h"
#include "public_cika_read_dialog.h"
#include "reader_rf.h"
#include "w_sys_manage_cloudsync_with_time.h"
#include "cJSON.h"


w_m_member_point::w_m_member_point(QWidget *parent) :
    QDialog(parent),ui(new Ui_w_m_member_Dialog)
{
    ui->setupUi(this);
    //! treewidget
    ui->treeWidget->header()->hide();
    ui->treeWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->treeWidget->setColumnCount(1);

    //! tableview
    tablemodel=new lds_model_sqltablemodel(this);
    tablemodel->setEditStrategy(lds_model_sqltablemodel::OnManualSubmit);
    tablemodel->setTable("t_m_member");
    tablemodel->save_set_header("vch_memberno", tr("会员编号"));
    tablemodel->save_set_header("vch_member", tr("会员名称"));
    tablemodel->save_set_header("ch_typeno", tr("会员类型"));
    tablemodel->save_set_header("dt_birthday", tr("生日"));
    tablemodel->save_set_header("vch_handtel", tr("手机"));
    tablemodel->save_set_header("vch_tel", tr("电话"));
    tablemodel->save_set_header("dt_limit", tr("有效期限"));
    tablemodel->save_set_header("ch_state", tr("状态"));
    tablemodel->save_set_header("ch_cardflag", tr("持卡"));
    tablemodel->save_set_header("vch_operID", tr("创建人员"));
    

    tablemodel->tablename_kvmap_insert("cey_sys_operator", backheader::SELECT_OPERID_NAME, false, lds_model_sqltablemodel::SELECT_AUTO_REFRESH_KEY);
    ui->tableView->setEditTriggers(QTableView::AllEditTriggers);
    ui->tableView->setModel(tablemodel);
    ui->tableView->rePecifyHeader(tablemodel->get_save_set_header_saver_column_list());
    ui->tableView->setItemDelegateForColumn(tablemodel->fieldIndex("ch_typeno"),
                                            new lds_model_sqltablemodel_delegate_com(ui->tableView, "t_m_member_type", "ch_typeno","vch_typename"));
    ui->tableView->setItemDelegateForColumn(tablemodel->fieldIndex("ch_state"),
                                            new lds_model_sqltablemodel_delegate_com(ui->tableView,  &w_m_member::get_VIP_MODEL_STATE()));
    ui->tableView->setItemDelegateForColumn(tablemodel->fieldIndex("ch_cardflag"),
                                            new lds_model_sqltablemodel_delegate_com(ui->tableView, &w_m_member::get_VIP_MODEL_FLAG()));
    ui->tableView->setItemDelegateForColumn(tablemodel->fieldIndex("vch_operID"),
                                            new lds_model_sqltablemodel_delegate_com(ui->tableView, tablemodel->tablename_kvmap_d("cey_sys_operator")));
    //! toolbar
    ui->widget->setup(QStringList()
                      << tr("详情") << tr("礼品资料")
                      << tr("充积分") << tr("减积分") << tr("换礼品")
                      << tr("刷新") << tr("退出"));
    connect(ui->treeWidget,SIGNAL(pressed(QModelIndex)),this,SLOT(treeselect()));
    connect(ui->lineEdit,SIGNAL(textChanged(QString)),this,SLOT(treeselect()));

    connect(ui->widget->index_widget(tr("详情")), SIGNAL(clicked()),this,SLOT(toliping_detail()));
    connect(ui->widget->index_widget(tr("礼品资料")), SIGNAL(clicked()),this,SLOT(toliping_info()));
    connect(ui->widget->index_widget(tr("充积分")), SIGNAL(clicked()),this,SLOT(toadd_point()));
    connect(ui->widget->index_widget(tr("减积分")), SIGNAL(clicked()),this,SLOT(tosub_point()));
    connect(ui->widget->index_widget(tr("换礼品")), SIGNAL(clicked()),this,SLOT(tochange_liping()));
    connect(ui->widget->index_widget(tr("刷新")), SIGNAL(clicked()),this,SLOT(torefresh()));
    connect(ui->widget->index_widget(tr("退出")), SIGNAL(clicked()),this,SLOT(toexit()));

    //! after
    ui->label_2->setVisible(false);
    ui->lineEdit_2->setVisible(false);
    //! after
    tablemodel_arg2 =
            " (t_m_member.ch_typeno = '%1' or t_m_member.ch_typeno like '%1%')  and "
            " (t_m_member.vch_memberno = '%2' or t_m_member.vch_memberno like '%2%')   "
            ;
    tablemodel->setFilter(tablemodel_arg2.arg("").arg(""));
    QTimer::singleShot(0, this,SLOT(torefresh()));
}

void w_m_member_point::torefresh()
{
    //yun
    w_sys_manage_cloudsync_with_time_running::exec_no_up_then_down_byconf(this);
    ui->widget->index_widget(tr("退出"))->show_flag_top_right(w_sys_manage_cloudsync_with_time_running::q_bt_get_ready_total_byconf());
    //~yun
    //treewidget
    ui->treeWidget->deleteAllItem();
    ui->treeWidget->rootitem=new QTreeWidgetItem(QStringList()
                                                 <<tr("全部类型"));
    ui->treeWidget->addTopLevelItem(ui->treeWidget->rootitem);
    lds_query query;
    query.execSelect("select concat(vch_typename,'[',ch_typeno,']') as 'nametype',ch_typeno  from t_m_member_type "
                     );
    while(query.next()){
        QTreeWidgetItem *item=new QTreeWidgetItem(QStringList()
                                                  <<QString("%1")
                                                  .arg(query.recordValue("nametype").toString())
                                                  );
        ui->treeWidget->rootitem->addChild(item);
    }

    ui->treeWidget->setItemSizeHint();//更新所有的 item的sizehint
    ui->treeWidget->restore();
    //tableview

    tablemodel->select();
    ui->tableView->resizeColumnsToContents();
    ui->tableView->restore();
}

void w_m_member_point::treeselect()
{
    QString ch_typeno;
    QString vch_memberno;
    //tree检索
    if(ui->treeWidget->currentItem()){
        int index=ui->treeWidget->deepofitem(ui->treeWidget->currentItem());
        if(index==1){
            ch_typeno=backheader::getinterdata(ui->treeWidget->currentItem()->text(0));
        }
    }
    vch_memberno = ui->lineEdit->text();

    tablemodel->setFilter(tablemodel_arg2.arg(ch_typeno).arg(vch_memberno));
    ui->tableView->restore();

}

void w_m_member_point::toliping_detail()
{
    w_m_member_point_detail dialog(this);
    dialog.setWindowTitle(_TEXT_SLOT(this));lds_roundeddialog_rect_align(&dialog).exec();

}

void w_m_member_point::toliping_info()
{
    w_m_member_point_liping_info dialog(this);
    dialog.setWindowTitle(_TEXT_SLOT(this));lds_roundeddialog_rect_align(&dialog).exec();

}

void w_m_member_point::toadd_point()
{
    w_m_member_point_recharge dialog("5",this);
    if(dialog._ini_error.isEmpty()){
        dialog.setWindowTitle(_TEXT_SLOT(this));
        lds_roundeddialog_rect_align(&dialog).exec();
        torefresh();
    } else {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID,dialog._ini_error);
    }
}

void w_m_member_point::tosub_point()
{   
    w_m_member_point_recharge dialog("6",this);
    if(dialog._ini_error.isEmpty()){
        dialog.setWindowTitle(_TEXT_SLOT(this));
        lds_roundeddialog_rect_align(&dialog).exec();
        torefresh();
    } else {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID,dialog._ini_error);
    }
}

void w_m_member_point::tochange_liping()
{
    if(!saas_pay_detail::SaaSFuncCheck(this, "0302"/*云、本地会员积分兑换*/)) {
        return;
    }

    w_m_member_point_recharge dialog("4",this);
    QSqlQueryModel *querymodel=new QSqlQueryModel(&dialog);
    querymodel->setQuery(QString("select vch_giftname as '%1', ch_giftno as '%2' , num_point as '%3' from t_m_gift ")
                         .arg(tr("礼品名称"))
                         .arg(tr("礼品编码"))
                         .arg(tr("积分值")));
    dialog.ui->comboBox_vch_memo->setMaxVisibleItems(5);
    dialog.ui->comboBox_vch_memo->setView(new lds_tableView);
    dialog.ui->comboBox_vch_memo->setModel(querymodel);
    dialog.ui->comboBox_vch_memo->setEditable(false);
    dialog.ui->comboBox_vch_memo->enableEmitColumn(2);

    dialog.ui->label_memo->setText(tr("礼品"));
    dialog.ui->comboBox_vch_memo->setCurrentIndex(-1);
    dialog.ui->lineEdit_num_point->setEnabled(false);
    connect(dialog.ui->comboBox_vch_memo,SIGNAL(signalEmitColumn(QString)), &dialog, SLOT(toUpdateGiftNumPoint(QString)));

    if(dialog._ini_error.isEmpty()){
        dialog.setWindowTitle(_TEXT_SLOT(this));
        lds_roundeddialog_rect_align(&dialog).exec();
        torefresh();
    } else {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID,dialog._ini_error);
    }
}

void w_m_member_point::toexit()
{
    w_sys_manage_cloudsync_with_time_running::exec_keepdata_byconf(this);
    this->reject();
}

//============================
#include "ui_w_bt_dish_unitset_dialog.h"


w_m_member_point_detail::w_m_member_point_detail(w_m_member_point *parent)
    :QDialog(parent),
      ui(new Ui_w_bt_dish_unitset_Dialog),
      _parent(parent)
{
    ui->setupUi(this);

    //! tableview
    tablemodel=new lds_model_sqltablemodel(this);
    tablemodel->setEditStrategy(lds_model_sqltablemodel::OnManualSubmit);
    tablemodel->setTable("t_m_point");
    
    tablemodel->save_set_header("ch_type", tr("积分类型"));
    tablemodel->save_set_header("ch_payno", tr("凭证编号"));
    tablemodel->save_set_header("num_point", tr("积分值"));
    tablemodel->save_set_header("vch_memo", tr("备注"));
    tablemodel->save_set_header("vch_operid", tr("操作员"));
    tablemodel->save_set_header("dt_operdate", tr("操作时间"));
    tablemodel->tablename_kvmap_insert("cey_sys_operator", backheader::SELECT_OPERID_NAME, false, lds_model_sqltablemodel::SELECT_AUTO_REFRESH_KEY);

    ui->tableView->setEditTriggers(QTableView::AllEditTriggers);
    ui->tableView->setModel(tablemodel);
    ui->tableView->rePecifyHeader(tablemodel->get_save_set_header_saver_column_list());
    ui->tableView->setItemDelegateForColumn(tablemodel->fieldIndex("vch_operid"),
                                            new lds_model_sqltablemodel_delegate_com(ui->tableView, tablemodel->tablename_kvmap_d("cey_sys_operator")));
    ui->tableView->setItemDelegateForColumn(tablemodel->fieldIndex("ch_type"),
                                            new lds_model_sqltablemodel_delegate_com(ui->tableView,  &w_m_member::get_VIP_MODEL_POINT_CH_TYPE()));

    if(_parent->ui->tableView->currentIndex().row()>=0){
        tablemodel->setFilter(QString("vch_memberno='%1' ")
                              .arg(_parent->tablemodel->model_data(_parent->ui->tableView->currentIndex().row(), "vch_memberno").toString())
                              );
    } else {
        tablemodel->select();
    }
    //! toolbar
    ui->widget->setup(QStringList()
                      << tr("打印") << tr("退出"), Qt::AlignRight);

    connect(ui->widget->index_widget(tr("打印")), SIGNAL(clicked()),this,SLOT(setPrintCount()));
    connect(ui->widget->index_widget(tr("退出")), SIGNAL(clicked()),this,SLOT(toexit()));
    //! after
    ui->widget->index_widget(tr("打印"))->setVisible(false);

    QTimer::singleShot(100, ui->tableView, SLOT(resizeColumnsToContents()));
}

void w_m_member_point_detail::setPrintCount()
{

}

void w_m_member_point_detail::toexit()
{
    this->reject();
}

//=======================================
#include "lds_messagebox.h"
w_m_member_point_liping_info::w_m_member_point_liping_info(w_m_member_point *parent):
    QDialog(parent),
    ui(new Ui_w_bt_dish_unitset_Dialog)
{
    ui->setupUi(this);
    //! tableview
    tablemodel=new lds_model_sqltablemodel(this);
    tablemodel->setTable("t_m_gift");
    tablemodel->setEditStrategy(lds_model_sqltablemodel::OnManualSubmit);

    ui->tableView->setEditTriggers(QTableView::AllEditTriggers);ui->tableView->setModel(tablemodel);

    tablemodel->save_set_header("ch_giftno", tr("礼品编码"));
    tablemodel->save_set_header("vch_giftname", tr("礼品名称"), true);
    tablemodel->save_set_header("num_point", tr("积分值"), true);
    ui->tableView->rePecifyHeader(tablemodel->get_save_set_header_saver_column_list());

    //! toolbar
    ui->widget->setup(QStringList() << tr("新增") << tr("删除") <<  tr("刷新") << tr("保存") << tr("退出"), Qt::AlignRight, lds_keytoolbutton::SYSKEY);
    connect(ui->widget->index_widget(tr("新增")), SIGNAL(clicked()),this,SLOT(tonew()));
    connect(ui->widget->index_widget(tr("删除")), SIGNAL(clicked()),this,SLOT(todel()));
    connect(ui->widget->index_widget(tr("刷新")), SIGNAL(clicked()),this,SLOT(torefresh()));
    connect(ui->widget->index_widget(tr("保存")), SIGNAL(clicked()),this,SLOT(tosave()));
    connect(ui->widget->index_widget(tr("退出")), SIGNAL(clicked()),this,SLOT(toexit()));


    QTimer::singleShot(100, this, SLOT(torefresh()));

}

void w_m_member_point_liping_info::tonew()
{
    int rowcount=tablemodel->rowCount();
    tablemodel->insertRow(rowcount);
    tablemodel->model_data_set(rowcount, tablemodel->fieldIndex("ch_giftno"), tablemodel->max_column_add_1_dec("ch_giftno"));
    tablemodel->model_data_set(rowcount, tablemodel->fieldIndex("num_point"), "0");

    ui->tableView->setCurrentIndex(tablemodel->model_index(rowcount, "vch_giftname"));
}

void w_m_member_point_liping_info::todel()
{
    tablemodel->removeRow(ui->tableView->currentIndex().row());
    tablemodel->trySubmitAll();
}

void w_m_member_point_liping_info::torefresh()
{
    tablemodel->select();
}

bool w_m_member_point_liping_info::tosave()
{
    for(int r=0,r_count=tablemodel->rowCount();r<r_count;r++){
        if(tablemodel->model_data(r, "ch_giftno").toString().isEmpty()){
            lds_messagebox::warning(this, MESSAGE_TITLE_VOID,tr("礼品编码不能为空"));
            return false;
        }
        if(tablemodel->model_data(r, "num_point").toString().isEmpty()){
            lds_messagebox::warning(this, MESSAGE_TITLE_VOID,tr("积分值不能为空"));
            return false;
        }
        if(tablemodel->model_data(r, "vch_giftname").toString().isEmpty()){
            lds_messagebox::warning(this, MESSAGE_TITLE_VOID,tr("礼品名称不能为空"));
            return false;
        }
    }

    if(tablemodel->trySubmitAll()){
        lds_messagebox::information(this, MESSAGE_TITLE_VOID,tr("保存成功"));
        return true;
    } else {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID,tr("数据库错误")+QString("%1").arg(__LINE__));
        return false;
    }
}

void w_m_member_point_liping_info::toexit()
{
    if(tablemodel->model_is_dirty()){
        switch(lds_messagebox::question(this, MESSAGE_TITLE_VOID,tr("有数据未保存,确认保存?"), tr("保存"),tr("不保存"),tr("取消"))){
        case 0:
            if(!tosave()){
                return;
            }
            break;
        case 1:
            break;
        case 2:
            return;
        }
    }

    this->reject();
}

//=============================
#include "n_func.h"

w_m_member_point_recharge::w_m_member_point_recharge(const QString &type, w_m_member_point *parent):
    QDialog(parent),
    _type(type)
{
    ui = new Ui_w_m_member_point_recharge_Dialog;
    _parent = parent;
    ui->setupUi(this);

    uo_rw_member  uo_member;
    uo_rw_member::read_card_type  lst_card;
    QString ls_memberno;
    uo_member.of_init(this, "4");

    if(_parent->ui->tableView->currentIndex().row()>=0){//-1
        if(_parent->tablemodel->model_data(_parent->ui->tableView->currentIndex().row(), "ch_state").toString() =="1"){//0

            ls_memberno=_parent->tablemodel->model_data(_parent->ui->tableView->currentIndex().row(), "vch_memberno").toString();
            lst_card=uo_member.of_read_card(this, ls_memberno);
            if(lst_card.flag){
                ui->lineEdit_vch_memberno->setText(ls_memberno);
                ui->lineEdit_ch_type->setText(backheader::VIPFINDVALUE(w_m_member::get_VIP_MODEL_POINT_CH_TYPE(), _type));//5指的是手工充积分
                ui->lineEdit_num_point_org->setValue(lst_card.point);
                ui->lineEdit_vch_operid->setText(public_sql::gs_operid);
                ui->lineEdit_dt_operdate->setText(n_func::f_get_sysdatetime().toString("yyyy-MM-dd hh:mm:ss"));
            }
        } else {//0
            _ini_error = tr("该会员状态不正常，不允许手动充积分");
        }//0
    } else {//-1
        //        _ini_error = tr("没有数据被选中");
    }

    //! toolbar
    ui->widget->setup(QStringList() << tr("云会员读卡") << tr("确定") << tr("取消"), Qt::AlignRight, lds_keytoolbutton::SYSKEY);
    connect(ui->widget->index_widget(tr("确定")), SIGNAL(clicked()),this,SLOT(took()));
    connect(ui->widget->index_widget(tr("取消")), SIGNAL(clicked()),this,SLOT(toexit()));
    connect(ui->widget->index_widget(0), SIGNAL(clicked()),this,SLOT(toreadm()));
    if(_type=="4"){

    }
}

void w_m_member_point_recharge::took()
{
    //clicked (none) returns long [pbm_bnclicked]
    QString ls_memberno;
    QString ls_memo;
    double ldec_point;
    double ldec_point_org;

    ls_memberno=ui->lineEdit_vch_memberno->text();
    ls_memo=ui->comboBox_vch_memo->currentText();
    ldec_point=ui->lineEdit_num_point->value();
    if(ui->lineEdit_vch_memberno->text().trimmed().isEmpty()) {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, tr("无会员卡"));
        return;
    }
    if(ldec_point > 10000) {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, tr("积分数值最大为")+"10000");
        return;
    }
    ldec_point_org=ui->lineEdit_num_point_org->value();
    if(_type=="5"){//积分充积分
        ldec_point=qAbs(ldec_point);
    }
    if(_type=="6"
            ||_type=="4"){//手工减积分
        ldec_point=-qAbs(ldec_point);
        if(ldec_point_org < qAbs(ldec_point)){
            lds_messagebox::warning(this, MESSAGE_TITLE_VOID,tr("原积分值不足以完成本次操作"));
            return;
        }
    }
    if(_type=="4"){//礼品
        ls_memo=tr("礼品:")+ui->comboBox_vch_memo->currentText();
    }

    /////////////云端
    if(ch_member_belong_state == "Y") {
        QMap<QString, QString> headInfo;
        headInfo.insert("orgCode",n_func::f_get_sysparm("orgCode"));
        headInfo.insert("userName",n_func::f_get_sysparm("cloud_user"));
        headInfo.insert("password",n_func::f_get_sysparm("cloud_password"));
        cJSONHttpData root = cJSON_CreateObject();
        cJSON_help::filter_cJSON_AddStringToObject( root.json, "memberno", ls_memberno);
        cJSON_help::filter_cJSON_AddStringToObject( root.json, "type", _type);
        cJSON_help::filter_cJSON_AddStringToObject( root.json, "point", ldec_point);
        cJSON_help::filter_cJSON_AddStringToObject( root.json, "memo", ls_memo);
        cJSON_help::filter_cJSON_AddStringToObject( root.json, "operid", public_sql::gs_operid);
        QString param = root.jsonToString();

        QString httpkey="http://" + n_func::f_get_sysparm("cloud_ip")+
                "/CateSystem/cvs/member/changeCvsMemberPointByNo-rest";
        cJSONHttpData json_yun_parameter=w_sys_manage_cloudsync::get_yun_httppos(this, httpkey, param, headInfo, ui->lineEdit_ch_type->text());
        if(200 == json_yun_parameter.httpStatus) {
            lds_messagebox::information(this, MESSAGE_TITLE_VOID,tr("操作成功"));
            this->accept();
            return;
        }
        lds_messagebox::information(this, MESSAGE_TITLE_VOID,tr("操作失败"));
        return;
    }
    //本地
    lds_query::tran_saction();
    lds_query query;
    if(query.execInsert("t_m_point",
                        qrtVariantPairList()
                        << qrtVariantPair("vch_memberno", ls_memberno)
                        << qrtVariantPair("ch_type", _type)
                        << qrtVariantPair("ch_payno", "")
                        << qrtVariantPair("num_point", ldec_point)
                        << qrtVariantPair("vch_memo", ls_memo)

                        << qrtVariantPair("vch_operID", public_sql::gs_operid)
                        << qrtVariantPair("dt_operdate", n_func::f_get_sysdatetime()))) {
        if(query.execInsertDuplicateUpdate("t_m_curamount",
                                           "vch_memberno",
                                           qrtVariantPairList()
                                           << qrtVariantPair("vch_memberno", ls_memberno)
                                           << qrtVariantPair("num_point", qrtFunc("ifnull(num_point, 0) + " + QString::number(ldec_point)))
                                           << qrtVariantPair("vch_operID", public_sql::gs_operid)
                                           )) {
            lds_query::com_mit();
            lds_messagebox::information(this, MESSAGE_TITLE_VOID,tr("操作成功"));

            blockIns printdata;
            printdata += backheader::printer.drawText("i,a00,s11", this->windowTitle()+"\n");//清除缓存 居中 2x2x
            printdata += backheader::printer.drawText("i", "");//清除缓存
            //操作积分\备注
            printdata += backheader::printer.drawText("", ui->label_vch_memberno->text()+":"+ui->lineEdit_vch_memberno->text() + "\n");//
            printdata += backheader::printer.drawText("", ui->label_5->text()+":"+lds::septNumber(ui->lineEdit_num_point_org->value())+ "\n");//
            if(ui->comboBox_vch_memo->currentText().length() > 0)
                printdata += backheader::printer.drawText("", ui->label_memo->text()+":"+ui->comboBox_vch_memo->currentText() + "\n");//
            printdata += backheader::printer.drawText("", ui->label_4->text()+":"+ lds::septNumber(ui->lineEdit_num_point->value())+ "\n");//
            //剩余积分
            query.execSelect(QString("SELECT num_point from t_m_curamount where vch_memberno = '%1' ")
                             .arg(ls_memberno));
            query.next();
            printdata += backheader::printer.drawText("", tr("剩余积分")+":"+query.recordValue("num_point").toString()+ "\n");//
            //操作员\操作时间
            printdata += backheader::printer.drawText("", ui->label_7->text()+":"+ui->lineEdit_vch_operid->text()+ "\n");//
            printdata += backheader::printer.drawText("", ui->label_8->text()+":"+ui->lineEdit_dt_operdate->text()+ "\n");//

            printdata += backheader::printer.drawTextEnd("1B6405 1D5601", "");//走纸5x， 切纸
            printdata =   backheader::printer.setPrintCount(printdata, 1);

            backheader::asyn_dataprint(printdata);

            this->accept();
            return;
        }
    }
    lds_query::roll_back();
    lds_messagebox::information(this, MESSAGE_TITLE_VOID,tr("操作失败"));
}

void w_m_member_point_recharge::toexit()
{
    this->reject();
}

void w_m_member_point_recharge::toreadm()
{
    ch_member_belong_state = "";
    ui->lineEdit_vch_memberno->setStyleSheet("");
    ui->label_vch_memberno->setStyleSheet("");
    ui->lineEdit_vch_memberno->setText("");
    QString cardno;
    uo_rw_member uo_member;
    n_func::READCARDINFO cardinfo;
    if(false == lds::sysconf->value("w_sys_manage_cloudsync_with_time/cloud_yun_member", false).toBool()) {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, tr("未勾选") + tr("使用云会员"));
        return;
    }
    if(!w_m_member_list::static_to_memeber_readcard_d(this, cardno, uo_member, true, vch_membernoData::choose_yun)){
        return;
    }
    if(cardno.isEmpty()){
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID,tr("无可查询的卡"));
        goto end ;
    }
    if(!n_func::qtcardinfo_readcard(cardinfo, cardno)){
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, cardinfo.errstring);
        goto end;
    }
    //判断卡的状态
    if(cardinfo.vch_memberno.isEmpty()){
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID,tr("有误,还未发卡或不存在,请核实"));
        goto end;
    }
    if(cardinfo.ch_cardflag != "Y") {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID,tr("会员卡")+cardno+tr(",还未发卡"));
        goto end;
    }
    if(cardinfo.ch_state != "1"){
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID,cardinfo.vch_memberno+tr("该会员状态不正常，不允许手动充积分"));
        goto end;
    }

    ui->lineEdit_vch_memberno->setText(cardno);
    ui->lineEdit_ch_type->setText(backheader::VIPFINDVALUE(w_m_member::get_VIP_MODEL_POINT_CH_TYPE(), _type));//5指的是手工充积分
    ui->lineEdit_num_point_org->setValue(cardinfo.num_point);
    ui->lineEdit_vch_operid->setText(public_sql::gs_operid);
    ui->lineEdit_dt_operdate->setText(n_func::f_get_sysdatetime().toString("yyyy-MM-dd hh:mm:ss"));
    ui->lineEdit_vch_memberno->setStyleSheet("color:green");
    ui->label_vch_memberno->setStyleSheet("color:green");
    ch_member_belong_state = "Y";
end:
    w_m_member_list::yun_member_close();//! 关闭云会员
}

void w_m_member_point_recharge::toUpdateGiftNumPoint(const QString &num_point)
{
    ui->lineEdit_num_point->setValue(lds::groupSeparatorStr2v(num_point));
}
