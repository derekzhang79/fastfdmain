#include "w_scr_dish_m_hang_pay_dialog.h"
#include "ui_w_scr_dish_m_hang_pay_dialog.h"
#include "lds_tableview_delegate_check.h"
#include <QStandardItemModel>
#include "lds_query.h"
#include "ui_w_m_member_report_detail_dialog.h"
#include "lds_messagebox.h"
#include <QPushButton>
#include <QMessageBox>
#include <QTimer>
#include <QTextEdit>
#include <QSqlError>
#include "backheader.h"
#include "n_func.h"
#include "lds_dialog_input.h"
#include <QtDebug>

w_scr_dish_m_hang_pay_Dialog::w_scr_dish_m_hang_pay_Dialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::w_scr_dish_m_hang_pay_Dialog)
{
    ui->setupUi(this);

    //1
    standmodel = new report_standmodel(this);
    standmodel->disable_data_transfer();
    ui->tableView->setTransferHheader();
    sql2 =
            " select "+QString()
            +" a.ch_balanceno as ch_payno"
            +", "+QString("if(a.ch_state='Y','%1','%2')").arg(tr("已还款")).arg(tr("未还款"))+"  as ch_state"
            +", a.num_remain  as num_remain_pay"

            +", a.vch_memberno  "
            +",b.vch_member  "
            + " from t_m_hang_balance  a"
            " LEFT JOIN t_m_member b ON a.vch_memberno = b.vch_memberno where b.vch_memberno = '%1' order by a.ch_state asc"
            ;
    sql =
            " select 'N', a.num_hangamount , a.vch_memberno,b.vch_member, b.vch_handtel, a.num_amount, a.num_point  from t_m_curamount a "
            " LEFT JOIN t_m_member b ON a.vch_memberno = b.vch_memberno where (b.vch_memberno like '%1%' or  b.vch_handtel like '%1%')  and ifnull(a.num_hangamount, 0) <> 0 order by a.num_hangamount desc"
            ;
    //2
    standmodel->setHorizontalHeaderLabels(QStringList() << tr("选中") <<  tr("会员号") << tr("会员名称") << tr("挂账金额")<< tr("手机号") /*<< tr("会员余额") << tr("会员积分")*/ );
    ui->tableView->setModel(standmodel);
    lds_tableview_delegate_check *deleg=new lds_tableview_delegate_check(
                QVector<int>()<<0,
                ui->tableView
                );
    deleg->setClickable();
    ui->tableView->setItemDelegateForColumn(0, deleg);
    ui->tableView->setEditTriggers(QTableView::NoEditTriggers);
    //3

    connect(ui->pushButton_all,SIGNAL(clicked()),this,SLOT(toall()));
    connect(ui->pushButton_allnot,SIGNAL(clicked()),this,SLOT(toallnot()));
    connect(ui->pushButton_exit,SIGNAL(clicked()),this,SLOT(toexit()));
    connect(ui->pushButton_pay,SIGNAL(clicked()),this,SLOT(topay()));
    connect(ui->pushButton_detail,SIGNAL(clicked()),this,SLOT(todetail()));
    connect(ui->lineEdit_filter,SIGNAL(textChanged(QString)),this,SLOT(toselect()));

    QTimer::singleShot(100, this, SLOT(toselect()));
}

w_scr_dish_m_hang_pay_Dialog::~w_scr_dish_m_hang_pay_Dialog()
{
    delete ui;
}

void w_scr_dish_m_hang_pay_Dialog::toall()
{
    for(int row = 0, rowcount = standmodel->rowCount(); row < rowcount; row++){
        standmodel->item(row, 0)->setText("Y");
    }
}

void w_scr_dish_m_hang_pay_Dialog::toallnot()
{
    for(int row = 0, rowcount = standmodel->rowCount(); row < rowcount; row++){
        standmodel->item(row, 0)->setText("N");
    }
}

void w_scr_dish_m_hang_pay_Dialog::toexit()
{
    this->reject();
}

void w_scr_dish_m_hang_pay_Dialog::topay()
{
    //1
    QDialog dialog(this);
    dialog.setWindowTitle(this->windowTitle());
    QTextEdit textedit;
    blockIns contentins;
    lds_toolbar_button b;
    b.setProperty("outer_stylesheet", "pushbutton");
    b.setText(tr("确定"));
    QVBoxLayout layout;
    layout.addWidget(&textedit);
    layout.addWidget(&b);
    dialog.setLayout(&layout);
    dialog.resize(400, 300);
    connect(&b,SIGNAL(clicked()),&dialog,SLOT(accept()));
    //2
    lds_query query;

    QList<rowMP> vch_membernolist;
    for(int row = 0, rowcount = standmodel->rowCount(); row < rowcount; row++){
        if(standmodel->item(row, 0)->text() == "Y"){
            rowMP rm;
            rm.r  = row;
            rm.m = standmodel->item(row, 1/*vch_memberno*/)->text();
            rm.p = standmodel->item(row, 3/*num_hangamount*/)->text().toDouble();
            rm.n = standmodel->item(row, 2/*vch_member*/)->text();
            if(rm.p != 0){
                vch_membernolist.append(rm);
            }
        }
    }

    if(vch_membernolist.isEmpty()){
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, tr("没有任何已挂账会员被选中"));
        return;
    }
    //1

    dialog.show();
    QString info;

    QString yvch_memberno;
    QString vch_memberno;
    QString ch_payno;
    double num_remain;
    double num_remain0;

    lds_dialog_input inputdialog(this);
    inputdialog.ui->label->setText(tr("支付现金:"));

    while(vch_membernolist.count() > 0){//loop2
        rowMP rm = vch_membernolist.takeAt(0);
        double curbillwillpay0;
        double curbillwillpay;
        //输入支付金额
        inputdialog.ui->lineEdit->setText(QString::number(rm.p));
        inputdialog.setWindowTitle(this->windowTitle()+"/"+rm.m);
        inputdialog.ui->lineEdit->selectAll();
        inputdialog.ui->lineEdit->setFocus();
        if(QDialog::Accepted != lds_roundeddialog_rect_align(&inputdialog).exec()){
            continue;
        }

        if(inputdialog.ui->lineEdit->text().toDouble() > rm.p){
            lds_messagebox::warning(this, MESSAGE_TITLE_VOID, tr("输入金额不能大于应收金额"));
            continue;
        }
        curbillwillpay0 = curbillwillpay = inputdialog.ui->lineEdit->text().toDouble();

        //开始单一会员付款
        yvch_memberno = vch_memberno = rm.m;
        blockIns mcontentins;
        query.execSelect(sql2.arg(vch_memberno));
        while(query.next()){//loop1
            ch_payno = query.recordValue(0).toString();
            //            num_pay = query.recordValue(5).toDouble();
            num_remain0 = num_remain = query.recordValue(2).toDouble();

            if(curbillwillpay <= 0) break;
            if(curbillwillpay > num_remain){

            } else {
                num_remain = curbillwillpay;
            }
            info = tr("正在挂账付款,会员号:%1,支付单号:%2").arg(vch_memberno).arg(ch_payno)+"    :";
            lds_query::tran_saction();
            lds_query query0;
            {
                if(query0.execUpdate("t_m_curamount",
                                     "num_hangamount", qrtFunc("num_hangamount -" + QString::number(num_remain)),
                                     qrtEqual("vch_memberno", vch_memberno))) {
                    if(query0.execUpdate("t_m_hang_balance",
                                         "num_remain", qrtFunc("num_remain -" + QString::number(num_remain)),
                                         qrtEqual("ch_balanceno", ch_payno))) {
                        if(query0.execUpdate("t_m_hang_balance",
                                             "ch_state", "Y",
                                             QString("  ch_balanceno = '%1' and ifnull(num_remain, 0) = 0 ").arg(ch_payno))){
                            //                        retv = w_sys_manage_cloudsync::upload_yun_pay_info(this, muy);
                            //                        if("ok" == retv)
                            {
                                curbillwillpay -= num_remain;
                                lds_query::com_mit();
                                info+=tr("成功")+"\n";
                                textedit.append(info);
                                mcontentins += backheader::printer.drawText("", tr("还款单号:")+ch_payno + "\n");//
                                mcontentins += backheader::printer.drawText("", tr("还款金额:")+QString::number(num_remain) + "\n");//
                                mcontentins += backheader::printer.drawText("", tr("剩余金额:")+QString::number(num_remain0-num_remain) + "\n");//
                                continue;
                            }
                        }
                    }
                }
            }
            info+=tr("失败")+"\n";
            textedit.append(info);
            lds_query::roll_back();
            if(1 == lds_messagebox::question(this ,_TEXT_SLOT(this), tr("会员(%1)挂账付款失败,是否继续").arg(rm.m), tr("继续"), tr("取消"))){
                goto end;
            }
            break;
        }//loop1
        if(!mcontentins.isEmpty()){
            blockIns ins;
            ins += backheader::printer.drawText("", tr("会员号:")+rm.m + "\n");//
            ins += backheader::printer.drawText("", tr("会员名称:")+rm.n + "\n");//
            ins += backheader::printer.drawText("", tr("会员还款前挂账金额:")+QString::number(rm.p) + "\n");//
            ins += backheader::printer.drawText("", tr("会员还款后挂账金额:")+QString::number(rm.p-curbillwillpay0+curbillwillpay) + "\n");//
            ins += mcontentins;
            contentins += ins;
            contentins += backheader::printer.drawText("-");//
        }
    }//loop2
end:


    if(!contentins.isEmpty()){
        blockIns printdata;
        printdata += backheader::printer.drawText("i,a00,s11", tr("会员挂账还款\n"));//清除缓存 居中 2x2x
        printdata += backheader::printer.drawText("i", "");//清除缓存

        printdata += contentins;
        printdata += backheader::printer.drawText("", tr("操作员")+":"+public_sql::gs_opername + "\n");//
        printdata += backheader::printer.drawText("", tr("操作日期")+":"+n_func::f_get_sysdatetime().toString("yyyy-MM-dd hh:mm:ss") + "\n");

        printdata += backheader::printer.drawTextEnd("1B6405 1D5601", "");//走纸5x， 切纸
        printdata =   backheader::printer.setPrintCount(printdata, 1);

        backheader::asyn_dataprint(printdata);
    }


    toselect();
    lds_roundeddialog_rect_align(&dialog).exec();
}

void w_scr_dish_m_hang_pay_Dialog::todetail()
{
    int row = ui->tableView->currentIndex().row();
    QPushButton *b = qobject_cast<QPushButton *>(this->sender());
    if( row < 0){
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, tr("没有行被选中"));
        return;
    }
    QDialog dialog(this);
    report_querymodel querymodel;
    Ui_w_m_member_report_detail_Dialog ui_detail;
    ui_detail.setupUi(&dialog);
    ui_detail.tableView->setTransferHheader();
    ui_detail.tableView->setModel(&querymodel);
    dialog.resize(750, 400);
    dialog.setWindowTitle(this->windowTitle() + b->text());
    ui_detail.label_desc->setText(tr("会员编号")+":"+standmodel->item(row, 1/*vch_memberno*/)->text());
    querymodel.setQuery(sql2.arg(standmodel->item(row, 1/*vch_memberno*/)->text()));
    if(querymodel.rowCount() > 0) {
        QTimer::singleShot(100, ui_detail.tableView, SLOT(resizeColumnsToContents()));
    }
    lds_roundeddialog_rect_align(&dialog).exec();
}

void w_scr_dish_m_hang_pay_Dialog::toselect()
{
    standmodel->removeRows(0, standmodel->rowCount());
    lds_query query;
    query.execSelect(sql.arg(ui->lineEdit_filter->text()));
    while(query.next()){
        standmodel->appendRow(QList<QStandardItem*>()
                              << new QStandardItem(query.recordValue(0).toString())
                              << new QStandardItem(query.recordValue("vch_memberno").toString())
                              << new QStandardItem(query.recordValue("vch_member").toString())
                              << new QStandardItem(query.recordValue("num_hangamount").toString())
                              << new QStandardItem(query.recordValue("vch_handtel").toString())
                              );
    }
    ui->tableView->resizeColumnsToContents();
}
