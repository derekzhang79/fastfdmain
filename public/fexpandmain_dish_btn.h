#ifndef FEXPANDMAIN_DISH_BTN_H
#define FEXPANDMAIN_DISH_BTN_H

#include <QFrame>
#include <QPushButton>
#include <QHBoxLayout>
class QScrollBar;


class fexpandmain_dish_btn : public QFrame
{
    Q_OBJECT
public:
    explicit fexpandmain_dish_btn(QWidget *parent = 0);
    ~fexpandmain_dish_btn();

    void setup(const QString &ch_tableno, QScrollBar *vbar);
    QPushButton *up ;
    QPushButton *down ;
    QPushButton *waimai ;
    QPushButton *desk ;
    QPushButton *waidai ;

    void retranslateView();
    void addMenuInitAndShow();
signals:
   void signal_waimai();
   void signal_waimaipay();

private slots:
    void toUpdaeEnabledFromScrollBar();

    void toDown();
    void toUp();
    void toInitWaimaiMenuAndShow();
private:
    void setup(const QStringList &list);
    void setup(QList<QPushButton *> &list);
    void setUpBtn(bool disabled);
    void setDownBtn(bool disabled);

    QList<QPushButton *> btn_list;
    QHBoxLayout *hlayout;
    QScrollBar *vbar;
};

#endif // FEXPANDMAIN_DISH_BTN_H
