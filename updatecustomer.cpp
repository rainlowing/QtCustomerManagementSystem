#include "updatecustomer.h"
#include "ui_updatecustomer.h"

UpdateCustomer::UpdateCustomer(DatabaseManager *dbManager, QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::UpdateCustomer)
    , m_dbManager(dbManager)
{
    ui->setupUi(this);
    ui->nameEdit->setFocus();

    connect(ui->btnConfirm, &QPushButton::clicked, this, &UpdateCustomer::handleDialogAccepted);
    connect(ui->btnCancel, &QPushButton::clicked, this, &UpdateCustomer::handleDialogRejected);
}


void UpdateCustomer::setBaseInfo(const QVariantMap &data) {
    m_data = data;

    ui->IDLabel->setText(data["customer_id"].toString());
    ui->nameEdit->setText(data["name"].toString());
    QString gender = data["gender"].toString();
    if (gender == "女") {
        ui->femaleRadio->setChecked(true);
    } else if (gender == "男") {
        ui->maleRadio->setChecked(true);
    } else {
        ui->noRadio->setChecked(true);
    }
    QDate birthday = QDate::fromString(data["birthday"].toString(), "yyyy-MM-dd");
    ui->birthdayEdit->setDate(birthday);
    ui->phoneEdit->setText(data["phone"].toString());
    ui->notePlain->setPlainText(data["note"].toString());
}


bool UpdateCustomer::isValid() {
    if (ui->nameEdit->text().isEmpty()) {
        return false;
    }
    return true;
}


void UpdateCustomer::handleDialogAccepted() {
    if (!isValid()) {
        emit returnCode(100);
        return;
    }

    QVariantMap data;
    data["customer_id"] = ui->IDLabel->text();
    data["name"] = ui->nameEdit->text();
    if (ui->femaleRadio->isChecked()) {
        data["gender"] = "女";
    } else if (ui->maleRadio->isChecked()) {
        data["gender"] = "男";
    } else {
        data["gender"] = "未知";
    }
    data["birthday"] = ui->birthdayEdit->date().toString("yyyy-MM-dd");
    data["phone"] = ui->phoneEdit->text();
    data["note"] = ui->notePlain->toPlainText();

    if (m_data == data) {
        emit returnCode(1);
        return;
    }

    QString name = ui->nameEdit->text();
    if (m_data["name"] != data["name"] && m_dbManager->isCustomerExists(name)) {
        emit returnCode(101);
        return;
    }

    if (m_dbManager->updateCT(data)) {
        emit returnCode(0);
        this->close();
    } else {
        emit returnCode(200);
    }
    return;
}


void UpdateCustomer::handleDialogRejected() {
    this->close();
}


UpdateCustomer::~UpdateCustomer()
{
    delete ui;
}
