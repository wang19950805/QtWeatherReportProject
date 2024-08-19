#include "citycodeutils.h"

#include <QByteArray>
#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonValue>

CityCodeUtils::CityCodeUtils()
{

}

QString CityCodeUtils::getCityCodeFromName(QString cityName)
{
    if(cityCodeMap.isEmpty()){
         initCityCodeMap();
    }
    QMap<QString,QString>::Iterator it = cityCodeMap.find(cityName);
    if(it == cityCodeMap.end()){
         it = cityCodeMap.find(cityName + "市");
    }
    if(it == cityCodeMap.end()){
          it = cityCodeMap.find(cityName + "县");
    }
    if(it == cityCodeMap.end()){
          it = cityCodeMap.find(cityName + "区");
    }
    if(it == cityCodeMap.end()){
          return "";
    }
    return it.value();
}

void CityCodeUtils::initCityCodeMap()
{
    QFile file(":/citycode.josn");
    file.open(QIODevice::ReadOnly);
    QByteArray citycodeData = file.readAll();
    file.close();

    QJsonDocument josnDoc = QJsonDocument::fromJson(citycodeData);
    QJsonArray cityArr = josnDoc.array();
    for(QJsonValue value : cityArr){
        if(value.isObject()){
            QString strName = value["city_name"].toString();
            QString strCode  = value["city_code"].toString();
            cityCodeMap.insert(strName,strCode);

        }
    }
}
