#ifndef FEXPANDMAIN
#define FEXPANDMAIN

////餐桌部分
#include <QString>

enum tableState{
    tableState_NONE = 1,           //none
    tableState_KONGXIAN=2,         //空闲
    tableState_YUDING = 4,         //预定
    tableState_KAITAI=8,           //开台
    tableState_YUDA = 16,          //预打
    tableState_DISABLE = 32,       //不可选
    tableState_BAR = 64,          //预打
    tableState_QR_CODE = 128,          //预打

};

class fexpandmain {
public:

    fexpandmain(){}
};

#endif // FEXPANDMAIN

