发送HTTP请求获取全国各地的天气数据,实现一个查询并显示天气数据的客户端程序    


![image](https://github.com/wang19950805/QtWeatherReportProject/blob/master/preview.png)

  http请求  
  
    //使用QNetworkAccessManager对象来管理http请求
    QNetworkAccessManager *manager = new QNetworkAccessManager(this);   
    
    //构建请求地址  
    QNetworkRequest request(QUrl("www.baidu.com"));
    
    //发送get请求
    manager->get(request);
    
    //获取http请求的reply对象
    QNetworkReply *reply = manager->get(request);
    
    //连接manager和reply的信号,实现相应的槽函数进行处理
    connect(manager,&QNetworkAccessManager::finished,this,&Widget::replyFinished);
    connect(reply,SIGNAL(errorOccurred(QNetworkReply::NetworkError)),this,SLOT(networkError_handler(QNetworkReply::NetworkError)));
    
    //网络请求状态码
    reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);

  json数据解析  

    //从QByteArray中获取一个QJsonDocument对象
    QJsonDocument jsonObj = QJsonDocument::fromJson(byteArr);
    
    //使用jsonObj.isObject()或jsonObj.isArray()判断json数据中是对象还是数组,并将其转换成对应类型
    //jsonObj.object() 或 jsonObj.array()

    //取值
    QString str = jsonObj["data"].toString;
    QJsonArray jsonArr = jsonObj["data"].toArray();
    QJsonObject dataObj = jsonObj.toObject();
    
    //遍历QJsonArray,其中的元素类型为QJsonValue
    for(QJsonValue value : jsonArr){
       ...;
    }  

    
  事件过滤器
  
    //为需要响应的控件或子控件安装事件
    ui->label_search->installEventFilter(this);
    //重写eventFilter方法,在此函数中可以对控件进行过滤,筛选出需要响应的控件进行操作
    //同样的也可以对Qevent事件进行过滤,判断出对各种类型的事件进行响应
    bool eventFilter(QObject *wacth,QEvent *event) override;
    



















  
