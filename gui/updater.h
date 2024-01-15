#ifndef UPDATER_H
#define UPDATER_H



#include "QtNetwork/qnetworkreply.h"
#include <QDialog>
#include <QTimer>




namespace Ui {
class updater;
}

class updater : public QDialog
{
    Q_OBJECT

public:
    QTimer timer;  // Declare the timer as a private member
    QString zipPath;
    explicit updater(QWidget *parent = nullptr);
    ~updater();
    bool CheckForUpdate(const QString &currentVersion);
    void setProgress(int value);
    void setMessage(const QString &message);
    void setDownloadInfo(const QString &downloadUrl, const QString &destinationPath);
    void startDownload(const QUrl &downloadUrl, const QString &destinationPath);
    void ShowReleaseNotesDialog(const QString &releaseNotes);
    QString destinationPath;
    bool ExtractZip(const QString &zipPath);
    QNetworkAccessManager manager;
    Ui::updater *ui;
    void onDownloadFinished();
    void startUpdateProcess(const QString& zipPath);
    void updateDownloadProgress(qint64 bytesReceived, qint64 bytesTotal);
    QNetworkReply *reply;
    QString downloadUrl;
    void onCheckBoxStateChanged(int state);
signals:

    void updateAvailableSignal(bool updateAvailable);
    void downloadCompleted(bool success);
    void downloadProgress(qint64 bytesReceived, qint64 bytesTotal);
    void downloadUrlSet(const QString &downloadUrl);
private slots:
    void onCancelButtonClicked();
    void onDownloadUrlSet(const QString &downloadUrl);
private:



};

#endif // UPDATER_H
