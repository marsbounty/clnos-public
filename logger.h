#ifndef LOGGER_H
#define LOGGER_H

#include <QObject>
#include <QPointer>
#include <QFile>
#include <QTextStream>

class Logger : public QObject
{
    Q_OBJECT
public:
    explicit Logger(QString file_name = "ClNos", bool standard = false, QObject *parent = nullptr);
    virtual ~Logger();
    static Logger *getGlobalLogger();
    static void setPath(QString path);
    static void DebugMessageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg);
    void addLog(QString prefix, QString msg);

    static void disableLogs(bool arg);
signals:

public slots:

private:
    static Logger* global_logger;
    static QString path;
    static bool logsDisabled;

    const QString file_name;
    QFile file;
    const bool is_gloabl_logger;
    QTextStream* log_stream;

    bool paused;

    void initializeLogFile();
    void close();
};

#endif // LOGGER_H
