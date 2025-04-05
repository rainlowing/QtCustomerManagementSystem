#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPushButton>
#include <QMessageBox>
#include <QSqlTableModel>
#include <QSqlRelationalTableModel>
#include <QString>
#include <QDialog>
#include <QGroupBox>
#include <QFormLayout>
#include <QLineEdit>
#include <QComboBox>
#include <QVariantMap>
#include <QLabel>
#include <QTimer>
#include <QTableView>
#include <QFileDialog>

#include <QXlsx/header/xlsxdocument.h>
#include <QXlsx/header/xlsxformat.h>

#include "databasemanager.h"
#include "logmanager.h"
#include "addnewconsumption.h"
#include "updateconsumption.h"
#include "searchcomsumption.h"
#include "addnewcustomer.h"
#include "searchcustomer.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    void connects();
    void initManagers();
    void initTimer();
    void setTables();
    void closeAllCPForm();
    void closeAllCTForm();

private slots:
    void onQuitButtonClicked();

    void updateTimer();

    void onCPAddButtonClicked();
    void handleCodeFromAddConsumption(int code);

    void onCPUpdateButtonClicked();
    void handleCodeFromUpdateConsumption(int code);

    void onCPSearchButtonClicked();
    void handleCodeFromSearchConsumption(int code);

    void onCPDeleteButtonClicked();

    void onCPExportButtonClicked();

    void onCTAddButtonClicked();
    void handleCodeFromAddCustomer(int code);

    void onCTSearchButtonClicked();
    void handleCodeFromSearchCustomer(int code);

    void onCTExportButtonClicked();

    void selectConsumption(const QString &condition);
    void selectCustomer(const QString &condition);
    void refreshConsumptionTableView();
    void refreshCustomerTableView();
    void exportToExcel(QTableView *tableView, const QString &filePath);

private:
    QString                                         message;
    Ui::MainWindow                                  *ui;
    DatabaseManager                                 *m_dbManager;
    QSqlRelationalTableModel                        *m_customerModel;
    QSqlRelationalTableModel                        *m_consumptionModel;

    QLabel                                          *m_timeLabel;
    QTimer                                          *m_timer;

    // 表单界面指针
    AddNewConsumption                               *m_addNewConsumption = nullptr;
    UpdateConsumption                               *m_updateConsumption = nullptr;
    SearchComsumption                               *m_searchConsumption = nullptr;
    AddNewCustomer                                  *m_addNewCustomer    = nullptr;
    SearchCustomer                                  *m_searchCustomer    = nullptr;
};
#endif // MAINWINDOW_H
