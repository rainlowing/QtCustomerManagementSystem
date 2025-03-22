#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPushButton>
#include <QMessageBox>

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

private slots:
    void onQuitButtonClicked();

private:
    Ui::MainWindow *ui;
    DatabaseManager *m_dbManager;
    LogManager *m_logManager;
};
#endif // MAINWINDOW_H
