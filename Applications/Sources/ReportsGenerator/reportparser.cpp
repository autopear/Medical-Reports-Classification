#include <QDomDocument>
#include <QFile>
#include <QDir>
#include "reportparser.h"

ReportParser::ReportParser(const QStringList &reports,
                           const QMap<QString, QString> &tags,
                           const QString &dir,
                           const QString &xml,
                           QObject *parent) :
    QThread(parent)
{
    foreach (QString report, reports)
        m_reports.append(report.replace("\\", "/"));
    m_tags = tags;
    m_dir = dir;
    m_xml = xml;
}

ReportParser::~ReportParser()
{
}

const static QStringList secondWords = QStringList() << "see" << "seen"
                                                     << "report" << "reported"
                                                     << "identify" << "identified"
                                                     << "observe" << "observed"
                                                     << "monitor" << "monitored";

void ReportParser::run()
{
    QDomDocument doc;
    doc.appendChild(doc.createProcessingInstruction("xml", "version=\"1.0\" encoding=\"UTF-8\""));

    QDomElement root = doc.createElement("Reports");
    doc.appendChild(root);

    for (int i=0; i<m_reports.size(); i++)
    {
        QString report = m_reports.at(i);
        QString path = QString("%1%2").arg(m_dir).arg(report);

        QFile *reportFile = new QFile(path);
        if (!reportFile->open(QFile::ReadOnly | QFile::Text))
        {
            qWarning(QString("Cannot open %1: %2").arg(QDir::toNativeSeparators(path)).arg(reportFile->errorString()).toUtf8());
            continue;
        }

        QMap<QString, QString> props;
        QString tag = QString(), content = QString();
        bool seizures = true;
        while (!reportFile->atEnd())
        {
            QString line = QString::fromUtf8(reportFile->readLine()).remove("\r").remove("\n").trimmed();
            if (line.isEmpty())
                continue;

            int tagIndex = line.indexOf(":");
            if (tagIndex > -1)
            {
                if (!tag.isEmpty())
                {
                    if (!content.isEmpty())
                    {
                        QString key = formatTag(tag);
                        if (m_tags.keys().contains(key))
                        {
                            props.insert(key, content);
                            if (key.compare("seizures") == 0 || key.compare("seizure") == 0)
                            {
                                QStringList tmp = formatValue(content).split(" ", QString::SkipEmptyParts);
                                if (tmp.size() == 1)
                                {
                                    QString v1 = tmp.first();
                                    if (v1.compare("no") == 0 || v1.compare("none") == 0)
                                        seizures = false;
                                }
                                else if (tmp.size() == 2)
                                {
                                    QString v1 = tmp.first();
                                    QString v2 = tmp.last();
                                    if ((v1.compare("no") == 0 || v1.compare("none") == 0) &&
                                            secondWords.contains(v2))
                                        seizures = false;
                                }
                                else
                                {
                                    QString singleSpaced = tmp.join(" ");
                                    if (singleSpaced.compare("none seen during the entire recording") == 0)
                                        seizures = false;
                                }
                            }
                        }
                        else
                            props.insert(key, tag.append(": ").append(content));

                        content.clear();
                    }

                    tag.clear();
                }

                tag = line.left(tagIndex);

                line = line.mid(tagIndex + 1).trimmed();

                if (!line.isEmpty())
                {
                    QString copy = line;
                    copy.remove("-").remove(" ");
                    if (!copy.isEmpty())
                    {
                        if (content.isEmpty())
                            content = line;
                        else
                            content = content.append("\n").append(line);
                    }
                }
            }
            else
            {
                QString copy = line;
                copy.remove("-").remove(" ");
                if (!copy.isEmpty())
                {
                    if (content.isEmpty())
                        content = line;
                    else
                        content = content.append("\n").append(line);
                }
            }
        }

        reportFile->close();
        delete reportFile;

        if (!tag.isEmpty() && !content.isEmpty())
        {
            tag = formatTag(tag);
            tag = m_tags.value(tag, tag);
            props.insert(tag, content);
        }

        if (props.isEmpty())
        {
            qWarning(QString("Report %1 is empty.").arg(QDir::toNativeSeparators(path)).toUtf8());
            continue;
        }

        QDomElement reportElement = doc.createElement("Report");
        reportElement.setAttribute("file", report);
        reportElement.setAttribute("state", seizures ? 0 : -1);

        foreach (QString key, props.keys())
        {
            QDomElement propElement = doc.createElement("Property");
            propElement.setAttribute("key", key);
            propElement.setAttribute("value", props.value(key));

            reportElement.appendChild(propElement);
        }

        root.appendChild(reportElement);

        emit reportParsed(i);
    }

    QFile *xml = new QFile(m_xml);
    if (!xml->open(QFile::WriteOnly | QFile::Truncate | QFile::Text))
    {
        emit errorOccurred(xml->errorString());
        delete xml;
        return;
    }

    xml->write(doc.toByteArray());
    xml->close();
    delete xml;
}

QString ReportParser::formatTag(const QString &tag)
{
    QString ret;
    for (int i=0; i<tag.size(); i++)
    {
        QChar c = tag.at(i).toLower();
        if ((c >= 'a' && c <= 'z') || (c >= '0' && c <= '9'))
            ret = ret.append(c);
    }
    return ret;
}

QString ReportParser::formatValue(const QString &value)
{
    QString replaced = value.toLower().replace('\t', ' ');
    QString ret;
    for (int i=0; i<replaced.size(); i++)
    {
        QChar c = replaced.at(i);
        if ((c >= 'a' && c <= 'z') || (c >= '0' && c <= '9') || c == ' ')
            ret = ret.append(c);
    }
    return ret;
}
