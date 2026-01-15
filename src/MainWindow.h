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
#include <atomic>

/**
 * @brief Worker class that runs initialization tasks in a separate thread.
 *
 * This follows the Single Responsibility Principle by handling
 * only the execution of initialization tasks.
 */
class InitializationWorker : public QObject
{
    Q_OBJECT

public:
    explicit InitializationWorker(QObject *parent = nullptr);

    void setTaskDurations(const QVector<int> &durations);

public slots:
    void runInitialization();
    void cancelInitialization();

signals:
    void taskStarted(int taskIndex, const QString &taskName);
    void taskCompleted(int taskIndex);
    void allTasksCompleted();
    void initializationError(const QString &error);

private:
    void simulateTask(int taskIndex, const QString &taskName, int durationMs);

    QVector<int> m_taskDurations;
    std::atomic<bool> m_cancelled;
};


/**
 * @brief Example MainWindow demonstrating splash screen integration.
 *
 * This class shows how to:
 * - Perform intensive initialization in a background thread
 * - Communicate progress to the splash screen
 * - Handle thread-safe status updates
 */
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

    /**
     * @brief Start the initialization process.
     * Call this after showing the splash screen.
     */
    void startInitialization();

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
     * @brief Emitted when all initialization is complete.
     */
    void initializationComplete();

    /**
     * @brief Emitted if initialization fails.
     */
    void initializationFailed(const QString &error);

private slots:
    void onTaskStarted(int taskIndex, const QString &taskName);
    void onTaskCompleted(int taskIndex);
    void onAllTasksCompleted();
    void onInitializationError(const QString &error);

private:
    void setupUi();
    void setupInitializationTasks();

    // UI Components
    QWidget *m_centralWidget;
    QVBoxLayout *m_mainLayout;
    QLabel *m_titleLabel;
    QTextEdit *m_logTextEdit;
    QLabel *m_statusLabel;

    // Threading
    QThread *m_workerThread;
    InitializationWorker *m_worker;

    // Task definitions
    struct InitTask {
        QString name;
        QString description;
        int durationMs;
    };
    QVector<InitTask> m_initTasks;
};

#endif // MAINWINDOW_H
