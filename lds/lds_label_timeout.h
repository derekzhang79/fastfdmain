#ifndef LDS_LABEL_TIMEOUT_H
#define LDS_LABEL_TIMEOUT_H

#include <QLabel>
class QTimer;
class lds_label_timeout : public QLabel
{
    Q_OBJECT
public:
    explicit lds_label_timeout(QWidget *parent = 0);
    void start(const QString &title /*表头*/,int timeout/*单位为s， 必须> 0*/);
signals:
    void signaltimeout();
private slots:
    void totrigger();
private:
    int resultTimeout;
    QString title;
    QTimer *timer;

};

#endif // LDS_LABEL_TIMEOUT_H
