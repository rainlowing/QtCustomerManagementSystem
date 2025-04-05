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

    connect(ui->cp_exportButton, &QPushButton::clicked, this, &MainWindow::onCPExportButtonClicked);

    connect(ui->ct_addButton, &QPushButton::clicked, this, &MainWindow::onCTAddButtonClicked);

    connect(ui->ct_searchButton, &QPushButton::clicked, this, &MainWindow::onCTSearchButtonClicked);

    connect(ui->ct_updateButton, &QPushButton::clicked, this, &MainWindow::onCTUpdateButtonClicked);

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


void MainWindow::closeAllCPForm() {
    if (m_addNewConsumption) m_addNewConsumption->close();
    if (m_updateConsumption) m_updateConsumption->close();
    if (m_searchConsumption) m_searchConsumption->close();
}

void MainWindow::closeAllCTForm() {
    if (m_addNewCustomer) m_addNewCustomer->close();
    if (m_searchCustomer) m_searchCustomer->close();
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

    closeAllCPForm();
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
        ui->statusBar->showMessage("修改消费记录前，请先选中任意一行记录或单元格", 5000);
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

    closeAllCPForm();
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
    case 101:
        ui->statusBar->showMessage("[修改消费记录失败] 数据库中没有对应姓名的顾客数据", 5000);
        break;
    case 200:
        ui->statusBar->showMessage("[修改消费记录失败] 数据库新增操作出现错误，详情信息请查看日志", 5000);
        break;
    default:
        break;
    }
}


void MainWindow::onCPSearchButtonClicked() {
    if (!m_searchConsumption) {
        m_searchConsumption = new SearchComsumption(m_dbManager, this);
        connect(m_searchConsumption, &SearchComsumption::returnCode, this, &MainWindow::handleCodeFromSearchConsumption);
        connect(m_searchConsumption, &SearchComsumption::selectConsumption, this, &MainWindow::selectConsumption);
    }

    closeAllCPForm();
    ui->consumptionViewTab->layout()->addWidget(m_searchConsumption);
    m_searchConsumption->setBaseInfo();
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

void MainWindow::onCPExportButtonClicked() {
    QString fileName = QFileDialog::getSaveFileName(this,
                                                    tr("保存文件"),
                                                    "",
                                                    tr("Excel 文件 (*.xlsx);;所有文件 (*.*)"));
    if (!fileName.isEmpty()) {
        exportToExcel(ui->consumptionTableView, fileName);
        ui->statusBar->showMessage("[导出为 Excel 文件成功] 保存路径为：" + fileName);
    } else {
        ui->statusBar->showMessage("[导出为 Excel 文件失败] 没有选择保存路径", 5000);
    }
}

void MainWindow::onCTAddButtonClicked() {
    if (!m_addNewCustomer) {
        m_addNewCustomer = new AddNewCustomer(m_dbManager, this);
        connect(m_addNewCustomer, &AddNewCustomer::returnCode, this, &MainWindow::handleCodeFromAddCustomer);
    }

    closeAllCTForm();
    ui->customerViewTab->layout()->addWidget(m_addNewCustomer);
    m_addNewCustomer->setBaseInfo();
}

void MainWindow::handleCodeFromAddCustomer(int code) {
    switch (code) {
    case 0:
        ui->statusBar->showMessage("[新增顾客信息成功] 已更新数据", 5000);
        break;
    case 100:
        ui->statusBar->showMessage("[新增顾客信息失败] 姓名不能为空", 5000);
        break;
    case 200:
        ui->statusBar->showMessage("[新增顾客信息失败] 数据库新增操作出现错误，请检查是否出现同名顾客，详情信息请查看日志", 5000);
    default:
        break;
    }
}

void MainWindow::onCTUpdateButtonClicked() {
    if (!m_updateCustomer) {
        m_updateCustomer = new UpdateCustomer(m_dbManager, this);
        connect(m_updateCustomer, &UpdateCustomer::returnCode, this, &MainWindow::handleCodeFromUpdateCustomer);
    }

    int row = ui->customersTableView->selectionModel()->currentIndex().row();
    if (row == -1) {
        ui->statusBar->showMessage("修改顾客信息前，请先选中任意一行记录或单元格", 5000);
        return;
    }

    QVariantMap data;
    data["customer_id"] = m_customerModel->record(row).value(1).toString();
    data["name"] = m_customerModel->record(row).value(2).toString();
    data["gender"] = m_customerModel->record(row).value(3).toString();
    data["birthday"] = m_customerModel->record(row).value(4).toString();
    data["phone"] = m_customerModel->record(row).value(5).toString();
    data["note"] = m_customerModel->record(row).value(9).toString();
    m_updateCustomer->setBaseInfo(data);

    closeAllCTForm();
    ui->customerViewTab->layout()->addWidget(m_updateCustomer);
}


void MainWindow::handleCodeFromUpdateCustomer(int code) {
    switch (code) {
    case 0:
        ui->statusBar->showMessage("[修改顾客信息成功] 已更新数据", 5000);
        break;
    case 1:
        ui->statusBar->showMessage("[修改顾客信息成功] 没有需要更新的数据", 5000);
        break;
    case 100:
        ui->statusBar->showMessage("[修改顾客信息失败] 顾客姓名项为空", 5000);
        break;
    case 101:
        ui->statusBar->showMessage("[修改顾客信息失败] 更新后的姓名在数据库中已存在", 5000);
        break;
    case 200:
        ui->statusBar->showMessage("[修改顾客信息失败] 数据库新增操作出现错误，详情信息请查看日志", 5000);
        break;
    default:
        break;
    }
}


void MainWindow::onCTSearchButtonClicked() {
    if (!m_searchCustomer) {
        m_searchCustomer = new SearchCustomer(m_dbManager, this);
        connect(m_searchCustomer, &SearchCustomer::returnCode, this, &MainWindow::handleCodeFromSearchCustomer);
        connect(m_searchCustomer, &SearchCustomer::selectCustomer, this, &MainWindow::selectCustomer);
    }

    closeAllCTForm();
    ui->customerViewTab->layout()->addWidget(m_searchCustomer);
    m_searchCustomer->setBaseInfo();
}

void MainWindow::handleCodeFromSearchCustomer(int code) {
    switch (code) {
    case 0:
        ui->statusBar->showMessage("[搜索顾客信息成功] 已得到数据", 5000);
        break;
    case 1:
        ui->statusBar->showMessage("[搜索顾客信息提示] 已清空筛选", 5000);
        break;
    case 100:
        ui->statusBar->showMessage("[搜索消顾客信息失败] 筛选条件为空或不符合正确格式", 5000);
        break;
    default:
        break;
    }
}

void MainWindow::onCTExportButtonClicked() {
    QString fileName = QFileDialog::getSaveFileName(this,
                                                    tr("保存文件"),
                                                    "",
                                                    tr("Excel 文件 (*.xlsx);;所有文件 (*.*)"));
    if (!fileName.isEmpty()) {
        exportToExcel(ui->customersTableView, fileName);
        ui->statusBar->showMessage("[导出为 Excel 文件成功] 保存路径为：" + fileName);
    } else {
        ui->statusBar->showMessage("[导出为 Excel 文件失败] 没有选择保存路径", 5000);
    }
}

void MainWindow::exportToExcel(QTableView *tableView, const QString &filePath) {
    QXlsx::Document xlsx;

    QXlsx::Format headerFormat;
    headerFormat.setFontBold(true);
    headerFormat.setFontSize(12);
    headerFormat.setBorderStyle(QXlsx::Format::BorderThin);

    // ==== 写入表头 ====
    QAbstractItemModel *model = tableView->model();
    for (int col = 0; col < model->columnCount(); ++col) {
        QString header = model->headerData(col, Qt::Horizontal).toString();
        xlsx.write(1, col + 1, header, headerFormat);
    }

    // ==== 写入数据 ====
    QXlsx::Format dataFormat;
    dataFormat.setBorderStyle(QXlsx::Format::BorderThin);

    for (int row = 0; row < model->rowCount(); ++row) {
        for (int col = 0; col < model->columnCount(); ++col) {
            QModelIndex index = model->index(row, col);
            QVariant data = model->data(index);

            // 处理特殊数据类型（如日期）
            if (data.type() == QVariant::DateTime) {
                xlsx.write(row + 2, col + 1, data.toDateTime(), dataFormat);
            } else {
                xlsx.write(row + 2, col + 1, data.toString(), dataFormat);
            }
        }
    }

    // ==== 自动调整列宽 ====
    for (int col = 0; col < model->columnCount(); ++col) {
        xlsx.setColumnWidth(col + 1, tableView->columnWidth(col) / 7); // 像素转Excel列宽单位
    }

    // ==== 保存文件 ====
    xlsx.saveAs(filePath);
}


void MainWindow::selectConsumption(const QString &condition) {
    if (!condition.isEmpty()) {
        m_consumptionModel->setFilter(condition);
    } else {
        m_consumptionModel->setFilter("");
    }

    refreshConsumptionTableView();
}

void MainWindow::selectCustomer(const QString &condition) {
    if (!condition.isEmpty()) {
        m_customerModel->setFilter(condition);
    } else {
        m_customerModel->setFilter("");
    }

    refreshCustomerTableView();
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
