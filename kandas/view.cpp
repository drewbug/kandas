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
#include "viewdelegate.h"

namespace Kandas
{
    namespace Client
    {

        class ViewPrivate
        {
            public:
                ViewPrivate(Kandas::Client::View *parent);

                Manager m_manager;
                ViewDelegate m_delegate;
        };

    }
}

Kandas::Client::ViewPrivate::ViewPrivate(Kandas::Client::View *parent)
    : m_delegate(parent, parent)
{
}

Kandas::Client::View::View(QWidget *parent)
    : QTreeView(parent)
    , p(new Kandas::Client::ViewPrivate(this))
{
    setItemDelegate(&p->m_delegate);
    setModel(p->m_manager.model());
    setHeaderHidden(true);

    connect(&p->m_manager, SIGNAL(initializationComplete(const QString &)), this, SIGNAL(initializationComplete(const QString &)));
}

Kandas::Client::View::~View()
{
    delete p;
}

#include "view.moc"
