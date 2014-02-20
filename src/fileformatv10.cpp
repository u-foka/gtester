#include "fileformatv10.h"

#include <stdexcept>

#include "testitemroot.h"
#include "testitemexecutable.h"
#include "testitemcase.h"
#include "testitem.h"

#include "utils.h"

FileFormatV10::FileFormatV10(QFile &file)
    : _file(file), _fileInfo(file.fileName()), _writer(&file), _reader(&file)
{
    _writer.setAutoFormatting(true);
}

FileFormatV10::~FileFormatV10()
{
}

void FileFormatV10::SaveItem(TestItemRoot *item)
{
    _writer.writeStartDocument();
    _writer.writeStartElement("GTester");
    _writer.writeAttribute("version", "1.0");

    item->saveChildren(this);

    _writer.writeEndElement();
    _writer.writeEndDocument();
}

void FileFormatV10::SaveItem(TestItemExecutable *item)
{
    QString path;

    try {
        path = Utils::makeRelative(item->getFileInfo().absoluteFilePath(), _fileInfo.absoluteFilePath());
    } catch (std::runtime_error &) {
        path = item->getFileInfo().absoluteFilePath();
    }

    _writer.writeStartElement("TestExecutable");
    _writer.writeAttribute("path", path);

    item->saveChildren(this);

    _writer.writeEndElement();
}

void FileFormatV10::SaveItem(TestItemCase *item)
{
    _writer.writeStartElement("TestCase");
    _writer.writeAttribute("name", item->getName());

    item->saveChildren(this);

    _writer.writeEndElement();
}

void FileFormatV10::SaveItem(TestItem *item)
{
    _writer.writeStartElement("Test");
    _writer.writeAttribute("name", item->getName());
    _writer.writeAttribute("enabled", item->getEnabled() ? "true" : "false");
    _writer.writeEndElement();
}

void FileFormatV10::ReadItem(TestItemRoot *item)
{
    _reader.readNext();
    if (_reader.atEnd() || ! _reader.isStartDocument())
        throw std::runtime_error("Start Document expected");

    _reader.readNext();
    if (_reader.atEnd() || ! _reader.isStartElement() || _reader.name() != "GTester"
            || _reader.attributes().value("version") != "1.0")
        throw std::runtime_error("Start Element: GTester version=1.0 expected");

    readChildren(item);

    if (! _reader.atEnd() || ! _reader.isEndDocument())
        throw std::runtime_error("End Document expected");
}

void FileFormatV10::ReadItem(TestItemExecutable *item)
{
    item->setFileInfo(QFileInfo(
        Utils::makeAbsolute(_reader.attributes().value("path").toString(),
            _file.fileName())
        ));

    readChildren(item);
}

void FileFormatV10::ReadItem(TestItemCase *item)
{
    item->setName(_reader.attributes().value("name").toString());
    readChildren(item);
}

void FileFormatV10::ReadItem(TestItem *item)
{
    item->setName(_reader.attributes().value("name").toString());
    item->setEnabled(_reader.attributes().value("enabled") == "true" ? true : false);

    // TestItem does not have children, read end element here
    _reader.readNext();
    if (_reader.atEnd() || ! _reader.isEndElement())
        throw std::runtime_error("End element expected");
}

void FileFormatV10::readChildren(TestItemBase *item)
{
    _reader.readNext();
    while (! _reader.atEnd()) {

        if (_reader.isWhitespace()) {
            _reader.readNext();
            continue;
        }

        if (_reader.isEndElement()) {
            _reader.readNext();
            break;
        }

        if (_reader.isStartElement()) {
            TestItemBase *child = constructItem(_reader.name().toString(), item);
            child->read(this);
        } else {
            throw std::runtime_error("Start element expected");
        }

        _reader.readNext();
    }
}

TestItemBase * FileFormatV10::constructItem(QString typeName, TestItemBase *parentItem)
{
    if        (typeName == "TestExecutable") {
        return new TestItemExecutable(parentItem);
    } else if (typeName == "TestCase") {
        return new TestItemCase(parentItem);
    } else if (typeName == "Test") {
        return new TestItem(parentItem);
    } else {
        throw std::runtime_error("Invalid node: " + typeName.toStdString());
    }
}
