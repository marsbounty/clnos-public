#include "versionloader.h"
#include "ui_versionloader.h"
#include <QNetworkReply>
#include <QXmlStreamReader>
#include <QApplication>
#include <settings.h>
#include <QFile>
#include <QDir>
#include <QProcess>
#include "logger.h"
#include "runguard.h"


versionLoader::versionLoader(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::versionLoader)
{
    ui->setupUi(this);
    currVersion = qApp->applicationVersion();
    quit = true;
    setWindowTitle("ClNos Update Check");
    setAttribute(Qt::WA_DeleteOnClose);
    cleanup();
    setupNetwork();
    sendRequest();
    updateUrl="";
    changelog = "";
    ui->changelogArea->hide();
    ui->progressBar->hide();
    updated = false;
    ui->updateNow->setEnabled(false);
    connect(&dm,SIGNAL(finished()),this,SLOT(downloadFinished()));
    connect(&dm,SIGNAL(log(QString)),this,SLOT(log(QString)));
    connect(&dm,SIGNAL(dlProgress(qint64,qint64)),this,SLOT(dlProgress(qint64,qint64)));
    setWindowIcon(QIcon(QPixmap(":/icons/ressources/Gold.png")));

}

versionLoader::~versionLoader()
{
    if(quit){
        exit(EXIT_SUCCESS);
    }
    delete ui;
}

QString versionLoader::fileChecksum(const QString fileName, QCryptographicHash::Algorithm hashAlgorithm)
{
    QFile f(fileName);
    if (f.open(QFile::ReadOnly)) {
        QCryptographicHash hash(hashAlgorithm);
        if (hash.addData(&f)) {
            return QString(hash.result().toHex());
        }
    }
    return "";
}


void versionLoader::reqFinished(QNetworkReply *reply)
{
    int status = 0;

    if(reply->error() == QNetworkReply::NoError){
        QXmlStreamReader reader(QString(reply->readAll()));
        QString msg;
        bool updateVendetta = Settings::getGlobalSettings()->getSetting("update/autoUpdateVendetta").toBool();
        bool updateNostale = Settings::getGlobalSettings()->getSetting("update/autoUpdateNostale").toBool();;

        while(!reader.atEnd() && !reader.hasError()) {
            if(reader.readNext() == QXmlStreamReader::StartElement) {
                if(reader.name()=="status"){
                    status = reader.readElementText().toInt();
                    if(status==1){
                        quit=false;
                        //msg.append("Version Up to Date :D");
                        //deleteLater();
                    }else if(status==0){
                        #ifdef QT_DEBUG
                            quit = false;
                        #endif
                    }else{
                        quit = true;
                    }
                }else if(reader.name()=="message"){
                    QString psa = reader.readElementText();
                    Settings::setPsa(psa);
                    if(quit){
                        msg.append(psa);
                    }
                }else if(reader.name()=="changelog"){
                    changelog = reader.readElementText();
                    if(!changelog.isEmpty()){
                          ui->changelog->setText(changelog);
                          ui->changelogArea->show();
                    }
                }else if(reader.name()=="dat"){
                    if(!updateNostale)return;
                    Settings::getGlobalSettings()->changeSetting("loginvars/dat",reader.readElementText());
                }else if(reader.name()=="xdat"){
                    if(!updateNostale)return;
                    Settings::getGlobalSettings()->changeSetting("loginvars/xdat",reader.readElementText());
                }else if(reader.name()=="nosversion"){
                    if(!updateNostale)return;
                    Settings::getGlobalSettings()->changeSetting("loginvars/version",reader.readElementText());
                }else if(reader.name()=="vendettaversion"){
                    if(!updateVendetta)return;
                    Settings::getGlobalSettings()->changeSetting("loginvars/vendettaversion",reader.readElementText());
                }else if(reader.name()=="vendettahash"){
                    if(!updateVendetta)return;
                    Settings::getGlobalSettings()->changeSetting("loginvars/vendettahash",reader.readElementText());
                }else if(reader.name()=="botnumber"){
                    Settings::getGlobalSettings()->changeSetting("max_botcount",reader.readElementText());    
                }else if(reader.name()=="files"){
                    QString hash = "";
                    QUrl url;
                    while(1){
                        if(reader.readNext()== QXmlStreamReader::StartElement) {
                            if(reader.name() == "url"){
                                //dm.append(QUrl(reader.readElementText()));
                                url = QUrl(reader.readElementText());
                            }else if(reader.name() == "hash"){
                                hash = reader.readElementText();
                            }

                        }else{
                            if(reader.name() == "files"){
                                break;
                            }else if(reader.name() == "file"){
                                if(!hash.isEmpty()&&!url.isEmpty()){
                                    QString path = url.path().replace("/nos/files",qApp->applicationDirPath());
                                    //Logger::getGlobalLogger()->addLog("UPDATER","Checking file hash for "+path);
                                    //qDebug()<<"Checking file hash for"<<path;
                                    if (QFile::exists(path)) {
                                        QString localHash = fileChecksum(path,QCryptographicHash::Md5);
                                        //Logger::getGlobalLogger()->addLog("UPDATER","Local Hash: "+localHash+" Remote Hash: "+hash);
                                        //qDebug()<< "Local hash "<< localHash;
                                        //qDebug()<< "remote hash"<< hash;
                                        if(localHash.toUpper()!=hash.toUpper()){
                                            dm.append(url);
                                            ui->updateNow->setEnabled(true);
                                            #ifndef QT_DEBUG
                                                quit = true;
                                            #endif
                                        }else{
                                            //Logger::getGlobalLogger()->addLog("UPDATER","File "+path+" up to date.");
                                            //qDebug()<<"File " << path << " already up to date!";
                                        }
                                    }else{
                                        dm.append(url);
                                        ui->updateNow->setEnabled(true);
                                        #ifndef QT_DEBUG
                                            quit = true;
                                        #endif

                                    }
                                    url.clear();
                                    hash.clear();
                                }
                            }
                        }
                    }
//                }else if(reader.name()=="updateurl"){
//                    updateUrl = reader.readElementText();
//                    if(!updateUrl.isEmpty()&&quit == true){
//                        ui->updateNow->setEnabled(true);
//                    }
                }
            }
        }
        if (reader.hasError())
        {
            ui->status->setText(reader.errorString());
            quit = true;
        }
        else if (reader.atEnd())
        {
            if(dm.getTotalCount()!=0){
                msg.append("Updates found!");
            }else if(status != 1){

            }else{
                deleteLater();
            }
            //msg.append(" Files found to check: " + QString::number(files.size()));
            ui->status->setText(msg);
        }

    }else{
        ui->status->setText(reply->errorString());
        quit = true;
    }
    delete reply;
}

void versionLoader::sslError(QNetworkReply *reply, const QList<QSslError> &errors)
{
    QString msg;
    foreach( const QSslError &error,errors){
        qWarning()<<error.errorString();
        msg.append(error.errorString());
        msg.append("<br>");
    }
    ui->status->setText(msg);
    quit = true;
}

void versionLoader::downloadFinished()
{
    ui->progressBar->hide();
    ui->status->setText("Download Finished");
    updated = true;
    ui->updateNow->setText("Restart");
    ui->updateNow->setEnabled(true);
}

void versionLoader::networkError(QNetworkReply::NetworkError code)
{
    //ui->status->setText(code.errorString());
}
void versionLoader::setupNetwork()
{
    net = new QNetworkAccessManager(this);
    //QList<QSslCertificate> allowedCAs;
    //QSslSocket::setDefaultCaCertificates(allowedCAs);
    connect(net,SIGNAL(finished(QNetworkReply*)),this,SLOT(reqFinished(QNetworkReply*)));
    connect(net,SIGNAL(sslErrors(QNetworkReply*,QList<QSslError>)),this,SLOT(sslError(QNetworkReply*,QList<QSslError>)));
    //qDebug()<<"Setting up NetworkManager!";
    Logger::getGlobalLogger()->addLog("UPDATER","Setting up Network Manager");
    ui->status->setText("Creating Connection to Server!");
}

void versionLoader::sendRequest()
{
    Logger::getGlobalLogger()->addLog("UPDATER","Checking for updates.");
    //qDebug()<<"Checking for App Updates!";
    ui->status->setText("Sending Request to Server!");
    net->get(QNetworkRequest(QUrl("https://clnos.someurl.com/clnos.php?version="+currVersion)));
}

void versionLoader::on_updateNow_clicked()
{
    ui->updateNow->setEnabled(false);
    ui->changelog->clear();
    if(updated){
        QProcess process;
        QString file = qApp->applicationDirPath() + "/clnos2.exe";
        process.startDetached(file);
        deleteLater();
    }else{
        dm.start();
        ui->progressBar->show();
        ui->status->setText("Downloading Updates.");
    }

}

void versionLoader::dlProgress(qint64 bytesReceived, qint64 bytesTotal)
{
    ui->progressBar->setMaximum(bytesTotal);
    ui->progressBar->setValue(bytesReceived);
}

void versionLoader::log(QString msg)
{
    ui->changelog->setText(ui->changelog->text() + msg);
}

void versionLoader::cleanup()
{
    QDirIterator it(qApp->applicationDirPath(), QStringList() << "*.old", QDir::Files, QDirIterator::Subdirectories);
    while (it.hasNext())
        QFile::remove(it.next());
}
