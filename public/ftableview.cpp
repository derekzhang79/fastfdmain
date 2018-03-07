#include "ftableview.h"
#include <QHeaderView>
#include "ftableview_delegate.h"
#include "ftableview_standmodel_virtual.h"
#include <QtDebug>
#include <QTimeLine>
#include <QLabel>
#include <QMouseEvent>
#include <QStateMachine>
#include <QSignalTransition>
#include <QPropertyAnimation>
#include <QTimer>
#include <QMovie>
#include <QEventLoop>
#include <QElapsedTimer>


//////////////////////////////////////样式表//////////////////////////////////////


void ftableview::setbgNormalColor10(const QColor &c)
{
    bgNormalColor10 = c;
}

QColor ftableview::getbgNormalColor10()
{
    return bgNormalColor10;
}

void ftableview::setTextColor(const QColor &c)
{
    textColor = c;
}

QColor ftableview::getTextColor()
{
    return textColor;
}

void ftableview::setbgBorderColor(const QColor &c)
{
    bgBorderColor = c;
}

QColor ftableview::getbgBorderColor()
{
    return bgBorderColor;
}

void ftableview::setbgExpand0(const QColor &c)
{
    bgExpand0 = c;
}

QColor ftableview::getbgExpand0()
{
    return bgExpand0;
}
//////////////////////////////////////样式表//////////////////////////////////////


ftableview::ftableview(QWidget *parent)
    : lds_tableview_timeout(parent)
{
    ////////////////////////////////////////////////////////////////////////////////////////////
    m = 0;
    this->verticalHeader()->hide();
    this->horizontalHeader()->hide();
    this->setEditTriggers(QTableView::NoEditTriggers);
    this->setFocusPolicy(Qt::NoFocus);
    this->setShowGrid(false);

    this->setSelectionBehavior(QTableView::SelectItems);
    this->setSelectionMode(QTableView::SingleSelection);
    this->verticalHeader()->setDefaultSectionSize(100);
    this->horizontalHeader()->setDefaultSectionSize(100);
    this->setIconSize(QSize(80,60));

    connect(this,SIGNAL(clicked(QModelIndex)),this,SLOT(tocenter(QModelIndex)));
}

void ftableview::setModel(ftableview_standmodel_virtual *model)
{
    this->m = model;
    QTableView::setModel(m);
}

void ftableview::enableSelectOneWhenMultiSelected()
{
    if(selectionMode() == QTableView::MultiSelection){
        connect(this, SIGNAL(released(QModelIndex)),this, SLOT(tocheckoneselect(QModelIndex)));
    }
}

void ftableview::enable_mouse_press_after_select()
{
    is_enable_mouse_press_after_select = true;
}

void ftableview::disConnectCenter()
{
    disconnect(this,SIGNAL(clicked(QModelIndex)),this,SLOT(tocenter(QModelIndex)));
}

void ftableview::updateFixed_fillData_addAnimation(int rcount, int ccount, int width, int height)
{
    if(m == 0)qDebug()<<"model is 0!!";

    updateFixed(rcount, ccount, width, height);
    //填充内容
    fillData(rcount, ccount);
    //3 动态添加图片
    addAnimation(rcount, ccount);
}

void ftableview::centerControl(const QModelIndex &index)
{
    QString key = index.data(Qt::UserRole+1).toString();
    if(key == "-1"){
        m->togeneratePre();
    } else if(key == "-2"){
        m->togenerateNext();
    } else if(key == "-3") {
        emit signalclickNew(index);
    }
}

void ftableview::fillData_addAnimation(int rcount, int ccount)
{
    if(m == 0)qDebug()<<"model is 0!!";
    updateFixed(rcount, ccount, 0, 0);
    //填充内容
    fillData(rcount, ccount);
    //3 动态添加图片
    addAnimation(rcount, ccount);
}

void ftableview::updateFixed_fillData(int rcount, int ccount, int width, int height)
{
    if(m == 0)qDebug()<<"model is 0!!";

    updateFixed(rcount, ccount, width, height);
    //填充内容
    fillData(rcount, ccount);
}

void ftableview::updateFixed(int rcount, int ccount, int width, int height)
{
    if(width == 0) {
        this->setHeaderResizeMode(Qt::Horizontal, QHeaderView::Stretch);
    } else {
        this->horizontalHeader()->setDefaultSectionSize(width);
        this->setFixedWidth(width*ccount);
    }
    if(height == 0) {
        this->setHeaderResizeMode(Qt::Vertical, QHeaderView::Stretch);
    } else {
        this->verticalHeader()->setDefaultSectionSize(height);
        this->setFixedHeight(height*rcount);
    }
}

void ftableview::addAnimation(int rcount, int ccount)
{
    QStateMachine *machine = this->findChild<QStateMachine *>();
    if(machine) {
        delete machine;
        machine = 0;
    }
    if(machine == 0) {
        //3 动态添加图片
        machine = new QStateMachine(this);
        QState *s1 = new QState(machine);
        QState *s2 = new QState(machine);

        s1->assignProperty(m, "step", 0);
        s2->assignProperty(m, "step", rcount*ccount);

        QPropertyAnimation *anm = new QPropertyAnimation(m, "step");
        anm->setDuration(2000);
        anm->setEasingCurve(QEasingCurve::OutQuad);

        QSignalTransition *trans;
        trans = s1->addTransition(m, SIGNAL(signalstepstart()),s2);
        trans->addAnimation(anm);
        trans = s2->addTransition(m, SIGNAL(signalstepstop()),s1);
        connect(s2,SIGNAL(propertiesAssigned()),m,SIGNAL(signalstepstop()));
        machine->setInitialState(s1);
        machine->start();
    }
}

void ftableview::fillData(int rcount, int ccount)
{
    m->set_row_column(rcount, ccount);
}

QStringList ftableview::get_usr_list(int role)
{
    QStringList tablenolist;
    QModelIndexList indexlist = selectionModel()->selectedIndexes();
    foreach(const QModelIndex &index, indexlist) {
        tablenolist.append(index.data(role).toString());
    }
    return tablenolist;
}

void ftableview::tocenter(const QModelIndex &index)
{
    QString key = index.data(Qt::UserRole+1).toString();
    if(key.toInt() < 0) {
        centerControl(index);
        return;
    }
    emit signalclick(index);
    emit signalclick(index.data(Qt::UserRole+1).toString());
    if(selectedIndexes().contains(index)) {
        emit signalselectedclick(index);
    }
}

void ftableview::tocheckoneselect(const QModelIndex &index)
{
    QString key = index.data(Qt::UserRole+1).toString();

    if(key.toInt() < 0 || key.isEmpty()) {//上一页、下一页、新增 ，空白处
        this->selectionModel()->select(index, QItemSelectionModel::Deselect);
        return;
    }
    QModelIndexList indexlist = this->selectedIndexes();
    for(int k = 0, count  = indexlist.count(); k < count; k++){
        if(indexlist[k] == index) {

        } else {
            this->selectionModel()->select(indexlist[k], QItemSelectionModel::Deselect);
        }
    }
}

void ftableview::mousePressEvent(QMouseEvent *event)
{
    QModelIndex index = indexAt(event->pos());
    if(is_enable_mouse_press_after_select
            && selectionModel()->selectedIndexes().contains(index)) {
        QString key = index.data(Qt::UserRole + 1).toString();
        if(key != "-1" && key != "-2" && !key.isEmpty()) {
            emit signal_enable_mouse_press_after_select(index);
            return;
        }
    }
    lds_tableview_timeout::mousePressEvent(event);
    //
}

void ftableview::enablePop(popTypes type)
{
    _type = type;
    connect(this,SIGNAL(signalselectedclick(QModelIndex)),this,SLOT(toshowpop(QModelIndex)));
}

void ftableview::toshowpop(const QModelIndex &index)
{

}
