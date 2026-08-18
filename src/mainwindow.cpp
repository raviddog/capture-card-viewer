#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QMouseEvent>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    QPalette p = palette();
    p.setColor(backgroundRole(), Qt::black);
    setPalette(p);

    _obsWidget = new ObsWidget;
    _obsWidget->setParent(centralWidget());
    _obsWidget->move(0, 0);

    _capturePicker = new CaptureDevicePicker;
    connect(_capturePicker, &CaptureDevicePicker::selectVideoDevice, _obsWidget, &ObsWidget::setCaptureDevice);
    connect(_capturePicker, &CaptureDevicePicker::selectAudioDevice, _obsWidget, &ObsWidget::setCaptureAudio);
    connect(_obsWidget, &ObsWidget::updateCurrentDevices, _capturePicker, &CaptureDevicePicker::setCurrentDevices);
    _capturePicker->setParent(centralWidget());
    _capturePicker->resize(400, 200);
    _capturePicker->move(0, 48);
    _capturePicker->setHidden(true);
    _capturePicker->raise();

    connect(ui->togglePickerWindow, &QPushButton::clicked, this, &MainWindow::sidebarToggled);
    ui->togglePickerWindow->hide();
    ui->togglePickerWindow->raise();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::fitSizeToContent()
{
    //  fit widget size
    if(!_obsWidget) return;
    resize(_obsWidget->size());
    resize(_obsWidget->size());
}

void MainWindow::mousePressEvent(QMouseEvent *event)
{
    if(event->button() == Qt::RightButton) {
        fitSizeToContent();
    }
}

void MainWindow::mouseDoubleClickEvent(QMouseEvent *event)
{
    if(event->button() == Qt::LeftButton)
    {
        if(windowState() & Qt::WindowFullScreen) {
            setWindowState(Qt::WindowNoState);
            fitSizeToContent();
        } else {
            setWindowState(Qt::WindowFullScreen);
        }
    }
}

void MainWindow::resizeEvent(QResizeEvent *event)
{
    Q_UNUSED(event);
    if(!_obsWidget) return;
    if(width() / 16 > height() / 9) {
        _obsWidget->resize(height() * 16 / 9, height());
    } else {
        _obsWidget->resize(width(), width() / 16 * 9);
    }
    // fitSizeToContent();
}

void MainWindow::sidebarToggled(bool currentlyShown)
{
    Q_UNUSED(currentlyShown);
    if(!_capturePicker || !_obsWidget)
        return;

    if(ui->togglePickerWindow->isChecked())
    {
        _capturePicker->setHidden(false);
        _capturePicker->loadDeviceList(_obsWidget->getVideoDeviceList(), _obsWidget->getAudioDeviceList());
    } else {
        _capturePicker->setHidden(true);
    }
}

void MainWindow::enterEvent(QEnterEvent *event)
{
    Q_UNUSED(event);
    ui->togglePickerWindow->show();
}

void MainWindow::leaveEvent(QEvent *event)
{
    Q_UNUSED(event);
    if(!_capturePicker) return;
    ui->togglePickerWindow->hide();
    ui->togglePickerWindow->setChecked(false);
    _capturePicker->setHidden(true);
}