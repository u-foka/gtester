#ifndef UTILS_H
#define UTILS_H

#include <QString>

class Utils
{
public:
    static QString makeRelative(QString path, QString relativeTo);
    static QString makeAbsolute(QString path, QString relativeTo);

private:
    Utils();

};

#endif // UTILS_H
