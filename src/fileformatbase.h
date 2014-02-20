#ifndef FILEFORMATBASE_H
#define FILEFORMATBASE_H

class TestItemRoot;
class TestItemExecutable;
class TestItemCase;
class TestItem;

class FileFormatBase
{
public:
    virtual ~FileFormatBase();

    virtual void SaveItem(TestItemRoot *item) =0;
    virtual void SaveItem(TestItemExecutable *item) =0;
    virtual void SaveItem(TestItemCase *item) =0;
    virtual void SaveItem(TestItem *item) =0;

    virtual void ReadItem(TestItemRoot *item) =0;
    virtual void ReadItem(TestItemExecutable *item) =0;
    virtual void ReadItem(TestItemCase *item) =0;
    virtual void ReadItem(TestItem *item) =0;
};

#endif // FILEFORMATBASE_H
