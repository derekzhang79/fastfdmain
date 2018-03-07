#ifndef LDS_ROUNDEDDIALOG_TITLE_H
#define LDS_ROUNDEDDIALOG_TITLE_H

#include <QWidget>

class lds_roundeddialog_title : public QWidget
{
    Q_OBJECT
public:
    lds_roundeddialog_title(QWidget *parent);
    lds_roundeddialog_title(QWidget *parent, const QString &title,QColor *titleColor);

    void setTitle(const QString &title);
    void setTitleColor(QColor *titleColor);
    void enableBottomBorder();
protected:
    virtual void paintEvent(QPaintEvent *);
    QString title;
    QColor *titleColor;
private:
    bool isbottomBorder;
};

#endif // LDS_ROUNDEDDIALOG_TITLE_H
