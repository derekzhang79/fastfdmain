#include "back_view_dialog.h"
#include "ui_back_view_dialog.h"
#include "ftableview_standmodel_sql_none.h"
#include "ftableview_delegate.h"
#include <QPainter>
#include <QtDebug>
#include "public_sql.h"
#include <QFile>
#include "w_bt_dish_base.h"
#include "w_bt_dish_table.h"
#include "w_bt_dish_suit.h"
#include "w_bt_dish_cookset.h"
#include "w_bt_dish_kitchen_dish.h"
#include "w_bt_dish_quit.h"
#include "w_m_member.h"
#include "w_m_member_cardic.h"
#include "w_m_member_list.h"
#include "w_m_member_paramset.h"
#include "w_m_member_point.h"
#include "w_m_member_recharge.h"
#include "w_m_member_statemanage.h"
#include "w_m_member_type.h"
#include "w_rpt_report_daymonweekanalysis.h"
#include "w_rpt_report_frame_dishquit.h"
#include "w_rpt_report_categorysalesanalysis.h"
#include "w_rpt_report_daybusinessreprot.h"
#include "w_rpt_report_directsalegrossprofitreport.h"
#include "w_rpt_report_periodbusinessanalysis.h"
#include "w_rpt_report_promotionreport.h"
#include "w_rpt_report_servantpromotquery.h"
#include "w_rpt_report_servicefeelowpinquery.h"
#include "w_rpt_report_tablebusinessreport.h"
#include "w_sys_manage.h"
#include "w_sys_manage_basicdataexport.h"
#include "w_sys_manage_basicdataimport.h"
#include "w_sys_manage_businessdataclear.h"
#include "w_sys_manage_companyinformation.h"
#include "w_sys_manage_mysql.h"
#include "w_sys_manage_operatorinformation.h"
#include "w_sys_manage_operatorright.h"
#include "w_sys_manage_sqlcopy.h"
#include "w_sys_manage_sysreboot.h"
#include "w_sys_manage_sysset.h"
#include "w_sys_manage_wirelessordermonitoring.h"
#include "w_ds_sale_anticheckout.h"
#include "w_ds_sale_businessserach.h"
#include "w_ds_sale_cashjournal.h"
#include "w_ds_sale_daybusiness.h"
#include "w_ds_sale_dishsaleout.h"
#include "w_ds_sale_qt.h"
#include "w_ds_sale_salejournal.h"
#include "w_bt_dish_suit.h"
#include "w_i_inventory_check.h"
#include "w_i_inventory_invoicing.h"
#include "w_i_inventory_query.h"
#include "w_i_inventory_summaryquery.h"
#include "backheader.h"
#include "business_documents_query.h"
#include "back_anti_checkout.h"
#include "lds_messagebox.h"
#include <QtDebug>
#include "public_sql.h"
#include "lds_messagebox.h"
#include "public_logindialog.h"
#include "w_sys_manage_outer_settings.h"
#include "w_sys_manage_outer_pay_set.h"
#include "w_sys_manage_cloudsync_with_time.h"
#include "w_bt_dish_paymode.h"
#include <QSvgRenderer>
#include "fastfd_update.h"
#include "qprocess.h"
#include "w_rpt_report_frame_member.h"
#include "w_rpt_report_frame_dishsale.h"
#include "w_rpt_report_frame_saleflow.h"
#include "w_rpt_report_frame_cashflow.h"
#include "w_rpt_report_frame_cash.h"
#include "back_view_svg_render.h"
#include "w_rpt_report_frame_shift.h"
#include "material_purchase_apply.h"
#include "material_purchase_in_storage.h"
#include "material_loss_apply.h"
#include "w_sys_manage_promotion_setting.h"
#include "w_rpt_report_frame_material_saleflow.h"
#include "w_rpt_report_frame_material_saleflow2.h"
#include "w_rpt_report_frame_dishsalerank.h"
#include "w_sys_manage_cloudsync_cloud2.h"
#ifdef Q_OS_WIN
#include "TCHAR.H"
#endif
#include "w_inventory_input.h"
#include "w_inventory_draw.h"
#include "lds_dialog_input.h"
#include "w_rpt_report_frame_servantpromotquery.h"

back_view_dialog::back_view_dialog(QWidget *parent) :
    lds_roundeddialog_rect0(parent)
{
    ui = new Ui_back_view_dialog;
    ui->setupUi(this);
    lds::setwflagFrameless(this);
    resize(lds::MAIN_WINDOW_SIZE);
    QVBoxLayout *vlayout = new QVBoxLayout(this);
    this->setLayout(vlayout);
    ui->widget_3->setTitleColor(&titleColor);
    vlayout->addWidget(ui->widget_3);
    vlayout->addWidget(ui->label_4);
    vlayout->addWidget(ui->widget_content);
    vlayout->addWidget(ui->widget_tail);
    vlayout->setContentsMargins(0, 0, 0, 9);
    vlayout->setSpacing(0);
    ui->frame->setFixedWidth(150);
    if(lds::FULL_WINDOW_SIZE.width() == 1920) {
        ui->frame->setFixedWidth(250);
    }
    dialog = 0;
    backlist_model = 0;

    QTimer::singleShot(0, this, SLOT(tofirstGoIn()));
    //3
}

back_view_dialog::~back_view_dialog()
{
    delete ui;
}

void back_view_dialog::tableview_span_column(QTableView *t,int c)
{
    for(int r = 0, rcount = t->verticalHeader()->count();r<rcount;r++) {
        t->setSpan(r, c, 1, 2);
    }
}

QPixmap back_view_dialog::get_fastfdbacklogo()
{
    QPixmap fastfdbacklogo;
    if(!QFile::exists("pos/fastfdbacklogo.png")){
        public_sql::copyfile(":/image/backview/backlogo.png", "pos/fastfdbacklogo.png");
    }
    fastfdbacklogo.load("pos/fastfdbacklogo.png");
    if(!fastfdbacklogo.isNull()){
        fastfdbacklogo = fastfdbacklogo.scaledToHeight(lds::BACK_LOGO_HEIGHT);
    }
    return fastfdbacklogo;
}

QStringList back_view_dialog::get_fastfdbacktext()
{
    QFile fastfdbacktext("pos/fastfdbacktext.txt");
    if(!fastfdbacktext.exists()){
        fastfdbacktext.open(QFile::WriteOnly);
        fastfdbacktext.write(tr("摇钱数餐饮软件").toLocal8Bit());
        fastfdbacktext.write("\n");
        fastfdbacktext.write("Nanjing Restaurant Software Co,Ltd.");
        fastfdbacktext.write("\n");
        fastfdbacktext.write("www.yqspos.com");
        fastfdbacktext.close();
    }
    if(fastfdbacktext.open(QFile::ReadOnly)){
        QString rets = fastfdbacktext.readAll();
        return rets.split("\n");
    }
    return QStringList();
}

void back_view_dialog::retranslateView()
{
    //1
    backlist_model->model_data_set(0, 0, tr("基础资料"), Qt::UserRole);
    backlist_model->model_data_set(1, 0, tr("会员设置"), Qt::UserRole);
    backlist_model->model_data_set(2, 0, tr("库存管理"), Qt::UserRole);
    backlist_model->model_data_set(3, 0, tr("报表分析"), Qt::UserRole);
    backlist_model->model_data_set(4, 0, tr("系统设置"), Qt::UserRole);
    backlist_model->model_data_set(5, 0, tr("云端连锁"), Qt::UserRole);
    backlist_model->model_data_set(6, 0, tr("前台"), Qt::UserRole);
    backlist_model->model_data_set(7, 0, tr("退出"), Qt::UserRole);


    //2
    ui->label_version->setText(fastfd_update::gs_soft_version);
    ui->label->clear();
    ui->label->setPixmap(back_view_dialog::get_fastfdbacklogo());
    QStringList ret_list = back_view_dialog::get_fastfdbacktext();
    ui->label_2->setText(ret_list.value(0).trimmed());
    ui->label_5->setText(ret_list.value(1).trimmed());
    ui->label_6->setText(ret_list.value(2).trimmed());


    //3
    for(int k = 0; k < ui->stackedWidget->count(); k++) {
        QTableView *tableview = static_cast<QTableView *>(ui->stackedWidget->widget(k));
        if(tableview) {
            QStandardItemModel *model = static_cast<QStandardItemModel *>(tableview->model());
            model->removeRows(0, model->rowCount());
            delete model;

            ui->stackedWidget->removeWidget(tableview);
            delete tableview;
        }
    }
    index_key_map.clear();
    toupdateSection(ui->tableView_list->currentIndex());
}

QString back_view_dialog::dialogClassName()
{
#ifdef QT_DEBUG
    if(dialog) {
        qDebug() << "=============:" <<  dialog->metaObject()->className();
    }
#endif
    return "";
}

void back_view_dialog::tofirstGoIn()
{
    //1
    backlist_model = new ftableview_standmodel_sql_none;
    backlist_model->set_row_column(8, 1);

    backlist_model->model_data_set(0, 0,"jczl", Qt::UserRole+1);
    backlist_model->model_data_set(1, 0,"hysz", Qt::UserRole+1);
    backlist_model->model_data_set(2, 0,"kcgl", Qt::UserRole+1);
    backlist_model->model_data_set(3, 0,"bbfx", Qt::UserRole+1);

    backlist_model->model_data_set(4, 0,"xtsz", Qt::UserRole+1);

    backlist_model->model_data_set(5, 0,"ydls", Qt::UserRole+1);

    backlist_model->model_data_set(6, 0,"qt", Qt::UserRole+1);
    backlist_model->model_data_set(7, 0,"tc", Qt::UserRole+1);

    ftableview_delegate *d = new ftableview_delegate(ui->tableView_list);
    d->keepConnect(ui->tableView_list);
    d->setMargin(1);
    d->indexisEmptystillDraw = true;

    ui->tableView_list->setModel(backlist_model);
    ui->tableView_list->setItemDelegate(d);
    ui->tableView_list->setHeaderResizeMode(Qt::Vertical, QHeaderView::Stretch);
    ui->tableView_list->setHeaderResizeMode(Qt::Horizontal, QHeaderView::Stretch);
    connect(ui->tableView_list,SIGNAL(clicked(QModelIndex)),this,SLOT(toupdateSection(QModelIndex)));

    ui->tableView_list->setCurrentIndex(backlist_model->index(0,0));
    retranslateView();
}

void back_view_dialog::toupdateSection(const QModelIndex &index)
{
    QString key = index.data(Qt::UserRole + 1).toString();
    if(key == "jczl") {
        if(index_key_map.value(index.row(), -1) == -1) {
            QTableView *tableview = creat_tableview();
            QStandardItemModel *model = new QStandardItemModel;
            model->appendColumn(QList<QStandardItem*>()
                                << new QStandardItem("0\t:/image/backview/c1\t"+tr("菜品资料维护"))
                                << new QStandardItem("0\t:/image/backview/c2\t"+tr("菜品做法设置"))
                                << new QStandardItem("0\t:/image/backview/c4\t"+tr("菜品套菜资料"))
                                << new QStandardItem("0\t:/image/backview/c5\t"+tr("餐桌资料"))
                                );
            model->appendColumn(QList<QStandardItem*>()
                                );
            model->appendColumn(QList<QStandardItem*>()
                                );
            model->appendColumn(QList<QStandardItem*>()
                                << new QStandardItem("1\t:/image/backview/f2\t"+tr("基础资料导出"))
                                << new QStandardItem("1\t:/image/backview/f1\t"+tr("基础资料导入"))
                                << new QStandardItem("1\t:/image/backview/c8\t"+tr("退菜原因"))
                                );
            tableview->setModel(model);

            tableview_span_column(tableview, 0);
            tableview->setSpan(2, 3, 2, 1);
            index_key_map.insert(index.row(), ui->stackedWidget->addWidget(tableview));
        }
    }
    if(key == "hysz") {
        if(index_key_map.value(index.row(), -1) == -1) {
            QTableView *tableview = creat_tableview();
            QStandardItemModel *model = new QStandardItemModel;
            model->appendColumn(QList<QStandardItem*>()
                                << new QStandardItem("0\t:/image/backview/b1\t"+tr("会员参数设置"))
                                << new QStandardItem("0\t:/image/backview/b2\t"+tr("会员类型变更"))
                                << new QStandardItem("0\t:/image/backview/b4\t"+tr("会员资料维护"))
                                << new QStandardItem("0\t:/image/backview/b3\t"+tr("会员积分管理"))
                                );
            model->appendColumn(QList<QStandardItem*>()
                                );
            model->appendColumn(QList<QStandardItem*>()
                                );
            model->appendColumn(QList<QStandardItem*>()
                                << new QStandardItem("1\t:/image/backview/b5\t"+tr("会员状态管理"))
                                << new QStandardItem("1\t:/image/backview/b6\t"+tr("充值"))
                                << new QStandardItem("1\t:/image/backview/b8\t"+tr("芯片清除"))
                                << new QStandardItem("1\t:/image/backview/b7\t"+tr("会员报表"))
                                );
            tableview->setModel(model);

            tableview_span_column(tableview, 0);
            index_key_map.insert(index.row(), ui->stackedWidget->addWidget(tableview));
        }
    }
    if(key == "kcgl") {
        if(index_key_map.value(index.row(), -1) == -1) {
            QTableView *tableview = creat_tableview();
            QStandardItemModel *model = new QStandardItemModel;
            model->appendColumn(QList<QStandardItem*>()
                                << new QStandardItem("0\t:/image/backview/d8\t"+tr("商品入库"))
                                << new QStandardItem("0\t:/image/backview/d5\t"+tr("商品退货"))
                                << new QStandardItem("0\t:/image/backview/d7\t"+tr("商品报损"))
                                << new QStandardItem("0\t:/image/backview/d9\t"+tr("库存盘点"))
                                );
            model->appendColumn(QList<QStandardItem*>()
                                );
            model->appendColumn(QList<QStandardItem*>()
                                );
            model->appendColumn(QList<QStandardItem*>()
                                << new QStandardItem("1\t:/image/backview/d4\t"+tr("盘点查询"))
                                << new QStandardItem("1\t:/image/backview/d3\t"+tr("进销存汇总"))
                                << new QStandardItem("1\t:/image/backview/d2\t"+tr("单据汇总查询"))
                                << new QStandardItem("1\t:/image/backview/d6\t"+tr("库存查询"))
                                );
            tableview->setModel(model);

            tableview_span_column(tableview, 0);
            index_key_map.insert(index.row(), ui->stackedWidget->addWidget(tableview));
        }
    }
    if(key == "bbfx") {
        if(index_key_map.value(index.row(), -1) == -1) {
            QTableView *tableview = creat_tableview();
            QStandardItemModel *model = new QStandardItemModel;
            model->appendColumn(QList<QStandardItem*>()
                                << new QStandardItem("1\t:/image/backview/a2\t"+tr("菜品消费报表"))
                                << new QStandardItem("1\t:/image/backview/a13\t"+tr("营业收银报表"))
                                << new QStandardItem("1\t:/image/backview/e7\t"+tr("销售流水"))
                                << new QStandardItem("1\t:/image/backview/e8\t"+tr("收银流水"))
                                << new QStandardItem("1\t:/image/backview/a3\t"+tr("菜品销售排行统计"))
                                );
            model->appendColumn(QList<QStandardItem*>()
                                << new QStandardItem("1\t:/image/backview/e2\t"+tr("营业数据查询"))
                                << new QStandardItem("1\t:/image/backview/e1\t"+tr("反结账查询"))
                                << new QStandardItem("1\t:/image/backview/a12\t"+tr("退菜情况报表"))
                                << new QStandardItem("1\t:/image/backview/a15\t"+tr("交班报表"))
                                << new QStandardItem("1\t:/image/backview/a1\t"+tr("菜品促销报表"))
                                );
            model->appendColumn(QList<QStandardItem*>()
                                << new QStandardItem("1\t:/image/backview/a8\t"+tr("日营业情况统计"))
                                << new QStandardItem("1\t:/image/backview/a9\t"+tr("日月周报表分析"))
                                << new QStandardItem("1\t:/image/backview/f21\t"+tr("原料销售流水"))
                                << new QStandardItem("1\t:/image/backview/a14\t"+tr("原料毛利报表"))
                                << new QStandardItem("1\t:/image/backview/a6\t"+tr("服务员提成查询"))
                                );
            model->appendColumn(QList<QStandardItem*>()
                                << new QStandardItem("1\t:/image/backview/a4\t"+tr("餐桌营业数据统计"))
                                << new QStandardItem("1\t:/image/backview/a5\t"+tr("服务费与低消查询"))
                                << new QStandardItem("1\t:/image/backview/a10\t"+tr("时段营业情况分析"))
                                << new QStandardItem("1\t:/image/backview/a7\t"+tr("品类销售营业分析"))
                                );
            tableview->setModel(model);
            //            tableview->setSpan(3, 1, 2, 1);
            tableview->setSpan(3, 3, 2, 1);
            index_key_map.insert(index.row(), ui->stackedWidget->addWidget(tableview));
        }
    }

    if(key == "xtsz") {
        if(index_key_map.value(index.row(), -1) == -1) {
            QTableView *tableview = creat_tableview();
            QStandardItemModel *model = new QStandardItemModel;
            model->appendColumn(QList<QStandardItem*>()
                                << new QStandardItem("0\t:/image/backview/f13\t"+tr("外部设备"))
                                << new QStandardItem("0\t:/image/backview/f14\t"+tr("收银设置"))
                                << new QStandardItem("0\t:/image/backview/f12\t"+tr("收银方式"))
                                << new QStandardItem("0\t:/image/backview/e3\t"+tr("促销设置"))
                                << new QStandardItem("0\t:/image/backview/f7\t"+tr("公司信息"))
                                );
            model->appendColumn(QList<QStandardItem*>()
                                );
            model->appendColumn(QList<QStandardItem*>()
                                );
            model->appendColumn(QList<QStandardItem*>()
                                << new QStandardItem("1\t:/image/backview/f5\t"+tr("操作员信息"))
                                << new QStandardItem("1\t:/image/backview/f6\t"+tr("操作员权限"))
                                << new QStandardItem("1\t:/image/backview/f4\t"+tr("数据库"))
                                << new QStandardItem("1\t:/image/backview/f20\t"+tr("在线升级"))
                                );
            tableview->setModel(model);
            tableview_span_column(tableview, 0);
            tableview->setSpan(3, 3, 2, 1);
            index_key_map.insert(index.row(), ui->stackedWidget->addWidget(tableview));
        }
    }
    if(key == "ydls") {
        if(index_key_map.value(index.row(), -1) == -1) {
            QTableView *tableview = creat_tableview();
            QStandardItemModel *model = new QStandardItemModel;
            model->appendColumn(QList<QStandardItem*>()
                                << new QStandardItem("0\t:/image/backview/f9\t"+tr("云设置"))
                                << new QStandardItem("0\t:/image/backview/g3\t"+tr("采购申请"))
                                << new QStandardItem("0\t:/image/backview/g2\t"+tr("门店入库"))
                                << new QStandardItem("0\t:/image/backview/g1\t"+tr("门店出库"))
                                );
            model->appendColumn(QList<QStandardItem*>()
                                );
            model->appendColumn(QList<QStandardItem*>()
                                );
            model->appendColumn(QList<QStandardItem*>()
                                );
            tableview->setModel(model);
            tableview_span_column(tableview, 0);
            index_key_map.insert(index.row(), ui->stackedWidget->addWidget(tableview));
        }
    }
    if(key == "qt") {
        //        if(0==lds_messagebox::question(this, MESSAGE_TITLE_VOID,tr("确认进入前台?"), tr("确认"),tr("取消"))){
        if(backheader::operid_right_code(this, "0241")){//前台营业【 0241 】
            this->accept();
            return;
        }
        //        }
        ui->tableView_list->setCurrentIndex(tableview_list_pre_row);
        return;
    }
    if(key == "tc") {
        if(0==lds_messagebox::question(this, MESSAGE_TITLE_VOID,tr("确认退出后台?"), tr("确认"),tr("取消"))){
            this->reject();
            return;
        }
        ui->tableView_list->setCurrentIndex(tableview_list_pre_row);
        return;
    }

    ui->stackedWidget->setCurrentIndex(index_key_map.value(index.row(), -1));
    tableview_list_pre_row = index;
}

void back_view_dialog::topop(const QModelIndex &index)
{
    int type = ui->tableView_list->currentIndex().row();
    int r = index.row();
    int c = index.column();
    QStringList texts = index.data().toString().split("\t", QString::SkipEmptyParts);
    QString text = texts.value(2);
    //    QString oldlanguage = lds::get_soft_curent_language_first();
    switch(type) {
    case 0:
        if(c == 0 && r == 0) {
            if(backheader::operid_right_code(this, "0101")){//菜品资料维护【 0101 】
                dialog=static_cast<QDialog *>(new w_bt_dish_base(this));
                dialog->setFixedSize(lds::MAIN_WINDOW_SIZE);
                dialog->setWindowTitle(text + dialogClassName());//菜品资料维护"));
            }
            break;
        }
        if(c == 0 && r == 1) {
            if(backheader::operid_right_code(this, "0104")){//菜品做法设置【 0104 】
                dialog=static_cast<QDialog *>(new w_bt_dish_cookset(this));
                dialog->setFixedSize(lds::MAIN_WINDOW_SIZE);
                dialog->setWindowTitle(text + dialogClassName());//菜品做法设置"));
            }
            break;
        }
        if(c == 0 && r == 2) {
            if(backheader::operid_right_code(this, "0102")){//菜品套菜资料【 0102 】
                dialog=static_cast<QDialog *>(new w_bt_dish_suit(this));
                dialog->setFixedSize(lds::MAIN_WINDOW_SIZE);
                dialog->setWindowTitle(text + dialogClassName());//菜品套菜资料"));
            }
            break;
        }
        if(c == 0 && r == 3) {
            if(backheader::operid_right_code(this, "0105")){//餐桌资料维护【 0105 】
                dialog=static_cast<QDialog *>(new w_bt_dish_table(this));
                dialog->setFixedSize(lds::MAIN_WINDOW_SIZE);
                dialog->setWindowTitle(text + dialogClassName());//餐桌资料维护"));
            }
            break;
        }
        if(c == 3 && r == 0) {
            if(backheader::operid_right_code(this, "0906")){//基础数据导出【 0906 】
                dialog=static_cast<QDialog *>(new w_sys_manage_basicdataexport(this));
                dialog->setWindowTitle(text + dialogClassName());//基础数据导出"));
            }
            break;
        }
        if(c == 3 && r == 1) {
            if(backheader::operid_right_code(this, "0907")){//基础数据导入【 0907 】
                dialog=static_cast<QDialog *>(new w_sys_manage_basicdataimport(this));
                dialog->setWindowTitle(text + dialogClassName());//基础数据导入"));
            }
            break;
        }
        if(c == 3 && r == 2) {//餐厅地图
            if(backheader::operid_right_code(this, "0110")){//退菜原因【 0110 】
                dialog=static_cast<QDialog *>(new w_bt_dish_quit(this));
                dialog->setWindowTitle(text + dialogClassName());//退菜原因"));
            }
            break;
        }
        break;
        //////////////////////////////////////////////////////////////////////
    case 1:
        if(c == 0 && r == 0) {
            if(backheader::operid_right_code(this, "0401")){//会员参数设置【 0401 】
                dialog=static_cast<QDialog *>(new w_m_member_paramset(this));
                dialog->setWindowTitle(text + dialogClassName());//会员参数设置"));
            }
            break;
        }
        if(c == 0 && r == 1) {
            if(backheader::operid_right_code(this, "0407")){//会员类型变更【 0407 】
                dialog = static_cast<QDialog *>(new w_m_member_type(this));
                dialog->setWindowTitle(text + dialogClassName());//会员类型变更"));
            }

            break;
        }
        if(c == 0 && r == 2) {
            if(backheader::operid_right_code(this, "0402")){//会员资料维护【 0402 】
                dialog=static_cast<QDialog *>(new w_m_member_list(this));
                dialog->setWindowTitle(text + dialogClassName());//会员资料维护"));
                dialog->setFixedSize(lds::MAIN_WINDOW_SIZE);
            }
            break;
        }
        if(c == 0 && r == 3) {
            if(backheader::operid_right_code(this, "0406")){//会员积分管理【 0406 】
                dialog=static_cast<QDialog *>(new w_m_member_point(this));
                dialog->setWindowTitle(text + dialogClassName());//会员积分管理"));
                dialog->setFixedSize(lds::MAIN_WINDOW_SIZE);
            }
            break;
        }


        if(c == 3 && r == 0) {
            if(backheader::operid_right_code(this, "0403")){//会员状态管理【 0403 】
                dialog=static_cast<QDialog *>(new w_m_member_statemanage(this));
                dialog->setWindowTitle(text + dialogClassName());//会员状态管理"));
                dialog->setFixedSize(lds::MAIN_WINDOW_SIZE);
            }
            break;
        }
        if(c == 3 && r == 1) {
            w_m_member_list::statictorecharge(this, 0, 0, "");
            break;
        }
        if(c == 3 && r == 2) {
            if(backheader::operid_right_code(this, "0413")){//芯片清除【 0413 】
                dialog=static_cast<QDialog *>(new w_m_member_cardic(this));
                dialog->setWindowTitle(text + dialogClassName());//芯片清除"));
            }
            break;
        }
        if(c == 3 && r == 3) {
            if(backheader::operid_right_code(this, "0411")){//会员报表【 0411 】
                dialog=static_cast<QDialog *>(new w_rpt_report_frame_member(this));
                dialog->setWindowTitle(text + dialogClassName());//会员报表"));
                dialog->setFixedSize(lds::MAIN_WINDOW_SIZE);
            }
            break;
        }
        break;
        //////////////////////////////////////////////////////////////////////
    case 2:
        if(c == 0 && r == 0) {
            if(backheader::operid_right_code(this, "0602")){//商品入库【 0602 】
                dialog=static_cast<QDialog *>(new w_inventory_input("RK",this));
                dialog->setWindowTitle(text + dialogClassName());//商品入库"));
                dialog->setFixedSize(lds::MAIN_WINDOW_SIZE);
            }
            break;
        }
        if(c == 0 && r == 1) {
            if(backheader::operid_right_code(this, "0603")){//商品退货【 0603 】
                dialog=static_cast<QDialog *>(new w_inventory_input("TH",this));
                dialog->setWindowTitle(text + dialogClassName());//商品退货"));
                dialog->setFixedSize(lds::MAIN_WINDOW_SIZE);
            }
            break;
        }
        if(c == 0 && r == 2) {
            if(backheader::operid_right_code(this, "0604")){//商品报损【 0604 】
                dialog=static_cast<QDialog *>(new w_inventory_input("CK",this));
                dialog->setWindowTitle(text + dialogClassName());//商品报损"));
                dialog->setFixedSize(lds::MAIN_WINDOW_SIZE);
            }
            break;
        }
        if(c == 0 && r == 3) {
            if(backheader::operid_right_code(this, "0605")){//库存盘点【 0605 】
                dialog=static_cast<QDialog *>(new w_inventory_draw(this));
                dialog->setWindowTitle(text + dialogClassName());
                dialog->setFixedSize(lds::MAIN_WINDOW_SIZE);
            }
            break;
        }

        if(c == 3 && r == 0) {
            if(backheader::operid_right_code(this, "0609")){//盘点查询【 0609 】
                dialog=static_cast<QDialog *>(new w_i_inventory_check(this));
                dialog->setWindowTitle(text + dialogClassName());//盘点查询"));
                dialog->setFixedSize(lds::MAIN_WINDOW_SIZE);
            }
            break;
        }
        if(c == 3 && r == 1) {
            if(backheader::operid_right_code(this, "0610")){//进销存汇总报表【 0610 】
                dialog=static_cast<QDialog *>(new w_i_inventory_Invoicing(this));
                dialog->setWindowTitle(text + dialogClassName());//进销存汇总报表"));
                dialog->setFixedSize(lds::MAIN_WINDOW_SIZE);
            }
            break;
        }
        if(c == 3 && r == 2) {
            if(backheader::operid_right_code(this, "0611")){//单据汇总查询【 0611 】
                dialog=static_cast<QDialog *>(new w_i_inventory_summaryquery(this));
                dialog->setWindowTitle(text + dialogClassName());//单据汇总查询"));
                dialog->setFixedSize(lds::MAIN_WINDOW_SIZE);
            }
            break;
        }
        if(c == 3 && r == 3) {
            if(backheader::operid_right_code(this, "0606")){//库存查询【 0606 】
                dialog=static_cast<QDialog *>(new w_i_inventory_query(this));
                dialog->setWindowTitle(text + dialogClassName());//库存查询"));
                dialog->setFixedSize(lds::MAIN_WINDOW_SIZE);
            }
            break;
        }
        break;
        //////////////////////////////////////////////////////////////////////
    case 3:
        if(c == 0 && r == 0) {
            if(backheader::operid_right_code(this, "0701")){//菜品消费报表【 0701 】
                dialog=static_cast<QDialog *>(new w_rpt_report_frame_dishsale(this));
                dialog->setWindowTitle(text + dialogClassName());//菜品消费报表"));
                dialog->resize(lds::MAIN_WINDOW_SIZE);
            }
            break;
        }
        if(c == 0 && r == 1) {
            if(backheader::operid_right_code(this, "0705")){//营业收银报表【 0705 】
                dialog=static_cast<QDialog *>(new w_rpt_report_frame_cash(this));
                dialog->setWindowTitle(text + dialogClassName());//营业收银报表"));
                dialog->resize(lds::MAIN_WINDOW_SIZE);
            }
            break;
        }
        if(c == 0 && r == 2) {
            if(backheader::operid_right_code(this, "0203")){//菜品销售流水查询【 0203 】
                dialog=static_cast<QDialog *>(new w_rpt_report_frame_saleflow(this));
                dialog->setWindowTitle(text + dialogClassName());//菜品销售流水查询"));
                dialog->resize(lds::MAIN_WINDOW_SIZE);
            }
            break;
        }
        if(c == 0 && r == 3) {
            if(backheader::operid_right_code(this, "0204")){//收银流水【 0204 】
                dialog=static_cast<QDialog *>(new w_rpt_report_frame_cashflow(this));
                dialog->setWindowTitle(text + dialogClassName());//收银流水"));
                dialog->resize(lds::MAIN_WINDOW_SIZE);
            }
            break;
        }
        if(c == 0 && r == 4) {
            if(backheader::operid_right_code(this, "0714")){//菜品销售排行统计【 0714 】
                dialog=static_cast<QDialog *>(new w_rpt_report_frame_dishsalerank(this));
                dialog->setWindowTitle(text + dialogClassName());//菜品销售排行统计"));
                dialog->resize(lds::MAIN_WINDOW_SIZE);
            }
            break;
        }
        if(c == 1 && r == 0) {
            dialog=static_cast<QDialog *>(new business_documents_query(this));
            dialog->setWindowTitle(text + dialogClassName());//营业单据查询"));
            dialog->resize(lds::MAIN_WINDOW_SIZE);
            break;
        }
        if(c == 1 && r == 1) {
            if(backheader::operid_right_code(this, "0227")){//反结账查询【 0227 】
                dialog=static_cast<QDialog *>(new back_anti_checkout(this));
                dialog->setWindowTitle(text + dialogClassName());//反结账查询"));
                dialog->resize(lds::MAIN_WINDOW_SIZE);
            }
            break;
        }
        if(c == 1 && r == 2) {
            if(backheader::operid_right_code(this, "0704")){//退菜情况报表【 0704 】
                dialog=static_cast<QDialog *>(new w_rpt_report_frame_dishquit(this));
                dialog->setWindowTitle(text + dialogClassName());//退菜情况报表"));
                dialog->resize(lds::MAIN_WINDOW_SIZE);
            }
            break;
        }
        if(c == 1 && r == 3) {
            dialog=static_cast<QDialog *>(new w_rpt_report_frame_shift(this));
            dialog->setWindowTitle(text + dialogClassName());//交班"));
            dialog->resize(lds::MAIN_WINDOW_SIZE);
            break;
        }
        if(c == 1 && r == 4) {
            if(backheader::operid_right_code(this, "0702")){//菜品促销报表【 0702 】
                dialog=static_cast<QDialog *>(new w_rpt_report_promotionreport(this));
                dialog->setWindowTitle(text + dialogClassName());//菜品促销报表"));
                dialog->resize(lds::MAIN_WINDOW_SIZE);
            }
            break;
        }


        if(c == 2 && r == 0) {
            if(backheader::operid_right_code(this, "0710")){//日营业情况统计【 0710 】
                dialog=static_cast<QDialog *>(new w_rpt_report_daybusinessreprot(this));
                dialog->setWindowTitle(text + dialogClassName());//日营业情况统计"));
                dialog->resize(lds::MAIN_WINDOW_SIZE);
            }
            break;
        }
        if(c == 2 && r == 1) {
            if(backheader::operid_right_code(this, "0708")){//日月周报表分析【 0708 】
                dialog=static_cast<QDialog *>(new w_rpt_report_daymonweekanalysis(this));
                dialog->setWindowTitle(text + dialogClassName());//日月周报表分析"));
                dialog->resize(lds::MAIN_WINDOW_SIZE);
            }
            break;
        }
        if(c == 2 && r == 2) {
            if(backheader::operid_right_code(this, "0708")){//原料销售流水
                dialog=static_cast<QDialog *>(new w_rpt_report_frame_material_saleflow(this));
                dialog->setWindowTitle(text + dialogClassName());//日月周报表分析"));
                dialog->resize(lds::MAIN_WINDOW_SIZE);
            }
            break;
        }
        if(c == 2 && r == 3) {
            if(backheader::operid_right_code(this, "0720")){//原料毛利报表 【 0720 】
                dialog=static_cast<QDialog *>(new w_rpt_report_directsalegrossprofitreport(this));
                dialog->setWindowTitle(text + dialogClassName());
                dialog->resize(lds::MAIN_WINDOW_SIZE);
            }
            break;
        }
        if(c == 2 && r == 4) {
            if(backheader::operid_right_code(this, "0715")){//服务员提成查询【 0715 】
                dialog=static_cast<QDialog *>(new w_rpt_report_servantpromotquery(this));
                dialog->setWindowTitle(text + dialogClassName());//服务员提成查询"));
                dialog->resize(lds::MAIN_WINDOW_SIZE);
            }
            break;
        }

        if(c == 3 && r == 0) {
            if(backheader::operid_right_code(this, "0703")){//餐桌营业数据统计【 0703 】
                dialog=static_cast<QDialog *>(new w_rpt_report_tablebusinessreport(this));
                dialog->setWindowTitle(text + dialogClassName());//餐桌营业数据统计"));
                dialog->resize(lds::MAIN_WINDOW_SIZE);
            }
            break;
        }
        if(c == 3 && r == 1) {
            if(backheader::operid_right_code(this, "0706")){//服务费与低消查询【 0706 】
                dialog=static_cast<QDialog *>(new w_rpt_report_servicefeelowpinquery(this));
                dialog->setWindowTitle(text + dialogClassName());//服务费与低消查询"));
                dialog->resize(lds::MAIN_WINDOW_SIZE);
            }
            break;
        }
        if(c == 3 && r == 2) {
            if(backheader::operid_right_code(this, "0713")){//时段营业情况分析【 0713 】
                dialog=static_cast<QDialog *>(new w_rpt_report_periodbusinessanalysis(this));
                dialog->setWindowTitle(text + dialogClassName());//时段营业情况分析"));
                dialog->resize(lds::MAIN_WINDOW_SIZE);
            }
            break;
        }
        if(c == 3&& r == 3) {
            if(backheader::operid_right_code(this, "0712")){//品类销售营业分析【 0712 】
                dialog=static_cast<QDialog *>(new w_rpt_report_categorysalesanalysis(this));
                dialog->setWindowTitle(text + dialogClassName());//品类销售营业分析"));
                dialog->resize(lds::MAIN_WINDOW_SIZE);
            }
            break;
        }
        if(c == 3 && r == 4) {
            break;
        }
        break;
        //////////////////////////////////////////////////////////////////////
    case 4:
        if(c == 0 && r == 0) {//外部设备
            dialog=static_cast<QDialog *>(new w_sys_manage_outer_settings(this));
            dialog->resize(lds::MAIN_WINDOW_SIZE);
            dialog->setWindowTitle(text + dialogClassName());
            break;
        }
        if(c == 0 && r == 1) {//收银设置
            dialog=static_cast<QDialog *>(new w_sys_manage_outer_pay_set(this));
            dialog->resize(lds::MAIN_WINDOW_SIZE);
            dialog->setWindowTitle(text + dialogClassName());//收银方式"));

            break;
        }
        if(c == 0 && r == 2) {
            if(backheader::operid_right_code(this, "0106")){//收银方式【 0106 】
                dialog=static_cast<QDialog *>(new w_bt_dish_paymode(this));
                dialog->setWindowTitle(text + dialogClassName());//收银方式"));
            }
            break;
        }
        if(c == 0 && r == 3) {//促销设置
            if(!saas_pay_detail::SaaSFuncCheck(this, "0505"/*促销*/)) {
                break;
            }
            if(backheader::operid_right_code(this, "0242")){//促销设置 0242 】
                dialog=qobject_cast<QDialog *>(new w_sys_manage_promotion_setting(this));
                dialog->setWindowTitle(text + dialogClassName());//促销设置"));
                dialog->resize(lds::MAIN_WINDOW_SIZE);
            }
            break;
        }
        if(c == 0 && r == 4) {//菜品厨打设置
            if(backheader::operid_right_code(this, "0901")){//公司信息【 0901 】
                dialog=static_cast<QDialog *>(new w_sys_manage_companyinformation(this));
                dialog->setWindowTitle(text + dialogClassName());//公司信息"));
            }
            break;
        }

        if(c == 3 && r == 0) {
            if(backheader::operid_right_code(this, "0903")){//操作员资料维护【 0903 】
                dialog=static_cast<QDialog *>(new w_sys_manage_operatorinformation(this));
                dialog->setWindowTitle(text + dialogClassName());//操作员资料维护"));
            }
            break;
        }
        if(c == 3 && r == 1) {
            if(backheader::operid_right_code(this, "0904")){//操作员权限维护【 0904 】
                dialog=static_cast<QDialog *>(new w_sys_manage_operatorright(this));
                dialog->setWindowTitle(text + dialogClassName());//操作员权限维护"));
                dialog->resize(lds::MAIN_WINDOW_SIZE);
            }
            break;
        }
        if(c == 3 && r == 2) {
            if(backheader::operid_right_code(this, "0905")){//数据库【 0905 】
                dialog=static_cast<QDialog *>(new w_sys_manage_sqlcopy(this));
                dialog->resize(lds::MAIN_WINDOW_SIZE);
                dialog->setWindowTitle(text + dialogClassName());//数据库"));
            }
            break;
        }
        if(c == 3 && r == 3) {//在线升级
            {
                QString errstring;
                //!等待
                lds_messagebox_loading_show loading(this, "WAITING...", "WAITING...");
                loading.show();
                //!~等待
                if(false == fastfd_update(this).check_need_update(&errstring)) {
                    lds_messagebox::warning(this, MESSAGE_TITLE_VOID, errstring);
                    return;
                }
            }
            if(1 == lds_messagebox::question(this, MESSAGE_TITLE_VOID, tr("有新版本更新"), tr("确认"), tr("取消"))) {
                return;
            }

            lds_dialog_input inputdialog(this);
            inputdialog.ui->label->setText(tr("密码") + ":");
            inputdialog.ui->lineEdit->setEchoMode(QLineEdit::Password);
#ifdef QT_DEBUG
            inputdialog.ui->lineEdit->setText(n_func::f_get_godpassword());
#endif
            inputdialog.setWindowTitle(tr("有新版本更新"));
            if(QDialog::Accepted != lds_roundeddialog_rect_align(&inputdialog).exec()){
                return;
            }
            if(inputdialog.ui->lineEdit->text()!=n_func::f_get_godpassword()){
                lds_messagebox::warning(this, MESSAGE_TITLE_VOID, tr("密码错误"));
                return;
            }
            public_sql::softUpdate();
            this->reject();
        }
        return;
        //////////////////////////////////////////////////////////////////////
    case 5:
        if(r == 0 && c ==0) {//云设置
            if(saas_pay_detail::isSaaSAA()) {
                dialog=static_cast<QDialog *>(new w_sys_manage_cloudsync_with_time(this));
            } else
            {
                dialog=static_cast<QDialog *>(new w_sys_manage_cloudsync_cloud2(this));
            }
            dialog->setWindowTitle(text + dialogClassName());//云同步"));
            dialog->resize(dialog->size());
            break;
        }
        if(r == 1 && c ==0) {//采购申请
            dialog= static_cast<QDialog *>(new material_purchase_apply(this));
            dialog->setWindowTitle(text + dialogClassName());//云同步"));
            dialog->resize(lds::MAIN_WINDOW_SIZE);
            break;
        }
        if(r == 2 && c ==0) {//门店入库
            dialog= static_cast<QDialog *>(new material_purchase_in_storage(this));
            dialog->setWindowTitle(text + dialogClassName());//云同步"));
            dialog->resize(lds::MAIN_WINDOW_SIZE);
            break;
        }
        if(r == 3 && c ==0) {//门店出库
            dialog= static_cast<QDialog *>(new material_loss_apply(this));
            dialog->setWindowTitle(text + dialogClassName());//云同步"));
            dialog->resize(lds::MAIN_WINDOW_SIZE);
            break;
        }
        break;
    }

    if(dialog){
        int ret = -1;
        if(dialog->size() == lds::MAIN_WINDOW_SIZE) {
            dialog->move(0,0);
            lds::setwflagFrameless(dialog);
            ret = dialog->exec();
        } else {
            ret = lds_roundeddialog_rect_align(dialog).exec();
        }
        delete dialog;
        dialog=0;
        if(public_sql::Reboot == ret) {
            this->reject();
            public_sql::save_login->hideall_and_back_login();
        }
    }
}

QTableView *back_view_dialog::creat_tableview()
{
    back_view_tableview *tableview = new back_view_tableview;
    itemDelegate *d = new itemDelegate;
    d->keepConnect(tableview);
    tableview->setItemDelegate(d);
    tableview->setStyleSheet("border:0;background:transparent;selection-background-color: transparent;");
    tableview->horizontalHeader()->hide();
    tableview->verticalHeader()->hide();
    lds::setHeaderResizeMode(tableview->verticalHeader(), QHeaderView::Stretch);
    lds::setHeaderResizeMode(tableview->horizontalHeader(), QHeaderView::Stretch);
    tableview->setSelectionMode(QTableView::SingleSelection);
    tableview->setEditTriggers(QTableView::NoEditTriggers);
    tableview->setShowGrid(false);
    connect(tableview, SIGNAL(clicked(QModelIndex)),this,SLOT(topop(QModelIndex)));
    return tableview;
}

back_view_dialog::itemDelegate::~itemDelegate()
{

}

void back_view_dialog::itemDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    //
    //    painter->setRenderHints(QPainter::Antialiasing);
    int pwidth = lds::MAIN_WINDOW_SIZE.width()/25;

    int border = 3;
    QStringList texts = index.data().toString().split("\t", QString::SkipEmptyParts);
    QString text = texts.value(2);
    int dir = texts.value(0).toInt();

    QString imagepath = texts.value(1);
    QRect rect = option.rect.adjusted(border,border,-border,-border);
    QFont f = painter->font();
    QRect rect_text;
    QRect pixmap_rect;
    QSize pixmap_size;
    if(dir == -1 || texts.isEmpty()){
        return;
    }
    //1
    if(option.state & QStyle::State_Selected) {
        imagepath += "p";
        painter->fillRect(rect, QColor("#ff650b"));
    } else {
        painter->setPen(*borderColor);
        painter->drawRect(rect);
        painter->setPen(Qt::NoPen);
        painter->fillRect(rect, *bgColor);
    }
    //2
    painter->setPen(*textColor);
    f.setPointSize(13);
    painter->setFont(f);

    //垂直
    if(dir == 1){
        QSvgRenderer svg_render;
        svg_render.load(imagepath+".svg");
        rect_text = QRect(rect.x() , rect.y() + rect.height()*2/3, rect.width(), rect.height()/3);
        //若是文本都绘制不完整的话，则不会绘制图片
        if(false == rect_text.contains(painter->boundingRect(rect_text, (Qt::AlignCenter|Qt::TextWordWrap), text))) {
            painter->drawText(rect, (Qt::AlignCenter|Qt::TextWordWrap), text);
            return;
        }

        pixmap_rect = QRect(rect.x() , rect.y() + rect.height() /6, rect.width(), rect.height()/3);
        pixmap_size = QSize(pwidth, pwidth);
        pixmap_rect = QStyle::alignedRect(Qt::LeftToRight, Qt::AlignCenter, pixmap_size, pixmap_rect);

        if(!back_view_svg_render::svg_render_by_rect(&svg_render, painter, pixmap_rect, rect)) {
            if(!QPixmap(imagepath).isNull())
                painter->drawPixmap(pixmap_rect.topLeft(), QPixmap(imagepath).scaled(pixmap_size));
        }
        painter->drawText(rect_text, (Qt::AlignHCenter|Qt::AlignTop|Qt::TextWordWrap), text);
    }

    if(dir == 0){
        QSvgRenderer svg_render;
        svg_render.load(imagepath+".svg");
        rect_text = QRect(rect.x() +rect.height() , rect.y(), rect.width()-rect.height(), rect.height());
        //若是文本都绘制不完整的话，则不会绘制图片
        if(false == rect_text.contains(painter->boundingRect(rect_text, (Qt::AlignCenter|Qt::TextWordWrap), text))) {
            painter->drawText(rect, (Qt::AlignCenter|Qt::TextWordWrap), text);
            return;
        }

        pixmap_rect = QRect(rect.x() , rect.y(), rect.height(), rect.height());
        pixmap_size = QSize(pwidth, pwidth);
        pixmap_rect = QStyle::alignedRect(Qt::LeftToRight, Qt::AlignCenter, pixmap_size, pixmap_rect);
        if(!back_view_svg_render::svg_render_by_rect(&svg_render, painter, pixmap_rect, rect)) {
            if(!QPixmap(imagepath).isNull())
                painter->drawPixmap(pixmap_rect.topLeft(), QPixmap(imagepath).scaled(pixmap_size));
        }
        painter->drawText(rect_text, (Qt::AlignVCenter|Qt::AlignLeft|Qt::TextWordWrap), text);
    }
}

void back_view_dialog::itemDelegate::keepConnect(back_view_tableview *tableview)
{
    bgColor = &tableview->bgColor;
    textColor = &tableview->textColor;
    borderColor = &tableview->borderColor;
}



void back_view_tableview::setbgColor(const QColor &color)
{
    bgColor = color;
}

QColor back_view_tableview::getbgColor() const
{
    return bgColor;
}

void back_view_tableview::settextColor(const QColor &color)
{
    textColor = color;
}

QColor back_view_tableview::gettextColor() const
{
    return textColor;
}

void back_view_tableview::setborderColor(const QColor &color)
{
    borderColor = color;
}

QColor back_view_tableview::getborderColor() const
{
    return borderColor;
}

back_view_tableview::back_view_tableview(QWidget *parent):QTableView(parent)
{
    this->setProperty("outer_stylesheet", "back_view_tableview");
}

