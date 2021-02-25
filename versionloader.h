#ifndef VERSIONLOADER_H
#define VERSIONLOADER_H

#include <QDialog>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include "downloadmanager.h"

namespace Ui {
class versionLoader;
}


class versionLoader : public QDialog
{
    Q_OBJECT

public:
    explicit versionLoader(QWidget *parent = 0);
    ~versionLoader();

private slots:
    void reqFinished(QNetworkReply *reply);
    void sslError(QNetworkReply *reply,const QList<QSslError> &errors);
    void downloadFinished();
    void networkError(QNetworkReply::NetworkError code);
    void on_updateNow_clicked();
    void dlProgress(qint64 bytesReceived, qint64 bytesTotal);
    void log(QString msg);

private:
    Ui::versionLoader *ui;
    QString currVersion;
    QNetworkAccessManager *net;
    bool quit;
    bool updated;
    QString updateUrl;
    QString changelog;
    void cleanup();

    QString fileChecksum(const QString fileName, QCryptographicHash::Algorithm hashAlgorithm);
    DownloadManager dm;
    void setupNetwork();
    void sendRequest();
};

#endif // VERSIONLOADER_H
