#include "w_scr_dish_main_table_800x600.h"
#include "ui_w_scr_dish_main_table_800x600.h"
#include <QTimer>
#include "lds_menu.h"
#include "lds_roundeddialog_rect_pop.h"
#include "n_func.h"
#include "w_scr_dish_pay_widget.h"
#include "back_view_dialog.h"
#include "public_sql.h"
#include "lds_dialog_input.h"
#include "lds_dialog_input_double.h"
#include "ui_lds_dialog_input_dialog_double.h"
#include "w_scr_dish_select_operid.h"
#include "public_printby_ch_billno.h"
#include "ftableview_delegate.h"

w_scr_dish_main_table_800x600::w_scr_dish_main_table_800x600(const QString &ch_tableno, const QString &ch_billno, const QString &orderSn, QWidget *parent, const w_m_member_telComing_table::telComingData &member_from_tel)
    :  w_scr_dish_main(ch_tableno, ch_billno, windowTypeZhongCan, parent)
{
    ui = new Ui::w_scr_dish_main_table_800x600;
    ui->setupUi(this);

    this->orderSn = orderSn;
    resize(lds::MAIN_WINDOW_SIZE);
    loadControl(
                ui->tableView_dish,
                ui->tableView_type,
                ui->tableView_subtype,
                ui->tableView,

                ui->label_logo,
                ui->label_text,
                ui->label_num,
                ui->label_total,
                ui->lineEdit_scan,
                ui->pushButton_pay
                );
    //

    //widget
    ui->frame_deskinfo->setTitleColor(&titleColor);
    ui->frame_deskinfo->enableBottomBorder();
    ui->frame_deskinfo->setGeometry(0, 0, lds::PAY_LEFT_SPACE, 150);

    ui->frame_bottom->resize(lds::MAIN_WINDOW_SIZE.width() - lds::PAY_LEFT_SPACE - ftableview_delegate::CELL_BORDER_SPACE, lds::QT_FIXED_HEIGHT_BOTTOM);
    ui->frame_bottom->move(lds::PAY_LEFT_SPACE + ftableview_delegate::CELL_BORDER_SPACE, lds::MAIN_WINDOW_SIZE.height() - lds::QT_FIXED_HEIGHT_BOTTOM);

    ui->frame_dish_bottom2->resize(lds::PAY_LEFT_SPACE,  lds::QT_FIXED_HEIGHT_BOTTOM);
    ui->frame_dish_bottom2->move(0, lds::MAIN_WINDOW_SIZE.height() - lds::QT_FIXED_HEIGHT_BOTTOM);//frame_dish_bottom 的背景色已经注册了，改名
    //    ui->frame_dish_bottom2->hide();

    ui->tableView->resize(lds::PAY_LEFT_SPACE,  lds::MAIN_WINDOW_SIZE.height() - ui->frame_deskinfo->height() - ui->frame_dish_bottom2->height());
    ui->tableView->move(0, ui->frame_deskinfo->height());

    int dish_width = lds::MAIN_WINDOW_SIZE.width() - lds::PAY_LEFT_SPACE;
    ui->tableView_type->move(lds::PAY_LEFT_SPACE, 0);
    ui->tableView_type->resize(dish_width, lds::QT_FIXED_HEIGHT_BOTTOM);

    ui->tableView_subtype->move(lds::PAY_LEFT_SPACE, lds::QT_FIXED_HEIGHT_BOTTOM);
    ui->tableView_subtype->resize(dish_width, lds::QT_FIXED_HEIGHT_BOTTOM);

    ui->tableView_dish->move(lds::PAY_LEFT_SPACE, 2 * lds::QT_FIXED_HEIGHT_BOTTOM);
    ui->tableView_dish->resize(dish_width, lds::MAIN_WINDOW_SIZE.height() - 3 * lds::QT_FIXED_HEIGHT_BOTTOM);
    //label_logo
    ui->label_logo->clear();
    ui->label_text->clear();
    ui->label_num->clear();
    ui->label_total->setText("");
    ui->tableView->setFocusPolicy(Qt::NoFocus);
    ui->tableView_type->setFocusPolicy(Qt::NoFocus);
    ui->tableView_subtype->setFocusPolicy(Qt::NoFocus);
    ui->tableView_dish->setFocusPolicy(Qt::NoFocus);
    ui->label_total->setFocusPolicy(Qt::NoFocus);
    hideWidget(ui->frame_function);
    hideWidget(ui->frame_cuicai);
    hideWidget(ui->frame_jiaodan);
    hideWidget(ui->frame_other_menu);

    loadData();

    if(member_from_tel.vch_memberno.length() > 0) {
        fexpandmain_2::to_m_read(this, tablemodel,  member_from_tel.vch_memberno);
        updatememberchecked();
    }
    connect(ui->label_total, SIGNAL(clicked()), this, SLOT(toSwitchSkin()));
}

w_scr_dish_main_table_800x600::~w_scr_dish_main_table_800x600()
{
    delete ui;
}

void w_scr_dish_main_table_800x600::menuhide()
{
    ui->pushButton_other->menu()->hide();
    ui->pushButton_bianma->menu()->hide();
}

void w_scr_dish_main_table_800x600::updateTakebillState()
{
}

void w_scr_dish_main_table_800x600::updateTakebillDesc()
{
}

void w_scr_dish_main_table_800x600::retranslateView()
{
    ui->retranslateUi(this);
    ui->frame_dish_bottom2->retranslateView();
}

void w_scr_dish_main_table_800x600::loadData()
{
    w_scr_dish_main::loadData();
    loadSn(orderSn);
    //
    ui->frame_dish_bottom2->setup(ch_tableno, ui->tableView->verticalScrollBar());
    {//frame_cuicai
        lds_menu *menu=new lds_menu(this);
        ui->pushButton_cuicai->setMenu(menu);
        menu->setPopWidget(ui->frame_cuicai);
    }
    {//frame_jiaodan
        lds_menu *menu=new lds_menu(this);
        ui->pushButton_jiaodan->setMenu(menu);
        menu->setPopWidget(ui->frame_jiaodan);
    }
    {//编码
        lds_menu *menu=new lds_menu(this);
        ui->pushButton_bianma->setMenu(menu);
        connect(menu,SIGNAL(aboutToShow()), this,SLOT(showBianMa()));
    }
    {
        lds_menu *menu=new lds_menu(this);
        ui->pushButton_other->setMenu(menu);
        menu->setPopWidget(ui->frame_other_menu);
    }

    connect(ui->pushButton_tuichu,SIGNAL(clicked()),this,SLOT(toTuichu()));
    connect(ui->pushButton_change,SIGNAL(clicked()),this,SLOT(tochange()));
    //    connect(ui->pushButton_detail,SIGNAL(clicked()), this, SLOT(todetail()));
    connect(ui->pushButton_present,SIGNAL(clicked()),this,SLOT(topresent()));
    connect(ui->label_btn_vch_member,SIGNAL(clicked()),this,SLOT(toReadMember()));
    connect(ui->label_vch_member, SIGNAL(clicked()), this, SLOT(tochangeMemberShowFormat()));
    connect(ui->pushButton_paysomedish, SIGNAL(clicked()), this, SLOT(toPaySomeDish()));
    connect(ui->pushButton_print, SIGNAL(clicked()), this, SLOT(toprint()));
    connect(ui->pushButton_reprint, SIGNAL(clicked()), this, SLOT(toreprint()));


    connect(ui->pushButton_guaqi, SIGNAL(clicked()), this, SLOT(toguaqi()));connect(ui->pushButton_guaqi, SIGNAL(clicked()), this, SLOT(to_menu_special_reject()));


    connect(ui->pushButton_cuicai1, SIGNAL(clicked()), this, SLOT(tocuicaipiece()));connect(ui->pushButton_cuicai1, SIGNAL(clicked()), this, SLOT(to_menu_special_reject()));
    connect(ui->pushButton_cuicai2, SIGNAL(clicked()), this, SLOT(tocuicaitable()));connect(ui->pushButton_cuicai2, SIGNAL(clicked()), this, SLOT(to_menu_special_reject()));
    connect(ui->pushButton_cuicai3, SIGNAL(clicked()), this, SLOT(tocuicaitype()));connect(ui->pushButton_cuicai3, SIGNAL(clicked()), this, SLOT(to_menu_special_reject()));

    connect(ui->pushButton_jiaodan1, SIGNAL(clicked()), this, SLOT(tojiaodanpiece()));connect(ui->pushButton_jiaodan1, SIGNAL(clicked()), this, SLOT(to_menu_special_reject()));
    connect(ui->pushButton_jiaodan2, SIGNAL(clicked()), this, SLOT(tojiaodantable()));connect(ui->pushButton_jiaodan2, SIGNAL(clicked()), this, SLOT(to_menu_special_reject()));
    connect(ui->pushButton_jiaodan3, SIGNAL(clicked()), this, SLOT(tojiaodantype()));connect(ui->pushButton_jiaodan3, SIGNAL(clicked()), this, SLOT(to_menu_special_reject()));


    connect(ui->pushButton_dishtmp, SIGNAL(clicked()), this,SLOT(toDishTmp()));
    //    connect(ui->pushButton_quitdish, SIGNAL(clicked()), this, SLOT(toquitdish()));
    connect(ui->pushButton_changyong,SIGNAL(clicked()), this ,SLOT(tochangyongcai()));
    connect(ui->lineEdit_scan, SIGNAL(signal_unvoid_returnPressed()), this, SLOT(toScanbar()));
    connect(ui->lineEdit_scan, SIGNAL(signal_void_returnPressed()), this, SLOT(topaybill()));
    connect(ui->pushButton_pay, SIGNAL(clicked()), this, SLOT(topaybill()));
    connect(ui->frame_dish_bottom2->desk, SIGNAL(clicked()), this, SLOT(toTuichu()));
    connect(ui->frame_dish_bottom2->waidai, SIGNAL(clicked()), this, SLOT(toWaiDai()));
    connect(ui->frame_dish_bottom2->waimai, SIGNAL(clicked()), this, SLOT(towaimai()));

    connect(ui->label_ch_tableno, SIGNAL(clicked()), this, SLOT(tochangeTableShowFormat()));
    connect(ui->label_btn_vch_waiter, SIGNAL(clicked()), this, SLOT(to_input_vch_waiter()));
    connect(ui->label_btn_int_person, SIGNAL(clicked()), this, SLOT(to_input_int_person()));
    connect(ui->label_btn_vch_memo, SIGNAL(clicked()), this, SLOT(to_input_vch_memo()));

    connect(tablemodel, SIGNAL(signals_num_total_change(QString,QString)), this, SLOT(to_switch_print_reprint()));
    ///特殊的整合，特殊会切换为收起
//    connect(ui->pushButton_function,SIGNAL(clicked()),this,SLOT(tofunctionshow()));
    ///~特殊的整合，特殊会切换为收起
    ///
    ui->label_vch_waiter->setText(tablemodel->get_cey_u_table_value(ch_billno, "vch_waiter"));
    ui->label_int_person->setText(tablemodel->get_cey_u_table_value(ch_billno, "int_person"));
    ui->label_vch_memo->setText(tablemodel->get_cey_u_table_value(ch_billno, "vch_memo"));

    table_format_index = lds::sysconf->value("w_scr_dish_main_table_800x600/table_format_index", 0).toInt();
    member_format_index = lds::sysconf->value("w_scr_dish_main_table_800x600/member_format_index", 0).toInt();
    changeTableShowFormat();

}

void w_scr_dish_main_table_800x600::updatequitDish(const QModelIndex &current, const QModelIndex &)
{
    //    int row = current.row();
    //    if(row < 0){
    //        return;
    //    }
    //    if(tablemodel->opera(row) == lds_model_sqltablemodel::OpInsert) {
    //        ui->pushButton_quitdish->setText(tr("删菜"));
    //    } else {
    //        ui->pushButton_quitdish->setText(tr("退菜"));
    //    }
}

void w_scr_dish_main_table_800x600::updatememberchecked()
{
    changeMemberShowFormat();
    w_scr_dish_main::updatememberchecked();
}

void w_scr_dish_main_table_800x600::tofunctionshow()
{
    if(menu_special == 0) {
        QDialog *dialog = new QDialog;
        dialog->setStyleSheet("QDialog{border:2px solid #ff650b;}");
        dialog->setFixedSize(120, 200);
        showWidget(ui->frame_function);
        ui->frame_function->setParent(dialog);
        ui->frame_function->move(2, 2);
        ui->frame_function->setFixedSize(116, 196);
//        ui->pushButton_function->setCheckable(true);
        menu_special = new lds_roundeddialog_rect_pop(dialog, this, dialog->windowFlags() | Qt::FramelessWindowHint);
        menu_special->setbgColor(QColor(1,1,1,1));//window 全透明会真的穿透
    }

//    menu_special->updatePos(ui->pushButton_function);
    menu_special->exec();
//    ui->pushButton_function->setChecked(false);
}

void w_scr_dish_main_table_800x600::tochangeTableShowFormat()
{
    table_format_index++;
    table_format_index =  table_format_index % 3;
    changeTableShowFormat();
}

void w_scr_dish_main_table_800x600::tochangeMemberShowFormat()
{
    member_format_index++;
    member_format_index =  member_format_index % 3;
    changeMemberShowFormat();
}

void w_scr_dish_main_table_800x600::to_input_vch_waiter()
{
    w_scr_dish_select_operid dialog(this);
    dialog.setWindowTitle(ui->label_btn_vch_waiter->text());
    dialog.setOperid(ui->label_vch_waiter->text());
    if(QDialog::Accepted == lds_roundeddialog_rect_align(&dialog).exec()) {
        ui->label_vch_waiter->setText(dialog.selectOperid());
        lds_query query;
        query.execUpdate("cey_u_table", "vch_waiter", ui->label_vch_waiter->text(), qrtEqual("ch_billno", ch_billno));
    }
}

void w_scr_dish_main_table_800x600::to_input_int_person()
{
    lds_dialog_input_double dialog(this);
    dialog.setWindowTitle(MESSAGE_TITLE_VOID);
    dialog.ui->label->setText(ui->label_btn_int_person->text());
    dialog.ui->lineEdit->setDecimals(0);
    dialog.ui->lineEdit->setValue(ui->label_int_person->text().toInt());
    if(QDialog::Accepted == lds_roundeddialog_rect_align(&dialog).exec()) {
        ui->label_int_person->setNum(dialog.ui->lineEdit->value());
        lds_query query;
        query.execUpdate("cey_u_table", "int_person", ui->label_int_person->text(), qrtEqual("ch_billno", ch_billno));
    }
}

void w_scr_dish_main_table_800x600::to_input_vch_memo()
{
    lds_dialog_input dialog(this);
    dialog.setWindowTitle(MESSAGE_TITLE_VOID);
    dialog.ui->label->setText(ui->label_btn_vch_memo->text());
    dialog.ui->lineEdit->setText(ui->label_vch_memo->text());
    dialog.hideKey();
    if(QDialog::Accepted == lds_roundeddialog_rect_align(&dialog).exec()) {
        ui->label_vch_memo->setText(dialog.ui->lineEdit->text());
        lds_query query;
        query.execUpdate("cey_u_table", "vch_memo", ui->label_vch_memo->text(), qrtEqual("ch_billno", ch_billno));
    }
}

void w_scr_dish_main_table_800x600::to_menu_special_reject()
{
    if(menu_special) {
        menu_special->reject();
    }
}

void w_scr_dish_main_table_800x600::to_switch_print_reprint()
{
    lds::hideWidget(ui->pushButton_reprint, tablemodel->rowCount() == 0 || tablemodel->model_is_dirty());
    lds::hideWidget(ui->pushButton_print, !ui->pushButton_reprint->isHidden());
}

void w_scr_dish_main_table_800x600::toreprint()
{
    public_printby_ch_billno printer(ch_billno);
    switch(lds_messagebox::question(this, MESSAGE_TITLE_VOID, tr("预打单"), tr("打印并返回桌台"), tr("打印"), tr("取消"))) {
    case 0:
        printer.toprint_preprint(printer.defpath(), tr("预打单"));
        public_sql::preprinttablemap.insert(ch_billno, true);
        this->reject();
        break;
    case 1:
        printer.toprint_preprint(printer.defpath(), tr("预打单"));
        public_sql::preprinttablemap.insert(ch_billno, true);
        break;
    case 2:
        break;
    }
}

void w_scr_dish_main_table_800x600::toReadMember()
{
    toMRead();
    changeMemberShowFormat();
}

void w_scr_dish_main_table_800x600::changeTableShowFormat()
{
    switch(table_format_index) {
    case 1://name
        ui->label_ch_tableno->setText(tablemodel->get_cey_bt_table_value(ch_tableno, "vch_tablename"));
        break;
    case 2://name(no)
        ui->label_ch_tableno->setText(tablemodel->get_cey_bt_table_value(ch_tableno, "vch_tablename") + "(" + ch_tableno + ")");
        break;
    default://no
        ui->label_ch_tableno->setText(ch_tableno);
        break;
    }
    lds::sysconf->setValue("w_scr_dish_main_table_800x600/table_format_index", table_format_index);
}

void w_scr_dish_main_table_800x600::changeMemberShowFormat()
{
    QString vch_memberno= tablemodel->vch_membernod.vch_memberno;
    QString vch_member = tablemodel->vch_membernod.vch_member;


    ui->label_vch_member->setText("");
    if(vch_memberno.length() == 0)
        return;
    if(tablemodel->vch_membernod.ch_member_belong_state == "Y") {
        ui->label_vch_member->setStyleSheet("color:#ff650b");
    } else {
        ui->label_vch_member->setStyleSheet("");
    }
    switch(member_format_index) {
    case 1://name
        ui->label_vch_member->setText(vch_member);
        break;
    case 2://name(no)
        ui->label_vch_member->setText(vch_member + "(" + vch_memberno + ")");
        break;
    default://no
        ui->label_vch_member->setText(vch_memberno);
        break;
    }
    lds::sysconf->setValue("w_scr_dish_main_table_800x600/member_format_index", member_format_index);
}
