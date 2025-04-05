#include "updatecustomer.h"
#include "ui_updatecustomer.h"

UpdateCustomer::UpdateCustomer(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::UpdateCustomer)
{
    ui->setupUi(this);
}

UpdateCustomer::~UpdateCustomer()
{
    delete ui;
}
