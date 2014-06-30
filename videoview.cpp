#include "videoview.h"
#include "videofiltersmodel.h"

#ifdef QT_5
#include <QDir>
#include <QApplication>
#include <QDesktopWidget>
#include <QPalette>
#include <QSize>
#else
#include <QtCore/QDir>
#include <QtGui/QApplication>
#include <QtGui/QDesktopWidget>
#endif


static const char* vlcArguments[] = {
    "--intf=dummy",
    "--ignore-config",
    "--no-media-library",
    "--no-one-instance",
    "--no-osd",
    "--no-snapshot-preview",
    "--no-stats",
    "--no-video-title-show",
    "-vvv"
};


VideoView::VideoView(QWidget *parent) :
    QWidget(parent),
    m_vlcInstance(libvlc_new(sizeof(vlcArguments) / sizeof(vlcArguments[0]), vlcArguments)),
    m_vlcMediaPlayer(0),
    m_recording(false),
    m_start(false),
    m_videoFiltersModel(new VideoFiltersModel(m_vlcInstance, this)) {

    setAutoFillBackground(true);

    QPalette palette = this->palette();
    palette.setColor(QPalette::Window, Qt::black);
    setPalette(palette);
}

VideoView::~VideoView() {
    /*
    libvlc_media_player_release(m_vlcMediaPlayer);
    if(m_videoFiltersModel != NULL) {
        delete m_videoFiltersModel;
    }
    */
    libvlc_release(m_vlcInstance);
}

void VideoView::start() {
    Q_ASSERT(!m_vlcMediaPlayer);


    //libvlc_media_t* vlcMedia = libvlc_media_new_location(m_vlcInstance, webcamMrl);
    libvlc_media_t* vlcMedia = libvlc_media_new_path(m_vlcInstance, _localsdp.toStdString().c_str());
    if (m_recording) {
        Q_ASSERT(!m_recordingFilePath.isEmpty());

        // FIXME: Some of these parameters could be configurable.
        const char* recordingOptionPattern =
                "sout=#duplicate{"
                    "dst=display,"
                    "dst='transcode{vcodec=theo,vb=1800,acodec=vorb,ab=128}:standard{access=file,dst=%1}'"
                "}";

        QString recordingOption = QString(recordingOptionPattern).arg(m_recordingFilePath);
        libvlc_media_add_option(vlcMedia, recordingOption.toUtf8().constData());
        libvlc_media_add_option(vlcMedia, "v4l2-caching=100");
    }

    QStringList enabledFilters = m_videoFiltersModel->enabledFilters();
    if (!enabledFilters.isEmpty()) {
        QString filtersList = enabledFilters.join(":");

        libvlc_media_add_option(vlcMedia, QString(":video-filter=%1").arg(filtersList).toUtf8().constData());
        if (m_recording)
            libvlc_media_add_option(vlcMedia, QString(":sout-transcode-vfilter=%1").arg(filtersList).toUtf8().constData());
    }

    m_vlcMediaPlayer = libvlc_media_player_new_from_media(vlcMedia);
    libvlc_media_release(vlcMedia);

#if defined(Q_OS_LINUX)
    libvlc_media_player_set_xwindow(m_vlcMediaPlayer, winId());
#elif defined(Q_OS_MAC)
    libvlc_media_player_set_nsobject(m_vlcMediaPlayer, winId());
#elif defined(Q_OS_WIN)
    libvlc_media_player_set_hwnd(m_vlcMediaPlayer, (void *)this->winId());
#else
//#error "unsupported platform"
#endif

    libvlc_media_player_play(m_vlcMediaPlayer);
    m_start = true;
}

void VideoView::stop() {
    Q_ASSERT(m_vlcMediaPlayer);

    libvlc_media_player_stop(m_vlcMediaPlayer);
    libvlc_media_player_release(m_vlcMediaPlayer);
    m_vlcMediaPlayer = 0;
    m_start = false;
}

void VideoView::takeSnapshot(const QString &filePath) {
    Q_ASSERT(m_vlcMediaPlayer);

    libvlc_video_take_snapshot(m_vlcMediaPlayer, 0, filePath.toUtf8().constData(), 0, 0);
}

void VideoView::startRecording(const QString &filePath) {
    Q_ASSERT(m_vlcMediaPlayer);
    Q_ASSERT(!m_recording);

    m_recording = true;
    m_recordingFilePath = filePath;
    stop();
    start();
}

void VideoView::stopRecording() {
    Q_ASSERT(m_vlcMediaPlayer);
    Q_ASSERT(m_recording);

    m_recording = false;
    m_recordingFilePath.clear();
    stop();
    start();
}

void VideoView::setSize(const QSize &size) {
    if(size.isValid()) {
        this->setMinimumSize(size.width(), size.height());
        this->setMaximumSize(size.width(), size.height());
    }
    return;
}

QSize VideoView::sizeHint() const {
    return QSize(400, 300);
}

void VideoView::closeEvent(QCloseEvent *event) {
    stop();
    QWidget::closeEvent(event);
}

QString VideoView::localsdp() const {
    return _localsdp;
}

void VideoView::setLocalsdp(const QString &localsdp) {
    _localsdp = localsdp;
}


void VideoView::enabledFiltersChanged()
{
    if (m_vlcMediaPlayer) {
        stop();
        start();
    }
}
