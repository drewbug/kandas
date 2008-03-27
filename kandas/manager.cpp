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

#include "manager.h"
#include "devicemodel.h"
#include "slotmodel.h"

#include <iostream>
#include <KLocalizedString>

//TODO: Recieve live updates. (Probably integrate initialisation into that.)

Kandas::Client::Manager::Manager()
    : m_deviceModel(new Kandas::Client::DeviceModel(this))
    , m_slotModel(new Kandas::Client::SlotModel(this))
    , m_interface("org.kandas", "/", QDBusConnection::systemBus(), this)
    , m_environment(Kandas::UnknownEnvironment)
{
    if (m_interface.engineVersion().value() == "") //no KaNDASd instance running
        std::cerr << i18n("ERROR: KaNDASd is not running.").toUtf8().data() << std::endl;
    connect(&m_interface, SIGNAL(initEnvironmentInfo(int)), this, SLOT(initEnvironment(int)));
    connect(&m_interface, SIGNAL(initDeviceInfo(const QString &)), this, SLOT(initDevice(const QString &)));
    connect(&m_interface, SIGNAL(initSlotInfo(int, const QString &, int)), this, SLOT(initSlot(int, const QString &, int)));
    connect(&m_interface, SIGNAL(initInfoComplete()), this, SLOT(initComplete()));
    connect(&m_interface, SIGNAL(initInfoComplete()), m_deviceModel, SLOT(lateInit()));
    m_interface.registerClient();
    m_interface.initClient();
}

Kandas::Client::Manager::~Manager()
{
    m_interface.unregisterClient();
    delete m_deviceModel;
}

Kandas::Client::DeviceModel *Kandas::Client::Manager::deviceModel() const
{
    return m_deviceModel;
}

Kandas::Client::SlotModel *Kandas::Client::Manager::slotModel() const
{
    return m_slotModel;
}

void Kandas::Client::Manager::initEnvironment(int state)
{
    switch (state)
    {
        case Kandas::SaneEnvironment: m_environment = Kandas::SaneEnvironment; break;
        case Kandas::NoDriverFound: m_environment = Kandas::NoDriverFound; break;
        case Kandas::NoAdminFound: m_environment = Kandas::NoAdminFound; break;
        default: m_environment = Kandas::UnknownEnvironment; break;
    }
}

void Kandas::Client::Manager::initDevice(const QString &device)
{
    //insert device into list if not already added
    foreach (Kandas::Client::DeviceInfo info, m_devices)
    {
        if (info.device == device)
            return;
    }
    m_devices << DeviceInfo(device);
}

void Kandas::Client::Manager::initSlot(int slot, const QString &device, int state)
{
    //convert state
    Kandas::SlotState slotState = Kandas::Undetermined;
    switch (state)
    {
        case Kandas::Connected: slotState = Kandas::Connected; break;
        case Kandas::Connecting: slotState = Kandas::Connecting; break;
        case Kandas::Disconnected: slotState = Kandas::Disconnected; break;
        case Kandas::Disconnecting: slotState = Kandas::Disconnecting; break;
        default: break; //leaves slotState = Kandas::Undetermined
    }
    //search for device
    QMutableListIterator<Kandas::Client::DeviceInfo> iterDevices(m_devices);
    while (iterDevices.hasNext())
    {
        Kandas::Client::DeviceInfo &info = iterDevices.next();
        if (info.device == device)
        {
            info.slotList << Kandas::Client::SlotInfo(slot, slotState);
            return;
        }
    }
    //device not found - insert new device and add this slot
    Kandas::Client::DeviceInfo info(device);
    info.slotList << Kandas::Client::SlotInfo(slot, slotState);
    m_devices << info;
}

void Kandas::Client::Manager::initComplete()
{
    disconnect(&m_interface, SIGNAL(initEnvironmentInfo(int)), this, SLOT(initEnvironment(int)));
    disconnect(&m_interface, SIGNAL(initDeviceInfo(const QString &)), this, SLOT(initDevice(const QString &)));
    disconnect(&m_interface, SIGNAL(initSlotInfo(int, const QString &, int)), this, SLOT(initSlot(int, const QString &, int)));
    disconnect(&m_interface, SIGNAL(initInfoComplete()), this, SLOT(initComplete()));
    //DEBUG
    m_slotModel->changeDevice(0);
}

#include "manager.moc"
