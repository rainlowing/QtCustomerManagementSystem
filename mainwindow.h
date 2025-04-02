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

#include "databasemanager.h"
#include "logmanager.h"
#include "addnewconsumption.h"
#include "updateconsumption.h"


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
    void setTables();
    QGroupBox *createCPAddFormGroup();
    QGroupBox *createCPUpdateFormGroup(int cr);

private slots:
    void onQuitButtonClicked();
    void onCPAddButtonClicked();
    void onCPUpdateButtonClicked();
    // void onCPSearchButtonClicked();
    // void onCPUpdateButtonClicked();
    // void onCPDeleteButtonClicked();
    // void onCTAddButtonClicked();
    // void onCTSearchButtonClicked();
    // void onCTUpdateButtonClicked();
    // void onCTDeleteButtonClicked();
    void refreshTableView();

private:
    QString                                         message;
    Ui::MainWindow                                  *ui;
    DatabaseManager                                 *m_dbManager;
    QSqlRelationalTableModel                        *m_customerModel;
    QSqlRelationalTableModel                        *m_consumptionModel;

    // 表单界面指针
    AddNewConsumption                               *m_addNewConsumption = nullptr;
    UpdateConsumption                               *m_updateConsumption = nullptr;
};
#endif // MAINWINDOW_H
