#ifndef LDS_ROUNDEDDIALOG_TITLE_FRAME_H
#define LDS_ROUNDEDDIALOG_TITLE_FRAME_H

#include <QFrame>

class lds_roundeddialog_title_frame : public QFrame
{
    Q_OBJECT
public:
    lds_roundeddialog_title_frame(QWidget *parent);

protected:
    virtual void paintEvent(QPaintEvent *e);
};

#endif // LDS_ROUNDEDDIALOG_TITLE_FRAME_H
