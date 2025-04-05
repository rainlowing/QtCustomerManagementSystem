#include "addnewcustomer.h"
#include "ui_addnewcustomer.h"

AddNewCustomer::AddNewCustomer(DatabaseManager *dbManager, QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::AddNewCustomer)
    , m_dbManager(dbManager)
{
    ui->setupUi(this);
    ui->nameEdit->setFocus();

    connect(ui->btnConfirm, &QPushButton::clicked, this, &AddNewCustomer::handleDialogAccepted);
    connect(ui->btnCancel, &QPushButton::clicked, this, &AddNewCustomer::handleDialogRejected);
}



AddNewCustomer::~AddNewCustomer()
{
    delete ui;
}

void AddNewCustomer::setBaseInfo() {
    ui->IDLabel->setText(m_dbManager->getNewCustomerID());
}

bool AddNewCustomer::isValid() {
    if (ui->nameEdit->text().isEmpty()) {
        return false;
    }
    return true;
}


void AddNewCustomer::handleDialogAccepted() {
    if (!isValid()) {
        emit returnCode(100);
        return;
    }

    QString gender = "未知";
    if (ui->femaleRadio->isChecked()) {
        gender = "女";
    } else if (ui->maleRadio->isChecked()) {
        gender = "男";
    }

    m_data["customer_id"] = ui->IDLabel->text();
    m_data["name"] = ui->nameEdit->text();
    m_data["gender"] = gender;
    m_data["birthday"] = ui->birthadayEdit->date().toString("yyyy-MM-dd");
    m_data["phone"] = ui->phoneEdit->text();
    m_data["note"] = ui->notePlain->toPlainText();

    if (m_dbManager->insertCT(m_data)) {
        emit returnCode(0);
        this->close();
    } else {
        emit returnCode(200);
    }
    return;
}


void AddNewCustomer::handleDialogRejected() {
    this->close();
}
