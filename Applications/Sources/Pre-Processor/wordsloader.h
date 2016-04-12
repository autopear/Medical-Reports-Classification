#ifndef WORDSLOADER_H
#define WORDSLOADER_H

#include <QMap>
#include <QThread>

class WordsLoader : public QThread
{
    Q_OBJECT
public:
    explicit WordsLoader(const QString &words,
                         QMap<QString, int> *map1,
                         QMap<int, QString> *map2,
                         QObject *parent = 0);
    ~WordsLoader();

protected:
    void run();

private:
    QString m_words;
    QMap<QString, int> *m_map1;
    QMap<int, QString> *m_map2;
};

#endif // WORDSLOADER_H
