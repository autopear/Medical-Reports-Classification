#include <QDomDocument>
#include <QFile>
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

        QDomElement reportElement = doc.createElement("Report");
        reportElement.setAttribute("file", report);
        reportElement.setAttribute("state", 0);

        for (int j=1; j<4; j++)
        {
            QDomElement propElement = doc.createElement("Property");
            propElement.setAttribute("key", QString("property_%1").arg(j));
            propElement.setAttribute("value", j);

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
