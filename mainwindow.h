#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSerialPort>
#include <QDateTime>
#include <QVector>
#include <QStandardItemModel>

#include "fenconfig.h"
#include "graphe.h"
#include "xmlmanager.h"
#include "sensordialog.h"

namespace Ui {
class MainWindow;
}


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();


private slots:
    void connecter(void);
    void deconnecter(void);
    void affichePref();
    void readData(QString sSource,QString sTrame);
    void readError(QString sError);
    void confChanged();
    void selDuree(QString sText);
    void initListFile();
    void openFiles();
    void removeFiles();
    void xmlError(QString sMsg);
    void litPressionOffset();
    void ecritPressionOffset();
    void litTemperatureOffset();
    void ecritTemperatureOffset();    
    void litIpCapteur();
    void litBroadcastCapteur();
    void ecritBroadcastCapteur();
    void litUdpPortCapteur();
    void ecritUdpPortCapteur();
    void litUdpPortSecCapteur();
    void ecritUdpPortSecCapteur();
    void litMDAStatus();
    void ecritMDAStatus();
    void litVaisalaStatus();
    void ecritVaisalaStatus();
    void litPeriodeCapteur();
    void ecritPeriodeCapteur();
    void litTimeoutWiFi();
    void ecritTimeoutWiFi();
    void litBaudrate();
    void ecritBaudrate();
    void etatWiFi();
    void litSSID();
    void ecritSSID();
    void reinitCapteur();

    void litIpDisplay();
    void litBroadcastDisplay();
    void ecritBroadcastDisplay();
    void litUdpPortDisplay();
    void ecritUdpPortDisplay();
    void litUdpPortSecDisplay();
    void ecritUdpPortSecDisplay();
    void litTimeoutWiFiDisplay();
    void ecritTimeoutWiFiDisplay();
    void shutdownDisplay();
    void displayChangeScale();
    void displayGetScale();
    void refreshDisplay();
    void litPeriodeDisplay();
    void ecritPeriodeDisplay();
    void litRecordRatio();
    void ecritRecordRatio();
    void litTypeGrf();
    void ecritTypeGrf();

    void findPression(double dPress);
    void findTemperature(double dTemp);



private:
    Ui::MainWindow *ui;

    SensorDialog* mSensor;
    SensorDialog* mDisplay;
    //SensorDialog::ConnexionType mTypeConnec;
   // QString mPortName;
    QString mTrameEnCours;
    QString mTrameEntiere;
    int mDuree; //heure
    fenConfig* fenPref;
    Graphe* wGrfPression;
    Graphe* wGrfTemperature;
    Graphe* wGrfHumidity;

    double mOffsetPression;
    double mOffsetTemperature;
    int mNbLignesLogData;


    XmlManager* mManagerXml;
    void decodeTrame(QString sTrame);
    void init();
    void changeGrfDuree(int nDuree);
    void graphDecale();
    void getXmlData(int nbFiles);
    double calculeTendance(QDateTime dtDebut, QDateTime dtFin);
    void afficheTendance(double dTendance);

    bool mRestart;
    QStandardItemModel *mListeModelFiles;
    QString mRepArchi;

    int mRatio;
    int mTrameCount;
    int mNbFiles;
    QDate dateEnCours;
    Graphe::stData lastMesure;
};

#endif // MAINWINDOW_H
