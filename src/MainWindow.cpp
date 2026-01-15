#include "MainWindow.h"
#include <QDebug>
#include <QDateTime>
#include <QApplication>
#include <QThread>
#include <QRandomGenerator>

// ============================================================================
// InitializationWorker Implementation
// ============================================================================

InitializationWorker::InitializationWorker(QObject *parent)
    : QObject(parent)
    , m_cancelled(false)
{
}

void InitializationWorker::setTaskDurations(const QVector<int> &durations)
{
    m_taskDurations = durations;
}

void InitializationWorker::runInitialization()
{
    m_cancelled = false;

    // Task names that will be displayed in the splash screen
    QStringList taskNames = {
        "Loading configuration files",
        "Initializing database connection",
        "Loading user preferences",
        "Preparing UI components",
        "Loading plugins",
        "Verifying license",
        "Connecting to services"
    };

    for (int i = 0; i < m_taskDurations.size() && !m_cancelled; ++i) {
        QString taskName = (i < taskNames.size()) ? taskNames[i] : QString("Task %1").arg(i + 1);
        simulateTask(i, taskName, m_taskDurations[i]);
    }

    if (!m_cancelled) {
        emit allTasksCompleted();
    }
}

void InitializationWorker::cancelInitialization()
{
    m_cancelled = true;
}

void InitializationWorker::simulateTask(int taskIndex, const QString &taskName, int durationMs)
{
    emit taskStarted(taskIndex, taskName);

    // Simulate intensive work with random variations
    int actualDuration = durationMs + QRandomGenerator::global()->bounded(-100, 100);
    actualDuration = qMax(100, actualDuration);

    // Simulate work in chunks to allow cancellation
    const int chunkSize = 50;
    int elapsed = 0;
    while (elapsed < actualDuration && !m_cancelled) {
        QThread::msleep(chunkSize);
        elapsed += chunkSize;
    }

    if (!m_cancelled) {
        emit taskCompleted(taskIndex);
    }
}


// ============================================================================
// MainWindow Implementation
// ============================================================================

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , m_centralWidget(nullptr)
    , m_mainLayout(nullptr)
    , m_titleLabel(nullptr)
    , m_logTextEdit(nullptr)
    , m_statusLabel(nullptr)
    , m_workerThread(nullptr)
    , m_worker(nullptr)
{
    setupInitializationTasks();
    setupUi();
}

MainWindow::~MainWindow()
{
    if (m_workerThread) {
        if (m_worker) {
            m_worker->cancelInitialization();
        }
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
    // Define initialization tasks with varying durations to simulate real scenarios
    m_initTasks = {
        {"config",     "Loading configuration files",      800},
        {"database",   "Initializing database connection", 1200},
        {"preferences","Loading user preferences",         600},
        {"ui",         "Preparing UI components",          900},
        {"plugins",    "Loading plugins",                  1500},
        {"license",    "Verifying license",                400},
        {"services",   "Connecting to services",           700}
    };
}

void MainWindow::startInitialization()
{
    // Create worker and thread
    m_workerThread = new QThread(this);
    m_worker = new InitializationWorker();
    m_worker->moveToThread(m_workerThread);

    // Prepare task durations
    QVector<int> durations;
    for (const auto &task : m_initTasks) {
        durations.append(task.durationMs);
    }
    m_worker->setTaskDurations(durations);

    // Connect signals
    connect(m_workerThread, &QThread::started, m_worker, &InitializationWorker::runInitialization);
    connect(m_worker, &InitializationWorker::taskStarted, this, &MainWindow::onTaskStarted);
    connect(m_worker, &InitializationWorker::taskCompleted, this, &MainWindow::onTaskCompleted);
    connect(m_worker, &InitializationWorker::allTasksCompleted, this, &MainWindow::onAllTasksCompleted);
    connect(m_worker, &InitializationWorker::initializationError, this, &MainWindow::onInitializationError);

    // Cleanup connections
    connect(m_workerThread, &QThread::finished, m_worker, &QObject::deleteLater);

    // Log start
    QString timestamp = QDateTime::currentDateTime().toString("hh:mm:ss.zzz");
    m_logTextEdit->append(QString("[%1] Starting initialization...").arg(timestamp));

    // Start the worker thread
    m_workerThread->start();
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

void MainWindow::onTaskStarted(int taskIndex, const QString &taskName)
{
    // Emit signal for splash screen (1-based step)
    emit initializationStepStarted(taskIndex + 1, taskName);

    // Log in the main window
    QString timestamp = QDateTime::currentDateTime().toString("hh:mm:ss.zzz");
    m_logTextEdit->append(QString("[%1] >> %2...").arg(timestamp, taskName));
}

void MainWindow::onTaskCompleted(int /* taskIndex */)
{
    QString timestamp = QDateTime::currentDateTime().toString("hh:mm:ss.zzz");
    m_logTextEdit->append(QString("[%1]    [OK]").arg(timestamp));
}

void MainWindow::onAllTasksCompleted()
{
    QString timestamp = QDateTime::currentDateTime().toString("hh:mm:ss.zzz");
    m_logTextEdit->append(QString("[%1] ========================================").arg(timestamp));
    m_logTextEdit->append(QString("[%1] All initialization tasks completed!").arg(timestamp));
    m_logTextEdit->append(QString("[%1] ========================================").arg(timestamp));

    m_statusLabel->setText("All systems operational");
    m_statusLabel->setStyleSheet(
        "font-size: 12px; color: #ffffff; padding: 5px; "
        "background-color: #27ae60; border-radius: 3px; font-weight: bold;"
    );

    emit initializationComplete();
}

void MainWindow::onInitializationError(const QString &error)
{
    QString timestamp = QDateTime::currentDateTime().toString("hh:mm:ss.zzz");
    m_logTextEdit->append(QString("[%1] ERROR: %2").arg(timestamp, error));

    m_statusLabel->setText("Initialization failed: " + error);
    m_statusLabel->setStyleSheet(
        "font-size: 12px; color: #ffffff; padding: 5px; "
        "background-color: #e74c3c; border-radius: 3px; font-weight: bold;"
    );

    emit initializationFailed(error);
}
