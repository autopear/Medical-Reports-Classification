#include <QCoreApplication>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QStringList>

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    if (argc != 4)
    {
        QFileInfo binInfo(QCoreApplication::applicationFilePath());
        printf(QString("Usage: %1 Source_List In_List Out_List\n").arg(binInfo.baseName()).toUtf8().data());
        return -1;
    }

    QFile *source = new QFile(QString(argv[1]));
    source->open(QFile::ReadOnly);
    QStringList sourceList = QString::fromUtf8(source->readAll()).split(QRegExp("[\r\n]"), QString::SkipEmptyParts);
    source->close();
    delete source;

    QFile *in = new QFile(QString(argv[2]));
    in->open(QFile::ReadOnly);
    QStringList inList = QString::fromUtf8(in->readAll()).split(QRegExp("[\r\n]"), QString::SkipEmptyParts);
    in->close();
    delete in;

    foreach (QString item, inList)
        sourceList.removeAll(item);

    QFile *out = new QFile(QString(argv[3]));
    out->open(QFile::WriteOnly);
    out->write(sourceList.join("\n").toUtf8());
    out->close();
    delete out;

    return 0;
}
