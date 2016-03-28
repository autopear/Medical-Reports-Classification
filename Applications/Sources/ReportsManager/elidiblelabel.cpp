#include <QFont>
#include <QFontMetrics>
#include <QResizeEvent>
#include "elidiblelabel.h"

ElidibleLabel::ElidibleLabel(QWidget *parent) :
    QLabel(parent)
{
    setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Preferred);
}

ElidibleLabel::~ElidibleLabel()
{
}

QString ElidibleLabel::displayText() const
{
    return QLabel::text();
}

QString ElidibleLabel::text() const
{
    return toolTip();
}

void ElidibleLabel::setText(const QString &text)
{
    if (toolTip().compare(text) != 0)
    {
        setToolTip(text);
        updateDisplayText(width());
    }
}

void ElidibleLabel::resizeEvent(QResizeEvent *event)
{
    updateDisplayText(event->size().width());
    QLabel::resizeEvent(event);
}

void ElidibleLabel::updateDisplayText(int w)
{
    QLabel::setText(fontMetrics().elidedText(toolTip(), Qt::ElideMiddle, w));
}
