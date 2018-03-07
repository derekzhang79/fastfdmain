#include "frontviewdialog_autoexpand.h"
#include <qdebug.h>
#include <QLabel>
#include <QToolButton>
#include <QApplication>
#include <QMouseEvent>


frontViewDialog_autoexpand::frontViewDialog_autoexpand(QWidget *parent) :
    QFrame(parent)
{
    horizontal_count=3;
    vertical_count=2;
    btn_is_dollar  = false;

    page_all=0;
    curpage=0;
    is_like_tab=true;
    tool_size = QSize(100,100);
    tool_icon_size = QSize(50,50);
    glayout=new QGridLayout;
    this->setLayout(glayout);
    glayout->setMargin(2);
    glayout->setSpacing(2);

    connect(this,SIGNAL(signal_next()),this,SLOT(btn_goto_next()));
    connect(this,SIGNAL(signal_pre()),this,SLOT(btn_goto_pre()));
}

frontViewDialog_autoexpand::~frontViewDialog_autoexpand()
{
}

void frontViewDialog_autoexpand::test()
{
    clearall_item();
    for(int x=0;x<20;x++)
        add_item(QString("%1").arg(x), " are you ready to go to library?", true, QPixmap(":/p"));
    generat_from_items();
}

void frontViewDialog_autoexpand::btn_goto_pre()
{
    curpage--;
    generat_from_items();
    emit signal_curpage(curpage, page_all);
}

void frontViewDialog_autoexpand::btn_goto_next()
{
    curpage++;
    generat_from_items();
    emit signal_curpage(curpage, page_all);
}

void frontViewDialog_autoexpand::glayout_clearall()
{
    for(int index=0,index_count=glayout->count();index<index_count;index++){
        QWidget* widget = glayout->itemAt(0)->widget();
        glayout->removeWidget(widget);
        //这儿不删除
        if(qobject_cast<QLabel*>(widget)) {delete widget;}
        else {widget->hide();}
    }
}

frontViewDialog_toolbtn *frontViewDialog_autoexpand::get_content_toolbtn(const QString &index)
{
    foreach(const content_item_icon &c, content){
        if(c.index==index){
            return c.toolbtn;
        }
    }
    return 0;
}

int frontViewDialog_autoexpand::count()
{
    return content.count();
}

void frontViewDialog_autoexpand::delete_all()
{
    for(int k = 0; k < content.count(); k++) {
        if(content[k].toolbtn) delete content[k].toolbtn;
    }
    content.clear();
    content_tool_save_map.clear();

}

frontViewDialog_toolbtn *frontViewDialog_autoexpand::get_content_tool_save_map_toolbtn(const QString &index)
{
    return content_tool_save_map.value(index);
}

int frontViewDialog_autoexpand::calulate_page_all()
{
    int glayout_count=horizontal_count*vertical_count;
    int ret_all=0;
    int count_remain=content.count();
    int count_remain_take=0;
    bool count_remain_flag=true;
    while((count_remain_take=one_by_one_page_all(count_remain, glayout_count, count_remain_flag))!=-1){
        ret_all++;
        count_remain_flag=false;
        count_remain=count_remain-count_remain_take;
    }
    return ret_all;
}

int frontViewDialog_autoexpand::one_by_one_page_all(int content_remain, int count, bool begin)
{
    if(content_remain<=0) return -1;//结束
    if(begin&&content_remain<=count){//开头
        return content_remain;
    }
    if(begin&&content_remain>count){//开头
        return count-1;
    }
    if((content_remain+1)<=count) {//尾部
        return count-1;
    }
    if((content_remain+1)>count){//中间
        return count-2;
    }
    return -1;
}

//-4[填充] -3[<<] -2[>>] -1[null] 0[0]
int frontViewDialog_autoexpand::calulate_content_index_from_curpage_index(int curpage_index)
{
    int count=vertical_count*horizontal_count;
    int ret_index = -1;
    switch(0){
    case 0:
        if(page_all==0){
            break;
        }
    case 1:
        if(page_all==1){
            ret_index=curpage_index;
            break;
        }
    case 2:
        if(page_all>1){
            if(curpage==-1){
                ret_index=-1;
                break;
            }

            if(curpage==0){//开头
                if(curpage_index==count-1){
                    ret_index=-2;//>>
                    break;
                }

                ret_index=curpage_index;
                break;
            }

            //2                                            4
            if(curpage>0 && curpage == page_all-1){//最后
                if(curpage_index==0){
                    ret_index=-3;//<<
                    break;
                }

                ret_index=curpage_index+curpage*count-(curpage)*2;
                break;
            }

            if(curpage>0 && curpage < page_all-1){//中间
                if(curpage_index==0){
                    ret_index=-3;//<<
                    break;
                }
                if(curpage_index==count-1){
                    ret_index=-2;//>>
                    break;
                }
                ret_index=curpage_index+curpage*count-(curpage)*2;
                break;
            }
        }
    }
    return content.count() <= ret_index ? -4 : ret_index ;
}

void frontViewDialog_autoexpand::add_item(const QString &index, const QString &text, bool _is_use_pixmap, const QPixmap &icon)
{
    content.append(content_item_icon(index, text, _is_use_pixmap, icon));
}

void frontViewDialog_autoexpand::clearall_item()
{
    content.clear();
    generat_from_items();
}

void frontViewDialog_autoexpand::item_event_click()
{
    frontViewDialog_toolbtn *toolbtn=get_content_toolbtn(is_like_tab_save);
    if(toolbtn){
        QMouseEvent event(QEvent::MouseButtonRelease, QPoint(1,1), Qt::MouseButton(0), 0, 0);
        QApplication::sendEvent(toolbtn, &event);
    }
}

void frontViewDialog_autoexpand::setallitemcheck(bool enabled)
{
    foreach(const content_item_icon &c, content){
        setCurrentIndex(c.index, 1, enabled);
    }
}

void frontViewDialog_autoexpand::generat_from_items()
{
    glayout_clearall();
    page_all=calulate_page_all();
    //    if(page_all==0) return;
    for(int index=0,actual_index=0,index_count=horizontal_count*vertical_count;index<index_count;index++){
        int typeindex=calulate_content_index_from_curpage_index(index);
        QWidget *toolButton=0;
        switch(typeindex){
        case -1://填充
            toolButton=new QLabel(this);

            break;
        case -4://填充
            toolButton=new QLabel(this);

            break;
        case -2://>>
        {
            QAbstractButton *b;
            toolButton = b = new frontViewDialog_toolbtn(this);b->setText(">>");
            connect(b,SIGNAL(clicked()),this,SIGNAL(signal_next()));
        }

            break;
        case -3://<<
        {
            QAbstractButton *b;
            toolButton = b = new frontViewDialog_toolbtn(this);b->setText("<<");
            connect(b,SIGNAL(clicked()),this,SIGNAL(signal_pre()));
        }

            break;
        default:
            if(content[typeindex].toolbtn == 0){
                content[typeindex].toolbtn=new frontViewDialog_toolbtn(this);
                content[typeindex].toolbtn->index=content[typeindex].index;
                content[typeindex].toolbtn->setAutoExclusive(is_like_tab);
                content[typeindex].toolbtn->is_use_pixmap=content[typeindex].is_use_pixmap;
                content[typeindex].toolbtn->setIcon(content[typeindex].icon);
                content[typeindex].toolbtn->setIconSize(tool_icon_size);
                content[typeindex].toolbtn->presscolor=presscolor;
                content[typeindex].toolbtn->isdollar=btn_is_dollar;
                connect(content[typeindex].toolbtn,SIGNAL(thisclicked(QString)),this,SIGNAL(signal_text(QString)));
                connect(content[typeindex].toolbtn,SIGNAL(thispressed(frontViewDialog_toolbtn *)),this,SIGNAL(signal_rb_pressed(frontViewDialog_toolbtn *)));

                if(is_like_tab&&is_like_tab_save.isEmpty())is_like_tab_save=content[typeindex].index;
            }
            //2
            content[typeindex].toolbtn->setText(content[typeindex].text);

            //3
            toolButton =  content[typeindex].toolbtn;
            if(content_tool_save_map.find(content[typeindex].index)==content_tool_save_map.end()){
                content_tool_save_map.insert(content[typeindex].index, static_cast<frontViewDialog_toolbtn*>(toolButton));
            }
            break;
        }

        toolButton->setFixedSize(tool_size);
        toolButton->setProperty("outer_stylesheet", btn_property);
        glayout->addWidget(toolButton, actual_index/horizontal_count,actual_index%horizontal_count);
        if(toolButton->isHidden())toolButton->show();
        actual_index++;
    }
}

void frontViewDialog_autoexpand::set_btn_property(const QString &property)
{
    btn_property=property;
}

void frontViewDialog_autoexpand::enable_dollar()
{
    btn_is_dollar = true;
}

QStringList frontViewDialog_autoexpand::hasSelected()
{
    QStringList list;
    foreach(const content_item_icon &c, content){
        if(c.toolbtn
                &&c.toolbtn->isChecked()){
            list.append(c.toolbtn->index);
        }
    }
    return list;
}

QList<frontViewDialog_toolbtn *> frontViewDialog_autoexpand::hasSelectedRb()
{
    QList<frontViewDialog_toolbtn *> list;
    foreach(const content_item_icon &c, content){
        if(c.toolbtn
                &&c.toolbtn->isChecked()){
            list.append(c.toolbtn);
        }
    }
    return list;
}

QStringList frontViewDialog_autoexpand::hasSelectedText()
{
    QStringList list;
    foreach(const content_item_icon &c, content){
        if(c.toolbtn&&c.toolbtn->isChecked())list.append(c.toolbtn->text());
    }
    return list;
}

void frontViewDialog_autoexpand::clearSelected()
{
    foreach(const content_item_icon &c, content){
        if(c.toolbtn&&c.toolbtn->isChecked()){
            c.toolbtn->setChecked(false);
            c.toolbtn->setNum(1);//默认的数量
        }
    }
}

QStringList frontViewDialog_autoexpand::curallText()
{
    QStringList list;
    foreach(const content_item_icon &c, content){
        list.append(c.toolbtn->text());
    }
    return list;
}

void frontViewDialog_autoexpand::setCurrentIndex(const QString &index)
{
    setCurrentIndex(index, 1);
}

void frontViewDialog_autoexpand::setCurrentIndex(const QString &index, float num)
{
    setCurrentIndex(index, num, true);
}

void frontViewDialog_autoexpand::setCurrentIndex(const QString &index, float num, bool ischecked)
{
    for(int k = 0, count = content.count(); k < count; k++) {

        if(content[k].toolbtn == 0) {
            content[k].toolbtn=new frontViewDialog_toolbtn(this);
            content[k].toolbtn->index=content[k].index;
            content[k].toolbtn->setAutoExclusive(is_like_tab);
            content[k].toolbtn->is_use_pixmap=content[k].is_use_pixmap;
            content[k].toolbtn->setIcon(content[k].icon);
            content[k].toolbtn->setIconSize(tool_icon_size);
            content[k].toolbtn->presscolor=presscolor;
            content[k].toolbtn->isdollar=btn_is_dollar;
            connect(content[k].toolbtn,SIGNAL(thisclicked(QString)),this,SIGNAL(signal_text(QString)));
            connect(content[k].toolbtn,SIGNAL(thispressed(frontViewDialog_toolbtn *)),this,SIGNAL(signal_rb_pressed(frontViewDialog_toolbtn *)));
        }
        if(content_tool_save_map.find(content[k].index)==content_tool_save_map.end()){
            content[k].toolbtn->hide();
        }

        if(content[k].toolbtn&&content[k].toolbtn->index == index){
            content[k].toolbtn->setChecked(ischecked);
            content[k].toolbtn->setNum(num);
            break;
        }
    }
}
