#include <QCoreApplication>
#include <QFile>
#include <QFileInfo>
#include <QDateTime>
#include <QDebug>

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    if (argc != 4)
    {
        QFileInfo binInfo(QCoreApplication::applicationFilePath());
        printf(QString("Usage: %1 Num_Files In_List Out_List\n").arg(binInfo.baseName()).toUtf8().data());
        return -1;
    }

    bool isInt;
    int num = QString(argv[1]).toInt(&isInt, 10);
    if (!isInt || num < 1)
    {
        printf("Please input a valid number");
        return -1;
    }

    QStringList inFiles;
    QStringList outFiles;

    QFile *in = new QFile(QString(argv[2]));
    if (!in->open(QFile::ReadOnly))
    {
        printf(QString("Cannot open %1.\n").arg(QFileInfo(in->fileName()).absoluteFilePath()).toUtf8().data());
        delete in;
        return -1;
    }
    while (!in->atEnd())
    {
        QString line = QString::fromUtf8(in->readLine()).remove("\r").remove("\n").trimmed();
        if (!line.isEmpty())
            inFiles.append(line);
    }
    in->close();
    delete in;

    int inSize = inFiles.size();
    int max = qMin(num, inSize);
    QList<int> rands;

    qsrand(QDateTime::currentMSecsSinceEpoch());

    for (int i=0; i<max; i++)
    {
        while (true)
        {
            int idx = qrand() % inSize;
            if (!rands.contains(idx))
            {
                outFiles.append(inFiles.at(idx));
                rands.append(idx);
                break;
            }
        }
    }

    outFiles.sort();

    QFile *out = new QFile(QString(argv[3]));
    if (!out->open(QFile::WriteOnly))
    {
        printf(QString("Cannot open %1.\n").arg(QFileInfo(out->fileName()).absoluteFilePath()).toUtf8().data());
        delete out;
        return -1;
    }
    out->write(outFiles.join("\n").toUtf8());
    out->close();
    delete out;

    printf("Done.\n");

    return a.exec();
}
