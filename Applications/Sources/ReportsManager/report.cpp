#include "report.h"

Report::Report() :
    QMap<QString, QString>(),
    m_report(QString()),
    m_state(Report::Unknown)
{
}

Report::Report(const QString &report, Report::State state) :
    QMap<QString, QString>(),
    m_report(report),
    m_state(state)
{
}

Report::~Report()
{
}

QString Report::report() const
{
    return m_report;
}

void Report::setReport(const QString &report)
{
    m_report = report;
}

Report::State Report::patientState() const
{
    return m_state;
}

void Report::setPatientState(Report::State state)
{
    m_state = state;
}

QDomElement Report::toXml(QDomDocument doc)
{
    QDomElement element = doc.createElement("Report");
    if (!m_report.isEmpty())
        element.setAttribute("file", m_report);
    element.setAttribute("state", (int)m_state);

    foreach (QString prop, keys())
    {
        QDomElement child = doc.createElement("Property");
        child.setAttribute("key", prop);
        child.setAttribute("value", value(prop));

        element.appendChild(child);
    }

    return element;
}
