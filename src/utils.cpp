#include "utils.h"

#include <cmath>
#include <stdexcept>
#include <QDir>
#include <QRegExp>

QString Utils::makeRelative(QString path, QString relativeTo)
{
    QStringList pathParts = path.split(QDir::separator());
    QStringList relativeToParts = relativeTo.split(QDir::separator());

    pathParts.removeAll("");
    relativeToParts.removeAll("");

    int commonRoot = -1;

    for (int i = 0; i < std::min(pathParts.count(), relativeToParts.count()); i++) {
        if (pathParts.at(i) == relativeToParts.at(i))
            commonRoot = i;
        else
            break;
    }

#ifdef Q_OS_WIN32
    if (commonRoot == -1)
        throw std::runtime_error("The given paths have no common root");
#endif

    int diff = relativeToParts.count() - (commonRoot+1) * 2 - 1;

    if (diff > 0) {
        for (int i = 0; i < diff; i++)
            pathParts.push_front("..");
    } else {
        for (int i = 0; i > diff; i--)
            pathParts.pop_front();
    }

    for (int i = std::max(0, diff); i < commonRoot+1 + diff; i++) {
        pathParts[i] = "..";
    }

    return pathParts.join(QDir::separator());
}

QString Utils::makeAbsolute(QString path, QString relativeTo)
{
#ifdef Q_OS_WIN32
    const QRegExp driveLetter("^\\w:\\\\");
    if (path.contains(driveLetter))
        return path;
#else
    if (path.startsWith(QDir::separator()))
        return path;
#endif

    QStringList pathParts = path.split(QDir::separator());
    QStringList relativeToParts = relativeTo.split(QDir::separator());

    pathParts.removeAll("");
    relativeToParts.removeAll("");

    relativeToParts.pop_back();

    QStringList combined = relativeToParts + pathParts;
    int i = combined.indexOf("..");
    while (i >= 0) {
        if (i == 0)
            throw std::runtime_error("Invalid path");

        combined.removeAt(i - 1); // before ".."
        combined.removeAt(i - 1); // ".."

        i = combined.indexOf("..");
    }

    QString out = combined.join(QDir::separator());
#ifdef Q_OS_UNIX
    out.prepend(QDir::separator());
#endif
    return out;
}
