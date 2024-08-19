#ifndef WIDGET_H
#define WIDGET_H

#include <QLabel>
#include <QList>
#include <QMenu>
#include <QNetworkReply>
#include <QWidget>
#include "day.h"
#include "citycodeutils.h"

QT_BEGIN_NAMESPACE
namespace Ui { class Widget; }
QT_END_NAMESPACE

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();

    Day days[7];
    QList<QLabel *>mDataList;
    QList<QLabel *>mWeekList;
    QList<QLabel *>mIconList;
    QList<QLabel *>mWeatherTypeList;
    QList<QLabel *>mAirQList;
    QList<QLabel *>mFxList;
    QList<QLabel *>mFlList;


public slots:
    void replyFinished(QNetworkReply *reply);
    void networkError_handler(QNetworkReply::NetworkError error);
protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    bool eventFilter(QObject *wacth,QEvent *event) override;

private slots:
    void on_lineEdit_search_returnPressed();

private:
    Ui::Widget *ui;
    QMenu *menuQuit;
    QPoint mOffset;
    QString strUrl;
    QNetworkAccessManager *manager;
    CityCodeUtils *cityCodeUtils;
    QMap<QString,QString> mTypeMap;

    void parseWeatherJsonData(QByteArray byteArr);
    void parseWeatherJsonDataFor7Days(QByteArray byteArr);
    void updateUI();
    void drawTempLineHigh();
    void drawTempLineLow();
};
#endif // WIDGET_H
