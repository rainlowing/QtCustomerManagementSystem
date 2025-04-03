#include "mainwindow.h"
#include "ui_mainwindow.h"


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , m_dbManager(new DatabaseManager(this))
{
    ui->setupUi(this);

    connects();
    initManagers();
    initTimer();
    setTables();
}

void MainWindow::connects() {
    connect(ui->quitButton, &QPushButton::clicked, this, &MainWindow::onQuitButtonClicked);

    connect(ui->cp_addButton, &QPushButton::clicked, this, &MainWindow::onCPAddButtonClicked);

    connect(ui->cp_updateButton, &QPushButton::clicked, this, &MainWindow::onCPUpdateButtonClicked);

    connect(ui->cp_searchButton, &QPushButton::clicked, this, &MainWindow::onCPSearchButtonClicked);

    connect(ui->cp_deleteButton, &QPushButton::clicked, this, &MainWindow::onCPDeleteButtonClicked);
    // connect(ui->cp_deleteButton, &QPushButton::clicked, this, &MainWindow::onCPDeleteButtonClicked);
    // connect(ui->cp_searchButton, &QPushButton::clicked, this, &MainWindow::onCPSearchButtonClicked);
    // connect(ui->ct_addButton, &QPushButton::clicked, this, &MainWindow::onCTAddButtonClicked);
    // connect(ui->ct_updateButton, &QPushButton::clicked, this, &MainWindow::onCTUpdateButtonClicked);
    // connect(ui->ct_deleteButton, &QPushButton::clicked, this, &MainWindow::onCTDeleteButtonClicked);
    // connect(ui->ct_searchButton, &QPushButton::clicked, this, &MainWindow::onCTSearchButtonClicked)

    connect(m_dbManager, &DatabaseManager::consumptionDataChanged, this, &MainWindow::refreshConsumptionTableView);
    connect(m_dbManager, &DatabaseManager::customerDataChanged, this, &MainWindow::refreshCustomerTableView);
}

// 初始化管理器
void MainWindow::initManagers() {
    if (!m_dbManager->initDB()) {
        LogManager::getInstance().log(Log::APPLICATION, Log::CRITICAL, "数据库连接失败...");
        QMessageBox::warning(this, "程序错误", "数据库无法正确连接。", QMessageBox::Button::Ok);
        QCoreApplication::exit(1);
    }
    LogManager::getInstance().log(Log::APPLICATION, Log::INFO, "程序启动。");
}


void MainWindow::initTimer() {
    m_timeLabel = new QLabel(this);
    ui->statusBar->addPermanentWidget(m_timeLabel);

    m_timer = new QTimer(this);
    connect(m_timer, &QTimer::timeout, this, &MainWindow::updateTimer);
    m_timer->start(1000);

    updateTimer();

}


void MainWindow::setTables() {
    m_customerModel = new QSqlRelationalTableModel(this, m_dbManager->getDB());
    m_customerModel->setTable("customers");

    m_consumptionModel = new QSqlRelationalTableModel(this, m_dbManager->getDB());
    m_consumptionModel->setTable("consumptions");
    m_consumptionModel->setRelation(2, QSqlRelation("customers", "customer_id", "name"));

    refreshConsumptionTableView();
    refreshCustomerTableView();
}


void MainWindow::closeAllCPFrom() {
    if (m_addNewConsumption) m_addNewConsumption->close();
    if (m_updateConsumption) m_updateConsumption->close();
    if (m_SearchConsumption) m_SearchConsumption->close();
}


void MainWindow::onQuitButtonClicked() {
    QApplication::quit();
}

void MainWindow::updateTimer() {
    const QString time = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
    m_timeLabel->setText(time);
}


void MainWindow::onCPAddButtonClicked() {
    if (!m_addNewConsumption) {
        m_addNewConsumption = new AddNewConsumption(m_dbManager, this);
        connect(m_addNewConsumption, &AddNewConsumption::returnCode, this, &MainWindow::handleCodeFromAddConsumption);
    }

    closeAllCPFrom();
    ui->consumptionViewTab->layout()->addWidget(m_addNewConsumption);
    m_addNewConsumption->setBaseInfo();
}


void MainWindow::handleCodeFromAddConsumption(int code) {
    switch (code) {
    case 0:
        ui->statusBar->showMessage("[新增消费记录成功] 已更新数据", 5000);
        break;
    case 100:
        ui->statusBar->showMessage("[新增消费记录失败] 顾客姓名项为空，或金额项小于零", 5000);
        break;
    case 200:
        ui->statusBar->showMessage("[新增消费记录失败] 数据库新增操作出现错误，详情信息请查看日志", 5000);
        break;
    default:
        break;
    }
}


void MainWindow::onCPUpdateButtonClicked() {
    if (!m_updateConsumption) {
        m_updateConsumption = new UpdateConsumption(m_dbManager, this);
        connect(m_updateConsumption, &UpdateConsumption::returnCode, this, &MainWindow::handleCodeFromUpdateConsumption);
    }

    int row = ui->consumptionTableView->selectionModel()->currentIndex().row();
    if (row == -1) {
        ui->statusBar->showMessage("修改消费信息前，请先选中任意一行记录或单元格", 5000);
        return;
    }

    QVariantMap data;
    data["consumption_id"] = m_consumptionModel->record(row).value(1).toString();
    data["name"] = m_consumptionModel->record(row).value(2).toString();
    data["service"] = m_consumptionModel->record(row).value(3).toString();
    data["amount"] = m_consumptionModel->record(row).value(4).toString();
    data["time"] = m_consumptionModel->record(row).value(6).toString();
    data["note"] = m_consumptionModel->record(row).value(7).toString();
    m_updateConsumption->setBaseInfo(data);

    closeAllCPFrom();
    ui->consumptionViewTab->layout()->addWidget(m_updateConsumption);
}

void MainWindow::handleCodeFromUpdateConsumption(int code) {
    switch (code) {
    case 0:
        ui->statusBar->showMessage("[修改消费记录成功] 已更新数据", 5000);
        break;
    case 1:
        ui->statusBar->showMessage("[修改消费记录成功] 没有需要更新的数据", 5000);
        break;
    case 100:
        ui->statusBar->showMessage("[修改消费记录失败] 顾客姓名项为空，或金额项小于零", 5000);
        break;
    case 200:
        ui->statusBar->showMessage("[修改消费记录失败] 数据库新增操作出现错误，详情信息请查看日志", 5000);
        break;
    default:
        break;
    }
}


void MainWindow::onCPSearchButtonClicked() {
    if (!m_SearchConsumption) {
        m_SearchConsumption = new SearchComsumption(m_dbManager, this);
        connect(m_SearchConsumption, &SearchComsumption::returnCode, this, &MainWindow::handleCodeFromSearchConsumption);
        connect(m_SearchConsumption, &SearchComsumption::selectConsumption, this, &MainWindow::selectConsumption);
    }

    closeAllCPFrom();
    ui->consumptionViewTab->layout()->addWidget(m_SearchConsumption);
    m_SearchConsumption->setBaseInfo();
}


void MainWindow::handleCodeFromSearchConsumption(int code) {
    switch (code) {
    case 0:
        ui->statusBar->showMessage("[搜索消费记录成功] 已得到数据", 5000);
        break;
    case 1:
        ui->statusBar->showMessage("[搜索消费记录提示] 已清空筛选", 5000);
        break;
    case 100:
        ui->statusBar->showMessage("[搜索消费记录失败] 筛选条件为空或不符合正确格式", 5000);
        break;
    default:
        break;
    }
}

void MainWindow::onCPDeleteButtonClicked() {
    int row = ui->consumptionTableView->selectionModel()->currentIndex().row();
    if (row == -1) {
        ui->statusBar->showMessage("删除消费信息前，请先选中任意一行记录或单元格", 5000);
        return;
    }

    if (QMessageBox::question(this, "确认删除", "确定要删除选中的行吗？此操作不可撤回。", QMessageBox::Yes | QMessageBox::No) != QMessageBox::Yes) {
        return;
    }

    if (m_consumptionModel->removeRow(row)) {
        if (m_consumptionModel->submitAll()) {
            refreshConsumptionTableView();
            refreshCustomerTableView();
            ui->statusBar->showMessage("[删除消费记录成功] 已删除消费记录并刷新");
        } else {
            ui->statusBar->showMessage("[删除消费记录失败] 提交删除失败");
            message = "提交删除失败（Failed to delete consumption record: " + m_consumptionModel->lastError().text() + "）";
            LogManager::getInstance().log(Log::DATABASE, Log::WARNING, message);
            m_consumptionModel->revertAll();
        }
        return;
    }

    ui->statusBar->showMessage("[删除消费记录失败] 无法删除改行记录");
}


void MainWindow::selectConsumption(const QString &condition) {
    if (!condition.isEmpty()) {
        qDebug() << condition;
        m_consumptionModel->setFilter(condition);
    } else {
        m_consumptionModel->setFilter("");
    }

    refreshConsumptionTableView();
}


void MainWindow::refreshConsumptionTableView() {
    if (m_consumptionModel->select()) {
        ui->consumptionTableView->setModel(m_consumptionModel);
        ui->consumptionTableView->hideColumn(0);
        ui->consumptionTableView->hideColumn(5);

        m_consumptionModel->setHeaderData(1, Qt::Horizontal, "消费记录ID");
        m_consumptionModel->setHeaderData(2, Qt::Horizontal, "顾客姓名");
        m_consumptionModel->setHeaderData(3, Qt::Horizontal, "服务");
        m_consumptionModel->setHeaderData(4, Qt::Horizontal, "金额（元）");
        m_consumptionModel->setHeaderData(6, Qt::Horizontal, "记录时间");
        m_consumptionModel->setHeaderData(7, Qt::Horizontal, "备注");

        ui->consumptionTableView->setSortingEnabled(true);
        ui->consumptionTableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    } else {
        message = "消费表无法正常显示（Table consumptons cannot display correctly: " + m_consumptionModel->lastError().text() + "）";
        LogManager::getInstance().log(Log::APPLICATION, Log::ERROR, message);
    }
}


void MainWindow::refreshCustomerTableView() {
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
        m_customerModel->setHeaderData(9, Qt::Horizontal, "备注");

        ui->customersTableView->setSortingEnabled(true);
        ui->customersTableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    } else {
        message = "顾客信息表无法正常显示（Table cuntomers cannot display correctly: " + m_customerModel->lastError().text() + "）";
        LogManager::getInstance().log(Log::APPLICATION, Log::ERROR, message);
    }

}


MainWindow::~MainWindow()
{
    delete ui;
}
