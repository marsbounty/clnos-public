/****************************************************************************
**
** Copyright (C) 2017 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** BSD License Usage
** Alternatively, you may use this file under the terms of the BSD license
** as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of The Qt Company Ltd nor the names of its
**     contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/


#include "downloadmanager.h"

#include <QTextStream>

#include <QDebug>
#include "logger.h"
#include <cstdio>

using namespace std;

DownloadManager::DownloadManager(QObject *parent)
    : QObject(parent)
{
    qDebug()<<"Downloader started";
    //Logger::getGlobalLogger()->addLog("UPDATER","Downloader started!");
}

DownloadManager::~DownloadManager()
{
    Logger::getGlobalLogger()->addLog("UPDATER","Downloader stopped!");
    //qDebug()<<"Downloader stopped";
}

void DownloadManager::append(const QStringList &urls)
{
    for (const QString &urlAsString : urls)
        append(QUrl::fromEncoded(urlAsString.toLocal8Bit()));

    if (downloadQueue.isEmpty())
        QTimer::singleShot(0, this, SIGNAL(finished()));
}

void DownloadManager::append(const QUrl &url)
{
    downloadQueue.enqueue(url);
    ++totalCount;
    //qDebug()<<"File recived in downloader!"<< url;
    Logger::getGlobalLogger()->addLog("UPDATER","File queued "+url.toString());
}

QString DownloadManager::saveFileName(const QUrl &url)
{

    QString path = url.path().replace("/nos/files",qApp->applicationDirPath());
    QString basename = path;//QFileInfo(path).fileName();

    QDir dir(QFileInfo(path).path());
    if (!dir.exists()) {
        dir.mkpath(".");
    }

    if (basename.isEmpty())
        basename = "download";


    if (QFile::exists(basename+".old")) {
        QFile::remove(basename+".old");
    }

    if (QFile::exists(basename)) {
        if(QFile::remove(basename)){
            //qDebug()<<"Removing old file";
            Logger::getGlobalLogger()->addLog("UPDATER","Removing old file");
        }else{
            QFile::rename(basename,basename+".old");
            //qDebug()<<"Couldn't remove file! Moved it.";
            Logger::getGlobalLogger()->addLog("UPDATER","Couldn't remove file! File moved.");
        }
    }
    return basename;
}

void DownloadManager::start()
{
    if (!downloadQueue.isEmpty())
        QTimer::singleShot(1, this, SLOT(startNextDownload()));
}

void DownloadManager::startNextDownload()
{
    //qDebug()<< "Loading next file";
    //Logger::getGlobalLogger()->addLog("UPDATER","Loading next file.");

    if (downloadQueue.isEmpty()) {
        emit log(QString("%1/%2 files downloaded successfully\n").arg(downloadedCount).arg(totalCount));
        emit finished();
        return;
    }

    QUrl url = downloadQueue.dequeue();

    QString filename = saveFileName(url);
    output.setFileName(filename);
    if (!output.open(QIODevice::WriteOnly)) {
        emit log(QString("Problem opening save file '%s' for download '%1': %2\n").arg(qPrintable(filename)).arg(url.toEncoded().constData()).arg(qPrintable(output.errorString())));

        startNextDownload();
        return;                 // skip this download
    }

    QNetworkRequest request(url);
    currentDownload = manager.get(request);
    connect(currentDownload, SIGNAL(downloadProgress(qint64,qint64)),
            SLOT(downloadProgress(qint64,qint64)));
    connect(currentDownload, SIGNAL(finished()),
            SLOT(downloadFinished()));
    connect(currentDownload, SIGNAL(readyRead()),
            SLOT(downloadReadyRead()));

    // prepare the output
    emit log(QString("Downloading %1...\n").arg(filename));
    downloadTime.start();
}

void DownloadManager::downloadProgress(qint64 bytesReceived, qint64 bytesTotal)
{
    emit dlProgress(bytesReceived, bytesTotal);
    //qDebug()<< bytesReceived << " from " << bytesTotal;

    // calculate the download speed
    double speed = bytesReceived * 1000.0 / downloadTime.elapsed();
    QString unit;
    if (speed < 1024) {
        unit = "bytes/sec";
    } else if (speed < 1024*1024) {
        speed /= 1024;
        unit = "kB/s";
    } else {
        speed /= 1024*1024;
        unit = "MB/s";
    }

    //progressBar.setMessage(QString::fromLatin1("%1 %2").arg(speed, 3, 'f', 1).arg(unit));
    //progressBar.update();
}

void DownloadManager::downloadFinished()
{
    //progressBar.clear();
    output.close();

    if (currentDownload->error()) {
        // download failed
        emit log(QString("Failed: %1\n").arg(qPrintable(currentDownload->errorString())));
        output.remove();
    } else {
        // let's check if it was actually a redirect
        if (isHttpRedirect()) {
            reportRedirect();
            output.remove();
        } else {
            emit log("Succeeded.\n");
            ++downloadedCount;
        }
    }

    currentDownload->deleteLater();
    startNextDownload();
}

void DownloadManager::downloadReadyRead()
{
    output.write(currentDownload->readAll());
}

bool DownloadManager::isHttpRedirect() const
{
    int statusCode = currentDownload->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    return statusCode == 301 || statusCode == 302 || statusCode == 303
           || statusCode == 305 || statusCode == 307 || statusCode == 308;
}

void DownloadManager::reportRedirect()
{
    int statusCode = currentDownload->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    QUrl requestUrl = currentDownload->request().url();
    qDebug() << "Request: " << requestUrl.toDisplayString()
                        << " was redirected with code: " << statusCode
                        << '\n';

    QVariant target = currentDownload->attribute(QNetworkRequest::RedirectionTargetAttribute);
    if (!target.isValid())
        return;
    QUrl redirectUrl = target.toUrl();
    if (redirectUrl.isRelative())
        redirectUrl = requestUrl.resolved(redirectUrl);
    qDebug() << "Redirected to: " << redirectUrl.toDisplayString()
                        << '\n';
}

int DownloadManager::getTotalCount() const
{
    return totalCount;
}
