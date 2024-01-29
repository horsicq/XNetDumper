// LoadAssembly.cpp
#include "LoadAssembly.h"
#include <QLibrary>
#include <QDebug>
#include <QFile>
#include <QDir>
#include <QCoreApplication>
#include <QDataStream>
#include <QBuffer>
#include <windows.h>
#include "die_script.h"
#include <QList>

QVector<LoadAssembly> loadedAssemblies;
void LoadAssemblyIntoMemory(const QString& dllPath)
{
    // Clear the vector to show only the currently loaded assembly
    loadedAssemblies.clear();

    // Load the DLL into memory
    QLibrary library(dllPath);
    if (library.load())
    {
        // DLL loaded successfully, create an instance of LoadedAssembly
        LoadAssembly loadedAssembly(dllPath);

        // Add it to the vector
        loadedAssemblies.push_back(loadedAssembly);
    }
    else
    {
        // Failed to load the DLL, handle the error
        qDebug() << "Failed to load DLL: " << library.errorString();
    }
}

QString GetAssemblyInfo(QLibrary* library) {
    QString assemblyInfo;

    // Get the module file name with consistent path separators
    QString moduleFileName = QDir::toNativeSeparators(library->fileName());

    // Extract only the filename from the full file path
    QFileInfo fileInfo(moduleFileName);
    QString moduleName = fileInfo.fileName();

    // Append the filename to the assemblyInfo string
    assemblyInfo += moduleName + "\r\n";

    qDebug() << "File Path from Memory: " << moduleFileName;

    // Get the handle to the loaded module (DLL)
    HMODULE hModule = GetModuleHandleW(reinterpret_cast<LPCWSTR>(moduleFileName.utf16()));
    if (!hModule) {
        qWarning() << "Failed to get module handle for" << moduleFileName;
        return "";
    }

    // Get the base address of the loaded module
    quintptr moduleBase = reinterpret_cast<quintptr>(hModule);

    // Read the DOS header to get the PE signature offset
    IMAGE_DOS_HEADER dosHeader;
    SIZE_T bytesRead = 0;

    if (!ReadProcessMemory(GetCurrentProcess(), reinterpret_cast<LPCVOID>(moduleBase),
                           &dosHeader, sizeof(dosHeader), &bytesRead) || bytesRead != sizeof(dosHeader)) {
        qWarning() << "Failed to read DOS header for" << moduleFileName;
        return "";
    }

    // Check for valid MZ header
    if (dosHeader.e_magic != IMAGE_DOS_SIGNATURE) {
        qWarning() << "Invalid MZ header for" << moduleFileName;
        return "";
    }

    // Read the PE signature offset
    quintptr peSignatureOffset = moduleBase + dosHeader.e_lfanew;

    // Read the PE signature
    IMAGE_NT_HEADERS ntHeaders;
    if (!ReadProcessMemory(GetCurrentProcess(), reinterpret_cast<LPCVOID>(peSignatureOffset),
                           &ntHeaders, sizeof(ntHeaders), &bytesRead) || bytesRead != sizeof(ntHeaders)) {
        qWarning() << "Failed to read NT headers for" << moduleFileName;
        return "";
    }

    // Check for valid PE signature
    if (ntHeaders.Signature != IMAGE_NT_SIGNATURE) {
        qWarning() << "Invalid PE signature for" << moduleFileName;
        return "";
    }

    // Access other information in the ntHeaders structure as needed
    quint32 entryPointRVA = ntHeaders.OptionalHeader.AddressOfEntryPoint;

    assemblyInfo += "Entry Point RVA: 0x" + QString::number(entryPointRVA, 16) + "\r\n";

    // Display more information about the PE headers
    assemblyInfo += "Machine: 0x" + QString::number(ntHeaders.FileHeader.Machine, 16) + "\r\n";

    // Access other information in the ntHeaders structure as needed
    qDebug() << "Entry Point RVA:" << QString::number(ntHeaders.OptionalHeader.AddressOfEntryPoint, 16);

    QByteArray fileData; // Assuming you have loaded the file into this QByteArray

    QString fileInformation = GetFileInformation(fileData, moduleFileName);
    if (fileInformation.isEmpty()) {
        qDebug() << "Error: Failed to retrieve file information.";
        // Handle the error or return an appropriate value
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

    dieScript.loadDatabase(defaultDatabase, true); // Load the database

    DiE_Script::SCAN_RESULT scanResult = dieScript.scanFile(moduleFileName, &options);

    QList<XBinary::SCANSTRUCT> listResult = DiE_Script::convert(&(scanResult.listRecords));


    ScanItemModel model(&listResult);

    XBinary::FORMATTYPE formatType = XBinary::FORMATTYPE_TEXT;

    sResult = model.toString(formatType);

    return sResult;
}
