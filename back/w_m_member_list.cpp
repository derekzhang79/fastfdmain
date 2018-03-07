#include "w_m_member.h"
#include "w_m_member_list.h"
//会员资料维护
#include "ui_w_m_member_dialog.h"
#include "lds_query.h"
#include "w_scr_dish_phone_pay.h"
#include "n_func.h"
#include "lds_messagebox.h"
#include <QSqlError>
#include <QtDebug>
#include "lds_model_sqltablemodel.h"
#include "lds_tableview_repecify.h"
#include "backheader.h"
#include "reader_rf.h"
#include "w_sys_manage_basicdataexport.h"
#include "public_cika_read_dialog.h"
#include "ui_w_m_member_sendcard_dialog.h"
#include "w_sys_manage_basicdataimport.h"
#include "w_scr_dish_pay_widget.h"
#include "w_sys_manage_cloudsync_with_time.h"
#include "w_bt_dish.h"
#ifdef Q_OS_WIN
#include "readcard_mwrf32.h"
#endif
#include "lds_model_sqltablemodel_delegate_com.h"

const lds_bool w_m_member_list::is_yun_member=false;

w_m_member_list::w_m_member_list(QWidget *parent) :
    QDialog(parent), ui(new Ui_w_m_member_Dialog)
{
    ui->setupUi(this);


    //! init
    //! treewidget
    ui->treeWidget->header()->hide();
    ui->treeWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->treeWidget->setColumnCount(1);

    //! tableview
    tablemodel=new lds_model_sqltablemodel(this);
    tablemodel->setEditStrategy(lds_model_sqltablemodel::OnManualSubmit);
    tablemodel->setTable("t_m_member");
    //! refresh
    tablemodel->save_set_header("vch_memberno", tr("会员编号"));
    tablemodel->save_set_header("vch_cardno", tr("卡面卡号"));
    tablemodel->save_set_header("vch_member", tr("会员名称"));
    tablemodel->save_set_header("dt_birthday", tr("生日"));
    tablemodel->save_set_header("vch_tel", tr("电话"));

    tablemodel->save_set_header("vch_handtel", tr("手机"));
    tablemodel->save_set_header("ch_state", tr("状态"));
    tablemodel->save_set_header("ch_cardflag", tr("持卡状态"));
    tablemodel->save_set_header("dt_limit", tr("有效期"));

    ui->tableView->setModel(tablemodel);
    ui->tableView->rePecifyHeader(tablemodel->get_save_set_header_saver_column_list());
    ui->tableView->setItemDelegateForColumn(tablemodel->fieldIndex("ch_state"), new lds_model_sqltablemodel_delegate_com(this, &w_m_member::get_VIP_MODEL_STATE()));
    ui->tableView->setItemDelegateForColumn(tablemodel->fieldIndex("ch_cardflag"), new lds_model_sqltablemodel_delegate_com(this, &w_m_member::get_VIP_MODEL_FLAG()));
    //! toolbar
    ui->widget->setup(QStringList()
                      << tr("会员类型")<< tr("新增") << tr("删除") << tr("批量新增")
                      << tr("发卡") << tr("充值") << tr("设密码")
                      << tr("刷新")
                      << tr("退出")
                      );


    connect(ui->widget->index_widget(tr("新增")), SIGNAL(clicked()),this,SLOT(tonew()));
    connect(ui->widget->index_widget(tr("删除")), SIGNAL(clicked()),this,SLOT(todel()));
    connect(ui->widget->index_widget(tr("批量新增")), SIGNAL(clicked()),this,SLOT(tonews()));
    connect(ui->widget->index_widget(tr("会员类型")), SIGNAL(clicked()),this,SLOT(tomembertype()));
    connect(ui->widget->index_widget(tr("发卡")), SIGNAL(clicked()),this,SLOT(tosendcard()));
    connect(ui->widget->index_widget(tr("充值")), SIGNAL(clicked()),this,SLOT(torecharge()));
    connect(ui->widget->index_widget(tr("设密码")), SIGNAL(clicked()),this,SLOT(tosetpwd()));
    connect(ui->widget->index_widget(tr("刷新")), SIGNAL(clicked()),this,SLOT(torefresh()));
    connect(ui->widget->index_widget(tr("退出")), SIGNAL(clicked()),this,SLOT(toexit()));

    connect(ui->treeWidget,SIGNAL(pressed(QModelIndex)),this,SLOT(treeselect()));
    connect(ui->tableView,SIGNAL(doubleClicked(QModelIndex)),this,SLOT(tosendcard()));
    connect(ui->lineEdit,SIGNAL(textChanged(QString)),this,SLOT(treeselect()));
    connect(ui->lineEdit,SIGNAL(returnPressed()),this,SLOT(tosendcard()));
    connect(ui->lineEdit_2,SIGNAL(textChanged(QString)),this,SLOT(treeselect()));

    if(lds::sysconf->value("w_sys_manage_cloudsync_with_time/cloud_no_use_local_member", false).toBool()){
        ui->widget->index_widget(tr("新增"))->setEnabled(false);
        ui->widget->index_widget(tr("删除"))->setEnabled(false);
        ui->widget->index_widget(tr("会员类型"))->setEnabled(false);
        ui->widget->index_widget(tr("批量新增"))->setEnabled(false);
    }
    ui->lineEdit->installEventFilter(this);
    tablemodel_arg3 =
            "    if('%1' = '', t_m_member.ch_typeno like '%', t_m_member.ch_typeno = '%1') "
            " and (vch_memberno like '%2%' or vch_member like '%2%' ) "
            " and (ifnull(vch_tel, '') like '%3%' or ifnull(vch_handtel, '') like '%3%') "
            ;
    QTimer::singleShot(0, this, SLOT(torefresh()));
}

lds_tableView *w_m_member_list::getTableview()
{
    return ui->tableView;
}

lds_treeWidget *w_m_member_list::getTreew()
{
    return ui->treeWidget;
}

lds_model_sqltablemodel *w_m_member_list::getTablemodel()
{
    return tablemodel;
}


void w_m_member_list::torefresh()
{
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
    treeselect();
    ui->lineEdit->setFocus();
}

void w_m_member_list::treeselect()
{
    QString ch_typeno;
    QString vch_memberno;
    QString vch_tel;

    if(ui->treeWidget->currentItem()){
        int index=ui->treeWidget->deepofitem(ui->treeWidget->currentItem());
        if(index==1){
            ch_typeno=backheader::getinterdata(ui->treeWidget->currentItem()->text(0));
        }
    }
    vch_memberno = ui->lineEdit->text();
    vch_tel = ui->lineEdit_2->text();
    tablemodel->setFilter(tablemodel_arg3.arg(ch_typeno).arg(vch_memberno).arg(vch_tel));
    ui->tableView->restore();
    ui->tableView->resizeColumnsToContents();
}

void w_m_member_list::setToolbarShown(bool f)
{
    ui->widget->index_widget(tr("会员类型"))->setVisible(f);
    ui->widget->index_widget(tr("删除"))->setVisible(f);
    ui->widget->index_widget(tr("批量新增"))->setVisible(f);
    ui->widget->index_widget(tr("充值"))->setVisible(f);
    ui->widget->index_widget(tr("设密码"))->setVisible(f);
    //    ui->widget->index_widget(tr("导出"))->setVisible(f);
    ui->widget->index_widget(tr("退出"))->setVisible(f);
    disconnect(ui->lineEdit,SIGNAL(returnPressed()),this,SLOT(tosendcard()));
}

void w_m_member_list::tonew()
{
    w_m_member_list_new dialog(this, tablemodel, ui->tableView);
    dialog.setWindowTitle(_TEXT_SLOT(this));
    lds_roundeddialog_rect_align(&dialog).exec();
}

void w_m_member_list::todel()
{
    int row=ui->tableView->currentIndex().row();
    if(row<0){
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID,tr("没有选中需要删除的会员"));
        return;
    }
    QString ls_memberno=tablemodel->model_data(row, "vch_memberno").toString();
    if(tablemodel->model_data(row, "ch_cardflag").toString()=="Y"){
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID,tr("已经发卡会员不能删除"));
        return;
    }

    lds_query query;
    query.execSelect(QString("select count(1) from t_m_deposit where vch_memberno='%1'").arg(ls_memberno));
    query.next();
    if(query.recordValue(0).toLongLong()>0){
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID,tr("会员已经存在金额历史，不能删除"));
        return;
    }
    query.execSelect(QString("select count(1) from t_m_pay where vch_memberno='%1'").arg(ls_memberno));
    query.next();
    if(query.recordValue(0).toLongLong()>0){
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID,tr("会员已经存在消费结算记录，不能删除"));
        return;
    }

    query.execSelect(QString("select count(1) "+n_func::f_m_switch_product("check_data_table","1") + " where vch_voucherno ='%1' and ch_paymodeno='01'").arg(ls_memberno));
    query.next();
    if(query.recordValue(0).toLongLong()>0){
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID,tr("会员已经存在消费结算记录，不能删除"));
        return;
    }
    if(0==lds_messagebox::question(this, MESSAGE_TITLE_VOID,tr("确认删除该会员?"), tr("确定"), tr("取消"))){
        //! <--transaction begin-->
        lds_query::tran_saction();
        if(query.execDelete("t_m_member", QString("vch_memberno='%1'").arg(ls_memberno))){
            lds_query::com_mit();
            lds_messagebox::information(this, MESSAGE_TITLE_VOID,tr("会员资料删除成功"));
        }else {
            lds_query::roll_back();
            lds_messagebox::warning(this, MESSAGE_TITLE_VOID,tr("会员资料删除失败"));
        }
        //! <--transaction end-->
        torefresh();
    }

}

void w_m_member_list::tonews()
{
    w_m_member_news dialog(this);
    dialog.setWindowTitle(_TEXT_SLOT(this));
    lds_roundeddialog_rect_align(&dialog).exec();
}

void w_m_member_list::tomembertype()
{
    w_m_member_list_type dialog(this);
    dialog.setWindowTitle(_TEXT_SLOT(this));
    lds_roundeddialog_rect_align(&dialog).exec();
    torefresh();
}

void w_m_member_list::tosendcard()
{
    QDateTime sqldt = n_func::f_get_sysdatetime();
    //云会员
    if(lds::sysconf->value("w_sys_manage_cloudsync_with_time/cloud_yun_member", false).toBool()){
        int selectindex;
        if(lds::sysconf->value("w_sys_manage_cloudsync_with_time/cloud_no_use_local_member", false).toBool()){
            selectindex = 0;
        } else {
            selectindex = lds_messagebox::question(this, MESSAGE_TITLE_VOID,  tr("选择模式") +"\n"+  tr("检测到您已经启用云会员"),tr("云会员"), tr("本地会员"),tr("退出"));
        }
        switch(selectindex){
        case 0://云会员
        {
            lds_dialog_input inputdialog(this);
            inputdialog.setWindowTitle(tr("输入-新增-会员信息"));
            inputdialog.ui->label->setText(tr("会员号:"));
            //获取所有的会员类型和类型号
            w_m_member_list_yunlock yunlocker;
            QString is_cardtype=n_func::f_m_get_parameter(this,"vip_card_type", tr("会员卡"));
            if(is_cardtype == tr("IC卡")){
                is_cardtype = "1";
            }
            while(1){//while
                QMap<QString, QVariant> comlist;
                if(w_sys_manage_cloudsync::get_yun_member_membertypelist(this, comlist)){
                    inputdialog.delay_set_focus();
                    if(QDialog::Accepted == lds_roundeddialog_rect_align(&inputdialog).exec()){
                        QString vch_memberno = inputdialog.ui->lineEdit->text();

                        if(vch_memberno.isEmpty()){
                            lds_messagebox::warning(this, MESSAGE_TITLE_VOID ,tr("有数据有空,请检查输入内容"));
                            break;
                        }
                        //发卡界面
                        membernewparam param;
                        Ui_w_m_member_sendcard_Dialog ui;
                        QDialog senddialog(this);
                        ui.setupUi(&senddialog);
                        senddialog.setWindowTitle(tr("云会员发卡"));
                        ui.label_record->setText("");
                        ui.comboBox_ch_typeno->addItems1(comlist);
                        backheader::ldsComSetdataByKVM(&w_m_member::get_VIP_MODEL_STATE(), ui.comboBox_ch_state);
                        backheader::ldsComSetdataByKVM(&w_m_member::get_VIP_MODEL_FLAG(), ui.comboBox_ch_cardflag);
                        ui.lineEdit_vch_cardno->setEnabled(false);
                        ui.lineEdit_vch_memberno->setEnabled(false);
                        ui.comboBox_ch_state->setEnabled(false);
                        ui.comboBox_ch_cardflag->setEnabled(false);
                        //检索已发卡的会员，
                        if(w_sys_manage_cloudsync::get_yun_member_byanyvchmemberno(this, vch_memberno, param)){
                            ui.comboBox_ch_typeno->setEnabled(false);//若是已发卡的话，会员类型不能修改
                            //已经有了 但是判断是否已发卡
                            if(param.miChState == "4"){
                                lds_messagebox::warning(this, MESSAGE_TITLE_VOID, tr("此会员已是作废状态，不能进行相应操作"));
                                break;
                            }
                            if(param.miChCardFlag == "Y"){
                                lds_messagebox::warning(this, MESSAGE_TITLE_VOID, tr("此会员已发卡，不能进行相应操作"));
                                break;
                            }
                            ui.lineEdit_vch_memberno->setText(param.miVchMemberNo);
                            ui.lineEdit_vch_member->setText(param.miVchMember);
                            ui.lineEdit_vch_cardno->setText(param.miVchCardNo);
                            ui.comboBox_ch_typeno->setcomusrdata(param.miChTypeNo);
                            ui.comboBox_ch_cardflag->setcomusrdata(param.miChCardFlag);
                            ui.comboBox_ch_state->setcomusrdata(param.miChState);
                            ui.lineEdit_vch_address->setText(param.miVchAddress);
                            ui.lineEdit_vch_handtel->setText(param.miVchHandTel);
                            ui.lineEdit_vch_id->setText(param.miVchCardNo);
                            ui.lineEdit_vch_tel->setText(param.miVchTel);
                            ui.dateTimeEdit_dt_birthday_show->setDateTime(QVariant(param.miDtBirthday).toDateTime());
                            ui.dateTimeEdit_dt_limit_show->setDateTime(QVariant(param.miDtLimit).toDateTime());
                            ui.dateTimeEdit_dt_operdate_show->setDateTime(QVariant(param.miDtOperDate).toDateTime());

                        } else {
                            lds_messagebox::information(this, MESSAGE_TITLE_VOID, tr("未发现该卡--->转为新增卡"));
                            ui.lineEdit_vch_memberno->setText(vch_memberno);
                            ui.lineEdit_vch_cardno->setText(vch_memberno);
                            ui.dateTimeEdit_dt_birthday_show->setDateTime(sqldt);
                            ui.dateTimeEdit_dt_limit_show->setDateTime(sqldt.addYears(1));
                            ui.dateTimeEdit_dt_operdate_show->setDateTime(sqldt);
                        }

                        ui.widget->setup(QStringList()<<tr("发卡")<<tr("取消"), Qt::AlignRight);
                        connect(ui.widget->index_widget(tr("发卡")) ,SIGNAL(clicked()),&senddialog, SLOT(accept()));
                        connect(ui.widget->index_widget(tr("取消")) ,SIGNAL(clicked()),&senddialog, SLOT(reject()));
                        while(QDialog::Accepted == lds_roundeddialog_rect_align(&senddialog).exec()){
                            if(ui.lineEdit_vch_member->text().isEmpty()){
                                lds_messagebox::warning(this, MESSAGE_TITLE_VOID, tr("会员名称不能为空"));
                                continue;
                            }
                            if(ui.lineEdit_vch_handtel->text().isEmpty()){
                                lds_messagebox::warning(this, MESSAGE_TITLE_VOID, tr("手机号不能为空"));
                                continue;
                            }
                            //新增
                            param.miVchMemberNo = ui.lineEdit_vch_memberno->text();
                            param.miVchMember = ui.lineEdit_vch_member->text();
                            param.miChTypeNo = ui.comboBox_ch_typeno->comusrdata().toString();
                            param.miVchId = ui.lineEdit_vch_id->text();

                            param.miDtBirthday = ui.dateTimeEdit_dt_birthday_show->dateTime().toString("yyyy-MM-dd hh:mm:ss");
                            param.miVchTel = ui.lineEdit_vch_tel->text();
                            param.miVchHandTel = ui.lineEdit_vch_handtel->text();
                            param.miVchAddress = ui.lineEdit_vch_address->text();
                            param.miDtLimit = ui.dateTimeEdit_dt_limit_show->dateTime().toString("yyyy-MM-dd hh:mm:ss");

                            param.miChState = "1";
                            param.miChStateStr = tr("正常");
                            //                            param.numLimit;
                            param.miChCardFlag = ui.comboBox_ch_cardflag->comusrdata().toString();
                            param.miVchCardNo = ui.lineEdit_vch_cardno->text();

                            //                            param.miVchPassword;
                            param.miVchOperId = public_sql::gs_operid;
                            param.miDtOperDate = sqldt.toString("yyyy-MM-dd hh:mm:ss");
                            //                            param.miIntBasedata;
                            //                            param.miIntBasedataStr;

                            param.miDtSendTime = sqldt.toString("yyyy-MM-dd hh:mm:ss");
                            param.orgCode = n_func::f_get_sysparm("orgCode");
                            //                            param.remark;
                            //                            param.idStr;

                            param.operate = sqldt.toString("yyyy-MM-dd hh:mm:ss");

                            if(w_sys_manage_cloudsync::get_yun_member_add(this, param)){
                                //发卡
                                if(is_cardtype == "1")
                                {
                                    //ic卡，需要写密码，并且写卡号
                                    QString icpasswd=n_func::f_m_get_parameter(this,"vip_ic_password");
                                    QByteArray transferpasswd = n_func::f_m_get_cardpwdtransfer(icpasswd);
                                    if(icpasswd.isEmpty()){
                                        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, tr("ic卡密码不能为空"));
                                        break;
                                    } else {
#ifdef Q_OS_WIN
                                        ReadCard_mwrf32 reader;reader.setDefPwd(n_func::f_m_defcardpwd);//需要设置一次默认密码，应为changepwd会不完全清卡
#else
                                        Reader_RF reader;
#endif
                                        //修改密码成功
                                        if(reader.pwdChange(n_func::f_m_defcardpwd, transferpasswd)){//0
                                            reader.setDefPwd(transferpasswd);
                                            //写数据
                                            if(reader.writecard(vch_memberno, "", "")){//1

                                            } else {//1
                                                lds_messagebox::warning(this, MESSAGE_TITLE_VOID, tr("ic卡，写入卡号失败"));
                                                break;
                                            }//1
                                        } else {//0
                                            lds_messagebox::warning(this, MESSAGE_TITLE_VOID, tr("ic卡，修改密码失败"));
                                            break;
                                        }//0
                                    }
                                }
                                if(w_sys_manage_cloudsync::get_yun_member_send(this, param.miVchMemberNo)){
                                    w_m_member_list::yun_member_close();
                                    lds_messagebox::warning(this, MESSAGE_TITLE_VOID, tr("发卡成功"));
                                    return;
                                }
                            }
                        }
                    }
                }
                break;
            }//while
        }
            return;
        case 1://本地会员
        {
            goto localmember;
        }
            return;
        case 2://退出
            return;
        }
    }

    goto localmember;
localmember:
    //普通
    if(ui->tableView->currentIndex().row() >= 0){
        statictosendcard(this, tablemodel, ui->tableView,true);
    } else {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID,tr("没有选中需要发卡的会员"));
    }
}

void w_m_member_list::statictosendcard(QWidget *parent, lds_model_sqltablemodel *tablemodel,lds_tableView *tableview, bool isenabled, const QString &title)
{
    int row=tableview->currentIndex().row();
    if(row>=0){
        w_m_member_list_sendcard dialog(parent, tablemodel, tableview, isenabled);
        dialog.setWindowTitle(title.isEmpty()?tr("会员卡发卡"):title);
        lds_roundeddialog_rect_align(&dialog).exec();
    }    else {
        lds_messagebox::warning(parent, MESSAGE_TITLE_VOID,tr("没有选中需要发卡的会员"));
    }
}

void w_m_member_list::statictorecharge(QWidget *parent, lds_model_sqltablemodel *tablemodel, lds_tableView *tableview, const QString &parm)
{
    if(parm=="C"){//取款金额
    } else if(parm=="D"){//退卡金额
    } else {
        if(!saas_pay_detail::SaaSFuncCheck(parent, "0303"/*云、本地会员储值*/)) {
            return;
        }
    }

    bool filter_by_handtel = true;
    if(!parm.isEmpty()
            ||(parm.isEmpty()&&backheader::operid_right_code(parent, "0404"))){
        w_m_member_list_recharge dialog(parent, tablemodel, tableview, parm, filter_by_handtel);
        lds_roundeddialog_rect_align(&dialog).exec();
        //        dialog.exec();
    }
}

void w_m_member_list::statictomembertype(w_m_member_list *parent)
{
    w_m_member_list_type dialog(parent);
    dialog.setWindowTitle(tr("会员类型"));
    lds_roundeddialog_rect_align(&dialog).exec();

}

bool w_m_member_list::static_to_memeber_readcard(QWidget *parent, QString &cardid, uo_rw_member &uo_member, bool ispay, bool isyun)
{
    vch_membernoData::chooseFlag choose = vch_membernoData::choose_local;
    if(isyun
            &&lds::sysconf->value("w_sys_manage_cloudsync_with_time/cloud_yun_member", false).toBool()
            ){
        if(lds::sysconf->value("w_sys_manage_cloudsync_with_time/cloud_no_use_local_member", false).toBool()){
            choose = vch_membernoData::choose_yun;
        } else {
            choose = vch_membernoData::choose_switch;
        }
    }

    return static_to_memeber_readcard_d(parent, cardid, uo_member, ispay, choose);
}

bool w_m_member_list::static_to_memeber_readcard_d(QWidget *parent, QString &cardid, uo_rw_member &uo_member, bool search_member, vch_membernoData::chooseFlag choose)
{
    QString cardno;
    QString errstring;
    bool ok;
    w_m_member_list::yun_member_close();
    lds_messagebox_loading_show loading(parent, MESSAGE_TITLE_VOID, "LOADING");

    switch(choose) {
    ////////////////////////////////////////////////////
    case vch_membernoData::choose_switch:
        switch(lds_messagebox::question(parent, MESSAGE_TITLE_VOID,  tr("选择模式") + "\n" + tr("检测到您已经启用云会员"),tr("云会员"), tr("本地会员"),tr("退出"))){
        case 0://云会员
            w_m_member_list::yun_member_open();
            break;
        case 1://本地会员
            break;
        default:
            return false;
        }//switch
        break;
        ////////////////////////////////////////////////////
    case vch_membernoData::choose_local:
        break;
        ////////////////////////////////////////////////////
    case vch_membernoData::choose_yun:
        w_m_member_list::yun_member_open();
        break;
    }

    loading.show_delay();
    uo_member.of_init(parent, "4", &ok);
    if(!ok) {
        goto failure;
    }
    loading.hide();
    if(uo_member.is_cardtype == "1") {//ic卡
        QString icpasswd=n_func::f_m_get_parameter(parent,"vip_ic_password");
        QByteArray transferpasswd = n_func::f_m_get_cardpwdtransfer(icpasswd);
#ifdef Q_OS_WIN
        ReadCard_mwrf32 reader;reader.setDefPwd(n_func::f_m_defcardpwd);//需要设置一次默认密码，应为changepwd会不完全清卡
#else
        Reader_RF reader;
#endif
        QString cardno_tmp1, cardno_tmp2;
        if(reader.setDefPwd(transferpasswd)){
            if(reader.readcard(cardno, cardno_tmp1, cardno_tmp2)){
                goto success;
            }
        }
        errstring=tr("读卡失败")+reader.lastError();
    }  else if(uo_member.is_cardtype=="2") {//磁卡
        loading.hide();

        public_cika_read_Dialog dialog(parent);
        //是前台支付但不是云会员
        if(search_member){
            dialog.setmemberlist();
        }
        dialog.setWindowTitle((w_m_member_list::is_yun_member?tr("云会员"):"")+tr("读卡"));
        if(lds_roundeddialog_rect_align(&dialog).exec()==QDialog::Accepted){
            cardno=dialog.getcardno();
            goto success;
        }
    } else {
        lds_messagebox::warning(parent, MESSAGE_TITLE_VOID,tr("无会员卡"));
        return false;
    }
    goto failure;
success:
    cardid=cardno.trimmed();
    return true;
failure:
    if(!errstring.isEmpty()){
        lds_messagebox::warning(parent, MESSAGE_TITLE_VOID,errstring);
    }
    w_m_member_list::yun_member_close();
    return false;
}

void w_m_member_list::yun_member_open()
{
    *(lds_bool *)& is_yun_member = true;
}

void w_m_member_list::yun_member_close()
{
    w_sys_manage_cloudsync::json_free();
    *(lds_bool *)& is_yun_member = false;
}

void w_m_member_list::torecharge()
{
    w_m_member_list::statictorecharge(this, tablemodel, ui->tableView,"");
}

void w_m_member_list::tosetpwd()
{
    if(ui->tableView->currentIndex().row() < 0){
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID,tr("没有行被选中"));
        return;
    }
    w_m_member_pwdchange dialog(this);
    QString error;
    if((error=dialog.check()).isEmpty()){
        dialog.setWindowTitle(_TEXT_SLOT(this));
        lds_roundeddialog_rect_align(&dialog).exec();
    } else {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID,error);
    }
}

void w_m_member_list::toexport()
{
    w_sys_manage_basicdataexport dialog(this);
    dialog.reset_export_tablenames(QStringList() << tablemodel->tableName());
    dialog.reset_export_confname(QString("%1/path").arg(this->metaObject()->className()));
    dialog.setWindowTitle(_TEXT_SLOT(this));
    lds_roundeddialog_rect_align(&dialog).exec();

}

void w_m_member_list::toimport()
{
    w_sys_manage_basicdataimport_sub dialog(QStringList() << tablemodel->tableName(), QString("%1/path").arg(this->metaObject()->className()), this);
    dialog.setWindowTitle(_TEXT_SLOT(this));
    lds_roundeddialog_rect_align(&dialog).exec();
}

void w_m_member_list::toexit()
{
    //yun
    w_sys_manage_cloudsync_with_time_running::exec_no_up_then_down_byconf(this);
    ui->widget->index_widget(tr("退出"))->show_flag_top_right(w_sys_manage_cloudsync_with_time_running::q_bt_get_ready_total_byconf());
    //~yun
    w_sys_manage_cloudsync_with_time_running::exec_keepdata_byconf(this);

    this->reject();
}

bool w_m_member_list::eventFilter(QObject *o, QEvent *e)
{
    if(QEvent::KeyPress == e->type()) {
        QKeyEvent *ke = ((QKeyEvent *)e);
        const int kvalue = ke->key();
        //kvalue;//46 32+14 0x2e
        if(kvalue == Qt::Key_Up){
            ui->tableView->selectRow(qMax(ui->tableView->currentIndex().row() - 1, 0));
            e->ignore();
            return true;
        }
        if(kvalue == Qt::Key_Down){
            ui->tableView->selectRow(qMin(ui->tableView->currentIndex().row() + 1, tablemodel->rowCount()));
            e->ignore();
            return true;
        }
    }
    return QDialog::eventFilter(o, e);
}

//====================================
#include <QDataWidgetMapper>
#include <QSqlRelationalDelegate>
#include <QTableView>
w_m_member_list_sendcard::w_m_member_list_sendcard(QWidget *parent, lds_model_sqltablemodel *m, lds_tableView *tableview, bool isenable):
    QDialog(parent),
    ui(new Ui_w_m_member_sendcard_Dialog),    _tablemodel(m),
    _tableview(tableview),
    _parent(parent)
{
    ui->setupUi(this);
    //! datawidget
    mapper = new QDataWidgetMapper(this);
    mapper->setSubmitPolicy(QDataWidgetMapper::AutoSubmit);
    mapper->setModel(_tablemodel);
    mapper->setItemDelegate(new QSqlRelationalDelegate(mapper));
    backheader::ldsComSetdataByKVM(&w_m_member::get_VIP_MODEL_STATE(), ui->comboBox_ch_state);
    backheader::ldsComSetdataByKVM(&w_m_member::get_VIP_MODEL_FLAG(), ui->comboBox_ch_cardflag);
    QSqlQueryModel querymodel;
    querymodel.setQuery("select ch_typeno, vch_typename from t_m_member_type");
    backheader::ldsComSetdataByKVM(&querymodel, ui->comboBox_ch_typeno);

    mapper->addMapping(ui->comboBox_ch_cardflag, _tablemodel->fieldIndex("ch_cardflag"),"comusrdata");
    mapper->addMapping(ui->comboBox_ch_state, _tablemodel->fieldIndex("ch_state"), "comusrdata");
    mapper->addMapping(ui->lineEdit_vch_address, _tablemodel->fieldIndex("vch_address"));
    mapper->addMapping(ui->lineEdit_vch_cardno, _tablemodel->fieldIndex("vch_cardno"));
    mapper->addMapping(ui->lineEdit_vch_handtel, _tablemodel->fieldIndex("vch_handtel"));
    mapper->addMapping(ui->lineEdit_vch_id, _tablemodel->fieldIndex("vch_id"));
    mapper->addMapping(ui->lineEdit_vch_member, _tablemodel->fieldIndex("vch_member"));
    mapper->addMapping(ui->lineEdit_vch_memberno, _tablemodel->fieldIndex("vch_memberno"));
    mapper->addMapping(ui->lineEdit_vch_operid, _tablemodel->fieldIndex("vch_operid"));
    mapper->addMapping(ui->lineEdit_vch_tel, _tablemodel->fieldIndex("vch_tel"));
    mapper->addMapping(ui->dateTimeEdit_dt_birthday_show,_tablemodel->fieldIndex("dt_birthday"));
    mapper->addMapping(ui->dateTimeEdit_dt_limit_show,_tablemodel->fieldIndex("dt_limit"));
    mapper->addMapping(ui->dateTimeEdit_dt_operdate_show,_tablemodel->fieldIndex("dt_operdate"));
    mapper->addMapping(ui->comboBox_ch_typeno, _tablemodel->fieldIndex("ch_typeno"), "comusrdata");
    //! tableview_other
    tablemodel=new lds_model_sqltablemodel(this);
    tablemodel->setEditStrategy(lds_model_sqltablemodel::OnManualSubmit);
    tablemodel->setTable("t_m_member_other");
    ui->tableView_other->setModel(tablemodel);
    ui->tableView_other->setEditTriggers(QTableView::AllEditTriggers);
    tablemodel->save_set_header("vch_item", tr("项目"), true);
    tablemodel->save_set_header("vch_memo", tr("内容"), true);
    ui->tableView_other->rePecifyHeader(tablemodel->get_save_set_header_saver_column_list());

    //! tableview_log
    tablemodel_3=new lds_model_sqltablemodel(this);
    tablemodel_3->setTable("t_m_member_log");
    ui->tableView_log->setModel(tablemodel_3);
    tablemodel_3->save_set_header("vch_memberno", tr("会员编号"));
    tablemodel_3->save_set_header("vch_matter", tr("项目"));
    tablemodel_3->save_set_header("vch_memo", tr("内容"));
    tablemodel_3->save_set_header("vch_operID", tr("操作人员"));
    tablemodel_3->save_set_header("dt_operdate", tr("操作时间"));
    ui->tableView_log->rePecifyHeader(tablemodel_3->get_save_set_header_saver_column_list());
    //! toolbar
    ui->widget_2->setup(QStringList()<<tr("新增")<<tr("删除"), Qt::AlignTop);
    ui->widget->setup(QStringList()<<tr("上一行")<<tr("下一行")<<tr("保存")<<tr("发卡")<<tr("退出"), Qt::AlignRight, lds_keytoolbutton::SYSKEY);
    connect(ui->widget_2->index_widget(tr("新增")),SIGNAL(clicked()),this,SLOT(tonew()));
    connect(ui->widget_2->index_widget(tr("删除")),SIGNAL(clicked()),this,SLOT(todel()));
    connect(ui->widget->index_widget(tr("发卡")),SIGNAL(clicked()),this,SLOT(tosendcard()));
    connect(ui->widget->index_widget(tr("上一行")),SIGNAL(clicked()),this,SLOT(toup()));
    connect(ui->widget->index_widget(tr("下一行")),SIGNAL(clicked()),this,SLOT(todown()));
    connect(ui->widget->index_widget(tr("保存")),SIGNAL(clicked()),this,SLOT(tosave()));
    connect(ui->widget->index_widget(tr("退出")),SIGNAL(clicked()),this,SLOT(toexit()));
    connect(ui->pushButton_ch_typeno, SIGNAL(clicked()), this, SLOT(toChangeType()));

    //! after
    //获取is_cardtype的内容
    QString ls_value=n_func::f_m_get_parameter(this,"vip_card_type", tr("无会员卡"));
    if(ls_value==tr("IC卡")){
        is_cardtype="1";
    } else if(ls_value==tr("磁卡")){
        is_cardtype="2";
    } else {//无会员
        is_cardtype="4";
    }
    //只是查看详细信息的
    if(!isenable){
        ui->frame_detail_show->setEnabled(isenable);
        ui->widget_2->setEnabled(isenable);
        ui->widget->index_widget(tr("发卡"))->setVisible(isenable);
        ui->widget->index_widget(tr("保存"))->setVisible(isenable);
    }
    //初始化数据
    cb5=new uo_rw_member;
    cb5->of_init(this, "2");//2是磁卡
    ui->comboBox_ch_cardflag->setEnabled(false);
    ui->comboBox_ch_state->setEnabled(false);
    ui->lineEdit_vch_operid->setEnabled(false);
    ui->dateTimeEdit_dt_operdate_show->setEnabled(false);

    ui->lineEdit_vch_memberno->setEnabled(false);
    ui->comboBox_ch_typeno->setEnabled(false);
    //更新当前index
    ui->label_record->setText(QString("%1/%2").arg(_tableview->currentIndex().row()+1).arg(_tableview->verticalHeader()->count()));

    //设置datetimelineedit为只有日期
    ui->dateTimeEdit_dt_birthday_show->setDisplayFormat("yyyy-MM-dd");
    ui->dateTimeEdit_dt_limit_show->setDisplayFormat("yyyy-MM-dd");
    ui->dateTimeEdit_dt_operdate_show->setDisplayFormat("yyyy-MM-dd");
    //! TIMESLOT
    QTimer::singleShot(0, this, SLOT(torefresh()));//信号槽的重写是需要这的写的，实现subclass 同时继承

}

void w_m_member_list_sendcard::tonew()
{
    int rowcount=tablemodel->rowCount();
    tablemodel->insertRow(rowcount);
    tablemodel->model_data_set(rowcount, "vch_memberno", ui->lineEdit_vch_memberno->text());
    tablemodel->model_data_set(rowcount, "vch_operid", public_sql::gs_operid);
}

void w_m_member_list_sendcard::todel()
{
    int row=ui->tableView_other->currentIndex().row();
    if(row<0){
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID,tr("请选择将要删除的记录"));
        return;
    }
    tablemodel->removeRow(row);
    if(tablemodel->trySubmitAll()){
        torefresh();
        lds_messagebox::information(this, MESSAGE_TITLE_VOID,tr("删除成功"));
        return;
    }
    lds_messagebox::warning(this, MESSAGE_TITLE_VOID,tr("数据库错误"));
}

void w_m_member_list_sendcard::tosendcard()
{
    lds_query query;
    query.execSelect(QString(" select vch_memberno from t_m_member where vch_memberno = '%1' ")
                     .arg(ui->lineEdit_vch_memberno->text()));
    if(false == query.next()) {
        if(1 == lds_messagebox::question(this, MESSAGE_TITLE_VOID, tr("会员卡不存在"), tr("新建"), tr("取消"))) {
            return;
        }
        if(false == createMember()) {
            return;
        }
    }
    sendCard();
}

void w_m_member_list_sendcard::toup()
{
    if(_tablemodel->model_is_dirty()){
        switch(lds_messagebox::question(this, MESSAGE_TITLE_VOID,tr("有数据未保存,确认保存?"), tr("保存"),tr("不保存"))){
        case 0:
            if(!tosave()){
                return;
            }
            break;
        case 1:
            break;
        }
    }
    _tableview->goprevious();
    mapper->setCurrentIndex(_tableview->currentIndex().row());
    torefresh();
    ui->label_record->setText(QString("%1/%2").arg(_tableview->currentIndex().row()+1).arg(_tableview->verticalHeader()->count()));
}

void w_m_member_list_sendcard::todown()
{
    if(_tablemodel->model_is_dirty()){
        switch(lds_messagebox::question(this, MESSAGE_TITLE_VOID,tr("有数据未保存,确认保存?"), tr("保存"),tr("不保存"))){
        case 0:
            if(!tosave()){
                return;
            }
            break;
        case 1:
            break;
        }
    }
    int row = _tableview->gonext();
    mapper->setCurrentIndex(row);
    torefresh();
    ui->label_record->setText(QString("%1/%2").arg(row+1).arg(_tableview->verticalHeader()->count()));
}

bool w_m_member_list_sendcard::tosave()
{
    QDateTime ldt;
    QDateTime ldt_old;
    QDateTime ldt_limit;
    QDateTime ldt_birth;
    QString ls_errtext;
    QString ls_memberno;
    QString ls_membername;
    QString ls_typeno;
    QString ls_attribute;
    QString ls_cardno;
    lds_query query;

    ls_memberno=ui->lineEdit_vch_memberno->text();
    ls_membername=ui->lineEdit_vch_member->text();
    ls_typeno=ui->comboBox_ch_typeno->comusrdata().toString();
    ls_cardno=ui->lineEdit_vch_cardno->text();
    if(ls_memberno.trimmed().isEmpty()){
        ls_errtext=tr("会员编号");
    } else if(ls_membername.trimmed().isEmpty()){
        ls_errtext=tr("会员名称");
    } else if(ls_typeno.trimmed().isEmpty()){
        ls_errtext=tr("会员类型");
    } else if(ls_cardno.trimmed().isEmpty()){
        if(is_cardtype=="1"){
            ls_errtext="";
        } else {
            ls_errtext=tr("卡面卡号");
        }
    }

    lds_query::tran_saction();
    switch(0){//switch
    case 0://0
        if(!ls_errtext.isEmpty()){
            ls_errtext=ls_errtext+tr("不能为空");
            break;
        }//0
    case 1://1
        ldt_limit=ui->dateTimeEdit_dt_limit_show->dateTime();
        if(ldt_limit < n_func::f_get_sysdatetime()){
            ls_errtext=ls_errtext+tr("有效期限不能小于当前日期");
            break;
        }//1
    case 2://2
        ldt_birth=ui->dateTimeEdit_dt_birthday_show->dateTime();
        if(ldt_birth < QDateTime::fromString("1800-01-01", "yyyy-MM-dd")){
            ls_errtext=ls_errtext+tr("系统不支持'超年龄'会员，请核实会员生日必须在1800年后");
            break;
        }//2
    case 3:{//3
        query.execSelect(QString("select count(0) from t_m_member where vch_cardno='%1' and vch_memberno <>'%2'").arg(ls_cardno).arg(ls_memberno));
        query.next();

        if(query.recordValue(0).toLongLong() > 0){
            ls_errtext=ls_errtext+tr("输入卡面卡号系统已存在，不能保存");
            break;
        }
        if(ui->lineEdit_vch_handtel->text().trimmed().isEmpty()){
            ls_errtext=ls_errtext+tr("手机号不能为空");
            break;
        }
    }//3
    case 4://4
        ldt=n_func::f_get_sysdatetime();
        ldt=ui->dateTimeEdit_dt_operdate_show->dateTime();
        if(ldt_old.isValid() && ldt_old > ldt){
            ls_errtext=ls_errtext+tr("系统时间小于上次保存时间,请确认系统时间是否正确");
            break;
        }
        ui->dateTimeEdit_dt_operdate_show->setDateTime(ldt);
        //4
    case 5://5
        if(!tablemodel->trySubmitAll()){
            ls_errtext=tr("数据库错误");
            break;
        }

        if(!tablemodel_3->trySubmitAll()){
            ls_errtext=tr("数据库错误");
            break;
        }
        //5
    case 6:
        ls_errtext.clear();
        if(!_tablemodel->trySubmitAll(&ls_errtext)) {
            break;
        }
        query.execSelect(QString("select ch_attribute from t_m_member_type where ch_typeno='%1'").arg(ls_typeno));
        query.next();
        ls_attribute=query.recordValue(0).toString();
        if(ls_attribute=="3"){
            ls_errtext=n_func::f_m_hang_curamount(ls_memberno);
            if(!ls_errtext.isEmpty()){
                break;
            }
        }
        if(!tablemodel->trySubmitAll(&ls_attribute)){
            break;
        }
        if(!tablemodel_3->trySubmitAll(&ls_attribute)){
            break;
        }
    case 7:break;
    }//switch
    //错误信息
    if(ls_errtext.isEmpty()){//! success
        lds_query::com_mit();
        //刷新
        torefresh();
        lds_messagebox::information(this, MESSAGE_TITLE_VOID,tr("会员资料保存成功"));
        return true;
    }
    //! failure
    lds_query::roll_back();
    lds_messagebox::warning(this, MESSAGE_TITLE_VOID,ls_errtext);
    return false;
}

void w_m_member_list_sendcard::toexit()
{
    this->reject();
}

void w_m_member_list_sendcard::toChangeType()
{
    w_m_member_list_type dialog(this);
    dialog.setWindowTitle(_TEXT_SLOT(this));
    lds_roundeddialog_rect_align(&dialog).exec();

    //save
    QVariant typeno = ui->comboBox_ch_typeno->curusrdata();
    //1
    QSqlQueryModel querymodel;
    querymodel.setQuery("select ch_typeno, vch_typename from t_m_member_type");
    backheader::ldsComSetdataByKVM(&querymodel, ui->comboBox_ch_typeno);
    //restore
    ui->comboBox_ch_typeno->setcomusrdata(typeno);

}

void w_m_member_list_sendcard::torefresh()
{
    _tablemodel->select();
    _tableview->resizeColumnsToContents();
    _tableview->restore();

    //mapper
    mapper->setCurrentIndex(_tableview->currentIndex().row());

    //tableView_other
    QString memberno=_tablemodel->model_data(_tableview->currentIndex().row(), "vch_memberno").toString();
    tablemodel->setFilter(QString("t_m_member_other.vch_memberno='%1'").arg(memberno));
    ui->tableView_other->restore();

    //tableView_log
    tablemodel_3->setFilter(QString("t_m_member_log.vch_memberno='%1'").arg(memberno));
    ui->tableView_log->restore();
}

bool w_m_member_list_sendcard::sendCard()
{
    //clicked (none) returns long [pbm_bnclicked]
    //long ancestorreturnvalue
    QString ls_memberno;
    QString ls_cardflag;
    QString ls_state;
    QString ls_cardno;
    QString ls_err;
    long ll_exist;
    lds_query query;
    ls_memberno = ui->lineEdit_vch_memberno->text();//PARENT.idw_main.object.vch_memberno[1]
    ls_cardflag = w_bt_dish::model_data(&w_m_member::get_VIP_MODEL_FLAG(), ui->comboBox_ch_cardflag->currentIndex(), 0);//ui->lineEdit_ch_cardflag_hide->text();//PARENT.idw_main.object.ch_cardflag[1]
    ls_state = w_bt_dish::model_data(&w_m_member::get_VIP_MODEL_STATE(), ui->comboBox_ch_state->currentIndex(), 0);//ui->lineEdit_ch_state_hide->text();//PARENT.idw_main.object.ch_state[1]
    ls_cardno = ui->lineEdit_vch_cardno->text();//PARENT.idw_main.object.vch_cardno[1]

    if(ls_memberno.isEmpty()) {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, tr("会员编号不能为空"));
        return false;
    }
    query.execSelect(QString(" select vch_memberno from t_m_member where vch_memberno = '%1' ")
                     .arg(ls_memberno));
    if(false == query.next()) {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, tr("会员卡不存在"));
        return false;
    }
    if(ls_cardno.trimmed().isEmpty()){
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID,tr("卡面卡号不能为空"));
        return false;
    }
    query.execSelect(QString("Select count(0) from t_m_member  where vch_cardno ='%1' and vch_memberno <> '%2' ")
                     .arg(ls_cardno)
                     .arg(ls_memberno)
                     );
    query.next();
    ll_exist=query.recordValue(0).toLongLong();
    if(ll_exist > 0){
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID,tr("输入的卡面卡号系统已存在，不能保存"));
        return false;
    }

    //! <--transaction begin-->
    lds_query::tran_saction();
    switch(0){
    case 0:
        if(!query.execUpdate("t_m_member",
                             "vch_cardno", ls_cardno,
                             QString("vch_memberno = '%1' ").arg(ls_memberno))){

            ls_err=tr("会员") + ls_memberno + tr("发卡失败");
            break;
        }
        if(ls_state=="4"){
            ls_err=tr("此会员已是作废状态，不能进行相应操作");
            break;
        } else if(ls_cardflag=="Y"){
            ls_err=tr("此会员已发卡~r~nIC卡可清除卡芯片再重发，非IC卡只能进行卡状态维护");
            break;
        } else {
            if((ls_err=cb5->of_send_card(this, ls_memberno)).isEmpty()){
                if(!(ls_err=n_func::f_m_insert_log(ls_memberno,tr("发卡"),"",0)).isEmpty()){
                    break;
                }
            }
        }
    }
    if(ls_err.isEmpty()){
        lds_query::com_mit();
        torefresh();
        lds_messagebox::information(this, MESSAGE_TITLE_VOID,tr("发卡成功"));
        return true;
    } else {
        lds_query::roll_back();
        torefresh();
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID,ls_err);
        return false;
    }
    //! <--transaction end-->
}

bool w_m_member_list_sendcard::createMember()
{
    QDateTime ldt_limit;
    QDateTime ldt_birth;
    QString ls_errtext;
    QString ls_memberno;
    QString ls_membername;
    QString ls_typeno;
    QString ls_attribute;
    QString ls_cardno;
    lds_query query;

    ls_memberno=ui->lineEdit_vch_memberno->text();
    ls_membername=ui->lineEdit_vch_member->text();
    ls_typeno=ui->comboBox_ch_typeno->comusrdata().toString();
    ls_cardno=ui->lineEdit_vch_cardno->text();
    if(ls_memberno.trimmed().isEmpty()){
        ls_errtext=tr("会员编号");
    } else if(ls_membername.trimmed().isEmpty()){
        ls_errtext=tr("会员名称");
    } else if(ls_typeno.trimmed().isEmpty()){
        ls_errtext=tr("会员类型");
    } else if(ls_cardno.trimmed().isEmpty()){
        if(is_cardtype=="1"){
            ls_errtext="";
        } else {
            ls_errtext=tr("卡面卡号");
        }
    }

    switch(0){//switch
    case 0://0
        query.execSelect(QString("select count(1) from t_m_member where vch_memberno='%1'").arg(ls_memberno));
        query.next();
        if(query.recordValue(0).toLongLong() > 0) {
            ls_errtext=tr("编号已经存在，请输入新的会员编号");
            break;
        }
        if(!ls_errtext.isEmpty()){
            ls_errtext=ls_errtext+tr("不能为空");
            break;
        }//0
    case 1://1
        ldt_limit=ui->dateTimeEdit_dt_limit_show->dateTime();
        if(ldt_limit < n_func::f_get_sysdatetime()){
            ls_errtext=ls_errtext+tr("有效期限不能小于当前日期");
            break;
        }//1
    case 2://2
        ldt_birth=ui->dateTimeEdit_dt_birthday_show->dateTime();
        if(ldt_birth < QDateTime::fromString("1800-01-01", "yyyy-MM-dd")){
            ls_errtext=ls_errtext+tr("系统不支持'超年龄'会员，请核实会员生日必须在1800年后");
            break;
        }//2
    case 3:{//3
        query.execSelect(QString("select count(0) from t_m_member where vch_cardno='%1' and vch_memberno <>'%2'").arg(ls_cardno).arg(ls_memberno));
        query.next();

        if(query.recordValue(0).toLongLong() > 0){
            ls_errtext=ls_errtext+tr("输入卡面卡号系统已存在，不能保存");
            break;
        }
        if(ui->lineEdit_vch_handtel->text().trimmed().isEmpty()){
            ls_errtext=ls_errtext+tr("手机号不能为空");
            break;
        }
    }//3
    case 4://4
        //4
    case 5://5
        if(!tablemodel->trySubmitAll()){
            ls_errtext=tr("数据库错误");

            break;
        }
        if(!tablemodel_3->trySubmitAll()){
            ls_errtext=tr("数据库错误");

            break;
        }
        //5
    case 6:
        //! <--transaction begin-->
        lds_query::tran_saction();
        ls_errtext.clear();
        if(!query.execInsert("t_m_member",qrtVariantPairList()

                             << qrtVariantPair("vch_memberno", ls_memberno)
                             << qrtVariantPair("vch_member", ls_membername)
                             << qrtVariantPair("ch_typeno", ls_typeno)
                             << qrtVariantPair("dt_limit", ldt_limit)
                             << qrtVariantPair("num_limit", 0)

                             << qrtVariantPair("vch_operID", public_sql::gs_operid)
                             << qrtVariantPair("dt_operdate", ui->dateTimeEdit_dt_operdate_show->dateTime())
                             << qrtVariantPair("vch_cardno", ls_cardno)
                             << qrtVariantPair("dt_birthday", ldt_birth)
                             << qrtVariantPair("vch_tel", ui->lineEdit_vch_tel->text())

                             << qrtVariantPair("vch_handtel", ui->lineEdit_vch_handtel->text())
                             << qrtVariantPair("vch_id", ui->lineEdit_vch_id->text())
                             << qrtVariantPair("vch_address", ui->lineEdit_vch_address->text()),
                             &ls_errtext)) {
            lds_query::roll_back();
            break;
        }
        query.execSelect(QString("select ch_attribute from t_m_member_type where ch_typeno='%1'").arg(ls_typeno));
        query.next();
        ls_attribute=query.recordValue(0).toString();
        if(ls_attribute=="3"){
            ls_errtext=n_func::f_m_hang_curamount(ls_memberno);
            if(!ls_errtext.isEmpty()){
                lds_query::roll_back();
                break;
            }
        }
        //! success
        lds_query::com_mit();
        //! <--transaction end-->
    case 7:break;
    }//switch

    //刷新
    torefresh();
    //错误信息
    if(ls_errtext.isEmpty()){//! success
        lds_messagebox::information(this, MESSAGE_TITLE_VOID,tr("会员资料保存成功"));
        return true;
    }
    //! failure
    lds_messagebox::warning(this, MESSAGE_TITLE_VOID,ls_errtext);
    return false;
}


//===================================
w_m_member_list_new::w_m_member_list_new(QWidget *parent, lds_model_sqltablemodel *tablemodel, lds_tableView *tableview, bool isnew):
    w_m_member_list_sendcard(parent, tablemodel, tableview, isnew)
{
    ui->tabWidget->setObjectName("tab");
    ui->widget->index_widget(tr("发卡"))->setVisible(true);
    ui->widget->index_widget(tr("上一行"))->setVisible(false);
    ui->widget->index_widget(tr("下一行"))->setVisible(false);
    ui->lineEdit_vch_memberno->setEnabled(true);
    ui->comboBox_ch_typeno->setEnabled(true);
    ui->label_record->setVisible(false);
    //    Removes the tab at position index from this stack of widgets. The page widget itself is not deleted.
    ui->tabWidget->removeTab(1);
    ui->tabWidget->removeTab(1);
    ui->dateTimeEdit_dt_limit_show->setDateTime(n_func::f_get_sysdatetime().addYears(30));
    ui->dateTimeEdit_dt_operdate_show->setDateTime(n_func::f_get_sysdatetime());
    ui->dateTimeEdit_dt_birthday_show->setDateTime(n_func::f_get_sysdatetime());
    ui->lineEdit_vch_operid->setText(public_sql::gs_operid);
    ui->comboBox_ch_cardflag->setcomusrdata("N");
}

void w_m_member_list_new::updateVch_tel_handtel(const QString &vch_tel)
{
    ui->lineEdit_vch_tel->setText(vch_tel);
    ui->lineEdit_vch_handtel->setText(vch_tel);
}

bool w_m_member_list_new::tosave()
{
    return createMember();
}

void w_m_member_list_new::torefresh()
{
    _tablemodel->select();
    _tableview->restore();

    //mapper
    mapper->setCurrentIndex(-1);
    w_m_member_list * mlist = qobject_cast<w_m_member_list *> (_parent);
    if(mlist)
        ui->comboBox_ch_typeno->setCurrentIndex(mlist->getTreew()->get_0_Index());
    //tableView_other
    QString memberno=_tablemodel->model_data(_tableview->currentIndex().row(), "vch_memberno").toString();
    tablemodel->setFilter(QString("t_m_member_other.vch_memberno='%1'").arg(memberno));
    ui->tableView_other->restore();

    //tableView_log
    tablemodel_3->select();
    ui->tableView_log->restore();

    //新增时，默认焦点
    ui->lineEdit_vch_memberno->setFocus();

}

void w_m_member_list_new::tosavesend()
{
    if(createMember()){
        if(sendCard()){
            clearData();
        }
    }
}

void w_m_member_list_new::clearData()
{
    ui->lineEdit_vch_address->clear();
    ui->lineEdit_vch_cardno->clear();
    ui->lineEdit_vch_handtel->clear();
    ui->lineEdit_vch_id->clear();
    ui->lineEdit_vch_member->clear();
    ui->lineEdit_vch_memberno->clear();
    ui->lineEdit_vch_operid->clear();
    ui->lineEdit_vch_tel->clear();
}


//====================================
#include "ui_w_bt_dish_unitset_dialog.h"
#include "lds_model_sqltablemodel.h"
#include "lds_tableview_repecify.h"

w_m_member_list_type::w_m_member_list_type(QWidget *parent):
    QDialog(parent)
{
    ui = new Ui_w_bt_dish_unitset_Dialog;
    ui->setupUi(this);
    //! tableview
    tablemodel=new lds_model_sqltablemodel;
    tablemodel->setTable("t_m_member_type");
    tablemodel->setEditStrategy(lds_model_sqltablemodel::OnManualSubmit);
    ui->tableView->setEditTriggers(QTableView::AllEditTriggers);
    ui->tableView->setModel(tablemodel);
    ui->tableView->setItemDelegateForColumn(tablemodel->fieldIndex("ch_attribute"),
                                            new lds_model_sqltablemodel_delegate_com(this, &w_m_member::get_VIP_MODEL_ATTRI()));
    ui->tableView->setItemDelegateForColumn(tablemodel->fieldIndex("ch_favourable"),
                                            new lds_model_sqltablemodel_delegate_com(this, &w_m_member::get_VIP_MODEL_FAVOUR()));
    ui->tableView->setItemDelegateForColumn(tablemodel->fieldIndex("ch_calc"),
                                            new lds_model_sqltablemodel_delegate_com(this, &w_m_member::get_VIP_MODEL_CALC()));

    //!refresh
    torefresh();
    //! toolbar
    ui->widget->setup(QStringList()<<tr("新增")<<tr("修改")<<tr("删除")<<tr("刷新")<<tr("退出"), Qt::AlignRight);
    connect(ui->widget->index_widget(tr("新增")),SIGNAL(clicked()),this,SLOT(tonew()));
    connect(ui->widget->index_widget(tr("修改")),SIGNAL(clicked()),this,SLOT(tochange()));
    connect(ui->widget->index_widget(tr("删除")),SIGNAL(clicked()),this,SLOT(todel()));
    connect(ui->widget->index_widget(tr("刷新")),SIGNAL(clicked()),this,SLOT(torefresh()));
    connect(ui->widget->index_widget(tr("退出")),SIGNAL(clicked()),this,SLOT(toexit()));
    connect(ui->tableView,SIGNAL(doubleClicked(QModelIndex)),this,SLOT(tochange()));

    //! after
    tablemodel->save_set_header("ch_typeno",tr("类型编码"));
    tablemodel->save_set_header("vch_typename",tr("类型名称"));
    tablemodel->save_set_header("ch_attribute",tr("会员卡种类"));
    tablemodel->save_set_header("ch_favourable",tr("优惠方式"));
    tablemodel->save_set_header("ch_calc",tr("是否积分"));

    ui->tableView->rePecifyHeader(tablemodel->get_save_set_header_saver_column_list());
    if(lds::get_soft_curent_language_first() != "CN")
        ui->tableView->resizeColumnsToContents();
}

lds_model_sqltablemodel *w_m_member_list_type::getTablemodel()
{
    return tablemodel;
}

lds_tableView *w_m_member_list_type::getTableView()
{
    return ui->tableView;
}

void w_m_member_list_type::tonew()
{
    w_m_member_list_type_new dialog(this);
    dialog.setWindowTitle(_TEXT_SLOT(this));lds_roundeddialog_rect_align(&dialog).exec();
}

void w_m_member_list_type::todel()
{
    int row=ui->tableView->currentIndex().row();
    if(row>=0){
        if(0==lds_messagebox::question(this, MESSAGE_TITLE_VOID,tr("确认删除?"), tr("确认"),tr("取消"))){
            lds_query query;
            query.execSelect(QString("select count(0) from t_m_member where ch_typeno='%1'").arg(tablemodel->model_data(row, "ch_typeno").toString()));
            query.next();
            if(query.recordValue(0).toLongLong() > 0){
                lds_messagebox::warning(this, MESSAGE_TITLE_VOID,tr("会员类型已经使用,不能删除"));
                return;
            }
            tablemodel->removeRow(row);
            if(tablemodel->trySubmitAll()){
                ui->tableView->restore();
            } else {
                lds_messagebox::warning(this, MESSAGE_TITLE_VOID,tr("数据库错误"));
            }
        }
    } else {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID,tr("没有所选行"));
    }
}

bool w_m_member_list_type::tosave()
{
    return true;
}

void w_m_member_list_type::torefresh()
{
    tablemodel->select();
    ui->tableView->restore();
}

void w_m_member_list_type::toexit()
{
    this->reject();
}

void w_m_member_list_type::tochange()
{
    int row = ui->tableView->currentIndex().row();
    if(row < 0) {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, tr("没有数据被选中"));
        return;
    }

    w_m_member_list_type_change dialog(this);
    dialog.setWindowTitle(tr("修改"));
    lds_roundeddialog_rect_align(&dialog).exec();

}
//===================================
#include "ui_w_m_member_list_type_new_dialog.h"
w_m_member_list_type_change::w_m_member_list_type_change(w_m_member_list_type *parent):
    QDialog(parent),
    ui(new Ui_w_m_member_list_type_new_Dialog),
    _parent(parent)
{
    ui->setupUi(this);
    backheader::ldsComSetdataByKVM(&w_m_member::get_VIP_MODEL_AREA(), ui->comboBox_ch_area_flag);
    backheader::ldsComSetdataByKVM(&w_m_member::get_VIP_MODEL_ATTRI(), ui->comboBox_ch_attribute);
    backheader::ldsComSetdataByKVM(&w_m_member::get_VIP_MODEL_CALC(), ui->comboBox_ch_calc);
    backheader::ldsComSetdataByKVM(&w_m_member::get_VIP_MODEL_CALCTYPE(), ui->comboBox_ch_calc_type);
    backheader::ldsComSetdataByKVM(&w_m_member::get_VIP_MODEL_FAVOUR(), ui->comboBox_ch_favourable);
    backheader::ldsComSetdataByKVM(&w_m_member::get_VIP_MODEL_M_PRICE(), ui->comboBox_ch_pricekind);

    connect(ui->comboBox_ch_attribute,SIGNAL(currentIndexChanged(int)),this,SLOT(is_recharge(int)));
    connect(ui->comboBox_ch_calc,SIGNAL(currentIndexChanged(int)),this,SLOT(is_point(int)));
    connect(ui->comboBox_ch_favourable,SIGNAL(currentIndexChanged(int)),this,SLOT(is_fav(int)));


    mapper = new QDataWidgetMapper(this);
    mapper->setSubmitPolicy(QDataWidgetMapper::AutoSubmit);
    mapper->setModel(_parent->tablemodel);
    mapper->setItemDelegate(new QSqlRelationalDelegate(mapper));

    mapper->addMapping(ui->lineEdit_int_backcash_rate, _parent->tablemodel->fieldIndex("int_backcash_rate"));
    mapper->addMapping(ui->lineEdit_ch_typeno, _parent->tablemodel->fieldIndex("ch_typeno"));
    mapper->addMapping(ui->lineEdit_int_calc_unit, _parent->tablemodel->fieldIndex("int_calc_unit"));
    mapper->addMapping(ui->lineEdit_int_card_baseamount, _parent->tablemodel->fieldIndex("int_card_baseamount"));
    mapper->addMapping(ui->lineEdit_int_discount, _parent->tablemodel->fieldIndex("int_discount"));
    mapper->addMapping(ui->lineEdit_vch_typename, _parent->tablemodel->fieldIndex("vch_typename"));

    mapper->addMapping(ui->comboBox_ch_area_flag, _parent->tablemodel->fieldIndex("ch_area_flag"), "comusrdata");
    mapper->addMapping(ui->comboBox_ch_attribute, _parent->tablemodel->fieldIndex("ch_attribute"), "comusrdata");
    mapper->addMapping(ui->comboBox_ch_calc, _parent->tablemodel->fieldIndex("ch_calc"), "comusrdata");
    mapper->addMapping(ui->comboBox_ch_calc_type, _parent->tablemodel->fieldIndex("ch_calc_type"), "comusrdata");
    mapper->addMapping(ui->comboBox_ch_favourable, _parent->tablemodel->fieldIndex("ch_favourable"), "comusrdata");
    mapper->addMapping(ui->comboBox_ch_pricekind, _parent->tablemodel->fieldIndex("ch_pricekind"), "comusrdata");

    //! toolbar
    ui->widget->setup(QStringList()<<tr("上一行")<<tr("下一行")<<tr("保存")<<tr("退出"), Qt::AlignRight);
    ui->widget->setup(QStringList()<<tr("保存")<<tr("退出"),Qt::AlignRight);
    connect(ui->widget->index_widget(tr("保存")), SIGNAL(clicked()),this,SLOT(tosave()));
    connect(ui->widget->index_widget(tr("退出")), SIGNAL(clicked()),this,SLOT(toexit()));
    connect(ui->widget->index_widget(tr("上一行")),SIGNAL(clicked()),this,SLOT(toup()));
    connect(ui->widget->index_widget(tr("下一行")),SIGNAL(clicked()),this,SLOT(todown()));

    connect(ui->comboBox_ch_calc, SIGNAL(currentIndexChanged(int)), this,SLOT(toSaaSCheck(int)));

    ui->lineEdit_ch_typeno->setEnabled(false);
    ui->comboBox_ch_attribute->setEnabled(false);
    is_fav(ui->comboBox_ch_favourable->currentIndex());
    QTimer::singleShot(0, this,SLOT(mapperupdate()));
}

bool w_m_member_list_type_change::tosave()
{
    if(_parent->tablemodel->trySubmitAll()){
        lds_messagebox::information(this, MESSAGE_TITLE_VOID,tr("保存成功"));
        return true;
    } else {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID,tr("数据库错误")+QString("%1").arg(__LINE__));
        return false;
    }
}

void w_m_member_list_type_change::toexit()
{
    if(_parent->tablemodel->model_is_dirty()){
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
    _parent->tablemodel->select();
    _parent->ui->tableView->restore();
    this->reject();
}

void w_m_member_list_type_change::is_recharge(int index)
{
    bool enabled = (ui->comboBox_ch_attribute->comusrdata(index).toString() == "2");//充值卡
    ui->lineEdit_int_backcash_rate->setEnabled(enabled);
    ui->lineEdit_int_card_baseamount->setEnabled(enabled);
}

void w_m_member_list_type_change::is_point(int index)
{
    bool enabled = (index==0);//积分
    ui->comboBox_ch_area_flag->setEnabled(enabled);
    ui->comboBox_ch_calc_type->setEnabled(enabled);
    ui->lineEdit_int_calc_unit->setEnabled(enabled);

}

void w_m_member_list_type_change::is_fav(int index)
{
    ui->comboBox_ch_pricekind->setEnabled(false);
    ui->lineEdit_int_discount->setEnabled(false);
    if(ui->comboBox_ch_favourable->itemData(index).toString() == "1"){//会员特价
        ui->comboBox_ch_pricekind->setEnabled(true);
    }
    if(ui->comboBox_ch_favourable->itemData(index).toString() == "2"){//固定折扣
        ui->lineEdit_int_discount->setEnabled(true);
    }
}

void w_m_member_list_type_change::toSaaSCheck(int index)
{
    Q_UNUSED(index)

    if(ui->comboBox_ch_calc == this->sender()) {
        if(ui->comboBox_ch_calc->curusrdata() == "Y") {
            if(!saas_pay_detail::SaaSFuncCheck(this, "0301"/*云、本地会员积分*/)) {
                ui->comboBox_ch_calc->setcomusrdata("N");
            }
        }
    }
}

void w_m_member_list_type_change::mapperupdate()
{
    mapper->setCurrentIndex(_parent->ui->tableView->currentIndex().row());
}

void w_m_member_list_type_change::toup()
{
    if(_parent->tablemodel->model_is_dirty()){
        switch(lds_messagebox::question(this, MESSAGE_TITLE_VOID,tr("有数据未保存,确认保存?"), tr("保存"),tr("不保存"))){
        case 0:
            if(!tosave()){
                return;
            }
            break;
        case 1:
            break;
        }
    }

    _parent->ui->tableView->goprevious();
    mapperupdate();
    ui->label_record->setText(QString("%1/%2").arg(_parent->ui->tableView->currentIndex().row()+1).arg(_parent->ui->tableView->verticalHeader()->count()));
}

void w_m_member_list_type_change::todown()
{
    if(_parent->tablemodel->model_is_dirty()){
        switch(lds_messagebox::question(this, MESSAGE_TITLE_VOID,tr("有数据未保存,确认保存?"), tr("保存"),tr("不保存"))){
        case 0:
            if(!tosave()){
                return;
            }
            break;
        case 1:
            break;
        }
    }

    _parent->ui->tableView->gonext();
    mapperupdate();
    ui->label_record->setText(QString("%1/%2").arg(_parent->ui->tableView->currentIndex().row()+1).arg(_parent->ui->tableView->verticalHeader()->count()));
}

//===================================
w_m_member_list_type_new::w_m_member_list_type_new(w_m_member_list_type *parent):
    w_m_member_list_type_change(parent)
{
    ui->widget->index_widget(tr("上一行"))->setVisible(false);
    ui->widget->index_widget(tr("下一行"))->setVisible(false);
    ui->lineEdit_ch_typeno->setEnabled(true);
    ui->comboBox_ch_attribute->setEnabled(true);
}

bool w_m_member_list_type_new::tosave()
{
    QString ls_memeber_typeno=ui->lineEdit_ch_typeno->text();
    QString ls_member_name=ui->lineEdit_vch_typename->text();
    QString ls_mess;
    QString ls_attribute=ui->comboBox_ch_attribute->currentText();
    bool ok;
    switch(0){
    case 0:
        if(ls_memeber_typeno.isEmpty()){
            ls_mess=tr("类型编号不能为空");
            break;
        }
        if(ls_member_name.isEmpty()){
            ls_mess=tr("会员卡类型名称不能为空");
            break;
        }
        if(ls_attribute.isEmpty()){
            ls_mess=tr("会员种类不能为空");
            break;
        }
        if(ls_memeber_typeno.length() != 2){
            ls_mess=tr("类型编号")+tr("必须为2位");
            break;

        }
    case 1:
        if(ui->lineEdit_int_backcash_rate->text().toInt() < 0){
            ls_mess=tr("充值卡付款还现比率不能小于0");
            break;
        }
        if(ui->lineEdit_int_backcash_rate->text().toInt() >= 100){
            ls_mess=tr("充值卡付款还现比率不能大于等于100");
            break;
        }
        if(ui->lineEdit_int_card_baseamount->value() < 0){
            ls_mess=tr("充值卡保本金额不能小于0");
            break;
        }

        ui->lineEdit_int_discount->text().toInt(&ok);
        if(!ok  && ui->lineEdit_int_discount->text().isEmpty()){
            ls_mess=tr("固定折扣为空");
            break;
        }
        if(!ok  && !ui->lineEdit_int_discount->text().isEmpty()){
            ls_mess=tr("固定折扣应当为纯数字");
            break;
        }

        if(ui->lineEdit_int_discount->text().toInt() < 0){
            ls_mess=tr("固定折扣不能小于0");
            break;
        }
        if(ui->lineEdit_int_discount->text().toInt() >100){
            ls_mess=tr("固定折扣不能大于100");
            break;
        }
    case 2:{
        lds_query query;
        query.execSelect(QString("select count(0) from t_m_member_type where ch_typeno = '%1' ")
                         .arg(ls_memeber_typeno));
        query.next();
        if(query.recordValue(0).toLongLong() > 0){
            ls_mess=tr("编号已经存在,请输入新的会员类型编号");
            break;
        }
        //! <--transaction begin-->
        lds_query::tran_saction();
        if(query.execInsert("t_m_member_type",qrtVariantPairList()

                            << qrtVariantPair("ch_typeno", ui->lineEdit_ch_typeno->text())
                            << qrtVariantPair("vch_typename", ui->lineEdit_vch_typename->text())
                            << qrtVariantPair("ch_attribute", ui->comboBox_ch_attribute->comusrdata())
                            << qrtVariantPair("int_discount", ui->lineEdit_int_discount->text())
                            << qrtVariantPair("ch_calc", ui->comboBox_ch_calc->comusrdata())

                            << qrtVariantPair("ch_calc_type", ui->comboBox_ch_calc_type->comusrdata())
                            << qrtVariantPair("int_calc_unit", ui->lineEdit_int_calc_unit->value())
                            << qrtVariantPair("ch_pricekind", ui->comboBox_ch_pricekind->comusrdata())
                            << qrtVariantPair("ch_area_flag", ui->comboBox_ch_area_flag->comusrdata())
                            << qrtVariantPair("ch_favourable", ui->comboBox_ch_favourable->comusrdata())

                            << qrtVariantPair("int_card_baseamount", ui->lineEdit_int_card_baseamount->value())
                            << qrtVariantPair("int_backcash_rate", ui->lineEdit_int_backcash_rate->text()),
                            &ls_mess)) {
            //! <--transaction commit-->
            lds_query::com_mit();
            _parent->tablemodel->select();
            lds_messagebox::information(this, MESSAGE_TITLE_VOID,tr("保存成功"));
            this->accept();
            return true;
        }
        lds_query::roll_back();
    }
        break;
    }
    //! <--transaction commit-->
    _parent->tablemodel->select();
    lds_messagebox::warning(this, MESSAGE_TITLE_VOID,ls_mess);
    return false;
}

void w_m_member_list_type_new::toexit()
{
    this->reject();
}

void w_m_member_list_type_new::mapperupdate()
{
    is_recharge(ui->comboBox_ch_attribute->currentIndex());
    is_point(ui->comboBox_ch_calc->currentIndex());
    is_fav(ui->comboBox_ch_favourable->currentIndex());
}

//===================================
#include "ui_w_m_member_list_recharge_dialog.h"
w_m_member_list_recharge::w_m_member_list_recharge(QWidget *parent, lds_model_sqltablemodel *tablemodel, lds_tableView *tableview, const QString &parm, bool filter_by_handtel):
    QDialog(parent),
    ui(new Ui_w_m_member_list_recharge_Dialog),
    _isready(false),
    _tablemodel(tablemodel),
    _tableview(tableview),
    _parm(parm)

{
    ui->setupUi(this);

    _filter_by_handtel = filter_by_handtel;
    ui->pushButton_repay->setText(tr("充值"));
    backheader::ldsComSetdataByKVM(&w_m_member::get_RECHARGE_MODEL_PAYMODE_RECHARGE(), ui->comboBox_ch_pay_mode);
    //! init
    this->setWindowTitle(tr("会员卡充值"));
    if(_parm=="C"){
        ui->lineEdit_num_realamount->setVisible(false);
        ui->label_num_realamount_t->setVisible(false);
        ui->label_num_deposit_t->setText(tr("取款金额"));
        ui->label_amount_t->setText(tr("取后面额"));
        ui->label_operdate_t->setText(tr("取款时间"));
        ui->pushButton_repay->setText(tr("取款"));
        this->setWindowTitle(tr("会员卡取款"));
        backheader::ldsComSetdataByKVM(&w_m_member::get_RECHARGE_MODEL_PAYMODE_FETCH(), ui->comboBox_ch_pay_mode);
    } else if(_parm=="D"){
        ui->lineEdit_num_realamount->setVisible(false);
        ui->label_num_realamount_t->setVisible(false);
        ui->label_num_deposit_t->setText(tr("退卡金额"));
        ui->label_amount_t->setText(tr("退后面额"));
        ui->label_operdate_t->setText(tr("退款时间"));
        ui->pushButton_repay->setText(tr("退卡"));
        this->setWindowTitle(tr("会员卡退卡"));
        backheader::ldsComSetdataByKVM(&w_m_member::get_RECHARGE_MODEL_PAYMODE_FETCH(), ui->comboBox_ch_pay_mode);
    }
    connect(ui->comboBox_ch_pay_mode,SIGNAL(currentIndexChanged(int)),this,SLOT(tocheckpaymode(int)));
    connect(ui->pushButton_print, SIGNAL(clicked()),this,SLOT(toremainprint()));
    connect(ui->pushButton_read, SIGNAL(clicked()),this,SLOT(toreadcard()));
    connect(ui->pushButton_repay, SIGNAL(clicked()),this,SLOT(torecharge()));
    connect(ui->lineEdit_num_deposit, SIGNAL(returnPressedDelay()),this,SLOT(torechargeOrRead()));
    connect(ui->lineEdit_num_realamount, SIGNAL(returnPressedDelay()),this,SLOT(torechargeOrRead()));

    connect(ui->pushButton_exit, SIGNAL(clicked()),this,SLOT(toexit()));
    connect(ui->lineEdit_num_realamount,SIGNAL(valueChanged(double)),ui->lineEdit_num_deposit,SLOT(setValue(double)));
    connect(ui->lineEdit_num_deposit,SIGNAL(valueChanged(double)),this,SLOT(TtolineEdit_amount()));
    connect(ui->checkBox,SIGNAL(toggled(bool)),this,SLOT(tochecksave()));
    connect(ui->checkBox_2,SIGNAL(toggled(bool)),this,SLOT(tochecksave_2()));
    connect(ui->pushButton_50,SIGNAL(clicked()),this,SLOT(toreplacenum()));
    connect(ui->pushButton_100,SIGNAL(clicked()),this,SLOT(toreplacenum()));
    connect(ui->pushButton_200,SIGNAL(clicked()),this,SLOT(toreplacenum()));
    connect(ui->pushButton_500,SIGNAL(clicked()),this,SLOT(toreplacenum()));
    connect(ui->pushButton_1000,SIGNAL(clicked()),this,SLOT(toreplacenum()));
    //! after

    ui->checkBox->setChecked(lds::sysconf->value("w_m_member_list_recharge/isprint").toBool());
    ui->checkBox_2->setChecked(lds::sysconf->value("w_m_member_list_recharge/rechange_open_drawer").toBool());

    ui->lineEdit_vch_memberno->setEnabled(false);
    ui->lineEdit_vch_member->setEnabled(false);
    ui->lineEdit_amount->setEnabled(false);
    ui->lineEdit_remain->setEnabled(false);
    ui->lineEdit_vch_operid->setEnabled(false);
    ui->lineEdit_operdate->setEnabled(false);

    ui->pushButton_exit->setFocusPolicy(Qt::NoFocus);
    ui->pushButton_print->setFocusPolicy(Qt::NoFocus);
    ui->pushButton_read->setFocusPolicy(Qt::NoFocus);
    ui->pushButton_repay->setFocusPolicy(Qt::NoFocus);
}

w_m_member_list_recharge::~w_m_member_list_recharge()
{
    w_m_member_list::yun_member_close();
}

void w_m_member_list_recharge::ue_fetch()
{
    //ue_fetch (none) returns integer
    QString ls_memberno;
    QString ls_deposit_mode;
    QString ls_pay_mode;
    QString ls_memo;
    QString ls_err;
    double ldec_deposit;
    double ldec_realamount;
    double ldec_remain;
    double ldec_amount;
    long ll_row;
    QDateTime ldt;
    QDateTime ldt_old;

    n_func::st_member_type  lst_member;

    ls_deposit_mode = "8";

    if(_tableview)ll_row = _tableview->currentIndex().row();
    if(_tablemodel)_tablemodel->model_data(ll_row, "remain").toDouble();
    ls_memberno=ui->lineEdit_vch_memberno->text();
    ldec_deposit=0-ui->lineEdit_num_deposit->value();
    ldec_realamount = ldec_deposit;
    ls_pay_mode = ui->comboBox_ch_pay_mode->comusrdata().toString();
    ls_memo = ui->lineEdit_vch_memo->text();
    ldec_remain = ui->lineEdit_remain->value();
    ldec_amount = ldec_deposit + ldec_remain;

    if(ls_memberno.trimmed().isEmpty()){
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID,tr("请先读卡/查卡,再充值"));
        return;
    }

    if(ldec_deposit>=0){
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID,tr("取款金额不能为非正数"));
        return;
    }

    lst_member = n_func::f_m_get_limit(this, ls_memberno);
    //2

    if(lst_member.as_m_state != "1"){
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID,tr("取款功能只针对正常状态会员，操作中止"));
        return;
    }
    if(lst_member.as_m_att != "2"){
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID,tr("取款功能只针对充值类会员，操作中止"));
        return;
    }
    ldt = n_func::f_get_sysdatetime();
    ldt_old = lst_member.dt_operdate;
    if(ldt_old.isValid() && ldt_old > ldt){
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID,tr("系统时间小于上次保存时间，请确认系统时间是否正确"));
        return;
    }
    //3
    if(n_func::f_m_get_parameter(this,"m_fetch_base", "0")=="1"){
        if(ldec_amount < 0){
            lds_messagebox::warning(this, MESSAGE_TITLE_VOID,tr("取款金额超过可用金额"));
            return;
        }
    }else {
        if((ldec_amount - lst_member.adec_m_limitdata) < 0){
            lds_messagebox::warning(this, MESSAGE_TITLE_VOID,tr("取款金额超过保本外可用金额"));
            return;
        }
    }

    if(ls_memberno.trimmed().isEmpty()){
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID,tr("卡号不存在，请先读卡或查卡"));
        return;
    }
    if(!_isready){
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID,tr("当前取款记录已取款,请重新读卡或查卡"));
        return;
    } else {
        if(1==lds_messagebox::question(this, MESSAGE_TITLE_VOID,tr("确定数据正确无误，是否取款"), tr("确认"), tr("取消"))){
            ls_err=tr("手动退出");
            return;
        }
    }
    //! <--transaction begin-->
    lds_query::tran_saction();
    switch(0){
    case 0:
        if(!(ls_err=n_func::f_m_deposit(this, ls_memberno,ldec_deposit,ldec_realamount,ldec_amount,ls_deposit_mode,ls_pay_mode,tr("会员取款"), _parm)).isEmpty()){
            break;
        }
        if(w_m_member_list::is_yun_member) break;///=若是云会员付款,或退卡则到此结束
    case 1:
        if(!(ls_err=n_func::f_m_insert_log(ls_memberno,tr("取款"),tr("取款金额:") + QString().sprintf("%0.2f", qAbs(ldec_deposit)), 0)).isEmpty()){
            break;
        }
    }

    if(ls_err.isEmpty()){
        ui->lineEdit_amount->setValue(ldec_amount);
        _isready=false;
        lds_query::com_mit();
    } else {
        lds_query::roll_back();
    }
    //! <--transaction end-->
    if(_tablemodel)_tablemodel->select();
    if(_tableview)_tableview->restore();

    if(ls_err.isEmpty()) {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID,tr("取款成功"));
        rechargeprint();
        card_info_clear();
    } else {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID,tr("数据库错误"));
    }
}

void w_m_member_list_recharge::ue_blankcard()
{
    //ue_back_card (none) returns (none)
    QString ls_err;
    QString ls_memberno;
    QString ls_memo;
    uo_rw_member uo_member;
    n_func::st_member_type  lst_member;
    QDateTime ldt;
    QDateTime ldt_old;
    double ldec_remain;
    double ldec_deposit;
    double ldec_amount;
    QString ls_value;
    double ldec_base;
    double ldec_point=0;
    QString ls_deposit_mode="5";

    uo_member.of_init(this, "4");
    ls_memberno=ui->lineEdit_vch_memberno->text();
    ldec_deposit=0-ui->lineEdit_num_deposit->value();
    ls_memo = ui->lineEdit_vch_memo->text();
    ldec_remain = ui->lineEdit_remain->value();
    //    if(_tablemodel)_tablemodel->record(ll_row, "remain").toDouble();
    ldec_amount = ldec_deposit + ldec_remain;

    if(ls_memberno.trimmed().isEmpty()){
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID,tr("请先读卡/查卡,再充值"));
        return;
    }
    if(ldec_deposit>=0){
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID,tr("退款金额不能为非正数"));
        return;
    }
    lst_member = n_func::f_m_get_limit(this, ls_memberno);
    //2
    if(lst_member.as_m_state == "4"){
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID,tr("此会员卡已作废，操作中止"));
        return;
    }
    if(lst_member.as_m_state == "3"){
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID,tr("此会员卡已挂失，无法退卡"));
        return;
    }
    if(lst_member.as_m_state == "2"){
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID,tr("止付卡不能退卡，只能启用"));
        return;
    }

    if(lst_member.as_m_cardflag != "Y"){
        ls_value = n_func::f_m_get_parameter(this,"vip_card_type",tr("无会员卡"));
        if(ls_value == tr("IC卡") || ls_value == tr("磁卡")){
            lds_messagebox::warning(this, MESSAGE_TITLE_VOID,tr("会员卡%1,还未发卡,不能退卡").arg(ls_memberno));
            return;
        }
    }
    ldt = n_func::f_get_sysdatetime();
    ldt_old = lst_member.dt_operdate;

    if(ldt_old.isValid() && ldt_old > ldt){
        if(1 == lds_messagebox::question(this, MESSAGE_TITLE_VOID,tr("系统时间小于上次保存时间，请确认系统时间是否正确"), tr("继续退卡"), tr("取消"))){
            return;
        }
    }
    //3
    if(n_func::f_m_get_parameter(this,"m_fetch_base", "0")=="1"){
        if(ldec_amount < 0){
            lds_messagebox::warning(this, MESSAGE_TITLE_VOID,tr("退款金额超过可用金额"));
            return;
        }
        if(ldec_amount>0){
            lds_messagebox::warning(this, MESSAGE_TITLE_VOID,tr("卡余额(包含保本金额,保本金额可取)不为零,不能退卡,~r~n可进行取款操作，待余额为零再退卡"));
            return;
        }
    }else {
        if((ldec_amount - lst_member.adec_m_limitdata) < 0){
            lds_messagebox::warning(this, MESSAGE_TITLE_VOID,tr("退款金额超过保本外可用金额"));
            return;
        }
        if((ldec_amount - lst_member.adec_m_limitdata) > 0){
            lds_messagebox::warning(this, MESSAGE_TITLE_VOID,tr("卡余额(不包含保本金额,保本金额不可取)不为零,不能退卡,~r~n可进行取款操作，待余额为零再退卡"));
            return;
        }
        ldec_base = lst_member.adec_m_limitdata;
    }



    lds_query query;
    //! <--transaction begin-->
    lds_query::tran_saction();
    switch(0){
    case 0:

        query.execSelect(QString("select num_point from t_m_curamount  where vch_memberno = '%1' ")
                         .arg(ls_memberno));
        query.next();
        ldec_point=query.recordValue("num_point").toDouble();

        if(ldec_point > 0) {
            if(1==lds_messagebox::question(this, MESSAGE_TITLE_VOID,tr("卡积分不为零,是要执相应的积分操作,还是系统自动冲减?点YSE按系统自动冲减,点NO自己执行相关操作"), tr("确认"), tr("取消"))){
                lds_messagebox::warning(this, MESSAGE_TITLE_VOID,tr("自己执行积分操作,取消退卡"));
                return;
            }
        }

        if(!lst_card.flag){
            ls_err=lst_card.errtext;
            break;
        }

        if(lst_card.amount>0){
            ls_err=n_func::f_m_deposit(this, ls_memberno,ldec_deposit,ldec_deposit,0,ls_deposit_mode,"6",tr("退卡还现"), _parm);
            if(!ls_err.isEmpty()){
                ls_err=tr("读取会员帐务数据失败，操作终止");
                break;
            }
        }
        if(w_m_member_list::is_yun_member) break;///=若是云会员付款,或退卡则到此结束
    case 1:
        if(!query.execUpdate("t_m_member",qrtVariantPairList()

                             << qrtVariantPair("ch_cardflag", "N")
                             << qrtVariantPair("ch_state", "1")
                             << qrtVariantPair("int_basedata", 0),
                             QString(" vch_memberno = '%1' ").arg(ls_memberno))) {
            ls_err=tr("修改会员发卡标志失败");
            break;
        }

        if(!query.execUpdate("t_m_curamount",qrtVariantPairList()
                             << qrtVariantPair("num_amount", 0)
                             << qrtVariantPair("num_point", 0),
                             QString(" vch_memberno = '%1' ").arg(ls_memberno))) {
            ls_err=tr("会员余额恢复");
            break;
        }

        ls_err=n_func::f_m_insert_log(lst_card.cardid,tr("退卡"),tr("金额:") +QString().sprintf("%0.2f",qAbs(ldec_deposit)),ldec_base);
        if(!ls_err.isEmpty()){
            ls_err=tr("读取会员帐务数据失败，操作终止");
            break;
        }

        //            IF  NOT (uo_member.of_clear_card(f_m_get_parameter(this,"vip_ic_password",""))) THEN //58
        ls_err.clear();
        if(uo_member.is_cardtype == "1"){
            QString icpasswd=n_func::f_m_get_parameter(this,"vip_ic_password");
            QByteArray transferpasswd = n_func::f_m_get_cardpwdtransfer(icpasswd);
#ifdef Q_OS_WIN
            ReadCard_mwrf32 reader;reader.setDefPwd(n_func::f_m_defcardpwd);//需要设置一次默认密码，应为changepwd会不完全清卡
#else
            Reader_RF reader;
#endif
            if(reader.setDefPwd(transferpasswd)){
                if(reader.writecard("","","")){
                    if(reader.pwdChange(transferpasswd, n_func::f_m_defcardpwd)){
                        //清除成功
                        break;
                    }
                }
            }
            ls_err=reader.lastError();
        }
    }
    if(ls_err.isEmpty()){
        lds_query::com_mit();
        ls_err=tr("退卡成功");
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID,ls_err);
        rechargeprint();
        card_info_clear();
    } else {
        lds_query::roll_back();
        ls_err=tr("退卡失败");
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID,ls_err);
    }
    //! <--transaction end-->
}

void w_m_member_list_recharge::rechargeprint()
{
    //打印内容
    //title 会员卡余额
    // 会员卡号
    //会员姓名
    //会员类型
    //会员余额
    //操作员
    //操作日期

    if(ui->checkBox_2->isChecked()) {//钱箱
        QString path  = lds::sysconf->value("system_setting/printerpath").toString();
        blockIns ins;
        ins += pieceIns(lds::sysconf->value("system_setting/qianxiangzhiling").toByteArray(), "");
        backheader::printer.synchPrint(ins, path);
    }

    if(ui->checkBox->isChecked()) {
        if(ui->lineEdit_vch_memberno->text().isEmpty()){
            lds_messagebox::warning(this, MESSAGE_TITLE_VOID,tr("会员编号为空，请先读卡"));
        } else {

            n_func::READCARDINFO cardinfo;
            {
                lds_messagebox_loading_show loading(this, MESSAGE_TITLE_VOID, "LOADING");
                loading.show_delay();
                if(!n_func::qtcardinfo_readcard(cardinfo, ui->lineEdit_vch_memberno->text() )) {
                    lds_messagebox::warning(this, MESSAGE_TITLE_VOID, cardinfo.errstring);
                    return;
                }
            }

            lds_query query;
            blockIns printdata;
            printdata += backheader::printer.drawText("i,a00,s11", "");
            query.execSelect("select vch_company from cey_sys_company limit 1;");
            if(query.next()) {
                printdata += backheader::printer.drawText("", query.recordValue("vch_company").toString()+"\n");//清除缓存 居中 2x2x
            }
            printdata += backheader::printer.drawText("", this->windowTitle() + (w_m_member_list::is_yun_member? ("(" +tr("云会员卡") + ")") : "")+  "\n");//清除缓存 居中 2x2x
            printdata += backheader::printer.drawText("i", "");//清除缓存

            printdata += backheader::printer.drawText("", ui->label_vch_memberno_t->text()+":"+cardinfo.vch_memberno + "\n");//
            printdata += backheader::printer.drawText("", ui->label_vch_member_t->text()+":"+cardinfo.vch_member+ "\n");//
            printdata += backheader::printer.drawText("", tr("会员类型")+":"+cardinfo.vch_typename + "\n");//
            printdata += backheader::printer.drawText("", ui->label_num_realamount_t->text()+":"+lds::septNumber(ui->lineEdit_num_realamount->value())+ "\n");//
            printdata += backheader::printer.drawText("", ui->label_num_deposit_t->text()+":"+ lds::septNumber(ui->lineEdit_num_deposit->value())+ "\n");//
            printdata += backheader::printer.drawText("", ui->label_remain_t->text() + ":" + lds::septNumber(cardinfo.num_amount)+ "\n");//
            printdata += backheader::printer.drawText("", ui->label_ch_pay_mode_t->text()+":"+ui->comboBox_ch_pay_mode->currentText()+ "\n");//
            printdata += backheader::printer.drawText("", tr("操作员")+":"+public_sql::gs_opername + "\n");//
            printdata += backheader::printer.drawText("", tr("操作日期")+":"+n_func::f_get_sysdatetime().toString("yyyy-MM-dd hh:mm:ss") + "\n");

            printdata += backheader::printer.drawTextEnd("1B6405 1D5601", "");//走纸5x， 切纸
            printdata =   backheader::printer.setPrintCount(printdata, 1);

            backheader::asyn_dataprint(printdata);
        }
    }
}

void w_m_member_list_recharge::toremainprint()
{
    //打印内容
    //title 会员卡余额
    // 会员卡号
    //会员姓名
    //会员类型
    //会员余额
    //操作员
    //操作日期


    if(ui->lineEdit_vch_memberno->text().isEmpty()){
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID,tr("会员编号为空，请先读卡"));
    } else {

        n_func::READCARDINFO cardinfo;
        {
            lds_messagebox_loading_show loading(this, MESSAGE_TITLE_VOID, "LOADING");
            loading.show_delay();
            if(!n_func::qtcardinfo_readcard(cardinfo, ui->lineEdit_vch_memberno->text() )) {
                lds_messagebox::warning(this, MESSAGE_TITLE_VOID, cardinfo.errstring);
                return;
            }
        }

        lds_query query;
        blockIns printdata;
        printdata += backheader::printer.drawText("i,a00,s11", "");
        query.execSelect("select vch_company from cey_sys_company limit 1;");
        if(query.next()) {
            printdata += backheader::printer.drawText("", query.recordValue("vch_company").toString()+"\n");//清除缓存 居中 2x2x
        }
        printdata += backheader::printer.drawText("", this->windowTitle() + (w_m_member_list::is_yun_member? ("(" +tr("云会员卡") + ")") : "")+  "\n");//清除缓存 居中 2x2x
        printdata += backheader::printer.drawText("i", "");//清除缓存

        printdata += backheader::printer.drawText("", ui->label_vch_memberno_t->text()+":"+cardinfo.vch_memberno + "\n");//
        printdata += backheader::printer.drawText("", ui->label_vch_member_t->text()+":"+cardinfo.vch_member+ "\n");//
        printdata += backheader::printer.drawText("", tr("会员类型")+":"+cardinfo.vch_typename + "\n");//
        printdata += backheader::printer.drawText("", ui->label_remain_t->text() + ":" + lds::septNumber(cardinfo.num_amount)+ "\n");//
        printdata += backheader::printer.drawText("", tr("操作员")+":"+public_sql::gs_opername + "\n");//
        printdata += backheader::printer.drawText("", tr("操作日期")+":"+n_func::f_get_sysdatetime().toString("yyyy-MM-dd hh:mm:ss") + "\n");

        printdata += backheader::printer.drawTextEnd("1B6405 1D5601", "");//走纸5x， 切纸
        printdata =   backheader::printer.setPrintCount(printdata, 1);

        backheader::asyn_dataprint(printdata);
    }
}

void w_m_member_list_recharge::toreadcard()
{
    n_func::st_member_type lst_member;
    QString cardno;
    QString as_memberno;
    uo_rw_member uo_member;
    QString ls_value;
    _isready=false;
    bool isyun=_parm.isEmpty()//充值
            ||_parm=="D"//退卡
            ||_parm=="C"//取款
            ;
    card_info_clear();
    //读卡
    if(!w_m_member_list::static_to_memeber_readcard(this, cardno, uo_member, _filter_by_handtel, isyun)){
        goto read_card_fail;
    }
    as_memberno=cardno;
    if(as_memberno.isEmpty()){
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID,tr("对应会员号为空"));
        goto read_card_fail ;
    }

    lst_card=uo_member.of_read_card(this, as_memberno);
    if(lst_card.flag == false){
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID,lst_card.errtext);
        goto read_card_fail;
    }
    if(lst_card.flag) {
        lst_member=n_func::f_m_get_limit(this, lst_card.cardid);
        if(lst_member.as_m_state.isEmpty()){
            lds_messagebox::warning(this, MESSAGE_TITLE_VOID,tr("非本公司卡或无效卡")+lst_card.cardid);
            goto read_card_fail;
        } else if(lst_member.as_m_state!="1"){
            if(this->_parm!="D"){
                lds_messagebox::warning(this, MESSAGE_TITLE_VOID,tr("此卡状态非正常,不能充值"));
                goto read_card_fail;
            }
        }

        if(lst_member.adt_m_limitdate <= n_func::f_get_sysdatetime()){
            lds_messagebox::warning(this, MESSAGE_TITLE_VOID,tr("此卡已经超过有效期"));
            goto read_card_fail;
        }

        if(this->_parm!="D"){
            if(lst_member.as_m_att!="2"){
                lds_messagebox::warning(this, MESSAGE_TITLE_VOID,tr("非充值卡,不能充值"));
                goto read_card_fail;
            }
        }

        if(lst_member.as_m_cardflag !="Y"){
            ls_value=n_func::f_m_get_parameter(this,"vip_card_type", tr("无会员卡"));
            if(ls_value==tr("IC卡") || ls_value==tr("磁卡")){
                if(this->_parm=="C"){
                    lds_messagebox::warning(this, MESSAGE_TITLE_VOID,tr("会员卡")+as_memberno+tr(",还未发卡,不能取款"));
                    goto read_card_fail;
                } else {
                    lds_messagebox::warning(this, MESSAGE_TITLE_VOID,tr("会员卡")+cardno+tr(",还未发卡,不能充值"));
                    goto read_card_fail;
                }
            }
        }
    } else {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID,lst_card.errtext);
        goto read_card_fail;
    }

read_card_success:
    //==
    ui->lineEdit_vch_memberno->setText(lst_card.cardid);
    ui->lineEdit_vch_member->setText(lst_member.as_member);
    ui->lineEdit_num_realamount->setValue(0);
    ui->lineEdit_num_deposit->setValue(0);
    ui->lineEdit_remain->setValue(lst_card.amount);
    ui->lineEdit_amount->setValue(0);
    ui->lineEdit_vch_operid->setText(public_sql::gs_operid);
    ui->lineEdit_operdate->setText(n_func::f_get_sysdatetime().toString("yyyy-MM-dd hh:mm"));
    _isready=true;
    if(_isready)TtolineEdit_amount();

    if(this->_parm=="C"){//取款
        ui->lineEdit_num_deposit->setFocus();
        ui->lineEdit_num_deposit->setValue(ui->lineEdit_remain->value());
    } else   if(this->_parm=="D"){//退卡
        ui->lineEdit_num_deposit->setFocus();
    } else {
        ui->lineEdit_num_realamount->setFocus();
    }
    return;
read_card_fail:
    w_m_member_list::yun_member_close();
    return;
}

void w_m_member_list_recharge::torecharge()
{
    if(this->_parm=="C"){//取款
        ue_fetch();
        return;
    }
    if(this->_parm=="D"){//退卡
        ue_blankcard();
        return;
    }
    QString ls_memberno=ui->lineEdit_vch_memberno->text();
    double ldec_deposit=ui->lineEdit_num_deposit->value();
    double ldec_realamount=ui->lineEdit_num_realamount->value();
    double ldec_remaint=ui->lineEdit_remain->value();
    double ldec_amount=ldec_deposit+ldec_remaint;
    QString ls_error;
    QString ls_deposit_mode="1";
    QString ls_pay_mode=ui->comboBox_ch_pay_mode->curusrdata().toString();//w_m_member::get_RECHARGE_MODEL_PAYMODE().item(ui->comboBox_ch_pay_mode->currentIndex(), 0)->text();
    QString ls_memo=tr("会员充值");
    //100，000
    double ldec_max_deposit=n_func::f_m_get_parameter(this,"m_max_deposit", "999999").remove(",").toDouble();

    switch(0) {
    case 0:
        if(ls_memberno.trimmed().isEmpty()){
            ls_error=(tr("请先读卡/查卡,再充值"));
            break;
        }
        if(ldec_deposit < 0 || ldec_realamount < 0){
            ls_error=(tr("充值金额与实收金额不能为负数"));
            break;
        }
        if(ldec_deposit>ldec_max_deposit){
            ls_error=(tr("充值金额不能大于最大充值金额，你可已在会员参数设置中更改该值"));
            break;
        }
        if(n_func::f_m_get_parameter(this,"m_deposit_negative", "0")=="1" && ldec_deposit != ldec_realamount){
            ls_error=(tr("充值金额与实收金额不等,不能充值,你可以在会员参数设置中修改此项"));
            break;
        }

        if (ldec_deposit < 0 && ldec_realamount != ldec_deposit){
            ls_error=(tr("负数充值时，充值金额与实收金额必须相等"));
            break;
        }
        if(ldec_deposit == 0 && ldec_realamount == 0){
            ls_error=tr("充值金额与实收金额不能全部为0，不能充值");
            break;
        }
        if((ldec_deposit < 0 && ldec_realamount >= 0) || (ldec_deposit >= 0 && ldec_realamount < 0)){
            ls_error=(tr("充值金额与实收金额正负不一致，不能充值"));
            break;
        }

        if(!_isready){
            ls_error=(tr("当前充值记录已充值,请重新读卡或查卡"));
            break;
        } else {
            if(1==lds_messagebox::question(this, MESSAGE_TITLE_VOID,tr("确定数据正确无误，是否充值"), tr("确认"), tr("取消"))){
                ls_error=tr("手动退出");
                break;
            }
        }
    case 1:
        //! <--transaction begin-->
        /*选中支付宝或者微信充值的事物*/
    {
        if("6" == ls_pay_mode) {
            if(false == w_scr_dish_phone_pay::isPayRecord("05")) {//非记账
                w_scr_dish_phone_pay dialog("05", ui->lineEdit_num_realamount->value(), this);
                if(QDialog::Accepted == lds_roundeddialog_rect_align(&dialog).exec()) {
                    ls_memo = OnlinePayTypeDesc(dialog.ret_pay_type) + ":" + dialog.ret_out_trade_no;
                } else {
                    ls_error = ui->comboBox_ch_pay_mode->currentText() + tr("取消");
                }
            }
        }
        if("7" == ls_pay_mode) {
            if(false == w_scr_dish_phone_pay::isPayRecord("06")) {//非记账
                w_scr_dish_phone_pay dialog("06", ui->lineEdit_num_realamount->value(), this);
                if(QDialog::Accepted == lds_roundeddialog_rect_align(&dialog).exec()) {
                    ls_memo = OnlinePayTypeDesc(dialog.ret_pay_type) + ":" + dialog.ret_out_trade_no;
                } else {
                    ls_error = ui->comboBox_ch_pay_mode->currentText() + tr("取消");
                }
            }
        }
        if("8" == ls_pay_mode) {
            if(false == w_scr_dish_phone_pay::isPayRecord("AA")) {//非记账
                w_scr_dish_phone_pay dialog("AA", ui->lineEdit_num_realamount->value(), this);
                if(QDialog::Accepted == lds_roundeddialog_rect_align(&dialog).exec()) {
                    ls_memo = tr("移动支付") + "_" + OnlinePayTypeDesc(dialog.ret_pay_type) + ":" + dialog.ret_out_trade_no;
                } else {
                    ls_error = ui->comboBox_ch_pay_mode->currentText() + tr("取消");
                }
            }
        }
    }

        lds_query::tran_saction();
        if((ls_error=n_func::f_m_deposit(this, ls_memberno,ldec_deposit/*充值金额*/,
                                         ldec_realamount/*实际充值金额*/,
                                         ldec_amount/*充值金额后剩余金额*/,ls_deposit_mode,ls_pay_mode,ls_memo,_parm)).isEmpty()){
            if(n_func::f_m_get_parameter(this,"vip_card_type", tr("IC卡"))==tr("IC卡")){

            }
        } else {
            lds_query::roll_back();
            break;
        }

        if(!(ls_error=n_func::f_m_insert_log(ls_memberno,tr("充值"),tr("充值金额:") + QString::number(ldec_deposit),0)).isEmpty()){
            lds_query::roll_back();
            break;
        }

        lds_query::com_mit();
        if(_tablemodel)_tablemodel->select();
        if(_tableview)_tableview->restore();
        //! <--transaction end-->
        break;
    }

    _isready=false;
    if(ls_error.isEmpty()){
        lds_messagebox::information(this, MESSAGE_TITLE_VOID,tr("充值成功"));
        rechargeprint();
        card_info_clear();
    } else {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID,ls_error);
    }

}

void w_m_member_list_recharge::torechargeOrRead()
{
    QString ls_memberno=ui->lineEdit_vch_memberno->text();
    if(ls_memberno.trimmed().length() == 0) {
        toreadcard();
    } else {
        torecharge();
    }
}

void w_m_member_list_recharge::toexit()
{
    //yun
    w_sys_manage_cloudsync_with_time_running::exec_no_up_then_down_byconf(this);
    ui->pushButton_exit->show_flag_top_right(w_sys_manage_cloudsync_with_time_running::q_bt_get_ready_total_byconf());
    //~yun
    w_sys_manage_cloudsync_with_time_running::exec_keepdata_byconf(this);

    this->reject();
}

void w_m_member_list_recharge::TtolineEdit_amount()
{
    if(_parm=="C"){//取款
        ui->lineEdit_amount->setValue(ui->lineEdit_remain->value() - ui->lineEdit_num_deposit->value());
    } else if(_parm=="D"){//退卡
        ui->lineEdit_amount->setValue(ui->lineEdit_remain->value() - ui->lineEdit_num_deposit->value());
    } else {//充值
        if(n_func::f_m_get_parameter(this,"m_deposit_negative", "0")=="1"){
            ui->lineEdit_num_deposit->setValue(ui->lineEdit_num_realamount->value());
        }
        ui->lineEdit_amount->setValue(ui->lineEdit_num_deposit->value() + ui->lineEdit_remain->value());
    }
    //    if(_parm!="C"){
    //        if(n_func::f_m_get_parameter(this,"m_deposit_negative", "0")=="1"){
    //            ui->lineEdit_num_deposit->setText(ui->lineEdit_num_realamount->text());
    //        }
    //        ui->lineEdit_amount->setText(QString().sprintf("%0.2f", (ui->lineEdit_num_deposit->text().toFloat() + ui->lineEdit_remain->text().toFloat())));

    //    } else {
    //        ui->lineEdit_amount->setText(QString().sprintf("%0.2f", (ui->lineEdit_remain->text().toFloat() - ui->lineEdit_num_deposit->text().toFloat())));

    //    }
}

void w_m_member_list_recharge::tochecksave()
{
    //更新配置文件

    lds::sysconf->setValue("w_m_member_list_recharge/isprint", ui->checkBox->isChecked());
}

void w_m_member_list_recharge::tochecksave_2()
{
    lds::sysconf->setValue("w_m_member_list_recharge/rechange_open_drawer", ui->checkBox->isChecked());

}

void w_m_member_list_recharge::tocheckpaymode(int index)
{
    if(index == 3 || index == 4){
        if(_parm=="C" || _parm=="D"){
            lds_messagebox::warning(this, MESSAGE_TITLE_VOID, tr("目前不支持%1支付方式").arg(ui->comboBox_ch_pay_mode->currentText()));
            ui->comboBox_ch_pay_mode->setCurrentIndex(0);
        }
    }
}

void w_m_member_list_recharge::toreplacenum()
{
    QDoubleSpinBox *l = qobject_cast<QDoubleSpinBox*>(this->focusWidget());
    QPushButton *p = qobject_cast<QPushButton *>(this->sender());
    if(l&&p) {
        l->setValue(p->text().toDouble());
        l->selectAll();
    }
}

void w_m_member_list_recharge::card_info_clear()
{
    ui->lineEdit_vch_memberno->clear();
    ui->lineEdit_vch_member->clear();
    ui->lineEdit_remain->setValue(0);
    ui->lineEdit_amount->setValue(0);
    ui->lineEdit_num_deposit->setValue(0);
    ui->lineEdit_num_realamount->setValue(0);
}

//==========================
#include "ui_w_m_member_pwdchange_dialog.h"
w_m_member_pwdchange::w_m_member_pwdchange(QWidget *parent):
    QDialog(parent),
    ui(new Ui_w_m_member_pwdchange_Dialog),
    _parent(static_cast<w_m_member_list* >(parent))

{
    ui->setupUi(this);
    //! toolbar
    ui->widget->setup(QStringList()<<tr("确定")<<tr("取消"),Qt::AlignRight, lds_keytoolbutton::SYSKEY);
    connect(ui->widget->index_widget(tr("确定")), SIGNAL(clicked()),this,SLOT(took()));
    connect(ui->widget->index_widget(tr("取消")), SIGNAL(clicked()),this,SLOT(tocancel()));

    ui->label_4->hide();
    ui->lineEdit_cur_operator->hide();
    ui->label_5->hide();
    ui->lineEdit_login_operator->hide();
}

void w_m_member_pwdchange::took()
{
    QString ls_password_old;
    QString ls_password;
    QString ls_password1;
    QString ls_err;


    ls_password_old = ui->lineEdit_oldpwd->text();
    ls_password = ui->lineEdit_newpwd->text();
    ls_password1 = ui->lineEdit_newpwd2->text();
    if(is_password.isEmpty()){

    } else {
        if(ls_password_old==is_password){

        } else {
            lds_messagebox::warning(this, MESSAGE_TITLE_VOID,tr("旧密码录入错误，不能修改密码"));
            return;
        }
    }
    if(ls_password==ls_password1){

    } else {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID,tr("新密码与确认密码不一样，修改密码中止"));
        return;
    }

    if(0==lds_messagebox::question(this, MESSAGE_TITLE_VOID,tr("确定按设置修改密码"),tr("确认"),tr("取消"))){
        //! <--transaction begin-->
        lds_query::tran_saction();
        lds_query query;

        if(query.execUpdate("t_m_member",qrtVariantPairList()

                            << qrtVariantPair("vch_password", ls_password),
                            QString(" vch_memberno = '%1' ").arg(is_memberno))) {
            lds_query::com_mit();
            ls_err=tr("修改密码成功，请记住新密码");
            _parent->torefresh();
            lds_messagebox::warning(this, MESSAGE_TITLE_VOID,ls_err);
            this->accept();
        } else {
            lds_query::roll_back();
            ls_err=tr("修改密码失败");
            _parent->torefresh();
            lds_messagebox::warning(this, MESSAGE_TITLE_VOID,ls_err);

        }
        //! <--transaction end-->
    }
}

void w_m_member_pwdchange::tocancel()
{
    this->reject();
}

QString w_m_member_pwdchange::check()
{
    is_memberno=_parent->getTablemodel()->model_data(_parent->getTableview()->currentIndex().row(),"vch_memberno").toString();
    lds_query query;
    query.execSelect(QString("select ifnull(vch_password, '') from t_m_member where vch_memberno='%1'").arg(is_memberno));
    if(query.next()){
        is_password=query.recordValue(0).toString();
        if(is_password.isEmpty()){
            ui->lineEdit_oldpwd->setEnabled(false);
            ui->lineEdit_newpwd->setFocus();
        } else {
            ui->lineEdit_oldpwd->setFocus();
        }
        return "";
    } else {
        return tr("指定会员号不存在,请刷新数据,核实");
    }
}

//=======================
#include "ui_w_m_member_news_dialog.h"
w_m_member_news::w_m_member_news(w_m_member_list *parent):
    QDialog(parent)
{

    _parent = parent;
    ui = new Ui_w_m_member_news_Dialog;
    ui->setupUi(this);

    //! init
    ui->comboBox_ch_typeno->addItemsBySql("select ch_typeno, vch_typename from t_m_member_type");
    ui->dateTimeEdit_dt_limit->setDateTime(n_func::f_get_sysdatetime().addYears(1));
    ui->dateTimeEdit_dt_limit->setDisplayFormat("yyyy-MM-dd hh:mm:ss");
    //! toolbar
    ui->widget->setup(QStringList() << tr("生成") << tr("退出"), Qt::AlignRight, lds_keytoolbutton::SYSKEY);
    connect(ui->widget->index_widget(tr("生成")), SIGNAL(clicked()),this,SLOT(took()));
    connect(ui->widget->index_widget(tr("退出")), SIGNAL(clicked()),this,SLOT(tocancel()));
    connect(ui->pushButton_type, SIGNAL(clicked()), this, SLOT(tonewtype()));

}

void w_m_member_news::took()
{
    //clicked (none) returns long [pbm_bnclicked]
    QString ls_memberno;
    QString ls_membername;
    QString ls_start;
    QString ls_begin;
    QString ls_end;
    QString ls_type;
    QString ls_cardno;
    qlonglong ll_begin;
    qlonglong ll_end;
    qlonglong ll_limit;
    double ldec_limit = 0;
    QDateTime ldt_limit;
    QDateTime ldt_today;
    QRegExp ls_filter;

    //    PARENT.dw_1.accepttext()
    ls_start = ui->lineEdit_code_start->text();//trim(PARENT.dw_1.object.code_start[1])
    ll_begin = ui->lineEdit_code_begin->text().toLongLong();//PARENT.dw_1.object.code_begin[1]
    ls_begin = ui->lineEdit_code_begin->text();//string(ll_begin)
    ll_end = ui->lineEdit_code_end->text().toLongLong();//PARENT.dw_1.object.code_end[1]
    ls_end = ui->lineEdit_code_end->text();//string(ll_end)
    ll_limit = ui->lineEdit_code_limit->text().toLongLong();//PARENT.dw_1.object.code_limit[1]
    ls_type = ui->comboBox_ch_typeno->curusrdata().toString();
    ldt_limit = ui->dateTimeEdit_dt_limit->dateTime();//PARENT.dw_1.object.dt_limit[1]
    ls_filter = QRegExp("["+ui->lineEdit_vch_filter->text().replace(",","")+"]");

    if(ll_limit==0){
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID,tr("编号位数不能为零"));
        return;
    }
    if(ldt_limit < n_func::f_get_sysdatetime()){
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID,tr("会员有效期限应该大于当前日期"));
        return;
    }
    if(ldt_limit.isNull()){
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID,tr("有效期限不能为空"));
        return;
    }
    if(ui->comboBox_ch_typeno->currentText().isEmpty()){
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID,tr("会员类型不能为空"));
        return;
    }
    if(ll_limit==0 || (ll_limit+ls_start.length()) > 20){
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID,tr("编号位数不能为空且不能大于%1位")
                                .arg(QString::number(20-ls_start.length()))
                                );
        return;
    }
    if(ll_end-ll_begin >= 10000){
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID,tr("一次能批量创建的会员不超过1万条"));
        return;
    }
    if(ll_limit < ls_end.trimmed().length()){
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID,tr("编号部分位数不能小于编号到(结束)实际位数"));
        return;
    }
    if(ll_begin > ll_end){
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID,tr("编号从(起始)不能大于编号到(结束)"));
        return;
    }
    if(ls_end.isEmpty()){
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID,tr("编号到(结束)不能为空"));
        return;
    }
    if(ls_begin.isEmpty()){
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID,tr("编号从(起始)不能为空"));
        return;
    }

    if(ls_start.isEmpty()){
        if(1==lds_messagebox::question(this, MESSAGE_TITLE_VOID,tr("编号前缀为空，(前缀可以固定生成在每个会员编号前面)，你还要继续吗？"), tr("继续"),tr("取消"))){
            return;
        }
    }

    //! <--transaction begin-->
    lds_query::tran_saction();
    QString errtext;
    lds_query query;
    for(qlonglong ll_i=ll_begin; ll_i <= ll_end; ll_i++){
        ls_memberno = ls_start +QString::number(ll_i).rightJustified(ll_limit, '0').right(ll_limit);// fill("0",ll_limit - len(string(i))) + string(i)
        ls_membername = "批量" + QString::number(ll_i);
        ls_cardno=ls_memberno;
        if(ls_memberno.contains(ls_filter)){
            continue;
        }
        query.execSelect(QString("Select count(0)    from t_m_member   where vch_memberno ='%1' ")
                         .arg(ls_memberno));
        query.next();
        if(query.recordValue(0).toLongLong() > 0){
            errtext=tr("批量范围中的会员编号系统已经存在，例如:%1，本次操作无效")
                    .arg(ls_memberno);
            break;
        }
        ldt_today=n_func::f_get_sysdatetime();

        if(query.execInsert("t_m_member",qrtVariantPairList()

                            << qrtVariantPair("vch_memberno", ls_memberno)
                            << qrtVariantPair("vch_cardno", ls_cardno)
                            << qrtVariantPair("vch_member", ls_membername)
                            << qrtVariantPair("ch_typeno", ls_type)
                            << qrtVariantPair("num_limit", ldec_limit)

                            << qrtVariantPair("dt_limit", ldt_limit)
                            << qrtVariantPair("vch_operID", public_sql::gs_operid)
                            << qrtVariantPair("dt_operdate", ldt_today),
                            &errtext)) {
        } else {
            break;
        }
    }

    if(errtext.isEmpty()){
        lds_query::com_mit();
        _parent->torefresh();
        lds_messagebox::information(this, MESSAGE_TITLE_VOID,tr("批量生成会员完成"));
    } else {
        lds_query::roll_back();
        _parent->torefresh();
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID,tr("批量生成会员失败")+"\n"+errtext);
    }
    //! <--transaction end-->
}

void w_m_member_news::tocancel()
{
    this->reject();
}

void w_m_member_news::tonewtype()
{
    w_m_member_list_type dialog(this);
    dialog.setWindowTitle(_TEXT_SLOT(this));
    lds_roundeddialog_rect_align(&dialog).exec();

    QVariant typeno = ui->comboBox_ch_typeno->curusrdata();
    ui->comboBox_ch_typeno->clear();
    ui->comboBox_ch_typeno->addItemsBySql("select ch_typeno, vch_typename from t_m_member_type");
    ui->comboBox_ch_typeno->setcomusrdata(typeno);
}

w_m_member_list_yunlock::w_m_member_list_yunlock()
{
    w_m_member_list::yun_member_open();
}

w_m_member_list_yunlock::~w_m_member_list_yunlock()
{
    w_m_member_list::yun_member_close();
}
