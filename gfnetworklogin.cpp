#include "gfnetworklogin.h"
#include <QtNetwork>
#include "settings.h"
#include <QDebug>
#include "player.h"
#include "playerui.h"
#include "logger.h"
#include "networkmanager.h"

gfNetworkLogin::gfNetworkLogin(Player *ply,QObject *parent) : QObject(parent),ply(ply)
{
    mng = new QNetworkAccessManager(this);
    connect(mng, &QNetworkAccessManager::sslErrors,this, &gfNetworkLogin::sslErrors);
    server = 0;
    username = "";
    token = "";
}

void gfNetworkLogin::startAuth(QString username, QString password, int server)
{
    QStringList gfLang({"en","de","fr","it","pl","es","cz","tr","ru"});
    QStringList locale({"en_US","de_DE","fr_FR","it_IT","pl_PL","es_ES","cs_CS","tr_TR","ru_RU"});

    QNetworkRequest request(QUrl("https://spark.gameforge.com/api/v1/auth/thin/sessions"));
    request.setRawHeader("User-Agent", "Mozilla/5.0 (Windows NT 6.3; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/65.0.3325.181 Safari/537.36");
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setRawHeader("Origin", "spark://www.gameforge.com");
    request.setRawHeader("TNT-Installation-Id",Settings::getGlobalSettings()->getSetting("hwid").toString().toUtf8());
    this->server = server;
    this->username = username;
    QJsonObject json;
    json.insert("gfLang", gfLang.at(server));
    json.insert("identity", username);
    json.insert("locale", locale.at(server));
    json.insert("password", password);
    json.insert("platformGameId", "dd4e22d6-00d1-44b9-8126-d8b40e0cd7c9");


    ply->getLog()->addLog("gfLogin",QString::fromStdString(QJsonDocument(json).toJson().toStdString()).replace(password,"*****"));

    if(username.contains("@")){
        connect(mng,SIGNAL(finished(QNetworkReply*)),this,SLOT(getAccounts(QNetworkReply*)));
    }else{
        connect(mng,SIGNAL(finished(QNetworkReply*)),this,SLOT(recieveToken(QNetworkReply*)));
    }
    mng->post(request, QJsonDocument(json).toJson());
    ply->getLog()->addLog("gfLogin","Sending auth request to server. locale: "+locale.at(server));
}


void gfNetworkLogin::getAccounts(QNetworkReply *reply){
    QByteArray response_data = reply->readAll();
    QJsonDocument out = QJsonDocument::fromJson(response_data);

    if(!out.object().contains("token")){
        ply->getLog()->addLog("gfLogin","Token ERROR:"+ reply->errorString());
        ply->gfLoginError(reply->errorString());
        reply->deleteLater();
        this->deleteLater();
        return;
    }

    QString tokentemp = out.object().value("token").toString();
    token = tokentemp;

    QNetworkRequest request(QUrl("https://spark.gameforge.com/api/v1/user/accounts"));
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setRawHeader("User-Agent", "Mozilla/5.0 (Windows NT 6.3; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/65.0.3325.181 Safari/537.36");
    request.setRawHeader("TNT-Installation-Id",Settings::getGlobalSettings()->getSetting("hwid").toString().toUtf8());//
    request.setRawHeader("Authorization",QString("Baerer %1").arg(token).toUtf8());
    request.setRawHeader("Origin", "spark://www.gameforge.com");

    disconnect(mng,SIGNAL(finished(QNetworkReply*)),this,SLOT(getAccounts(QNetworkReply*)));
    connect(mng,SIGNAL(finished(QNetworkReply*)),this,SLOT(recieveToken2(QNetworkReply*)));
    mng->get(request);
    reply->deleteLater();

}

void gfNetworkLogin::recieveToken2(QNetworkReply *reply)
{

    QByteArray response_data = reply->readAll();
    QJsonDocument out = QJsonDocument::fromJson(response_data);

    ply->getLog()->addLog("gfLogin",out.object().keys().join("---")+QString::fromStdString(out.toJson().toStdString()));
    QString mainkey = out.object().keys().at(0);
    QStringList usernames;
    foreach (QString id, out.object().keys()) {
        usernames.append( out.object().value(id).toObject().value("displayName").toString());
    }
    ply->getLog()->addLog("gfLogin",usernames.join("---"));
    reply->deleteLater();

    disconnect(mng,SIGNAL(finished(QNetworkReply*)),this,SLOT(recieveToken2(QNetworkReply*)));
    connect(mng,SIGNAL(finished(QNetworkReply*)),this,SLOT(recieveCode(QNetworkReply*)));


    QNetworkRequest request(QUrl("https://spark.gameforge.com/api/v1/auth/thin/codes"));
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setRawHeader("TNT-Installation-Id",Settings::getGlobalSettings()->getSetting("hwid").toString().toUtf8());//
    request.setRawHeader("Authorization",QString("Baerer %1").arg(token).toUtf8());
    request.setHeader(QNetworkRequest::UserAgentHeader,"GameforgeClient/2.0.48");
    request.setRawHeader("Origin", "spark://www.gameforge.com");

    QJsonObject json;
    json.insert("platformGameAccountId", mainkey);

    mng->post(request, QJsonDocument(json).toJson());

    ply->getLog()->addLog("gfLogin","Sending request for login Code for user "+usernames.at(0));

}

void gfNetworkLogin::recieveToken(QNetworkReply *reply)
{

    QByteArray response_data = reply->readAll();
    QJsonDocument out = QJsonDocument::fromJson(response_data);

    if(!out.object().contains("token")){
        ply->getLog()->addLog("gfLogin","Token ERROR:"+ reply->errorString());
        ply->gfLoginError(reply->errorString());
        reply->deleteLater();
        this->deleteLater();
        return;
    }

    ply->getLog()->addLog("gfLogin","Recieved login token."+reply->errorString());

    QString tokentemp = out.object().value("token").toString();
    token = tokentemp;
    QString platformGameAccountId = out.object().value("platformGameAccountId").toString();
    reply->deleteLater();

    ply->getLog()->addLog("gfLogin",QString(response_data) +QString::fromStdString(out.toJson().toStdString()));
    disconnect(mng,SIGNAL(finished(QNetworkReply*)),this,SLOT(recieveToken(QNetworkReply*)));
    connect(mng,SIGNAL(finished(QNetworkReply*)),this,SLOT(recieveCode(QNetworkReply*)));


    QNetworkRequest request(QUrl("https://spark.gameforge.com/api/v1/auth/thin/codes"));
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setRawHeader("TNT-Installation-Id",Settings::getGlobalSettings()->getSetting("hwid").toString().toUtf8());//
    request.setRawHeader("Authorization",QString("Baerer %1").arg(token).toUtf8());
    request.setHeader(QNetworkRequest::UserAgentHeader,"GameforgeClient/2.0.43");

    QJsonObject json;
    json.insert("platformGameAccountId", platformGameAccountId);

    mng->post(request, QJsonDocument(json).toJson());

    ply->getLog()->addLog("gfLogin","Sending request for login Code");

}

void gfNetworkLogin::recieveCode(QNetworkReply *reply){


    QByteArray response_data = reply->readAll();
    QJsonDocument out = QJsonDocument::fromJson(response_data);
    int statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();

    ply->getLog()->addLog("gfLogin",QString::number(reply->size())+"StatusCode: "+QString::number(statusCode)+QString(response_data) + QString::fromStdString(out.toJson().toStdString()));
    if(!out.object().contains("code")){
        ply->getLog()->addLog("gfLogin","Code ERROR:"+ reply->errorString());
        ply->getUI()->showGFLoginError(reply->errorString());
        reply->deleteLater();
        this->deleteLater();
        return;
    }
    ply->getLog()->addLog("gfLogin","Recieved login Code.");
    QString code = out.object().value("code").toString();
    ply->getUI()->showGFLoginError(QString(code.toUtf8().toHex()).toUpper());
    disconnect(mng,SIGNAL(finished(QNetworkReply*)));
    QList<int> ports = {4000,4001,4002,4003,4004,4005,4006,4008,4007,4000};

    ply->getNet()->gfLogin(QString(code.toUtf8().toHex()).toUpper(),ports.at(server),server);
    reply->deleteLater();
    this->deleteLater();

}

void gfNetworkLogin::sslErrors(QNetworkReply *reply, const QList<QSslError> &errors)
{
    ply->getLog()->addLog("gfLogin",reply->errorString()+"\n"+errors.at(0).errorString());
    ply->getUI()->showGFLoginError(reply->errorString()+"\n"+errors.at(0).errorString());
    reply->deleteLater();
    this->deleteLater();
}
