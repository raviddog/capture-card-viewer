#include "obswidget.h"

#include "obs/obs.h"
#include <QDebug>
#include <QResizeEvent>
#include <QTextStream>

ObsWidget::ObsWidget(QWidget *parent)
    : QWidget(parent)
{
    initialize(_width, _height);
}

ObsWidget::~ObsWidget()
{
    if(_isOutput)
        stopStreaming();
}

void ObsWidget::initialize(int w, int h)
{
    if(!obs_startup("en-US", "./data", nullptr)) {
        qDebug() << "Failed to start OBS";
        return;
    }

    obs_add_data_path("./data/libobs/");
    obs_add_module_path("./obs-plugins/64bit/", "./data/obs-plugins/%module%/");

    obs_load_all_modules();
    obs_post_load_modules();

    resetVideo(60, w, h);
    resetAudio(48000);

    gs_init_data info = {};
    info.cx = w;
    info.cy = h;
    info.format = GS_BGRA;
    info.zsformat = GS_Z16;
    info.num_backbuffers = 1;
    info.window.hwnd = (HWND)this->winId();
    info.adapter = 0;

    _display = obs_display_create(&info, 0x00000000);
    if(!_display) {
        qDebug() << "Failed to init display";
        return;
    }

    //  display add callback
    obs_display_add_draw_callback(_display, renderCallback, this);

    // Create video capture source
    obs_data_t *settings = obs_data_create();
    _videoSource = obs_source_create("dshow_input", "Capture Card", settings, nullptr);
    obs_data_set_bool(settings, "capture_audio", true);
    obs_source_set_monitoring_type(_videoSource, OBS_MONITORING_TYPE_MONITOR_ONLY);
    obs_source_set_volume(_videoSource, 1.f);
    obs_source_set_muted(_videoSource, true);
    obs_data_release(settings);

    //  Create audio capture source
    settings = obs_data_create();
    _audioSource = obs_source_create("wasapi_input_capture", "Line In", settings, nullptr);
    obs_source_set_monitoring_type(_audioSource, OBS_MONITORING_TYPE_MONITOR_ONLY);
    obs_source_set_muted(_audioSource, true);
    obs_source_set_muted(_audioSource, true);
    obs_data_release(settings);

    obs_set_output_source(0, _videoSource);
    obs_set_output_source(0, _audioSource);
    _scene = obs_scene_create("MainScene");
    _sceneItem = obs_scene_add(_scene, _videoSource);
    _sceneItem = obs_scene_add(_scene, _audioSource);

    setSourceTransformFit();

    // setupOutput();

    _settingCache.setFileName("lastDevice.dat");
    loadSettings();
}

void ObsWidget::loadSettings()
{
    if(_settingCache.open(QIODevice::ReadOnly)) {
        QDataStream in(&_settingCache);

        QMap<QString, QString> data;
        in >> data;

        QString videoDevice = data.value("videoDevice");
        QChar useCaptureDeviceAudio = data.value("useCaptureDeviceAudio").front();
        QString audioDevice = data.value("audioDevice");
        QString audioDeviceId = data.value("audioDeviceId");

        setCaptureDevice(videoDevice);
        setCaptureAudio(useCaptureDeviceAudio == 'Y', QPair<QString, QString>(audioDevice, audioDeviceId));

        _settingCache.close();
    }
}

void ObsWidget::writeSettings()
{
    if(_settingCache.open(QIODevice::WriteOnly)) {
        QDataStream out(&_settingCache);
        QMap<QString, QString> data;

        data.insert("videoDevice", _currentVideoSource);
        data.insert("useCaptureDeviceAudio", _useVideoDeviceAudio ? "Y" : "N");
        data.insert("audioDevice", _currentAudioSource);
        data.insert("audioDeviceId", _currentAudioSourceId);

        out << data;
        _settingCache.close();
    }
}

void ObsWidget::renderCallback(void *param, uint32_t cx, uint32_t cy)
{
    Q_UNUSED(cx)
    Q_UNUSED(cy)

    ObsWidget *self = static_cast<ObsWidget*>(param);
    if(!self)
        return;

    obs_source_t *scene_source = obs_scene_get_source(self->_scene);
    obs_source_video_render(scene_source);
}

int ObsWidget::resetVideo(int fps, int w, int h)
{
    obs_video_info ovi{};
    ovi.graphics_module = "libobs-d3d11";
    ovi.fps_num         = fps;
    ovi.fps_den         = 1;
    ovi.base_width      = w;
    ovi.base_height     = h;
    ovi.output_width    = w;
    ovi.output_height   = h;
    ovi.output_format   = VIDEO_FORMAT_BGRA;
    ovi.adapter         = 0;
    ovi.gpu_conversion  = true;
    ovi.colorspace      = VIDEO_CS_DEFAULT;
    ovi.range           = VIDEO_RANGE_DEFAULT;
    ovi.scale_type      = OBS_SCALE_BICUBIC;

    int ret = obs_reset_video(&ovi);
    if (ret != OBS_VIDEO_SUCCESS) {
        qDebug() << "Video init failed with:" << ret;
    }

    return ret;
}

bool ObsWidget::resetAudio(int sampleRate)
{
    obs_audio_info oai = {};
    oai.samples_per_sec = sampleRate;
    oai.speakers = SPEAKERS_STEREO;

    bool ret = obs_reset_audio(&oai);
    if (!ret) {
        qDebug() << "Failed to init audio";
        return ret;
    }

    ret = obs_set_audio_monitoring_device("Default", "default");
    if (!ret) {
        qDebug() << "Failed to init audio monitoring";
    }
    return ret;
}

void ObsWidget::setSourceTransformFit()
{
    //  Set source to fit output
    obs_transform_info itemInfo;
    vec2_set(&itemInfo.pos, 0.0f, 0.0f);
    vec2_set(&itemInfo.scale, 1.0f, 1.0f);
    itemInfo.alignment = OBS_ALIGN_LEFT | OBS_ALIGN_TOP;
    itemInfo.rot = 0.0f;

    vec2_set(&itemInfo.bounds, _width, _height);
    itemInfo.bounds_type = OBS_BOUNDS_SCALE_INNER;
    itemInfo.bounds_alignment = OBS_ALIGN_CENTER;
    itemInfo.crop_to_bounds = false;

    obs_sceneitem_set_info2(_sceneItem, &itemInfo);
}

void ObsWidget::setPreviewEnabled(bool enabled)
{
    obs_display_set_enabled(_display, enabled);
}

void ObsWidget::setPreviewSize(int w, int h)
{
    obs_display_resize(_display, w, h);
}

QStringList ObsWidget::getWindowList()
{
    QStringList windowList;

    obs_properties_t *properties = obs_source_properties(_videoSource);
    obs_property_t *prop_window = obs_properties_get(properties, "window");

    size_t count = obs_property_list_item_count(prop_window);
    for(size_t i = 0; i < count; i++)
    {
        QString windowName = QString::fromUtf8(obs_property_list_item_string(prop_window, i));
        windowList.append(windowName);
    }

    return windowList;
}

QStringList ObsWidget::getVideoDeviceList()
{
    QStringList videoDevices;
    obs_properties_t *properties = obs_source_properties(_videoSource);
    obs_property_t *prop_device = obs_properties_get(properties, "video_device_id");

    size_t count = obs_property_list_item_count(prop_device);
    for(size_t i = 0; i < count; i++) {
        QString deviceName = QString::fromUtf8(obs_property_list_item_string(prop_device, i));
        videoDevices.append(deviceName);
    }
    return videoDevices;
}

QList<QPair<QString, QString>> ObsWidget::getAudioDeviceList()
{
    QList<QPair<QString, QString>> audioDevices;
    obs_properties_t *properties = obs_source_properties(_audioSource);
    obs_property_t *prop_device = obs_properties_get(properties, "device_id");

    size_t count = obs_property_list_item_count(prop_device);
    for(size_t i = 0; i < count; i++) {
        QString deviceName = QString::fromUtf8(obs_property_list_item_name(prop_device, i));
        QString deviceId = QString::fromUtf8(obs_property_list_item_string(prop_device, i));
        audioDevices.append(QPair<QString, QString>(deviceName, deviceId));
    }
    return audioDevices;
}

void ObsWidget::setCaptureDevice(QString deviceName)
{    
    if(deviceName.isEmpty())
        return;

    _currentVideoSource = deviceName;
    QByteArray winId = deviceName.toUtf8();

    obs_data_t *settings = obs_source_get_settings(_videoSource);
    obs_data_set_string(settings, "video_device_id", winId.constData());
    obs_data_set_int(settings, "buffering", 2);
    obs_data_set_int(settings, "res_type", 0);
    obs_data_set_bool(settings, "active", true);
    obs_data_set_string(settings, "color_space", "default");
    obs_data_set_string(settings, "color_range", "default");
    obs_data_set_int(settings, "audio_output_mode", 0);
    obs_source_update(_videoSource, settings);
    obs_data_release(settings);

    if(obs_source_muted(_videoSource)) {
        emit updateCurrentDevices(_currentVideoSource, _currentAudioSource);
    } else {
        emit updateCurrentDevices(_currentVideoSource, "Video Capture device");
    }

    writeSettings();
}

void ObsWidget::setCaptureWindow(QString windowName)
{
    if(windowName.isEmpty())
        return;
    QByteArray winId = windowName.toUtf8();
    obs_data_t *settings = obs_source_get_settings(_videoSource);
    obs_data_set_string(settings, "window", winId.constData());
    obs_source_update(_videoSource, settings);
    obs_data_release(settings);

    if(!_isOutput)
        startStreaming();
}

void ObsWidget::setCaptureAudio(bool useVideoDevice, QPair<QString, QString> device)
{
    _useVideoDeviceAudio = useVideoDevice;
    if(!device.first.isEmpty())
    {
        _currentAudioSource = device.first;
        _currentAudioSourceId = device.second;
        QByteArray deviceId = device.second.toUtf8();
        obs_data_t *settings = obs_source_get_settings(_audioSource);
        obs_data_set_string(settings, "device_id", deviceId.constData());
        obs_source_update(_audioSource, settings);
        obs_data_release(settings);
    }

    if(useVideoDevice) {
        obs_source_set_muted(_videoSource, false);
        obs_source_set_muted(_audioSource, true);
        emit updateCurrentDevices(_currentVideoSource, "Video Capture device");
    } else {
        obs_source_set_muted(_videoSource, true);
        obs_source_set_muted(_audioSource, false);
        emit updateCurrentDevices(_currentVideoSource, _currentAudioSource);
    }

    writeSettings();
}

void ObsWidget::setupOutput()
{
    /*
     * encoder list:
     * obs_qsv11
     * obs_nvenc_h264_tex
     * h264_texture_amf
     * ffmpeg_nvenc
     * ffmpeg_vaapi
     * ffmpeg_openh264
     * ffmpeg_aac
     * ffmpeg_opus
     *
     *
     * */

    obs_data_t *streamEncSettings = obs_data_create();
    obs_data_set_string(streamEncSettings, "rate_control",  "CBR");
    obs_data_set_int(   streamEncSettings, "bitrate",       4300);
    obs_data_set_string(streamEncSettings, "preset",        "p3");
    obs_data_set_string(streamEncSettings, "multipass",     "disabled");
    obs_data_set_int(   streamEncSettings, "bf",            0);
    obs_data_set_bool(  streamEncSettings, "lookahead",     false);
    obs_data_set_string(streamEncSettings, "profile",       "main");
    obs_data_set_string(streamEncSettings, "tune",          "ull");

    obs_encoder_t *videoStreaming = obs_video_encoder_create("obs_nvenc_h264_tex", "adv_video_stream", streamEncSettings, nullptr);
    obs_encoder_set_video(videoStreaming, obs_get_video());
    // obs_encoder_release(videoStreaming);
    obs_data_release(streamEncSettings);

    obs_encoder_t *audioStreaming = obs_audio_encoder_create("ffmpeg_opus", "adv_audio_stream", nullptr, 0, nullptr);
    obs_encoder_set_audio(audioStreaming, obs_get_audio());
    // obs_encoder_release(audioStreaming);

    obs_data_t *serviceSettings = obs_data_create();
    obs_data_set_string(serviceSettings, "server", "srt://localhost:8890?streamid=publish:raviddog:060905e2-1492-4dcf-a3be-30bda7ec84c7:&pkt_size=1316");
    obs_data_set_bool(serviceSettings, "use_auth", false);
    obs_data_set_string(serviceSettings, "key", "");
    obs_data_set_bool(serviceSettings, "bwtets", false);
    obs_data_set_bool(serviceSettings, "repeat_headers", true);

    obs_service_t *service = obs_service_create("rtmp_custom", "default_service", serviceSettings, nullptr);
    obs_data_release(serviceSettings);

    obs_data_t *output_settings = obs_data_create();
    obs_data_set_string(output_settings, "bind_ip", "default");
    obs_data_set_string(output_settings, "ip_family", "IPv4+IPv6");
    obs_data_set_bool(output_settings, "new_socket_loop_enabled", false);
    obs_data_set_bool(output_settings, "low_latency_mode_enabled", true);
    obs_data_set_bool(output_settings, "dyn_bitrate", false);

    _output = obs_output_create("ffmpeg_mpegts_muxer", "adv_stream", output_settings, nullptr);
    obs_data_release(output_settings);

    obs_output_set_video_encoder(_output, videoStreaming);
    obs_output_set_audio_encoder(_output, audioStreaming, 0);

    obs_output_set_delay(_output, 0, OBS_OUTPUT_DELAY_PRESERVE);
    obs_output_set_reconnect_settings(_output, 25, 2);

    obs_output_set_service(_output, service);
}

void ObsWidget::startStreaming()
{
    obs_output_start(_output);
    _isOutput = true;
}

void ObsWidget::stopStreaming()
{
    obs_output_stop(_output);
    _isOutput = false;
}
