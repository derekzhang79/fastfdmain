#ifndef ELECTRONICCOUNTPRICE_H
#define ELECTRONICCOUNTPRICE_H
#include <QObject>
class ElectronicCountPrice_kaifeng;

/**
1.建议在头文件里定义指正,方便调用;
2.    if(countprice_scale == 0) {
        countprice_scale=new   ElectronicCountPrice(0, "", "");
        //lineEdit_39会接受回读信息,
        connect(countprice_scale,SIGNAL(weighComming(QString)),ui->lineEdit_39,SLOT(setText(QString)));
    }
    countprice_scale->stop();//停止
    countprice_scale->setType(ui->comboBox_47->currentText());
    countprice_scale->setBaudrate(ui->comboBox_49->currentText());
    if(!countprice_scale->tryStart_qs(ui->comboBox_46->currentText())){//开始
        lds_messagebox::warning(this, tr("提示"),countprice_scale->lastError());//错误信息
    }
3.
*/
class ElectronicCountPrice : public QObject
{
    Q_OBJECT
private:
    QString _type;
    int _baudrate;
    ElectronicCountPrice_kaifeng *kaifeng;
    void setSendData(const QByteArray &data);
    QString jieshi(uchar a);
public:
    explicit ElectronicCountPrice(QObject *parent);
    ~ElectronicCountPrice();

    bool isOpen();
    bool tryStart_qs(const QString &path);
    void stop();
    QString lastError();

    inline QString type(){return _type;}
    void setType(const QString &type);
    inline int baudrate(){return _baudrate;}
    void setBaudrate(int baudrate);
public slots:
    void qupi();//发送去皮指令
    void zhiling();//发送置零指令
signals:
    void qupiComming(bool);//回读去皮信息
    void lingweiComming(bool);//回读指令信息
    void weighComming(const QString &weigh);//回读取重信息
    void weighCommingx1000(double);//回读取重信息

    void dataComming(const QString &weigh);//回读取重信息
    void signalsuccessrate(int cur, int total);
private slots:
    void toreceive(const QByteArray & data);
private:
    void emitWeighComming(float kg);
private:
    int cur;
    int total;

    ///////
public:
    static QStringList creat_pricecount_scaletypes();
};

#endif // ELECTRONICCOUNTPRICE_H
