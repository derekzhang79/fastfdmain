#ifndef BACKHEADER_H
#define BACKHEADER_H

#include <QObject>
#include <QSize>
#include <QStandardItemModel>
#include "public_mainprinter.h"
#include "lds_tableview_repecify.h"

#include "w_sys_manage_cloudsync.h"
#include "lds_combobox.h"
#include "lds_query.h"
#include "public_sql.h"
#include <QSqlQueryModel>
//__FILE__
//__FUNCTION__
//__LINE__
//Qt::Dialog|Qt::WindowTitleHint;//只有logo和close的dialog
//Qt::Dialog|Qt::WindowTitleHint|Qt::CustomizeWindowHint; //什么都没有的dialog


class backheader : public QObject
{
    Q_OBJECT

public:
    explicit backheader(QObject *parent = 0);

    static QRect static_calculat_center_rect(QWidget *dialog, const QSize &size, int flag);
    static void static_dialogshow(QDialog &dialog, const QString &title, const QSize &size=QSize(), int flag=Qt::AlignCenter);
    static void static_dialogshow(QWidget &dialog, const QString &title, const QSize &size=QSize(), int flag=Qt::AlignCenter);

    static bool operid_right_code(QWidget *parent, const QString& purview_no);

    static void standmodelfilldata(QStandardItemModel *model, const QString &sql);
    static QString getinterdata(const QString &_data_);//xx[data]
    static QString getouterdata(const QString &_data_);//xx[data]
    static QString getouterdata(const QStringList &_data_s, const QString &cmpvalue_1);//xx[data]
    static int set_index_of_interdata(const QString &inter, QComboBox *c);
    static public_mainprinter printer;//异步打印机
    /*异步*/
    static void asyn_dataprint(const blockIns &printdata);//开始打印
    static void asyn_dataprint(const blockIns &printdata, const QString &path);//开始打印
    static void asyn_dataprint(const QString &printdata, const QString &path);//开始打印
    static void asyn_dataprin_test(const QString &path="");//开始打印
    static QString print_def_path();

    /*同步*/
    static bool syn_dataprint(const blockIns &printdata);//开始打印
    static bool syn_dataprint(const blockIns &printdata, const QString &path);//开始打印
    static void syn_dataprin_test(const QString &path="");//开始打印
    static void syn_dataprin_test_byresolveself(const QString &path, int instype, const QString & printtype, int paperwidth, int xinye_beep_count);//开始打印
    //
    static void resolve(const QString &path, lds_query &query, blockIns &ins);
private:
    static void resolve(int instype, const QString &printtype, int paperwidth, int xinye_beep_count, blockIns &ins);
public:
    //检索第几个的值
    static int VIPFINDVALUE(const QStandardItemModel &model, const QString &cmpvalue, int cmpindex);
    //查找第一列的值，返回第二列的值
    static QString VIPFINDVALUE(const QStandardItemModel &model, const QString &cmpvalue_0);
    static void modelSetHeader(QAbstractItemModel *model, const QStringList &headers);

    static QString SELECT_OPERID_NAME_FLAG;
    static QString SELECT_OPERID_NAME;

    static void regetvch_operid_name();//name(id)
    static QStringList vch_operid_name;
    const QMap<QString, QVariant> *get_operid_name_map();

    static void regetvch_dish_typename();//name(id)
    static QStringList vch_dish_typename;

    static void regetvch_unitname();//name(id)
    static QStringList vch_unitname;

    static void ldsComSetdataByKVM(const QAbstractItemModel *model, lds_ComboBox *com);
    static void fillData(QComboBox *com, const QMap<QString, QVariant> * map, bool first_is_empty = false, const QString &emptyTile = "");
    static void fillData(QComboBox *com, const QMap<int, QString> * map, bool first_is_empty = false, const QString &emptyTile = "");
    static void fillData(QComboBox *com, const QMap<QString, QString> * map, bool first_is_empty = false, const QString &emptyTile = "");
    static void fillData(QComboBox *com, const QAbstractItemModel *m, bool first_is_empty = false, const QString &emptyTile = "");
private:
    static QMap<QString, QVariant> operid_name_map;

};

//菜品检索的窗口

#endif // BACKHEADER_H
