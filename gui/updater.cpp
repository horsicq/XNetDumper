#include "updater.h"
#include "ui_updater.h"
#include <QFile>
#include <QBuffer>
#include <QtConcurrent>

updater::updater(QWidget *parent)
    : QDialog(parent),
    ui(new Ui::updater)
{
    ui->setupUi(this);
    qDebug() << "updater constructor called.";
    connect(ui->checkBox, &QCheckBox::stateChanged, this, &updater::onCheckBoxStateChanged);
    connect(this, &updater::downloadUrlSet, this, &updater::onDownloadUrlSet);
    resize(414, 118);  // Adjust the size as needed
    ui->textEdit->hide();
    // Load and set an image in the imageLabel
    QPixmap image(":/images/Die.png");
    ui->imageLabel->setPixmap(image);
    ui->imageLabel->setScaledContents(true);
    connect(ui->cancelButton, &QPushButton::clicked, this, &updater::onCancelButtonClicked);
    QTimer timer;
    timer.start(100);  // Adjust the interval as needed
}

void updater::onDownloadUrlSet(const QString &downloadUrl)
{
    if(ui->checkBox->isChecked())
    {
        ui->textEdit->setText(downloadUrl);
    }
}


updater::~updater()
{
    delete ui;
}

void updater::setProgress(int value)
{
    ui->progressBar->setValue(value);
}

void updater::setMessage(const QString &message)
{

    ui->labelMessage->setText(message);
}

void updater::setDownloadInfo(const QString &downloadUrl, const QString &destinationPath)
{
    this->downloadUrl = downloadUrl;
    this->destinationPath = destinationPath;
    ui->labelMessage->setText("Downloading...");
    ui->textEdit->setText(downloadUrl);


}

void updater::startDownload(const QUrl &downloadUrl, const QString &destinationPath)
{
    // Reset UI
    ui->progressBar->setValue(0);
    ui->labelMessage->setText("Downloading...");
    ui->checkBox->setEnabled(true);
    this->downloadUrl = downloadUrl.toString();
    emit downloadUrlSet(this->downloadUrl);

    // qDebug() << "Download URL:" << downloadUrl.toString();
    qDebug() << "In startDownload, this:" << this;

    qDebug() << "In startDownload, destinationPath:" << destinationPath;

    // Configure the network request
    QNetworkRequest request(downloadUrl);
    request.setAttribute(QNetworkRequest::RedirectPolicyAttribute, QNetworkRequest::NoLessSafeRedirectPolicy);

    // Start download asynchronously
    reply = manager.get(request);

    // Connect signals for download progress
    connect(reply, &QNetworkReply::downloadProgress, this, &updater::updateDownloadProgress);

    // Open the destination file for writing
    QFile *file = new QFile(destinationPath);
    if (file->open(QIODevice::WriteOnly))
    {
        // Connect signal for readyRead
        connect(reply, &QNetworkReply::readyRead, this, [this, file]{
            qint64 bytesWritten = file->write(reply->readAll());
            if (bytesWritten == -1) {
                qDebug() << "File write error:" << file->errorString();
            }
        });
        // Connect signal for download completion
        connect(reply, &QNetworkReply::finished, this, [this, file]{
            file->flush();
            file->close();
            onDownloadFinished();
        });

        // Show the dialog
        show();
    }
    else
    {
        qDebug() << "Failed to create the destination file.";
        accept();
    }
}

// Slot for handling Cancel button click
void updater::onCancelButtonClicked()
{

    // Check if there is an ongoing download
    if (reply && (reply->isRunning() || reply->isFinished())) {
        // Disconnect signals to avoid further interactions with the reply
        disconnect(reply, nullptr, nullptr, nullptr);

        // Abort the download
        reply->abort();

        // Wait for the reply to finish aborting
        while (reply->isRunning()) {
            QCoreApplication::processEvents();
        }

        // Clean up the reply object
        reply->deleteLater();
        reply = nullptr;

        // Close the dialog
        reject();
    }
}

void updater::updateDownloadProgress(qint64 bytesReceived, qint64 bytesTotal)
{
    int percentage = static_cast<int>((static_cast<double>(bytesReceived) / bytesTotal) * 100);
    ui->progressBar->setValue(percentage);
    // Ensure the dialog is shown
    if (!isVisible()) {
        show();
    }

    // Process events to update the UI
    QCoreApplication::processEvents();
}

void updater::onDownloadFinished() {
    qDebug() << "In onDownloadFinished, this:" << this;

    // Request application termination
    qApp->quit();

    // Wait for the application to exit
    while (qApp->closingDown()) {
        QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
    }

    // Check for errors
    if (reply->error() != QNetworkReply::NoError)
    {
        qDebug() << "Network error:" << reply->errorString();
        return;
    }

    // The download is complete, so extraction can now be performed
    QString destinationDir = QApplication::applicationDirPath() + "/";
    QDir directory(destinationDir);
    QStringList zipFiles = directory.entryList(QStringList() << "*.zip" << "*.ZIP", QDir::Files);

    // Connect the aboutToQuit signal to a slot that handles extraction
    connect(qApp, &QCoreApplication::aboutToQuit, this, [=]() {
        foreach(QString filename, zipFiles) {
            QString zipPath = destinationDir + filename;

            // Debugging information
            qDebug() << "zipPath:" << zipPath;

            // Ensure the file exists
            if (!QFile::exists(zipPath)) {
                qDebug() << "Zip file does not exist.";
                return;
            }
            // Debugging information
            qDebug() << "Starting update process...";
            // Stop the timer
            timer.stop();
            // Extract the zip file using QProcess
            QProcess unzipProcess;
            unzipProcess.setProgram("unzip");
            QStringList arguments;
            arguments << "-o" << zipPath << "-d" << destinationDir;
            unzipProcess.setArguments(arguments);

            if (unzipProcess.startDetached() && unzipProcess.waitForFinished()) {
                qDebug() << "Extraction completed successfully.";
            } else {
                qDebug() << "Extraction failed.";
            }
        }
    });

    // Clean up the reply object
    reply->deleteLater();

    // Close the dialog
    accept();
}


void updater::onCheckBoxStateChanged(int state)
{
    if (state == Qt::Checked) {
        // The checkbox is checked, show the QTextEdit and set its text to the download URL
        qDebug() << "Download URL:" << downloadUrl;  // Debug print

        ui->textEdit->setText(downloadUrl);
        ui->textEdit->show();
        // Resize the window to show the QTextEdit
        resize(414, 193);  // Adjust the size as needed
    } else {
        // The checkbox is not checked, clear the QTextEdit and hide it

        ui->textEdit->hide();
        // Resize the window to the original size
        resize(414, 118);  // Adjust the size as needed
    }
}

void updater::ShowReleaseNotesDialog(const QString &releaseNotes)
{

}

