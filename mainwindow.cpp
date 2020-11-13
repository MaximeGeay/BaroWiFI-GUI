#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QSettings>
#include <QDebug>
#include <QDateTime>
#include <QMessageBox>

#define version "Barographe 1.0.3"
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setWindowTitle(version);
    fenPref=new fenConfig;
    wGrfPression=new Graphe;
    wGrfTemperature=new Graphe;
    wGrfHumidity=new Graphe;
    mSensor=new SensorDialog;
    mListeModelFiles=new QStandardItemModel;
    ui->gridLayoutPress->addWidget(wGrfPression);
    ui->gridLayoutTemp->addWidget(wGrfTemperature);
    ui->gridLayoutHumidity->addWidget(wGrfHumidity);
    mManagerXml=new XmlManager;
    mRestart=false;
    mNbLignesLogData=0;

    init();
    getXmlData(mNbFiles);

    QObject::connect(this->ui->actionPreferences1,SIGNAL(triggered()),this,SLOT(affichePref()));
    QObject::connect(this->ui->actionPreferences2,SIGNAL(triggered()),this,SLOT(affichePref()));
    QObject::connect(this->ui->actionConnecter1,SIGNAL(triggered()),this,SLOT(connecter()));
    QObject::connect(this->ui->actionConnecter2,SIGNAL(triggered()),this,SLOT(connecter()));
    QObject::connect(this->ui->actionDeconnecter1,SIGNAL(triggered()),this,SLOT(deconnecter()));
    QObject::connect(this->ui->actionDeconnecter2,SIGNAL(triggered()),this,SLOT(deconnecter()));
    QObject::connect(mSensor,&SensorDialog::dataReceived,this,&MainWindow::readData);
    QObject::connect(mSensor,&SensorDialog::errorString,this,&MainWindow::readError);
    QObject::connect(this->fenPref,&fenConfig::confChanged,this,&MainWindow::confChanged);
    QObject::connect(this->ui->comboEchelleTemps,&QComboBox::currentTextChanged,this,&MainWindow::selDuree);
    QObject::connect(this->ui->btn_Open,&QPushButton::clicked,this,&MainWindow::openFiles);
    QObject::connect(this->ui->btn_Remove,&QPushButton::clicked,this,&MainWindow::removeFiles);
    QObject::connect(ui->btn_ReadPressure,&QPushButton::clicked,this,&MainWindow::litPressionOffset);
    QObject::connect(ui->btn_WritePressure,&QPushButton::clicked,this,&MainWindow::ecritPressionOffset);
    QObject::connect(ui->spPressionRec,QOverload<double>::of(&QDoubleSpinBox::valueChanged),this,&MainWindow::findPression);
    QObject::connect(ui->btn_ReadTemperature,&QPushButton::clicked,this,&MainWindow::litTemperatureOffset);
    QObject::connect(ui->btn_WriteTemperature,&QPushButton::clicked,this,&MainWindow::ecritTemperatureOffset);
    QObject::connect(ui->spTempRec,QOverload<double>::of(&QDoubleSpinBox::valueChanged),this,&MainWindow::findTemperature);
    QObject::connect(ui->btn_ReadIpSensor,&QPushButton::clicked,this,&MainWindow::litIpCapteur);
    QObject::connect(ui->btn_ReadBroadcast,&QPushButton::clicked,this,&MainWindow::litBroadcastCapteur);
    QObject::connect(ui->btn_ReadUdpPort,&QPushButton::clicked,this,&MainWindow::litUdpPortCapteur);
    QObject::connect(ui->btn_WriteBroadcast,&QPushButton::clicked,this,&MainWindow::ecritBroadcastCapteur);
    QObject::connect(ui->btn_WriteUdpPort,&QPushButton::clicked,this,&MainWindow::ecritUdpPortCapteur);
    QObject::connect(ui->btn_reinit,&QPushButton::clicked,this,&MainWindow::reinitCapteur);
    QObject::connect(ui->btn_ReadPeriod,&QPushButton::clicked,this,&MainWindow::litPeriodeCapteur);
    QObject::connect(ui->btn_WritePeriod,&QPushButton::clicked,this,&MainWindow::ecritPeriodeCapteur);
    QObject::connect(ui->btn_RAZLog,&QPushButton::clicked,ui->te_Log,&QTextEdit::clear);
    QObject::connect(ui->btn_ReadSSID,&QPushButton::clicked,this,&MainWindow::litSSID);
    QObject::connect(ui->btn_WriteSSID,&QPushButton::clicked,this,&MainWindow::ecritSSID);
    QObject::connect(ui->btn_WiFiStatus,&QPushButton::clicked,this,&MainWindow::etatWiFi);
    QObject::connect(ui->btn_ReadTimeoutWiFi,&QPushButton::clicked,this,&MainWindow::litTimeoutWiFi);
    QObject::connect(ui->btn_WriteTimeoutWiFi,&QPushButton::clicked,this,&MainWindow::ecritTimeoutWiFi);
    QObject::connect(ui->btn_ReadBaudRate,&QPushButton::clicked,this,&MainWindow::litBaudrate);
    QObject::connect(ui->btn_WriteBaudRate,&QPushButton::clicked,this,&MainWindow::ecritBaudrate);
    QObject::connect(ui->btn_ReadMDA,&QPushButton::clicked,this,&MainWindow::litMDAStatus);
    QObject::connect(ui->btn_ReadUdpPortSec,&QPushButton::clicked,this,&MainWindow::litUdpPortSecCapteur);
    QObject::connect(ui->btn_WriteMDA,&QPushButton::clicked,this,&MainWindow::ecritMDAStatus);
    QObject::connect(ui->btn_WriteUdpPortSec,&QPushButton::clicked,this,&MainWindow::ecritUdpPortSecCapteur);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::connecter()
{
    //init();


    mSensor->setSensorType(mTypeConnec); 
    if (mSensor->setConnected())

    {

        ui->actionConnecter1->setEnabled(false);
        ui->actionConnecter2->setEnabled(false);
        ui->actionDeconnecter1->setEnabled(true);
        ui->actionDeconnecter2->setEnabled(true);
        QDateTime dateHeure=QDateTime::currentDateTimeUtc();
        QString sFile=QString("%1/Baro_%2.xml").arg(mRepArchi).arg(dateHeure.toString("yyyyMMdd"));
        mManagerXml->initXml(sFile);
        dateEnCours=dateHeure.date();
    }


}

void MainWindow::deconnecter()
{
    mSensor->setDisconnected();
    ui->actionConnecter1->setEnabled(true);
    ui->actionConnecter2->setEnabled(true);
    ui->actionDeconnecter1->setEnabled(false);
    ui->actionDeconnecter2->setEnabled(false);
    ui->statusBar->showMessage("Déconnecté",3000);


    if(mRestart)
    {
        this->connecter();
        mRestart=false;
    }


}

void MainWindow::affichePref()
{
    this->fenPref->showFen(mSensor->isConnected());
}

void MainWindow::readData(QString sTrame)
{
    ui->statusBar->showMessage(sTrame);
    if(mNbLignesLogData>5000)
        ui->te_Log->clear();

    QString sLog=QString("%1: %2").arg(QDateTime::currentDateTime().toString("hh:mm:ss")).arg(sTrame);
    ui->te_Log->append(sLog);
    mNbLignesLogData++;
    decodeTrame(sTrame);
}

void MainWindow::readError(QString sError)
{
    ui->statusBar->showMessage(sError,3000);
}

void MainWindow::confChanged()
{
    init();
    getXmlData(mNbFiles);
    mRestart=true;
    this->deconnecter();
}

void MainWindow::selDuree(QString sText)
{

    int nDuree=0;
    if(sText.contains("heure"))
    {
        nDuree=sText.section(" ",0,0).toInt();
    }
    if(sText.contains("jour"))
    {
        nDuree=sText.section(" ",0,0).toInt();
        nDuree=nDuree*24;

    }
    if(sText.contains("semaine"))
    {
        nDuree=sText.section(" ",0,0).toInt();
        nDuree=nDuree*168;

    }
    if(sText.contains("mois"))
    {
        nDuree=sText.section(" ",0,0).toInt();
        nDuree=nDuree*24*QDate::currentDate().daysInMonth();
    }
    if(sText.contains("Total"))
    {
        //à écrire
    }


    changeGrfDuree(nDuree);

}

void MainWindow::initListFile()
{

    mListeModelFiles->clear();
    QStringList lFiles=mManagerXml->listeFiles(mRepArchi);
    QString sUnFichier;
    QString sDate;
    int nIndex=0;


    QStringListIterator it(lFiles);
    while(it.hasNext())
    {

        sUnFichier=it.next();
        sDate=sUnFichier.section("/",-1).section("_",1,1).section(".",0,0);
        QDateTime dt=QDateTime::fromString(sDate,"yyyyMMdd");
        sDate=dt.toString("ddd dd MMM yyyy");

        QList<QStandardItem*> lUneLigne;
        lUneLigne.append(new QStandardItem(sDate));
        lUneLigne.append(new QStandardItem(sUnFichier));
        lUneLigne.append(new QStandardItem(QString("%1").arg(nIndex)));
        mListeModelFiles->appendRow(lUneLigne);

        nIndex++;

    }

    ui->listFile->setModel(mListeModelFiles);


}

void MainWindow::openFiles()
{
    QItemSelectionModel *select=ui->listFile->selectionModel();
    QModelIndexList listeSelect=select->selectedIndexes();
    if(!listeSelect.isEmpty())
    {
        Graphe* grfHistoPression=new Graphe;
        Graphe* grfHistoTemperature=new Graphe;
        Graphe* grfHistoHumidity=new Graphe;
        grfHistoPression->initGraphe(mDuree,Graphe::Pression);
        grfHistoTemperature->initGraphe(mDuree,Graphe::Temperature);
        grfHistoHumidity->initGraphe(mDuree,Graphe::Humidity);
        QVector<Graphe::stData>* tabDatas=new QVector<Graphe::stData>;

        for(int i=0;i<listeSelect.size();i++)
        {
            QString sPath=mListeModelFiles->item(listeSelect[i].row(),1)->text();

            tabDatas=mManagerXml->readFile(sPath);
            if(!tabDatas->isEmpty())
            {
                for(int n=0;n<tabDatas->size();n++)
                {
                    grfHistoPression->ajoutData(tabDatas->at(n));
                    grfHistoTemperature->ajoutData(tabDatas->at(n));
                    grfHistoHumidity->ajoutData(tabDatas->at(n));
                }


            }

        }

        grfHistoPression->setX(grfHistoPression->debutGraphe().first,grfHistoPression->finGraphe().first);
        grfHistoTemperature->setX(grfHistoTemperature->debutGraphe().first,grfHistoTemperature->finGraphe().first);
        grfHistoHumidity->setX(grfHistoHumidity->debutGraphe().first,grfHistoHumidity->finGraphe().first);

        grfHistoPression->show();
        grfHistoTemperature->show();
        grfHistoHumidity->show();
    }
}

void MainWindow::removeFiles()
{
    QItemSelectionModel *select=ui->listFile->selectionModel();
    QModelIndexList listeSelect=select->selectedIndexes();
    QStringList listeFichiers;
    if(!listeSelect.isEmpty())
    {


        for(int i=0;i<listeSelect.size();i++)
        {
            QString sPath=mListeModelFiles->item(listeSelect[i].row(),1)->text();

            listeFichiers<<sPath;

        }

        if(!listeFichiers.isEmpty())
        {
            QString sMesg;
            QStringListIterator it (listeFichiers);
            while(it.hasNext())
            {
                sMesg="\r\n"+it.next();
            }

            int rep=QMessageBox::question(this,"Suppression de fichiers","Etes-vous sûr de vouloir supprimer définitivement les fichiers suivants:"+sMesg,QMessageBox::Yes,QMessageBox::No);
            if(rep==QMessageBox::Yes)
            {
                QStringListIterator its (listeFichiers);
                while(its.hasNext())
                {

                    QFile::remove(its.next());
                }

                initListFile();
            }


        }
    }
}

void MainWindow::xmlError(QString sMsg)
{

    ui->statusBar->showMessage(sMsg);
}

void MainWindow::litPressionOffset()
{
    QString sMsg=QString("$BARO,getPressureOffset,\n");
    mSensor->sendMessage(sMsg);
}

void MainWindow::ecritPressionOffset()
{
    double dOffset=ui->spPression->value()*100;
    QString sMsg=QString("$BARO,setPressureOffset,%1,\n").arg(QString::number(dOffset,'f',0));
    mSensor->sendMessage(sMsg);
}

void MainWindow::litTemperatureOffset()
{
    QString sMsg=QString("$BARO,getTemperatureOffset,\n");
    mSensor->sendMessage(sMsg);
}

void MainWindow::ecritTemperatureOffset()
{
    double dOffset=ui->spTemperature->value()*10;
    QString sMsg=QString("$BARO,setTemperatureOffset,%1,\n").arg(QString::number(dOffset,'f',0));
    mSensor->sendMessage(sMsg);
}

void MainWindow::litIpCapteur()
{
    QString sMsg=QString("$BARO,getIpAddress,\n");
    mSensor->broadcastMessage(sMsg);
}

void MainWindow::litBroadcastCapteur()
{
    QString sMsg=QString("$BARO,getBroadcastAddress,\n");
    mSensor->sendMessage(sMsg);
}

void MainWindow::ecritBroadcastCapteur()
{
    QString sMsg=QString("$BARO,setBroadcastAddress,%1,\n").arg(ui->le_broadcast->text());
    mSensor->sendMessage(sMsg);
}

void MainWindow::litUdpPortCapteur()
{
    QString sMsg=QString("$BARO,getUdpPort,\n");
    mSensor->sendMessage(sMsg);
}

void MainWindow::ecritUdpPortCapteur()
{
    QString sMsg=QString("$BARO,setUdpPort,%1,\n").arg(ui->sp_UdpPort->value());
    mSensor->sendMessage(sMsg);
}

void MainWindow::litUdpPortSecCapteur()
{
    QString sMsg=QString("$BARO,getUdpPortSec,\n");
    mSensor->sendMessage(sMsg);
}

void MainWindow::ecritUdpPortSecCapteur()
{
    QString sMsg=QString("$BARO,setUdpPortSec,%1,\n").arg(ui->sp_UdpPortSec->value());
    mSensor->sendMessage(sMsg);
}

void MainWindow::litMDAStatus()
{
    QString sMsg=QString("$BARO,getMDAStatus,\n");
    mSensor->sendMessage(sMsg);
}

void MainWindow::ecritMDAStatus()
{
    QString sMsg=QString("$BARO,setMDAStatus,%1,\n").arg(ui->cb_MDAStatus->isChecked());
    mSensor->sendMessage(sMsg);

}

void MainWindow::litPeriodeCapteur()
{
    QString sMsg=QString("$BARO,getPeriod,\n");
    mSensor->sendMessage(sMsg);
}

void MainWindow::ecritPeriodeCapteur()
{
    double dPeriod=ui->sp_Periode->value()*1000;
    QString sMsg=QString("$BARO,setPeriod,%1,\n").arg(QString::number(dPeriod,'f',0));
    mSensor->sendMessage(sMsg);

}

void MainWindow::litTimeoutWiFi()
{
    QString sMsg=QString("$BARO,getTimeoutWiFi,\n");
    mSensor->sendMessage(sMsg);
}

void MainWindow::ecritTimeoutWiFi()
{
    int nTimeout=ui->sp_TimeoutWiFi->value()*1000;
    QString sMsg=QString("$BARO,setTimeoutWiFi,%1,\n").arg(nTimeout);
    mSensor->sendMessage(sMsg);
}

void MainWindow::litBaudrate()
{
    QString sMsg=QString("$BARO,getBaudRate,\n");
    mSensor->sendMessage(sMsg);
}

void MainWindow::ecritBaudrate()
{

    QString sQuestion,sMsg;
    if(mSensor->getSensorType()==SensorDialog::Serie)
        sQuestion="Etes-vous sûr de vouloir modifier la vitesse de communication Série?\nVous allez perdre la communication, pensez à modifier également ce paramètre dans les préfèrences de l'application.";
    else
        sQuestion="Etes-vous sûr de vouloir modifier la vitesse de communication Série?";

    if(QMessageBox::question(this,QString("Modification de la vitesse de communication Série"),sQuestion)==QMessageBox::Yes)
    {

    QString sBaudrate=ui->cb_Baudrate->currentText();
    QString sMsg=QString("$BARO,setBaudRate,%1,\n").arg(sBaudrate);
    mSensor->sendMessage(sMsg);
    }
}

void MainWindow::etatWiFi()
{
    QString sMsg=QString("$BARO,getWiFiStatus,\n");
    mSensor->sendMessage(sMsg);
}

void MainWindow::litSSID()
{
    QString sMsg=QString("$BARO,getSSID,\n");
    mSensor->sendMessage(sMsg);
}

void MainWindow::ecritSSID()
{
    QString sQuestion,sMsg;
    if(mSensor->getSensorType()==SensorDialog::UDP)
        sQuestion="Etes-vous sûr de vouloir modifier le nom du réseau WiFi sur lequel se connecte le capteur?\nVous risquez de perdre la communication";
    else
        sQuestion="Etes-vous sûr de vouloir modifier le nom du réseau WiFi sur lequel se connecte le capteur?";

    if(QMessageBox::question(this,QString("Modification de la connexion WiFi"),sQuestion)==QMessageBox::Yes)
    {
        sMsg=QString("$BARO,setSSID,%1,\n").arg(ui->le_SSID->text());
        mSensor->sendMessage(sMsg);

        bool ok;
        QString text = QInputDialog::getText(this, tr("Mot de passe WiFi"),
                                                   tr("Mot de passe:"), QLineEdit::Normal,
                                                   "", &ok);
              if (ok)
              {
                  if(text.isEmpty())
                      sMsg=QString("$BARO,resetPassword,\n");
                  else
                        sMsg=QString("$BARO,setPassword,%1,\n").arg(text);
                  mSensor->sendMessage(sMsg);
              }

              if(QMessageBox::question(this,"Connexion au nouveau réseau",QString("Souhaitez-vous procéder à la connexion du capteur au réseau %1").arg(ui->le_SSID->text()))==QMessageBox::Yes)
              {
                  sMsg=QString("$BARO,setWiFiConnect,\n");
                   mSensor->sendMessage(sMsg);
              }
    }


}

void MainWindow::reinitCapteur()
{
    if(QMessageBox::question(this,QString("Réinitialisation du capteur"),"Etes-vous sûr de vouloir réinitialiser le capteur?\nUne connexion serie sera nécessaire pour paramétrer la connexion WiFi")==QMessageBox::Yes)
    {
        QString sMsg=QString("$BARO,init,\n");
        mSensor->sendMessage(sMsg);
    }
}

void MainWindow::findPression(double dPress)
{
    if(mSensor->isConnected())
    {
    double dPressionBrute=lastMesure.pression-mOffsetPression;
    qDebug()<<"PressionBrute"<<dPressionBrute;
    double dOffset=dPress-dPressionBrute;
    ui->spPression->setValue(dOffset);


    }
}

void MainWindow::findTemperature(double dTemp)
{
    if(mSensor->isConnected())
    {
    double dTempBrute=lastMesure.temperature-mOffsetTemperature;
    double dOffset=dTemp-dTempBrute;
    ui->spTemperature->setValue(dOffset);
    }
}



void MainWindow::decodeTrame(QString sTrame)
{
    //Graphe::stData uneMesure;

    QDate currentDate=QDateTime::currentDateTimeUtc().date();
    if(dateEnCours.day()!=currentDate.day())
    {
        //pourchangement de fichier;
        mRestart=true;
        deconnecter();
        return;
    }
    qDebug()<<sTrame;

    //"$IIXDR,P,1.01080,B,Barometer,C,26.90,C,Temperature,H,46.36,P,Humidity*48\r\n"
    if(sTrame.section(",",0,0)=="$IIXDR"&&sTrame.contains("B")&&sTrame.contains("C")&&sTrame.contains("H"))
   // if(sTrame.section(",",0,0)=="$IIXDR")
    {
        mTrameCount++;

        lastMesure.pression=sTrame.section(",",2,2).toDouble()*1000;
        lastMesure.temperature=sTrame.section(",",6,6).toDouble();
        lastMesure.humidity=sTrame.section(",",10,10).toDouble();

        lastMesure.temperature=lastMesure.temperature;
        lastMesure.pression=lastMesure.pression;
        lastMesure.humidity=lastMesure.humidity;
        lastMesure.dateHeure=QDateTime::currentDateTimeUtc();

        QString sPression=QString::number(lastMesure.pression,'f',1);
        QString sTemperature=QString::number(lastMesure.temperature,'f',1);
        QString sHumidity=QString::number(lastMesure.humidity,'f',1);
        QString sDateHeure=lastMesure.dateHeure.toString("ddd dd MMM yyyy hh:mm:ss");
        ui->l_Pression->setText(QString("Pression: %1 HPa").arg(sPression));
        ui->l_Temperature->setText(QString("Temperature: %1 °C").arg(sTemperature));
        ui->l_Humidity->setText(QString("Humidité: %1 %").arg(sHumidity));
        ui->l_Heure->setText(QString("Dernière mesure : %1 UTC").arg(sDateHeure));

        wGrfPression->ajoutData(lastMesure);
        wGrfTemperature->ajoutData(lastMesure);
        wGrfHumidity->ajoutData(lastMesure);
        graphDecale();


        if(mRatio==mTrameCount)
        {
            mManagerXml->addData(lastMesure);
            mTrameCount=0;
        }

    }

    if(sTrame.section(",",0,0)=="$BARO")
    {
        if(sTrame.section(",",1,1)=="PressureOffset")
        {
            double dOffset=sTrame.section(",",2,2).toDouble();
                    dOffset=dOffset/100;
                    qDebug()<<"Offset"<<dOffset<<sTrame.section(",",2,2);
            ui->spPression->setValue(dOffset);
            mOffsetPression=dOffset;
        }

        if(sTrame.section(",",1,1)=="TemperatureOffset")
        {
            double dOffset=sTrame.section(",",2,2).toDouble();
                    dOffset=dOffset/10;
                    qDebug()<<"Offset"<<dOffset<<sTrame.section(",",2,2);
            ui->spTemperature->setValue(dOffset);
            mOffsetTemperature=dOffset;
        }

        if(sTrame.section(",",1,1)=="IpAddress")
        {
            ui->le_IpSensor->setText(sTrame.section(",",2,2));
        }
        if(sTrame.section(",",1,1)=="BroadcastAddress")
        {
            ui->le_broadcast->setText(sTrame.section(",",2,2));
        }
        if(sTrame.section(",",1,1)=="UdpPort")
        {
            ui->sp_UdpPort->setValue(sTrame.section(",",2,2).toInt());
        }
        if(sTrame.section(",",1,1)=="UdpPortSec")
        {
            ui->sp_UdpPortSec->setValue(sTrame.section(",",2,2).toInt());
        }
        if(sTrame.section(",",1,1)=="MDAStatus")
        {
            bool bStatus=sTrame.section(",",2,2).toInt();
            ui->cb_MDAStatus->setChecked(bStatus);

        }
        if(sTrame.section(",",1,1)=="period")
        {
            double dPeriod=sTrame.section(",",2,2).toDouble();
                   dPeriod=dPeriod/1000;
            ui->sp_Periode->setValue(dPeriod);
        }
        if(sTrame.section(",",1,1)=="WiFiConnected")
        {
            if(sTrame.section(",",2,2)=="true")
            {
                ui->le_WiFiStatus->setText("Capteur connecté");
            }
            else
            {
                ui->le_WiFiStatus->setText("Capteur non connecté");
            }
        }

        if(sTrame.section(",",1,1)=="SSID")
        {
            ui->le_SSID->setText(sTrame.section(",",2,2));
        }
        if(sTrame.section(",",1,1)=="timeoutWiFi")
        {
            int nTimeout=sTrame.section(",",2,2).toInt();
                   nTimeout=nTimeout/1000;
            ui->sp_TimeoutWiFi->setValue(nTimeout);
        }
        if(sTrame.section(",",1,1)=="baudRate")
                {
                    QString sBaudRate=sTrame.section(",",2,2);

                    ui->cb_Baudrate->setCurrentIndex(ui->cb_Baudrate->findText(sBaudRate));
                }





    }


}

void MainWindow::init()
{
    QSettings settings ("Barographe","BaroConfig");
    mPortName=settings.value("PortName","").toString();
    bool bType=settings.value("ConnecType",0).toBool();
    if(bType)
        mTypeConnec=SensorDialog::UDP;
    else
        mTypeConnec=SensorDialog::Serie;
    mRepArchi=settings.value("RepArchi",QStandardPaths::locate(QStandardPaths::HomeLocation,QString(),QStandardPaths::LocateDirectory)+"Barographe_Datas").toString();
    mDuree=settings.value("Duree",3).toInt();

    mOffsetPression=settings.value("OffsetPression","0").toDouble();
    mOffsetTemperature=settings.value("OffsetTemperature","0").toDouble();
    mNbFiles=settings.value("NbFiles",1).toInt();


    mRatio=settings.value("Ratio",1).toInt();
    mTrameCount=0;

    ui->comboEchelleTemps->setCurrentIndex(settings.value("IndexDuree",0).toInt());

    wGrfPression->initGraphe(mDuree,Graphe::Type::Pression);
    wGrfTemperature->initGraphe(mDuree,Graphe::Type::Temperature);
    wGrfHumidity->initGraphe(mDuree,Graphe::Type::Humidity);
    changeGrfDuree(mDuree);

    initListFile();

}

void MainWindow::changeGrfDuree(int nDuree)
{
    if(nDuree>0)
    {
        mDuree=nDuree;
        QSettings settings ("Barographe","BaroConfig");
        settings.setValue("Duree",mDuree);
        settings.setValue("IndexDuree",ui->comboEchelleTemps->currentIndex());
        graphDecale();
        wGrfPression->replot();
        wGrfTemperature->replot();
        wGrfHumidity->replot();


    }


}

void MainWindow::graphDecale()
{
    qint64 dureeS=mDuree*3600;
    QDateTime dtNow=QDateTime::currentDateTimeUtc();
    QDateTime dtFin=dtNow.addSecs(dureeS);


    QDateTime dtDebutGrf=wGrfPression->debutGraphe().first;
    QDateTime dtFinGrf=wGrfPression->finGraphe().first;


    qint64 nDureeGrf=dtDebutGrf.secsTo(dtFinGrf);
    qDebug()<<"Sensor connected"<<mSensor->isConnected();
    if(!mSensor->isConnected())
    {
        if(dureeS>nDureeGrf)
        {

            wGrfPression->setX(dtDebutGrf,dtFinGrf);
            wGrfTemperature->setX(dtDebutGrf,dtFinGrf);
            wGrfHumidity->setX(dtDebutGrf,dtFinGrf);
            afficheTendance(calculeTendance(dtDebutGrf,dtFinGrf));

        }
        else
        {
            QDateTime dtDebut=dtFinGrf.addSecs(-dureeS);
            wGrfPression->setX(dtDebut,dtFinGrf);
            wGrfTemperature->setX(dtDebut,dtFinGrf);
            wGrfHumidity->setX(dtDebut,dtFinGrf);
            afficheTendance(calculeTendance(dtDebut,dtFinGrf));

        }
    }
    else {


        if(dureeS>nDureeGrf)
        {
            wGrfPression->setX(dtDebutGrf,dtFin);
            wGrfTemperature->setX(dtDebutGrf,dtFin);
            wGrfHumidity->setX(dtDebutGrf,dtFin);
            afficheTendance(calculeTendance(dtDebutGrf,dtFin));
        }
        else
        {
            dtDebutGrf=dtNow.addSecs(-dureeS);
            wGrfPression->setX(dtDebutGrf,dtNow);
            wGrfTemperature->setX(dtDebutGrf,dtNow);
            wGrfHumidity->setX(dtDebutGrf,dtNow);
            afficheTendance(calculeTendance(dtDebutGrf,dtNow));

        }
    }


}

void MainWindow::getXmlData(int nbFiles)
{
    QVector<Graphe::stData>* tabDatas=new QVector<Graphe::stData>;

    QDate dateToFind;
    for (int i = nbFiles-1; i >= 0; i--)
    {
        dateToFind=QDate::currentDate().addDays(-i);


        if(mManagerXml->dayFileExist(mRepArchi,dateToFind))
        {
            tabDatas=mManagerXml->readDayFile(mRepArchi,dateToFind);
        }

        if(!tabDatas->isEmpty())
        {
            for(int n=0;n<tabDatas->size();n++)
            {
                wGrfPression->ajoutData(tabDatas->at(n));
                wGrfTemperature->ajoutData(tabDatas->at(n));
                wGrfHumidity->ajoutData(tabDatas->at(n));
            }
            graphDecale();

        }
    }
}

double MainWindow::calculeTendance(QDateTime dtDebut,QDateTime dtFin)
{
    Graphe::stData mesureDebut,mesureFin;

    qint64 deltaNow=dtFin.secsTo(QDateTime::currentDateTimeUtc());

    if(deltaNow==0)
        mesureFin=mManagerXml->dataLast(mRepArchi,dtFin.date());
    else
        mesureFin=mManagerXml->dataAt(mRepArchi,dtFin);

    mesureDebut=mManagerXml->dataAt(mRepArchi,dtDebut);

    if(!mesureDebut.dateHeure.isValid())
        mesureDebut=mManagerXml->dataFirst(mRepArchi,dtDebut.date());

    if(!mesureFin.dateHeure.isValid())
        mesureFin=mManagerXml->dataLast(mRepArchi,dtFin.date());


    ;
    double dTendance=mesureFin.pression-mesureDebut.pression;

    return dTendance;
}

void MainWindow::afficheTendance(double dTendance)
{
    QString sMsg;
    if(dTendance>100 || dTendance<-100)
    {
        sMsg="Tendance non calculée";
    }
    else {
        sMsg=QString("Tendance: %1").arg(QString::number(dTendance,'f',1));
    }
    ui->l_Tendance->setText(sMsg);
}
