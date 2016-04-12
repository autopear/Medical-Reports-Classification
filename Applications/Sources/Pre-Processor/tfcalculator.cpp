#include <QRadioButton>
#include "exportsvmdialog.h"
#include "tfcalculator.h"

TFCalculator::TFCalculator(ExportSVMDialog *dialog, QObject *parent) :
    QThread(parent),
    m_dialog(dialog)
{
}

TFCalculator::~TFCalculator()
{
}

void TFCalculator::run()
{
    foreach (QString report, m_dialog->m_mapDocWords.keys())
    {
        QStringList words = m_dialog->m_mapDocWords.value(report);

        QMap<QString, int> map;
        foreach (QString word, words)
        {
            int count = map.value(word, 0);
            count++;
            map.insert(word, count);
        }
        m_dialog->m_mapWordCount.insert(report, map);
    }
}
