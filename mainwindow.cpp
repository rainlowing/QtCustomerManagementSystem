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
    setTables();
}

void MainWindow::connects() {
    connect(ui->quitButton, &QPushButton::clicked, this, &MainWindow::onQuitButtonClicked);
}

void MainWindow::initManagers() {
    if (!m_logManager->initializeLogs()) {
        m_logManager->log(Log::LOG_APPLICATION, Log::CRITICAL, "初始化日志系统失败...");
        QCoreApplication::exit(1);
    }
    m_logManager->log(Log::LOG_APPLICATION, Log::INFO, "日志系统初始化成功。");

    if (!m_dbManager->initializeDataBase(*m_logManager)) {
        m_logManager->log(Log::LOG_APPLICATION, Log::CRITICAL, "数据库连接失败...");
        QCoreApplication::exit(1);
    }

    m_logManager->log(Log::LOG_APPLICATION, Log::INFO, "程序启动。");
}

void MainWindow::setTables() {
    m_customerModel = new QSqlRelationalTableModel(this, m_dbManager->getDB());
    m_customerModel->setTable("customers");
    if (m_customerModel->select()) {
        ui->customersTableView->setModel(m_customerModel);
        ui->customersTableView->hideColumn(0);

        m_customerModel->setHeaderData(1, Qt::Horizontal, "顾客ID");
        m_customerModel->setHeaderData(2, Qt::Horizontal, "姓名");
        m_customerModel->setHeaderData(3, Qt::Horizontal, "性别");
        m_customerModel->setHeaderData(4, Qt::Horizontal, "生日");
        m_customerModel->setHeaderData(5, Qt::Horizontal, "电话号码");
        m_customerModel->setHeaderData(6, Qt::Horizontal, "首次消费记录");
        m_customerModel->setHeaderData(7, Qt::Horizontal, "最新消费记录");
        m_customerModel->setHeaderData(8, Qt::Horizontal, "总消费额（元）");

        // 设置表格属性：允许排序、自动调整列宽
        ui->customersTableView->setSortingEnabled(true);
        ui->customersTableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    } else {
        message = "顾客信息表无法正常显示（Table cuntomers cannot display correctly: " + m_customerModel->lastError().text() + "）";
        m_logManager->log(Log::LOG_APPLICATION, Log::ERROR, message);
    }

    m_consumptionModel = new QSqlRelationalTableModel(this, m_dbManager->getDB());
    m_consumptionModel->setTable("consumption_records");
    m_consumptionModel->setRelation(1, QSqlRelation("customers", "id", "name"));
    if (m_consumptionModel->select()) {
        ui->consumptionTableView->setModel(m_consumptionModel);

        m_consumptionModel->setHeaderData(0, Qt::Horizontal, "消费记录ID");
        m_consumptionModel->setHeaderData(1, Qt::Horizontal, "姓名");
        m_consumptionModel->setHeaderData(2, Qt::Horizontal, "服务");
        m_consumptionModel->setHeaderData(3, Qt::Horizontal, "金额（元）");
        m_consumptionModel->setHeaderData(4, Qt::Horizontal, "记录时间");

        ui->consumptionTableView->setSortingEnabled(true);
        ui->consumptionTableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    } else {
        message = "消费表无法正常显示（Table consumpton_records cannot display correctly: " + m_consumptionModel->lastError().text() + "）";
        m_logManager->log(Log::LOG_APPLICATION, Log::ERROR, message);
    }
}

void MainWindow::onQuitButtonClicked() {
    QApplication::quit();
}

MainWindow::~MainWindow()
{
    delete ui;
}
