#include "lds_menu.h"
#include <QVBoxLayout>

static void showWidget(QWidget *w)
{
    if(w->isHidden())
        w->show();
}
lds_menu::lds_menu(QWidget *parent) : QMenu(parent)
{
    this->setWindowFlags(Qt::Popup);
    widget = 0;
}

lds_menu::~lds_menu()
{
}

void lds_menu::setPopWidget(QWidget *widget)
{
    this->widget = widget;
    connect(this, SIGNAL(aboutToShow()), this,SLOT(addPopWidget()));
}

void lds_menu::addPopWidget()
{
    if(widget
            && this->actions().count() == 0) {
        QWidgetAction *widgetaction=new QWidgetAction(this);
        widget->resize(widget->sizeHint());
        showWidget(widget);
        widgetaction->setDefaultWidget(widget);
        this->addAction(widgetaction);
    }
}


