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
    setTables();
}

void MainWindow::connects() {
    connect(ui->quitButton, &QPushButton::clicked, this, &MainWindow::onQuitButtonClicked);
    connect(ui->cp_addButton, &QPushButton::clicked, this, &MainWindow::onCPAddButtonClicked);
    // connect(ui->cp_updateButton, &QPushButton::clicked, this, &MainWindow::onCPUpdateButtonClicked);
    // connect(ui->cp_deleteButton, &QPushButton::clicked, this, &MainWindow::onCPDeleteButtonClicked);
    // connect(ui->cp_searchButton, &QPushButton::clicked, this, &MainWindow::onCPSearchButtonClicked);
    // connect(ui->ct_addButton, &QPushButton::clicked, this, &MainWindow::onCTAddButtonClicked);
    // connect(ui->ct_updateButton, &QPushButton::clicked, this, &MainWindow::onCTUpdateButtonClicked);
    // connect(ui->ct_deleteButton, &QPushButton::clicked, this, &MainWindow::onCTDeleteButtonClicked);
    // connect(ui->ct_searchButton, &QPushButton::clicked, this, &MainWindow::onCTSearchButtonClicked);
    connect(m_dbManager, &DatabaseManager::dataChanged, this, &MainWindow::refreshTableView);
}

// 初始化管理器
void MainWindow::initManagers() {
    if (!m_dbManager->initDB()) {
        LogManager::getInstance().log(Log::APPLICATION, Log::CRITICAL, "数据库连接失败...");
        QCoreApplication::exit(1);
    }
    LogManager::getInstance().log(Log::APPLICATION, Log::INFO, "程序启动。");
}

void MainWindow::setTables() {
    m_customerModel = new QSqlRelationalTableModel(this, m_dbManager->getDB());
    m_customerModel->setTable("customers");

    m_consumptionModel = new QSqlRelationalTableModel(this, m_dbManager->getDB());
    m_consumptionModel->setTable("consumptions");
    m_consumptionModel->setRelation(2, QSqlRelation("customers", "customer_id", "name"));

    refreshTableView();
}


void MainWindow::onQuitButtonClicked() {
    QApplication::quit();
}

// QGroupBox *MainWindow::createCPUpdateFormGroup(int currentRow) {
//     QGroupBox *fromGroup = new QGroupBox("记录信息");
//     QFormLayout *formLayout = new QFormLayout(fromGroup);

//     QString id = m_consumptionModel->record(currentRow).value(0).toString();
//     QString name = m_consumptionModel->record(currentRow).value(1).toString();
//     QString service = m_consumptionModel->record(currentRow).value(2).toString();
//     QString amount = m_consumptionModel->record(currentRow).value(3).toString();
//     QString time = m_consumptionModel->record(currentRow).value(4).toString();

//     QLineEdit *idEdit = new QLineEdit();
//     idEdit->setObjectName("idEdit");
//     idEdit->setReadOnly(true);
//     QComboBox *nameCombo = new QComboBox();
//     nameCombo->setObjectName("nameCombo");
//     nameCombo->setEditable(true);
//     QComboBox *serviceCombo = new QComboBox();
//     serviceCombo->setObjectName("serviceCombo");
//     serviceCombo->setEditable(true);
//     QLineEdit *amountEdit = new QLineEdit();
//     amountEdit->setObjectName("amountEdit");
//     QLineEdit *timeEdit = new QLineEdit();
//     timeEdit->setObjectName("timeEdit");
//     timeEdit->setReadOnly(true);

//     m_dbManager->selectAllName(nameCombo);
//     m_dbManager->selectAllService(serviceCombo);

//     idEdit->setText(id);
//     nameCombo->setCurrentText(name);
//     serviceCombo->setCurrentText(service);
//     amountEdit->setText(amount);
//     timeEdit->setText(time);

//     formLayout->addRow("消费记录ID", idEdit);
//     formLayout->addRow("姓名", nameCombo);
//     formLayout->addRow("服务", serviceCombo);
//     formLayout->addRow("金额（元）", amountEdit);
//     formLayout->addRow("记录时间", timeEdit);

//     return fromGroup;

// }

// QString MainWindow::autoGenerateCRID() {
//     QString currentDate = QDateTime::currentDateTime().toString("yyyyMMdd");
//     QString prefix = "CR-" + currentDate + "-";
//     int nextNumber = 1;

//     QSqlQuery query;
//     QString sql = QString("SELECT MAX(SUBSTR(consumption_id, LENGTH(consumption_id) - 3, 4)) FROM consumption_records WHERE consumption_id LIKE '%1%'").arg(prefix);
//     if (query.exec(sql)) {
//         if (query.next()) {
//             QString maxStr = query.value(0).toString();
//             if (!maxStr.isEmpty()) {
//                 nextNumber = maxStr.toInt() + 1;
//             }
//         }
//     } else {
//         message = "生成新消费记录 ID 失败（Failed to auto ganerate a new consumption record ID: "
//                   + query.lastError().text() + "）";
//         LogManager::getInstance().log(Log::DATABASE, Log::CRITICAL, message);
//     }

//     QString numStr = QString("%1").arg(nextNumber, 4, 10, QLatin1Char('0'));
//     return prefix + numStr;
// }


// bool MainWindow::handleCPUpdateDialogAccepted(QGroupBox *fromGroup) {
//     QComboBox *nameCombo = fromGroup->findChild<QComboBox*>("nameCombo");
//     QLineEdit *amountEdit = fromGroup->findChild<QLineEdit*>("amountEdit");

//     if (nameCombo->currentText().isEmpty() || amountEdit->text().isEmpty()) {
//         QMessageBox::warning(this, "错误", "姓名或金额不能为空");
//         return false;
//     }

//     if (m_dbManager->updateCP(fromGroup)) {
//         m_consumptionModel->select();
//         m_customerModel->select();
//     }
//     return true;
// }


void MainWindow::onCPAddButtonClicked() {
    if (!m_addNewConsumption) {
        m_addNewConsumption = new AddNewConsumption(m_dbManager, this);
    }

    m_addNewConsumption->show();
}

void MainWindow::refreshTableView() {
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

        // 设置表格属性：允许排序、自动调整列宽
        ui->customersTableView->setSortingEnabled(true);
        ui->customersTableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    } else {
        message = "顾客信息表无法正常显示（Table cuntomers cannot display correctly: " + m_customerModel->lastError().text() + "）";
        LogManager::getInstance().log(Log::APPLICATION, Log::ERROR, message);
    }

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

// void MainWindow::onCPSearchButtonClicked()
// {

// }

// void MainWindow::onCPUpdateButtonClicked() {
//     int currentRow = ui->consumptionTableView->selectionModel()->currentIndex().row();
//     if (currentRow < 0)
//         return;

//     QDialog dlg(this);
//     dlg.setWindowTitle("修改记录");
//     dlg.setMinimumSize(300, 200);
//     QVBoxLayout *mainLayout = new QVBoxLayout(&dlg);
//     QGroupBox *formGroup = createCPUpdateFormGroup(currentRow);
//     mainLayout->addWidget(formGroup, 1);
//     QHBoxLayout *btnLayout = new QHBoxLayout();
//     QPushButton *btnConfirm = new QPushButton("确认");
//     QPushButton *btnCancel = new QPushButton("取消");
//     btnConfirm->setFixedWidth(80);
//     btnCancel->setFixedWidth(80);
//     btnLayout->addStretch();
//     btnLayout->addWidget(btnConfirm);
//     btnLayout->addWidget(btnCancel);
//     connect(btnConfirm, &QPushButton::clicked, &dlg, &QDialog::accept);
//     connect(btnCancel, &QPushButton::clicked, &dlg, &QDialog::reject);
//     mainLayout->addLayout(btnLayout);
//     if (dlg.exec() == QDialog::Accepted) {
//         handleCPUpdateDialogAccepted(formGroup);
//     }

// }

// void MainWindow::onCPDeleteButtonClicked()
// {

// }

// void MainWindow::onCTAddButtonClicked()
// {

// }

// void MainWindow::onCTSearchButtonClicked()
// {

// }

// void MainWindow::onCTUpdateButtonClicked()
// {

// }

// void MainWindow::onCTDeleteButtonClicked()
// {

// }


MainWindow::~MainWindow()
{
    delete ui;
}
