#include "w_sys_manage_operatorright.h"
//操作员权限
#include "ui_w_sys_manage_operatorright_dialog.h"
#include <QStandardItemModel>
#include "backheader.h"
#include "lds_query.h"
#include <QSqlRecord>
#include <QSqlError>
#include "lds_messagebox.h"
#include "lds_tableview_delegate_check.h"
#include "ui_w_m_member_pwdchange_dialog.h"
#include "n_func.h"
#include "ui_w_bt_dish_unitset_dialog.h"
#include "lds_model_sqltablemodel_delegate_com.h"


w_sys_manage_operatorright::w_sys_manage_operatorright(QWidget *parent) :
    QDialog(parent),
    ui(new Ui_w_sys_manage_operatorright_Dialog)
{
    ui->setupUi(this);
    //! tableview
    querymodel=new QSqlQueryModel;
    standmodel=new QStandardItemModel;

    ui->tableView->setEditTriggers(QTableView::AllEditTriggers);ui->tableView->setModel(querymodel);ui->tableView->horizontalHeader()->setStretchLastSection(true);
    ui->tableView_2->setEditTriggers(QTableView::AllEditTriggers);ui->tableView_2->setModel(standmodel);

    lds_tableview_delegate_check *degate=new lds_tableview_delegate_check(QVector<int>()
                                                                          << 2, ui->tableView);
    lds_tableview_delegate_check *degate_2=new lds_tableview_delegate_check(QVector<int>()
                                                                            << 2, ui->tableView_2);
    degate->setClickable();
    ui->tableView->setItemDelegate(degate);
    degate_2->setClickable();
    ui->tableView_2->setItemDelegate(degate_2);
    //! toolbar
    ui->widget->setup(QStringList() << tr("全选") << tr("全不选") << "|"<<tr("菜品权限")<<tr("权限复制")<< "|" << tr("修改密码") << tr("保存") <<tr("刷新") << tr("退出"));
    connect(ui->widget->index_widget(tr("全选")), SIGNAL(clicked()),this,SLOT(toSelectAll()));
    connect(ui->widget->index_widget(tr("全不选")), SIGNAL(clicked()),this,SLOT(toselectno()));
    connect(ui->widget->index_widget(tr("菜品权限")), SIGNAL(clicked()),this,SLOT(todishright()));
    connect(ui->widget->index_widget(tr("权限复制")), SIGNAL(clicked()),this,SLOT(torightcopy()));
    connect(ui->widget->index_widget(tr("修改密码")), SIGNAL(clicked()),this,SLOT(topwdchange()));
    connect(ui->widget->index_widget(tr("保存")), SIGNAL(clicked()),this,SLOT(tosave()));
    connect(ui->widget->index_widget(tr("刷新")), SIGNAL(clicked()),this,SLOT(torefresh()));
    connect(ui->widget->index_widget(tr("退出")), SIGNAL(clicked()),this,SLOT(toexit()));

    connect(ui->tableView,SIGNAL(selectChanged(int)),this,SLOT(torightmodelupdate()));
    connect(ui->lineEdit_filter, SIGNAL(textChanged(QString)),this,SLOT(torightfind(QString)));
    ui->tableView->selectRow(0);
    //!refresh
    QTimer::singleShot(10, this ,SLOT(torefresh()));
}

void w_sys_manage_operatorright::torefresh()
{
    QString sql = "(select vch_operID, vch_operator, ch_flag from cey_sys_operator)";
    //    if(public_sql::gs_operid == "0000"){
    //        sql =  "select '0000' as vch_operID,'system' as vch_operator,'Y' as ch_flag union " + sql;
    //    }
    querymodel->setQuery(sql);
    backheader::modelSetHeader(querymodel, QStringList() << tr("代码") << tr("姓名") << tr("是否生效"));
    ui->tableView->restore(0);
    torightmodelupdate();
    ui->tableView_2->restore();
}

void w_sys_manage_operatorright::toSelectAll()
{
    int row=ui->tableView->currentIndex().row();
    if(row >= 0){
        for(int r=0,r_count=standmodel->rowCount(); r<r_count; r++){//0
            standmodel->item(r,2)->setText("Y");
        }//0
    }
}

void w_sys_manage_operatorright::toselectno()
{
    int row=ui->tableView->currentIndex().row();
    if(row >= 0){
        for(int r=0,r_count=standmodel->rowCount(); r<r_count; r++){//0
            standmodel->item(r,2)->setText("N");
        }//0
    }
}

void w_sys_manage_operatorright::todishright()
{
    int row=ui->tableView->currentIndex().row();
    if(row < 0) {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, tr("没有数据被选中"));
        return;
    }
    QString vch_operid = querymodel->record(row).value("vch_operID").toString();
    w_sys_manage_operatorright_dish_type_right dialog(vch_operid, this);
    lds_roundeddialog_rect_align(&dialog).exec();
}

void w_sys_manage_operatorright::torightcopy()
{
    w_sys_manage_operatorright_rightcopy dialog(this);
    dialog.setWindowTitle(_TEXT_SLOT(this));
    lds_roundeddialog_rect_align(&dialog).exec();
}

void w_sys_manage_operatorright::topwdchange()
{
    QString cur_operID;
    if(public_sql::gs_operid == "0000"
            && 0 == lds_messagebox::question(this, MESSAGE_TITLE_VOID, tr("修改密码"), tr("管理员账号"), tr("其他账号"))){
        cur_operID = public_sql::gs_operid;
    } else {
        int row=ui->tableView->currentIndex().row();
        if(row < 0) {
            lds_messagebox::warning(this, MESSAGE_TITLE_VOID, tr("没有选中行"));
            return;
        }
        cur_operID = querymodel->record(row).value("vch_operID").toString();
    }
    w_sys_manage_operatorright_pwdchange dialog(this);
    //默认选择是登录账号
    dialog.ui->lineEdit_cur_operator->setText(public_sql::gs_operid);
    dialog.ui->lineEdit_login_operator->setText(public_sql::gs_operid);
    //有选中 则修改账号
    dialog.ui->lineEdit_cur_operator->setText(cur_operID);
    //如果admin 来修改非admin 密码， 则旧密码不需要填写
    if(public_sql::gs_operid=="0000" && dialog.ui->lineEdit_cur_operator->text() !="0000") {
        dialog.ui->lineEdit_oldpwd->setEnabled(false);
    }
    dialog.ui->lineEdit_login_operator->setEnabled(false);
    dialog.setWindowTitle(_TEXT_SLOT(this));
    lds_roundeddialog_rect_align(&dialog).exec();
}

void w_sys_manage_operatorright::tosave()
{
    int row=ui->tableView->currentIndex().row();
    QString errsql;
    lds_query query;
    //! <--transaction begin-->
    lds_query::tran_saction();
    if(row >= 0){
        QString ls_operid=querymodel->record(ui->tableView->currentIndex().row()).value("vch_operID").toString();
        if(query.execDelete("cey_sys_oper_purview", QString("vch_operID = '%1'")
                            .arg(ls_operid)
                            )){
            for(int r=0,r_count=standmodel->rowCount(); r<r_count; r++){//0
                if(standmodel->item(r,2)->text().toUpper() =="Y"){
                    if(query.execInsert("cey_sys_oper_purview", qrtVariantPairList()
                                        << qrtVariantPair("vch_operID", ls_operid)
                                        << qrtVariantPair("ch_purviewno", standmodel->item(r, 3)->text()))){
                        //
                        continue;
                    }
                    errsql=query.recordError();
                    break;
                }
            }//0
        } else {
            errsql=query.recordError();
        }
    }

    if(errsql.isEmpty()){
        lds_query::com_mit();
        torefresh();
        lds_messagebox::information(this, MESSAGE_TITLE_VOID,tr("保存成功"));
    } else {
        
        lds_query::roll_back();
        torefresh();
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID,tr("数据库错误"));
    }
}

void w_sys_manage_operatorright::toexit()
{
    this->reject();
}

void w_sys_manage_operatorright::torightmodelupdate()
{
    standmodel->removeRows(0, standmodel->rowCount());

    lds_query query;
    query.execSelect(QString("select  vch_purview_typename ,vch_purviewname , (case when ch_purviewno in(select ch_purviewno from cey_sys_oper_purview where vch_operid = '%1') then 'Y' else 'N' end)  , ch_purviewno FROM cey_sys_purview_list order by vch_purview_typename asc")
                     .arg(querymodel->record(ui->tableView->currentIndex().row()).value("vch_operID").toString())
                     );
    while(query.next()){
        QList<QStandardItem *> items;
        items << new QStandardItem(query.recordValue(0).toString());items.last()->setFlags(Qt::ItemIsEnabled|Qt::ItemIsSelectable);
        items << new QStandardItem(query.recordValue(1).toString());items.last()->setFlags(Qt::ItemIsEnabled|Qt::ItemIsSelectable);
        items << new QStandardItem(query.recordValue(2).toString());
        items << new QStandardItem(query.recordValue(3).toString());items.last()->setFlags(Qt::ItemIsEnabled|Qt::ItemIsSelectable);
        standmodel->appendRow(items);
    }
    backheader::modelSetHeader(standmodel, QStringList() << tr("模块类型") << tr("模块名称") << tr("权限") << tr("模块编号"));
    if(standmodel->rowCount() > 0){
        ui->tableView_2->resizeColumnsToContents();
    }
}

void w_sys_manage_operatorright::torightfind(const QString &text)
{
    for(int row = 0, rowcount = standmodel->rowCount(); row< rowcount; row ++) {
        if(standmodel->item(row, 1)->text().contains(text)){
            ui->tableView_2->selectRow(row);
            break;
        }
        if(standmodel->item(row, 3)->text().contains(text)){
            ui->tableView_2->selectRow(row);
            break;
        }
    }
}

//========================================
#include "ui_w_sys_manage_operatorright_rightcopy_dialog.h"
w_sys_manage_operatorright_rightcopy::w_sys_manage_operatorright_rightcopy(w_sys_manage_operatorright *parent):
    QDialog(parent),
    ui(new Ui_w_sys_manage_operatorright_rightcopy_Dialog),
    _parent(parent)
{
    ui->setupUi(this);
    //! tableview
    querymodel=new report_querymodel;
    QString vch_operator;
    if(_parent->ui->tableView->currentIndex().row()>=0){
        vch_operidfrom=_parent->querymodel->record(_parent->ui->tableView->currentIndex().row()).value("vch_operID").toString();
        vch_operator=_parent->querymodel->record(_parent->ui->tableView->currentIndex().row()).value("vch_operator").toString();
    }

    querymodel->setQuery(QString("select vch_operID, vch_operator  from cey_sys_operator where vch_operID <> '%1' ")
                         .arg(vch_operidfrom)
                         );
    ui->tableView->setEditTriggers(QTableView::AllEditTriggers);
    ui->tableView->setTransferHheader();
    ui->tableView->setModel(querymodel);
    ui->label_2->setText(vch_operator+"("+vch_operidfrom+")");

    //! toolbar
    ui->widget->setup(QStringList() << tr("权限copy") << tr("退出"), Qt::AlignRight);
    connect(ui->widget->index_widget(tr("权限copy")), SIGNAL(clicked()),this,SLOT(tocopy()));
    connect(ui->widget->index_widget(tr("退出")), SIGNAL(clicked()),this,SLOT(toexit()));

}

void w_sys_manage_operatorright_rightcopy::tocopy()
{
    if(ui->tableView->currentIndex().row()>=0){
        QString vch_operidto=querymodel->record(ui->tableView->currentIndex().row()).value("vch_operID").toString();
        QString errstring;
        lds_query query;
        //! <--transaction begin-->
        lds_query::tran_saction();
        if(query.execDelete("cey_sys_oper_purview", QString("vch_operID = '%1'").arg(vch_operidto),  &errstring)){
            if(query.execInsert("cey_sys_oper_purview",
                                QString(" select '%1' as vch_operID, ch_purviewno from  cey_sys_oper_purview where vch_operID = '%2' ")
                                .arg(vch_operidto)
                                .arg(vch_operidfrom), &errstring)) {
                lds_query::com_mit();
                _parent->torefresh();
                lds_messagebox::information(this, MESSAGE_TITLE_VOID,tr("保存成功"));
                return;
            }
        }
        lds_query::roll_back();
        _parent->torefresh();
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, errstring);
        //! <--transaction end-->
    }
}

void w_sys_manage_operatorright_rightcopy::toexit()
{
    this->reject();
}

//=============================
w_sys_manage_operatorright_pwdchange::w_sys_manage_operatorright_pwdchange(w_sys_manage_operatorright *parent):
    QDialog(parent),
    ui(new Ui_w_m_member_pwdchange_Dialog)
{
    ui->setupUi(this);
    //! toolbar
    ui->widget->setup(QStringList() << tr("确定") << tr("退出"), Qt::AlignRight, lds_keytoolbutton::SYSKEY);
    connect(ui->widget->index_widget(tr("确定")), SIGNAL(clicked()),this,SLOT(took()));
    connect(ui->widget->index_widget(tr("退出")), SIGNAL(clicked()),this,SLOT(toexit()));

    //! after
    ui->lineEdit_cur_operator->setEnabled(false);
    ui->lineEdit_newpwd->setEchoMode(QLineEdit::Password);
    ui->lineEdit_newpwd2->setEchoMode(QLineEdit::Password);
    ui->lineEdit_oldpwd->setEchoMode(QLineEdit::Password);
}

void w_sys_manage_operatorright_pwdchange::took()
{
    //clicked (none) returns long [pbm_bnclicked]
    //long ancestorreturnvalue
    QString ls_operid;
    QString ls_old;
    QString ls_new;
    QString ls_confirm;
    QString ls_err;
    lds_query query;

    ls_operid=ui->lineEdit_cur_operator->text();
    ls_old=ui->lineEdit_oldpwd->text();
    ls_new=ui->lineEdit_newpwd->text();
    ls_confirm=ui->lineEdit_newpwd2->text();

    //如果是自己修改自己的密码， 则必须输入旧密码
    if(public_sql::gs_operid==ls_operid){
        if(ls_old.trimmed().isEmpty()){
            lds_messagebox::warning(this, MESSAGE_TITLE_VOID,tr("请输入旧密码错误"));
            return;
        }
    }

    //新密码不为空
    if(ls_new.trimmed().isEmpty()){
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID,tr("新密码不能为空"));
        return;
    }
    //新旧密码一致
    if(ls_new != ls_confirm){
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID,tr("新密码与确认新密码不一样"));
        return;
    }

    //如果是 修改 admin的话，需要匹配旧密码
    if(ls_operid == "0000"){//0
        if(!n_func::f_cmp_syspwd(ls_old)){// != n_func::f_get_sysparm("system_pass", "0000")){
            lds_messagebox::warning(this, MESSAGE_TITLE_VOID,tr("旧密码错误"));
            return;
        }
    }   else {
        //如果是admin 来修改 非admin密码的话
        if(public_sql::gs_operid=="0000"){
        } else {
            //如果是非admin 来修改 非admin密码的话
            query.execSelect(QString("Select count(0)  from cey_sys_operator  where vch_operID ='%1' and ifnull(vch_password , '') = '%2'")
                             .arg(ls_operid)
                             .arg(ls_old));
            query.next();
            if(query.recordValue(0).toLongLong()==0){
                lds_messagebox::warning(this, MESSAGE_TITLE_VOID,tr("旧密码错误"));
                return;
            }
        }
    }//0
    //! <--transaction begin-->
    lds_query::tran_saction();
    if(ls_operid=="0000"){
        if((ls_err=n_func::f_set_sysparm("system_pass", ls_new, tr("系统管理员密码"))).isEmpty()){

        }
    } else {
        if(query.execUpdate("cey_sys_operator",qrtVariantPairList()

                            << qrtVariantPair("vch_password", ls_new),
                            QString(" vch_operID = '%1' ").arg(ls_operid))){

        } else {
            ls_err=query.recordError();
        }
    }
    if(ls_err.isEmpty()){
        lds_query::com_mit();
        lds_messagebox::information(this, MESSAGE_TITLE_VOID,tr("密码修改成功，请记住新密码"));
        this->accept();
    } else {
        lds_query::roll_back();
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID,tr("出现异常，密码修改失败"));
    }
    //! <--transaction end-->

}

void w_sys_manage_operatorright_pwdchange::toexit()
{
    this->reject();
}


w_sys_manage_operatorright_dish_type_right::w_sys_manage_operatorright_dish_type_right(const QString &vch_operid, QWidget *parent)
    : QDialog(parent)
{
    combobox = new lds_ComboBox(this);
    pushbutton_new = new lds_toolbar_button("pushbutton", this);
    pushbutton_del = new lds_toolbar_button("pushbutton", this);
    lds_toolbar_button *pushbutton_ok = new lds_toolbar_button("pushbutton", this);
    tableview = new lds_tableView(this);
    label_desc = new QLabel(this);
    QHBoxLayout *hlayout = new QHBoxLayout;
    QVBoxLayout *vlayout = new QVBoxLayout;
    hlayout->addStretch(1);
    hlayout->addWidget(new QLabel(tr("操作员"), this));
    hlayout->addWidget(combobox);
    hlayout->addWidget(pushbutton_new);
    hlayout->addWidget(pushbutton_del);
    hlayout->addWidget(pushbutton_ok);
    vlayout->addWidget(tableview, 1);
    vlayout->addWidget(label_desc);
    vlayout->addLayout(hlayout);
    this->setLayout(vlayout);

    pushbutton_new->setText(tr("新增"));
    pushbutton_ok->setText(tr("退出"));
    pushbutton_del->setText(tr("删除"));
    combobox->addItem(tr("全部"), "%");
    combobox->addItemsBySql(" Select vch_operID, vch_operator from cey_sys_operator ");
    combobox->setMinimumWidth(150);

    standmodel = new QStandardItemModel(this);
    standmodel->setHorizontalHeaderLabels(QStringList() << tr("操作员") << tr("菜品大类编码") << tr("菜品大类")/*<< tr("权限")*/);
    tableview->setModel(standmodel);
    tableview->setEditTriggers(QTableView::NoEditTriggers);
    tableview->setItemDelegateForColumn(0,  new lds_model_sqltablemodel_delegate_com(this,combobox));

    connect(pushbutton_new, SIGNAL(clicked()), this, SLOT(tonew()));
    connect(pushbutton_del, SIGNAL(clicked()), this, SLOT(todel()));
    connect(pushbutton_ok, SIGNAL(clicked()), this, SLOT(took()));
    connect(combobox, SIGNAL(currentIndexChanged(int)), this, SLOT(refresh()));

    setWindowTitle(tr("菜品权限"));
    combobox->setcomusrdata(vch_operid);
    refresh();
    resize(600, 400);
}

w_sys_manage_operatorright_dish_type_right::~w_sys_manage_operatorright_dish_type_right()
{
}

void w_sys_manage_operatorright_dish_type_right::tonew()
{
    QString vch_operid = combobox->curusrdata().toString();

    w_sys_manage_operatorright_dish_type_right_new_type dialog(vch_operid, this);
    dialog.setWindowTitle(_TEXT_SLOT(this));
    if(QDialog::Accepted == lds_roundeddialog_rect_align(&dialog).exec()) {
        lds_query query;
        foreach(const QString &dish_type, dialog.get_dish_type_list()) {
            query.execInsert("cey_bt_dish_type_oper",
                             qrtVariantPairList()
                             << qrtVariantPair("vch_operid", vch_operid)
                             << qrtVariantPair("ch_dish_typeno", dish_type)
                             );
        }
        refresh();
    }
}

void w_sys_manage_operatorright_dish_type_right::todel()
{

    int row = tableview->currentIndex().row();
    if(row < 0) {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, tr("没有数据被选中"));
        return;
    }
    QString vch_operid = standmodel->item(row, 0)->text();
    lds_query query;
    query.execDelete("cey_bt_dish_type_oper", QString("vch_operid = '%1' and ch_dish_typeno = '%2'")
                     .arg(vch_operid)
                     .arg(standmodel->item(row, 1)->text()));
    qDebug() << query.lastQuery();
    refresh();
}

void w_sys_manage_operatorright_dish_type_right::took()
{
    this->accept();
}

void w_sys_manage_operatorright_dish_type_right::tocancel()
{
    this->reject();
}

void w_sys_manage_operatorright_dish_type_right::refresh()
{
    QString vch_operid = combobox->curusrdata().toString();
    pushbutton_new->setEnabled(vch_operid != "%");

    standmodel->removeRows(0, standmodel->rowCount());

    QSet<QString> vch_operid_set;
    QSet<QString> ch_dish_typeno_set;
    lds_query query;
    query.execSelect(QString(" select *, (select a.vch_dish_typename from cey_bt_dish_type a where a.ch_dish_typeno = b.ch_dish_typeno) as vch_dish_typename  from cey_bt_dish_type_oper b where b.vch_operid like '%1' order by vch_operid asc ; ")
                     .arg(combobox->curusrdata().toString()));
    while(query.next()) {
        standmodel->appendRow(QList<QStandardItem*>()
                              << new QStandardItem(query.recordValue("vch_operid").toString())
                              << new QStandardItem(query.recordValue("ch_dish_typeno").toString())
                              << new QStandardItem(query.recordValue("vch_dish_typename").toString())
                              //                              << new QStandardItem(query.recordValue("ch_state").toString())
                              );
        vch_operid_set.insert(query.recordValue("vch_operid").toString());
        ch_dish_typeno_set.insert(query.recordValue("ch_dish_typeno").toString());
    }

    label_desc->setText(tr("操作员") + ":" + QString::number(vch_operid_set.count()) + "," +
                        tr("大类") + ":" + QString::number(ch_dish_typeno_set.count()));
    tableview->restore(0);
}


w_sys_manage_operatorright_dish_type_right_new_type::w_sys_manage_operatorright_dish_type_right_new_type(const QString &vch_operid, QWidget *parent)
    : QDialog(parent)
{
    ui = new Ui_w_bt_dish_unitset_Dialog;
    ui->setupUi(this);

    querymodel = new report_querymodel(this);
    querymodel->setQuery(QString(" select ch_dish_typeno, vch_dish_typename from cey_bt_dish_type where ch_dish_typeno not in (select ch_dish_typeno from cey_bt_dish_type_oper where vch_operid = '%1'); ")
                         .arg(vch_operid));
    ui->tableView->setEditTriggers(QTableView::NoEditTriggers);
    ui->tableView->setTransferHheader();
    ui->tableView->setModel(querymodel);
    ui->tableView->setSelectionMode(QTableView::MultiSelection);


    ui->widget->setup(QStringList()  << tr("全选") << tr("全不选") << tr("确定") << tr("退出"), Qt::AlignRight);
    connect(ui->widget->index_widget(tr("全选")), SIGNAL(clicked()), ui->tableView, SLOT(toSelectAll()));
    connect(ui->widget->index_widget(tr("全不选")), SIGNAL(clicked()), ui->tableView, SLOT(toClearAll()));
    connect(ui->widget->index_widget(tr("确定")), SIGNAL(clicked()), this, SLOT(accept()));
    connect(ui->widget->index_widget(tr("退出")), SIGNAL(clicked()), this, SLOT(reject()));
}

w_sys_manage_operatorright_dish_type_right_new_type::~w_sys_manage_operatorright_dish_type_right_new_type()
{
    delete ui;
}

QStringList w_sys_manage_operatorright_dish_type_right_new_type::get_dish_type_list()
{
    QStringList rets;
    QList<int> rowlist = ui->tableView->getSelectedRowListAsc();
    foreach(int row, rowlist) {
        rets << querymodel->model_data(row, "ch_dish_typeno").toString();
    }
    return rets;
}
