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

#include "databasemanager.h"
#include "logmanager.h"
#include "addnewconsumption.h"
#include "updateconsumption.h"
#include "searchcomsumption.h"

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
    void closeAllCPFrom();

private slots:
    void onQuitButtonClicked();

    void updateTimer();

    void onCPAddButtonClicked();
    void handleCodeFromAddConsumption(int code);

    void onCPUpdateButtonClicked();
    void handleCodeFromUpdateConsumption(int code);

    void onCPSearchButtonClicked();
    void handleCodeFromSearchConsumption(int code);

    void selectConsumption(const QString &condition);
    void refreshConsumptionTableView();
    void refreshCustomerTableView();

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
    SearchComsumption                               *m_SearchConsumption = nullptr;
};
#endif // MAINWINDOW_H
