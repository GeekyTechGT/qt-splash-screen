#include "MainWindow.h"
#include <QDebug>
#include <QDateTime>
#include <QApplication>
#include <QThread>
#include <QRandomGenerator>
#include <QtConcurrent>
#include <QTimer>
#include <QHeaderView>

// ============================================================================
// MainWindow Implementation
// ============================================================================

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , m_centralWidget(nullptr)
    , m_mainLayout(nullptr)
    , m_titleLabel(nullptr)
    , m_logTextEdit(nullptr)
    , m_dataTable(nullptr)
    , m_statusLabel(nullptr)
    , m_currentTaskIndex(0)
    , m_cancelled(false)
    , m_workerThread(nullptr)
{
    setupInitializationTasks();
    setupUi();
}

MainWindow::~MainWindow()
{
    m_cancelled = true;
    if (m_workerThread) {
        m_workerThread->quit();
        m_workerThread->wait(3000);
        delete m_workerThread;
    }
}

void MainWindow::setupUi()
{
    setWindowTitle("Qt Splash Screen Demo - Main Application");
    setMinimumSize(800, 600);

    m_centralWidget = new QWidget(this);
    setCentralWidget(m_centralWidget);

    m_mainLayout = new QVBoxLayout(m_centralWidget);
    m_mainLayout->setContentsMargins(20, 20, 20, 20);
    m_mainLayout->setSpacing(15);

    // Title
    m_titleLabel = new QLabel("Application Initialized Successfully!", this);
    m_titleLabel->setStyleSheet("font-size: 24px; font-weight: bold; color: #2c3e50;");
    m_titleLabel->setAlignment(Qt::AlignCenter);
    m_mainLayout->addWidget(m_titleLabel);

    // Log area
    QLabel *logLabel = new QLabel("Initialization Log:", this);
    logLabel->setStyleSheet("font-size: 14px; font-weight: bold; color: #34495e;");
    m_mainLayout->addWidget(logLabel);

    m_logTextEdit = new QTextEdit(this);
    m_logTextEdit->setReadOnly(true);
    m_logTextEdit->setMaximumHeight(150);
    m_logTextEdit->setStyleSheet(
        "QTextEdit {"
        "  background-color: #1e1e1e;"
        "  color: #00ff00;"
        "  font-family: 'Consolas', 'Courier New', monospace;"
        "  font-size: 12px;"
        "  border: 1px solid #3498db;"
        "  border-radius: 5px;"
        "  padding: 10px;"
        "}"
    );
    m_mainLayout->addWidget(m_logTextEdit);

    // Data table (populated during initialization with 10k rows)
    QLabel *tableLabel = new QLabel("Data Table (10,000 rows loaded during init):", this);
    tableLabel->setStyleSheet("font-size: 14px; font-weight: bold; color: #34495e;");
    m_mainLayout->addWidget(tableLabel);

    m_dataTable = new QTableWidget(this);
    m_dataTable->setColumnCount(5);
    m_dataTable->setHorizontalHeaderLabels({"ID", "Name", "Value", "Status", "Timestamp"});
    m_dataTable->horizontalHeader()->setStretchLastSection(true);
    m_dataTable->setAlternatingRowColors(true);
    m_dataTable->setStyleSheet(
        "QTableWidget {"
        "  background-color: #ffffff;"
        "  alternate-background-color: #f5f5f5;"
        "  border: 1px solid #3498db;"
        "  border-radius: 5px;"
        "  gridline-color: #ddd;"
        "}"
        "QHeaderView::section {"
        "  background-color: #3498db;"
        "  color: white;"
        "  padding: 8px;"
        "  font-weight: bold;"
        "  border: none;"
        "}"
    );
    m_mainLayout->addWidget(m_dataTable, 1);  // stretch factor 1

    // Status bar
    m_statusLabel = new QLabel("Ready", this);
    m_statusLabel->setStyleSheet(
        "font-size: 12px; color: #27ae60; padding: 5px; "
        "background-color: #ecf0f1; border-radius: 3px;"
    );
    m_mainLayout->addWidget(m_statusLabel);
}

void MainWindow::setupInitializationTasks()
{
    // Define initialization tasks with their methods
    // isHeavyTask = true means the task will run on a worker thread
    m_initTasks = {
        {"config",      "Loading configuration files",
            [this]() { taskLoadConfiguration(); }, false},

        {"database",    "Initializing database connection",
            [this]() { taskInitializeDatabase(); }, true},

        {"preferences", "Loading user preferences",
            [this]() { taskLoadUserPreferences(); }, false},

        {"ui",          "Preparing UI components",
            [this]() { taskPrepareUIComponents(); }, true},

        {"plugins",     "Loading plugins",
            [this]() { taskLoadPlugins(); }, true},

        {"license",     "Verifying license",
            [this]() { taskVerifyLicense(); }, false},

        {"services",    "Connecting to services",
            [this]() { taskConnectToServices(); }, true},

        {"datatable",   "Populating data table (10,000 rows)",
            [this]() { taskPopulateDataTable(); }, true}
    };
}

void MainWindow::initialize()
{
    m_currentTaskIndex = 0;
    m_cancelled = false;

    logMessage("Starting initialization...");

    // Start the first task
    runNextTask();
}

void MainWindow::runNextTask()
{
    if (m_cancelled) {
        return;
    }

    if (m_currentTaskIndex >= m_initTasks.size()) {
        // All tasks completed
        logMessage("========================================");
        logMessage("All initialization tasks completed!");
        logMessage("========================================");

        m_statusLabel->setText("All systems operational");
        m_statusLabel->setStyleSheet(
            "font-size: 12px; color: #ffffff; padding: 5px; "
            "background-color: #27ae60; border-radius: 3px; font-weight: bold;"
        );

        emit initializationComplete();
        return;
    }

    const InitTask &task = m_initTasks[m_currentTaskIndex];

    // Emit signal for splash screen (1-based step)
    emit initializationStepStarted(m_currentTaskIndex + 1, task.description);
    logTaskStart(task.description);

    if (task.isHeavyTask) {
        // Run heavy task in a separate thread using QtConcurrent
        QFuture<void> future = QtConcurrent::run([this, task]() {
            task.taskMethod();

            // When done, notify main thread using QMetaObject::invokeMethod
            QMetaObject::invokeMethod(this, "onTaskCompleted", Qt::QueuedConnection);
        });
    } else {
        // Run quick task on main thread but use a timer to not block UI
        QTimer::singleShot(0, this, [this, task]() {
            task.taskMethod();
            onTaskCompleted();
        });
    }
}

void MainWindow::onTaskCompleted()
{
    if (m_cancelled) {
        return;
    }

    // Log completion on the main thread
    QMetaObject::invokeMethod(this, [this]() {
        logTaskComplete();
        emit initializationStepCompleted(m_currentTaskIndex + 1);

        // Move to next task
        m_currentTaskIndex++;
        runNextTask();
    }, Qt::QueuedConnection);
}

QStringList MainWindow::getInitializationTasks() const
{
    QStringList tasks;
    for (const auto &task : m_initTasks) {
        tasks.append(task.description);
    }
    return tasks;
}

int MainWindow::getInitializationStepCount() const
{
    return m_initTasks.size();
}

void MainWindow::logMessage(const QString &message)
{
    // Ensure we're on the main thread for UI updates
    if (QThread::currentThread() != QApplication::instance()->thread()) {
        QMetaObject::invokeMethod(this, [this, message]() {
            logMessage(message);
        }, Qt::QueuedConnection);
        return;
    }

    QString timestamp = QDateTime::currentDateTime().toString("hh:mm:ss.zzz");
    m_logTextEdit->append(QString("[%1] %2").arg(timestamp, message));
}

void MainWindow::logTaskStart(const QString &taskName)
{
    if (QThread::currentThread() != QApplication::instance()->thread()) {
        QMetaObject::invokeMethod(this, [this, taskName]() {
            logTaskStart(taskName);
        }, Qt::QueuedConnection);
        return;
    }

    QString timestamp = QDateTime::currentDateTime().toString("hh:mm:ss.zzz");
    m_logTextEdit->append(QString("[%1] >> %2...").arg(timestamp, taskName));
}

void MainWindow::logTaskComplete()
{
    if (QThread::currentThread() != QApplication::instance()->thread()) {
        QMetaObject::invokeMethod(this, [this]() {
            logTaskComplete();
        }, Qt::QueuedConnection);
        return;
    }

    QString timestamp = QDateTime::currentDateTime().toString("hh:mm:ss.zzz");
    m_logTextEdit->append(QString("[%1]    [OK]").arg(timestamp));
}

// ============================================================================
// INITIALIZATION TASK IMPLEMENTATIONS
// ============================================================================

void MainWindow::taskLoadConfiguration()
{
    // Simulates loading configuration files (quick task)
    // In a real app: read JSON/XML config files, parse settings, etc.

    int duration = 300 + QRandomGenerator::global()->bounded(200);
    QThread::msleep(duration);

    // Example: simulate reading some config values
    // QSettings settings("app.ini", QSettings::IniFormat);
    // m_config = settings.value("key").toString();
}

void MainWindow::taskInitializeDatabase()
{
    // Simulates database connection (heavy task - runs in worker thread)
    // In a real app: open SQLite/PostgreSQL connection, run migrations, etc.

    int duration = 800 + QRandomGenerator::global()->bounded(400);

    // Simulate work in chunks to allow cancellation
    const int chunkSize = 50;
    int elapsed = 0;
    while (elapsed < duration && !m_cancelled) {
        QThread::msleep(chunkSize);
        elapsed += chunkSize;
    }

    // Example:
    // QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    // db.setDatabaseName("app.db");
    // db.open();
}

void MainWindow::taskLoadUserPreferences()
{
    // Simulates loading user preferences (quick task)
    // In a real app: read user settings, theme preferences, etc.

    int duration = 200 + QRandomGenerator::global()->bounded(150);
    QThread::msleep(duration);

    // Example:
    // QSettings settings;
    // m_theme = settings.value("theme", "light").toString();
}

void MainWindow::taskPrepareUIComponents()
{
    // Simulates preparing UI components with heavy computation (heavy task)
    // In a real app: load large resources, prepare complex layouts,
    // compute initial data for charts, etc.

    int duration = 600 + QRandomGenerator::global()->bounded(300);

    const int chunkSize = 50;
    int elapsed = 0;
    while (elapsed < duration && !m_cancelled) {
        QThread::msleep(chunkSize);
        elapsed += chunkSize;

        // Example: compute some data that will be displayed
        // for (int i = 0; i < 1000; i++) {
        //     m_chartData.append(computeComplexValue(i));
        // }
    }

    // If we need to update UI from here, use QMetaObject::invokeMethod:
    // QMetaObject::invokeMethod(this, [this]() {
    //     m_someWidget->setData(m_computedData);
    // }, Qt::QueuedConnection);
}

void MainWindow::taskLoadPlugins()
{
    // Simulates loading plugins (heavy task)
    // In a real app: scan plugin directories, load DLLs, initialize plugins

    int duration = 1000 + QRandomGenerator::global()->bounded(500);

    const int chunkSize = 50;
    int elapsed = 0;
    while (elapsed < duration && !m_cancelled) {
        QThread::msleep(chunkSize);
        elapsed += chunkSize;
    }

    // Example:
    // QDir pluginsDir(QApplication::applicationDirPath() + "/plugins");
    // for (const QString &fileName : pluginsDir.entryList(QDir::Files)) {
    //     QPluginLoader loader(pluginsDir.absoluteFilePath(fileName));
    //     loader.load();
    // }
}

void MainWindow::taskVerifyLicense()
{
    // Simulates license verification (quick task)
    // In a real app: check license file, validate with server, etc.

    int duration = 150 + QRandomGenerator::global()->bounded(100);
    QThread::msleep(duration);

    // Example:
    // LicenseManager::instance()->verify();
}

void MainWindow::taskConnectToServices()
{
    // Simulates connecting to external services (heavy task)
    // In a real app: connect to REST APIs, WebSocket servers, etc.

    int duration = 500 + QRandomGenerator::global()->bounded(200);

    const int chunkSize = 50;
    int elapsed = 0;
    while (elapsed < duration && !m_cancelled) {
        QThread::msleep(chunkSize);
        elapsed += chunkSize;
    }

    // Example:
    // m_apiClient->connect("https://api.example.com");
    // m_webSocket->open(QUrl("wss://realtime.example.com"));
}

void MainWindow::taskPopulateDataTable()
{
    // Heavy task that populates a table with 10,000 rows
    // This runs in a worker thread and updates the UI in batches
    // using QMetaObject::invokeMethod to keep the UI responsive.

    const int TOTAL_ROWS = 10000;
    const int BATCH_SIZE = 500;  // Insert rows in batches of 500

    QStringList statuses = {"Active", "Pending", "Completed", "Failed", "Processing"};

    for (int i = 0; i < TOTAL_ROWS && !m_cancelled; i += BATCH_SIZE) {
        // Generate a batch of rows in the worker thread
        QVector<QStringList> batch;
        batch.reserve(BATCH_SIZE);

        int batchEnd = qMin(i + BATCH_SIZE, TOTAL_ROWS);
        for (int row = i; row < batchEnd && !m_cancelled; ++row) {
            // Simulate some CPU-intensive data generation
            double value = qSin(row * 0.1) * 100 + QRandomGenerator::global()->bounded(50);
            QString status = statuses[QRandomGenerator::global()->bounded(statuses.size())];
            QString timestamp = QDateTime::currentDateTime()
                .addSecs(-QRandomGenerator::global()->bounded(86400))
                .toString("yyyy-MM-dd hh:mm:ss");

            QStringList rowData;
            rowData << QString::number(row + 1)
                    << QString("Item_%1").arg(row + 1, 5, 10, QChar('0'))
                    << QString::number(value, 'f', 2)
                    << status
                    << timestamp;

            batch.append(rowData);
        }

        // Send the batch to the main thread for UI update
        // Using QMetaObject::invokeMethod with Qt::BlockingQueuedConnection
        // ensures we wait for the UI update before generating the next batch,
        // preventing memory buildup and keeping UI responsive
        QMetaObject::invokeMethod(this, [this, batch]() {
            addTableRowsBatch(batch);
        }, Qt::BlockingQueuedConnection);

        // Small delay between batches to keep UI extra smooth
        QThread::msleep(10);
    }
}

void MainWindow::addTableRowsBatch(const QVector<QStringList> &rows)
{
    // This method runs on the main thread and updates the table
    // It's called from taskPopulateDataTable via QMetaObject::invokeMethod

    // Temporarily disable updates for faster insertion
    m_dataTable->setUpdatesEnabled(false);

    int startRow = m_dataTable->rowCount();
    m_dataTable->setRowCount(startRow + rows.size());

    for (int i = 0; i < rows.size(); ++i) {
        const QStringList &rowData = rows[i];
        int row = startRow + i;

        for (int col = 0; col < rowData.size() && col < m_dataTable->columnCount(); ++col) {
            QTableWidgetItem *item = new QTableWidgetItem(rowData[col]);

            // Right-align numeric columns
            if (col == 0 || col == 2) {
                item->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
            } else {
                item->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);
            }

            // Color-code status column
            if (col == 3) {
                if (rowData[col] == "Active") {
                    item->setForeground(QColor("#27ae60"));
                } else if (rowData[col] == "Failed") {
                    item->setForeground(QColor("#e74c3c"));
                } else if (rowData[col] == "Pending") {
                    item->setForeground(QColor("#f39c12"));
                } else if (rowData[col] == "Processing") {
                    item->setForeground(QColor("#3498db"));
                }
            }

            m_dataTable->setItem(row, col, item);
        }
    }

    // Re-enable updates and refresh
    m_dataTable->setUpdatesEnabled(true);

    // Process events to keep UI responsive
    QApplication::processEvents();
}
