#ifndef SERVER_H
#define SERVER_H

#include <QMainWindow>
#include <QTcpSocket>
#include <QTcpServer>
#include <QNetworkAccessManager>

QT_BEGIN_NAMESPACE
namespace Ui { class Server; }
QT_END_NAMESPACE

class Server : public QMainWindow
{
    Q_OBJECT

public:
    Server(QWidget *parent = nullptr);
    ~Server();

private slots:
    virtual void slotNewConnection();
    void slotReadClient();
    void on_stopServer_clicked();
    void on_startServer_clicked();
    void replyFinished(QNetworkReply*);
    void sendToClient(QTcpSocket *pSocket, QByteArray bts);

private:
    QNetworkAccessManager* manager;
    Ui::Server *ui;
    QTcpServer *tcpServer;
    QString idName;
    quint16     blockSize;
    QList<QTcpSocket *> authClients;
    QList<QString> regClients;
};
#endif // SERVER_H
