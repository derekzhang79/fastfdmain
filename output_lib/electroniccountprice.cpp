#include "electroniccountprice.h"
#include "printer_0.h"
#include "electroniccountprice_kaifeng.h"
#include "qextserialport.h"
#include <QtDebug>
#include "printer_pos_serial.h"

static int StrHexToDecimal(QString strHex)
{
    bool ok;
    return strHex.toInt(&ok,16);
}

ElectronicCountPrice::ElectronicCountPrice(QObject *parent) :
    QObject(parent)
{
    kaifeng = new ElectronicCountPrice_kaifeng;
    connect(kaifeng,SIGNAL(receiveData(QByteArray)),this,SLOT(toreceive(QByteArray)));
}

ElectronicCountPrice::~ElectronicCountPrice()
{
    delete kaifeng;
}

bool ElectronicCountPrice::isOpen()
{
    return kaifeng->printer->isOpen();
}

bool ElectronicCountPrice::tryStart_qs(const QString &path)
{
    cur = 0;
    total = 0;
    return kaifeng->tryStart_qs(path,_baudrate);
}

void ElectronicCountPrice::stop()
{
    kaifeng->stop();
}

QString ElectronicCountPrice::lastError()
{
    return kaifeng->lastError();
}

void ElectronicCountPrice::setType(const QString &type)
{
    if(_type != type){
        _type = type;
        switch(creat_pricecount_scaletypes().indexOf(_type)){
        case 0://凯丰
            setSendData("S");//'S'=QByteArray::fromHex("53")
            break;
        case 1://凯士
            setSendData("");
            break;
        case 2://KF-PC秤
            setSendData("");
            break;
        case 3://顶尖PBX
            setSendData("!0R");
            break;
        case 4://大华ASC-A
            setSendData("P");
            break;
        case 5://顶尖PS1X
            setSendData("W");
            kaifeng->printer->psettings.DataBits=DATA_7;
            kaifeng->printer->psettings.Parity=PAR_EVEN;
            kaifeng->printer->psettings.StopBits=STOP_1;
            break;
        case 6://风华电子称
            setSendData("S");//'S'=QByteArray::fromHex("53")
            break;
        case 7://友中衡计价秤
        case 8://友中衡加强版
            setSendData(QByteArray::fromHex("0157300D"));
            break;
            //建宇
        case 9:
        case 10:
        case 11:
        case 12:
        case 13:
            setSendData("");
            break;
        }
    }
}

void ElectronicCountPrice::setBaudrate(int baudrate)
{
    kaifeng->setBaudrate((_baudrate = baudrate));
}

void ElectronicCountPrice::setSendData(const QByteArray &data)
{
    kaifeng->setSendData(data);
}

QString ElectronicCountPrice::jieshi(uchar a)
{
    QString ret;
    switch(a){
    case 0xee:
        ret="0";
        break;
    case 0xfe:
        ret="0.";
        break;
    case 0x0a:
        ret="1";
        break;
    case 0x1a:
        ret="1.";
        break;
    case 0x67:
        ret="2";
        break;
    case 0x77:
        ret="2.";
        break;
    case 0x4f:
        ret="3";
        break;
    case 0x5f:
        ret="3.";
        break;
    case 0x8b:
        ret="4";
        break;
    case 0x9b:
        ret="4.";
        break;
    case 0xcd:
        ret="5";
        break;
    case 0xdd:
        ret="5.";
        break;
    case 0xed:
        ret="6";
        break;
    case 0xfd:
        ret="6.";
        break;
    case 0x0e:
        ret="7";
        break;
    case 0x1e:
        ret="7.";
        break;
    case 0xef:
        ret="8";
        break;
    case 0xff:
        ret="8.";
        break;
    case 0xcf:
        ret="9";
        break;
    case 0xdf:
        ret="9.";
        break;
    case 0xaf:
        ret="A";
        break;
    case 0xe9:
        ret="B";
        break;
    case 0xe4:
        ret="C";
        break;
    case 0x6b:
        ret="D";
        break;
    case 0xe5:
        ret="E";
        break;
    case 0xa5:
        ret="F";
        break;
    case 0x68:
        ret="U";
        break;
    case 0x69:
        ret="O";
        break;
    case 0x01:
        ret="-";
        break;
    case 0x11:
        ret="-.";
        break;
    case 0x29:
        ret="N";
        break;
    case 0xe0:
        ret="L";
        break;
    case 0x41:
        ret="=";
        break;
    case 0x00:
        ret=" ";
        break;
    case 0x10:
        ret=" .";
        break;
    default:
        ret="X";
        break;
    }
    return ret;
}
//202020302e3037306b670d
//___0.070kg
//2020302e3730306b6743340d
//__0.700kgC4 43340d
//c4 =1100 0100
//
//0x01
//0x02
//0x04
//0x08
//0x10
//0x20
//0x40
//0x80
void ElectronicCountPrice::toreceive(const QByteArray &data)
{
    static QByteArray predata;
    static bool iscapture = false;
    predata += data;
    if(iscapture) return;
    QString ret="00";
    total++;


    emit dataComming(data);
    QByteArray x0a;
    x0a += 0x0a;
    QByteArray x0d;
    x0d += 0x0d;
    QByteArray x0d0a;
    x0d0a += x0d;
    x0d0a += x0a;
    QByteArray x0a0d;
    x0a0d += x0a;
    x0a0d += x0d;

    switch(creat_pricecount_scaletypes().indexOf(_type)){//switch
    case 0: //凯丰
    {//
        bool ok;
        float f = data.toFloat(&ok) / 1000.0;//千克 小数点后四位
        if(ok){
            emitWeighComming(f);
            cur++;
        }
    }
        break;
    case 1: //凯士
    {
        bool ok;
        float f = data.mid(2,6).trimmed().toFloat(&ok);//千克 小数点后四位
        if(ok){
            emitWeighComming(f);
            cur++;
        }
    }
        break;
    case 2: //KF-PC秤
    {
        // Sleep(100);//延时100毫秒保证接收到的是一条完整的数据,而不是脱节的
        QByteArray temp=data;
        QByteArray newdata = printer_0::bytecutBybeginEnd(data, QByteArray().append(0x04), QByteArray().append(0x28));
        if(newdata.size() >= 24){
            //        }
            //        QList<QByteArray> newdatalist =  data.split(0x0d);

            //        if (temp.size()>=24){
            //            if (temp[0]==0x04 && temp[1]==0x02 && temp[2]==0x08 && temp[3]==0x20 && temp[20]==0x06&& temp[21]==0x82&& temp[22]==0x48&& temp[23]==0x28 )
            {
                QString s;
                QString zhongliang;
                QString danjia;
                QString heji;
                for(int i = 3; i < 20; i++){
                    s.sprintf("%02x", (unsigned char)temp.at(i));
                    switch(i){
                    case 4:
                        zhongliang=jieshi(StrHexToDecimal(s));//strHex.toInt(&ok,16);
                        if (zhongliang.indexOf(".")>0)
                        {
                            zhongliang=zhongliang.left(1);
                            //  ui->led_qupi->turnOn();
                            emit qupiComming(true);
                        }
                        else
                        {
                            //  ui->led_qupi->turnOff();
                            emit qupiComming(false);
                        }
                        break;
                    case 5:

                        zhongliang+=jieshi(StrHexToDecimal(s));
                        break;
                    case 6:

                        zhongliang+=jieshi(StrHexToDecimal(s));
                        break;
                    case 7:

                        zhongliang+=jieshi(StrHexToDecimal(s));
                        break;
                    case 8:
                        if (jieshi(StrHexToDecimal(s)).indexOf(".")>0)
                        {
                            emit lingweiComming(true);
                            zhongliang+=jieshi(StrHexToDecimal(s)).left(1);
                        }
                        else
                        {
                            emit lingweiComming(false);
                            zhongliang+=jieshi(StrHexToDecimal(s));
                        }

                        //ui->label_10->setText(zhongliang);
                        emitWeighComming(zhongliang.toFloat());
                        cur++;
                        break;

                    default:
                        break;
                    }
                }
            }
        }



        //        QString cret;
        //        char cc[] = {0x04, 0x02, 0x08, 0x20};//重量标志
        //        if(data.length() == 8
        //                && data.startsWith(QByteArray(cc))){
        //            iscapture = true;
        //            QByteArray call = data.right(4);
        //            foreach(uchar c, call){
        //                cret += KF_PC_TRANS(c);
        //            }
        //            ret=cret;
        //        }
        //        /*if(!cret.isEmpty()) */
        //        emit this->receiveData(ret);
    }
        break;
    case 3://顶尖PBX
    {
        int index0, index1;//head1 索引， head2索引
        char head1,head2,flag1,sign,checksum,tail1,tail2,flag2;
        QByteArray weight,unit;
        head1=0x01;
        head2=0x02;
        tail1=0x03;
        tail2=0x04;
        if((index0=data.indexOf(QByteArray()+head1+head2))>-1){//0x0102固定为0x0102 开头
            if((index1=data.indexOf(QByteArray()+tail1+tail2))>-1){//0x0304固定为0x0304 结束
                QByteArray inter =data.mid(index0, index1-index0 + 3);
                if(inter.count()==16){
                    flag1=inter[2];
                    sign=inter[3];
                    weight=QByteArray()+inter[4]+inter[5]+inter[6]+inter[7]+inter[8]+inter[9];
                    unit=QByteArray()+inter[10] + inter[11];
                    checksum=inter[12];
                    flag2=inter[15];
                    if(flag1=='F'){
                        //重量溢出，或没开机
                        //qDebug() << "重量溢出，或没开机";
                    } else if(flag1=='U'){
                        //重量不稳定
                        //qDebug() << "重量不稳定";
                    } else if(flag1=='S'){
                        //重量稳定
                        //qDebug() << "重量稳定";
                    }
                    if(flag2&0x10){//0001 1100
                        //【1重量为零，重量非零】
                        //qDebug() << "重量为零";
                    } else {
                        //qDebug() << "重量非零";
                    }
                    if(flag2&0x08){
                        //【1去皮模式，非去皮模式】
                        //qDebug() << "去皮模式";
                    }else {
                        //qDebug() << "非去皮模式";
                    }
                    if(flag2&0x04){
                        //【1重量溢出或没开机，0重量正常或已经开机】
                        //qDebug() << "重量溢出或没开机";
                    }else {
                        //qDebug() << "重量正常或已经开机";
                    }
                    if(printer_0::_2xor(inter.left(12))!=checksum){
                        //奇偶校验失败
                        //qDebug() << "奇偶校验失败";
                    }
                    //                    if(sign!='-'){
                    float f = weight.trimmed().toFloat();//千克 小数点后四位
                    if(sign == '-')
                        f*=-1;

                    emitWeighComming(f);

                    cur++;
                    //                    }
                }
            }
        }
    }
        break;
    case 4://大华ASC-A
    {
        float f = 0;
        int index0, index1;
        char head1,head2,head3,tail1,tail2,tail3;
        head1=0x02;//头
        head2=0x20;//称重数据定位
        head3=0x20;//称重数据定位
        tail1=0x20;
        tail2=0x4b;
        tail3=0x47;
        if((index0=data.indexOf(QByteArray()+head1+head2+head3))>-1){//0x0102固定为0x0102 开头
            if((index1=data.indexOf(QByteArray()+tail1+tail2+tail3))>-1){//0x0304固定为0x0304 结束
                QByteArray inter =data.mid(index0, index1-index0);
                inter=inter.mid(3);
                if(inter.count()==5){
                    f = inter.toFloat();//千克 小数点后四位
                }
            }
        }
        emitWeighComming(f);

        cur++;
    }
        break;
    case 5://顶尖PS1X
    {
        //0.105
        QList<QByteArray> newdatalist =  data.split('\n');
        QByteArray newdata;
        for(int end = newdatalist.count() - 1; end >=0; end--){
            newdata = newdatalist[end].trimmed();
            if(newdata.toHex().startsWith("02")){
                if(newdata.length() == 7){
                    bool ok;
                    float f=data.mid(1, 5).toInt(&ok)*1.0/1000.0;
                    if(ok){
                        emitWeighComming(f);

                        cur++;
                        break;
                    }
                    //            }
                }
            }
        }
    }
        break;
    case 6://风华电子称
    {
        //0 1 2     3 4 5
        if(data.length()==6){
            //            if(data[5]=='+'){
            QByteArray num=data[5]+data.mid(0, 5);
            float f= num.toInt()*1.0/1000.0;
            emitWeighComming(f);

            cur++;
            //            }
        }
    }
        break;
    case 7://友中衡计价秤
    {
        //c4 0xc0 + 0x04
        static QByteArray cachedata;
        cachedata += data;
        QList<QByteArray> newdatalist =  cachedata.split(0x0d);
        QByteArray newdata;
        int end = newdatalist.count() - 1;
        for(; end >=0; end--){
            newdata = newdatalist[end];
            if(newdata.count() >= 10){
                bool ok=false;
                float f= newdata.right(10).left(8).replace(' ',"").toFloat(&ok);
                if(ok){
                    emitWeighComming(f);

                    cur++;
                    cachedata.clear();
                    break;
                }
            }
        }
    }
        break;
    case 8://友中衡加强版
    {
        //c4 0xc0 + 0x04
        static QByteArray cachedata;
        cachedata += data;
        QList<QByteArray> newdatalist =  cachedata.split(0x0d);
        QByteArray newdata;
        int end = newdatalist.count() - 1;
        for(; end >=0; end--){
            newdata = newdatalist[end];
            if(newdata.count() >= 10){
                bool ok=false;
                float f= newdata.right(10).left(8).replace(' ',"").toFloat(&ok);
                QByteArray state = QByteArray::fromHex(newdata.right(12).left(2));
                if(ok){
                    emitWeighComming(f);

                    cur++;
                    if(state.count() == 1){
                        emit lingweiComming((state[0]&0x01) > 0);
                        emit qupiComming((state[0]&0x02) > 0);
                    }
                    cachedata.clear();
                    break;
                }
            }
        }
    }
        break;

    case 9://建宇-DAHUA
    {
        int index0 = data.indexOf(x0a0d) + 2;
        int index1 = data.indexOf(x0a0d, index0);
        if(index1 - index0 == 5/*净重*/ + 2 + 5/*单价*/ + 2 + 6/*总价*/) {
            float f = data.mid(index0, 5).trimmed().toFloat()/1000;
            emitWeighComming(f);


            //            info += "hahua" + data.mid(index0, 5);
            //            info += "\n";
            //            qDebug() << data.mid(index0, 5);
            //_0006  0.006
            //没有负数
            //默认小数点为3位 001754 === 1.754
        }
    }
        break;
    case 10://建宇-YAOHUA
    {
        int index0 = data.indexOf("=") + 1;
        int index1 = data.indexOf("=", index0);
        if(index1 - index0 == 8) {
            QByteArray d = data.mid(index0, 8);
            QByteArray d0;
            foreach(const char &c, d) {
                d0.prepend(c);
            }
            float f = d0.toFloat();
            emitWeighComming(f);


            //            info += "yaohua" + line.mid(index0, 8);
            //            info += "\n";
            //812.0000  = 0.218
            //没有负数
            //            qDebug() << line.mid(index0, 8);
            //0.00200- == -200.0kg
        }
    }
        break;
    case 11://建宇-TSCALE
    {
        //cRLF 0d0a
        int index0 = data.indexOf("ST,") + 3;
        int index1 = data.indexOf(x0d0a, index0);
        //        ui->lineEdit_jijia_readback->setText(QString::number(index0) + ":" + QString::number(index1));
        if(index1 - index0 == 3/*NT,/GS,*/ + 1/*+-*/+7/*weight*/+3/*,kg*/) {
            float f = data.mid(index0 + 3, 8).replace(" ", "").toFloat();
            emitWeighComming(f);

            //            info += "tscale" + data.mid(index0,2) + data.mid(index0 + 3, 8);
            //            info += "\n";
            //读取到的数据
            //__1.345
            //-_1.345
        }
    }
        break;
    case 12://建宇-G-310
    {
        //cRLF 0d0a
        int index0 = data.indexOf(x0d) + 1;
        int index1 = data.indexOf(x0d, index0);
        //        ui->lineEdit_jijia_readback->setText(QString::number(index0) + ":" + QString::number(index1));
        if(index1 - index0 == 7/*www.www*/) {
            float f  =data.mid(index0, 7).trimmed().toFloat();
            emitWeighComming(f);


            //            info += "G-310" + data.mid(index0, 7);
            //            info += "\n";
            //            qDebug() << data.mid(index0, 7);
            //读不到负数:UF
            //__0.125
            //www.www
        }
    }
        break;
    case 13://建宇-STE-1

    {
        //cRLF 0d0a
        int index0 = data.indexOf("N") + 1;
        int index1 = data.indexOf(x0d0a, index0);
        //        ui->lineEdit_jijia_readback->setText(QString::number(index0) + ":" + QString::number(index1));
        if(index1 - index0 == 7/*净重*/+1/*T*/+7/*皮重*/) {
            float f  =data.mid(index0, 7).trimmed().toFloat();
            emitWeighComming(f);
            //            info += "STE-1" + data.mid(index0, 16);
            //            info += "\n";
            //没有负数
            //__1.256
            //            qDebug() << data.mid(index0+1, 7);
            //www.www
            //            qDebug() << data.mid(index0+1+7+1, 7);
            //www.www
        }
    }
        break;
    }//switch
    //千克 小数点后四位
    iscapture = false;
    emit signalsuccessrate(cur, total);
}

void ElectronicCountPrice::emitWeighComming(float kg)
{
    emit weighComming(QString().sprintf("%.4f",kg));
    emit weighCommingx1000(kg  * 1000.0);
}

void ElectronicCountPrice::qupi()
{
    switch(creat_pricecount_scaletypes().indexOf(_type)){//switch
    case 0: //凯丰
        break;
    case 1://凯士PR PLUS
        break;
    case 2://KF-PC秤
        kaifeng->printer->writeHex("8C");
        break;
    case 3://顶尖PBX
        break;
    case 4://大华ASC-A
        break;
    case 5://顶尖PS1X
        break;
    case 6://风华电子称
        kaifeng->printer->writeHex("5A");
        break;
    case 7://友中衡计价秤
    case 8://友中衡加强版
        kaifeng->printer->writeHex("0125EF0D");
        break;
    }
}

void ElectronicCountPrice::zhiling()
{
    switch(creat_pricecount_scaletypes().indexOf(_type)){//switch
    case 0: //凯丰
        break;
    case 1://凯士PR PLUS
        break;
    case 2://KF-PC秤
        kaifeng->printer->writeHex("8D");
        break;
    case 3://顶尖PBX
        break;
    case 4://大华ASC-A
        break;
    case 5://顶尖PS1X
        break;
    case 6://风华电子称
        kaifeng->printer->writeHex("59");
        break;
    case 7://友中衡计价秤
    case 8://友中衡加强版
        kaifeng->printer->writeHex("0125FF0D");
        break;
    }
}

QStringList ElectronicCountPrice::creat_pricecount_scaletypes()
{
    static QStringList pricecout_scaletypes;
    if(pricecout_scaletypes.isEmpty()){
        pricecout_scaletypes << QObject::tr("凯丰(商超专用)")
                             << QObject::tr("凯士PR PLUS")
                             << QObject::tr("KF-PC秤")
                             << QObject::tr("顶尖PBX")
                             << QObject::tr("大华ASC-A")
                             << QObject::tr("顶尖PS1X")
                             << QObject::tr("风华电子称")
                             << QObject::tr("友中衡计价秤")
                             << QObject::tr("友中衡加强版")


                             << QObject::tr("建宇")+"-"+"DAHUA"
                             << QObject::tr("建宇")+"-"+"YAOHUA"
                             << QObject::tr("建宇")+"-"+"TSCALE"
                             << QObject::tr("建宇")+"-"+"G-310"
                             << QObject::tr("建宇")+"-"+"STE-1"
                                ;
    }
    return pricecout_scaletypes;
}


