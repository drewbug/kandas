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

                bool m_clean;
                OrgKandasInterface *m_interface;

                Kandas::EnvironmentState m_environment;
                QList<QString> m_devices;
                QHash<int, Kandas::SlotInfo> m_slots;
        };

    }
}

Kandas::Console::BaseWorkerPrivate::BaseWorkerPrivate(BaseWorker *parent)
    : m_clean(true)
    , m_interface(new OrgKandasInterface("org.kandas", "/", QDBusConnection::systemBus(), parent))
    , m_environment(Kandas::UnknownEnvironment)
{
    QString version = m_interface->interfaceVersion().value();
    if (version == "") //no KaNDASd instance running
    {
        std::cerr << i18n("ERROR: KaNDASd is not running.").toUtf8().data() << std::endl;
        m_clean = false;
    }
    else if (version != "0.1")
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
    connect(p->m_interface, SIGNAL(initEnvironmentInfo(int)), SLOT(initEnvironment(int)));
    connect(p->m_interface, SIGNAL(initDeviceInfo(const QString&)), SLOT(initDevice(const QString&)));
    connect(p->m_interface, SIGNAL(initSlotInfo(int, const QString&, int)), SLOT(initSlot(int, const QString&, int)));
    connect(p->m_interface, SIGNAL(initInfoComplete()), SLOT(executeJobs()));
    //initiation sequence
    p->m_interface->registerClient();
    p->m_interface->initClient();
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

Kandas::EnvironmentState Kandas::Console::BaseWorker::environment() const
{
    return p->m_environment;
}

QList<QString> Kandas::Console::BaseWorker::devicesList() const
{
    return p->m_devices;
}

QHash<int, Kandas::SlotInfo> Kandas::Console::BaseWorker::slotsList() const
{
    return p->m_slots;
}

void Kandas::Console::BaseWorker::initEnvironment(int state)
{
    switch (state)
    {
        case Kandas::SaneEnvironment: p->m_environment = Kandas::SaneEnvironment; break;
        case Kandas::NoDriverFound: p->m_environment = Kandas::NoDriverFound; break;
        case Kandas::NoAdminFound: p->m_environment = Kandas::NoAdminFound; break;
        default: p->m_environment = Kandas::UnknownEnvironment; break;
    }
}

void Kandas::Console::BaseWorker::initDevice(const QString &device)
{
    p->m_devices << device;
}

void Kandas::Console::BaseWorker::initSlot(int slot, const QString &device, int state)
{
    Kandas::SlotState slotState = Kandas::Undetermined;
    switch (state)
    {
        case Kandas::Connected: slotState = Kandas::Connected; break;
        case Kandas::Connecting: slotState = Kandas::Connecting; break;
        case Kandas::Disconnected: slotState = Kandas::Disconnected; break;
        case Kandas::Disconnecting: slotState = Kandas::Disconnecting; break;
        default: break; //leaves slotState = Kandas::Undetermined
    }
    p->m_slots[slot] = Kandas::SlotInfo(device, slotState);
}

void Kandas::Console::BaseWorker::executeJobs()
{
    if (execute())
        qApp->quit();
}

#include "base-worker.moc"
