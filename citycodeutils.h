#ifndef CITYCODEUTILS_H
#define CITYCODEUTILS_H

#include <QMap>
#include <QString>



class CityCodeUtils
{
public:
    CityCodeUtils();
    QMap<QString,QString>cityCodeMap;

    QString getCityCodeFromName(QString cityName);

private:
    void initCityCodeMap();

};

#endif // CITYCODEUTILS_H
