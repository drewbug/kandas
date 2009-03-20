/***************************************************************************
 *   Copyright 2008-2009 Stefan Majewsky <majewsky@gmx.net>
 *
 *   This program is free software; you can redistribute it and/or
 *   modify it under the terms of the GNU General Public
 *   License as published by the Free Software Foundation; either
 *   version 2 of the License, or (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program; if not, write to the Free Software
 *   Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 ***************************************************************************/

#include "view.h"
#include "devicemodel.h"
#include "iconwidget.h"
#include "manager.h"
#include "ndasmodel.h"
#include "slotmodel.h"

#include <QAbstractTextDocumentLayout>
#include <QApplication>
#include <QLabel>
#include <QListView>
#include <QPainter>
#include <QTextDocument>
#include <QTreeView>
#include <QUrl>
#include <QVBoxLayout>
#include <kwidgetitemdelegate.h>

namespace Kandas
{
    namespace Client
    {

        class ViewDelegate : public KWidgetItemDelegate
        {
            public:
                ViewDelegate(QAbstractItemView *view, QObject *parent = 0);

                virtual QList<QWidget *> createItemWidgets() const;
                virtual void updateItemWidgets(const QList<QWidget*> widgets, const QStyleOptionViewItem& option, const QPersistentModelIndex& index) const;
                virtual void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
                virtual QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const;
        };

        class ViewPrivate
        {
            public:
                ViewPrivate(Kandas::Client::View *parent);

                QVBoxLayout m_layout;
                QListView m_deviceList, m_slotList;
                QTreeView m_treeView;
                Manager m_manager;
                ViewDelegate m_deviceDelegate, m_slotDelegate, m_treeDelegate;
        };

    }
}

Kandas::Client::ViewPrivate::ViewPrivate(Kandas::Client::View *parent)
    : m_deviceDelegate(&m_deviceList, parent)
    , m_slotDelegate(&m_slotList, parent)
    , m_treeDelegate(&m_treeView, parent)
{
}

Kandas::Client::View::View(QWidget *parent)
    : QWidget(parent)
    , p(new Kandas::Client::ViewPrivate(this))
{
    p->m_deviceList.setItemDelegate(&p->m_deviceDelegate);
    p->m_deviceList.setModel(p->m_manager.deviceModel());
    p->m_deviceList.setSelectionMode(QAbstractItemView::SingleSelection);
    p->m_deviceList.setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    p->m_slotList.setItemDelegate(&p->m_slotDelegate);
    p->m_slotList.setModel(p->m_manager.slotModel());
    p->m_slotList.setSelectionMode(QAbstractItemView::SingleSelection);
    p->m_slotList.setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    p->m_treeView.setItemDelegate(&p->m_treeDelegate);
    p->m_treeView.setModel(p->m_manager.model());
    p->m_treeView.setHeaderHidden(true);

    p->m_layout.addWidget(&p->m_deviceList);
    p->m_layout.addWidget(&p->m_slotList);
    p->m_layout.addWidget(&p->m_treeView);
    setLayout(&p->m_layout);

    connect(p->m_deviceList.selectionModel(), SIGNAL(currentChanged(const QModelIndex &, const QModelIndex &)), &p->m_manager, SLOT(selectedDeviceChanged(const QModelIndex &)));
    connect(p->m_manager.deviceModel(), SIGNAL(modelReset()), this, SLOT(resetDeviceSelection()));

    connect(&p->m_manager, SIGNAL(initializationComplete(const QString &)), this, SIGNAL(initializationComplete(const QString &)));
}

Kandas::Client::View::~View()
{
    delete p;
}

void Kandas::Client::View::resetDeviceSelection()
{
    QModelIndex selection = p->m_manager.deviceModel()->index(0, 0);
    p->m_deviceList.selectionModel()->select(selection, QItemSelectionModel::ClearAndSelect);
    p->m_manager.selectedDeviceChanged(selection);
}

void Kandas::Client::View::connectDeviceRead()
{
    if (!p->m_deviceList.selectionModel()->hasSelection())
        return;
    QModelIndex selected = p->m_deviceList.selectionModel()->currentIndex();
    QString device = p->m_manager.deviceModel()->data(selected, Kandas::Client::ItemIdentifierRole).toString();
    p->m_manager.connectDevice(device, true);
}

void Kandas::Client::View::connectDeviceWrite()
{
    if (!p->m_deviceList.selectionModel()->hasSelection())
        return;
    QModelIndex selected = p->m_deviceList.selectionModel()->currentIndex();
    QString device = p->m_manager.deviceModel()->data(selected, Kandas::Client::ItemIdentifierRole).toString();
    p->m_manager.connectDevice(device, false);
}

void Kandas::Client::View::connectSlotRead()
{
    if (!p->m_slotList.selectionModel()->hasSelection())
        return;
    QModelIndex selected = p->m_slotList.selectionModel()->currentIndex();
    int slot = p->m_manager.slotModel()->data(selected, Kandas::Client::ItemIdentifierRole).toInt();
    p->m_manager.connectSlot(slot, true);
}

void Kandas::Client::View::connectSlotWrite()
{
    if (!p->m_slotList.selectionModel()->hasSelection())
        return;
    QModelIndex selected = p->m_slotList.selectionModel()->currentIndex();
    int slot = p->m_manager.slotModel()->data(selected, Kandas::Client::ItemIdentifierRole).toInt();
    p->m_manager.connectSlot(slot, false);
}

void Kandas::Client::View::disconnectDevice()
{
    if (!p->m_deviceList.selectionModel()->hasSelection())
        return;
    QModelIndex selected = p->m_deviceList.selectionModel()->currentIndex();
    QString device = p->m_manager.deviceModel()->data(selected, Kandas::Client::ItemIdentifierRole).toString();
    p->m_manager.disconnectDevice(device);
}

void Kandas::Client::View::disconnectSlot()
{
    if (!p->m_slotList.selectionModel()->hasSelection())
        return;
    QModelIndex selected = p->m_slotList.selectionModel()->currentIndex();
    int slot = p->m_manager.slotModel()->data(selected, Kandas::Client::ItemIdentifierRole).toInt();
    p->m_manager.disconnectSlot(slot);
}

Kandas::Client::ViewDelegate::ViewDelegate(QAbstractItemView *view, QObject *parent)
    : KWidgetItemDelegate(view, parent)
{
}

QList<QWidget *> Kandas::Client::ViewDelegate::createItemWidgets() const
{
    //container widget
    QWidget *container = new QWidget;
    QGridLayout *layout = new QGridLayout;
    container->setLayout(layout);
    //contained widgets
    static const QSize iconSize(48, 48);
    Kandas::Client::IconWidget* iconWidget = new Kandas::Client::IconWidget(iconSize);
    iconWidget->setEnabled(false);
    Kandas::Client::IconWidget* buttonWidget = new Kandas::Client::IconWidget(iconSize);
    QLabel* headlineWidget = new QLabel;
    QLabel* sublineWidget = new QLabel;
    QFont headlineFont = headlineWidget->font();
    headlineFont.setBold(true);
    headlineWidget->setFont(headlineFont);
    //build layout
    layout->addWidget(iconWidget, 0, 0, 2, 1);
    layout->addWidget(headlineWidget, 0, 1, Qt::AlignLeft | Qt::AlignBottom);
    layout->addWidget(sublineWidget, 1, 1, Qt::AlignLeft | Qt::AlignTop);
    layout->addWidget(buttonWidget, 0, 2, 2, 1);
    return QList<QWidget *>() << container;
}

#include <KIcon>

void Kandas::Client::ViewDelegate::updateItemWidgets(const QList<QWidget*> widgets, const QStyleOptionViewItem& option, const QPersistentModelIndex& index) const
{
    //retrieve data
    const QString displayText = index.model()->data(index, Qt::DisplayRole).toString();
    const QVariant decoration = index.model()->data(index, Qt::DecorationRole);
    const QString statusText = index.model()->data(index, Kandas::Client::ConnectionStatusRole).toString();
    //retrieve widgets
    QWidget* container = widgets[0];
    QGridLayout* layout = qobject_cast<QGridLayout*>(container->layout());
    Kandas::Client::IconWidget* iconWidget = qobject_cast<Kandas::Client::IconWidget*>(layout->itemAtPosition(0, 0)->widget());
    QLabel* headlineWidget = qobject_cast<QLabel*>(layout->itemAtPosition(0, 1)->widget());
    QLabel* sublineWidget = qobject_cast<QLabel*>(layout->itemAtPosition(1, 1)->widget());
    Kandas::Client::IconWidget* buttonWidget = qobject_cast<Kandas::Client::IconWidget*>(layout->itemAtPosition(0, 2)->widget());
    //update icon
    iconWidget->setIcon(qvariant_cast<QIcon>(decoration));
    headlineWidget->setText(displayText);
    sublineWidget->setText(statusText);
    buttonWidget->setIcon(KIcon("preferences-plugin"));
    //update size of layout and text color
    container->setGeometry(QRect(QPoint(0, 0), sizeHint(option, index)));
    //update text color in palette
    QPalette basePalette = iconWidget->palette();
    if (option.state & QStyle::State_Selected)
        basePalette.setBrush(QPalette::Text, option.palette.highlightedText());
    headlineWidget->setPalette(basePalette);
    sublineWidget->setPalette(basePalette);
    //DEBUG
    QTreeView* tv = qobject_cast<QTreeView*>(itemView());
    if (tv)
        tv->expand(index);
}

void Kandas::Client::ViewDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    //paint background for selected or hovered item
    QStyleOptionViewItemV4 opt = option;
    itemView()->style()->drawPrimitive(QStyle::PE_PanelItemViewItem, &opt, painter, 0);
    //paint widgets
    KWidgetItemDelegate::paintWidgets(painter, option, index); //is deprecated in KDE 4.2, but we want to support 4.1 also
}

QSize Kandas::Client::ViewDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    Q_UNUSED(option)
    Q_UNUSED(index)
    //only the height of the size hint is interesting
    static const int topMargin = QApplication::style()->pixelMetric(QStyle::PM_LayoutTopMargin);
    static const int bottomMargin = QApplication::style()->pixelMetric(QStyle::PM_LayoutBottomMargin);
    static const int iconSize = 48;
    static const int verticalSizeHint = iconSize + topMargin + bottomMargin;
    return QSize(itemView()->viewport()->width(), verticalSizeHint);
}

#include "view.moc"
