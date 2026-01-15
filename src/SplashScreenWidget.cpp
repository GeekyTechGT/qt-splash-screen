#include "SplashScreenWidget.h"
#include <QScreen>
#include <QGuiApplication>
#include <QDebug>
#include <QGraphicsDropShadowEffect>
#include <QRegion>

SplashScreenWidget::SplashScreenWidget(QWidget *parent)
    : QWidget(parent)
    , m_mainLayout(nullptr)
    , m_logoWidget(nullptr)
    , m_appNameLabel(nullptr)
    , m_appVersionLabel(nullptr)
    , m_progressBar(nullptr)
    , m_statusLabel(nullptr)
    , m_progressContainer(nullptr)
    , m_bounceTimer(nullptr)
    , m_progressPosition(0)
    , m_bounceDirection(1)
    , m_minimumDisplayDuration(2000)
    , m_totalSteps(100)
    , m_currentStep(0)
    , m_progressMode(ProgressMode::Indeterminate)
    , m_logoSize(128, 128)
    , m_windowSize(450, 350)
    , m_borderRadius(20)
    , m_backgroundColor(255, 255, 255)
    , m_minimumDurationElapsed(false)
    , m_finishRequested(false)
    , m_isClosed(false)
{
    setupUi();
}

SplashScreenWidget::~SplashScreenWidget()
{
    if (m_bounceTimer) {
        m_bounceTimer->stop();
    }
}

void SplashScreenWidget::setupUi()
{
    // Window flags for splash screen behavior with transparency
    setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint | Qt::SplashScreen);
    setAttribute(Qt::WA_TranslucentBackground, true);
    setFixedSize(m_windowSize);

    // Main layout with adequate margins
    m_mainLayout = new QVBoxLayout(this);
    m_mainLayout->setContentsMargins(40, 35, 40, 30);
    m_mainLayout->setSpacing(10);

    // Logo container (centered)
    QWidget *logoContainer = new QWidget(this);
    logoContainer->setObjectName("logoContainer");
    QHBoxLayout *logoLayout = new QHBoxLayout(logoContainer);
    logoLayout->setContentsMargins(0, 0, 0, 0);

    m_logoWidget = new QSvgWidget(this);
    m_logoWidget->setFixedSize(m_logoSize);
    m_logoWidget->setObjectName("splashLogo");
    logoLayout->addStretch();
    logoLayout->addWidget(m_logoWidget);
    logoLayout->addStretch();

    m_mainLayout->addStretch(2);
    m_mainLayout->addWidget(logoContainer);

    // App name label
    m_appNameLabel = new QLabel(this);
    m_appNameLabel->setObjectName("splashAppName");
    m_appNameLabel->setAlignment(Qt::AlignCenter);
    m_appNameLabel->setText("Application");
    m_mainLayout->addWidget(m_appNameLabel);

    // App version label
    m_appVersionLabel = new QLabel(this);
    m_appVersionLabel->setObjectName("splashAppVersion");
    m_appVersionLabel->setAlignment(Qt::AlignCenter);
    m_appVersionLabel->setText("v1.0.0");
    m_mainLayout->addWidget(m_appVersionLabel);

    m_mainLayout->addStretch(3);

    // Progress section container with fixed layout
    m_progressContainer = new QWidget(this);
    m_progressContainer->setObjectName("progressContainer");
    QVBoxLayout *progressLayout = new QVBoxLayout(m_progressContainer);
    progressLayout->setContentsMargins(0, 0, 0, 0);
    progressLayout->setSpacing(12);

    // Modern progress bar
    m_progressBar = new QProgressBar(this);
    m_progressBar->setObjectName("splashProgressBar");
    m_progressBar->setTextVisible(false);
    m_progressBar->setMinimum(0);
    m_progressBar->setMaximum(100);
    m_progressBar->setValue(0);
    m_progressBar->setFixedHeight(12);
    progressLayout->addWidget(m_progressBar);

    // Status label with fixed height to prevent overlap
    m_statusLabel = new QLabel(this);
    m_statusLabel->setObjectName("splashStatusLabel");
    m_statusLabel->setAlignment(Qt::AlignCenter);
    m_statusLabel->setText("Initializing...");
    m_statusLabel->setFixedHeight(24);
    m_statusLabel->setWordWrap(false);
    progressLayout->addWidget(m_statusLabel);

    m_mainLayout->addWidget(m_progressContainer);

    // Bounce timer for indeterminate progress
    m_bounceTimer = new QTimer(this);
    connect(m_bounceTimer, &QTimer::timeout, this, &SplashScreenWidget::updateBouncingProgress);

    setObjectName("splashScreen");
}

void SplashScreenWidget::paintEvent(QPaintEvent * /* event */)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);

    // Create rounded rectangle path
    QPainterPath path;
    path.addRoundedRect(rect(), m_borderRadius, m_borderRadius);

    // Draw shadow first (offset)
    painter.save();
    painter.translate(0, 4);
    painter.fillPath(path, QColor(0, 0, 0, 40));
    painter.restore();

    // Draw background with gradient
    QLinearGradient gradient(0, 0, 0, height());
    gradient.setColorAt(0, m_backgroundColor);
    gradient.setColorAt(0.5, m_backgroundColor.darker(102));
    gradient.setColorAt(1, m_backgroundColor.darker(105));

    painter.fillPath(path, gradient);

    // Draw subtle border
    painter.setPen(QPen(QColor(0, 0, 0, 20), 1));
    painter.drawPath(path);
}

void SplashScreenWidget::applyRoundedMask()
{
    QPainterPath path;
    path.addRoundedRect(rect(), m_borderRadius, m_borderRadius);
    QRegion region(path.toFillPolygon().toPolygon());
    setMask(region);
}

void SplashScreenWidget::setLogoPath(const QString &svgPath)
{
    if (QFile::exists(svgPath)) {
        m_logoWidget->load(svgPath);
    } else {
        qWarning() << "SplashScreen: Logo file not found:" << svgPath;
    }
}

void SplashScreenWidget::setLogoSize(const QSize &size)
{
    m_logoSize = size;
    m_logoWidget->setFixedSize(size);
}

void SplashScreenWidget::setMinimumDisplayDuration(int milliseconds)
{
    m_minimumDisplayDuration = milliseconds;
}

void SplashScreenWidget::setStyleSheetPath(const QString &qssPath)
{
    loadStyleSheet(qssPath);
}

void SplashScreenWidget::loadStyleSheet(const QString &path)
{
    QFile styleFile(path);
    if (styleFile.open(QFile::ReadOnly | QFile::Text)) {
        QString styleSheet = QString::fromUtf8(styleFile.readAll());
        setStyleSheet(styleSheet);
        styleFile.close();
        qDebug() << "SplashScreen: Stylesheet loaded from" << path;
    } else {
        qWarning() << "SplashScreen: Could not load stylesheet:" << path;
    }
}

void SplashScreenWidget::setBackgroundColor(const QColor &color)
{
    m_backgroundColor = color;
    update();
}

void SplashScreenWidget::setWindowSize(const QSize &size)
{
    m_windowSize = size;
    setFixedSize(size);
}

void SplashScreenWidget::setBorderRadius(int radius)
{
    m_borderRadius = radius;
    update();
}

void SplashScreenWidget::setTotalSteps(int steps)
{
    m_totalSteps = steps;
    m_progressBar->setMaximum(steps);
}

void SplashScreenWidget::setProgressMode(ProgressMode mode)
{
    m_progressMode = mode;

    if (mode == ProgressMode::Indeterminate) {
        m_progressBar->setMaximum(100);
        m_progressBar->setValue(0);
    } else {
        m_progressBar->setMaximum(m_totalSteps);
        m_progressBar->setValue(m_currentStep);
    }
}

void SplashScreenWidget::setAppName(const QString &name)
{
    m_appNameLabel->setText(name);
}

void SplashScreenWidget::setAppVersion(const QString &version)
{
    m_appVersionLabel->setText(version);
}

void SplashScreenWidget::setProgress(int step)
{
    m_currentStep = qBound(0, step, m_totalSteps);

    if (m_progressMode == ProgressMode::Determinate) {
        m_progressBar->setValue(m_currentStep);
        emit progressChanged(m_currentStep, m_totalSteps);
    }
}

void SplashScreenWidget::incrementProgress()
{
    setProgress(m_currentStep + 1);
}

void SplashScreenWidget::setStatusMessage(const QString &message)
{
    m_statusLabel->setText(message);
    QApplication::processEvents();
}

void SplashScreenWidget::startSplash()
{
    m_elapsedTimer.start();
    m_minimumDurationElapsed = false;
    m_finishRequested = false;
    m_isClosed = false;

    // Center on screen
    if (QScreen *screen = QGuiApplication::primaryScreen()) {
        QRect screenGeometry = screen->availableGeometry();
        int x = (screenGeometry.width() - width()) / 2;
        int y = (screenGeometry.height() - height()) / 2;
        move(screenGeometry.x() + x, screenGeometry.y() + y);
    }

    show();
    raise();
    activateWindow();

    // Start bouncing animation if in indeterminate mode
    if (m_progressMode == ProgressMode::Indeterminate) {
        startBouncingAnimation();
    }

    // Timer for minimum duration
    QTimer::singleShot(m_minimumDisplayDuration, this, &SplashScreenWidget::onMinimumDurationElapsed);

    QApplication::processEvents();
}

void SplashScreenWidget::finishSplash()
{
    m_finishRequested = true;
    checkAndClose();
}

bool SplashScreenWidget::isReadyToClose() const
{
    return m_minimumDurationElapsed && m_finishRequested;
}

void SplashScreenWidget::setProgressPosition(int pos)
{
    m_progressPosition = pos;
    if (m_progressMode == ProgressMode::Indeterminate) {
        m_progressBar->setValue(pos);
    }
}

void SplashScreenWidget::onMinimumDurationElapsed()
{
    m_minimumDurationElapsed = true;
    checkAndClose();
}

void SplashScreenWidget::updateBouncingProgress()
{
    m_progressPosition += BOUNCE_STEP * m_bounceDirection;

    if (m_progressPosition >= 100) {
        m_progressPosition = 100;
        m_bounceDirection = -1;
    } else if (m_progressPosition <= 0) {
        m_progressPosition = 0;
        m_bounceDirection = 1;
    }

    m_progressBar->setValue(m_progressPosition);
}

void SplashScreenWidget::startBouncingAnimation()
{
    m_progressPosition = 0;
    m_bounceDirection = 1;
    m_bounceTimer->start(BOUNCE_INTERVAL_MS);
}

void SplashScreenWidget::stopBouncingAnimation()
{
    if (m_bounceTimer) {
        m_bounceTimer->stop();
    }
}

void SplashScreenWidget::checkAndClose()
{
    if (m_minimumDurationElapsed && m_finishRequested && !m_isClosed) {
        m_isClosed = true;
        stopBouncingAnimation();

        // Complete the progress bar before closing
        if (m_progressMode == ProgressMode::Determinate) {
            m_progressBar->setValue(m_totalSteps);
        } else {
            m_progressBar->setValue(100);
        }

        setStatusMessage("Ready!");
        QApplication::processEvents();

        // Small delay to show completion
        QTimer::singleShot(200, this, [this]() {
            emit splashFinished();
            close();
        });
    }
}
