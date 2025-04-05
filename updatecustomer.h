#ifndef UPDATECUSTOMER_H
#define UPDATECUSTOMER_H

#include <QDialog>

namespace Ui {
class UpdateCustomer;
}

class UpdateCustomer : public QDialog
{
    Q_OBJECT

public:
    explicit UpdateCustomer(QWidget *parent = nullptr);
    ~UpdateCustomer();

private:
    Ui::UpdateCustomer *ui;
};

#endif // UPDATECUSTOMER_H
