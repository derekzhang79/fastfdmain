#ifndef FRONTVIEWDIALOG_AUTOEXPAND_H
#define FRONTVIEWDIALOG_AUTOEXPAND_H

#include <QWidget>
#include <QPushButton>
#include <QGridLayout>
#include "frontviewdialog_toolbtn.h"
#include <QMap>
#include <QFrame>

class frontViewDialog_autoexpand : public QFrame
{
    Q_OBJECT
public:
    struct content_item_icon{//保存item的信息
        content_item_icon(
                const QString & _index, const QString &_text, bool _is_use_pixmap=false, const QPixmap &_icon=QPixmap()):
            index(_index),
            text(_text),
            icon(_icon),
            toolbtn(0),
            is_use_pixmap(_is_use_pixmap){}
        QString index;
        QString text;
        QPixmap icon;
        frontViewDialog_toolbtn *toolbtn;
        bool is_use_pixmap;
    };
public:
    explicit frontViewDialog_autoexpand(QWidget *parent = 0);
    ~frontViewDialog_autoexpand();
    void test();

    void add_item(
            const QString & index ,
            const QString &text,
            bool _is_use_pixmap=true,
            const QPixmap &icon=QPixmap());//添加item

    void generat_from_items();//生成当前页数的item
    bool is_like_tab;//默认打开
    QSize tool_size;
    QSize tool_icon_size;
    void set_btn_property(const QString &property);
    void enable_dollar();

    QString btn_property;
    bool btn_is_dollar;
    QColor presscolor;

    //horizontal_count*vertical_count 必须大于2，等于2会死循环；horizontal_count和vertical_count都>0
    int horizontal_count;//水平数量
    int vertical_count;//垂直数量
    void clearall_item();
    void item_event_click();

    QStringList hasSelected();//key
    QList<frontViewDialog_toolbtn *> hasSelectedRb();//key
    QStringList hasSelectedText();//value
    void clearSelected();//clearr
    QStringList curallText();//所有d key
    void setCurrentIndex(const QString &index);
    void setCurrentIndex(const QString &index, float num);
    void setCurrentIndex(const QString &index, float num, bool ischecked);
    void setallitemcheck(bool enabled);

    frontViewDialog_toolbtn *get_content_toolbtn(const QString &index);
    int count();
    void delete_all();
private slots:
    void btn_goto_pre();//按钮上一页
    void btn_goto_next();//按钮下一页
signals:
    void signal_pre();
    void signal_next();
    void signal_text(const QString &text);
    void signal_rb_pressed(frontViewDialog_toolbtn *rb);
    void signal_curpage(int cur, int total);
private:
    QGridLayout *glayout;
    void glayout_clearall();//删除布局中所有的item

    QList<content_item_icon> content;
    QMap<QString, frontViewDialog_toolbtn *> content_tool_save_map;

    frontViewDialog_toolbtn *get_content_tool_save_map_toolbtn(const QString &index);

    QString is_like_tab_save;
    int page_all;//需要动态计算
    int calulate_page_all();//计算总页数
    int one_by_one_page_all(int content_remain, int count, bool begin);//一页的计算

    int curpage;//当前页数
    int calulate_content_index_from_curpage_index(int curpage_index);
};

#endif // FRONTVIEWDIALOG_AUTOEXPAND_H
