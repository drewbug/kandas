/***************************************************************************
 *   Copyright (C) 2008 Stefan Majewsky <majewsky@gmx.net>
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
#include "device.h"

#include <QHash>
#include <QVBoxLayout>

namespace Kandas
{
    namespace Client
    {

        class ViewPrivate
        {
            public:
                ViewPrivate() {}

                QVBoxLayout m_layout;
                QHash<QString, Device *> m_devices;
        };

    }
}

Kandas::Client::View::View(QWidget *parent)
    : QWidget(parent)
    , p(new Kandas::Client::ViewPrivate())
{
    setLayout(&p->m_layout);
    addDevice("Abstellraum");
    addDevice("Firmenzentrale");
}

Kandas::Client::View::~View()
{
    delete p;
}

Kandas::Client::Device *Kandas::Client::View::addDevice(const QString &name)
{
    if (p->m_devices.contains(name))
        return 0;
    Kandas::Client::Device *device = new Kandas::Client::Device(name, this);
    p->m_devices[name] = device;
    p->m_layout.addWidget(device);
    return device;
}
