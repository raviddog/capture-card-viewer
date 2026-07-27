#include "capturedevicepicker.h"
#include "ui_capturedevicepicker.h"

CaptureDevicePicker::CaptureDevicePicker(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::CaptureDevicePicker)
{
    ui->setupUi(this);

    connect(ui->deviceList, &QListWidget::itemClicked, this, &CaptureDevicePicker::deviceClicked);
}

CaptureDevicePicker::~CaptureDevicePicker()
{
    delete ui;
}

void CaptureDevicePicker::deviceClicked(QListWidgetItem *item)
{
    if(!item)
        return;

    if(item->text().isEmpty())
        return;

    emit selectDevice(item->text());
}

void CaptureDevicePicker::loadDeviceList(QStringList devices)
{
    ui->deviceList->clear();
    ui->deviceList->addItems(devices);
}
