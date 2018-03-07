#include "lds_lineedit.h"
#include <QTimer>
#include <QtDebug>
#include <QPainter>

lds_lineedit::lds_lineedit(QWidget *parent):
    QLineEdit(parent){
    connect(this,SIGNAL(getfocus()),this,SLOT(selectAll()),Qt::QueuedConnection);//必须队列排队才可以的
}

lds_lineedit::~lds_lineedit()
{
}

void lds_lineedit::disableSelectAllWhenGetFocus()
{
    disconnect(this,SIGNAL(getfocus()),this,SLOT(selectAll()));//必须队列排队才可以的
}

void lds_lineedit::setLittleText(const QString &little_text, const QColor &little_text_color)
{
    this->little_text = little_text;
    this->little_text_color = little_text_color;
    update();
}

void lds_lineedit::tokey(int key)
{
    switch(key) {
    case Qt::Key_0:
    case Qt::Key_1:
    case Qt::Key_2:
    case Qt::Key_3:
    case Qt::Key_4:
    case Qt::Key_5:
    case Qt::Key_6:
    case Qt::Key_7:
    case Qt::Key_8:
    case Qt::Key_9:
        insert(QString::number(key - Qt::Key_0));
        break;
    case Qt::Key_Backspace:
        backspace();
        break;
    case Qt::Key_Return:
    case Qt::Key_Enter:
        if(!text().isEmpty()) {
            emit signal_unvoid_returnPressed();
        } else {
            emit signal_void_returnPressed();
        }
        break;
    }
}

void lds_lineedit::focusOutEvent(QFocusEvent *e)
{
    QLineEdit::focusOutEvent(e);
    emit outfocus();
}

void lds_lineedit::focusInEvent(QFocusEvent *e)
{
    QLineEdit::focusInEvent(e);
    emit getfocus();
}

void lds_lineedit::paintEvent(QPaintEvent *e)
{
    QLineEdit::paintEvent(e);
    if(little_text.length() > 0) {
        QPainter painter(this);
        painter.setPen(little_text_color);
        QFont f = this->font();
        f.setPixelSize(8);
        painter.setFont(f);
        painter.drawText(this->rect().adjusted(5, 5, -5, -5), int(Qt::AlignRight | Qt::AlignBottom), little_text);
    }
}
