#include <QLibrary>
#include <QDebug>
#include <QFile>
#include <QDir>
#include <QCoreApplication>
#include <QDataStream>
#include <QBuffer>
#include <windows.h>


QString GetFileInformation(const QString& filePath, const QString& pwszDatabase);

class LoadedAssembly {
public:
    // Constructor
    LoadedAssembly(const QString& path) : path(path) {}

    // Copy constructor
    LoadedAssembly(const LoadedAssembly& other) : path(other.path) {}

    // Copy assignment operator
    LoadedAssembly& operator=(const LoadedAssembly& other) {
        if (this != &other) {
            path = other.path;
        }
        return *this;
    }

    // Getter for the path
    const QString& GetPath() const { return path; }

private:
    QString path;
};

QVector<LoadedAssembly> loadedAssemblies;
void LoadAssemblyIntoMemory(const QString& dllPath)
{
    // Clear the vector to show only the currently loaded assembly
    loadedAssemblies.clear();

    // Load the DLL into memory
    QLibrary library(dllPath);
    if (library.load())
    {
        // DLL loaded successfully, create an instance of LoadedAssembly
        LoadedAssembly loadedAssembly(dllPath);

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
    assemblyInfo += moduleFileName + "\r\n";

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
    assemblyInfo += "Number of Sections: " + QString::number(ntHeaders.FileHeader.NumberOfSections) + "\r\n";

    // Access other information in the ntHeaders structure as needed
    qDebug() << "Entry Point RVA:" << QString::number(ntHeaders.OptionalHeader.AddressOfEntryPoint, 16);



    // Get the directory of the executable to construct the absolute database path
    QString exePath = QCoreApplication::applicationDirPath();  // Use applicationDirPath() instead of applicationFilePath()
    QDir dir(exePath);
    QString absoluteDbPath = QDir::toNativeSeparators(dir.absoluteFilePath("db"));

    // Debugging: Print the constructed absolute path
    qDebug() << "Constructed Absolute Database Path: " << absoluteDbPath;

    // Check if the "db" directory exists
    if (!QDir(absoluteDbPath).exists()) {
        qDebug() << "Error: The 'db' directory does not exist. Current working directory: " << QDir::currentPath();
        // Handle the error or return an appropriate value
        return "";
    }

    QString fileInformation = GetFileInformation(moduleFileName, absoluteDbPath);
    if (fileInformation.isEmpty()) {
        qDebug() << "Error: Failed to retrieve file information.";
        // Handle the error or return an appropriate value
        return "";
    }

    assemblyInfo += "\r\nFile Information:\r\n" + fileInformation;
    return assemblyInfo;
}

extern "C" __declspec(dllimport) int __cdecl DIE_VB_ScanFile(const wchar_t* pwszFileName, int nFlags, const wchar_t* pwszDatabase, wchar_t* pwszBuffer, int nBufferSize);
QString GetFileInformation(const QString& filePath, const QString& pwszDatabase)
{
    try {
        const int nBufferSize = 10000;
        QString sBuffer;
        sBuffer.fill(' ', nBufferSize);  // Allocate buffer

        // Use Qt functions for file operations
        QFile file(filePath);
        if (!file.open(QIODevice::ReadOnly)) {
            qDebug() << "Failed to open file:" << file.errorString();
            return "Failed to open file: " + file.errorString();
        }

        // Convert QChar* to wchar_t*
        wchar_t* wcharBuffer = reinterpret_cast<wchar_t*>(sBuffer.data());

        // Use DIE_VB_ScanFile to get the file information
        int nResult = DIE_VB_ScanFile(file.fileName().toStdWString().c_str(), 0, pwszDatabase.toStdWString().c_str(), wcharBuffer, nBufferSize - 1);
        qDebug() << "Scan result:" << nResult;

        QString result;
        if (nResult > 0) {
            // Trim the buffer to the actual length of the result
            sBuffer.resize(nResult);
            result = sBuffer;
        }
        else {
            qDebug() << "Scan failed or no results. Error Code:" << nResult;
            result = "Scan failed or no results. Error Code: " + QString::number(nResult);
        }

        // Replace all '\n' with '\r\n' in result
        result.replace("\n", "\r\n");


        qDebug() << "File Information:" << result;  // Add this line for debugging

        return result;
    }
    catch (const QFileDevice::FileError& fileError) {
        qDebug() << "QFileDevice error:" << fileError;
        return "QFileDevice error: " + QString::number(fileError);
    }

}
