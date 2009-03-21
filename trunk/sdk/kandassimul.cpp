/***************************************************************************
 *   Copyright 2008 Stefan Majewsky <majewsky@gmx.net>
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

#include "kandassimul.h"

#include <iostream>
#include <QDir>
#include <QFile>

Kandas::Simulator::Simulator(const QString &baseDirectory)
    : m_baseDirectory(baseDirectory)
{
}

Kandas::Simulator::~Simulator()
{
}

void Kandas::Simulator::setDevice(const QString &device, const QString &serial, Kandas::DeviceState state, bool hasWriteKey)
{
    if (!m_devices.contains(device))
    {
        m_devices[device] = Kandas::DeviceSimulation();
        std::cout << "Device added." << std::endl;
    }
    else
        std::cout << "Device updated." << std::endl;
    m_devices[device].serial = serial;
    m_devices[device].state = state;
    m_devices[device].hasWriteKey = hasWriteKey;
    write();
}

void Kandas::Simulator::removeDevice(const QString &device)
{
    if (m_devices.contains(device))
    {
        m_devices.remove(device);
        std::cout << "Device removed." << std::endl;
        QMutableHashIterator<int, Kandas::SlotSimulation> iterSlots(m_slots);
        while (iterSlots.hasNext())
        {
            if (iterSlots.next().value().device == device)
            {
                std::cout << "Slot " << iterSlots.key() << " auto-removed." << std::endl;
                iterSlots.remove();
            }
        }
        write();
    }
    else
        std::cout << "ERROR: Device does not exist." << std::endl;
}

void Kandas::Simulator::setSlot(int slot, const QString &device, Kandas::SlotState state)
{
    if (!m_devices.contains(device))
    {
        std::cout << "ERROR: Device does not exist." << std::endl;
        return;
    }
    if (!m_slots.contains(slot))
    {
        m_slots[slot] = Kandas::SlotSimulation();
        std::cout << "Slot added." << std::endl;
    }
    else
        std::cout << "Slot updated." << std::endl;
    m_slots[slot].device = device;
    m_slots[slot].blockDevice = QString("ndassimul-%1-0").arg(m_devices[device].serial);
    m_slots[slot].state = state;
    write();
}

void Kandas::Simulator::removeSlot(int slot)
{
    if (m_slots.contains(slot))
    {
        m_slots.remove(slot);
        write();
        std::cout << "Slot removed." << std::endl;
    }
    else
        std::cout << "ERROR: Slot does not exist." << std::endl;
}

void Kandas::Simulator::write()
{
    QHashIterator<QString, Kandas::DeviceSimulation> iterDevices(m_devices);
    QHashIterator<int, Kandas::SlotSimulation> iterSlots(m_slots);
    //create and flush directory
    QDir baseDir(m_baseDirectory);
    if (!baseDir.exists())
        baseDir.mkpath(m_baseDirectory);
    //write device list
    QFile devList(QString("%1/devs").arg(m_baseDirectory));
    if (devList.open(QIODevice::WriteOnly | QIODevice::Truncate) && devList.isWritable())
    {
        devList.write(QByteArray("#discard\n"));
        iterDevices.toFront();
        while (iterDevices.hasNext())
        {
            Kandas::DeviceSimulation device = iterDevices.next().value();
            QString deviceName = iterDevices.key();
            QString lineTemplate("%1 discard %2 %3 discard %4 discard\n");
            QString stateText;
            switch (device.state)
            {
                case Kandas::DeviceOffline: stateText = "Offline"; break;
                case Kandas::DeviceOnline: stateText = "Online"; break;
                case Kandas::DeviceConnectionError: stateText = "Connection Error"; break;
                case Kandas::DeviceLoginError: stateText = "Login Error"; break;
            }
            devList.write(lineTemplate
                .arg(deviceName)
                .arg(device.hasWriteKey ? "true" : "false")
                .arg(device.serial)
                .arg(stateText).toUtf8()
            );
        }
    }
    devList.close();
    //for each device, write slot list
    iterDevices.toFront();
    while (iterDevices.hasNext())
    {
        QString device = iterDevices.next().key();
        baseDir.mkpath(QString("%1/devices/%2").arg(m_baseDirectory).arg(device));
        QFile slotList(QString("%1/devices/%2/slots").arg(m_baseDirectory).arg(device));
        if (slotList.open(QIODevice::WriteOnly | QIODevice::Truncate) && slotList.isWritable())
        {
            iterSlots.toFront();
            while (iterSlots.hasNext())
            {
                iterSlots.next();
                if (iterSlots.value().device == device)
                    slotList.write(QString("%1\n").arg(iterSlots.key()).toUtf8());
            }
        }
        slotList.close();
    }
    //for each slot, write some info
    iterSlots.toFront();
    while (iterSlots.hasNext())
    {
        iterSlots.next();
        if (iterSlots.value().state == Kandas::SlotOffline)
            continue;
        baseDir.mkpath(QString("%1/slots/%2").arg(m_baseDirectory).arg(iterSlots.key()));
        QFile slotDevName(QString("%1/slots/%2/devname").arg(m_baseDirectory).arg(iterSlots.key()));
        if (slotDevName.open(QIODevice::WriteOnly | QIODevice::Truncate) && slotDevName.isWritable())
            slotDevName.write(iterSlots.value().blockDevice.toUtf8());
        slotDevName.close();
        QFile slotInfo(QString("%1/slots/%2/info").arg(m_baseDirectory).arg(iterSlots.key()));
        if (slotInfo.open(QIODevice::WriteOnly | QIODevice::Truncate) && slotInfo.isWritable())
        {
            slotInfo.write(QByteArray("#discard\n"));
            slotInfo.write((iterSlots.value().state == Kandas::ConnectedSlot || iterSlots.value().state == Kandas::DisconnectingSlot) ? "Enabled\n" : "Disabled\n");
        }
        slotInfo.close();
    }
}

void Kandas::Simulator::loop()
{
    std::cout << "Welcome to KaNDASsimul. Type 'help' for help." << std::endl;
    forever
    {
        std::cout << "KaNDASsimul:" << m_baseDirectory.toUtf8().data() << "> ";
        const int bufferSize = 1024; char buffer[bufferSize];
        std::cin.getline(buffer, bufferSize);
        QString command = QString(buffer).simplified();
        QStringList args = command.split(' ');
        QString commandName = args.at(0);
        if (commandName == "dev")
        {
            if (args.count() != 5)
            {
                std::cout << "ERROR: Wrong argument count." << std::endl;
                continue;
            }
            setDevice(args[1], args[2], (Kandas::DeviceState) args[3].toInt(), args[4].toInt() != 0);
        }
        else if (commandName == "rmdev")
        {
            if (args.count() != 2)
            {
                std::cout << "ERROR: Wrong argument count." << std::endl;
                continue;
            }
            removeDevice(args[1]);
        }
        else if (commandName == "slot")
        {
            if (args.count() != 4)
            {
                std::cout << "ERROR: Wrong argument count." << std::endl;
                continue;
            }
            setSlot(args[1].toInt(), args[2], (Kandas::SlotState) args[3].toInt());
        }
        else if (commandName == "rmslot")
        {
            if (args.count() != 2)
            {
                std::cout << "ERROR: Wrong argument count." << std::endl;
                continue;
            }
            removeSlot(args[1].toInt());
        }
        else if (commandName == "quit" || commandName == "exit")
        {
            break;
        }
        else if (commandName == "help")
        {
            std::cout << "\n"
                "KaNDASsimul simulates an NDAS driver's procfs in the given directory.\n"
                "Available commands on the KaNDASsimul prompt are:\n\n"
                "dev [name] [serial] [state] [writable] Adds or changes this device.\n"
                "rmdev [name]                           Removes this device.\n"
                "slot [number] [device] [state]         Adds or changes this slot.\n"
                "rmslot [number]                        Removes this slot.\n"
                "quit | exit | help                     Just too obvious.\n\n"
                "Possible values for 'writable' are 0 or 1.\n\n"
                "Possible states for the 'dev' command are:\n"
                "0                                      Device is offline.\n"
                "1                                      Device is online.\n"
                "10                                     Connection error on device.\n"
                "11                                     Login error on device.\n\n"
                "Possible states for the 'slot' command are:\n"
                "10                                     Slot is disconnected.\n"
                "11                                     Slot is connected.\n\n"
                "It is not possible to emulate unavailability and transition states.\n"
            << std::endl;
        }
        else
        {
            std::cout << "ERROR: Unknown command name. Type 'help' for help." << std::endl;
        }
    }
}

int main(int argc, char **argv)
{
    if (argc != 2)
    {
        std::cout << "Usage: kandassimul [directory]" << std::endl;
        std::cout << "KaNDASsimul simulates an NDAS driver's procfs tree in the given directory." << std::endl;
    }
    Kandas::Simulator sim(argv[1]);
    sim.loop();
    return 0;
}

