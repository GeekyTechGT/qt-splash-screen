#ifndef SPLASHSCREENWIDGET_H
#define SPLASHSCREENWIDGET_H

#include <QWidget>
#include <QLabel>
#include <QProgressBar>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QSvgWidget>
#include <QTimer>
#include <QElapsedTimer>
#include <QPropertyAnimation>
#include <QFile>
#include <QApplication>
#include <QPainter>
#include <QPainterPath>

/**
 * @brief Custom reusable splash screen widget for Qt applications.
 *
 * Features:
 * - SVG logo support
 * - Bouncing progress bar (indeterminate mode)
 * - Step-based progress bar (determinate mode)
 * - Status message display
 * - QSS stylesheet loading
 * - Minimum display duration
 *
 * SOLID Principles applied:
 * - Single Responsibility: Only handles splash screen display
 * - Open/Closed: Configurable via setters without modification
 * - Interface Segregation: Clean public interface
 */
class SplashScreenWidget : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(int progressPosition READ progressPosition WRITE setProgressPosition)

public:
    /**
     * @brief Progress bar display mode
     */
    enum class ProgressMode {
        Indeterminate,  ///< Bouncing progress bar
        Determinate     ///< Step-based progress bar
    };

    explicit SplashScreenWidget(QWidget *parent = nullptr);
    ~SplashScreenWidget() override;

    // Configuration setters
    void setLogoPath(const QString &svgPath);
    void setLogoSize(const QSize &size);
    void setMinimumDisplayDuration(int milliseconds);
    void setStyleSheetPath(const QString &qssPath);
    void setBackgroundColor(const QColor &color);
    void setWindowSize(const QSize &size);
    void setBorderRadius(int radius);
    void setTotalSteps(int steps);
    void setProgressMode(ProgressMode mode);
    void setAppName(const QString &name);
    void setAppVersion(const QString &version);

    // Progress control
    void setProgress(int step);
    void incrementProgress();
    void setStatusMessage(const QString &message);

    // Lifecycle control
    void startSplash();
    void finishSplash();
    bool isReadyToClose() const;

    // Getters
    int progressPosition() const { return m_progressPosition; }
    void setProgressPosition(int pos);

signals:
    /**
     * @brief Emitted when splash screen is ready to close
     * (minimum duration elapsed and finish was requested)
     */
    void splashFinished();

    /**
     * @brief Emitted when progress changes
     */
    void progressChanged(int current, int total);

protected:
    void paintEvent(QPaintEvent *event) override;

private slots:
    void onMinimumDurationElapsed();
    void updateBouncingProgress();

private:
    void setupUi();
    void applyRoundedMask();
    void loadStyleSheet(const QString &path);
    void startBouncingAnimation();
    void stopBouncingAnimation();
    void checkAndClose();

    // UI Components
    QVBoxLayout *m_mainLayout;
    QSvgWidget *m_logoWidget;
    QLabel *m_appNameLabel;
    QLabel *m_appVersionLabel;
    QProgressBar *m_progressBar;
    QLabel *m_statusLabel;
    QWidget *m_progressContainer;

    // Animation
    QTimer *m_bounceTimer;
    int m_progressPosition;
    int m_bounceDirection;
    static constexpr int BOUNCE_STEP = 2;
    static constexpr int BOUNCE_INTERVAL_MS = 15;

    // Configuration
    int m_minimumDisplayDuration;
    int m_totalSteps;
    int m_currentStep;
    ProgressMode m_progressMode;
    QSize m_logoSize;
    QSize m_windowSize;
    int m_borderRadius;
    QColor m_backgroundColor;

    // State tracking
    QElapsedTimer m_elapsedTimer;
    bool m_minimumDurationElapsed;
    bool m_finishRequested;
    bool m_isClosed;
};

#endif // SPLASHSCREENWIDGET_H
