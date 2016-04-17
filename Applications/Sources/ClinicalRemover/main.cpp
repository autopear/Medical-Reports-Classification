#include <QCoreApplication>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QRegExp>
#include <QStringList>

static void findAllReports(const QString &dirPath, QStringList *reports)
{
    QDir dir(dirPath);
    foreach (QFileInfo info, dir.entryInfoList(QDir::Files | QDir::Dirs | QDir::NoDotAndDotDot))
    {
        if (info.isDir())
            findAllReports(info.absoluteFilePath(), reports);
        else if (info.isFile() && info.fileName().endsWith(".txt", Qt::CaseInsensitive))
            reports->append(info.absoluteFilePath());
        else
            continue;
    }
}

static QString formatTag(const QString &tag)
{
    QString ret;
    for (int i=0; i<tag.size(); i++)
    {
        QChar c = tag.at(i);
        if (c >= 'a' && c <= 'z')
            ret = ret.append(c);
    }
    return ret;
}

static QString extractTag(const QString &line)
{
    int idx = line.indexOf(":");
    if (idx > 1)
        return formatTag(line.left(idx).toLower());
    else
        return QString();
}

const static QStringList tags = QStringList() << "briefhistory"
                                              << "cinicalhistory"
                                              << "cliinicalidstory"
                                              << "clincalhistory"
                                              << "clinicalhistory"
                                              << "clinicalidstory"
                                              << "cllnicalhistory"
                                              << "clnicalhistory"
                                              << "clticalhistory"
                                              << "history"
                                              << "idstory"
                                              << "medicalhistory"
                                              << "pasthistory"
                                              << "pastmedicalhistory"
                                              << "pleaseaddtothehistory";

static QString trimReport(const QStringList &lines)
{
    QStringList list;
    bool skip = false;
    foreach (QString line, lines)
    {
        QString tag = extractTag(line);
        if (skip)
        {
            if (!tag.isEmpty() && !tags.contains(tag))
            {
                skip = false;
                list.append(line);
            }
        }
        else
        {
            if (tags.contains(tag))
                skip = true;
            else
                list.append(line);
        }
    }

    return list.join("\n");
}

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    if (argc != 3)
    {
        QFileInfo binInfo(QCoreApplication::applicationFilePath());
        printf(QString("Usage: %1 In_Dir Out_Dir\n").arg(binInfo.baseName()).toUtf8().data());
        return -1;
    }

    QStringList reports;
    findAllReports(argv[1], &reports);
    if (reports.isEmpty())
    {
        printf("No report can be found.\n");
        return 0;
    }

    reports.sort();

    QString dirPrefix = QFileInfo(argv[1]).absoluteFilePath();
    if (!dirPrefix.endsWith("/"))
        dirPrefix = dirPrefix.append('/');

    foreach (QString report, reports)
    {
        QFileInfo inInfo(report);

        printf(QString("Trimming %1...\n").arg(QDir::toNativeSeparators(inInfo.absoluteFilePath())).toUtf8().data());

        QFile *in = new QFile(report);
        if (!in->open(QFile::ReadOnly))
        {
            printf(QString("Cannot open %1...\n").arg(QDir::toNativeSeparators(inInfo.absoluteFilePath())).toUtf8().data());
            delete in;
            continue;
        }

        QStringList lines = QString::fromUtf8(in->readAll()).split(QRegExp("[\r\n]"), QString::SkipEmptyParts);
        in->close();
        delete in;

        QString trimmed = trimReport(lines);

        QString base = inInfo.absoluteFilePath().remove(0, dirPrefix.size());
        QString outDir;
        if (base.contains("/"))
            outDir = QString("%1/%2/").arg(argv[2]).arg(base.left(base.lastIndexOf("/"))).replace("\\", "/").replace("//", "/");
        else
            outDir = QString("%1/").arg(argv[2]).replace("\\", "/").replace("//", "/");

        QDir od(outDir);
        if (!od.exists())
        {
            if (!od.mkpath(outDir))
            {
                printf(QString("Cannot create directory %1...\n").arg(QDir::toNativeSeparators(outDir)).toUtf8().data());
                continue;
            }
        }

        QString outPath = QString("%1%2").arg(outDir).arg(inInfo.fileName());

        QFile *out = new QFile(outPath);
        if (!out->open(QFile::WriteOnly))
        {
            printf(QString("Cannot open %1...\n").arg(QDir::toNativeSeparators(outPath)).toUtf8().data());
            delete out;
            continue;
        }

        out->write(trimReport(lines).toUtf8());

        out->close();
        delete out;
    }

    printf("Done.\n");

    return a.exec();
}
