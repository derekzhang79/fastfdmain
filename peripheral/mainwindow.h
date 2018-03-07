#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui {
class MainWindow;
}

class ElectronicCountPrice;
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    static QString appName();
private slots:
    void on_comboBox_currentIndexChanged(int index);

    void on_groupBox_4_toggled(bool arg1);
    void on_groupBox_3_toggled(bool arg1);
    void on_groupBox_2_toggled(bool arg1);
    void on_groupBox_toggled(bool arg1);
    void on_pushButton_2_clicked();

    void on_pushButton_3_clicked();

    void on_comboBox_7_currentIndexChanged(int index);

    void on_pushButton_4_clicked();

    void on_pushButton_5_clicked();

    void on_pushButton_7_clicked();

    void tocountpricerate(int cur, int total);

    void on_pushButton_hex_test_clicked();

private:
    Ui::MainWindow *ui;
    ElectronicCountPrice *countprice_scale;
};

#endif // MAINWINDOW_H
