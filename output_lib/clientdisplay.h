#ifndef CLIENTDISPLAY_H
#define CLIENTDISPLAY_H

#include <QObject>
class ClientDisplay_NO;

//客显得波特率已经内置了，不需要添加
//
class ClientDisplay
{
private:
    ClientDisplay_NO *display;
public:
    explicit ClientDisplay(const QString &type);
    ClientDisplay();
    void setType(const QString &type);

    ~ClientDisplay();
    QString _type;

    bool writeCom(int sqr, const QString &in, const QString &devpath, const QString &name=QString());
    bool cleanCom(const QString &devpath, const QString &/*baudrate*/);
    bool startShowinfo(const QString &text, const QString &devpath);
    QString lastError();
private:
    QString err0;
///
public:
    static QStringList creat_clientdisplay_ledtypes();
};

#endif // CLIENTDISPLAY_H
