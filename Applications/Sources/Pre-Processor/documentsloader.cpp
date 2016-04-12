#include <QCheckBox>
#include <QListWidget>
#include "mainwindow.h"
#include "documentsloader.h"

DocumentsLoader::DocumentsLoader(MainWindow *window,
                                 QMap<QString, QStringList> *map1,
                                 QMap<QString, int> *map2,
                                 QObject *parent) :
    QThread(parent),
    m_window(window),
    m_map1(map1),
    m_map2(map2)
{
}

DocumentsLoader::~DocumentsLoader()
{
}

void DocumentsLoader::run()
{
    int index = 1;

    QMap<QString, int> tmp;
    for (int i=0; i<m_window->m_listFiles->count(); i++)
    {
        QListWidgetItem *item = m_window->m_listFiles->item(i);
        switch (item->checkState())
        {
        case Qt::Checked:
            tmp.insert(item->text().replace("\\", "/"), 1);
            break;
        case Qt::Unchecked:
            tmp.insert(item->text().replace("\\", "/"), -1);
            break;
        default:
            tmp.insert(item->text().replace("\\", "/"), 0);
            break;
        }
    }

    if (m_window->m_checkUniqueReport->isChecked())
    {
        QStringList stemsList;
        foreach (QString report, m_window->m_map.keys())
        {
            QStringList stems = m_window->m_map.value(report);

            if (m_window->m_checkSortWords->isChecked())
                stems.sort();
            if (m_window->m_checkUniqueWords->isChecked())
                stems.removeDuplicates();

            QString stemsStr = stems.join(" ");
            if (!stemsList.contains(stemsStr))
            {
                stemsList.append(stemsStr);
                m_map1->insert(report, stems);
                m_map2->insert(report, tmp.value(report));
                index++;
            }
        }
    }
    else
    {
        foreach (QString report, m_window->m_map.keys())
        {
            m_map1->insert(report, m_window->m_map.value(report));
            m_map2->insert(report, tmp.value(report));
            index++;
        }
    }
}
