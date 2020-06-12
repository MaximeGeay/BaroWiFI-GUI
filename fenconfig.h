#ifndef FENCONFIG_H
#define FENCONFIG_H

#include <QWidget>
#include "sensordialog.h"


namespace Ui {
class fenConfig;
}



class fenConfig : public QWidget
{
    Q_OBJECT

public:
    explicit fenConfig(QWidget *parent = nullptr);
    ~fenConfig();



signals:

    void confChanged();
public slots:


    void showFen(bool bIsRunning);


private slots:
    void valider();

    void majInfo();
    void selRepArchi();
    void majConnecType();

private:
    Ui::fenConfig *ui;
    QString mPortName;
    QString mBaudrate;
    bool mConnec;
    bool bConfChanged;
    QString mIpSensor;
    int mPortUDP;


    int mNbFiles;
    QString mRepArchi;

    bool mIsRunning;
    bool initRepArchi(QString sRep);
    void initFen();
    void selConnecType(SensorDialog::ConnexionType);
    int mRatio;

};

#endif // FENCONFIG_H
