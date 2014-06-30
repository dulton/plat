#ifndef VIDEOVIEW_H
#define VIDEOVIEW_H

#ifdef QT_5
#include <QWidget>
#include <QSet>
#include <QSize>
#else
#include <QtGui/QWidget>
#include <QtCore/QSet>
#endif
#include <vlc/vlc.h>
#include "videofiltersmodel.h"

class VideoView : public QWidget
{
    Q_OBJECT
public:
    explicit VideoView(QWidget *parent = 0);
    ~VideoView();
    void start();
    void stop();

    void takeSnapshot(const QString& filePath);

    void startRecording(const QString& filePath);
    void stopRecording();
    bool isRecording() const { return m_recording; }
    bool isStart() const {return m_start; }

    QSize videoSize() const;
    void setSize(const QSize &size);
    QSize sizeHint() const;
    VideoFiltersModel* videoFiltersModel() const { return m_videoFiltersModel; }
    QString localsdp() const;
    void setLocalsdp(const QString &localsdp);
signals:

protected slots:
    void enabledFiltersChanged();
private:
    void closeEvent(QCloseEvent* event);

private:
    libvlc_instance_t* m_vlcInstance;
    libvlc_media_player_t* m_vlcMediaPlayer;
    bool m_recording;
    bool m_start;
    QString m_recordingFilePath;
    VideoFiltersModel* m_videoFiltersModel;
    QString _localsdp;
};

#endif // VIDEOVIEW_H
