#ifndef SEARCHCUSTOMER_H
#define SEARCHCUSTOMER_H

#include <QDialog>
#include <QVariantMap>
#include <QStringList>

#include "databasemanager.h"

namespace Ui {
class SearchCustomer;
}

class SearchCustomer : public QDialog
{
    Q_OBJECT

public:
    explicit SearchCustomer(DatabaseManager *dbManager, QWidget *parent = nullptr);
    ~SearchCustomer();

    void setBaseInfo();

private:
    bool isValid();

signals:
    void returnCode(int code);
    void selectCustomer(const QString &condition);

private slots:
    void handleDialogAccepted();
    void handleDialogCleared();
    void handleDialogRejected();

    void on_nameCheck_checkStateChanged(const Qt::CheckState &arg1);

    void on_birthdayCheck_checkStateChanged(const Qt::CheckState &arg1);

    void on_genderCheck_checkStateChanged(const Qt::CheckState &arg1);

    void on_amountCheck_checkStateChanged(const Qt::CheckState &);

    void on_greaterAndLessCheck_checkStateChanged(const Qt::CheckState &);

private:
    Ui::SearchCustomer      *ui;
    DatabaseManager         *m_dbManager;
    QVariantMap             m_data;
};

#endif // SEARCHCUSTOMER_H
