#ifndef LDS_OGG_PLAY_H
#define LDS_OGG_PLAY_H

#ifdef QT_PHONON_SUPPORT
#include <phonon>
#endif
#include <QStringList>
#include <QThread>

class lds_ogg_play
{
public:
    lds_ogg_play();
    static bool play(const QString &ogg_path, double value, int decimals = 2, QString *errstring = 0);
    static bool isPlaying();
private:
    static QStringList playlist(double value, int decimals = 2);
    static QString charOggName(const QChar &c);
    static void remove(QStringList &list, int start, int length);

#ifdef QT_PHONON_SUPPORT
    static Phonon::MediaObject mobject;
#endif

    static QList<QStringList> current_playlist;
    static bool isplaying;
    static QString ogg_path;
};

class lds_ogg_play_event : public QObject {
    Q_OBJECT
public:
    lds_ogg_play_event(QObject *parent = 0) : QObject(parent){}
public slots:
    void toplay(const QString &ogg_path, double value, int decimals);
};

class lds_ogg_play_thread : public QThread {
    Q_OBJECT
public:
    lds_ogg_play_thread(QObject *parent = 0) : QThread(0){}
    ~lds_ogg_play_thread();
    void play(const QString &ogg_path, double value, int decimals = 2);
signals:
    void signal_play(const QString &ogg_path, double value, int decimals);
protected:
    void run();
};

#endif // LDS_OGG_PLAY_H
