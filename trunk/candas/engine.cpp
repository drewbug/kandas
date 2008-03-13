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

#include "engine.h"
#include "interface.h"

#include <iostream>
#include <KLocalizedString>

Kandas::Console::Engine::Engine(const QList<QString> &upTargets, const QList<QString> &downTargets, const QList<QString> &stateTargets)
    : QObject()
    , m_upTargets(upTargets)
    , m_downTargets(downTargets)
    , m_stateTargets(stateTargets)
    , m_interface(new OrgKdeKandasInterface("org.kde.kandas", "/", QDBusConnection::systemBus(), this))
{
    //connect interface signals
    connect(m_interface, SIGNAL(initEnvironmentInfo(int)), SLOT(initEnvironment(int)));
    connect(m_interface, SIGNAL(initDeviceInfo(const QString&)), SLOT(initDevice(const QString&)));
    connect(m_interface, SIGNAL(initSlotInfo(int, const QString&, int)), SLOT(initSlot(int, const QString&, int)));
    connect(m_interface, SIGNAL(initInfoComplete()), SLOT(executeJobs()));
    //initiation sequence
    m_interface->registerClient(Kandas::DirectModifier);
    m_interface->initClient();
}

Kandas::Console::Engine::~Engine()
{
    m_interface->unregisterClient(Kandas::DirectModifier);
    delete m_interface;
}

//initiation sequence

void Kandas::Console::Engine::initEnvironment(int state)
{
    switch (state)
    {
        case Kandas::SaneEnvironment: m_environment = Kandas::SaneEnvironment; break;
        case Kandas::NoDriverFound: m_environment = Kandas::NoDriverFound; break;
        case Kandas::NoAdminFound: m_environment = Kandas::NoAdminFound; break;
        default: m_environment = Kandas::UnknownEnvironment; break;
    }
}

void Kandas::Console::Engine::initDevice(const QString &device)
{
    m_devices << device;
}

void Kandas::Console::Engine::initSlot(int slot, const QString &device, int state)
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
    m_slots[slot] = Kandas::SlotInfo(device, slotState);
}

void Kandas::Console::Engine::executeJobs()
{
    //state reports
    foreach (QString target, m_stateTargets)
    {
        if (m_devices.contains(target))
            stateDevice(target);
        else
        {
            int slot = target.toInt();
            if (m_slots.contains(slot))
                stateSlot(slot);
            else
                std::cerr << i18n("Warning: Target \"%1\" not found", target).toUtf8().data() << std::endl;
        }
    }
    //up targets
    foreach (QString target, m_upTargets)
    {
        if (m_devices.contains(target))
            upDevice(target);
        else
        {
            int slot = target.toInt();
            if (m_slots.contains(slot))
                upSlot(slot);
            else
                std::cerr << i18n("Target \"%1\" not found", target).toUtf8().data() << std::endl;
        }
    }
    //down targets
    foreach (QString target, m_downTargets)
    {
        if (m_devices.contains(target))
            downDevice(target);
        else
        {
            int slot = target.toInt();
            if (m_slots.contains(slot))
                downSlot(slot);
            else
                std::cerr << i18n("Target \"%1\" not found", target).toUtf8().data() << std::endl;
        }
    }
}

void Kandas::Console::Engine::stateDevice(const QString &device)
{
    std::cout << i18n("Device \"%1\"", device).toUtf8().data() << std::endl;
    std::cout << '\t' << i18n("Slot IDs:").toUtf8().data();
    bool foundSlots = false;
    QHashIterator<int, Kandas::SlotInfo> iterSlots(m_slots);
    while (iterSlots.hasNext())
    {
        iterSlots.next();
        if (iterSlots.value().device == device)
        {
            std::cout << ' ' << iterSlots.key();
            foundSlots = true;
        }
    }
    if (!foundSlots)
        std::cout << i18n("No slots found").toUtf8().data();
    std::cout << std::endl;
}

void Kandas::Console::Engine::stateSlot(int slot)
{
    std::cout << i18n("Slot %1", slot).toUtf8().data() << std::endl;
    Kandas::SlotInfo info = m_slots[slot];
    std::cout << '\t' << i18n("Associated device: %1", info.device).toUtf8().data() << std::endl;
    switch (info.state)
    {
        case Kandas::Undetermined:
            std::cout << '\t' << i18n("State: undetermined").toUtf8().data() << std::endl;
            break;
        case Kandas::Disconnected:
            std::cout << '\t' << i18n("State: disconnected").toUtf8().data() << std::endl;
            break;
        case Kandas::Connected:
            std::cout << '\t' << i18n("State: connected").toUtf8().data() << std::endl;
            break;
        case Kandas::Disconnecting:
            std::cout << '\t' << i18n("State: disconnecting").toUtf8().data() << std::endl;
            break;
        case Kandas::Connecting:
            std::cout << '\t' << i18n("State: connecting").toUtf8().data() << std::endl;
            break;
    }
}

void Kandas::Console::Engine::upDevice(const QString &/*device*/)
{
}

void Kandas::Console::Engine::upSlot(int /*slot*/)
{
}

void Kandas::Console::Engine::downDevice(const QString &/*device*/)
{
}

void Kandas::Console::Engine::downSlot(int /*slot*/)
{
}
