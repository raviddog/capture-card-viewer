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

    void loadDeviceList(QStringList videoDevices, QList<QPair<QString, QString>> audioDevices);

signals:
    void selectVideoDevice(const QString &deviceId);
    void selectAudioDevice(const bool useVideoDevice, const QPair<QString, QString> &device);

public slots:
    void setCurrentDevices(QString videoDevice, QString audioDevice);

private slots:
    void videoDeviceClicked(QListWidgetItem *item);
    void audioDeviceClicked(QListWidgetItem *item);
    void useCaptureDeviceAudioToggled(bool toggled);

private:
    Ui::CaptureDevicePicker *ui;
};

#endif // CAPTUREDEVICEPICKER_H
