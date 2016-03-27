#include <QAction>
#include <QContextMenuEvent>
#include <QKeyEvent>
#include <QMenu>
#include <QMessageBox>
#include <QMouseEvent>
#include <QStringList>
#include "newpropertydialog.h"
#include "propertylistwidget.h"

PropertyListWidget::PropertyListWidget(QWidget *parent) :
    QListWidget(parent),
    m_editingItem(0),
    m_editingProp(QString())
{
    m_actionDelete = new QAction(tr("&Delete"), this);
    m_actionDelete->setShortcut(QKeySequence::Delete);
    connect(m_actionDelete, SIGNAL(triggered(bool)),
            this, SLOT(deleteProperty()));

    m_actionNew = new QAction(tr("&New"), this);
    m_actionNew->setShortcut(QKeySequence::New);
    connect(m_actionNew, SIGNAL(triggered(bool)),
            this, SLOT(newProperty()));

    m_actionRename = new QAction(tr("&Rename"), this);
    m_actionRename->setShortcut(QKeySequence::Replace);
    connect(m_actionRename, SIGNAL(triggered(bool)),
            this, SLOT(renameProperty()));

    setEditTriggers(QAbstractItemView::DoubleClicked | QAbstractItemView::EditKeyPressed);
    setSelectionMode(QAbstractItemView::SingleSelection);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setTextElideMode(Qt::ElideRight);
}

PropertyListWidget::~PropertyListWidget()
{
    delete m_actionDelete;
    delete m_actionNew;
    delete m_actionRename;
}

QString PropertyListWidget::currentProperty() const
{
    QList<QListWidgetItem *> items = selectedItems();
    if (items.size() == 1)
        return items.first()->text();
    else
        return QString();
}

QStringList PropertyListWidget::properties() const
{
    return m_props;
}

void PropertyListWidget::clearAllproperties()
{
    clear();
    m_props.clear();
}

void PropertyListWidget::addNewProperty(const QString &prop)
{
    QString newProp = prop.trimmed().toLower();
    if (newProp.isEmpty())
    {
        QMessageBox::warning(this,
                             tr("Empty Property"),
                             tr("The property must not be empty."));
        return;
    }
    if (m_props.contains(newProp))
    {
        QMessageBox::warning(this,
                             tr("Duplicate Property"),
                             tr("Property \"%1\" already exists.").arg(newProp));
        return;
    }

    m_props.append(newProp);
    m_props.sort();

    QListWidgetItem *newItem = new QListWidgetItem(newProp);
    newItem->setFlags(Qt::ItemIsEditable | Qt::ItemIsEnabled | Qt::ItemIsSelectable);
    newItem->setToolTip(newProp);

    insertItem(m_props.indexOf(newProp), newItem);

    setCurrentItem(newItem, QItemSelectionModel::ClearAndSelect);
}

void PropertyListWidget::addNewProperties(const QStringList &props)
{
    QStringList toAdd, invalids;
    bool hasEmpty = false;
    foreach (QString prop, props)
    {
        prop = prop.trimmed().toLower();
        if (prop.isEmpty())
            hasEmpty = true;
        else if (m_props.contains(prop))
            invalids.append(prop);
        else
            toAdd.append(prop);
    }

    toAdd.removeDuplicates();
    toAdd.sort();

    invalids.removeDuplicates();
    invalids.sort();

    if (invalids.size() == 1)
    {
        QMessageBox::warning(this,
                             tr("Duplicate Property"),
                             tr("Property \"%1\" already exists.").arg(invalids.first()));
    }
    else if (invalids.size() > 1)
    {
        QMessageBox::warning(this,
                             tr("Duplicate properties"),
                             tr("properties \"%1\" already exist.").arg(invalids.join("\", \"")));
    }
    else if (hasEmpty)
    {
        QMessageBox::warning(this,
                             tr("Empty Property"),
                             tr("All properties must not be empty."));
    }
    else
    {

    }

    if (toAdd.isEmpty())
    {
        QMessageBox::warning(this,
                             tr("Nothing to Add"),
                             tr("No property can be added."));
        return;
    }

    m_props.append(toAdd);
    m_props.sort();

    foreach (QString newProp, toAdd)
    {
        QListWidgetItem *newItem = new QListWidgetItem(newProp);
        newItem->setFlags(Qt::ItemIsEditable | Qt::ItemIsEnabled | Qt::ItemIsSelectable);
        newItem->setToolTip(newProp);

        insertItem(m_props.indexOf(newProp), newItem);
    }

    setCurrentRow(m_props.indexOf(toAdd.last()), QItemSelectionModel::ClearAndSelect);
}

void PropertyListWidget::deleteProperty()
{
    QModelIndexList indexes = selectedIndexes();
    if (indexes.size() == 1)
    {
        int row = indexes.first().row();
        QListWidgetItem *item = takeItem(row);

        QString prop = item->text();
        delete item;

        m_props.removeOne(prop);

        setCurrentRow(qMin(row, count() - 1), QItemSelectionModel::ClearAndSelect);

        emit propertyDeleted(prop);

        emit currentTextChanged(prop);
    }
}

void PropertyListWidget::renameProperty()
{
    QList<QListWidgetItem *> items = selectedItems();
    if (items.size() == 1)
    {
        m_editingItem = items.first();
        m_editingProp = m_editingItem->text();
        editItem(m_editingItem);
    }
}

void PropertyListWidget::newProperty()
{
    NewPropertyDialog *dlg = new NewPropertyDialog(this);
    if (dlg->exec() == QDialog::Accepted)
    {
        QString prop = dlg->newProperty();

        if (m_props.contains(prop))
        {
            QMessageBox::warning(this,
                                 tr("Duplicate Property"),
                                 tr("Property \"%1\" already exists.").arg(prop));
            delete dlg;
            return;
        }

        m_props.append(prop);
        m_props.sort();

        QListWidgetItem *newItem = new QListWidgetItem(prop);
        newItem->setFlags(Qt::ItemIsEditable | Qt::ItemIsEnabled | Qt::ItemIsSelectable);
        newItem->setToolTip(prop);

        insertItem(m_props.indexOf(prop), newItem);

        setCurrentItem(newItem, QItemSelectionModel::ClearAndSelect);

        emit propertyAdded(prop);

        emit currentTextChanged(prop);
    }
    delete dlg;
}

void PropertyListWidget::closeEditor(QWidget *editor, QAbstractItemDelegate::EndEditHint hint)
{
    QListWidget::closeEditor(editor, hint);
    if (m_editingItem)
    {
        QString newProp = m_editingItem->text().trimmed().toLower();
        if (newProp.isEmpty())
        {
            QMessageBox::warning(this,
                                 tr("Rename Property"),
                                 tr("The new property must not be empty."));
            m_editingItem->setText(m_editingProp);
            m_editingItem->setToolTip(m_editingProp);
        }
        else if (newProp.compare(m_editingProp) == 0)
        {
            //Do nothing
        }
        else if (m_props.contains(newProp))
        {
            QMessageBox::warning(this,
                                 tr("Rename Property"),
                                 tr("Property \"%1\" already exists.").arg(newProp));
            m_editingItem->setText(m_editingProp);
            m_editingItem->setToolTip(m_editingProp);
        }
        else
        {
            m_props.removeOne(m_editingProp);
            m_props.append(newProp);
            m_props.sort();

            for (int i=0; i<count(); i++)
            {
                QListWidgetItem *listItem = item(i);
                QString property = m_props.at(i);
                if (listItem->text().compare(property) != 0)
                {
                    listItem->setText(property);
                }

                listItem->setToolTip(property);

                if (property.compare(newProp) == 0)
                    setCurrentRow(i, QItemSelectionModel::ClearAndSelect);
            }

            emit propertyChanged(m_editingProp, newProp);

            emit currentTextChanged(newProp);
        }

        m_editingItem = 0;
        m_editingProp = QString();
    }
}

void PropertyListWidget::contextMenuEvent(QContextMenuEvent *event)
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

void PropertyListWidget::keyPressEvent(QKeyEvent *event)
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

void PropertyListWidget::mouseDoubleClickEvent(QMouseEvent *event)
{
    if (event->modifiers() == Qt::NoModifier && event->button() == Qt::LeftButton)
    {
        QListWidgetItem *item = itemAt(event->pos());
        if (item)
        {
            m_editingItem = item;
            m_editingProp = m_editingItem->text();
            editItem(m_editingItem);
        }
        else
            m_actionNew->trigger();
    }
    QListWidget::mouseDoubleClickEvent(event);
}
