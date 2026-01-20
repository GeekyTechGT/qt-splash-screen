#include <QApplication>
#include <QDir>
#include <QDebug>

#include "SplashScreenWidget.h"
#include "MainWindow.h"

/**
 * @brief Main entry point demonstrating the custom splash screen.
 *
 * This example shows how to:
 * 1. Create and configure the splash screen
 * 2. Connect it with MainWindow initialization
 * 3. Handle the transition from splash to main window
 *
 * Usage in your own applications:
 * - Copy SplashScreenWidget.h/.cpp to your project
 * - Configure the splash screen with your logo, colors, and styles
 * - Connect your initialization signals to the splash screen
 */
int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    // Set application info
    app.setApplicationName("Qt Splash Screen Demo");
    app.setApplicationVersion("1.0.0");
    app.setOrganizationName("GeekyTech");

    // ==========================================================================
    // SPLASH SCREEN SETUP
    // ==========================================================================

    SplashScreenWidget splash;

    // Configure appearance
    splash.setWindowSize(QSize(500, 400));
    splash.setLogoSize(QSize(120, 120));
    splash.setBorderRadius(24);
    splash.setAppName("Qt Splash Screen Demo");
    splash.setAppVersion("v1.0.0");
    splash.setBackgroundColor(QColor("#ffffff"));

    // Load custom logo (SVG)
    QString logoPath = QDir::currentPath() + "/resources/icons/app_logo.svg";
    splash.setLogoPath(logoPath);

    // Load custom stylesheet
    QString stylePath = QDir::currentPath() + "/resources/style/light.qss";
    splash.setStyleSheetPath(stylePath);

    // Configure timing
    splash.setMinimumDisplayDuration(3000);  // Minimum 3 seconds visible

    // ==========================================================================
    // MAIN WINDOW SETUP
    // ==========================================================================

    MainWindow mainWindow;

    // Configure splash for determinate progress based on MainWindow tasks
    // The splash screen will know exactly how many tasks to expect
    int totalSteps = mainWindow.getInitializationStepCount();
    splash.setTotalSteps(totalSteps);
    splash.setProgressMode(SplashScreenWidget::ProgressMode::Determinate);

    // ==========================================================================
    // SIGNAL CONNECTIONS
    // ==========================================================================

    // Connect MainWindow initialization progress to splash screen
    QObject::connect(&mainWindow, &MainWindow::initializationStepStarted,
                     [&splash](int step, const QString &message) {
                         splash.setProgress(step);
                         splash.setStatusMessage(message);
                     });

    // When initialization completes, signal splash to finish
    QObject::connect(&mainWindow, &MainWindow::initializationComplete,
                     [&splash]() {
                         splash.finishSplash();
                     });

    // Handle initialization errors
    QObject::connect(&mainWindow, &MainWindow::initializationFailed,
                     [&splash](const QString &error) {
                         splash.setStatusMessage("Error: " + error);
                         splash.finishSplash();
                     });

    // When splash is finished, show main window
    QObject::connect(&splash, &SplashScreenWidget::splashFinished,
                     [&mainWindow]() {
                         mainWindow.show();
                         mainWindow.raise();
                         mainWindow.activateWindow();
                     });

    // ==========================================================================
    // START APPLICATION
    // ==========================================================================

    // Show splash and start initialization
    splash.startSplash();

    // Start MainWindow initialization
    // Tasks run sequentially, with heavy tasks in background threads
    mainWindow.initialize();

    return app.exec();
}
