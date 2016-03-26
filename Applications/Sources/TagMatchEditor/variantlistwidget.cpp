#include <QAction>
#include <QContextMenuEvent>
#include <QKeyEvent>
#include <QMenu>
#include <QMessageBox>
#include <QMouseEvent>
#include <QStringList>
#include "newvariantdialog.h"
#include "variantlistwidget.h"

VariantListWidget::VariantListWidget(QWidget *parent) :
    QListWidget(parent),
    m_editingItem(0),
    m_editingVar(QString())
{
    m_actionDelete = new QAction(tr("&Delete"), this);
    m_actionDelete->setShortcut(QKeySequence::Delete);
    connect(m_actionDelete, SIGNAL(triggered(bool)),
            this, SLOT(deleteVariant()));

    m_actionNew = new QAction(tr("&New"), this);
    m_actionNew->setShortcut(QKeySequence::New);
    connect(m_actionNew, SIGNAL(triggered(bool)),
            this, SLOT(newVariant()));

    m_actionRename = new QAction(tr("&Rename"), this);
    m_actionRename->setShortcut(QKeySequence::Replace);
    connect(m_actionRename, SIGNAL(triggered(bool)),
            this, SLOT(renameVariant()));

    setEditTriggers(QAbstractItemView::DoubleClicked | QAbstractItemView::EditKeyPressed);
    setSelectionMode(QAbstractItemView::SingleSelection);
}

VariantListWidget::~VariantListWidget()
{
    delete m_actionDelete;
    delete m_actionNew;
    delete m_actionRename;
}

QStringList VariantListWidget::variants() const
{
    return m_vars;
}

void VariantListWidget::setVariants(const QString &tag, const QStringList &vars)
{
    QStringList newVars;
    foreach (QString var, vars)
    {
        var = var.toLower().trimmed();
        if (!var.isEmpty() && !newVars.contains(var))
            newVars.append(var);
    }

    newVars.sort();

    if (newVars != m_vars)
    {
        clear();

        m_vars = newVars;

        foreach (QString var, newVars)
        {
            QListWidgetItem *newItem = new QListWidgetItem(var);
            newItem->setFlags(Qt::ItemIsEditable | Qt::ItemIsEnabled | Qt::ItemIsSelectable);

            addItem(newItem);
        }
    }

    m_tag = tag;
}

void VariantListWidget::clearAllVariants()
{
    clear();
    m_vars.clear();
}

void VariantListWidget::deleteVariant()
{
    QList<QListWidgetItem *> items = selectedItems();
    if (items.size() == 1)
    {
        QListWidgetItem *item = items.first();
        QString var = item->text();
        if (var.compare(m_tag) == 0)
        {
            QMessageBox::warning(this,
                                 tr("Delete Variant"),
                                 tr("You must not delete \"%1\".").arg(var));
            return;
        }

        m_vars.removeOne(var);

        int r = row(item);

        takeItem(r);

        delete item;

        setCurrentRow(qMin(r, count() - 1), QItemSelectionModel::ClearAndSelect);

        emit variantsChanged(m_vars);
    }
}

void VariantListWidget::renameVariant()
{
    QList<QListWidgetItem *> items = selectedItems();
    if (items.size() == 1)
    {
        if (items.first()->text().compare(m_tag) == 0)
        {
            QMessageBox::warning(this,
                                 tr("Rename Variant"),
                                 tr("You must not rename \"%1\".").arg(m_tag));
            return;
        }

        m_editingItem = items.first();
        m_editingVar = m_editingItem->text();
        editItem(m_editingItem);
    }
}

void VariantListWidget::newVariant()
{
    NewVariantDialog *dlg = new NewVariantDialog(this);
    if (dlg->exec() == QDialog::Accepted)
    {
        QString var = dlg->variant();

        if (m_vars.contains(var))
        {
            QMessageBox::warning(this,
                                 tr("Duplicate Variant"),
                                 tr("Variant \"%1\" already exists.").arg(var));
            delete dlg;
            return;
        }

        m_vars.append(var);
        m_vars.sort();

        QListWidgetItem *newItem = new QListWidgetItem(var);
        newItem->setFlags(Qt::ItemIsEditable | Qt::ItemIsEnabled | Qt::ItemIsSelectable);

        insertItem(m_vars.indexOf(var), newItem);

        setCurrentItem(newItem, QItemSelectionModel::ClearAndSelect);

        emit variantsChanged(m_vars);
    }
    delete dlg;
}

void VariantListWidget::closeEditor(QWidget *editor, QAbstractItemDelegate::EndEditHint hint)
{
    QListWidget::closeEditor(editor, hint);
    if (m_editingItem)
    {
        QString newVar = m_editingItem->text().trimmed().toLower();
        if (newVar.isEmpty())
        {
            QMessageBox::warning(this,
                                 tr("Rename Variant"),
                                 tr("The new variant must not be empty."));
            m_editingItem->setText(m_editingVar);
        }
        else if (newVar.compare(m_editingVar) == 0)
        {
            //Do nothing
        }
        else if (m_vars.contains(newVar))
        {
            QMessageBox::warning(this,
                                 tr("Rename Variant"),
                                 tr("Variant \"%1\" already exists.").arg(newVar));
            m_editingItem->setText(m_editingVar);
        }
        else
        {
            m_vars.removeOne(m_editingVar);
            m_vars.append(newVar);
            m_vars.sort();

            for (int i=0; i<count(); i++)
            {
                QListWidgetItem *listItem = item(i);
                QString variant = m_vars.at(i);
                if (listItem->text().compare(variant) != 0)
                    listItem->setText(variant);

                if (variant.compare(newVar) == 0)
                    setCurrentRow(i, QItemSelectionModel::ClearAndSelect);
            }

            emit variantsChanged(m_vars);
        }

        m_editingItem = 0;
        m_editingVar = QString();
    }
}

void VariantListWidget::contextMenuEvent(QContextMenuEvent *event)
{
    QMenu *menu = new QMenu(this);
    menu->setAttribute(Qt::WA_DeleteOnClose);

    menu->addAction(m_actionNew);

    if (selectedItems().size() == 1)
    {
        menu->addSeparator();
        menu->addAction(m_actionRename);
        menu->addSeparator();
        menu->addAction(m_actionDelete);
    }

    menu->exec(event->globalPos());
    event->accept();
}

void VariantListWidget::keyPressEvent(QKeyEvent *event)
{
    if (event->matches(QKeySequence::Delete) || (event->modifiers() == Qt::NoModifier && event->key() == Qt::Key_Backspace))
    {
        event->accept();
        m_actionDelete->trigger();
    }
    else if (event->matches(QKeySequence::New))
    {
        event->accept();
        m_actionNew->trigger();
    }
    else if (event->matches(QKeySequence::Replace))
    {
        event->accept();
        m_actionRename->trigger();
    }
    else
        QListWidget::keyPressEvent(event);
}

void VariantListWidget::mouseDoubleClickEvent(QMouseEvent *event)
{
    if (event->modifiers() == Qt::NoModifier && event->button() == Qt::LeftButton)
    {
        QListWidgetItem *item = itemAt(event->pos());
        if (item)
        {
            if (item->text().compare(m_tag) == 0)
            {
                QMessageBox::warning(this,
                                     tr("Rename Variant"),
                                     tr("You must not rename \"%1\".").arg(m_tag));
                return;
            }

            m_editingItem = item;
            m_editingVar = m_editingItem->text();
            editItem(m_editingItem);
        }
        else
            m_actionNew->trigger();
    }
    QListWidget::mouseDoubleClickEvent(event);
}
