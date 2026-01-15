# Qt Custom Splash Screen Widget

A professional, reusable splash screen widget for Qt C++ applications with modern UI/UX design.

## Features

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
- **Thread-Safe**: Works seamlessly with background initialization threads
- **SOLID Principles**: Clean, maintainable, and extensible code

## Requirements

- Qt 6.x (tested with Qt 6.10.1)
- CMake 3.16+
- Ninja (optional, recommended)
- C++17 compatible compiler
- Qt Modules: Core, Gui, Widgets, Svg, SvgWidgets

## Project Structure

```
qt-splash-screen/
├── src/
│   ├── SplashScreenWidget.h    # Splash screen header
│   ├── SplashScreenWidget.cpp  # Splash screen implementation
│   ├── MainWindow.h            # Example main window header
│   ├── MainWindow.cpp          # Example main window implementation
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
- Copy required Qt DLLs
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
#include "YourMainWindow.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    
    // Create and configure splash screen
    SplashScreenWidget splash;
    splash.setLogoPath(":/icons/your_logo.svg");
    splash.setAppName("Your Application");
    splash.setAppVersion("v1.0.0");
    splash.setMinimumDisplayDuration(2000);  // 2 seconds minimum
    splash.setStyleSheetPath(":/style/theme.qss");
    
    // Create main window
    YourMainWindow mainWindow;
    
    // Configure progress mode
    splash.setTotalSteps(mainWindow.getInitStepCount());
    splash.setProgressMode(SplashScreenWidget::ProgressMode::Determinate);
    
    // Connect signals
    QObject::connect(&mainWindow, &YourMainWindow::initProgress,
                     [&splash](int step, const QString &msg) {
                         splash.setProgress(step);
                         splash.setStatusMessage(msg);
                     });
    
    QObject::connect(&mainWindow, &YourMainWindow::initComplete,
                     &splash, &SplashScreenWidget::finishSplash);
    
    QObject::connect(&splash, &SplashScreenWidget::splashFinished,
                     &mainWindow, &QMainWindow::show);
    
    // Start
    splash.startSplash();
    mainWindow.startInitialization();
    
    return app.exec();
}
```

### Indeterminate Mode (Bouncing Progress)

```cpp
splash.setProgressMode(SplashScreenWidget::ProgressMode::Indeterminate);
splash.startSplash();

// When initialization is complete:
splash.finishSplash();
```

### Determinate Mode (Step-Based Progress)

```cpp
splash.setTotalSteps(5);
splash.setProgressMode(SplashScreenWidget::ProgressMode::Determinate);
splash.startSplash();

// During initialization:
splash.setProgress(1);
splash.setStatusMessage("Loading configuration...");

splash.setProgress(2);
splash.setStatusMessage("Connecting to database...");

// ... etc

splash.finishSplash();
```

## API Reference

### Configuration Methods

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

### Control Methods

| Method | Description |
|--------|-------------|
| `startSplash()` | Show splash and start animations |
| `finishSplash()` | Signal that initialization is complete |
| `setProgress(int)` | Set current progress step |
| `incrementProgress()` | Increment progress by 1 |
| `setStatusMessage(QString)` | Update status message |

### Signals

| Signal | Description |
|--------|-------------|
| `splashFinished()` | Emitted when splash is ready to close |
| `progressChanged(int, int)` | Emitted on progress update (current, total) |

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
    background: #3498db;
    border-radius: 3px;
}

/* Status message */
#splashStatusLabel {
    font-size: 12px;
    color: #6c757d;
}
```

## Thread Safety

The `MainWindow` example demonstrates proper thread-safe initialization:

1. Heavy work runs in a separate `QThread`
2. Progress updates are emitted via signals
3. Qt's signal-slot mechanism ensures thread-safe UI updates

```cpp
// Worker runs in background thread
class InitializationWorker : public QObject {
    Q_OBJECT
signals:
    void taskStarted(int index, const QString &name);
    void taskCompleted(int index);
    void allTasksCompleted();
};

// MainWindow connects worker signals to splash screen
connect(&worker, &InitializationWorker::taskStarted,
        this, &MainWindow::onTaskStarted);  // Thread-safe!
```

## License

MIT License - Feel free to use in your projects.

## Author

Cristian Tesconi (cristiantesco@gmail.com / geeky.tech.gt@gmail.com)