#ifndef W_BT_DISH_KITCHENPRINTER_PRINTERSET_PORT_H
#define W_BT_DISH_KITCHENPRINTER_PRINTERSET_PORT_H

#include <QFrame>

namespace Ui {
class w_bt_dish_kitchenprinter_printerset_port;
}

class w_bt_dish_kitchenprinter_printerset_port : public QFrame
{
    Q_OBJECT

public:
    explicit w_bt_dish_kitchenprinter_printerset_port(QWidget *parent = 0);
    ~w_bt_dish_kitchenprinter_printerset_port();

    void setPrinterport(const QString &port);
    QString printerport();
    void refreshPort();
    void set_com_usb_enable(bool e);
public slots:
    void toporttype(int index);
private slots:
    void toselectvidpidlist();
private:
    Ui::w_bt_dish_kitchenprinter_printerset_port *ui;
protected:
    void resizeEvent(QResizeEvent *e);
};

#endif // W_BT_DISH_KITCHENPRINTER_PRINTERSET_PORT_H
