#ifndef EXPORTSVMDIALOG_H
#define EXPORTSVMDIALOG_H

#include <QDialog>
#include <QMap>

class QCheckBox;
class QGroupBox;
class QLabel;
class QProgressBar;
class QPushButton;
class QRadioButton;
class QSpinBox;

class DocumentsLoader;
class IDFCalculator;
class MainWindow;
class TFCalculator;
class WordsLoader;

class ExportSVMDialog : public QDialog
{
    Q_OBJECT
public:
    explicit ExportSVMDialog(const QString &words, MainWindow *parent = 0);
    ~ExportSVMDialog();

    static qreal getTFLogNorm(int count);
    static qreal getTFDoubleNorm(int count, int max);

    static qreal getIDF(int count, int max);
    static qreal getIDFSmooth(int count, int max);
    static qreal getIDFP(int count, int max);

    static int findMax(QMap<QString, int> *map);

private slots:
    void onStarted();
    void onPreFinished();
    void onPostFinished();
    void updateProgressBar();
    void onExportFinished();
    void exportLight();
    void exportRank();

    friend class IDFCalculator;
    friend class TFCalculator;
    friend class SVMExporter;
private:
    void loadSettings();
    void saveSettings();

    MainWindow *m_window;

    QRadioButton *m_radioTF1;
    QRadioButton *m_radioTF2;
    QRadioButton *m_radioTF3;
    QRadioButton *m_radioTF4;
    QGroupBox *m_groupTF;

    QLabel *m_labelMultiply;

    QRadioButton *m_radioIDF1;
    QRadioButton *m_radioIDF2;
    QRadioButton *m_radioIDF3;
    QRadioButton *m_radioIDF4;
    QGroupBox *m_groupIDF;

    QCheckBox *m_checkWords;
    QCheckBox *m_checkDocs;
    QCheckBox *m_checkTF;
    QCheckBox *m_checkDF;
    QGroupBox *m_groupLoad;

    QLabel *m_labelDecimals;
    QSpinBox *m_spinDecimals;
    QCheckBox *m_checkEmpty;
    QGroupBox *m_groupOpt;

    QProgressBar *m_bar;

    QPushButton *m_btnLight;
    QPushButton *m_btnRank;

    WordsLoader *m_wl;
    DocumentsLoader *m_dl;
    TFCalculator *m_tfc;
    IDFCalculator *m_idfc;

    int m_wordCount;
    int m_docCount;
    QMap<QString, int> m_mapWordID;
    QMap<int, QString> m_mapIDWord;
    QMap<QString, int> m_mapDocState;
    QMap<QString, QStringList> m_mapDocWords;
    QMap<QString, QMap<QString, int> > m_mapWordCount;
    QMap<QString, int> m_mapDocCount;

    QString m_lastLight;
    QString m_lastRank;
};

#endif // EXPORTSVMDIALOG_H
