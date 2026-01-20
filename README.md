# Qt Custom Splash Screen Widget

A professional, reusable splash screen widget for Qt C++ applications with modern UI/UX design and proper multithreading support for CPU-intensive initialization tasks.

## Features

### Splash Screen Widget
- **Modern UI Design**: Rounded corners, gradient backgrounds, subtle shadows
- **SVG Logo Support**: Load and display high-quality SVG logos
- **Dual Progress Modes**:
  - **Indeterminate**: Bouncing progress bar for unknown duration tasks
  - **Determinate**: Step-based progress bar with configurable total steps
- **Modern Progress Bar**: Gradient-styled progress indicator with smooth animations
- **Status Messages**: Real-time feedback on initialization progress
- **QSS Styling**: Full support for Qt StyleSheets
- **Customizable Border Radius**: Adjustable rounded corners
- **Minimum Display Duration**: Ensures splash visibility even for fast initializations

### MainWindow Initialization System
- **Centralized `initialize()` Method**: Single entry point for all initialization tasks
- **Task-Based Architecture**: Each initialization step is a separate method
- **Mixed Task Types**:
  - **Quick Tasks**: Run on main thread (config loading, license verification)
  - **Heavy Tasks**: Run on worker threads using QtConcurrent (database, plugins, services)
- **Thread-Safe UI Updates**: Uses `QMetaObject::invokeMethod` for safe cross-thread communication
- **Non-Blocking UI**: Event loop remains responsive during all operations
- **Progress Tracking**: Automatic signal emission for splash screen updates
- **Batch Processing**: Large UI updates (e.g., 10,000 table rows) processed in batches

## Architecture

### Threading Model

```
┌─────────────────────────────────────────────────────────────────┐
│                         MAIN THREAD                             │
│  ┌──────────────┐    ┌──────────────┐    ┌──────────────┐      │
│  │ SplashScreen │◄───│  MainWindow  │───►│  UI Widgets  │      │
│  │   Widget     │    │  initialize()│    │  (Table etc) │      │
│  └──────────────┘    └──────┬───────┘    └──────────────┘      │
│         ▲                   │                    ▲              │
│         │                   │                    │              │
│         │ signals      QtConcurrent::run    invokeMethod       │
│         │                   │                    │              │
└─────────┼───────────────────┼────────────────────┼──────────────┘
          │                   ▼                    │
┌─────────┼───────────────────────────────────────┼───────────────┐
│         │            WORKER THREADS             │               │
│         │     ┌─────────────────────────┐       │               │
│         └─────│   Heavy Task Execution  │───────┘               │
│               │  - Database connection  │                       │
│               │  - Plugin loading       │                       │
│               │  - Data generation      │                       │
│               └─────────────────────────┘                       │
└─────────────────────────────────────────────────────────────────┘
```

### Task Execution Flow

```
initialize()
    │
    ▼
┌─────────────────┐
│ Task 1 (Quick)  │──► Main Thread ──► emit signal ──► Splash updates
└────────┬────────┘
         ▼
┌─────────────────┐
│ Task 2 (Heavy)  │──► Worker Thread ──► QMetaObject::invokeMethod ──► onTaskCompleted()
└────────┬────────┘
         ▼
┌─────────────────┐
│ Task 3 (Quick)  │──► Main Thread ──► emit signal ──► Splash updates
└────────┬────────┘
         ▼
        ...
         ▼
┌─────────────────┐
│ Task N (Heavy)  │──► Worker Thread with UI batch updates
└────────┬────────┘
         ▼
emit initializationComplete()
```

## Requirements

- Qt 6.x (tested with Qt 6.10.1)
- CMake 3.16+
- Ninja (optional, recommended)
- C++17 compatible compiler
- Qt Modules: Core, Gui, Widgets, Svg, SvgWidgets, Concurrent

## Project Structure

```
qt-splash-screen/
├── src/
│   ├── SplashScreenWidget.h    # Splash screen header
│   ├── SplashScreenWidget.cpp  # Splash screen implementation
│   ├── MainWindow.h            # Main window with initialization system
│   ├── MainWindow.cpp          # Task implementations and threading
│   └── main.cpp                # Application entry point
├── resources/
│   ├── style/
│   │   └── light.qss           # Light theme stylesheet
│   └── icons/
│       └── app_logo.svg        # Example SVG logo
├── CMakeLists.txt              # CMake build configuration
├── build.bat                   # Windows build script (CMake + Ninja)
└── README.md                   # This file
```

## Quick Start

### Building (Windows)

1. Open Command Prompt or PowerShell
2. Navigate to project directory
3. Run the build script:

```batch
build.bat
```

The script will:
- Configure CMake with Ninja generator
- Build the project
- Copy required Qt DLLs (including Qt6Concurrent.dll)
- Launch the application

### Build Options

```batch
build.bat          # Build and run
build.bat clean    # Clean build (removes build/ and bin/) and run
build.bat norun    # Build only, don't run
```

### Manual CMake Build

```batch
mkdir build && cd build
cmake -G "Ninja" -DCMAKE_PREFIX_PATH="C:/eng_apps/Qt/6.10.1/mingw_64" ..
ninja
```

## Usage in Your Application

### Basic Integration

```cpp
#include "SplashScreenWidget.h"
#include "MainWindow.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    // Create and configure splash screen
    SplashScreenWidget splash;
    splash.setLogoPath(":/icons/your_logo.svg");
    splash.setAppName("Your Application");
    splash.setAppVersion("v1.0.0");
    splash.setMinimumDisplayDuration(2000);
    splash.setStyleSheetPath(":/style/theme.qss");

    // Create main window
    MainWindow mainWindow;

    // Configure determinate progress mode
    int totalSteps = mainWindow.getInitializationStepCount();
    splash.setTotalSteps(totalSteps);
    splash.setProgressMode(SplashScreenWidget::ProgressMode::Determinate);

    // Connect initialization signals to splash screen
    QObject::connect(&mainWindow, &MainWindow::initializationStepStarted,
                     [&splash](int step, const QString &message) {
                         splash.setProgress(step);
                         splash.setStatusMessage(message);
                     });

    QObject::connect(&mainWindow, &MainWindow::initializationComplete,
                     [&splash]() {
                         splash.finishSplash();
                     });

    QObject::connect(&splash, &SplashScreenWidget::splashFinished,
                     [&mainWindow]() {
                         mainWindow.show();
                     });

    // Start splash and initialization
    splash.startSplash();
    mainWindow.initialize();

    return app.exec();
}
```

## Implementing Initialization Tasks

### Task Structure in MainWindow

Tasks are defined using a simple structure that specifies:
- Task name and description
- The method to execute
- Whether it's a heavy task (runs on worker thread)

```cpp
struct InitTask {
    QString name;
    QString description;
    std::function<void()> taskMethod;
    bool isHeavyTask;  // true = run in worker thread
};
```

### Defining Tasks

```cpp
void MainWindow::setupInitializationTasks()
{
    m_initTasks = {
        // Quick tasks (main thread)
        {"config", "Loading configuration files",
            [this]() { taskLoadConfiguration(); }, false},

        {"license", "Verifying license",
            [this]() { taskVerifyLicense(); }, false},

        // Heavy tasks (worker thread)
        {"database", "Initializing database connection",
            [this]() { taskInitializeDatabase(); }, true},

        {"plugins", "Loading plugins",
            [this]() { taskLoadPlugins(); }, true},

        // Heavy task with UI updates
        {"datatable", "Populating data table (10,000 rows)",
            [this]() { taskPopulateDataTable(); }, true}
    };
}
```

### Quick Task Implementation

Quick tasks run on the main thread and are suitable for fast operations:

```cpp
void MainWindow::taskLoadConfiguration()
{
    // Read configuration files
    QSettings settings("app.ini", QSettings::IniFormat);
    m_config = settings.value("key").toString();

    // Quick tasks can update UI directly
    m_statusLabel->setText("Configuration loaded");
}
```

### Heavy Task Implementation

Heavy tasks run on a worker thread using QtConcurrent:

```cpp
void MainWindow::taskInitializeDatabase()
{
    // This runs in a worker thread - DO NOT access UI directly!

    // Simulate heavy work with cancellation support
    const int chunkSize = 50;
    int elapsed = 0;
    while (elapsed < 1000 && !m_cancelled) {
        QThread::msleep(chunkSize);
        elapsed += chunkSize;

        // Perform actual database work here
    }

    // If you need to update UI, use QMetaObject::invokeMethod:
    QMetaObject::invokeMethod(this, [this]() {
        m_statusLabel->setText("Database connected");
    }, Qt::QueuedConnection);
}
```

### Heavy Task with Batch UI Updates

For tasks that need to update large amounts of UI data (like populating a table with thousands of rows), use batch processing:

```cpp
void MainWindow::taskPopulateDataTable()
{
    // This runs in a worker thread

    const int TOTAL_ROWS = 10000;
    const int BATCH_SIZE = 500;

    for (int i = 0; i < TOTAL_ROWS && !m_cancelled; i += BATCH_SIZE) {
        // Generate batch data in worker thread
        QVector<QStringList> batch;
        for (int row = i; row < qMin(i + BATCH_SIZE, TOTAL_ROWS); ++row) {
            // CPU-intensive data generation
            double value = qSin(row * 0.1) * 100;
            batch.append({QString::number(row), QString::number(value)});
        }

        // Send batch to main thread for UI update
        // BlockingQueuedConnection ensures we wait for UI update
        // before generating next batch (prevents memory buildup)
        QMetaObject::invokeMethod(this, [this, batch]() {
            addTableRowsBatch(batch);
        }, Qt::BlockingQueuedConnection);

        // Small delay keeps UI extra responsive
        QThread::msleep(10);
    }
}

void MainWindow::addTableRowsBatch(const QVector<QStringList> &rows)
{
    // This runs on main thread - safe to update UI

    // Disable updates for faster batch insertion
    m_dataTable->setUpdatesEnabled(false);

    int startRow = m_dataTable->rowCount();
    m_dataTable->setRowCount(startRow + rows.size());

    for (int i = 0; i < rows.size(); ++i) {
        // Insert row data
        for (int col = 0; col < rows[i].size(); ++col) {
            m_dataTable->setItem(startRow + i, col,
                new QTableWidgetItem(rows[i][col]));
        }
    }

    // Re-enable updates
    m_dataTable->setUpdatesEnabled(true);

    // Process events to keep UI responsive
    QApplication::processEvents();
}
```

## Key Threading Patterns

### Pattern 1: Simple Worker Thread Task

```cpp
// In runNextTask():
if (task.isHeavyTask) {
    QtConcurrent::run([this, task]() {
        task.taskMethod();
        QMetaObject::invokeMethod(this, "onTaskCompleted", Qt::QueuedConnection);
    });
}
```

### Pattern 2: Worker Thread with UI Callback

```cpp
void MainWindow::heavyTask()
{
    // Heavy computation in worker thread
    auto result = performHeavyComputation();

    // Update UI from main thread
    QMetaObject::invokeMethod(this, [this, result]() {
        m_widget->setData(result);
    }, Qt::QueuedConnection);
}
```

### Pattern 3: Batch Processing with Blocking Queue

```cpp
void MainWindow::taskWithBatchUIUpdates()
{
    for (int batch = 0; batch < totalBatches; ++batch) {
        auto data = generateBatchData();

        // BlockingQueuedConnection: wait for UI update before continuing
        QMetaObject::invokeMethod(this, [this, data]() {
            updateUIWithBatch(data);
        }, Qt::BlockingQueuedConnection);
    }
}
```

### Pattern 4: Thread-Safe Logging

```cpp
void MainWindow::logMessage(const QString &message)
{
    // Check if we're on the main thread
    if (QThread::currentThread() != QApplication::instance()->thread()) {
        // Redirect to main thread
        QMetaObject::invokeMethod(this, [this, message]() {
            logMessage(message);
        }, Qt::QueuedConnection);
        return;
    }

    // Safe to update UI here
    m_logTextEdit->append(message);
}
```

## API Reference

### SplashScreenWidget

#### Configuration Methods

| Method | Description |
|--------|-------------|
| `setLogoPath(QString)` | Set SVG logo file path |
| `setLogoSize(QSize)` | Set logo display size |
| `setAppName(QString)` | Set application name |
| `setAppVersion(QString)` | Set version string |
| `setWindowSize(QSize)` | Set splash window size |
| `setBackgroundColor(QColor)` | Set background color (gradient base) |
| `setBorderRadius(int)` | Set corner radius in pixels (default: 20) |
| `setStyleSheetPath(QString)` | Load QSS stylesheet |
| `setMinimumDisplayDuration(int)` | Set minimum display time (ms) |
| `setTotalSteps(int)` | Set total progress steps |
| `setProgressMode(ProgressMode)` | Set Indeterminate or Determinate |

#### Control Methods

| Method | Description |
|--------|-------------|
| `startSplash()` | Show splash and start animations |
| `finishSplash()` | Signal that initialization is complete |
| `setProgress(int)` | Set current progress step |
| `incrementProgress()` | Increment progress by 1 |
| `setStatusMessage(QString)` | Update status message |

#### Signals

| Signal | Description |
|--------|-------------|
| `splashFinished()` | Emitted when splash is ready to close |
| `progressChanged(int, int)` | Emitted on progress update (current, total) |

### MainWindow

#### Public Methods

| Method | Description |
|--------|-------------|
| `initialize()` | Start the initialization process |
| `getInitializationTasks()` | Get list of task descriptions |
| `getInitializationStepCount()` | Get total number of tasks |

#### Signals

| Signal | Description |
|--------|-------------|
| `initializationStepStarted(int, QString)` | Emitted when a task starts |
| `initializationStepCompleted(int)` | Emitted when a task completes |
| `initializationComplete()` | Emitted when all tasks are done |
| `initializationFailed(QString)` | Emitted on error |

## Styling with QSS

The splash screen uses object names for styling:

```qss
/* Main container */
#splashScreen {
    background: #ffffff;
    border-radius: 12px;
}

/* App name */
#splashAppName {
    font-size: 28px;
    font-weight: bold;
    color: #2c3e50;
}

/* App version */
#splashAppVersion {
    font-size: 14px;
    color: #7f8c8d;
}

/* Progress bar */
#splashProgressBar {
    background-color: #e9ecef;
    border-radius: 3px;
}

#splashProgressBar::chunk {
    background: qlineargradient(x1:0, y1:0, x2:1, y2:0,
        stop:0 #667eea, stop:1 #764ba2);
    border-radius: 3px;
}

/* Status message */
#splashStatusLabel {
    font-size: 12px;
    color: #6c757d;
}
```

## Demo Application Features

The included demo application showcases:

1. **8 Initialization Tasks**:
   - Loading configuration files (quick)
   - Initializing database connection (heavy)
   - Loading user preferences (quick)
   - Preparing UI components (heavy)
   - Loading plugins (heavy)
   - Verifying license (quick)
   - Connecting to services (heavy)
   - Populating data table with 10,000 rows (heavy with UI updates)

2. **Data Table**: Demonstrates batch insertion of 10,000 rows with:
   - Color-coded status column
   - Numeric formatting
   - Alternating row colors
   - Non-blocking UI during population

3. **Initialization Log**: Real-time log of all initialization steps

## Performance Considerations

- **Batch Size**: For large UI updates, use batches of 100-500 items
- **BlockingQueuedConnection**: Use when you need to pace data generation with UI updates
- **QueuedConnection**: Use for fire-and-forget UI notifications
- **setUpdatesEnabled(false)**: Disable widget updates during batch insertions
- **processEvents()**: Call sparingly to keep UI responsive without excessive overhead

## License

MIT License - Feel free to use in your projects.

## Author

Cristian Tesconi (cristiantesco@gmail.com / geeky.tech.gt@gmail.com)
