#ifndef CAPTUREDEVICEPICKER_H
#define CAPTUREDEVICEPICKER_H

#include <QWidget>
#include <QListWidgetItem>

namespace Ui {
class CaptureDevicePicker;
}

class CaptureDevicePicker : public QWidget
{
    Q_OBJECT

public:
    explicit CaptureDevicePicker(QWidget *parent = nullptr);
    ~CaptureDevicePicker();

    void loadDeviceList(QStringList devices);

signals:
    void selectDevice(const QString &deviceId);

private slots:
    void deviceClicked(QListWidgetItem *item);

private:
    Ui::CaptureDevicePicker *ui;
};

#endif // CAPTUREDEVICEPICKER_H
