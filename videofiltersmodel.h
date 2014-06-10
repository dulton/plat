#ifndef VIDEOFILTERSMODEL_H
#define VIDEOFILTERSMODEL_H

#include <QtCore/QAbstractItemModel>

#include <vlc/vlc.h>
#include <QtCore/QStringList>

//class libvlc_instance_t;

class VideoFiltersModel : public QAbstractItemModel
{
    Q_OBJECT

private:
    struct VideoFilter {
        VideoFilter(const QString& name, const QString& prettyName, const QString& description, bool enabled = false)
            : name(name)
            , prettyName(prettyName)
            , description(description)
            , enabled(enabled)
        {
        }

        QString name;
        QString prettyName;
        QString description;
        bool enabled;
    };

public:
    VideoFiltersModel(libvlc_instance_t* vlcInstance, QObject* parent = 0);
    ~VideoFiltersModel();

    QStringList enabledFilters() const;

    QVariant data(const QModelIndex& index, int role) const;
    Qt::ItemFlags flags(const QModelIndex& index) const;
    int columnCount(const QModelIndex& parent) const;
    QModelIndex index(int row, int column, const QModelIndex& parent) const;
    QModelIndex parent(const QModelIndex& index) const;
    int rowCount(const QModelIndex& parent) const;
    bool setData(const QModelIndex& index, const QVariant& value, int role = Qt::EditRole);

signals:
    void enabledFiltersChanged();

protected:
    void load();
    void save();

private:
    libvlc_instance_t* m_vlcInstance;
    QList<VideoFilter> m_filters;
};

#endif // VIDEOFILTERSMODEL_H


