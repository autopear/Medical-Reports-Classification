#include <QRadioButton>
#include "exportsvmdialog.h"
#include "idfcalculator.h"

IDFCalculator::IDFCalculator(ExportSVMDialog *dialog, QObject *parent) :
    QThread(parent),
    m_dialog(dialog)
{
}

IDFCalculator::~IDFCalculator()
{
}

void IDFCalculator::run()
{
    foreach (QString report, m_dialog->m_mapDocWords.keys())
    {
        QStringList words = m_dialog->m_mapDocWords.value(report);
        words.removeDuplicates();

        foreach (QString word, words)
        {
            int count = m_dialog->m_mapDocCount.value(word, 0);
            count++;
            m_dialog->m_mapDocCount.insert(word, count);
        }
    }
}
