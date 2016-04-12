#ifndef DOCUMENTSLOADER_H
#define DOCUMENTSLOADER_H

#include <QMap>
#include <QThread>

class MainWindow;

class DocumentsLoader : public QThread
{
    Q_OBJECT
public:
    explicit DocumentsLoader(MainWindow *window,
                             QMap<QString, QStringList> *map1,
                             QMap<QString, int> *map2,
                             QObject *parent = 0);
    ~DocumentsLoader();

protected:
    void run();

private:
    MainWindow *m_window;
    QMap<QString, QStringList> *m_map1;
    QMap<QString, int> *m_map2;
};

#endif // DOCUMENTSLOADER_H
