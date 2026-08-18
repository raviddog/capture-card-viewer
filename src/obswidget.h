#ifndef OBSWIDGET_H
#define OBSWIDGET_H

#include <QWidget>
#include <QStringList>
#include <QFile>

#include "obs/obs.h"

class ObsWidget : public QWidget
{
    Q_OBJECT
public:
    explicit ObsWidget(QWidget *parent = nullptr);
    ~ObsWidget();

    void initialize(int w, int h);

    QStringList getWindowList();
    QStringList getVideoDeviceList();
    QList<QPair<QString, QString>> getAudioDeviceList();

    void setCaptureDevice(QString deviceName);
    void setCaptureWindow(QString windowName);
    void setCaptureAudio(bool useVideoDevice, QPair<QString, QString> device);

    void startStreaming();
    void stopStreaming();

    static void renderCallback(void *param, uint32_t cx, uint32_t cy);

signals:
    void updateCurrentDevices(QString currentVideoSource, QString currentAudioSource);

private:

    int resetVideo(int fps, int w, int h);
    bool resetAudio(int sampleRate);
    void setSourceTransformFit();

    void setupOutput();

    void setPreviewEnabled(bool enabled);
    void setPreviewSize(int w, int h);

    void loadSettings();
    void writeSettings();

    obs_display_t *_display = nullptr;
    obs_scene_t *_scene = nullptr;
    obs_source_t *_videoSource = nullptr;
    obs_source_t *_audioSource = nullptr;
    obs_sceneitem_t *_sceneItem = nullptr;

    QString _currentVideoSource;
    QString _currentAudioSource;
    QString _currentAudioSourceId;
    bool    _useVideoDeviceAudio;

    obs_output_t *_output = nullptr;

    bool _isOutput = false;

    int _width = 1920;
    int _height = 1080;

    QFile _settingCache;
};

#endif // OBSWIDGET_H
