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
    mParam.typeConnexion=bType;

}

SensorDialog::ConnexionType SensorDialog::getSensorType()
{
    return mParam.typeConnexion;
}

void SensorDialog::setParameters(SensorDialog::Parameters param)
{
    mParam=param;
    QSettings settings ("Barographe","BaroConfig");
    settings.setValue("PortName",mParam.PortSerie);
    settings.setValue("Baudrate",mParam.Baudrate);
    settings.setValue("ConnecType",mParam.typeConnexion);
    settings.setValue("IpSensor",mParam.ipAddress);
    settings.setValue("PortUDP",mParam.PortUDP);

}

SensorDialog::Parameters SensorDialog::getParameters()
{
    return mParam;
}



bool SensorDialog::setConnected()
{
    init();

    bool bRes=false;
    if(mParam.typeConnexion==Serie)
     {
        mSeriaPort->setPortName(mParam.PortSerie);
        if(mParam.Baudrate=="4800")
            mSeriaPort->setBaudRate(QSerialPort::Baud4800);
        if(mParam.Baudrate=="9600")
            mSeriaPort->setBaudRate(QSerialPort::Baud9600);
        if(mParam.Baudrate=="19200")
            mSeriaPort->setBaudRate(QSerialPort::Baud19200);
        if(mParam.Baudrate=="38400")
            mSeriaPort->setBaudRate(QSerialPort::Baud38400);
        if(mParam.Baudrate=="115200")
            mSeriaPort->setBaudRate(QSerialPort::Baud115200);

        mSeriaPort->setParity(QSerialPort::NoParity);
        mSeriaPort->setDataBits(QSerialPort::Data8);
        bRes=mSeriaPort->open(QIODevice::ReadWrite);
        if(bRes)
        {

            emit errorString(QString("Connecté à %1").arg(mParam.PortSerie));
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

    if(mParam.typeConnexion==UDP)
    {
        if(mUdpSocket->bind(QHostAddress::Any,mParam.PortUDP))
        {
             emit errorString(QString("Connecté au port UDP %1").arg(mParam.PortUDP));
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
    if(mParam.typeConnexion==Serie)
        mSeriaPort->close();
    if(mParam.typeConnexion==UDP)
        mUdpSocket->close();
}

bool SensorDialog::isConnected()
{
    if(mParam.typeConnexion==Serie)
        return mSeriaPort->isOpen();

    if(mParam.typeConnexion==UDP)
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
    if(mParam.typeConnexion==Serie)
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
            emit errorString(QString("Message envoyé sur %1 : %2").arg(mParam.PortSerie).arg(sMessage));
            bRes=true;
        }


    }

    if(mParam.typeConnexion==UDP)
    {


        QByteArray datagram = sMessage.toLocal8Bit();

       qint64 result=mUdpSocket->writeDatagram(datagram.data(), datagram.size(),QHostAddress(mParam.ipAddress),mParam.PortUDP);

       if (result==-1)
       {
           bRes=false;
           emit errorString(QString("Message non envoyé : %1").arg(sMessage));
           emit errorString(mUdpSocket->errorString());

       }
       else
       {
           emit errorString(QString("Message envoyé sur le port %1 : %2").arg(mParam.PortUDP).arg(sMessage));
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

    if(mParam.typeConnexion==Serie)
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
            emit errorString(QString("Message envoyé sur %1 : %2").arg(mParam.PortSerie).arg(sMessage));
            bRes=true;
        }


    }

    if(mParam.typeConnexion==UDP)
    {

        QByteArray datagram = sMessage.toLocal8Bit();

       qint64 result=mUdpSocket->writeDatagram(datagram.data(), datagram.size(),QHostAddress("255.255.255.255"),mParam.PortUDP);

       if (result==-1)
       {
           bRes=false;
           emit errorString(QString("Message non envoyé : %1").arg(sMessage));
           emit errorString(mUdpSocket->errorString());

       }
       else
       {
           emit errorString(QString("Message envoyé sur le port %1 : %2").arg(mParam.PortUDP).arg(sMessage));
           bRes=true;

       }

    }

    return bRes;
}

void SensorDialog::readData()
{
    if(mParam.typeConnexion==Serie)
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

    if(mParam.typeConnexion==UDP)
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
    mParam.PortSerie=settings.value("PortName","").toString();
    mParam.Baudrate=settings.value("BaudRate",4800).toString();
    bool bType=settings.value("ConnecType",0).toBool();
    if(bType)
       mParam.typeConnexion=SensorDialog::UDP;
    else
        mParam.typeConnexion=SensorDialog::Serie;

    mParam.ipAddress=settings.value("IpSensor").toString();
    mParam.PortUDP=settings.value("PortUDP",50000).toInt();

}
