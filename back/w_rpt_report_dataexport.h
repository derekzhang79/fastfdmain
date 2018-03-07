#ifndef W_RPT_REPORT_DATAEXPORT_H
#define W_RPT_REPORT_DATAEXPORT_H

#include <QDialog>
#include <QTableView>
#include "lds_tableview.h"

class QAbstractItemModel;
namespace Ui {
class w_rpt_report_dataExport;
}

//windowtitle 即为导出文件的文件名
class w_rpt_report_dataExport : public QDialog
{
    Q_OBJECT

public:
    explicit w_rpt_report_dataExport(lds_tableView *tableview,
                                     lds_tableView *tail_tableview,QWidget *parent = 0);
    ~w_rpt_report_dataExport();
    void defFilepath(const QString &tabtext);
public slots:
    void toexit();
    void toexport();
    void toexport_sql();
    void toumount_upan();
    void export_info_progress(const QString &info, int v, int total);
    void togetfilepath();
private:
    Ui::w_rpt_report_dataExport *ui;

    lds_tableView *tail_tableview;

    ///////////
public:
    w_rpt_report_dataExport(lds_tableView *content_tableview,
                            const QString &sql, lds_tableView *tail_tableview, QWidget *parent = 0);
private:
    QString sql;
    lds_tableView *content_tableview;
};

#endif // W_RPT_REPORT_DATAEXPORT_H
