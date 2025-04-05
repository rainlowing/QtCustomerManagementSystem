#include "addnewconsumption.h"
#include "ui_addnewconsumption.h"

AddNewConsumption::AddNewConsumption(DatabaseManager *dbManager, QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::AddNewConsumption)
    , m_dbManager(dbManager)
{
    ui->setupUi(this);
    ui->nameCombo->setFocus();
    ui->amountEdit->setValidator(new QDoubleValidator(this));

    connect(ui->btnConfirm, &QPushButton::clicked, this, &AddNewConsumption::handleDialogAccepted);
    connect(ui->btnCancel, &QPushButton::clicked, this, &AddNewConsumption::handleDialogRejected);
}

AddNewConsumption::~AddNewConsumption() {
    delete ui;
}

void AddNewConsumption::setBaseInfo() {
    ui->IDLabel->setText(m_dbManager->getNewConsumptionID());
    m_dbManager->selectAllName(ui->nameCombo);
    ui->nameCombo->setCurrentIndex(-1);
    m_dbManager->selectAllService(ui->serviceCombo);
    ui->serviceCombo->setCurrentIndex(-1);
    ui->timeLabel->setText(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"));
}

bool AddNewConsumption::isValid() {
    if (ui->nameCombo->currentText().isEmpty() ||
        ui->amountEdit->text().isEmpty() ||
        ui->amountEdit->text().toDouble() < 0.00 ||
        ui->amountEdit->text().toDouble() > 99999999.99) {
        return false;
    }
    return true;
}


void AddNewConsumption::handleDialogAccepted() {
    if (!isValid()) {
        emit returnCode(100);
        return;
    }

    m_data["consumption_id"] = ui->IDLabel->text();
    m_data["name"] = ui->nameCombo->currentText();
    m_data["service"] = ui->serviceCombo->currentText();
    m_data["amount"] = ui->amountEdit->text();
    m_data["time"] = ui->timeLabel->text();
    m_data["note"] = ui->notePlain->toPlainText();

    if (m_dbManager->insertCP(m_data)) {
        emit returnCode(0);
        this->close();
    } else {
        emit returnCode(200);
    }
    return;
}

void AddNewConsumption::handleDialogRejected() {
    this->close();
}
