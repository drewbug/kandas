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
#include "manager.h"
#include "ndasmodel.h"
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
    m_normalView.setHeaderHidden(true);
    m_normalView.setModel(m_manager.model());
    m_errorView.setItemDelegate(&m_errorDelegate);
    m_errorView.setHeaderHidden(true);
    m_errorView.setModel(m_manager.systemModel());
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

#include "view.moc"
