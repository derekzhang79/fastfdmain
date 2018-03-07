#ifndef MYHELPER2_H
#define MYHELPER2_H

#include <QDesktopWidget>
#include <QFile>
#include <QTextCodec>
#include <QMessageBox>
#include <QAbstractButton>
#include <QCoreApplication>
#include <QFileDialog>
#include <QTime>
#include <QProcess>
#include <QDir>
#include <QSound>
#include <QApplication>
#include <QStyleFactory>
#include <QInputDialog>
#include <QStyle>
#include <QDebug>
#include<QString>
#include<QSqlDatabase>
#include "frmmessagebox.h"
#include<QNetworkReply>
#include<QHostInfo>
#include<QCryptographicHash>
class myHelper2:public QObject
{
public:



static  QString pub_oper_id;
static  QString pub_cpu_name;
static  QString pub_cpu_id;
static  QString pub_dangkou;
static  QSqlDatabase database ;
static  QString pub_ip;
static  QString pub_booth_name;//档口名字

    //设置全局为plastique样式
   // static void SetStyle()
   // {
  //      QApplication::setStyle(QStyleFactory::create("Plastique"));
        //QApplication::setPalette(QApplication::style()->standardPalette());//替换应用程序的颜色组合
  //  }
//设置皮肤样式
static void SetStyle(const QString &styleName)
{
    QFile file(QString(":/image/%1.css").arg(styleName));
    file.open(QFile::ReadOnly);
    QString qss = QLatin1String(file.readAll());
    qApp->setStyleSheet(qss);
    qApp->setPalette(QPalette(QColor("#F0F0F0")));
}
    //设置编码为GB2312
    static void SetGB2312Code()
    {
        QTextCodec *codec=QTextCodec::codecForName("GB2312");
        QTextCodec::setCodecForLocale(codec);
        QTextCodec::setCodecForCStrings(codec);
        QTextCodec::setCodecForTr(codec);
    }

    //设置编码为UTF8
    static void SetUTF8Code()
    {
        QTextCodec *codec=QTextCodec::codecForName("UTF-8");
        QTextCodec::setCodecForLocale(codec);
        QTextCodec::setCodecForCStrings(codec);
        QTextCodec::setCodecForTr(codec);
    }

    //显示信息框，仅确定按钮
    static void ShowMessageBoxInfo(QString info)
    {
        QMessageBox msg;
        msg.setStyleSheet("font:12pt '宋体'");
        msg.setWindowTitle("提示");
        msg.setText(info);
        msg.setIcon(QMessageBox::Information);
        msg.addButton("确定",QMessageBox::ActionRole);
        msg.exec();
    }

    static void ShowMessageBoxInfoX(QString info)
    {
        QMessageBox::information(0,"提示",info,QMessageBox::Ok);
    }

    //显示错误框，仅确定按钮
    static void ShowMessageBoxError(QString info)
    {
        QMessageBox msg;
        msg.setStyleSheet("font:12pt '宋体'");
        msg.setWindowTitle("错误");
        msg.setText(info);
        msg.setIcon(QMessageBox::Critical);
        msg.addButton("确定",QMessageBox::ActionRole);
        msg.exec();
    }

    static void ShowMessageBoxErrorX(QString info)
    {
        QMessageBox::critical(0,"错误",info,QMessageBox::Ok);
    }

    //显示询问框，确定和取消按钮
    static int ShowMessageBoxQuesion(QString info)
    {
        QMessageBox msg;
        msg.setStyleSheet("font:12pt '宋体'");
        msg.setWindowTitle("询问");
        msg.setText(info);
        msg.setIcon(QMessageBox::Question);
        msg.addButton("确定",QMessageBox::ActionRole);
        msg.addButton("取消",QMessageBox::RejectRole);

        return msg.exec();
    }


  static  QString getcpuid()
    {
         #ifdef Q_OS_UNIX


        system("cat /proc/cpuinfo | grep Serial >/etc/cpuid");


        QString fileName = "/etc/cpuid";
        QString str;QFile file(fileName);
             if(!file.open(QIODevice::ReadOnly | QIODevice::Text))
             {

return "ff";
             }
         QTextStream in(&file);
         str=in.readLine();//读取一行//如果读取多行



      return str.right(19);
    #else

    return "ff86";
#endif
    }
  static  QString get_localmachine_name()
    {
        QString machineName     = QHostInfo::localHostName();
        return machineName;
    }


    static int ShowMessageBoxQuesionX(QString info)
    {
        return QMessageBox::question(0,"询问",info,QMessageBox::Yes|QMessageBox::No);
    }

    //显示标准输入框
    static QString ShowInputBox(QWidget *frm,QString info)
    {
        bool ok;
        return QInputDialog::getText(frm,"提示",info,QLineEdit::Password,"",&ok);
    }

    //16进制字符串转字节数组
    static QByteArray HexStrToByteArray(QString str)
    {
        QByteArray senddata;
        int hexdata,lowhexdata;
        int hexdatalen = 0;
        int len = str.length();
        senddata.resize(len/2);
        char lstr,hstr;
        for(int i=0; i<len; )
        {
            hstr=str[i].toAscii();
            if(hstr == ' ')
            {
                i++;
                continue;
            }
            i++;
            if(i >= len)
                break;
            lstr = str[i].toAscii();
            hexdata = ConvertHexChar(hstr);
            lowhexdata = ConvertHexChar(lstr);
            if((hexdata == 16) || (lowhexdata == 16))
                break;
            else
                hexdata = hexdata*16+lowhexdata;
            i++;
            senddata[hexdatalen] = (char)hexdata;
            hexdatalen++;
        }
        senddata.resize(hexdatalen);
        return senddata;
    }

    static char ConvertHexChar(char ch)
    {
        if((ch >= '0') && (ch <= '9'))
            return ch-0x30;
        else if((ch >= 'A') && (ch <= 'F'))
            return ch-'A'+10;
        else if((ch >= 'a') && (ch <= 'f'))
            return ch-'a'+10;
        else return (-1);
    }

    //字节数组转16进制字符串
    static QString ByteArrayToHexStr(QByteArray data)
    {
        QString temp="";
        QString hex=data.toHex();
        for (int i=0;i<hex.length();i=i+2)
        {
            temp+=hex.mid(i,2)+"";
        }
        return temp.trimmed().toUpper();
    }

    //16进制字符串转10进制
    static int StrHexToDecimal(QString strHex)
    {
        bool ok;
        return strHex.toInt(&ok,16);
    }

    //10进制字符串转10进制
    static int StrDecimalToDecimal(QString strDecimal)
    {
        bool ok;
        return strDecimal.toInt(&ok,10);
    }

    //2进制字符串转10进制
    static int StrBinToDecimal(QString strBin)
    {
        bool ok;
        return strBin.toInt(&ok,2);
    }

    //16进制字符串转2进制字符串
    static QString StrHexToStrBin(QString strHex)
    {
        uchar decimal=StrHexToDecimal(strHex);
        QString bin=QString::number(decimal,2);

        uchar len=bin.length();
        if (len<8)
        {
            for (int i=0;i<8-len;i++)
            {
                bin="0"+bin;
            }
        }

        return bin;
    }

    //10进制转2进制字符串一个字节
    static QString DecimalToStrBin1(int decimal)
    {
        QString bin=QString::number(decimal,2);

        uchar len=bin.length();
        if (len<=8)
        {
            for (int i=0;i<8-len;i++)
            {
                bin="0"+bin;
            }
        }

        return bin;
    }

    //10进制转2进制字符串两个字节
    static QString DecimalToStrBin2(int decimal)
    {
        QString bin=QString::number(decimal,2);

        uchar len=bin.length();
        if (len<=16)
        {
            for (int i=0;i<16-len;i++)
            {
                bin="0"+bin;
            }
        }

        return bin;
    }

    //计算校验码
    static uchar GetCheckCode(uchar data[],uchar len)
    {
        uchar temp=0;

        for (uchar i=0;i<len;i++)
        {
            temp+=data[i];
        }

        return temp%256;
    }

    //将溢出的char转为正确的uchar
    static uchar GetUChar(char data)
    {
        uchar temp;
        if (data>127)
        {
            temp=data+256;
        }
        else
        {
            temp=data;
        }
        return temp;
    }

    //延时
    static void Sleep(int sec)
    {
        QTime dieTime = QTime::currentTime().addMSecs(sec);
        while( QTime::currentTime() < dieTime )
            QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
    }

    //获取当前路径
    static QString GetCurrentPath()
    {
        return QString(QCoreApplication::applicationDirPath()+"/");
    }

    //播放声音
    static void PlaySound(QString soundName)
    {
        QSound::play(soundName);
    }

    //设置系统日期时间
    static void SetSystemDateTime(int year,int month,int day,int hour,int min,int sec)
    {
        QProcess p(0);

        p.start("cmd");
        p.waitForStarted();
        p.write(QString("date %1-%2-%3\n").arg(year).arg(month).arg(day).toAscii());
        p.closeWriteChannel();
        p.waitForFinished(1000);
        p.close();

        p.start("cmd");
        p.waitForStarted();
        p.write(QString("time %1:%2:%3.00\n").arg(hour).arg(min).arg(sec).toAscii());
        p.closeWriteChannel();
        p.waitForFinished(1000);
        p.close();
    }
    //判断是否是IP地址
    static bool IsIP(QString IP)
    {
        QRegExp RegExp("((2[0-4]\\d|25[0-5]|[01]?\\d\\d?)\\.){3}(2[0-4]\\d|25[0-5]|[01]?\\d\\d?)");
        return RegExp.exactMatch(IP);
    }

    //显示信息框,仅确定按钮
    static void ShowMessageBoxInfo2(QString info)
    {
        frmMessageBox *msg = new frmMessageBox;
        msg->SetMessage(info, 0);
        msg->exec();
    }

    //显示错误框,仅确定按钮
    static void ShowMessageBoxError2(QString info)
    {
        frmMessageBox *msg = new frmMessageBox;
        msg->SetMessage(info, 2);
        msg->exec();
    }

    //显示询问框,确定和取消按钮
    static int ShowMessageBoxQuesion2(QString info)
    {
        frmMessageBox *msg = new frmMessageBox;
        msg->SetMessage(info, 1);
        return msg->exec();
    }
    //窗体居中，并且只有关闭按钮，不能调整大小
    static void FormOnlyCloseInCenter(QWidget *frm)
    {
        //设置窗体居中
        QDesktopWidget desktop;
        int screenX=desktop.availableGeometry().width();
        int screenY=desktop.availableGeometry().height()-40;
        int frmX=frm->width();
        int frmY=frm->height();
        QPoint movePoint(screenX/2-frmX/2,screenY/2-frmY/2);
        frm->move(movePoint);

        //设置窗体不能调整大小
        frm->setFixedSize(frmX,frmY);

        //设置窗体只有最小化按钮
        frm->setWindowFlags(Qt::WindowCloseButtonHint);        
    }

    //窗体居中显示
    static void FormInCenter(QWidget *frm)
    {
        int screenX=qApp->desktop()->width();
        int screenY=qApp->desktop()->height()-60;
        int wndX=frm->width();
        int wndY=frm->height();
        QPoint movePoint((screenX-wndX)/2,(screenY-wndY)/2);
        frm->move(movePoint);
    }

    //窗体没有最大化按钮
    static void FormNoMaxButton(QWidget *frm)
    {
        frm->setWindowFlags(Qt::WindowMinimizeButtonHint);
    }

    //窗体只有关闭按钮
    static void FormOnlyCloseButton(QWidget *frm)
    {
        frm->setWindowFlags(Qt::WindowCloseButtonHint);
    }

    //窗体不能改变大小
    static void FormNotResize(QWidget *frm)
    {
        frm->setFixedSize(frm->width(),frm->height());
    }

    //获取桌面大小
    static QSize GetDesktopSize()
    {
        QDesktopWidget desktop;
        return QSize(desktop.availableGeometry().width(),desktop.availableGeometry().height());
    }

    //获取选择的文件
    static QString GetFileName(QString filter)
    {
        return QFileDialog::getOpenFileName(NULL,"选择文件",QCoreApplication::applicationDirPath(),filter);
    }

    //获取选择的文件集合
    static QStringList GetFileNames(QString filter)
    {
        return QFileDialog::getOpenFileNames(NULL,"选择文件",QCoreApplication::applicationDirPath(),filter);
    }

    //获取选择的目录
    static QString GetFolderName()
    {
        return QFileDialog::getExistingDirectory();
    }

    //获取文件名，含拓展名
    static QString GetFileNameWithExtension(QString strFilePath)
    {
        QFileInfo fileInfo(strFilePath);
        return fileInfo.fileName();
    }

    //获取选择文件夹中的文件
    static QStringList GetFolderFileNames(QStringList filter)
    {
        QStringList fileList;
        QString strFolder = QFileDialog::getExistingDirectory();
        if (!strFolder.length()==0)
        {
            QDir myFolder(strFolder);

            if (myFolder.exists())
            {
                fileList= myFolder.entryList(filter);
            }
        }
        return fileList;
    }

    //文件夹是否存在
    static bool FolderIsExist(QString strFolder)
    {
        QDir tempFolder(strFolder);
        return tempFolder.exists();
    }

    //文件是否存在
    static bool FileIsExist(QString strFile)
    {
        QFile tempFile(strFile);
        return tempFile.exists();
    }


    //MD5

    static QString getmd5(QString sour)
    {
        QString md5;
      //  QString pwd="123456";
        QByteArray bb;
        bb = QCryptographicHash::hash ( sour.toAscii(), QCryptographicHash::Md5 );
        md5.append(bb.toHex());
    }

    //复制文件
    static bool CopyFile(QString sourceFile, QString targetFile)
    {
        if (FileIsExist(targetFile))
        {
            int ret=QMessageBox::information(NULL,"提示","文件已经存在，是否替换？",QMessageBox::Yes | QMessageBox::No);
            if (ret!=QMessageBox::Yes)
            {
                return false;
            }
        }
        return QFile::copy(sourceFile,targetFile);
    }



    //解密
  static  QString of_str_decrypt(QString as_source)
    {
        //Public function of_str_decrypt (string as_source) returns string
        //string as_source
        long i;
        long ll_len;
        QString ls_text;
        QString ls_result;
        QString ls_return;

        //Has been Shielded.
        ls_text = as_source.trimmed();
        ll_len = ls_text.length();
        ls_result = "";
        for(int i = 0; i < ll_len; i++){ // 123  -> 321
            //前插入
            ls_result.push_front(QChar((char)(ls_text.at(i).unicode() -  (2 * (ll_len - i - 3)))));//char(asc(mid(ls_text,i,1)) - (2 * (ll_len - i - 2)));
        }
        return ls_result;
    }



  //utf-8 to gbk
  static QByteArray  utf8togbk(const QString &in)
  {
      QByteArray b;
      //获取app的编码
      QByteArray pre_codec=QTextCodec::codecForLocale()->name();
      //设置为打印机可以识别的编码
      QTextCodec::setCodecForLocale(QTextCodec::codecForName("gb2312"));
      //转换
      b.append(in.toLocal8Bit());
      //还原
      QTextCodec::setCodecForLocale(QTextCodec::codecForName(pre_codec));
      return b;
  }




    //加密
    QString of_str_encrypt(QString as_source)
    {
        //Public function of_str_encrypt (string as_source) returns string
        //string as_source
        long i;
        long ll_len;
        QString ls_text;
        QString ls_result;


        //Has been Shielded.
        ls_text = as_source.trimmed();
        ll_len = ls_text.length();
        ls_result = "";
        for(int i = 0; i < ll_len ; i++){//321  -> 123
            ls_result.push_front(QChar((char)(ls_text.at(i).unicode() +  (2 * (i - 2)))));
        }
        return ls_result;

    }

  static  bool do_post(QString url, QByteArray content, QByteArray &responseData)
    {
      QNetworkAccessManager *netManager = new QNetworkAccessManager();
       QEventLoop eventLoop;
        // QObject::connect(netManager,SIGNAL(finished(QNetworkReply*)),this,SLOT(replyFinished(QNetworkReply*)));
        connect(netManager, SIGNAL(finished(QNetworkReply*)), &eventLoop, SLOT(quit()));

        int contentLength = content.length();
        QNetworkRequest req;
        req.setUrl(QUrl(url));
        //application/octet-stream
        req.setHeader(QNetworkRequest::ContentTypeHeader,"application/x-www-form-urlencoded");
        req.setHeader(QNetworkRequest::ContentLengthHeader,contentLength);
        QNetworkReply *reply2=netManager->post(req,content);
        //connect(reply2,SIGNAL(uploadProgress(qint64,qint64)),SLOT(updateProgress(qint64,qint64)));

        eventLoop.exec();       //block until finish
        // QByteArray responseData;
        QVariant status_code = reply2->attribute(QNetworkRequest::HttpStatusCodeAttribute);
        if(reply2->error() == QNetworkReply::NoError)
        {



    //load_dialog->hide();
            responseData = reply2->readAll();
            delete reply2;//移除reply2

            return true;
        }
        else
        {
            myHelper2::ShowMessageBoxError2(reply2->errorString());
            delete reply2;//移除reply2
            return false;
        }
    }




};

#endif // MYHELPER2_H
