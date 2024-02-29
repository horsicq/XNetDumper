// LoadAssembly.cpp
#include "LoadAssembly.h"
#include <QLibrary>
#include <QDebug>
#include <QFile>
#include <QDir>
#include <QCoreApplication>
#include <QDataStream>
#include <QBuffer>

#include <QList>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>


QVector<LoadAssembly> loadedAssemblies;
void LoadAssemblyIntoMemory(const QString& dllPath)
{

    loadedAssemblies.clear();
    QLibrary library(dllPath);
    if (library.load())
    {
        LoadAssembly loadedAssembly(dllPath);
        loadedAssemblies.push_back(loadedAssembly);
    }
    else
    {
        qDebug() << "Failed to load DLL: " << library.errorString();
    }
}

QString GetAssemblyInfo(QLibrary* library) {
    QString assemblyInfo;

    QString moduleFileName = QDir::toNativeSeparators(library->fileName());
    QFileInfo fileInfo(moduleFileName);
    QString moduleName = fileInfo.fileName();

    assemblyInfo += moduleName + "\r\n";

    qDebug() << "File Path from Memory: " << moduleFileName;

    HMODULE hModule = GetModuleHandleW(reinterpret_cast<LPCWSTR>(moduleFileName.utf16()));
    if (!hModule) {
        qWarning() << "Failed to get module handle for" << moduleFileName;
        return "";
    }

    quintptr moduleBase = reinterpret_cast<quintptr>(hModule);

    IMAGE_DOS_HEADER dosHeader;
    SIZE_T bytesRead = 0;

    if (!ReadProcessMemory(GetCurrentProcess(), reinterpret_cast<LPCVOID>(moduleBase),
                           &dosHeader, sizeof(dosHeader), &bytesRead) || bytesRead != sizeof(dosHeader)) {
        qWarning() << "Failed to read DOS header for" << moduleFileName;
        return "";
    }

    if (dosHeader.e_magic != IMAGE_DOS_SIGNATURE) {
        qWarning() << "Invalid MZ header for" << moduleFileName;
        return "";
    }

    quintptr peSignatureOffset = moduleBase + dosHeader.e_lfanew;

    IMAGE_NT_HEADERS ntHeaders;
    if (!ReadProcessMemory(GetCurrentProcess(), reinterpret_cast<LPCVOID>(peSignatureOffset),
                           &ntHeaders, sizeof(ntHeaders), &bytesRead) || bytesRead != sizeof(ntHeaders)) {
        qWarning() << "Failed to read NT headers for" << moduleFileName;
        return "";
    }

    if (ntHeaders.Signature != IMAGE_NT_SIGNATURE) {
        qWarning() << "Invalid PE signature for" << moduleFileName;
        return "";
    }

    quint32 entryPointRVA = ntHeaders.OptionalHeader.AddressOfEntryPoint;

    assemblyInfo += "Entry Point RVA: 0x" + QString::number(entryPointRVA, 16) + "\r\n";
    assemblyInfo += "Machine: 0x" + QString::number(ntHeaders.FileHeader.Machine, 16) + "\r\n";

    QByteArray fileData;

    QString fileInformation = GetFileInformation(fileData, moduleFileName);
    if (fileInformation.isEmpty()) {
        qDebug() << "Error: Failed to retrieve file information.";
        return "";
    }

    assemblyInfo += "\r\nFile Information:\r\n" + fileInformation;
    return assemblyInfo;
}


QString GetFileInformation(QByteArray fileData, const QString& moduleFileName) {
    QString sResult;

    DiE_Script::OPTIONS options = {};
    options.bIsDeepScan = true;
    options.bIsVerbose = true;
    options.bIsRecursiveScan = false;

    DiE_Script dieScript;

    QString defaultDatabase = "$app/db";
    if (fileData.isEmpty()) {
        fileData = defaultDatabase.toUtf8();
    }

    dieScript.loadDatabase(defaultDatabase, true);

    DiE_Script::SCAN_RESULT scanResult = dieScript.scanFile(moduleFileName, &options);

    QList<XBinary::SCANSTRUCT> listResult = DiE_Script::convert(&(scanResult.listRecords));

    ScanItemModel model(&listResult);

    XBinary::FORMATTYPE formatType = XBinary::FORMATTYPE_TEXT;

    sResult = model.toString(formatType);

    return sResult;
}


