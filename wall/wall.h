#ifndef WALL_H
#define WALL_H

#include <QWidget>
#include <QMainWindow>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <qgumbodocument.h>
#include <qgumbonode.h>
#include <QSystemTrayIcon>
#include <QTimer>
#include <QSettings>


class wall : public QMainWindow
{
    Q_OBJECT
    QSettings settings, autoIn;

public:

    explicit wall(QWidget *parent = 0);
    ~wall();

private:
    int count = 1,
        t = 1,
        now = 0,
        testi = 1;
    void parseHtml(QString);
    void parse();
    void filename();
    void doDownload();
    void deftheme();
    void saveSettings(QString str);
    void nowclear();
    void themeChoose(QString);
    void timeChoose(QString);
    void checkOffTheme();
    void checkOffColor();
    bool revivefromsleep();

    QString zastavki = "http://www.zastavki.com/",  // the original link to the website
            site,   // where we will look for wallpaper
            screenPath,     //link to the wallpaper on your computer
            theme, //selected theme
            color,  //selected color
            url,    //link to wallpaper
            sec_site,
            now_theme,
            now_color;
    QImage img;
    QMenu *trayIconMenu,
          *themes,
          *colors,
          *timers;
    QAction *minimizeAction,
            *restoreAction,
            *quitAction,
            *changeNow,
            *action,
            *luckAction;
    QSystemTrayIcon *trayIcon;
    QTimer *timer;



private slots:
    void randomTheme();
    void setTime(QAction*);
    void trayIconActivated(QSystemTrayIcon::ActivationReason reason);
    void trayActionExecute();
    void setTrayIconActions();
    void showTrayIcon();
    void about();
    void toReplyFinished();
    void replyFinished(QNetworkReply*);
    void themeChange(QAction *);
    void requestFinished(QNetworkReply*);
    void on_changeButton_clicked(); //changing the wallpaper
};

#endif // WALL_H
