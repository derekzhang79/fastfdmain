#include "fexpandmain_dish_btn.h"
#include <QtDebug>
#include <QScrollBar>
#include <QWidgetAction>
#include "lds_menu.h"
#include "ui_waimai_menu.h"
#include "w_scr_dish_main.h"


fexpandmain_dish_btn::fexpandmain_dish_btn(QWidget *parent) :
    QFrame(parent)
{
    hlayout = new QHBoxLayout;
    this->setLayout(hlayout);
    hlayout->setContentsMargins(4, 4, 0, 4);
    hlayout->setSpacing(2);
    vbar = 0;

    up = new QPushButton(this);
    up->setFocusPolicy(Qt::NoFocus);
    up->setProperty("outer_stylesheet", "pushbutton_bottom");
    up->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    up->setIcon(QPixmap(":/image/btn_icon/28.png"));
    up->setIconSize(QSize(40, 40));

    down = new QPushButton(this);
    down->setFocusPolicy(Qt::NoFocus);
    down->setProperty("outer_stylesheet", "pushbutton_bottom");
    down->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    down->setIcon(QPixmap(":/image/btn_icon/27.png"));
    down->setIconSize(QSize(40, 40));

    waidai = new QPushButton(this);
    waidai->setFocusPolicy(Qt::NoFocus);
    waidai->setProperty("outer_stylesheet", "pushbutton_bottom");
    waidai->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    waidai->setIcon(QPixmap(":/image/btn_icon/25.png"));
    waidai->setIconSize(QSize(40, 40));

    waimai = new QPushButton(this);
    waimai->setFocusPolicy(Qt::NoFocus);
    waimai->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    waimai->setIcon(QPixmap(":/image/btn_icon/24.png"));
    waimai->setIconSize(QSize(40, 40));

    desk = new QPushButton(this);
    desk->setFocusPolicy(Qt::NoFocus);
    desk->setProperty("outer_stylesheet", "pushbutton_bottom");
    desk->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    desk->setIcon(QPixmap(":/image/btn_icon/26.png"));
    desk->setIconSize(QSize(40, 40));

    setup(QList<QPushButton*> ()  << desk << waidai << waimai << up << down );
    retranslateView();
}

fexpandmain_dish_btn::~fexpandmain_dish_btn()
{
    qDebug() << __FUNCTION__;
}

void fexpandmain_dish_btn::setup(const QStringList &list)
{
    foreach(const QString &str, list) {
        QPushButton *p = new QPushButton(str);
        p->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
        hlayout->addWidget(p);
        btn_list << p;
    }
}

void fexpandmain_dish_btn::setup(QList<QPushButton *> &list)
{
    foreach(QPushButton* p, list) {
        hlayout->addWidget(p);
        btn_list << p;
    }
}

void fexpandmain_dish_btn::setup(const QString &ch_tableno, QScrollBar *vbar)
{
    this->vbar = vbar;
    setDownBtn(false);
    setUpBtn(false);

    connect(up, SIGNAL(clicked()), this,SLOT(toUp()));
    connect(down, SIGNAL(clicked()), this,SLOT(toDown()));
    connect(vbar, SIGNAL(rangeChanged(int,int)), this,SLOT(toUpdaeEnabledFromScrollBar()));

    if(ch_tableno == w_scr_dish_main::Q000) {
        waimai->setProperty("outer_stylesheet", "pushbutton_bottom_menu");
    } else {
        waimai->setProperty("outer_stylesheet", "pushbutton_bottom");
    }
}

void fexpandmain_dish_btn::retranslateView()
{
//    waidai->setText(tr("外带"));
//    waimai->setText(tr("外卖"));
    //    desk->setText(tr("台位"));
}

void fexpandmain_dish_btn::addMenuInitAndShow()
{
    connect(waimai, SIGNAL(clicked(bool)), this, SLOT(toInitWaimaiMenuAndShow()));
}

void fexpandmain_dish_btn::toInitWaimaiMenuAndShow()
{
    if(waimai->menu() == 0) {
        QWidget *w = new QWidget;
        Ui_waimai_menu *ui = new Ui_waimai_menu;
        ui->setupUi(w);
        w->setFixedSize(w->size());
        connect(ui->pushButton_waimai, SIGNAL(clicked()),this, SIGNAL(signal_waimai()));
        connect(ui->pushButton_waimaipay, SIGNAL(clicked()),this, SIGNAL(signal_waimaipay()));

        QWidgetAction *widgetaction=new QWidgetAction(waimai);
        widgetaction->setDefaultWidget(w);

        lds_menu* m=new lds_menu(this);
        m->addAction(widgetaction);

        waimai->setMenu(m);
        waimai->showMenu();
    }
}

void fexpandmain_dish_btn::setUpBtn(bool disabled)
{
    up->setDisabled(disabled);
}

void fexpandmain_dish_btn::setDownBtn(bool disabled)
{
    down->setDisabled(disabled);
}

void fexpandmain_dish_btn::toUpdaeEnabledFromScrollBar()
{
    /*down*/setDownBtn(vbar->value() == vbar->maximum());
    /*up*/setUpBtn(vbar->value() == vbar->minimum());
}

void fexpandmain_dish_btn::toDown()
{
    vbar->setValue(vbar->value()+vbar->pageStep());
    toUpdaeEnabledFromScrollBar();
}

void fexpandmain_dish_btn::toUp()
{
    vbar->setValue(vbar->value()-vbar->pageStep());
    toUpdaeEnabledFromScrollBar();
}
