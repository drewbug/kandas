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
#include "manager.h"
#include "ndasmodel.h"
#include "slotmodel.h"
#include "viewdelegate.h"

#include <QAbstractTextDocumentLayout>
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

#include "view.moc"
