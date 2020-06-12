#include<QSettings>
#include<QDebug>

#include "sensordialog.h"

SensorDialog::SensorDialog()
{
    mSeriaPort=new QSerialPort;
    mUdpSocket=new QUdpSocket;


    QObject::connect(mSeriaPort,&QSerialPort::readyRead,this,&SensorDialog::readData);
    QObject::connect(mUdpSocket,&QUdpSocket::readyRead,this,&SensorDialog::readData);
}

SensorDialog::~SensorDialog()
{
    delete mSeriaPort;
    delete mUdpSocket;
}

void SensorDialog::setSensorType(SensorDialog::ConnexionType bType)
{
    mTypeConnexion=bType;

}

SensorDialog::ConnexionType SensorDialog::getSensorType()
{
    return mTypeConnexion;
}



bool SensorDialog::setConnected()
{
    init();

    bool bRes=false;
    if(mTypeConnexion==Serie)
     {
        mSeriaPort->setPortName(mPortSerie);
        if(mBaudrate=="4800")
            mSeriaPort->setBaudRate(QSerialPort::Baud4800);
        if(mBaudrate=="9600")
            mSeriaPort->setBaudRate(QSerialPort::Baud9600);
        if(mBaudrate=="19200")
            mSeriaPort->setBaudRate(QSerialPort::Baud19200);
        if(mBaudrate=="38400")
            mSeriaPort->setBaudRate(QSerialPort::Baud38400);
        if(mBaudrate=="115200")
            mSeriaPort->setBaudRate(QSerialPort::Baud115200);

        mSeriaPort->setParity(QSerialPort::NoParity);
        mSeriaPort->setDataBits(QSerialPort::Data8);
        bRes=mSeriaPort->open(QIODevice::ReadWrite);
        if(bRes)
        {

            emit errorString(QString("Connecté à %1").arg(mPortSerie));
            mTrameEnCours="";
        }
        else
        {

            if(mSeriaPort->error()==QSerialPort::DeviceNotFoundError)
                emit errorString("Port série introuvable");
            else {
                if(mSeriaPort->error()==QSerialPort::PermissionError)
                    emit errorString("Problème de droits sur le port série");
                else {
                    emit errorString("Echec d'ouverture du port série");
                }
            }
        }
    }

    if(mTypeConnexion==UDP)
    {
        if(mUdpSocket->bind(QHostAddress::Any,mPortUDP))
        {
             emit errorString(QString("Connecté au port UDP %1").arg(mPortUDP));
            bRes=true;
        }
        else
        {
            emit errorString(mUdpSocket->errorString());
            bRes=false;
        }
    }



    return bRes;
}

void SensorDialog::setDisconnected()
{
    if(mTypeConnexion==Serie)
        mSeriaPort->close();
    if(mTypeConnexion==UDP)
        mUdpSocket->close();
}

bool SensorDialog::isConnected()
{
    if(mTypeConnexion==Serie)
        return mSeriaPort->isOpen();

    if(mTypeConnexion==UDP)
    {

       if(mUdpSocket->state()==QUdpSocket::BoundState)
        return  true;
    }


    return false;
}

bool SensorDialog::sendMessage(QString sMessage)
{
    bool bRes=false;

    if(!isConnected())
    {
        emit errorString(QString ("Capteur non connecté"));
        return false;
    }
    if(mTypeConnexion==Serie)
    {
        qint64 result=mSeriaPort->write(sMessage.toLocal8Bit());
        if(result==-1)
        {
            emit errorString(QString("Message non envoyé : %1").arg(sMessage));
            emit errorString(mSeriaPort->errorString());
            bRes=false;
        }
        else
        {
            emit errorString(QString("Message envoyé sur %1 : %2").arg(mPortSerie).arg(sMessage));
            bRes=true;
        }


    }

    if(mTypeConnexion==UDP)
    {


        QByteArray datagram = sMessage.toLocal8Bit();

       qint64 result=mUdpSocket->writeDatagram(datagram.data(), datagram.size(),QHostAddress(mIpSensor),mPortUDP);

       if (result==-1)
       {
           bRes=false;
           emit errorString(QString("Message non envoyé : %1").arg(sMessage));
           emit errorString(mUdpSocket->errorString());

       }
       else
       {
           emit errorString(QString("Message envoyé sur le port %1 : %2").arg(mPortUDP).arg(sMessage));
           bRes=true;

       }

    }

    return bRes;
}

bool SensorDialog::broadcastMessage(QString sMessage)
{
    bool bRes=false;

    if(!isConnected())
    {
        emit errorString(QString ("Capteur non connecté"));
        return false;
    }

    if(mTypeConnexion==Serie)
    {
        qint64 result=mSeriaPort->write(sMessage.toLocal8Bit());
        if(result==-1)
        {
            emit errorString(QString("Message non envoyé : %1").arg(sMessage));
            emit errorString(mSeriaPort->errorString());
            bRes=false;
        }
        else
        {
            emit errorString(QString("Message envoyé sur %1 : %2").arg(mPortSerie).arg(sMessage));
            bRes=true;
        }


    }

    if(mTypeConnexion==UDP)
    {

        QByteArray datagram = sMessage.toLocal8Bit();

       qint64 result=mUdpSocket->writeDatagram(datagram.data(), datagram.size(),QHostAddress("255.255.255.255"),mPortUDP);

       if (result==-1)
       {
           bRes=false;
           emit errorString(QString("Message non envoyé : %1").arg(sMessage));
           emit errorString(mUdpSocket->errorString());

       }
       else
       {
           emit errorString(QString("Message envoyé sur le port %1 : %2").arg(mPortUDP).arg(sMessage));
           bRes=true;

       }

    }

    return bRes;
}

void SensorDialog::readData()
{
    if(mTypeConnexion==Serie)
    {
        QByteArray cBuf;

        for(int i=0;i<mSeriaPort->bytesAvailable();i++)
        {
            cBuf=mSeriaPort->read(1);

            if(cBuf=="$")
            {
                mTrameEnCours=cBuf;
            }
            else if (cBuf=="\n") {
                QString sTrame=mTrameEnCours+"\n";
                emit dataReceived(sTrame);

                mTrameEnCours="";

            }
            else
            {
                mTrameEnCours=mTrameEnCours+cBuf;
            }
        }
    }

    if(mTypeConnexion==UDP)
    {
        while (mUdpSocket->hasPendingDatagrams())
        {
            QByteArray datagram;
            datagram.resize(mUdpSocket->pendingDatagramSize());
            mUdpSocket-> readDatagram(datagram.data(),datagram.size()); // datagram.data est un char
            QString sTrame=datagram.data();  // convertion qbytearray en string
            emit dataReceived(sTrame); // transmission du signal avec la trame recu



        }
    }
}

void SensorDialog::init()
{
    QSettings settings ("Barographe","BaroConfig");
    mPortSerie=settings.value("PortName","").toString();
    mBaudrate=settings.value("BaudRate",4800).toString();
    bool bType=settings.value("ConnecType",0).toBool();
    if(bType)
        mTypeConnexion=SensorDialog::UDP;
    else
        mTypeConnexion=SensorDialog::Serie;

    mIpSensor=settings.value("IpSensor").toString();
    mPortUDP=settings.value("PortUDP",50000).toInt();

}
