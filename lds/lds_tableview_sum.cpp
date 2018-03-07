#include "lds_tableview_sum.h"
#include <QTableView>
#include <QVBoxLayout>
#include <QScrollBar>
#include <QDebug>
#include <QHeaderView>
#include <QStandardItemModel>
#include "lds_tableview.h"
#include "lds_model_sqlstandardmodel.h"

lds_tableview_sum::lds_tableview_sum(QWidget *parent) :
    QWidget(parent)
{
    //tableView
    tableView = new lds_tableView(this);
    tableView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    tableView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    tableView->setShowNest(true);
    //lineEdit
    lineView = new lds_tableView(this);
    lineView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    lineView->horizontalHeader()->hide();
    lineView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    lineView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    lineView->verticalHeader()->setDefaultSectionSize(30);
    lineView->setFixedHeight(30);
    lineView->setStyleSheet("color:#ff650b;");
//    lineView->setVerticalHeader(new lineViewHeader(Qt::Vertical, lineView));

    //layout
    QGridLayout *gridLayout = new QGridLayout();
    QVBoxLayout *verticalLayout = new QVBoxLayout();
    QHBoxLayout *hblayout=new QHBoxLayout;
    verticalLayout->addWidget(tableView);
    verticalLayout->addWidget(lineView);
    verticalLayout->addLayout(hblayout);
    gridLayout->addLayout(verticalLayout, 0, 0, 1, 1);
    gridLayout->addWidget(tableView->verticalScrollBar(), 0, 1, 1, 1);
    gridLayout->addWidget(tableView->horizontalScrollBar(), 1, 0, 1, 1);

    verticalLayout->setSpacing(0);
    verticalLayout->setContentsMargins(0, 0, 0, 0);
    gridLayout->setSpacing(0);
    gridLayout->setContentsMargins(0, 0, 0, 0);
    hblayout->setSpacing(0);
    hblayout->setContentsMargins(0,0,0,0);
    setLayout(gridLayout);

    //bar
    tableView->horizontalScrollBar()->hide();
    tableView->verticalScrollBar()->hide();
    //lineView
    linemodel=new lds_model_sqlstandardmodel(this);
    linemodel->enableNoItemFlags();
    lineView->setModel(linemodel);
    lineView->hide();
}

void lds_tableview_sum::setModel(QAbstractItemModel *model)
{
    //table setmodel
    parentmodel=model;
    tableView->setModel(model);
    //QItemSelectionModel
    QItemSelectionModel *selectionModel;
    if(tableView->selectionModel()){
        selectionModel=tableView->selectionModel();
    } else {
        selectionModel=new QItemSelectionModel(parentmodel);
        tableView->setSelectionModel(selectionModel);
    }

    //水平滚动条保存同步
    connect(tableView->horizontalScrollBar(),SIGNAL(valueChanged(int)),lineView->horizontalScrollBar(),SLOT(setValue(int)));
    //水平垂直滚动条的显示
    connect(tableView->horizontalScrollBar(),SIGNAL(rangeChanged(int,int)),this,SLOT(update_tableview_bar(int,int)));
    connect(tableView->verticalScrollBar(),SIGNAL(rangeChanged(int,int)),this,SLOT(update_tableview_bar(int,int)));

    //更新lineview的数据，只保留一行
    linemodel_remove_and_filldata();
    lineView->verticalHeader()->setHidden(!parentmodel->rowCount());

}

QStringList lds_tableview_sum::updateSumRange(int from, int to)
{
    QStringList ret_list;
    for(;from <= to; from ++) {
        ret_list << updateSum(from);
    }
    return ret_list;
}

void lds_tableview_sum::update_tableview_bar( int min, int max)
{
    //更新两个滚动条显示情况
    Q_UNUSED(min)
    QScrollBar *bar=qobject_cast<QScrollBar *>(sender());
    if(!bar) return;
    bar->setVisible(max>0);
}

void lds_tableview_sum::update_lineview_header(int logicalIndex, int oldSize, int newSize)
{
    //同步sectionwidth
    Q_UNUSED(oldSize)
    lineView->horizontalHeader()->resizeSection(logicalIndex, newSize);
}

QString lds_tableview_sum::updateSum(int column, const QVariant &value)
{
    updatelinemodel();
    //更新lineview的数据，只保留一行
    if(linemodel->rowCount()<=0
            ||linemodel->columnCount()<=column) {
        return "";
    }

    int vitual_column=column;
    int actual_column=0;
    for(int c=0,c_count=tableView->horizontalHeader()->count();c<c_count;c++){
        if(vitual_column<=0)break;
        actual_column++;
        if(tableView->isColumnHidden(c)){
        } else {
            vitual_column--;
        }
    }

    QString retv;
    if(linemodel->rowCount() > 0 && linemodel->columnCount() > actual_column){
        linemodel->item(0, actual_column)->setData(value, Qt::DisplayRole);
        retv = lineView->hh_paint_transfer(actual_column);
    } else {
        qDebug() << "linemodel row_x_column not adapted";
    }
    return retv;

}

void lds_tableview_sum::setEditTriggers(QAbstractItemView::EditTriggers triggers)
{
    tableView->setEditTriggers(triggers);
}

void lds_tableview_sum::refreshSizeForContent()
{
    tableView->resizeColumnsToContents();
    lineView->resizeColumnsToContents();

    for(int c=0,c_count=tableView->horizontalHeader()->count();c<c_count;c++){
        if(tableView->isColumnHidden(c)) {
            lineView->setColumnHidden(c, tableView->isColumnHidden(c));
        } else {
            tableView->setColumnWidth(c, qMax(lineView->columnWidth(c), tableView->columnWidth(c)));
            lineView->setColumnWidth(c, tableView->columnWidth(c));
        }
    }
}

void lds_tableview_sum::setTransferHheader()
{
    tableView->setTransferHheader();
    lineView->setTransferHheader();
}

QString lds_tableview_sum::updateSum(int column)
{
    updatelinemodel();
    //更新lineview的数据，只保留一行
    if(linemodel->rowCount()<=0
            ||linemodel->columnCount()<=column) {
        return "";
    }

    QVariant::Type sum_type = QVariant::Invalid;
    double f_sum=0.0;
    for(int r=0,r_count=parentmodel->rowCount();r<r_count;r++){
        QVariant d = parentmodel->index(r, column).data();
        f_sum += d.toDouble();
        if(sum_type == QVariant::Invalid)
            sum_type = qMax(sum_type, d.type());
    }
    QVariant sum;
    if(sum_type >= QVariant::Double) {
        sum = f_sum;
    } else {
        sum = qlonglong(f_sum);
    }

    linemodel->item(0, column)->setData(sum, Qt::DisplayRole);
    return lineView->hh_paint_transfer(column);
}

void lds_tableview_sum::linerefresh()
{
    if(parentmodel->columnCount() != linemodel->columnCount()){
        //更新行数和列数
        linemodel_remove_and_filldata();

        linemodel->setColumnCount(parentmodel->columnCount());
        linemodel->setRowCount(1);
        //更新section的宽度，所有;更新隐藏列
    }

    refreshSizeForContent();
}

void lds_tableview_sum::scrollbarinfo(int step)
{
}

void lds_tableview_sum::linemodel_remove_and_filldata()
{
    linemodel->removeRows(0, linemodel->rowCount());
    for(int c=0,c_count=parentmodel->columnCount();c<c_count;c++){
        if(c==0){
            linemodel->setItem(0, c, new QStandardItem(tr("合计")));
        } else {
            linemodel->setItem(0, c, new QStandardItem(""));
        }
        linemodel->setHeaderData(c, Qt::Horizontal, parentmodel->headerData(c, Qt::Horizontal));
    }
}

void lds_tableview_sum::updatelinemodel()
{
    if(linemodel->rowCount() == 0 || linemodel->columnCount() == 0){
        linemodel_remove_and_filldata();
    }
    lineView->verticalHeader()->setHidden(!parentmodel->rowCount());
    linemodel->setHeaderData(0, Qt::Vertical, parentmodel->rowCount());
    lineView->show();

}

void lds_tableview_sum::update_lineview_selection(const QItemSelection &selected, const QItemSelection &deselected)
{
    Q_UNUSED(deselected)
    //有选中
    if(selected.indexes().count()>0){

        //选中最后一列
        int e=selected.indexes().last().isValid()?selected.indexes().last().column():-1;
        lineView->selectColumn(e);
        //更新水平滚动条
        lineView->horizontalScrollBar()->setValue(tableView->horizontalScrollBar()->value());
    }
}

void lds_tableview_sum::update_tableview_verticalheader()
{
    if(linemodel->columnCount() != parentmodel->columnCount()){
        linerefresh();
    }
    linemodel->setVerticalHeaderLabels(QStringList() << QString("%1").arg(parentmodel->rowCount()));
    lineView->verticalHeader()->setHidden(!parentmodel->rowCount());
}

QVariant lds_tableview_sum::RedModel::data(const QModelIndex &index, int role) const
{
    QVariant _d=QStandardItemModel::data(index,role);

    if(role==Qt::TextColorRole){
        return QColor("#ff650b");
    }
    if(role==Qt::BackgroundColorRole){
        return QPalette().alternateBase();
    }
    return _d;
}

void lds_tableview_sum::ItemDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    return lds_tableview_delegate_virtual::paint(painter, option, index);

    QPen pen;
    pen.setColor(QColor("red"));
    //默认的背景色
    QColor color_2=qvariant_cast<QColor>(index.data(Qt::BackgroundRole));
    if(color_2.isValid()){
        painter->setPen(Qt::NoPen);
        painter->setBrush(color_2);
        painter->drawRect(option.rect);
    }

    //选中状态的背景色
    if(option.state & QStyle::State_Selected){
        QColor color=option.palette.highlight().color();
        painter->setPen(Qt::NoPen);
        painter->setBrush(color);
        painter->drawRect(option.rect);
        pen.setColor(QColor("white"));
    }

    if(index.data().toString().length() != 0) {
        QFont font = painter->font();
        QFontMetrics fontm(font);
        qreal fontr =  fontm.height() * 1.0 / fontm.width("0");
        qDebug() << fontr << option.rect.width() << index.data().toString() << index.data().toString().length();
        font.setPixelSize(qMin(qreal(option.rect.height() -10)  ,fontr * (option.rect.width() / index.data().toString().length()) - 2));
        painter->setFont(font);
    }
    //绘制文字
    painter->setPen(pen);
    int flags = Qt::AlignVCenter|Qt::AlignLeft;
    painter->drawText(option.rect, flags, index.data().toString());
}

QString lds_tableview_sum::lineViewHeader::paint_transfer(int logicalIndex) const
{
    return lds_tableview_header::paint_transfer(logicalIndex);
}
