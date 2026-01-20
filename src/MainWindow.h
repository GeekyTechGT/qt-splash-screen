#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QThread>
#include <QFuture>
#include <QFutureWatcher>
#include <QLabel>
#include <QVBoxLayout>
#include <QTextEdit>
#include <QProgressBar>
#include <QTableWidget>
#include <QMutex>
#include <functional>
#include <atomic>

/**
 * @brief Example MainWindow demonstrating splash screen integration.
 *
 * This class shows how to:
 * - Perform intensive initialization using internal task methods
 * - Communicate progress to the splash screen
 * - Handle thread-safe status updates using QMetaObject::invokeMethod
 * - Support both quick tasks and CPU-intensive tasks without blocking UI
 */
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

    /**
     * @brief Start the initialization process.
     * This method launches all initialization tasks (both quick and heavy)
     * in sequence, emitting progress signals for each completed task.
     */
    void initialize();

    /**
     * @brief Get the list of initialization task names.
     */
    QStringList getInitializationTasks() const;

    /**
     * @brief Get the number of initialization steps.
     */
    int getInitializationStepCount() const;

signals:
    /**
     * @brief Emitted when an initialization step starts.
     * @param step The current step number (1-based)
     * @param message Description of what's being initialized
     */
    void initializationStepStarted(int step, const QString &message);

    /**
     * @brief Emitted when an initialization step completes.
     * @param step The completed step number (1-based)
     */
    void initializationStepCompleted(int step);

    /**
     * @brief Emitted when all initialization is complete.
     */
    void initializationComplete();

    /**
     * @brief Emitted if initialization fails.
     */
    void initializationFailed(const QString &error);

private slots:
    void onTaskCompleted();

private:
    void setupUi();
    void setupInitializationTasks();
    void runNextTask();
    void logMessage(const QString &message);
    void logTaskStart(const QString &taskName);
    void logTaskComplete();

    // ========================================================================
    // INITIALIZATION TASK METHODS
    // Each task method performs a specific initialization operation.
    // Quick tasks run on the main thread.
    // Heavy tasks run on a worker thread using QtConcurrent.
    // ========================================================================

    // Quick tasks (main thread)
    void taskLoadConfiguration();
    void taskLoadUserPreferences();
    void taskVerifyLicense();

    // Heavy/CPU-intensive tasks (worker thread)
    void taskInitializeDatabase();
    void taskPrepareUIComponents();
    void taskLoadPlugins();
    void taskConnectToServices();

    // Heavy task with UI updates - populates table with 10k rows
    void taskPopulateDataTable();
    void addTableRowsBatch(const QVector<QStringList> &rows);

    // UI Components
    QWidget *m_centralWidget;
    QVBoxLayout *m_mainLayout;
    QLabel *m_titleLabel;
    QTextEdit *m_logTextEdit;
    QTableWidget *m_dataTable;
    QLabel *m_statusLabel;

    // Task management
    struct InitTask {
        QString name;
        QString description;
        std::function<void()> taskMethod;
        bool isHeavyTask;  // true = run in worker thread
    };
    QVector<InitTask> m_initTasks;
    int m_currentTaskIndex;
    std::atomic<bool> m_cancelled;

    // Threading
    QThread *m_workerThread;
    QMutex m_mutex;
};

#endif // MAINWINDOW_H
