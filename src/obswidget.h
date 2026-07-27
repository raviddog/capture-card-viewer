#ifndef OBSWIDGET_H
#define OBSWIDGET_H

#include <QWidget>
#include <QStringList>
#include <QFile>

#include "obs/obs.h"

class ObsWidget : public QWidget
{
public:
    explicit ObsWidget(QWidget *parent = nullptr);
    ~ObsWidget();

    void initialize(int w, int h);

    QStringList getWindowList();
    QStringList getCaptureDeviceList();

    void setCaptureDevice(QString deviceName);
    void setCaptureWindow(QString windowName);
    void setCaptureAudio(bool isCapture);

    void startStreaming();
    void stopStreaming();

    static void renderCallback(void *param, uint32_t cx, uint32_t cy);

private:

    int resetVideo(int fps, int w, int h);
    bool resetAudio(int sampleRate);
    void setSourceTransformFit();

    void setupOutput();


    void setPreviewEnabled(bool enabled);
    void setPreviewSize(int w, int h);

    obs_display_t *_display = nullptr;
    obs_scene_t *_scene = nullptr;
    obs_source_t *_source = nullptr;
    obs_sceneitem_t *_sceneItem = nullptr;

    obs_output_t *_output = nullptr;

    bool _isOutput = false;


    int _width = 1920;
    int _height = 1080;

    QFile _settingCache;
};

#endif // OBSWIDGET_H
