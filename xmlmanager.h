#ifndef XMLMANAGER_H
#define XMLMANAGER_H

#include <QObject>

#include <QtXml>
#include <QVector>

#include "graphe.h"

class XmlManager : public QObject
{
    Q_OBJECT
public:
    explicit XmlManager(QObject *parent = nullptr);

signals:
    void xmlError(QString sMsg);
public slots:
    void initXml(QString sPath);
    void addData(Graphe::stData uneData);
    QString currentPath();
    bool dayFileExist(QString sRepArchi, QDate dToday);
    QVector<Graphe::stData>* readDayFile(QString sRepArchi, QDate dToday);
    QVector<Graphe::stData>* readFile(QString sPathToFile);

    Graphe::stData dataAt(QString sRepArchi, QDateTime dtHeure);
    Graphe::stData dataLast(QString sRepArchi, QDate dToday);
     Graphe::stData dataFirst(QString sRepArchi,QDate dToday);

     QStringList listeFiles(QString sPath);


private:
    QString mPathToFile;
    QDomDocument* dom;



};

#endif // XMLMANAGER_H
