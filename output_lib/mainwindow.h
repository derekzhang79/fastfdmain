#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include <QMainWindow>
#include "libusb-1.0/libusb.h"

//namespace Ui {
//class QtUSBTes;
//}

//class QtUSBTes : public QMainWindow
//{
//    Q_OBJECT

//public:
//    explicit QtUSBTes(QWidget *parent = 0);
//    ~QtUSBTes();

//private slots:
//    void on_pushButton_clicked();

//    void on_pushButton_2_clicked();

//private:
//    Ui::QtUSBTes *ui;
//    struct libusb_config_descriptor *conf_desc;
//    quint8 endpoint_in, endpoint_out;
//    libusb_device *dev;
//    int OutSize;
//    libusb_device_handle *handle;

//    void print_devs(libusb_device **devs);
//};

//#endif // QTUSBTES_H


////qtusbtes.cpp

//#include "qtusbtes.h"
//#include "ui_qtusbtes.h"

//QtUSBTes::QtUSBTes(QWidget *parent) :
//    QMainWindow(parent),
//    ui(new Ui::QtUSBTes)
//{
//    ui->setupUi(this);

//    ui->pushButton_2->setText("打开");
//}

//QtUSBTes::~QtUSBTes()
//{
//    delete ui;
//}

//void QtUSBTes::on_pushButton_clicked()
//{
//    unsigned char intTransData[2] = {0xAA, 0x55};
//    OutSize = sizeof(intTransData);
//    int intTrans = libusb_interrupt_transfer(handle, endpoint_in,  intTransData, OutSize, &OutSize, 5000);
//    if (intTrans >= 0)
//    {
//        ;
//    }
//    else
//    {
//        return;
//    }
//    intTransData[0] =  0x01;/**< 如果使用了 REPORT_ID 则 USB 发送数据缓冲区第一个字节必须为 REPORT_ID 以告知系统该数据属于哪个 ID */
//    intTransData[1] =  0x01;
//    intTrans = libusb_interrupt_transfer(handle, endpoint_out,  intTransData, OutSize, &OutSize, 5000);
//    if (intTrans >= 0)
//    {
//        ;
//    }
//    else
//    {
//        return;
//    }
//    intTransData[0] =  0x02;/**< 如果使用了 REPORT_ID 则 USB 发送数据缓冲区第一个字节必须为 REPORT_ID 以告知系统该数据属于哪个 ID */
//    intTransData[1] =  0x01;
//    intTrans = libusb_interrupt_transfer(handle, endpoint_out,  intTransData, OutSize, &OutSize, 5000);
//    if (intTrans >= 0)
//    {
//        ;
//    }
//    else
//    {
//        return;
//    }
//    intTrans = intTrans;
//    OutSize = OutSize;
//}

//void QtUSBTes::on_pushButton_2_clicked()
//{
//    if("打开"==ui->pushButton_2->text())
//    {
//        ui->pushButton_2->setText("关闭");

//        int r;

//        r = libusb_init(NULL);
//        if (r < 0)
//            return;

//        handle = libusb_open_device_with_vid_pid(NULL, 1155, 22352);
//        dev = libusb_get_device(handle);

//        libusb_get_config_descriptor(dev, 0, &conf_desc);
//        for(int i=0; i<conf_desc->bNumInterfaces; i++)
//        {
//            for (int j=0; j<conf_desc->interface[i].num_altsetting; j++)
//            {
//                for (int k=0; k<conf_desc->interface[i].altsetting[j].bNumEndpoints; k++)
//                {
//                    const struct libusb_endpoint_descriptor *endpoint = &conf_desc->interface[i].altsetting[j].endpoint[k];
//                    if (endpoint->bEndpointAddress & LIBUSB_ENDPOINT_IN)
//                    {
//                        endpoint_in = endpoint->bEndpointAddress;
//                    }
//                    else
//                    {
//                        endpoint_out = endpoint->bEndpointAddress;
//                    }
//                }
//            }
//        }
//        r = libusb_set_configuration(handle, 1);
//        if (r < 0)
//            return;
//        libusb_get_config_descriptor(dev, 0, &conf_desc);
//        ui->pushButton->setEnabled(true);
//    }
//    else
//    {
//        ui->pushButton_2->setText("打开");
//        libusb_free_config_descriptor(conf_desc);
//        libusb_close(handle);
//        libusb_exit(NULL);
//        ui->pushButton->setEnabled(false);
//    }
//}

#endif // MAINWINDOW_H
