# Qt Splash Screen Widget

A production‑ready splash screen widget for Qt 6 C++ applications, plus a demo app that shows a safe, responsive initialization flow. This README is optimized for onboarding: it explains what the project is, how it is structured, how to build it, and how to integrate it into other apps.

---

## Table of Contents

1. Overview
2. Features
3. Requirements
4. Project Structure
5. Build and Deploy (Windows)
6. Manual Build (CMake)
7. Using the Widget
8. API Reference (SplashScreenWidget)
9. Initialization Flow Patterns
10. Example App Behavior
11. Troubleshooting
12. License

---

## 1. Overview

`SplashScreenWidget` provides a clean, professional startup experience for desktop apps. It supports both determinate and indeterminate progress, status messages, and QSS styling. The included example app demonstrates how to drive the splash screen from a real initialization pipeline without blocking the UI.

This repository contains:
- A shared library: **`qt-splash-screen`**
- An example application: **`qt-splash-screen-example`**

---

## 2. Features

### SplashScreenWidget
- Rounded corners, subtle drop shadow, gradient background
- SVG logo support
- Two progress modes
  - Indeterminate (animated)
  - Determinate (step‑based)
- Status message updates
- QSS styling support
- Configurable border radius
- Minimum display duration to avoid flicker

### Example Application
- Step‑based initialization with progress updates
- Safe UI updates from worker threads
- QSS + SVG resource loading

---

## 3. Requirements

- Qt 6.x (tested with Qt 6.10.1)
- CMake 3.16+
- C++17 compiler
- Qt modules: Core, Gui, Widgets, Svg, SvgWidgets, Concurrent

---

## 4. Project Structure

```
qt-splash-screen/
├── src/
│   ├── SplashScreenWidget.h
│   └── SplashScreenWidget.cpp
├── example/
│   ├── main.cpp
│   ├── MainWindow.h
│   └── MainWindow.cpp
├── resources/
│   ├── style/
│   │   └── light.qss
│   └── icons/
│       └── app_logo.svg
├── cmake/
│   ├── ProjectOptions.cmake
│   ├── ExternalLibraries.cmake
│   └── run_windeployqt.cmake
├── CMakeLists.txt
├── CMakePresets.json
├── project_manager.bat
└── README.md
```

---

## 5. Build and Deploy (Windows)

### Recommended Workflow
Use the interactive script:

```batch
project_manager.bat
```

Menu options:
- **Build Library**: builds only `qt-splash-screen`
- **Build Example**: builds only `qt-splash-screen-example`
- **Deploy**: creates a minimal deploy folder for the library only

### Build Output Location
Each preset builds into its own folder. Example:

```
build/windows-mingw64-debug/
```

### Deploy Output (Windows)
Deploy is intentionally minimal and contains only the library artifacts. It creates:

```
deploy/windows/debug/
deploy/windows/release/
```

and copies:
- `qt-splash-screen.dll`
- `libqt-splash-screen.dll.a` (if produced by MinGW)

The example executable is **not** deployed.

---

## 6. Manual Build (CMake)

### Using presets

```batch
cmake --preset windows-mingw64-debug
cmake --build --preset windows-mingw64-debug --target qt-splash-screen
```

### Without presets

```batch
mkdir build && cd build
cmake -G "Ninja" -DCMAKE_PREFIX_PATH="C:/path/to/Qt/6.x/your_kit" ..
ninja
```

---

## 7. Using the Widget

### Basic Integration

```cpp
#include <QApplication>
#include "SplashScreenWidget.h"
#include "MainWindow.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    SplashScreenWidget splash;
    splash.setLogoPath(":/icons/app_logo.svg");
    splash.setAppName("My App");
    splash.setAppVersion("v1.0.0");
    splash.setStyleSheetPath(":/style/light.qss");
    splash.setMinimumDisplayDuration(1500);

    MainWindow window;

    const int totalSteps = window.getInitializationStepCount();
    splash.setTotalSteps(totalSteps);
    splash.setProgressMode(SplashScreenWidget::ProgressMode::Determinate);

    QObject::connect(&window, &MainWindow::initializationStepStarted,
                     [&splash](int step, const QString &message) {
                         splash.setProgress(step);
                         splash.setStatusMessage(message);
                     });

    QObject::connect(&window, &MainWindow::initializationComplete,
                     [&splash]() { splash.finishSplash(); });

    QObject::connect(&splash, &SplashScreenWidget::splashFinished,
                     [&window]() { window.show(); });

    splash.startSplash();
    window.initialize();

    return app.exec();
}
```

### Progress Modes
- **Indeterminate**: use when total duration is unknown.
- **Determinate**: use when tasks can be counted (recommended).

---

## 8. API Reference (SplashScreenWidget)

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

---

## 9. Initialization Flow Patterns

### Pattern A: Main‑Thread Tasks (Quick)
Use for small, fast operations that can safely update UI directly.

```cpp
void MainWindow::taskLoadConfiguration() {
    QSettings settings("app.ini", QSettings::IniFormat);
    m_config = settings.value("key").toString();
    m_statusLabel->setText("Configuration loaded");
}
```

### Pattern B: Worker Thread Task
Use for CPU‑heavy operations that must not touch UI directly.

```cpp
void MainWindow::taskInitializeDatabase() {
    // Worker thread code
    performHeavySetup();

    // Safe UI update
    QMetaObject::invokeMethod(this, [this]() {
        m_statusLabel->setText("Database connected");
    }, Qt::QueuedConnection);
}
```

### Pattern C: Batch UI Updates
Use for large UI inserts (tables, lists).

```cpp
void MainWindow::taskPopulateDataTable() {
    const int TOTAL_ROWS = 10000;
    const int BATCH_SIZE = 500;

    for (int i = 0; i < TOTAL_ROWS; i += BATCH_SIZE) {
        QVector<QStringList> batch = generateBatch(i, BATCH_SIZE);

        QMetaObject::invokeMethod(this, [this, batch]() {
            addTableRowsBatch(batch);
        }, Qt::BlockingQueuedConnection);
    }
}
```

---

## 10. Example App Behavior

The example application demonstrates:
- Determinate progress updates
- Thread‑safe UI updates via signals and `QMetaObject::invokeMethod`
- Resource loading (QSS + SVG)

It is intentionally lightweight and **not** part of the deploy output.

---

## 11. Troubleshooting

### Build menu returns immediately to main menu
- Ensure `cmake` is in `PATH`.
- Run `project_manager.bat` from the project root (or let it set the root).

### Deploy folder is empty
- Ensure the library build completed for the selected preset.
- Deploy only copies library artifacts (not the example exe).

### Preset warnings about unused variables
- This repository uses a minimal CMake setup. Avoid unrelated cache variables.

---

## 12. License

MIT License.

---

## Author

Cristian Tesconi (cristiantesco@gmail.com / geeky.tech.gt@gmail.com)
