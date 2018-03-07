#ifndef FEXPANDMAIN_TABLE_SUB_CP_VIEW_H
#define FEXPANDMAIN_TABLE_SUB_CP_VIEW_H

#include <QFrame>

namespace Ui {
class fexpandmain_table_sub_cp_view;
}

class fexpandmain_table_sub_cp_view : public QFrame
{
    Q_OBJECT

public:
    explicit fexpandmain_table_sub_cp_view(QWidget *parent = 0);
    ~fexpandmain_table_sub_cp_view();

private:
    Ui::fexpandmain_table_sub_cp_view *ui;
};

#endif // FEXPANDMAIN_TABLE_SUB_CP_VIEW_H
