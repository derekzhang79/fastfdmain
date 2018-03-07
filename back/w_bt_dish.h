#ifndef W_BT_DISH_H
#define W_BT_DISH_H

#include <QDialog>
#include "lds_tableview_repecify.h"
#include <QStandardItemModel>
#include "backheader.h"
#include "lds.h"

class QComboBox;
class w_bt_dish : public QDialog
{
    Q_OBJECT
public:
    explicit w_bt_dish(QWidget *parent = 0);
private:
    //基础资料
    static QStandardItemModel   MODEL_DISH_DEDUCTTYPE;//t提成方式
    //基础资料
    static QStandardItemModel   MODEL_DISH_SERVICE_MODE;//服务费模式
    static QStandardItemModel   MODEL_DISH_SERVICE_MODE_CUT;//服务费模式
    static QStandardItemModel   MODEL_DISH_SERVICE_MODE_CUT_HELP;//服务费模式
    //基础资料
    static QStandardItemModel   MODEL_DISH_LOW_MODE;//低消方式
    //基础资料
    static QStandardItemModel   MODEL_UNIT_NUM_TYPE;//数量类型
    //基础资料
    static QStandardItemModel   MODEL_UNIT;//单位
    //餐桌资料
    static QStandardItemModel   MODEL_TABLE_SERVICE_FLAG_MODE;//Y:折前服务率; N:折后服务率
    //餐桌人数说明
    static QStandardItemModel   MODEL_TABLE_INT_PERSON;

    static QList< QPair<QString, QString> > kvaddresslist;
    static QMap<QString, QString> kcmap;
    static QMap<QString, QString> kcmapdesc;
public:
    static void clearCache();
    //基础资料
    static QStandardItemModel&   get_MODEL_DISH_DEDUCTTYPE();//t提成方式
    //基础资料
    static QStandardItemModel&   get_MODEL_DISH_SERVICE_MODE();//服务费模式
    static QStandardItemModel&   get_MODEL_DISH_SERVICE_MODE_CUT();//服务费模式
    static QStandardItemModel&   get_MODEL_DISH_SERVICE_MODE_CUT_HELP();//服务费模式
    //基础资料
    static QStandardItemModel&   get_MODEL_DISH_LOW_MODE();//低消方式
    //基础资料
    static QStandardItemModel&   get_MODEL_UNIT_NUM_TYPE();//数量类型
    //基础资料
    static QStandardItemModel&   get_MODEL_UNIT();//单位
    //餐桌资料
    static QStandardItemModel&   get_MODEL_TABLE_SERVICE_FLAG_MODE();//Y:折前服务率; N:折后服务率
    //餐桌人数说明
    static QStandardItemModel&   get_MODEL_TABLE_INT_PERSON();

    static QMap<QString, QString>& get_kcmap();
    static QMap<QString, QString>& get_kcmapdesc();
    static QList< QPair<QString, QString> >& get_kvaddresslist();


    static void fillcomdata(QComboBox *c, QMap<QString, QVariant> &m);
    static void fillcomdata2(QComboBox *c, QMap<QString, QVariant> *m);

    enum kitchenState{
        KITCHEN_MIN_DEF = 0x00,
        KITCHEN_FINISH,//厨打完成
        KITCHEN_VOID ,//置空不在使用
        KITCHEN_QUIT,//退菜
        KITCHEN_CUICAI ,//催菜

        //5
        KITCHEN_JIAOQI ,//叫起
        KITCHEN_GUAQI ,//挂起
        KITCHEN_ZHUANTAI ,//转台
        KITCHEN_FUZHI ,//菜品复制
        KITCHEN_BINGTAI,//并台

        KITCHEN_JiACAI,//加菜单
        KITCHEN_WAIMAI,//加菜单
        KITCHEN_ZHUANTAI_CAIPIN,//菜品转台
        KITCHEN_QUIT_KUAICAN,//快餐退菜

        /*0x0b*/KITCHEN_MAX,//最大值
        ///最大为16个、还有2个空余
    };
    Q_DECLARE_FLAGS(kitchenStates, kitchenState)

    static void get_vch_plan_k3o3(QString &state, QString &id, const QString &ch_dishno, kitchenState s0 = KITCHEN_MIN_DEF);

    static char k2o3_state2hex_char(kitchenState state);
    static QString k2o3_stae2hex_str(kitchenState state);
    static QString k2o3_name(kitchenState state);
    static QMap<int, QString>& k2o3_get_state_name_map_ref();

    static QString model_data(QAbstractItemModel *m, int row, int column);

private:
};

Q_DECLARE_OPERATORS_FOR_FLAGS(w_bt_dish::kitchenStates)
#endif // W_BT_DISH_H
