#include "updateconsumption.h"
#include "ui_updateconsumption.h"

UpdateConsumption::UpdateConsumption(DatabaseManager *dbManager, QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::UpdateConsumption)
    , m_dbManager(dbManager)
{
    ui->setupUi(this);
    ui->nameCombo->setFocus();
    ui->amountEdit->setValidator(new QDoubleValidator(this));

    connect(ui->btnConfirm, &QPushButton::clicked, this, &UpdateConsumption::handleDialogAccepted);
    connect(ui->btnCancel, &QPushButton::clicked, this, &UpdateConsumption::handleDialogRejected);
}


void UpdateConsumption::setBaseInfo(const QVariantMap &data) {
    m_dbManager->selectAllName(ui->nameCombo);
    m_dbManager->selectAllService(ui->serviceCombo);

    m_data = data;

    ui->IDLabel->setText(data["consumption_id"].toString());
    ui->nameCombo->setCurrentText(data["name"].toString());
    ui->serviceCombo->setCurrentText(data["service"].toString());
    ui->amountEdit->setText(data["amount"].toString());
    ui->timeLabel->setText(data["time"].toString());
    ui->notePlain->setPlainText(data["note"].toString());
}


bool UpdateConsumption::isValid() {
    if (ui->nameCombo->currentText().isEmpty() ||
        ui->amountEdit->text().isEmpty() ||
        ui->amountEdit->text().toDouble() < 0.00 ||
        ui->amountEdit->text().toDouble() > 99999999.99 ) {
        return false;
    }
    return true;
}


void UpdateConsumption::handleDialogAccepted() {
    if (!isValid()) {
        emit returnCode(100);
        return;
    }

    QString name = ui->nameCombo->currentText();
    if (!m_dbManager->isCustomerExists(name)) {
        emit returnCode(101);
        return;
    }

    QVariantMap data;
    data["consumption_id"] = ui->IDLabel->text();
    data["name"] = name;
    data["service"] = ui->serviceCombo->currentText();
    data["amount"] = ui->amountEdit->text();
    data["note"] = ui->notePlain->toPlainText();

    m_data.remove("time");
    if (m_data == data) {
        emit returnCode(1);
        return;
    }

    if (m_dbManager->updateCP(data)) {
        emit returnCode(0);
        this->close();
    } else {
        emit returnCode(200);
    }
    return;
}

void UpdateConsumption::handleDialogRejected() {
    this->close();
}



UpdateConsumption::~UpdateConsumption()
{
    delete ui;
}

