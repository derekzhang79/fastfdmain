#ifndef PUBLIC_TOOLBUTON_WRAP_H
#define PUBLIC_TOOLBUTON_WRAP_H

#include <QToolButton>
/*
QFont::Light	25	25
QFont::Normal	50	50
QFont::DemiBold	63	63
QFont::Bold	75	75
QFont::Black	87	87
*/
class public_toolbuton_wrap : public QToolButton
{
    Q_OBJECT
public:
    explicit public_toolbuton_wrap(QWidget *parent = 0);
    int xp1, yp1, xp2, yp2;
    int textfont_size;

protected:
    void paintEvent(QPaintEvent *);

protected:
    int textfont_weight;//
    int textflags;
};

#endif // PUBLIC_TOOLBUTON_WRAP_H
