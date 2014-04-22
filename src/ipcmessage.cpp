#include "ipcmessage.h"

#include <QDataStream>

IpcMessage::IpcMessage()
    : _type(Activate), _values()
{
}

IpcMessage::~IpcMessage()
{
}

IpcMessage::MessageType IpcMessage::Type() const
{
    return _type;
}

IpcMessage::MessageType IpcMessage::Type(MessageType newType)
{
    _type = newType;
    return _type;
}

const IpcMessage::ValueList & IpcMessage::Values() const
{
    return _values;
}

IpcMessage::ValueList & IpcMessage::Values()
{
    return _values;
}

void IpcMessage::Serialize(QByteArray &ba) const
{
    QDataStream s(&ba, QIODevice::WriteOnly);

    int intType = _type;
    s << intType;
    s << _values.count();

    for (ValueList::ConstIterator i = _values.constBegin(); i != _values.constEnd(); i++) {
        s << i.key();
        s << i.value();
    }
}

void IpcMessage::Parse(const QByteArray &ba)
{
    _values.clear();

    QDataStream s(ba);
    int intType;
    int count;

    s >> intType;
    _type = static_cast<MessageType>(intType);
    s >> count;
    for (int i = 0; i < count; i++) {
        QString key;
        QString value;
        s >> key >> value;
        _values.insert(key, value);
    }
}
