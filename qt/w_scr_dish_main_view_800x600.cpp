#include "w_scr_dish_main_view_800x600.h"
#include "ui_w_scr_dish_main_view_800x600.h"
#include <QTimer>
#include "lds_menu.h"
#include "lds_messagebox.h"
#include "w_scr_dish_main_bar_cancel.h"
#include "w_scr_dish_main_bar_cancel_win.h"
#include "lds_dialog_input.h"
#include "fexpandmain_2.h"
#include "ftableview_delegate.h"

w_scr_dish_main_view_800x600::w_scr_dish_main_view_800x600(QWidget *parent) :
    w_scr_dish_main(Q000, "", windowTypeKuaiCan, parent),
    ui(new Ui::w_scr_dish_main_view_800x600)
{
    ui->setupUi(this);
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
    ui->frame_deskinfo->resize(lds::PAY_LEFT_SPACE, 80);
    ui->frame_deskinfo->move(0, 0);
    ui->frame_deskinfo->setTitleColor(&titleColor);
    ui->frame_deskinfo->enableBottomBorder();

    ui->frame_bottom->resize(lds::MAIN_WINDOW_SIZE.width() - lds::PAY_LEFT_SPACE - ftableview_delegate::CELL_BORDER_SPACE, lds::QT_FIXED_HEIGHT_BOTTOM);
    ui->frame_bottom->move(lds::PAY_LEFT_SPACE + ftableview_delegate::CELL_BORDER_SPACE, lds::MAIN_WINDOW_SIZE.height() - lds::QT_FIXED_HEIGHT_BOTTOM);

    ui->frame_dish_bottom2->resize(lds::PAY_LEFT_SPACE,  lds::QT_FIXED_HEIGHT_BOTTOM);
    ui->frame_dish_bottom2->move(0, lds::MAIN_WINDOW_SIZE.height() - ui->frame_bottom->height());//frame_dish_bottom 的背景色已经注册了，改名
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
    hideWidget(ui->frame);

    ui->pushButton_language->initKeys_conf();
    loadData();
    connect(ui->label_total, SIGNAL(clicked()), this, SLOT(toSwitchSkin()));

    connect(public_sql::save_login_object(), SIGNAL(signal_TelNoComing(QString)), this, SLOT(toTelComing(QString)));
}

w_scr_dish_main_view_800x600::~w_scr_dish_main_view_800x600()
{
    delete ui;
}

void w_scr_dish_main_view_800x600::menuhide()
{
    if(ui->pushButton_other->menu()) {
        ui->pushButton_other->menu()->hide();
    }
}

void w_scr_dish_main_view_800x600::updateTakebillState()
{
    lds_query query;
    query.execSelect("select count(0) from cey_u_hungbill where dt_operdate >= curdate() ");
    query.next();
    ui->pushButton_takebill->set_rt_checked(query.recordValue(0).toInt() > 0);
}

void w_scr_dish_main_view_800x600::updateTakebillDesc()
{
    if(tablemodel->rowCount() == 0) {
        ui->pushButton_takebill->setText(tr("取单"));
    } else {
        ui->pushButton_takebill->setText(tr("挂单"));
    }
}

void w_scr_dish_main_view_800x600::retranslateView()
{
    ui->retranslateUi(this);
    ui->frame_dish_bottom2->retranslateView();
}

void w_scr_dish_main_view_800x600::loadData()
{
    w_scr_dish_main::loadData();
    //
    ui->frame_dish_bottom2->setup(ch_tableno, ui->tableView->verticalScrollBar());
    {//其他f
        showWidget(ui->frame);
        lds_menu *menu=new lds_menu(this);
        QWidgetAction *widgetaction=new QWidgetAction(ui->pushButton_other);
        widgetaction->setDefaultWidget(ui->frame);
        menu->addAction(widgetaction);
        ui->pushButton_other->setMenu(menu);
    }
    {//编码
        lds_menu *menu=new lds_menu(this);
        ui->pushButton_bianma->setMenu(menu);
        connect(menu,SIGNAL(aboutToShow()), this,SLOT(showBianMa()));
    }
    //
    if(lds::sysconf->value("system_setting/kuaican_mpay_enabled", false).toBool()) {
        ui->pushButton_pay->enableLongPress(true);
        ui->pushButton_pay->setText(lds::sysconf->value("w_scr_dish_main/paymode", 1).toInt() == 0 ? tr("结算") : tr("会员付款"));
    }
    {
        connect(ui->pushButton_allbillcancel,SIGNAL(clicked()),this,SLOT(toAllbillcancel()));
        connect(ui->pushButton_change,SIGNAL(clicked()),this,SLOT(tochange()));
        //        connect(ui->pushButton_del,SIGNAL(clicked()),this,SLOT(toquitdish()));
        connect(ui->pushButton_exitbill,SIGNAL(clicked()),this,SLOT(toExitbill()));

        //        connect(ui->pushButton_detail,SIGNAL(clicked()), this, SLOT(todetail()));
        connect(ui->pushButton_present,SIGNAL(clicked()),this,SLOT(topresent()));
        connect(ui->pushButton_mhangpay,SIGNAL(clicked()),this,SLOT(tomhangpay()));
        connect(ui->pushButton_takebill,SIGNAL(clicked()),this,SLOT(toTakebill()));
        connect(ui->pushButton_takeweight,SIGNAL(clicked()),this,SLOT(toTakeweight()));

        connect(ui->pushButton_pay,SIGNAL(clicked()),this,SLOT(topaybill()));
        connect(ui->pushButton_pay,SIGNAL(timeout()),this,SLOT(topaybillSelectPayMode()));
        connect(ui->pushButton_tuichu,SIGNAL(clicked()),this,SLOT(toTuichu()));
        connect(ui->pushButton_member,SIGNAL(clicked()),this,SLOT(toMRead()));
        connect(ui->frame_dish_bottom2, SIGNAL(signal_waimai()),this,SLOT(towaimai()));
        connect(ui->frame_dish_bottom2, SIGNAL(signal_waimaipay()),this,SLOT(towaimaipay()));
        connect(ui->frame_dish_bottom2, SIGNAL(signal_weixinwaimai()),this,SLOT(toweixinwaimai()));
        connect(ui->frame_dish_bottom2, SIGNAL(signal_weixinquitbill()),this,SLOT(toweixinquitbill()));
        connect(ui->frame_dish_bottom2, SIGNAL(signal_weixinbill_import()),this,SLOT(toweixinbill_import()));
        connect(ui->pushButton_wine, SIGNAL(clicked()), this, SLOT(toWine()));
        connect(ui->frame_dish_bottom2->desk, SIGNAL(clicked()), this, SLOT(toRestaurant()));
        connect(ui->frame_dish_bottom2->waidai, SIGNAL(clicked()), this, SLOT(toWaiDai()));
        ui->frame_dish_bottom2->addMenuInitAndShow();
    }
    {
        connect(ui->pushButton_sys_yingyedanju,SIGNAL(clicked()),this,SLOT(tosysyingyedanju()));
        connect(ui->pushButton_language,SIGNAL(languageChanged()),this,SLOT(toLanguageSwitch()));
        connect(ui->pushButton_language,SIGNAL(timeout()),this,SLOT(toLanguageSet()));
        connect(ui->pushButton_sys_yingyeqingkuang,SIGNAL(clicked()),this,SLOT(tosysyingyeqingkuang()));
        connect(ui->pushButton_sys_jiaoban,SIGNAL(clicked()),this,SLOT(tosysjiaoban()));
        connect(ui->pushButton_sys_chongzhi,SIGNAL(clicked()),this,SLOT(tosyschongzhi()));
        connect(ui->pushButton_sys_qukuan,SIGNAL(clicked()),this,SLOT(tosysqukuan()));
        connect(ui->pushButton_sys_xiugaimima,SIGNAL(clicked()),this,SLOT(tosysxiugaimima()));
        connect(ui->pushButton_sys_switchback,SIGNAL(clicked()),this,SLOT(tosysswitchback()));
        connect(ui->pushButton_sys_kaiqianxiang,SIGNAL(clicked()),this,SLOT(tosyskaiqianxiang()));
        connect(ui->pushButton_changyong,SIGNAL(clicked()), this ,SLOT(tochangyongcai()));
        connect(ui->pushButton_sys_suoping, SIGNAL(clicked()),this,SLOT(tosyssuoping()));
        connect(ui->pushButton_sys_guqing, SIGNAL(clicked()),this,SLOT(tosysguqing()));
        connect(ui->lineEdit_scan, SIGNAL(signal_unvoid_returnPressed()), this, SLOT(toScanbar()));
        connect(ui->lineEdit_scan, SIGNAL(signal_void_returnPressed()), this, SLOT(topaybill()));
        connect(ui->pushButton_tel_coming, SIGNAL(clicked()), this, SLOT(toTelComing()));
    }
}

void w_scr_dish_main_view_800x600::updatememberchecked()
{
    ui->pushButton_member->set_rt_checked(tablemodel->vch_membernod);
    w_scr_dish_main::updatememberchecked();
}

void w_scr_dish_main_view_800x600::toWine()
{
    fexpandmain_2::wineTake(this);
}

