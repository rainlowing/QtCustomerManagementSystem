#ifndef UPDATECONSUMPTION_H
#define UPDATECONSUMPTION_H

#include <QDialog>
#include <QVariantMap>
#include <QMessageBox>
#include <QPushButton>
#include <QDoubleValidator>

#include "databasemanager.h"

namespace Ui {
class UpdateConsumption;
}

class UpdateConsumption : public QDialog
{
    Q_OBJECT

public:
    explicit UpdateConsumption(DatabaseManager *dbManager, QWidget *parent = nullptr);
    ~UpdateConsumption();

    void setBaseInfo(const QVariantMap &data);

private:
    bool isValid();

private slots:
    void handleDialogAccepted();
    void handleDialogRejected();

signals:
    void returnCode(int code);

private:
    Ui::UpdateConsumption   *ui;
    DatabaseManager         *m_dbManager;
    QVariantMap             m_data;
};

#endif // UPDATECONSUMPTION_H
