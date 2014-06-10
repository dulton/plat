#include "videofiltersmodel.h"

#include <vlc/vlc.h>

#include <QtCore/QDebug>
#include <QtCore/QSet>
#include <QtCore/QSettings>

VideoFiltersModel::VideoFiltersModel(libvlc_instance_t* vlcInstance, QObject* parent)
    : QAbstractItemModel(parent)
    , m_vlcInstance(vlcInstance)
{
    load();
}

VideoFiltersModel::~VideoFiltersModel()
{
    save();
}

void VideoFiltersModel::load()
{
    QSet<QString> enabledFilters;

    QSettings settings;
    int size = settings.beginReadArray("filters");
    for (int i = 0; i < size; ++i) {
        settings.setArrayIndex(i);
        enabledFilters.insert(settings.value("name").toString());
    }
    settings.endArray();

    libvlc_module_description_t* list = libvlc_video_filter_list_get(m_vlcInstance);
    libvlc_module_description_t* module = list;
    while (module) {
        QString moduleName(module->psz_name);
        QString shortName(module->psz_shortname);
        QString description(module->psz_help);
        m_filters << VideoFilter(moduleName, shortName, description, enabledFilters.contains(moduleName));
        module = module->p_next;
    }
    libvlc_module_description_list_release(module);
}

void VideoFiltersModel::save()
{
    QSettings settings;
    settings.remove("filters");
    settings.beginWriteArray("filters");

    int j = 0;
    for (int i = 0; i < m_filters.size(); ++i) {
        const VideoFilter& filter = m_filters.at(i);
        if (filter.enabled) {
            settings.setArrayIndex(j++);
            settings.setValue("name", filter.name);
        }
    }

    settings.endArray();
}

QStringList VideoFiltersModel::enabledFilters() const
{
    QStringList enabledFilters;

    for (int i = 0; i < m_filters.size(); ++i) {
        const VideoFilter& filter = m_filters.at(i);
        if (filter.enabled)
            enabledFilters << filter.name;
    }

    return enabledFilters;
}

QVariant VideoFiltersModel::data(const QModelIndex& index, int role) const
{
    Q_ASSERT(index.isValid());
    Q_ASSERT(index.column() == 0);
    Q_ASSERT(index.row() >= 0 && index.row() < m_filters.size());

    switch (role) {
    case Qt::DisplayRole:
        return m_filters[index.row()].prettyName;
    case Qt::ToolTipRole:
        return m_filters[index.row()].description;
    case Qt::CheckStateRole:
        return m_filters[index.row()].enabled ? Qt::Checked : Qt::Unchecked;
    }

    return QVariant();
}

Qt::ItemFlags VideoFiltersModel::flags(const QModelIndex& index) const
{
    return QAbstractItemModel::flags(index) | Qt::ItemIsUserCheckable;
}

QModelIndex VideoFiltersModel::index(int row, int column, const QModelIndex& parent) const
{
    Q_UNUSED(parent)
#ifdef QT_5
    return createIndex(row, column, (void *)0);
#else
    return createIndex(row, column, 0);
#endif

}

int VideoFiltersModel::columnCount(const QModelIndex& parent) const
{
    return parent.isValid() ? 0 : 1;
}

QModelIndex VideoFiltersModel::parent(const QModelIndex& index) const
{
    Q_UNUSED(index)
    return QModelIndex();
}

int VideoFiltersModel::rowCount(const QModelIndex& parent) const
{
    return parent.isValid() ? 0 : m_filters.size();
}

bool VideoFiltersModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
    Q_ASSERT(index.isValid());
    Q_ASSERT(index.column() == 0);
    Q_ASSERT(index.row() >= 0 && index.row() < m_filters.size());

    switch (role) {
    case Qt::DisplayRole:
        return false;
    case Qt::CheckStateRole:
        VideoFilter& filter = m_filters[index.row()];

        Qt::CheckState checkState = static_cast<Qt::CheckState>(value.value<int>());
        filter.enabled = checkState == Qt::Checked;

        emit dataChanged(index, index);
        emit enabledFiltersChanged();
        return true;
    }

    return QAbstractItemModel::setData(index, value, role);
}
