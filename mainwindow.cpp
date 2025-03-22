#include "mainwindow.h"
#include "ui_mainwindow.h"


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , m_dbManager(new DatabaseManager(this))
    , m_logManager(new LogManager(this))
{
    ui->setupUi(this);

    connects();
    initManagers();
}

void MainWindow::connects() {
    connect(ui->quitButton, &QPushButton::clicked, this, &MainWindow::onQuitButtonClicked);
}

void MainWindow::initManagers() {
    // m_dbManager->initializeDataBase();
    if (!m_logManager->initializeLogs()) {
        QMessageBox::critical(this, "Error", "初始化日志系统失败...");
        QCoreApplication::exit(1);
    }
    m_logManager->log(Log::Category::LOG_APPLICATION, Log::Level::INFO, "程序启动。");
}

void MainWindow::onQuitButtonClicked() {
    QApplication::quit();
}

MainWindow::~MainWindow()
{
    delete ui;
}
