#include "widget.h"
#include "ui_widget.h"

#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QMessageBox>
#include <QMouseEvent>
#include <QNetworkAccessManager>
#include <QPainter>
#include <QPixmap>

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);
    this->setFont(QFont("Arial",15));
    this->setFixedSize(507,936);
    setWindowFlag(Qt::FramelessWindowHint);
    menuQuit = new QMenu(this);
    menuQuit->setStyleSheet("QMenu::item {color:white}");
    QAction *actionClose = new QAction("退出",this);
    menuQuit->addAction(actionClose);
    connect(menuQuit,&QMenu::triggered,this,[=]{
        this->close();
    });

    //http请求
    manager = new QNetworkAccessManager(this);
//    QNetworkRequest request(QUrl("http://t.weather.itboy.net/"));

    strUrl = "http://t.weather.itboy.net/api/weather/city/101010100";
    QNetworkRequest request(strUrl);
//    QNetworkRequest request(QUrl("www.baidu.com"));
    //        manager->post();
    QNetworkReply *reply = manager->get(request);

    connect(manager,&QNetworkAccessManager::finished,this,&Widget::replyFinished);
    connect(reply,SIGNAL(errorOccurred(QNetworkReply::NetworkError)),this,SLOT(networkError_handler(QNetworkReply::NetworkError)));


    ui->label_search->installEventFilter(this);


//    mTypeMap.insert("暴雪",":/res/type/Baoxue.png");
//    mTypeMap.insert("暴雪",":/res/type/Baoxue.png");
//    mTypeMap.insert("暴雪",":/res/type/Baoxue.png");
//    mTypeMap.insert("暴雪",":/res/type/Baoxue.png");
//    mTypeMap.insert("暴雪",":/res/type/Baoxue.png");
//    mTypeMap.insert("暴雪",":/res/type/Baoxue.png");
//    mTypeMap.insert("暴雪",":/res/type/Baoxue.png");

    //绘制温度变化折线图
    ui->widget_7->installEventFilter(this);
     ui->widget_8->installEventFilter(this);

    mAirQList << ui->label_airQ01<< ui->label_airQ02<< ui->label_airQ03<< ui->label_airQ04<< ui->label_airQ05<< ui->label_airQ06;

}

Widget::~Widget()
{
    delete ui;
}

void Widget::replyFinished(QNetworkReply *reply)
{
    qDebug()<<"replyFinished";
    QByteArray byteData;
    QMessageBox msgBox;
    switch (reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt()) {
    case 400:
        qDebug()<<reply->errorString();

        msgBox.setWindowTitle("错误");
        msgBox.setText("网络连接失败!");
        msgBox.setStyleSheet("QPushButton{color:black}");
        msgBox.setStandardButtons(QMessageBox::Ok);
        msgBox.exec();
        break;
    case 200:

        byteData = reply->readAll();
        parseWeatherJsonData(byteData);
        qDebug()<<QString::fromUtf8(byteData);
        break;
    default:
        break;
    }


}

void Widget::networkError_handler(QNetworkReply::NetworkError error)
{
    qDebug()<<"error:"<<error;
}

void Widget::mousePressEvent(QMouseEvent *event)
{
    if(event->button() == Qt::RightButton){
//        menuQuit->show();
        menuQuit->exec(QCursor::pos());
    }
    if(event->button() == Qt::LeftButton){
        mOffset = event->globalPos() - this->pos();
    }


}

void Widget::mouseMoveEvent(QMouseEvent *event)
{
    this->move(event->globalPos() - mOffset);
}

bool Widget::eventFilter(QObject *wacth, QEvent *event)
{
    if(wacth == ui->label_search){
        if(event->type() == QEvent::MouseButtonPress){
            QString searchStr = ui->lineEdit_search->text();
//            QString cityCode = cityCodeUtils->getCityCodeFromName(searchStr);
            QString cityCode = "101010100";
            if(cityCode != ""){
                //                strUrl += "&cityid=" + cityCode;

                strUrl = "http://t.weather.itboy.net/api/weather/city/101010100";
                manager->get(QNetworkRequest(QUrl(strUrl)));

            }else{
                QMessageBox msgBox;
                msgBox.setWindowTitle("错误");
                msgBox.setText("请输入正确的城市!");
                msgBox.setStyleSheet("QPushButton{color:black}");
                msgBox.setStandardButtons(QMessageBox::Ok);
                msgBox.exec();
            }
        }
    }

    if(wacth == ui->widget_7){
        drawTempLineHigh();

    }
    if(wacth == ui->widget_8){
        drawTempLineLow();
    }



    return QWidget::eventFilter(wacth,event);
}

void Widget::parseWeatherJsonData(QByteArray byteArr){

    QJsonDocument jsonObj = QJsonDocument::fromJson(byteArr);
    if(!jsonObj.isEmpty()&&jsonObj.isObject()){
        QJsonObject dataObj = jsonObj.object();
        //解析城市信息
        QJsonObject cityInfo = dataObj["cityInfo"].toObject();
        QString city = cityInfo["city"].toString();
        ui->label_city->setText(city);

       //解析日期
        QString time = dataObj["time"].toString();
        ui->label_currentTime->setText(time);
        //解析气温
        QJsonObject data = dataObj["data"].toObject();
        QString wendu = data["wendu"].toString();
        ui->label_tmp->setText(wendu);
        //天气类型
        QJsonArray forecast = data["forecast"].toArray();
        QJsonObject forecastObj = forecast[0].toObject();
        QString type = forecastObj["type"].toString();
        ui->label_weatherType->setText(type);
//        ui->label_weatherIcon->setText(mTypeMap.find(type).value());
        //气温范围
        QString higtTmp = forecastObj["high"].toString();
        QString lowTmp = forecastObj["low"].toString();
        ui->label_tmpRange->setText(higtTmp+"  "+lowTmp);
        //风向和等级
        QString fx = forecastObj["fx"].toString();
        QString fl = forecastObj["fl"].toString();
        ui->label_fx->setText(fx);
        ui->label_fl->setText(fl);
        //pm2.5
        double pm25 = data["pm25"].toDouble();
        ui->label_pm25->setText(QString::number(pm25));
        //湿度
        QString shidu = data["shidu"].toString();
        ui->label_shidu->setText(shidu);
        //空气质量
        QString quality = data["quality"].toString();
        ui->label_quality->setText(quality);
        //tips
        QString notice = forecastObj["notice"].toString();
        ui->label_notice->setText(notice);
    }

}

void Widget::parseWeatherJsonDataFor7Days(QByteArray byteArr)
{
    QJsonDocument jsonObj = QJsonDocument::fromJson(byteArr);
    if(!jsonObj.isNull() && jsonObj.isObject()){
        QJsonObject jsonData = jsonObj.object();
        days[0].mCity = jsonData["city"].toString();
        days[0].mPm25 = jsonData["api"].toObject()["pm25"].toString();
        if(jsonData.contains("data") && jsonData["data"].isArray()){
            QJsonArray jsonArr = jsonData["data"].toArray();
            for (int i = 0; i < jsonArr.size(); ++i) {
                QJsonObject jsonDay = jsonArr[i].toObject();
                days[i].mData = jsonDay["date"].toString();
                days[i].mWeek = jsonDay["date"].toString();
                days[i].mWeather = jsonDay["date"].toString();
                days[i].mTemp = jsonDay["date"].toString();
                days[i].mTempHigh = jsonDay["date"].toString();
                days[i].mTempLow = jsonDay["date"].toString();
                days[i].mFX = jsonDay["date"].toString();
                days[i].mFL = jsonDay["date"].toString();
                days[i].mAirq = jsonDay["date"].toString();
                days[i].mTips = jsonDay["date"].toString();

            }
        }
    }
    updateUI();
}

void Widget::updateUI()
{
//    //城市
//    ui->label_city->setText(city);

//    //解析日期
//    ui->label_currentTime->setText(time);
//    //解析气温

//    ui->label_tmp->setText(wendu);
//    //天气类型
//    ui->label_weatherType->setText(type);
//    ui->label_weatherIcon->setText(mTypeMap.find(type).value());
//    //气温范围
//    ui->label_tmpRange->setText(higtTmp+"  "+lowTmp);
//    //风向和等级
//    ui->label_fx->setText(fx);
//    ui->label_fl->setText(fl);
//    //pm2.5
//    ui->label_pm25->setText(QString::number(pm25));
//    //湿度
//    QString shidu = data["shidu"].toString();
//    ui->label_shidu->setText(shidu);
//    //空气质量
//    ui->label_quality->setText(quality);
//    //tips
//    ui->label_notice->setText(notice);
//    QPixmap pixmap;
//    for (int i = 0; i < 6; ++i) {
//        mWeekList[i]->setText(days[i].mWeek);
//        QStringList list = days[i].mData.split("-"); //2024-08-15
//        mDataList[i]->setText(list.at(1)+"/"+list.at(2));
//        //设置icon缩放适应
//        pixmap = mTypeMap[days[i].mWeather];
//        pixmap = pixmap.scaled(mIconList[i]->size(),Qt::KeepAspectRatio,Qt::SmoothTransformation);
//        mIconList[i]->setMaximumHeight(50);
//        mIconList[i]->setMaximumWidth(50);
//        mIconList[i]->setPixmap(pixmap);

//        mWeatherTypeList[i]->setText(days[i].mWeather);

//        mAirQList[i]->setText(days[i].mAirq);

//        mFxList[i]->setText(days[i].mFX);
//        mFlList[i]->setText(days[i].mFL);

//        QString airQ = days[i].mAirq;
//        if(airQ == "优"){

//        }
//        if(airQ == "良"){

//        }
//        if(airQ == "轻度污染"){

//        }
//        if(airQ == "中度污染"){

//        }
//        if(airQ == "重度污染"){

//        }
//    }

}

void Widget::drawTempLineHigh()
{

    QPainter painter(ui->widget_7);
    painter.setBrush(Qt::yellow);
    painter.setPen(Qt::yellow);

    //计算平均温度
    int avgHigh;
    int sum = 0;
    for (int i = 0; i < 6; ++i) {
        sum += days[i].mTempHigh.toInt();
    }
    avgHigh = sum / 6;
    //计算当天温度偏移值
    int offset = 0;
   //定义6个点
    QPoint points[6];
    for (int i = 0; i < 6; ++i) {
        offset = days[i].mTempHigh.toInt() - avgHigh;
        points[i].setX(mAirQList[i]->x() + mAirQList[i]->width() / 2);
        points[i].setY(ui->widget_7->height()/2 - offset);

        //绘制
        painter.drawEllipse(QPoint(points[i]),3,3);
        painter.drawText(points[i].x() - 15,points[i].y() - 15,days[i].mTempHigh + "℃");
    }

    //画线
    for (int i = 0; i < 5; ++i) {
        painter.drawLine(points[i],points[i+1]);
    }

}

void Widget::drawTempLineLow()
{
    QPainter painter(ui->widget_8);
    painter.setBrush(Qt::blue);
    painter.setPen(Qt::blue);
    //计算平均温度
    int avgLow;
    int sum = 0;
    for (int i = 0; i < 6; ++i) {
        sum += days[i].mTempHigh.toInt();
    }
    avgLow = sum / 6;
    //计算当天温度偏移值
    int offset = 0;

   //定义6个点
    QPoint points[6];
    for (int i = 0; i < 6; ++i) {
        offset = days[i].mTempLow.toInt() - avgLow;
        points[i].setX(mAirQList[i]->x() + mAirQList[i]->width() / 2);
        points[i].setY(ui->widget_8->height()/2 - offset);

        //绘制
        painter.drawEllipse(QPoint(points[i]),3,3);
        painter.drawText(points[i].x() - 15,points[i].y() - 15,days[i].mTempHigh + "℃");
    }

    //画线
    for (int i = 0; i < 5; ++i) {
        painter.drawLine(points[i],points[i+1]);
    }

}

void Widget::on_lineEdit_search_returnPressed()
{

}

