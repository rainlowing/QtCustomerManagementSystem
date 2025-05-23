#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QCoreApplication>
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
#include <QItemSelectionModel>
#include <QStandardItem>
#include <QStyledItemDelegate>

#include <QXlsx/header/xlsxdocument.h>
#include <QXlsx/header/xlsxformat.h>

#include "databasemanager.h"
#include "logmanager.h"
#include "addnewconsumption.h"
#include "updateconsumption.h"
#include "searchcomsumption.h"
#include "addnewcustomer.h"
#include "updatecustomer.h"
#include "searchcustomer.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class ReadOnlyDelegate : public QStyledItemDelegate {
public:
    QWidget* createEditor(QWidget *parent,
                          const QStyleOptionViewItem &option,
                          const QModelIndex &index) const override {
        return nullptr; // 阻止编辑器创建
    }
};

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
    void setTableViews();
    void closeAllCPForm();
    void closeAllCTForm();
    void refreshAllTableViews();

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

    void onCTUpdateButtonClicked();
    void handleCodeFromUpdateCustomer(int code);

    void onCTSearchButtonClicked();
    void handleCodeFromSearchCustomer(int code);

    void onCTDeleteButtonClicked();

    void onCTExportButtonClicked();

    // void onDASearchButtonClicked();

    void onDAExportButtonClicked();

    void selectConsumption(const QString &condition);
    void selectCustomer(const QString &condition);
    void refreshConsumptionTableView();
    void refreshCustomerTableView();
    void refreshDailyTableView();
    void exportToExcel(QTableView *tableView, const QString &filePath);

    void dailySalesSum();

private:
    QString                                         message;
    Ui::MainWindow                                  *ui;
    DatabaseManager                                 *m_dbManager;
    QSqlRelationalTableModel                        *m_customerModel;
    QSqlRelationalTableModel                        *m_consumptionModel;
    QSqlRelationalTableModel                        *m_dailyModel;

    QLabel                                          *m_timeLabel;
    QTimer                                          *m_timer;

    // 表单界面指针
    AddNewConsumption                               *m_addNewConsumption = nullptr;
    UpdateConsumption                               *m_updateConsumption = nullptr;
    SearchComsumption                               *m_searchConsumption = nullptr;
    AddNewCustomer                                  *m_addNewCustomer    = nullptr;
    UpdateCustomer                                  *m_updateCustomer    = nullptr;
    SearchCustomer                                  *m_searchCustomer    = nullptr;
};
#endif // MAINWINDOW_H
