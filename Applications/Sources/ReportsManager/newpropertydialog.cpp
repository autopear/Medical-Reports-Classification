#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>
#include <QVBoxLayout>
#include "newpropertydialog.h"

NewPropertyDialog::NewPropertyDialog(QWidget *parent) :
    QDialog(parent)
{
    m_label = new QLabel(tr("New Property:"), this);

    m_edit = new QLineEdit(tr("property"), this);
    m_edit->selectAll();
    m_edit->setFixedWidth(m_edit->sizeHint().width() * 1.5);

    m_btnOk = new QPushButton(tr("&OK"), this);
    m_btnOk->setAutoDefault(false);
    m_btnOk->setDefault(true);
    connect(m_btnOk, SIGNAL(clicked(bool)),
            this, SLOT(onOk()));

    m_btnCancel = new QPushButton(tr("&Cancel"), this);
    m_btnCancel->setAutoDefault(false);
    connect(m_btnCancel, SIGNAL(clicked(bool)),
            this, SLOT(reject()));

    int w = qMax(m_btnOk->sizeHint().width(), m_btnCancel->sizeHint().width());
    m_btnOk->setFixedWidth(w);
    m_btnCancel->setFixedWidth(w);

    QHBoxLayout *btnsLayout = new QHBoxLayout();
    btnsLayout->setContentsMargins(0, 0, 0, 0);
    btnsLayout->addStretch();
    btnsLayout->addWidget(m_btnOk);
    btnsLayout->addStretch();
    btnsLayout->addWidget(m_btnCancel);
    btnsLayout->addStretch();

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addWidget(m_label);
    layout->addWidget(m_edit);
    layout->addLayout(btnsLayout);
    setLayout(layout);

    setWindowTitle(tr("New Property"));
    setWindowFlags(Qt::Dialog | Qt::WindowTitleHint | Qt::WindowCloseButtonHint | Qt::MSWindowsFixedSizeDialogHint);
    setFixedSize(sizeHint());
}

NewPropertyDialog::~NewPropertyDialog()
{
    delete m_label;
    delete m_edit;
    delete m_btnOk;
    delete m_btnCancel;
}

QString NewPropertyDialog::newProperty() const
{
    return m_edit->text().trimmed().toLower();
}

void NewPropertyDialog::onOk()
{
    if (m_edit->text().trimmed().isEmpty())
    {
        QMessageBox::warning(this,
                             tr("Empty Property"),
                             tr("The property must not be empty."));
        m_edit->setFocus();
        return;
    }

    accept();
}
