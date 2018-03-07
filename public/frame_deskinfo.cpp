#include "frame_deskinfo.h"
#include <QGridLayout>
#include <QVariant>
#include <QtDebug>
#include "back_view_dialog.h"
#include "lds.h"
#include "ui_fexpandmain_payover.h"

frame_deskinfo::frame_deskinfo(QWidget *parent) :
    lds_roundeddialog_title_frame(parent)
{
    setObjectName("frame_deskinfo");

    label_11 = new QLabel(this);    label_11->setProperty("outer_stylesheet", "label_upon_bg");
    label_5 = new QLabel(this);      label_5->setProperty("outer_stylesheet", "label_upon_bg");
    label_deskno = new QLabel(this);                label_deskno->setProperty("outer_stylesheet", "label_upon_bg");
    label_opentableman = new QLabel(this);          label_opentableman->setProperty("outer_stylesheet", "label_upon_bg");
    label_total = new QPushButton(this);        label_total->setProperty("outer_stylesheet", "xx_amount");
    label_total->setFocusPolicy(Qt::NoFocus);
    label_servant = new QLabel(this);           label_servant->setProperty("outer_stylesheet", "label_upon_bg");
    label_7 = new QLabel(this);          label_7->setProperty("outer_stylesheet", "label_upon_bg");
    label_personnum = new QLabel(this);         label_personnum->setProperty("outer_stylesheet", "label_upon_bg");
    label_ch_billno = new QLabel(this);         label_ch_billno->setProperty("outer_stylesheet", "label_upon_bg");
    label = new QLabel(this);        label->setProperty("outer_stylesheet", "label_upon_bg");
    label_10 = new QLabel(this);    label_10->setProperty("outer_stylesheet", "label_upon_bg");
    label_num = new QLabel(this);               label_num->setProperty("outer_stylesheet", "xx_num");

    QHBoxLayout *hlayout = new QHBoxLayout;
    label_logo = new QLabel(this);label_logo->setProperty("outer_stylesheet", "label_upon_bg");
    label_text = new QLabel(this);label_text->setProperty("outer_stylesheet", "label_upon_bg");
    hlayout->addWidget(label_logo);
    hlayout->addWidget(label_text);
    hlayout->addStretch(1);
    label_logo->clear();
    QPixmap p = back_view_dialog::get_fastfdbacklogo();
    label_logo->setPixmap(p);
    QStringList ret_list = back_view_dialog::get_fastfdbacktext();
    label_text->setText(ret_list.value(0).trimmed());
    label_text->setAlignment(Qt::AlignBottom|Qt::AlignLeft);
    label_text->setStyleSheet("font-size:14pt;");
    if(lds::MAIN_WINDOW_SIZE.width() == 800) {
        label_text->setStyleSheet("font-size:12pt;");
    }
    //    label_text->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    QGridLayout *gridLayout = new QGridLayout;
    gridLayout->addWidget(label_11, 1, 2, 1, 1);
    gridLayout->addWidget(label_5, 1, 0, 1, 1);
    gridLayout->addWidget(label_deskno, 1, 1, 1, 1);
    gridLayout->addWidget(label_opentableman, 1, 3, 1, 1);
    gridLayout->addWidget(label_total, 3, 2, 1, 3);
    gridLayout->addWidget(label_servant, 2, 1, 1, 1);
    gridLayout->addWidget(label_7, 2, 2, 1, 1);
    gridLayout->addWidget(label_personnum, 2, 3, 1, 1);
    gridLayout->addWidget(label_ch_billno, 0, 1, 1, 4);
    gridLayout->addWidget(label, 0, 0, 1, 1);
    gridLayout->addWidget(label_10, 2, 0, 1, 1);
    gridLayout->addWidget(label_num, 3, 0, 1, 2);

    QLabel *label_v = new QLabel(this);
    label_v->setStyleSheet("background-color:#ff650b");
    label_v->setFixedHeight(lds::BORDER_DEFAULT_HEIGHT);

    QVBoxLayout *vlayout = new QVBoxLayout;
    vlayout->addLayout(hlayout);
    vlayout->addWidget(label_v);
    vlayout->addLayout(gridLayout, 1);
    vlayout->setMargin(margin=9);
    label_text->setFixedWidth(lds::PAY_LEFT_SPACE - margin*2 - p.width());

    this->setLayout(vlayout);

    retranslateView();
}

frame_deskinfo::~frame_deskinfo()
{
    qDebug() << __FUNCTION__;

}

void frame_deskinfo::retranslateView()
{
    label_11->setText(tr("开台人:"));
    label_5->setText(tr("桌号:"));
    label_7->setText(tr("人数:"));
    label->setText(tr("单号:"));
    label_10->setText(tr("服务员:"));
}

QPushButton *frame_deskinfo::get_label_total()
{
    return label_total;
}

QString frame_deskinfo::get_ch_tableno()
{
    return label_deskno->text();
}

void frame_deskinfo::setrestaurantshow(bool isshow)
{
    label_servant->setVisible(isshow);
    label_deskno->setVisible(isshow);
    label_ch_billno->setVisible(isshow);
    label_7->setVisible(false);
    label_11->setVisible(isshow);
    label->setVisible(isshow);
    label_opentableman->setVisible(isshow);
    label_5->setVisible(isshow);
    label_10->setVisible(isshow);
    label_personnum->setVisible(false);
}

void frame_deskinfo::label_total_setText(const QString &text)
{
    label_total->setText(text);
}

void frame_deskinfo::label_ch_billno_setText(const QString &text)
{
    label_ch_billno->setText(text);
}

void frame_deskinfo::label_deskno_setText(const QString &text)
{
    label_deskno->setText(text);
}

void frame_deskinfo::label_opentableman_setText(const QString &text)
{
    label_opentableman->setText(text);
}

void frame_deskinfo::label_personnum_setText(const QString &text)
{
    label_personnum->setText(text);
}

void frame_deskinfo::label_servant_setText(const QString &text)
{
    label_servant->setText(text);
}

void frame_deskinfo::label_num_setText(const QString &text)
{
    label_num->setText(text);
}
