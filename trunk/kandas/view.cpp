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
#include "connectmenu.h"
#include "manager.h"
#include "ndasdevice.h"
#include "ndasmodel.h"
#include "ndasslot.h"
#include "ndassystemmodel.h"
#include "viewdelegate.h"

#include <QTreeView>

namespace Kandas
{
    namespace Client
    {

        class ViewPrivate
        {
            public:
                ViewPrivate(Kandas::Client::View *parent);

                Manager m_manager;
                QTreeView m_normalView, m_errorView;
                ViewDelegate m_normalDelegate, m_errorDelegate;
        };

    }
}

Kandas::Client::ViewPrivate::ViewPrivate(Kandas::Client::View *parent)
    : m_normalDelegate(&m_normalView, parent)
    , m_errorDelegate(&m_errorView, parent)
{
    m_normalView.setItemDelegate(&m_normalDelegate);
    m_normalView.setModel(m_manager.model());
    m_normalView.setSelectionMode(QAbstractItemView::NoSelection);
    m_normalView.setHeaderHidden(true);
    m_normalView.setItemsExpandable(false);
    m_normalView.setRootIsDecorated(false);
    m_errorView.setItemDelegate(&m_errorDelegate);
    m_errorView.setModel(m_manager.systemModel());
    m_errorView.setSelectionMode(QAbstractItemView::NoSelection);
    m_errorView.setHeaderHidden(true);
    m_errorView.setItemsExpandable(false);
    m_errorView.setRootIsDecorated(false);
}

Kandas::Client::View::View(QWidget *parent)
    : QStackedWidget(parent)
    , p(new Kandas::Client::ViewPrivate(this))
{
    addWidget(&p->m_normalView);
    addWidget(&p->m_errorView);
    if (p->m_manager.systemModel()->clean())
        setCurrentWidget(&p->m_normalView);
    else
        setCurrentWidget(&p->m_errorView);

    connect(&p->m_manager, SIGNAL(initializationComplete(const QString &)), this, SIGNAL(initializationComplete(const QString &)));
    connect(&p->m_manager, SIGNAL(systemStateChanged(Kandas::SystemState)), this, SLOT(systemStateChanged(Kandas::SystemState)));
    connect(&p->m_normalDelegate, SIGNAL(actionTriggered(const QPersistentModelIndex &, const QString &)), this, SLOT(actionTriggered(const QPersistentModelIndex &, const QString &)));

}

Kandas::Client::View::~View()
{
    delete p;
}

void Kandas::Client::View::systemStateChanged(Kandas::SystemState state)
{
    Q_UNUSED(state)
    QWidget* newWidget;
    if (p->m_manager.systemModel()->clean())
        newWidget = &p->m_normalView;
    else
        newWidget = &p->m_errorView;
    if (currentWidget() != newWidget)
        setCurrentWidget(newWidget);
}

void Kandas::Client::View::actionTriggered(const QPersistentModelIndex &index, const QString &action)
{
    QModelIndex dataIndex(index);
    if (action == QLatin1String("connect-device"))
    {
        Kandas::Client::NdasDevice* device = ndasdata_cast<Kandas::Client::NdasDevice*>(index.internalPointer());
        new Kandas::Client::ConnectMenu(&p->m_manager, device->name());
//         p->m_manager.connectDevice(device->name(), true);
    }
    else if (action == QLatin1String("disconnect-device"))
    {
        Kandas::Client::NdasDevice* device = ndasdata_cast<Kandas::Client::NdasDevice*>(index.internalPointer());
        p->m_manager.disconnectDevice(device->name());
    }
    else if (action == QLatin1String("connect-slot"))
    {
        Kandas::Client::NdasSlot* slot = ndasdata_cast<Kandas::Client::NdasSlot*>(index.internalPointer());
        new Kandas::Client::ConnectMenu(&p->m_manager, slot->number());
//         p->m_manager.connectSlot(slot->number(), true);
    }
    else if (action == QLatin1String("disconnect-slot"))
    {
        Kandas::Client::NdasSlot* slot = ndasdata_cast<Kandas::Client::NdasSlot*>(index.internalPointer());
        p->m_manager.disconnectSlot(slot->number());
    }
    //TODO: allow to choose between read/write and readonly mode
}

#include "view.moc"
