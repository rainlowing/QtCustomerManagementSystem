#ifndef SEARCHCOMSUMPTION_H
#define SEARCHCOMSUMPTION_H

#include <QDialog>
#include <QDoubleValidator>
#include <QCheckBox>
#include <QVariantMap>

#include "databasemanager.h"

namespace Ui {
class SearchComsumption;
}

class SearchComsumption : public QDialog
{
    Q_OBJECT

public:
    explicit SearchComsumption(DatabaseManager *dbManager, QWidget *parent = nullptr);
    ~SearchComsumption();

    void setBaseInfo();

private:
    bool isValid();


signals:
    void returnCode(int code);
    void selectConsumption(const QString &condition);

private slots:
    void handleDialogAccepted();
    void handleDialogCleared();
    void handleDialogRejected();

    void on_nameCheck_checkStateChanged(const Qt::CheckState &arg1);

    void on_serviceCheck_checkStateChanged(const Qt::CheckState &arg1);

    void on_timeCheck_checkStateChanged(const Qt::CheckState &arg1);

    void on_amountCheck_checkStateChanged(const Qt::CheckState &);

    void on_greaterAndLessCheck_checkStateChanged(const Qt::CheckState &);


private:
    Ui::SearchComsumption       *ui;
    DatabaseManager             *m_dbManager;
    QVariantMap                 m_data;
    int                         code;
    QList<QVariantMap>          result;
    bool                        m_check = false;
};

#endif // SEARCHCOMSUMPTION_H
