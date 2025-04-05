#include "searchcomsumption.h"
#include "ui_searchcomsumption.h"

SearchComsumption::SearchComsumption(DatabaseManager *dbManager, QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::SearchComsumption)
    , m_dbManager(dbManager)
{
    ui->setupUi(this);
    ui->nameCheck->setFocus();
    ui->greaterEdit->setValidator(new QDoubleValidator(this));
    ui->lessEdit->setValidator(new QDoubleValidator(this));

    connect(ui->btnConfirm, &QPushButton::clicked, this, &SearchComsumption::handleDialogAccepted);
    connect(ui->btnClear, &QPushButton::clicked, this, &SearchComsumption::handleDialogCleared);
    connect(ui->btnCancel, &QPushButton::clicked, this, &SearchComsumption::handleDialogRejected);
    connect(ui->amountCheck, &QCheckBox::checkStateChanged, this, &SearchComsumption::on_amountCheck_checkStateChanged);
    connect(ui->greaterCheck, &QCheckBox::checkStateChanged, this, &SearchComsumption::on_greaterAndLessCheck_checkStateChanged);
    connect(ui->lessCheck, &QCheckBox::checkStateChanged, this, &SearchComsumption::on_greaterAndLessCheck_checkStateChanged);
}


void SearchComsumption::setBaseInfo() {
    m_dbManager->selectAllName(ui->nameCombo);
    m_dbManager->selectAllService(ui->serviceCombo);
    handleDialogCleared();
}


bool SearchComsumption::isValid() {
    if (!ui->nameCheck->isChecked() && ui->nameCombo->currentText().isEmpty()) {
        return false;
    }

    if (!ui->serviceCheck->isChecked() && ui->serviceCombo->currentText().isEmpty()) {
        return false;
    }

    if (ui->greaterCheck->isChecked() && (ui->greaterEdit->text().isEmpty() || ui->greaterEdit->text().toDouble() < 0.00)) {
        return false;
    }

    if (ui->lessCheck->isChecked() && (ui->lessEdit->text().isEmpty() || ui->lessEdit->text().toDouble() > 99999999.99)) {
        return false;
    }

    if (!ui->timeCheck->isChecked() && ui->startTime->date() > ui->endTime->date()) {
        return false;
    }

    return true;
}


void SearchComsumption::handleDialogAccepted() {
    if (!isValid()) {
        emit returnCode(100);
        return;
    }

    if (!ui->nameCheck->isChecked()) {
        m_data["name"] = ui->nameCombo->currentText();
    } else {
        m_data["name"] = "";
    }

    if (!ui->serviceCheck->isChecked()) {
        m_data["service"] = ui->serviceCombo->currentText();
    } else {
        m_data["service"] = "";
    }

    m_data["min_amount"] = 0.00;
    m_data["max_amount"] = 99999999.99;
    if (ui->greaterCheck->isChecked()) {
        m_data["min_amount"] = ui->greaterEdit->text().toDouble();
    }
    if (ui->lessCheck->isChecked()) {
        m_data["max_amount"] = ui->lessEdit->text().toDouble();
    }

    m_data["start_time"] = "2000-01-01";
    m_data["end_time"] = "2099-12-31";
    if (!ui->timeCheck->isChecked()) {
        m_data["start_time"] = ui->startTime->date().toString("yyyy-MM-dd");
        m_data["end_time"] = ui->endTime->date().toString("yyyy-MM-dd");
    }

    QStringList selectFields;

    if (!m_data["name"].toString().isEmpty()) {
            selectFields.append("name = '" + m_data["name"].toString() + "'");
    }

    if (!m_data["service"].toString().isEmpty()) {
        selectFields.append("service = '" + m_data["service"].toString() + "'");
    }

    selectFields.append("amount BETWEEN " + m_data["min_amount"].toString() + " AND " + m_data["max_amount"].toString());
    selectFields.append("time BETWEEN '" + m_data["start_time"].toString() + "' AND '" + m_data["end_time"].toString() + "'");

    QString condition = selectFields.join(" AND ");
    emit selectConsumption(condition);
    emit returnCode(0);

    return;
}


void SearchComsumption::handleDialogCleared() {
    ui->nameCheck->setChecked(true);
    ui->nameCombo->setCurrentIndex(-1);
    ui->nameCombo->setDisabled(true);

    ui->serviceCheck->setChecked(true);
    ui->serviceCombo->setCurrentIndex(-1);
    ui->serviceCombo->setDisabled(true);

    ui->amountCheck->setChecked(true);

    ui->greaterCheck->setChecked(false);
    ui->greaterEdit->setText("0.00");
    ui->greaterEdit->setReadOnly(true);
    ui->greaterEdit->setEnabled(false);

    ui->lessCheck->setChecked(false);
    ui->lessEdit->setText("99999999.99");
    ui->lessEdit->setReadOnly(true);
    ui->lessEdit->setEnabled(false);

    ui->timeCheck->setChecked(true);
    ui->startTime->setEnabled(false);
    ui->startTime->setDate(QDate(2000, 01, 01));
    ui->endTime->setEnabled(false);
    ui->endTime->setDate(QDate(2099, 12, 31));

    emit selectConsumption("");
    emit returnCode(1);
}


void SearchComsumption::handleDialogRejected() {
    emit selectConsumption("");
    this->close();
}


SearchComsumption::~SearchComsumption()
{
    delete ui;
}


void SearchComsumption::on_nameCheck_checkStateChanged(const Qt::CheckState &arg1) {
    if (arg1 == Qt::Checked) {
        ui->nameCombo->setCurrentIndex(-1);
        ui->nameCombo->setDisabled(true);
        ui->nameCombo->setEditable(false);
    } else {
        ui->nameCombo->setDisabled(false);
        ui->nameCombo->setEditable(true);
    }
}


void SearchComsumption::on_serviceCheck_checkStateChanged(const Qt::CheckState &arg1) {
    if (arg1 == Qt::Checked) {
        ui->serviceCombo->setCurrentIndex(-1);
        ui->serviceCombo->setDisabled(true);
        ui->serviceCombo->setEditable(false);
    } else {
        ui->serviceCombo->setDisabled(false);
        ui->serviceCombo->setEditable(true);
    }
}


void SearchComsumption::on_amountCheck_checkStateChanged(const Qt::CheckState &) {
    if (ui->amountCheck->isChecked()) {
        ui->greaterCheck->setChecked(false);
        ui->greaterCheck->setEnabled(false);

        ui->greaterEdit->setText("0.00");
        ui->greaterEdit->setReadOnly(true);
        ui->greaterEdit->setEnabled(false);

        ui->lessCheck->setChecked(false);
        ui->lessCheck->setEnabled(false);

        ui->lessEdit->setText("99999999.99");
        ui->lessEdit->setReadOnly(true);
        ui->lessEdit->setEnabled(false);
    } else {
        ui->greaterCheck->setChecked(true);
        ui->greaterCheck->setEnabled(true);

        ui->greaterEdit->setText("0.00");
        ui->greaterEdit->setReadOnly(false);
        ui->greaterEdit->setEnabled(true);

        ui->lessCheck->setChecked(true);
        ui->lessCheck->setEnabled(true);

        ui->lessEdit->setText("99999999.99");
        ui->lessEdit->setReadOnly(false);
        ui->lessEdit->setEnabled(true);
    }
}


void SearchComsumption::on_greaterAndLessCheck_checkStateChanged(const Qt::CheckState &) {
    if (!ui->greaterCheck->isChecked() && !ui->lessCheck->isChecked()) {
        ui->amountCheck->setChecked(true);
    }
}


void SearchComsumption::on_timeCheck_checkStateChanged(const Qt::CheckState &arg1) {
    if (arg1 == Qt::Checked) {
        ui->startTime->setEnabled(false);
        ui->startTime->setDate(QDate(2000, 01, 01));
        ui->endTime->setEnabled(false);
        ui->endTime->setDate(QDate(2099, 12, 31));
    } else {
        ui->startTime->setEnabled(true);
        ui->startTime->setReadOnly(false);
        ui->endTime->setEnabled(true);
        ui->endTime->setReadOnly(false);
    }
}














