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

#include "databasemanager.h"
#include "logmanager.h"


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
    QString autoGenerateCRID();
    bool handleCPAddDialogAccepted(QGroupBox *gb);
    bool handleCPSearchDialogAccepted(QGroupBox *gb);
    bool handleCPUpdateDialogAccepted(QGroupBox *gb);

private slots:
    void onQuitButtonClicked();
    // void onCPAddButtonClicked();
    // void onCPSearchButtonClicked();
    // void onCPUpdateButtonClicked();
    // void onCPDeleteButtonClicked();
    // void onCTAddButtonClicked();
    // void onCTSearchButtonClicked();
    // void onCTUpdateButtonClicked();
    // void onCTDeleteButtonClicked();

private:
    QString                                         message;
    Ui::MainWindow                                  *ui;
    DatabaseManager                                 *m_dbManager;
    QSqlRelationalTableModel                        *m_customerModel;
    QSqlRelationalTableModel                        *m_consumptionModel;
};
#endif // MAINWINDOW_H
