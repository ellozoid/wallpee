#include "wall.h"
#include "shlobj.h"
#include <ctime>
#include <QMessageBox>
#include <QMenu>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QDebug>
#include <QDir>

wall::wall(QWidget *parent): QMainWindow(parent),autoIn("HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Run",QSettings::NativeFormat),
    settings("HKEY_CURRENT_USER\\Software\\Wallpee", QSettings::NativeFormat)
{
    //setupUi(this);
    //setWindowIcon(QIcon(":/images/wallpee002.ico"));
//    revivefromsleep();
//    qDebug() << now_theme;
    if(!revivefromsleep()){
        deftheme();
    }
    this -> setTrayIconActions();
    this -> showTrayIcon();
    timer = new QTimer(this);
    autoIn.setValue("Wallpee", QCoreApplication::applicationFilePath().replace('/','\\'));
    connect(action, SIGNAL(triggered()), this, SLOT(about()));
    connect(timer, SIGNAL(timeout()), this, SLOT(on_changeButton_clicked()));
    connect(themes, SIGNAL(triggered(QAction*)), this, SLOT(themeChange(QAction*)));
    connect(colors, SIGNAL(triggered(QAction*)), this, SLOT(themeChange(QAction*)));
    connect(timers, SIGNAL(triggered(QAction*)), this, SLOT(setTime(QAction*)));

}
bool wall::revivefromsleep(){
    QString str = settings.value("theme").toString();
    if(!str.isEmpty() && !settings.value("theme").isNull() && str != "empty"){
        themeChoose(str);
        return true;
    }
    str = settings.value("color").toString();
    if(!str.isEmpty() && !settings.value("color").isNull() && str != "empty"){
        themeChoose(str);
        return true;
    }
    str = settings.value("timer").toString();
    if(!str.isEmpty()){
        timeChoose(str);
        return true;
    }
    return false;
}

void wall::timeChoose(QString str){
    if(str != "Без таймера"){
        if(str == "Пол часа"){
           timer->start(30 * 60 * 1000); //Пол часа
        }else if(str == "1 час"){
            timer->start(60 * 60 * 1000); //1 час
        }else if(str == "2 часа"){
            timer->start(2 * 60 * 60 * 1000); //2 часа
        }else if(str == "3 часа"){
            timer->start(3 * 60 * 60 * 1000); //3 часа
        }else if(str == "4 часа"){
            timer->start(4 * 60 * 60 * 1000); //4 часа
        }else if(str == "5 часов"){
            timer->start(5 * 60 * 60 * 1000); //5 часов
        }
    }
    if(str == "Без таймера" && timer->isActive()){
        timer->stop();
    }
}

void wall::setTime(QAction *a){
    QString str = a->text();

    timeChoose(str);
    saveSettings(str);

}
void wall::saveSettings(QString str){
    if(!now_color.isEmpty()){
        settings.setValue("color", now_color);
        if(!settings.value("theme").isNull() && settings.value("theme").toString() != "empty"){
            settings.setValue("theme", "empty");
        }
    }
    if(!now_theme.isEmpty()){
        settings.setValue("theme", now_theme);
        if(!settings.value("color").isNull() && settings.value("color").toString() != "empty"){
            settings.setValue("color", "empty");
        }
    }
    if(str != "0"){
        settings.setValue("timer", str);
    }

}

void wall::about(){
    QMessageBox::about(this, "О программе",
                 "<b>Wallpee</b><br>Версия 1.0<br>Copyright &#169; 2015 ellozoid (<a href=\"ellozoid@gmail.com\">ellozoid@gmail.com</a>)<br>Все изображения принадлежат их владельцам.<br>");
}
wall::~wall()
{
    //delete ui;
}

void wall::on_changeButton_clicked(){
    parse();
}

void wall::requestFinished(QNetworkReply* rep) {
    if (rep->error() == QNetworkReply::NoError) {
        QByteArray rawdata = rep->readAll();
        QString html = QString::fromUtf8(rawdata);

        parseHtml(html);

    } else {
        qDebug() << "request failed:(parse)" << testi++ << rep->errorString() << '\n' << rep->url() << '\n' << site << '\n' << url;
    }

    rep->deleteLater();
}
void wall::parseHtml(QString html) {
    srand( time( 0 ) );
    try {
        QGumboDocument doc = QGumboDocument::parse(html);
        QGumboNode root = doc.rootNode();
        if(count == 1){
            count++;
            auto nodes = root.getElementsByClassName("thumbnail center img-thumb");
            int r = rand()%11 + 1;
            for (auto& node: nodes) {
                if(t++ == r){
                    sec_site = site;
                    site = "http://www.zastavki.com" + node.getAttribute("href");
                    qDebug() << "site is " << site;
                    parse();
                    t = 1;
                    return;
                }
            }
            trayIcon->showMessage("Ошибка", "Страница не найдена[1p:" + !now_theme.isEmpty()?now_theme:now_color + "]");
        }
        if(count == 2){
            site = sec_site;
            count--;
            auto nodes = root.getElementsByClassName("original-link");
            for (auto& node: nodes) {
                url = zastavki + node.getAttribute("href");
                qDebug() << "url is " << url;
                filename();
                doDownload();
                return;
            }
            trayIcon->showMessage("Ошибка", "Не могу найти ссылку на скачивание[2p:" + !now_theme.isEmpty()?now_theme:now_color + "]");
        }
    } catch (...) {
        qCritical() << "smth wrong";
    }
}

void wall::deftheme(){
        srand( time( 0 ) );
        theme = "rus/Nature/" + QString::number(rand()%30 + 2) + '/';
        now_theme = "Природа";
        site = zastavki + theme;
}

void wall::parse(){
    QNetworkAccessManager *nm = new QNetworkAccessManager(this);
    QNetworkRequest requesturl;
    requesturl.setUrl(QUrl(site));
    connect(nm, SIGNAL(finished(QNetworkReply*)), this, SLOT(requestFinished(QNetworkReply*)));
    nm->get(requesturl);
}
void wall::nowclear(){
    if(!now_color.isEmpty()){
        now_color.clear();
    }
    if(!now_theme.isEmpty()){
        now_theme.clear();
    }
}
void wall::checkOffTheme(){
    for(int i = 0; i < themes->actions().size(); i++){
        if(themes->actions()[i]->isChecked())
            themes->actions()[i]->setChecked(false);
    }
}
void wall::checkOffColor(){
    for(int i = 0; i < colors->actions().size(); i++){
        if(colors->actions()[i]->isChecked())
            colors->actions()[i]->setChecked(false);
    }
}
void wall::themeChoose(QString str){
    srand( time( 0 ) );

    //Themes
    if(str == "Природа"){
        checkOffTheme();
        themes->actions()[0]->setChecked(true);
        theme = "rus/Nature/" + QString::number(rand()%30 + 2) + '/';
        nowclear();
        now_theme = "Природа";
    }
    if(str == "Новый год"){
        checkOffTheme();
        themes->actions()[1]->setChecked(true);
        theme = "rus/Holidays/New_Year_wallpapers/" + QString::number(rand()%30 + 2) + '/';
        nowclear();
        now_theme = "Новый год";
    }
    if(str == "Девушки"){
        checkOffTheme();
        themes->actions()[2]->setChecked(true);
        theme = "rus/Girls/" + QString::number(rand()%30 + 2) + '/';
        nowclear();
        now_theme = "Девушки";
    }
    if(str == "Автомобили"){
        checkOffTheme();
        themes->actions()[3]->setChecked(true);
        theme = "rus/Auto/" + QString::number(rand()%9 + 2) + '/';
        nowclear();
        now_theme = "Автомобили";
    }
    if(str == "Кошки"){
        checkOffTheme();
        themes->actions()[4]->setChecked(true);
        theme = "rus/Animals/Cats/" + QString::number(rand()%30 + 2) + '/';
        nowclear();
        now_theme = "Кошки";
    }
    if(str == "Зима"){
        checkOffTheme();
        themes->actions()[5]->setChecked(true);
        theme = "rus/Nature/Nature/Seasons/Winter/" + QString::number(rand()%30 + 2) + '/';
        nowclear();
        now_theme = "Зима";
    }

    //Colors
    if(str == "Красный"){
        checkOffColor();
        colors->actions()[0]->setChecked(true);
        nowclear();
        now_color = "Красный";
        theme = "?r=site%2Fcolors&service=colors&red=255&green=0&blue=0&orderby=date&page=" + QString::number(rand()%3 + 1) + '/';
    }
    if(str == "Золотой"){
        checkOffColor();
        colors->actions()[1]->setChecked(true);
        nowclear();
        now_color = "Золотой";
        theme = "?r=site%2Fcolors&service=colors&red=255&green=215&blue=0&orderby=date&page=" + QString::number(rand()%3 + 1) + '/';
    }
    if(str == "Тёмно-зелёный"){
        checkOffColor();
        colors->actions()[2]->setChecked(true);
        nowclear();
        now_color = "Тёмно-зелёный";
        theme = "?r=site%2Fcolors&service=colors&red=0&green=100&blue=0&orderby=date&page=" + QString::number(rand()%3 + 1) + '/';
    }
    if(str == "Зелёный"){
        checkOffColor();
        colors->actions()[3]->setChecked(true);
        nowclear();
        now_color = "Зелёный";
        theme = "?r=site%2Fcolors&service=colors&red=0&green=255&blue=0&orderby=date&page=" + QString::number(rand()%3 + 1) + '/';
    }
    if(str == "Синий"){
        checkOffColor();
        colors->actions()[4]->setChecked(true);
        nowclear();
        now_color = "Синий";
        theme = "?r=site%2Fcolors&service=colors&red=0&green=&blue=255&orderby=date&page=" + QString::number(rand()%3 + 1) + '/';
    }
    if(str == "Небесно-синий"){
        checkOffColor();
        colors->actions()[5]->setChecked(true);
        nowclear();
        now_color = "Небесно-синий";
        theme = "?r=site%2Fcolors&service=colors&red=191&green=&blue=255&orderby=date&page=" + QString::number(rand()%3 + 1) + '/';
    }
    site = zastavki + theme;
}
void wall::randomTheme(){
//    QList <QString> themeList = {"Природа", "Новый год", "Девушки", "Автомобили", "Кошки", "Зима"};
//    srand(time(0));
//    themeChoose(themeList.at(rand()%(themeList.count())));
}

void wall::themeChange(QAction *action){

    QString str = action->text();
    themeChoose(str);
    saveSettings("0");
}
void wall::filename(){
    QStringList filePathList = url.split('/');
    QString fileName = filePathList.at(filePathList.count() - 1);
    screenPath = "C:\\WallpeeImages\\";
    screenPath += fileName;
    QDir dir("C:\\");
    if(!dir.exists("C:\\WallpeeImages")){
        dir.mkdir("WallpeeImages");
    }
}
void wall::replyFinished(QNetworkReply *Reply)
{
    if (Reply->error() == QNetworkReply::NoError)
    {
        QByteArray data = Reply->readAll();
        img = QImage::fromData(data);
    }else{

        qDebug() << "request fail(doDownload): " << Reply->errorString();
    }

    Reply->deleteLater();
}

void wall::doDownload()
{
    QNetworkReply *reply;
    QNetworkAccessManager* networkManager = new QNetworkAccessManager(this);
    reply = networkManager->get(QNetworkRequest(QUrl(url)));
    connect(reply, SIGNAL(finished()), this, SLOT(toReplyFinished()));
    connect(networkManager, SIGNAL(finished(QNetworkReply*)), this, SLOT(replyFinished(QNetworkReply*)));

}
void wall::toReplyFinished(){
    img.save(screenPath);
    //SystemParametersInfoA
    if(SystemParametersInfo(SPI_SETDESKWALLPAPER, 0, (PVOID)screenPath.utf16(), SPIF_SENDWININICHANGE | SPIF_UPDATEINIFILE) == true){
        qDebug() << "It\'s ok, wall is changed";
    }else{
        qDebug() << "It\'s bad, wall isnt changed";
    }
    if(!now_color.isEmpty()){
        trayIcon->showMessage("Обои изменены", "Цвет: " + now_color);
    }else if(!now_theme.isEmpty()){
        trayIcon->showMessage("Обои изменены", "Тема: " + now_theme);
    }
}
//Show app in tray
void wall::showTrayIcon()
{
    // Создаём экземпляр класса и задаём его свойства...
    trayIcon = new QSystemTrayIcon(this);
    QIcon trayImage(":/images/icon.png");
    trayIcon -> setIcon(trayImage);
    trayIcon -> setContextMenu(trayIconMenu);

    // Подключаем обработчик клика по иконке...
    connect(trayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)), this, SLOT(trayIconActivated(QSystemTrayIcon::ActivationReason)));

    // Выводим значок...
    trayIcon -> show();
}
void wall::trayActionExecute()
{
    on_changeButton_clicked();
}

void wall::trayIconActivated(QSystemTrayIcon::ActivationReason reason)
{
    switch (reason)
    {
        case QSystemTrayIcon::Trigger:
        case QSystemTrayIcon::DoubleClick:
            this -> trayActionExecute();
            break;
        default:
            break;
    }
}
void wall::setTrayIconActions()
{
    // Setting actions...
    changeNow = new QAction(QIcon(":/images/png/play21.png"), "Сменить сейчас", this);
    //restoreAction = new QAction("Восстановить", this);
    //minimizeAction = new QAction("Свернуть", this);
    quitAction = new QAction(QIcon(":/images/png/man349.png"), "Выход", this);
    action = new QAction(QIcon(":/images/png/help1.png"), "О программе", this);
    luckAction = new QAction("Мне повезет", this);
    // Connecting actions to slots...
    //connect (minimizeAction, SIGNAL(triggered()), this, SLOT(hide()));
    //connect (restoreAction, SIGNAL(triggered()), this, SLOT(showNormal()));
    connect (quitAction, SIGNAL(triggered()), qApp, SLOT(quit()));
    connect(changeNow, SIGNAL(triggered()), this, SLOT(on_changeButton_clicked()));
    connect(luckAction, SIGNAL(triggered()), this, SLOT(randomTheme()));
    // Setting system tray's icon menu...
    themes = new QMenu("Тема:", this);
    themes->setIcon(QIcon(":/images/png/books8.png"));
    themes->addAction("Природа");
    themes->addAction("Новый год");
    themes->addAction("Девушки");
    themes->addAction("Автомобили");
    themes->addAction("Кошки");
    themes->addAction("Зима");


    colors = new QMenu("Цвета:", this);
    colors->setIcon(QIcon(":/images/png/bucket15.png"));
    colors->addAction("Красный");
    colors->addAction("Золотой");
    colors->addAction("Тёмно-зелёный");
    colors->addAction("Зелёный");
    colors->addAction("Синий");
    colors->addAction("Небесно-синий");

    timers = new QMenu("Таймер:", this);
    timers->setIcon(QIcon(":/images/png/chronometer10.png"));
    timers->addAction("Без таймера");
    timers->addAction("Пол часа");
    timers->addAction("1 час");
    timers->addAction("2 часа");
    timers->addAction("3 часа");
    timers->addAction("4 часа");
    timers->addAction("5 часов");
    for(int i = 0; i < themes->actions().size(); i++){
        themes->actions()[i]->setCheckable(true);
        colors->actions()[i]->setCheckable(true);
        timers->actions()[i]->setCheckable(true);
    }
    trayIconMenu = new QMenu(this);
    trayIconMenu->addAction (quitAction);
    trayIconMenu->addAction(action);
    trayIconMenu->addSeparator();
    trayIconMenu->addMenu(timers);
    trayIconMenu->addMenu(themes);
    trayIconMenu->addMenu(colors);
    trayIconMenu->addAction(changeNow);
    trayIconMenu->addAction(luckAction);
}
