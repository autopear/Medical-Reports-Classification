#include <QDir>
#include <QFile>
#include <QFileInfo>
#include "stemmer.h"
#include "reportsparser.h"

ReportsParser::ReportsParser(const QString &dir, QObject *parent) :
    QThread(parent),
    m_dir(dir)
{
    if (!m_dir.endsWith("/"))
        m_dir = m_dir.append("/");
}

ReportsParser::~ReportsParser()
{

}

void ReportsParser::run()
{
    QStringList reports;
    findAllReports(m_dir, m_dir, &reports);
    reports.sort();

    emit reportsListed(reports);

    if (reports.isEmpty())
        return;

    foreach (QString report, reports)
    {
        QString filePath = QString("%1%2").arg(m_dir).arg(report);

        QFile *file = new QFile(filePath);
        file->open(QFile::ReadOnly);
        QString content = QString::fromUtf8(file->readAll());
        file->close();
        delete file;

        emit reportStemmed(report, Stemmer::stemSentence(content));
    }
}

void ReportsParser::findAllReports(const QString &root, const QString &dir, QStringList *reports)
{
    foreach (QFileInfo info, QDir(dir).entryInfoList(QDir::Dirs | QDir::Files | QDir::NoDotAndDotDot))
    {
        if (info.isDir())
            findAllReports(root, info.absoluteFilePath(), reports);
        else if (info.isFile() && info.fileName().endsWith(".txt", Qt::CaseInsensitive))
        {
            QString filePath = info.absoluteFilePath();
            reports->append(filePath.remove(0, root.size()));
        }
        else
        {
        }
    }
}
