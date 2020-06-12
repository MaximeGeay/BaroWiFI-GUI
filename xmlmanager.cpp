#include "xmlmanager.h"

#include<QXmlStreamWriter>
#include <QMessageBox>
#include <QDateTime>

XmlManager::XmlManager(QObject *parent) : QObject(parent)
{

}

void XmlManager::initXml(QString sPath)
{

    dom=new QDomDocument("DomXML");
    QFile xmlDoc(sPath);
    if(xmlDoc.exists())
    {

        if(!xmlDoc.open(QIODevice::ReadOnly))
        {
            emit xmlError("Le document XML n'a pas pu être ouvert.");



                    return;
        }
        if (!dom->setContent(&xmlDoc))
            {
                 xmlDoc.close();
                 emit xmlError("Le document XML n'a pas pu être attribué à l'objet QDomDocument.");
                 return;
            }

        xmlDoc.close();
        mPathToFile=sPath;
    }
    else {


       if(!xmlDoc.open(QIODevice::WriteOnly))
       {
           emit xmlError("Le document XML n'a pas pu être créé.");
       }
        QXmlStreamWriter writer(&xmlDoc);
        QString uri="http://cqt.barographe.fr/Datas";
        writer.setAutoFormatting(true);
        writer.writeStartDocument();
        writer.writeStartElement(uri,tr("Datas"));
        writer.writeEndElement();
        writer.writeEndDocument();

         xmlDoc.close();

         initXml(sPath);


    }



}

void XmlManager::addData(Graphe::stData uneData)
{
        QDomElement docElem = dom->documentElement();
        QDomElement write_elem=dom->createElement("Mesure");
        write_elem.setAttribute("DateHeure",uneData.dateHeure.toString("dd/MM/yyyy hh:mm:ss"));
        write_elem.setAttribute("Pression",QString::number(uneData.pression,'f',1));
        write_elem.setAttribute("Temperature",QString::number(uneData.temperature,'f',1));
        write_elem.setAttribute("Humidity",QString::number(uneData.humidity,'f',1));
        docElem.appendChild(write_elem);

        QString write_doc=dom->toString();

        QFile fichier(mPathToFile);
        if(!fichier.open(QIODevice::WriteOnly))
        {
            fichier.close();
            emit xmlError( "Impossible d'écrire dans le document XML");
            return;
        }

        QTextStream stream(&fichier);

        stream<<write_doc;

        fichier.close();

}

QString XmlManager::currentPath()
{
    return mPathToFile;

}

bool XmlManager::dayFileExist(QString sRepArchi,QDate dToday)
{
    QString sFile=QString("%1/Baro_%2.xml").arg(sRepArchi).arg(dToday.toString("yyyyMMdd"));

    QFile xmlDoc(sFile);
    if(xmlDoc.exists())
        return true;
    else
        return false;


}

QVector<Graphe::stData>* XmlManager::readDayFile(QString sRepArchi, QDate dToday)
{
    Graphe::stData uneMesure;
    QVector<Graphe::stData>* vTabData=new QVector<Graphe::stData>;
   QDomDocument xmlDom;
   QString xmlDoc=QString("%1/Baro_%2.xml").arg(sRepArchi).arg(dToday.toString("yyyyMMdd"));
   QFile f(xmlDoc);
   if(!f.open(QIODevice::ReadOnly))
   {
       emit xmlError("Le document XML n'a pas pu être ouvert.");

   }

   xmlDom.setContent(&f);
   f.close();

   QDomElement root=xmlDom.documentElement();
   QDomElement Component=root.firstChild().toElement();
   while(!Component.isNull())
   {
       // <Mesure Temperature="23.6" Pression="1016.7" DateHeure="07/02/2019 13:06:46"/>
       if(Component.tagName()=="Mesure")
       {
       uneMesure.pression=Component.attribute("Pression").toDouble();
       uneMesure.temperature=Component.attribute("Temperature").toDouble();
       uneMesure.humidity=Component.attribute("Humidity").toDouble();
       uneMesure.dateHeure=QDateTime::fromString(Component.attribute("DateHeure"),"dd/MM/yyyy hh:mm:ss");
       uneMesure.dateHeure.setTimeZone(QTimeZone::utc());
       vTabData->append(uneMesure);

       }
       Component=Component.nextSibling().toElement();
   }

   return vTabData;



}

QVector<Graphe::stData> *XmlManager::readFile(QString sPathToFile)
{
    Graphe::stData uneMesure;
    QVector<Graphe::stData>* vTabData=new QVector<Graphe::stData>;
   QDomDocument xmlDom;
   QString xmlDoc=QString("%1").arg(sPathToFile);
   QFile f(xmlDoc);
   if(!f.open(QIODevice::ReadOnly))
   {
       emit xmlError("Le document XML n'a pas pu être ouvert.");

   }

   xmlDom.setContent(&f);
   f.close();

   QDomElement root=xmlDom.documentElement();
   QDomElement Component=root.firstChild().toElement();
   while(!Component.isNull())
   {
       // <Mesure Temperature="23.6" Pression="1016.7" DateHeure="07/02/2019 13:06:46"/>
       if(Component.tagName()=="Mesure")
       {
       uneMesure.pression=Component.attribute("Pression").toDouble();
       uneMesure.temperature=Component.attribute("Temperature").toDouble();
       uneMesure.humidity=Component.attribute("Humidity").toDouble();
       uneMesure.dateHeure=QDateTime::fromString(Component.attribute("DateHeure"),"dd/MM/yyyy hh:mm:ss");
       uneMesure.dateHeure.setTimeZone(QTimeZone::utc());
       vTabData->append(uneMesure);

       }
       Component=Component.nextSibling().toElement();
   }

   return vTabData;
}

Graphe::stData XmlManager::dataAt(QString sRepArchi,QDateTime dtHeure)
{
    Graphe::stData uneMesure, mesurePrec, notFound;

    dtHeure.setTimeZone(QTimeZone::utc());
    QDate dDate=dtHeure.date();
    QDomDocument xmlDom;
    QDateTime dtCourante;
    bool bFound=false;

    qint64 deltaT=0;
    qint64 deltaTPrec=0;

    QString xmlDoc=QString("%1/Baro_%2.xml").arg(sRepArchi).arg(dDate.toString("yyyyMMdd"));
    QFile f(xmlDoc);
    if(!f.open(QIODevice::ReadOnly))
    {
        emit xmlError("Le document XML n'a pas pu être ouvert.");

    }

    xmlDom.setContent(&f);
    f.close();

    QDomElement root=xmlDom.documentElement();
    QDomElement Component=root.firstChild().toElement();
    while(!Component.isNull())
    {
        // <Mesure Temperature="23.6" Pression="1016.7" DateHeure="07/02/2019 13:06:46"/>
        if(Component.tagName()=="Mesure")
        {
        uneMesure.pression=Component.attribute("Pression").toDouble();
        uneMesure.temperature=Component.attribute("Temperature").toDouble();
        uneMesure.humidity=Component.attribute("Humidity").toDouble();
        uneMesure.dateHeure=QDateTime::fromString(Component.attribute("DateHeure"),"dd/MM/yyyy hh:mm:ss");
        uneMesure.dateHeure.setTimeZone(QTimeZone::utc());
        deltaT=uneMesure.dateHeure.secsTo(dtHeure);
        if(deltaT<0)
            deltaT=-deltaT;

        if(mesurePrec.dateHeure.isValid())
        {
                if(deltaTPrec<deltaT || deltaT==0)
                {
                    bFound=true;
                    break;
                }
        }

        mesurePrec=uneMesure;
        deltaTPrec=deltaT;


        }

        Component=Component.nextSibling().toElement();
    }

    if(bFound)

    {
        return mesurePrec;
    }
    else {
        return notFound;
    }



}

Graphe::stData XmlManager::dataLast(QString sRepArchi,QDate dToday)
{
    Graphe::stData uneMesure;

   QDomDocument xmlDom;
   QString xmlDoc=QString("%1/Baro_%2.xml").arg(sRepArchi).arg(dToday.toString("yyyyMMdd"));
   QFile f(xmlDoc);
   if(!f.open(QIODevice::ReadOnly))
   {
       emit xmlError("Le document XML n'a pas pu être ouvert.");

   }

   xmlDom.setContent(&f);
   f.close();

   QDomElement root=xmlDom.documentElement();
   QDomElement Component=root.firstChild().toElement();
   while(!Component.isNull())
   {
       // <Mesure Temperature="23.6" Pression="1016.7" DateHeure="07/02/2019 13:06:46"/>
       if(Component.tagName()=="Mesure")
       {
       uneMesure.pression=Component.attribute("Pression").toDouble();
       uneMesure.temperature=Component.attribute("Temperature").toDouble();
       uneMesure.humidity=Component.attribute("Humidity").toDouble();
       uneMesure.dateHeure=QDateTime::fromString(Component.attribute("DateHeure"),"dd/MM/yyyy hh:mm:ss");


       }
       Component=Component.nextSibling().toElement();
   }

   return uneMesure;
}

Graphe::stData XmlManager::dataFirst(QString sRepArchi, QDate dToday)
{
    Graphe::stData uneMesure;

   QDomDocument xmlDom;
   QString xmlDoc=QString("%1/Baro_%2.xml").arg(sRepArchi).arg(dToday.toString("yyyyMMdd"));
   QFile f(xmlDoc);
   if(!f.open(QIODevice::ReadOnly))
   {
       emit xmlError("Le document XML n'a pas pu être ouvert.");

   }

   xmlDom.setContent(&f);
   f.close();

   QDomElement root=xmlDom.documentElement();
   QDomElement Component=root.firstChild().toElement();
   while(!Component.isNull())
   {
       // <Mesure Temperature="23.6" Pression="1016.7" DateHeure="07/02/2019 13:06:46"/>
       if(Component.tagName()=="Mesure")
       {
       uneMesure.pression=Component.attribute("Pression").toDouble();
       uneMesure.temperature=Component.attribute("Temperature").toDouble();
       uneMesure.humidity=Component.attribute("Humidity").toDouble();
       uneMesure.dateHeure=QDateTime::fromString(Component.attribute("DateHeure"),"dd/MM/yyyy hh:mm:ss");

        break;
       }
       Component=Component.nextSibling().toElement();
   }

   return uneMesure;
}

QStringList XmlManager::listeFiles(QString sPath)
{
    QStringList lFilter,lFiles;
    lFilter <<"*.xml";
    QDirIterator it(sPath,lFilter,QDir::Files | QDir::NoSymLinks);
    while(it.hasNext())
        lFiles<<it.next();

    lFiles.sort();
    return lFiles;
}
