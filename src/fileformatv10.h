#ifndef FILEFORMATV10_H
#define FILEFORMATV10_H

#include <QFile>
#include <QFileInfo>
#include <QXmlStreamWriter>
#include <QXmlStreamReader>

#include "fileformatbase.h"

class TestItemBase;

class FileFormatV10 : public FileFormatBase
{
public:
    FileFormatV10(QFile &file);
    virtual ~FileFormatV10();

    virtual void SaveItem(TestItemRoot *item);
    virtual void SaveItem(TestItemExecutable *item);
    virtual void SaveItem(TestItemCase *item);
    virtual void SaveItem(TestItem *item);

    virtual void ReadItem(TestItemRoot *item);
    virtual void ReadItem(TestItemExecutable *item);
    virtual void ReadItem(TestItemCase *item);
    virtual void ReadItem(TestItem *item);

private:
    QFile &_file;
    QFileInfo _fileInfo;
    QXmlStreamWriter _writer;
    QXmlStreamReader _reader;

    void readChildren(TestItemBase *item);
    TestItemBase *constructItem(QString typeName, TestItemBase *parentItem);

};

#endif // FILEFORMATV10_H
