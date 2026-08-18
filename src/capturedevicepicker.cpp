#include "capturedevicepicker.h"
#include "ui_capturedevicepicker.h"

CaptureDevicePicker::CaptureDevicePicker(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::CaptureDevicePicker)
{
    ui->setupUi(this);

    connect(ui->videoDeviceList, &QListWidget::itemClicked, this, &CaptureDevicePicker::videoDeviceClicked);
    connect(ui->audioDeviceList, &QListWidget::itemClicked, this, &CaptureDevicePicker::audioDeviceClicked);
    connect(ui->cbUseCaptureDeviceAudio, &QCheckBox::toggled, this, &CaptureDevicePicker::useCaptureDeviceAudioToggled);
}

CaptureDevicePicker::~CaptureDevicePicker()
{
    delete ui;
}

void CaptureDevicePicker::videoDeviceClicked(QListWidgetItem *item)
{
    if(!item || item->text().isEmpty())
        return;

    emit selectVideoDevice(item->text());
}

void CaptureDevicePicker::audioDeviceClicked(QListWidgetItem *item)
{
    if(!item || item->text().isEmpty())
        return;

    emit selectAudioDevice(ui->cbUseCaptureDeviceAudio->isChecked(), QPair<QString, QString>(item->text(), item->toolTip()));
}

void CaptureDevicePicker::useCaptureDeviceAudioToggled(bool toggled)
{
    ui->audioDeviceList->setEnabled(!toggled);
    emit selectAudioDevice(toggled, QPair<QString, QString>());
}

void CaptureDevicePicker::loadDeviceList(QStringList videoDevices, QList<QPair<QString, QString>> audioDevices)
{
    ui->videoDeviceList->clear();
    ui->videoDeviceList->addItems(videoDevices);
    ui->audioDeviceList->clear();
    for(QPair<QString, QString> &audioDevice : audioDevices) {
        QListWidgetItem *item = new QListWidgetItem(ui->audioDeviceList);
        item->setText(audioDevice.first);
        item->setToolTip(audioDevice.second);
    }
}

void CaptureDevicePicker::setCurrentDevices(QString videoDevice, QString audioDevice)
{
    ui->lblCurVideoDevice->setText(QString("Current Device: %1").arg(videoDevice));
    ui->lblCurAudioDevice->setText(QString("Current Device: %1").arg(audioDevice));
}
