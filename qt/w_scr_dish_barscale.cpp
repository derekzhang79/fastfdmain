#include "w_scr_dish_barscale.h"
#include "lds_query.h"
#include "labelprint_goodinfo_add_dialog.h"

#include "lds_query.h"
#include "lds_messagebox.h"

w_scr_dish_barscale::w_scr_dish_barscale(QWidget *parent) :
    barscale_main_dialog(parent)
{
}

void w_scr_dish_barscale::togoodadd()
{
    //1
    QList<BARSCAEL_ITEMS> items;
    labelprint_goodinfo_add_dialog dialog(this);
    dialog.model->setHorizontalHeaderLabels(QStringList() << tr("商品名") << tr("商品代码") << tr("价格")<<tr("有效期"));
    lds_query query;
    query.execSelect("select vch_dishname, vch_barcode, num_price,'' as dt_validdate from cey_bt_dish where vch_barcode <> '' ");
    while(query.next()){
        dialog.model->appendRow(QList<QStandardItem*>()
                                <<new QStandardItem(query.recordValue("vch_dishname").toString())
                                <<new QStandardItem(query.recordValue("vch_barcode").toString())
                                <<new QStandardItem(query.recordValue("num_price").toString())
                                <<new QStandardItem(query.recordValue("dt_validdate").toString())
                                );
    }
    //2
    if(QDialog::Accepted==lds_roundeddialog_rect_align(&dialog).exec()){
        QModelIndexList indexlist=dialog.ui->tableView->selectionModel()->selectedIndexes();
        QSet<int> index_list;
        foreach(QModelIndex index, indexlist){
            index_list.insert(index.row());
        }
        foreach(int index, index_list){
            items.append(BARSCAEL_ITEMS(
                             ""/*plu为空*/,
                             dialog.model->item(index, 0)->text(),
                             dialog.model->item(index, 1)->text(),
                             dialog.model->item(index, 2)->text(),
                             "")
                         );
        }
    }
    goodsadd(items);
}
