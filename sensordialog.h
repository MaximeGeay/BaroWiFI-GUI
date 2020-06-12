#ifndef SENSORDIALOG_H
#define SENSORDIALOG_H


#include <QUdpSocket>
#include <QSerialPort>

class SensorDialog :public QObject
{
    Q_OBJECT
public:
    enum ConnexionType{
        Serie=0x00,UDP=0x01
    };

    SensorDialog();
    ~SensorDialog();
    void setSensorType(ConnexionType bType);
    ConnexionType getSensorType();

    bool setConnected();
    void setDisconnected();
    bool isConnected();
    bool sendMessage(QString sMessage);
    bool broadcastMessage(QString sMessage);

signals:
    void dataReceived(QString);
    void errorString(QString);

private slots:
     void readData();
private:

    ConnexionType mTypeConnexion;
    QSerialPort* mSeriaPort;
    QUdpSocket* mUdpSocket;
    QString mPortSerie;
    QString mBaudrate;
    QString mIpSensor;
    QString mTrameEnCours;
    QString mTrameEntiere;

    void init();

    int mPortUDP;
};

#endif // SENSORDIALOG_H
