#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QFileDialog>
#include <QDir>
#include <QMessageBox>
#include <QXmlStreamWriter>
#include <QDebug>
#include <QMediaMetaData>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);


    player = new QMediaPlayer(this);//初始化播放器
    playlist = new QMediaPlaylist(this);//初始化播放列表
    playlist->setPlaybackMode(QMediaPlaylist::Loop);
    player->setPlaylist(playlist);
    ui->musicList->setVisible(false);
    ui->labelMask->setVisible(false);


    connect(player,SIGNAL(positionChanged(qint64)),this,SLOT(onPositionChanged(qint64)));

    connect(player,SIGNAL(durationChanged(qint64)),this,SLOT(onDurationChanged(qint64)));

    connect(player,SIGNAL(durationChanged(qint64)),this,SLOT(onDurationChanged(qint64)));

    connect(playlist,SIGNAL(currentIndexChanged(int)),this,SLOT(onPlaylistChanged(int)));

    connect(player,SIGNAL(stateChanged(QMediaPlayer::State)),this,SLOT(onStateChanged(QMediaPlayer::State)));

    connect(player,SIGNAL(metaDataAvailableChanged(bool)),this,SLOT(onMetaDataAvailableChanged(bool)));

    setMediaWidgetIconAndText();
    setVolumeIconAndText();
    setPlayingText();

    QFile file(configName);
    if(file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        QXmlStreamWriter stream(&file);
        stream.setAutoFormatting(true);

        stream.writeStartDocument();

        stream.writeStartElement("About");

        stream.writeAttribute("content", "此文件由系统创建,请勿编辑");
        stream.writeTextElement("title", "Qt Home");
        stream.writeEndElement();
        stream.writeEndDocument();
        file.close();

        qDebug()<<configName;
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::onStateChanged(QMediaPlayer::State state)
{
    if(state == QMediaPlayer::StoppedState && isFinish == true)
        player->play();
    setMediaWidgetIconAndText();
}

void MainWindow::onMetaDataAvailableChanged(bool available)
{
    if(available)
        setInfoLabel();
}

void MainWindow::setMediaWidgetIconAndText()
{
    QListWidgetItem *item = ui->musicList->currentItem();
    switch(player->state())
    {
        case QMediaPlayer::PlayingState:
            this->setWindowTitle(tr("正在播放 ") + item->text() + " - MusicPlayer");

            ui->playpause->setToolTip(tr("暂停"));

            ui->playpause->setIcon(QIcon(":/icon/res/UI_Pause.png"));

            if(!ui->listView->isChecked())
                ui->labelMask->setVisible(false);
        break;

        case QMediaPlayer::PausedState:
            this->setWindowTitle("MusicPlayer");

            ui->playpause->setToolTip(tr("播放"));

            ui->playpause->setIcon(QIcon(":/icon/res/UI_Play.png"));

            if(!ui->listView->isChecked())
                ui->labelMask->setVisible(true);
        break;

        case QMediaPlayer::StoppedState:
            ui->LabRatio->setText("00:00/00:00");
            this->setWindowTitle("MusicPlayer");

            ui->playpause->setToolTip(("请添加音乐"));

            ui->playpause->setIcon(QIcon(":/icon/res/UI_Play.png"));
        break;
    }
}

void MainWindow::onPlaylistChanged(int position)
{
    ui->musicList->setCurrentRow(position);
    setMediaWidgetIconAndText();
}

void MainWindow::onDurationChanged(qint64 duration)
{
    ui->positionSlider->setMaximum(duration);

    int secs = duration / 1000;
    int mins = secs / 60;
    secs = secs % 60;

    QString strSecs = QString::number(secs);
    QString strMins = QString::number(mins);
    if(strSecs.length() < 2)
        strSecs = "0" + strSecs;
    if(strMins.length() < 2)
        strMins = "0" + strMins;
    durationTime = strMins +":" + strSecs;

    setPlayingText();
}

void MainWindow::onPositionChanged(qint64 position)
{
    if(ui->positionSlider->isSliderDown())
        return;
    ui->positionSlider->setSliderPosition(position);
    int secs = position / 1000;
    int mins = secs / 60;
    secs = secs % 60;

    QString strSecs = QString::number(secs);
    QString strMins = QString::number(mins);
    if(strSecs.length() < 2)
        strSecs = "0" + strSecs;
    if(strMins.length() < 2)
        strMins = "0" + strMins;
    positionTime = strMins +":" + strSecs;

    setPlayingText();
}

void MainWindow::on_addmusic_triggered()
{
    QStringList fileList = QFileDialog::getOpenFileNames(this,
                                                         tr("选择音频文件"),
                                                         QDir::homePath() + tr("/音乐/"),
                                                         tr("音频文件 (*.mp3 *.wav *.aac *.wma *.m4a)"));
    if(fileList.count() < 1)
        return;


    for (int i = 0;i < fileList.count();i++)
    {
        QString file = fileList.at(i);
        playlist->addMedia(QUrl::fromLocalFile(file));
        QFileInfo fileInfo(file);
        ui->musicList->addItem(fileInfo.fileName());
    }

    if(player->state() == QMediaPlayer::StoppedState)
        playlist->setCurrentIndex(0);
    player->play();
    setMediaWidgetIconAndText();
}

void MainWindow::on_removemusic_triggered()
{
    int pos = ui->musicList->currentRow();
    QListWidgetItem *item = ui->musicList->takeItem(pos);
    delete item;
    playlist->removeMedia(pos);

    //configIni->beginGroup("Setting");
    //configIni->setValue("playList",playlist.ci);
}

void MainWindow::on_cleanmusic_triggered()
{
    playlist->clear();
    //ui->musicList->clear();
    player->stop();
    setMediaWidgetIconAndText();
    ui->labelInfo->setText("");
    ui->labelPicture->setText("");
    ui->labelPicture->setPixmap(QPixmap(""));
}

void MainWindow::on_musicList_doubleClicked(const QModelIndex &index)
{
    int row = index.row();
    playlist->setCurrentIndex(row);
    player->play();
    setMediaWidgetIconAndText();
}

void MainWindow::on_playpause_clicked()
{
    if(player->state() == QMediaPlayer::PausedState)
    {
        if(playlist->currentIndex() < 0)
            playlist->setCurrentIndex(0);
        player->play();
    }
    else if(player->state() == QMediaPlayer::PlayingState)
        player->pause();
    setMediaWidgetIconAndText();
}

void MainWindow::on_positionSlider_valueChanged(int value)
{
    player->setPosition(value);
}

void MainWindow::on_positionSlider_sliderPressed()
{
    bool isplaying = false;
    if(player->state() == QMediaPlayer::PlayingState)
        isplaying = true;
    player->pause();
    if(ui->positionSlider->isSliderDown() && isplaying == true)
        player->play();
    setMediaWidgetIconAndText();
}

void MainWindow::on_previous_clicked()
{
    bool isPlayBackModeChanged = false;
    if(playlist->playbackMode() == QMediaPlaylist::CurrentItemInLoop)
    {
        playlist->setPlaybackMode(QMediaPlaylist::Loop);
        isPlayBackModeChanged = true;
    }

    playlist->previous();
    player->play();
    setMediaWidgetIconAndText();
    if(isPlayBackModeChanged)
        playlist->setPlaybackMode(QMediaPlaylist::CurrentItemInLoop);
}

void MainWindow::on_next_clicked()
{
    bool isPlayBackModeChanged = false;
    if(playlist->playbackMode() == QMediaPlaylist::CurrentItemInLoop)
    {
        playlist->setPlaybackMode(QMediaPlaylist::Loop);
        isPlayBackModeChanged = true;
    }

    playlist->next();
    player->play();
    setMediaWidgetIconAndText();
    if(isPlayBackModeChanged)
        playlist->setPlaybackMode(QMediaPlaylist::CurrentItemInLoop);
}

void MainWindow::on_volumeBtn_clicked()
{
    if(!player->isMuted())
    {
        player->setMuted(true);
    }
    else
    {
        player->setMuted(false);
    }
    setVolumeIconAndText();
}

void MainWindow::on_volumeSlider_valueChanged(int value)
{
    player->setVolume(value);
    volume = value;
    setVolumeIconAndText();
}

void MainWindow::setVolumeIconAndText()
{
    if(player->isMuted())
    {
        ui->volumeBtn->setIcon(QIcon(":/icon/res/MD_Volume_mute.png"));

        ui->volumeLab->setText(tr("已静音"));
    }
    else
    {
        if(volume > 0 && volume < 100)
            ui->volumeBtn->setIcon(QIcon(":/icon/res/MD_Volume_down.png"));
        else if(volume == 0)
            ui->volumeBtn->setIcon(QIcon(":/icon/res/MD_Volume_off.png"));
        else if(volume == 100)
            ui->volumeBtn->setIcon(QIcon(":/icon/res/MD_Volume_up.png"));

        ui->volumeLab->setText(QString::number(volume) + "%");
    }
}

void MainWindow::setPlayingText()
{
    ui->LabRatio->setText(positionTime + "/" + durationTime);
    ui->LabRatio->setToolTip(positionTime + "/" + durationTime);
    ui->positionSlider->setToolTip(positionTime + "/" + durationTime);

    if(ui->positionSlider->value() == ui->positionSlider->maximum() && ui->positionSlider->maximum() != 0 && playlist->mediaCount() != 0 && player->state() == QMediaPlayer::PlayingState && ui->positionSlider->isSliderDown() == false)
    {
        playlist->next();
        player->play();
        setMediaWidgetIconAndText();
        isFinish = true;
    }
    setMediaWidgetIconAndText();
}

void MainWindow::on_playModeBtn_clicked()
{
    switch(playlist->playbackMode())
    {
        case QMediaPlaylist::Loop:
            playlist->setPlaybackMode(QMediaPlaylist::Random);
            ui->playModeBtn->setIcon(QIcon(":/icon/res/UI_Random.png"));
        break;
        case QMediaPlaylist::Random:
            playlist->setPlaybackMode(QMediaPlaylist::CurrentItemInLoop);
            ui->playModeBtn->setIcon(QIcon(":/icon/res/UI_Single_music_circulation.png"));
        break;
        case QMediaPlaylist::CurrentItemInLoop:
            playlist->setPlaybackMode(QMediaPlaylist::Loop);
            ui->playModeBtn->setIcon(QIcon(":/icon/res/UI_Loop.png"));
        break;
    }
}

void MainWindow::on_about_triggered()
{
    QMessageBox::about(this, tr("关于"), tr("Music Player Version Bata 1.0 \n 用Qt制作 \n 由暗丶dark/沉寂°丶制作 \n Qt的版权属于The Qt Company \n 本项目支持MIT许可证"));
}

void MainWindow::on_listView_stateChanged(int arg)
{
    switch (arg) {
        case 0:
        ui->listView->setChecked(false);
        ui->musicList->setVisible(false);
        ui->labelInfo->setVisible(true);
        ui->labelPicture->setVisible(true);
        break;
        case 2:
        ui->listView->setChecked(true);
        ui->musicList->setVisible(true);
        ui->labelInfo->setVisible(false);
        ui->labelPicture->setVisible(false);
        break;
    }
}

void MainWindow::setInfoLabel()
{   
    info.Title = player->metaData(QMediaMetaData::Title).value<QString>();
    info.AlbumTitle = player->metaData(QMediaMetaData::AlbumTitle).value<QString>();
    info.Publisher = player->metaData(QMediaMetaData::Publisher).value<QString>();
    info.Copyright = player->metaData(QMediaMetaData::Copyright).value<QString>();
    info.Year = player->metaData(QMediaMetaData::Year).value<int>();
    info.CoverArtImage = player->metaData(QMediaMetaData::CoverArtImage).value<QImage>();
    info.ContributingArtist = player->metaData(QMediaMetaData::ContributingArtist).value<QStringList>();

    for(int i = 0 ; i< info.ContributingArtist.size() ; ++i)
    {
        i == 0 ? info.Artist = "" : info.Artist = info.Artist;
        info.Artist += info.ContributingArtist.at(i);
        i == info.ContributingArtist.size() - 2 ? info.Artist += "/" : info.Artist += "";
    }

    if(info.Title.isEmpty())
        info.Title = "未知标题";

    if(info.Artist.isEmpty())
        info.Artist = "未知艺术家";

    if(info.AlbumTitle.isEmpty())
        info.AlbumTitle = "未知专辑";

    if(info.Publisher.isEmpty())
        info.Publisher = "未知出版商";

    if(info.Copyright.isEmpty())
        info.Copyright = "未知版权信息";

    if(info.CoverArtImage.isNull())
        ui->labelPicture->setText("无封面");
    else
    {
        ui->labelPicture->setText("");
        ui->labelPicture->setPixmap(QPixmap::fromImage(info.CoverArtImage));
    }



    info.Final = "标题:" +info.Title + "\n" + "作者:" +info.Artist + "\n" + "专辑:" +info.AlbumTitle + "\n" + "出版商:" +info.Publisher + "\n" + "版权信息:" +info.Copyright + "\n";
    info.Year == 0 ? info.Final += "年代:" + YearNull : info.Final += "年代:" + QString::number(info.Year);
    ui->labelInfo->setText(info.Final);
}


