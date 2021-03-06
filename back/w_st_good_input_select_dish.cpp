#include "w_st_good_input_select_dish.h"
#include "ui_w_st_good_input_select_dish.h"
#include "lds_model_sqltablemodel_delegate_com.h"
#include "lds_query.h"
#include "backheader.h"
#include <QTimer>
#include <QWidgetAction>
#include "lds_menu.h"

w_st_good_input_select_dish::w_st_good_input_select_dish(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::w_st_good_input_select_dish)
{
    ui->setupUi(this);
    //! treewidget
    ui->treeWidget->header()->hide();
    ui->treeWidget->setColumnCount(1);

    //! tableview
    tablemodel=new lds_model_sqltablemodel(this);
    tablemodel->setTable("cey_bt_dish");
    tablemodel->setEditStrategy(lds_model_sqltablemodel::OnManualSubmit);
    
    ui->tableView->setTransferHheader();
    ui->tableView->setEditTriggers(QTableView::AllEditTriggers);
    ui->tableView->setModel(tablemodel);
    ui->tableView->setItemDelegateForColumn(tablemodel->fieldIndex("ch_unitno"), new lds_model_sqltablemodel_delegate_com(
                                                this, tablemodel, "cey_bt_unit","ch_unitno","vch_unitname"
                                                ));
    ui->tableView->setItemDelegateForColumn(tablemodel->fieldIndex("vch_pur_unitno"), new lds_model_sqltablemodel_delegate_com(
                                                this, tablemodel, "cey_bt_unit","ch_unitno","vch_unitname"
                                                ));
    ui->tableView->setSelectionMode(QTableView::MultiSelection);
    ui->tableView->justShowColumns(QList<int>()
                                << tablemodel->fieldIndex("ch_dishno")
                               << tablemodel->fieldIndex("vch_dishname")
                               << tablemodel->fieldIndex("ch_unitno")
                               << tablemodel->fieldIndex("vch_pur_unitno")
                               << tablemodel->fieldIndex("num_sale_price")
                               );
    connect(ui->treeWidget,SIGNAL(pressed(QModelIndex)),this,SLOT(treeselect()));
    connect(ui->lineEdit,SIGNAL(textChanged(QString)),this,SLOT(treeselect()));
    connect(ui->pushButton_selectall, SIGNAL(clicked()), ui->tableView, SLOT(toSelectAll()));
    connect(ui->pushButton_selectno, SIGNAL(clicked()), ui->tableView, SLOT(toClearAll()));
    connect(ui->pushButton_ok, SIGNAL(clicked()),this,SLOT(took()));
    connect(ui->pushButton_cancel, SIGNAL(clicked()),this,SLOT(toexit()));

    //menu
    ui->label_desc->setFixedSize(200, 200);
    ui->label_desc->setWordWrap(true);
    ui->label_desc->setAlignment(Qt::AlignTop | Qt::AlignLeft);
    lds_menu *menu=new lds_menu(this);
    QWidgetAction *widgetaction = new QWidgetAction(ui->pushButton_question);
    widgetaction->setDefaultWidget(ui->label_desc);
    menu->addAction(widgetaction);
    ui->pushButton_question->setMenu(menu);

    this->setWindowTitle(tr("选择菜品"));
    resize(800, 560);
    //select
    QTimer::singleShot(0, this, SLOT(torefresh()));
}

w_st_good_input_select_dish::~w_st_good_input_select_dish()
{
    delete ui;
}

QVariant w_st_good_input_select_dish::get_tablemodel_value(int row, const QString &valuename)
{
    return tablemodel->model_data(row, valuename);
}

void w_st_good_input_select_dish::filter_by_itemflag()
{
    s_filter_by_itemflag = " and ifnull(item_flag, '') = 'Y' ";
    ui->label_desc->setText(ui->label_desc->text() +"\n" "item_flag");
}

void w_st_good_input_select_dish::filter_by_material()
{
    s_filter_by_material = " and ifnull(ch_material_flag, '') = 'Y' ";
    ui->label_desc->setText(ui->label_desc->text() +"\n" "ch_material_flag");
}

void w_st_good_input_select_dish::filter_by_st_check()
{
    s_filter_by_st_check =
            " and "
            " ("
            "not exists (select cey_st_check_detail.ch_materialno "
            " from cey_st_check_master, cey_st_check_detail"
            " where cey_st_check_master.ch_sheetno = cey_st_check_detail.ch_sheetno"
            " and cey_st_check_master.ch_state = 'N'  and cey_st_check_detail.ch_materialno = cey_bt_dish.ch_dishno)"
            ")"
            ;
    ui->label_desc->setText(ui->label_desc->text() +"\n" "st_check");
}

void w_st_good_input_select_dish::filter_by_no_ch_dishno(const QString &ch_dishno)
{
    s_filter_by_no_ch_dishno = QString(" and ch_dishno <> '%1'  ").arg(ch_dishno);
    ui->label_desc->setText(ui->label_desc->text() +"\n" "no_ch_dishno:" + ch_dishno);
}

void w_st_good_input_select_dish::filter_by_no_ch_dishno_2(const QString &ch_dishno)
{
    s_filter_by_no_ch_dishno_2 = QString(" and ch_dishno <> '%1'  ").arg(ch_dishno);
    ui->label_desc->setText(ui->label_desc->text() +"\n" "no_ch_dishno_2:" + ch_dishno);
}

void w_st_good_input_select_dish::filter_by_no_dish_suit()
{
    s_filter_by_no_dish_suit = QString(" and ifnull(ch_suitflag, '') <> 'Y' ");
    ui->label_desc->setText(ui->label_desc->text() +"\n" "no_dish_suit");
}

void w_st_good_input_select_dish::filter_by_no_dish_tempflag()
{
    s_filter_by_no_dish_temp = QString(" and ifnull(ch_tempflag, '') <> 'Y' ");
    ui->label_desc->setText(ui->label_desc->text() +"\n" "no_dish_temp");
}

void w_st_good_input_select_dish::enable_single_selected()
{
    ui->tableView->setSelectionMode(QTableView::SingleSelection);
}

void w_st_good_input_select_dish::justShowColumns(const QStringList &vch_columns)
{
    QList<int> int_columns;
    foreach(const QString &column, vch_columns) {
        int_columns << tablemodel->fieldIndex(column);
    }
    ui->tableView->justShowColumns(int_columns);
}

void w_st_good_input_select_dish::treeselect()
{
    QString ch_dish_typeno;
    QString ch_sub_typeno;
    QString vch_spell;
    //tree检索
    if(ui->treeWidget->currentItem()){
        int index=ui->treeWidget->deepofitem(ui->treeWidget->currentItem());
        if(index>0){
            if(index==1) {
                ch_dish_typeno=backheader::getinterdata(ui->treeWidget->currentItem()->text(0));
            }
            if(index==2) {
                ch_sub_typeno=backheader::getinterdata(ui->treeWidget->currentItem()->text(0));
            }
        }
    }
    //过滤
    vch_spell = ui->lineEdit->text().toUpper();
    //检索
    tablemodel->setFilter(QString("ch_dish_typeno like '%1%'  and ch_sub_typeno like '%2%'  and (ch_dishno like '%3%' or vch_spell like '%3%')"
                                  " and  ifnull(ch_stopflag, '')  <> 'Y' "    //没有停用
                                  + s_filter_by_itemflag //库存管理
                                  + s_filter_by_material //物料
                                  + s_filter_by_st_check //未盘点
                                  + s_filter_by_no_ch_dishno //不匹配商品1
                                  + s_filter_by_no_ch_dishno_2 //不匹配商品2
                                  + s_filter_by_no_dish_suit //不匹配主套菜
                                  + s_filter_by_no_dish_temp
                                  )
                          .arg(ch_dish_typeno)
                          .arg(ch_sub_typeno)
                          .arg(vch_spell)
                          );
    ui->tableView->restore();
    ui->tableView->resizeColumnsToContents();
}

void w_st_good_input_select_dish::torefresh()
{
    //refresh
    lds_query query;
    //treewidget
    ui->treeWidget->deleteAllItem();
    ui->treeWidget->rootitem=new QTreeWidgetItem(QStringList()
                                                 <<tr("全部类型"));
    ui->treeWidget->addTopLevelItem(ui->treeWidget->rootitem);

    query.execSelect("select  concat(vch_dish_typename,'[',ch_dish_typeno,']')     as 'nametype', ch_dish_typeno  from cey_bt_dish_type"
                     );
    while(query.next()){
        QTreeWidgetItem *item=new QTreeWidgetItem(QStringList()
                                                  <<query.recordValue("nametype").toString()
                                                  );
        ui->treeWidget->rootitem->addChild(item);
        //获取[]的内容
        lds_query query_1;
        QString no=query.recordValue("nametype").toString();
        no=no.split("[")[1];
        no.chop(1);
        query_1.execSelect(QString(
                               "select concat(vch_sub_typename,'[',ch_sub_typeno,']') as 'nametype' from cey_bt_dish_subtype where ch_dish_typeno = '%1' order by ch_sub_typeno asc;"
                               )
                           .arg(no)
                           );
        while(query_1.next()){
            QTreeWidgetItem *item1=new QTreeWidgetItem(QStringList()
                                                       <<query_1.recordValue("nametype").toString()
                                                       );
            item->addChild(item1);
        }
    }
    ui->treeWidget->setItemSizeHint();//更新所有的 item的sizehint

    ui->treeWidget->restore();
    //
    treeselect();
}

void w_st_good_input_select_dish::took()
{
    ret_row_list = ui->tableView->getSelectedRowListAsc();
    if(ret_row_list.isEmpty()) {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, tr("没有数据被选中"));
        return;
    }
    this->accept();
}

void w_st_good_input_select_dish::toexit()
{
    this->reject();
}
