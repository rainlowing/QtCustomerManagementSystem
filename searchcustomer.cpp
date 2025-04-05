#include "searchcustomer.h"
#include "ui_searchcustomer.h"

SearchCustomer::SearchCustomer(DatabaseManager *dbManager, QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::SearchCustomer)
    , m_dbManager(dbManager)
{
    ui->setupUi(this);
    ui->nameCheck->setFocus();

    connect(ui->btnConfirm, &QPushButton::clicked, this, &SearchCustomer::handleDialogAccepted);
    connect(ui->btnClear, &QPushButton::clicked, this, &SearchCustomer::handleDialogCleared);
    connect(ui->btnCancel, &QPushButton::clicked, this, &SearchCustomer::handleDialogRejected);
    connect(ui->amountCheck, &QCheckBox::checkStateChanged, this, &SearchCustomer::on_amountCheck_checkStateChanged);
    connect(ui->greaterCheck, &QCheckBox::checkStateChanged, this, &SearchCustomer::on_greaterAndLessCheck_checkStateChanged);
    connect(ui->lessCheck, &QCheckBox::checkStateChanged, this, &SearchCustomer::on_greaterAndLessCheck_checkStateChanged);
}

void SearchCustomer::setBaseInfo() {
    m_dbManager->selectAllName(ui->nameCombo);
    this->handleDialogCleared();
}


bool SearchCustomer::isValid() {
    if (!ui->nameCheck->isChecked() && ui->nameCombo->currentText().isEmpty()) {
        return false;
    }

    if (ui->greaterCheck->isChecked() && (ui->greaterEdit->text().isEmpty() || ui->greaterEdit->text().toDouble() < 0.00)) {
        return false;
    }

    if (ui->lessCheck->isChecked() && (ui->lessEdit->text().isEmpty() || ui->lessEdit->text().toDouble() > 99999999.99)) {
        return false;
    }

    if (!ui->birthdayCheck->isChecked() && ui->startTime->date() > ui->endTime->date()) {
        return false;
    }

    return true;
}

void SearchCustomer::handleDialogAccepted() {
    if (!isValid()) {
        emit returnCode(100);
        return;
    }

    if (!ui->nameCheck->isChecked()) {
        m_data["name"] = ui->nameCombo->currentText();
    } else {
        m_data["name"] = "";
    }

    m_data["gender"] = "";
    if (!ui->genderCheck->isChecked()) {
        if (ui->femaleRadio->isChecked()) {
            m_data["gender"] = "女";
        } else if (ui->maleRadio->isChecked()) {
            m_data["gender"] = "男";
        } else {
            m_data["gender"] = "未知";
        }
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
    if (!ui->birthdayCheck->isChecked()) {
        m_data["start_time"] = ui->startTime->date().toString("yyyy-MM-dd");
        m_data["end_time"] = ui->endTime->date().toString("yyyy-MM-dd");
    }

    QStringList selectFields;

    if (!m_data["name"].toString().isEmpty()) {
        selectFields.append("name = '" + m_data["name"].toString() + "'");
    }

    if (!m_data["gender"].toString().isEmpty()) {
        selectFields.append("gender = '" + m_data["gender"].toString() + "'");
    }

    selectFields.append("total_amount BETWEEN " + m_data["min_amount"].toString() + " AND " + m_data["max_amount"].toString());

    if (!ui->birthdayCheck->isChecked()) {
         selectFields.append("birthday BETWEEN '" + m_data["start_time"].toString() + "' AND '" + m_data["end_time"].toString() + "'");
    }

    QString condition = selectFields.join(" AND ");
    emit selectCustomer(condition);
    emit returnCode(0);

    return;
}

void SearchCustomer::handleDialogCleared() {
    ui->nameCheck->setChecked(true);
    ui->nameCombo->setCurrentIndex(-1);
    ui->nameCombo->setDisabled(true);

    ui->genderCheck->setChecked(true);
    ui->femaleRadio->setChecked(false);
    ui->femaleRadio->setDisabled(true);
    ui->maleRadio->setChecked(false);
    ui->maleRadio->setDisabled(true);
    ui->noRadio->setChecked(false);
    ui->noRadio->setDisabled(true);

    ui->amountCheck->setChecked(true);

    ui->greaterCheck->setChecked(false);
    ui->greaterEdit->setText("0.00");
    ui->greaterEdit->setReadOnly(true);
    ui->greaterEdit->setEnabled(false);

    ui->lessCheck->setChecked(false);
    ui->lessEdit->setText("99999999.99");
    ui->lessEdit->setReadOnly(true);
    ui->lessEdit->setEnabled(false);

    ui->birthdayCheck->setChecked(true);
    ui->startTime->setEnabled(false);
    ui->startTime->setDate(QDate(2000, 01, 01));
    ui->endTime->setEnabled(false);
    ui->endTime->setDate(QDate(2099, 12, 31));

    emit selectCustomer("");
    emit returnCode(1);
}

void SearchCustomer::handleDialogRejected() {
    emit selectCustomer("");
    this->close();
}

SearchCustomer::~SearchCustomer()
{
    delete ui;
}


void SearchCustomer::on_nameCheck_checkStateChanged(const Qt::CheckState &arg1) {
    if (arg1 == Qt::Checked) {
        ui->nameCombo->setCurrentIndex(-1);
        ui->nameCombo->setDisabled(true);
    } else {
        ui->nameCombo->setDisabled(false);
    }
}


void SearchCustomer::on_birthdayCheck_checkStateChanged(const Qt::CheckState &arg1) {
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


void SearchCustomer::on_genderCheck_checkStateChanged(const Qt::CheckState &arg1) {
    if (arg1 == Qt::Checked) {
        ui->femaleRadio->setChecked(false);
        ui->femaleRadio->setDisabled(true);
        ui->maleRadio->setChecked(false);
        ui->maleRadio->setDisabled(true);
        ui->noRadio->setChecked(false);
        ui->noRadio->setDisabled(true);
    } else {
        ui->femaleRadio->setDisabled(false);
        ui->femaleRadio->setChecked(true);
        ui->maleRadio->setDisabled(false);
        ui->maleRadio->setChecked(false);
        ui->noRadio->setChecked(true);
        ui->noRadio->setDisabled(false);
    }
}

void SearchCustomer::on_amountCheck_checkStateChanged(const Qt::CheckState &) {
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


void SearchCustomer::on_greaterAndLessCheck_checkStateChanged(const Qt::CheckState &) {
    if (!ui->greaterCheck->isChecked() && !ui->lessCheck->isChecked()) {
        ui->amountCheck->setChecked(true);
    }
}

