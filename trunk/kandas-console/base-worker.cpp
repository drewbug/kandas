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
                Kandas::Console::DeviceList m_devices;
                Kandas::Console::SlotList m_slots;
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
        Kandas::Console::printError(i18n("KaNDASd is not running."));
        m_clean = false;
    }
    else if (version != "0.2")
    {
        Kandas::Console::printError(i18n("Incompatible daemon detected (version \"%1\").", version));
        m_clean = false;
    }
}

Kandas::Console::BaseWorkerPrivate::~BaseWorkerPrivate()
{
    delete m_interface;
    qDeleteAll(m_devices);
    qDeleteAll(m_slots);
}

Kandas::Console::BaseWorker::BaseWorker()
    : p(new BaseWorkerPrivate(this))
{
    //connect interface signals
    connect(p->m_interface, SIGNAL(systemInfo(int)), SLOT(systemInfo(int)));
    connect(p->m_interface, SIGNAL(deviceInfo(const QString&, const QString&, int, bool)), SLOT(deviceInfo(const QString&, const QString&, int, bool)));
    connect(p->m_interface, SIGNAL(slotInfo(int, const QString&, const QString&, int)), SLOT(slotInfo(int, const QString&, const QString&, int)));
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

Kandas::Console::DeviceList Kandas::Console::BaseWorker::devicesList() const
{
    return p->m_devices;
}

Kandas::Console::SlotList Kandas::Console::BaseWorker::slotsList() const
{
    return p->m_slots;
}

void Kandas::Console::BaseWorker::setAutoTimeout(bool enableAutoTimeout)
{
    p->m_withAutoTimeout = enableAutoTimeout;
}

void Kandas::Console::BaseWorker::systemInfo(int state)
{
    p->m_system = (Kandas::SystemState) state;
}

void Kandas::Console::BaseWorker::deviceInfo(const QString &deviceName, const QString &serial, int state, bool hasWriteKey)
{
    Kandas::Console::Device* device = new Kandas::Console::Device;
    device->name = deviceName;
    device->serial = serial;
    device->state = (Kandas::DeviceState) state;
    device->hasWriteKey = hasWriteKey;
    //if a device with this name does already exist, replace it
    Kandas::Console::Device* oldDevice = p->m_devices.device(deviceName);
    if (oldDevice)
    {
        p->m_devices.removeAll(oldDevice);
        delete oldDevice;
    }
    p->m_devices << device;
}

void Kandas::Console::BaseWorker::slotInfo(int slotNumber, const QString &device, const QString &blockDevice, int state)
{
    Kandas::Console::Slot* slot = new Kandas::Console::Slot;
    slot->number = slotNumber;
    slot->device = device;
    slot->blockDevice = blockDevice;
    slot->state = (Kandas::SlotState) state;
    //if a slot with this name does already exist, replace it
    Kandas::Console::Slot* oldSlot = p->m_slots.slot(slotNumber);
    if (oldSlot)
    {
        p->m_slots.removeAll(oldSlot);
        delete oldSlot;
    }
    p->m_slots << slot;
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
    Kandas::Console::printWarning(i18n("Operation timed out."));
    qApp->quit();
}

void Kandas::Console::printMessage(const QString& message, bool errorOutput)
{
    (errorOutput ? std::cerr : std::cout) << message.toUtf8().data() << std::endl;
}

void Kandas::Console::printError(const QString& message)
{
    Kandas::Console::printMessage(i18n("Error: %1", message), true);
}

void Kandas::Console::printWarning(const QString& message)
{
    Kandas::Console::printMessage(i18n("Warning: %1", message), true);
}

#include "base-worker.moc"
