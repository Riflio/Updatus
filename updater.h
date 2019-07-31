#ifndef UPDATER_H
#define UPDATER_H

#include <QObject>
#include "packadge.h"
#include "downloadmanager.h"

/**
* @brief Обновляем/устанавливаем пакеты физически
*/
class updater : public QObject
{
    Q_OBJECT
public:
    explicit updater(QObject *parent = nullptr);
    int goInstall(const QList<PackadgeCandidate*> &instList);

signals:

public slots:

private slots:
    void onDownloadComplete(QTemporaryFile * cnfFile);

private:
    DownloadManager * _dlMr;

};

#endif // UPDATER_H
