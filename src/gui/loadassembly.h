// LoadAssembly.h

#ifndef LOADASSEMBLY_H
#define LOADASSEMBLY_H

#include "qnetworkreply.h"
#include <QLibrary>
#include <QObject>
#include <windows.h>
#include "die_script.h"

class LoadAssembly : public QObject
{
    Q_OBJECT

public:

    LoadAssembly(const QString& path) : path(path) {}

    LoadAssembly(const LoadAssembly& other) : path(other.path) {}
    LoadAssembly& operator=(const LoadAssembly& other) {
        if (this != &other) {
            path = other.path;
        }
        return *this;
    }
    const QString& GetPath() const { return path; }

private:
    QString path;



signals:
    void _removeResultSignal(const QString& result);
};
QString GetFileInformation(QByteArray fileData, const QString& moduleFileName);
QString GetAssemblyInfo(QLibrary* library);
#endif // LOADASSEMBLY_H
