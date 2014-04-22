#ifndef IPCMESSAGE_H
#define IPCMESSAGE_H

#include <QString>
#include <QMap>
#include <QByteArray>

class IpcMessage
{
public:
    enum MessageType {
        Activate,
        OpenDocument
    };
    typedef QMap<QString, QString> ValueList;

    IpcMessage();
    virtual ~IpcMessage();

    MessageType Type() const;
    MessageType Type(MessageType newType);
    const ValueList & Values() const;
    ValueList & Values();

    void Serialize(QByteArray &ba) const;
    void Parse(const QByteArray &ba);

private:
    MessageType _type;
    ValueList _values;

};

#endif // IPCMESSAGE_H
