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

#include "base-worker.h"
#include "interface.h"

#include <iostream>
#include <QApplication>
#include <QHash>
#include <QList>
#include <KLocalizedString>

namespace Kandas
{
    namespace Console
    {

        class BaseWorkerPrivate
        {
            public:
                BaseWorkerPrivate(BaseWorker *parent);
                ~BaseWorkerPrivate();

                bool m_clean, m_withAutoTimeout;
                OrgKandasInterface *m_interface;

                Kandas::SystemState m_system;
                QList<QString> m_devices;
                QHash<int, Kandas::SlotInfo> m_slots;
        };

    }
}

Kandas::Console::BaseWorkerPrivate::BaseWorkerPrivate(BaseWorker *parent)
    : m_clean(true)
    , m_withAutoTimeout(false)
    , m_interface(new OrgKandasInterface("org.kandas", "/", QDBusConnection::systemBus(), parent))
    , m_system(Kandas::SystemUnchecked)
{
    QString version = m_interface->interfaceVersion().value();
    if (version == "") //no KaNDASd instance running
    {
        std::cerr << i18n("ERROR: KaNDASd is not running.").toUtf8().data() << std::endl;
        m_clean = false;
    }
    else if (version != "0.2")
    {
        std::cerr << i18n("ERROR: Unknown KaNDASd version \"%1\" detected.", version).toUtf8().data() << std::endl;
        m_clean = false;
    }
}

Kandas::Console::BaseWorkerPrivate::~BaseWorkerPrivate()
{
    delete m_interface;
}

Kandas::Console::BaseWorker::BaseWorker()
    : p(new BaseWorkerPrivate(this))
{
    //connect interface signals
    connect(p->m_interface, SIGNAL(systemInfo(int)), SLOT(systemInfo(int)));
    connect(p->m_interface, SIGNAL(deviceInfo(const QString&)), SLOT(deviceInfo(const QString&)));
    connect(p->m_interface, SIGNAL(slotInfo(int, const QString&, int)), SLOT(slotInfo(int, const QString&, int)));
    connect(p->m_interface, SIGNAL(initComplete()), SLOT(executeJobs()));
    //initiation sequence
    p->m_interface->registerClient();
}

Kandas::Console::BaseWorker::~BaseWorker()
{
    p->m_interface->unregisterClient();
    delete p;
}

bool Kandas::Console::BaseWorker::clean() const
{
    return p->m_clean;
}

OrgKandasInterface *Kandas::Console::BaseWorker::interface() const
{
    return p->m_interface;
}

Kandas::SystemState Kandas::Console::BaseWorker::system() const
{
    return p->m_system;
}

QList<QString> Kandas::Console::BaseWorker::devicesList() const
{
    return p->m_devices;
}

QHash<int, Kandas::SlotInfo> Kandas::Console::BaseWorker::slotsList() const
{
    return p->m_slots;
}

void Kandas::Console::BaseWorker::setAutoTimeout(bool enableAutoTimeout)
{
    p->m_withAutoTimeout = enableAutoTimeout;
}

void Kandas::Console::BaseWorker::systemInfo(int state)
{
    switch (state)
    {
        case Kandas::SystemChecked: p->m_system = Kandas::SystemChecked; break;
        case Kandas::NoDriverFound: p->m_system = Kandas::NoDriverFound; break;
        case Kandas::NoAdminFound: p->m_system = Kandas::NoAdminFound; break;
        default: p->m_system = Kandas::SystemUnchecked; break;
    }
}

void Kandas::Console::BaseWorker::deviceInfo(const QString &device)
{
    p->m_devices << device;
}

void Kandas::Console::BaseWorker::slotInfo(int slot, const QString &device, int state)
{
    p->m_slots[slot] = Kandas::SlotInfo(device, (Kandas::SlotState) state);
}

void Kandas::Console::BaseWorker::executeJobs()
{
    if (execute())
        qApp->quit();
    else if (p->m_withAutoTimeout)
        QTimer::singleShot(5000, this, SLOT(autoTimeout()));
}

void Kandas::Console::BaseWorker::autoTimeout()
{
    std::cerr << i18n("Operation timed out.").toUtf8().data() << std::endl;
    qApp->quit();
}

#include "base-worker.moc"
