#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMediaPlayer>
#include <QMediaPlaylist>
#include <QSettings>
#include <QDate>
#include <QImage>
#include <QMediaObject>
#include <QMediaMetaData>
#include <QCoreApplication>
#include <QStandardItemModel>



QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

private:
    QMediaPlayer *player;//播放器
    QMediaPlaylist *playlist;//播放列表
    QString durationTime = "00:00";//总时间
    QString positionTime = "00:00";//当前播放时间
    int volume = 100;
    bool isFinish = false;
    QString configName = QCoreApplication::applicationDirPath() + "/app.config";

    struct musicInfomation
    {
        QString Title;
        QStringList ContributingArtist;
        QString Artist;
        QString Copyright;
        QString Publisher;
        QString AlbumTitle;
        int Year = 0;
        QImage CoverArtImage;

        QString Final;
    }musicInfomation;

    struct musicInfomation info;
    QString YearNull = "未知年份";


public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void onPlaylistChanged(int position);
    void onDurationChanged(qint64 duration);
    void onPositionChanged(qint64 position);
    void onMetaDataAvailableChanged(bool available);
    void onStateChanged(QMediaPlayer::State state);
    void setMediaWidgetIconAndText();
    void setVolumeIconAndText();
    void setPlayingText();
    void setInfoLabel();

    void on_addmusic_triggered();

    void on_removemusic_triggered();

    void on_cleanmusic_triggered();

    void on_musicList_doubleClicked(const QModelIndex &index);

    void on_playpause_clicked();

    void on_positionSlider_valueChanged(int value);

    void on_positionSlider_sliderPressed();

    void on_previous_clicked();

    void on_next_clicked();

    void on_volumeBtn_clicked();

    void on_volumeSlider_valueChanged(int value);

    void on_playModeBtn_clicked();

    void on_about_triggered();

    void on_listView_stateChanged(int arg);

private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
