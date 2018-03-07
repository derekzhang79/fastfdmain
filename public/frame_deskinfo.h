#ifndef FRAME_DESKINFO_H
#define FRAME_DESKINFO_H

#include <QFrame>
#include <QLabel>
#include <QPushButton>
#include "lds_roundeddialog_title_frame.h"

class frame_deskinfo : public lds_roundeddialog_title_frame
{
    Q_OBJECT
public:
    explicit frame_deskinfo(QWidget *parent = 0);
    ~frame_deskinfo();
    QLabel *label_11;// QLabel(frame_deskinfo);
    QLabel *label_5;// QLabel(frame_deskinfo);
    QLabel *label_deskno;// QLabel(frame_deskinfo);
    QLabel *label_opentableman;// QLabel(frame_deskinfo);
    QLabel *label_servant;// QLabel(frame_deskinfo);
    QLabel *label_7;// QLabel(frame_deskinfo);
    QLabel *label_personnum;// QLabel(frame_deskinfo);
    QLabel *label_ch_billno;// QLabel(frame_deskinfo);
    QLabel *label;// QLabel(frame_deskinfo);
    QLabel *label_10;// QLabel(frame_deskinfo);
    QLabel *label_num;// QLabel(frame_deskinfo);
    QPushButton *label_total;
    QLabel *label_logo;
    QLabel *label_text;
    int margin;

    void retranslateView();
    void setrestaurantshow(bool isshow);

    void label_total_setText(const QString &text);
    void label_ch_billno_setText(const QString &text);
    void label_deskno_setText(const QString &text);
    void label_opentableman_setText(const QString &text);
    void label_personnum_setText(const QString &text);
    void label_servant_setText(const QString &text);
    void label_num_setText(const QString &text);

    QPushButton *get_label_total();
    QString get_ch_tableno();
};

#endif // FRAME_DESKINFO_H
