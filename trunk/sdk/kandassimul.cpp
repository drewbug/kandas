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

void Kandas::Simulator::addDevice(const QString &device)
{
    if (!m_devices.contains(device))
    {
        m_devices << device;
        write();
        std::cout << "Device added." << std::endl;
    }
    else
        std::cout << "ERROR: Device was already added." << std::endl;
}

void Kandas::Simulator::removeDevice(const QString &device)
{
    if (m_devices.contains(device))
    {
        m_devices.removeAll(device);
        QMutableHashIterator<int, Kandas::SlotInfo> iterSlots(m_slots);
        while (iterSlots.hasNext())
        {
            if (iterSlots.next().value().device == device)
                iterSlots.remove();
        }
        write();
        std::cout << "Device removed." << std::endl;
    }
    else
        std::cout << "ERROR: Device does not exist." << std::endl;
}

void Kandas::Simulator::setSlot(int slot, const QString &device, int state)
{
    if (!m_devices.contains(device))
        m_devices << device;
    if (m_slots.contains(slot))
    {
        m_slots[slot].state = (Kandas::SlotState) state;
        m_slots[slot].device = device;
        write();
        std::cout << "Slot updated." << std::endl;
    }
    else
    {
        m_slots[slot] = Kandas::SlotInfo(device, (Kandas::SlotState) state);
        write();
        std::cout << "Slot added." << std::endl;
    }
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
    //create and flush directory
    QDir baseDir(m_baseDirectory);
    if (!baseDir.exists())
        baseDir.mkpath(m_baseDirectory);
    //write device list
    QFile devList(QString("%1/devs").arg(m_baseDirectory));
    if (devList.open(QIODevice::WriteOnly | QIODevice::Truncate) && devList.isWritable())
    {
        devList.write(QByteArray("#discard\n"));
        foreach (QString device, m_devices)
            devList.write(QString("%1\n").arg(device).toUtf8());
    }
    devList.close();
    //for each device, write slot list
    QHashIterator<int, Kandas::SlotInfo> iterSlots(m_slots);
    foreach (QString device, m_devices)
    {
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
        if (iterSlots.value().state == Kandas::Undetermined)
            continue;
        baseDir.mkpath(QString("%1/slots/%2").arg(m_baseDirectory).arg(iterSlots.key()));
        QFile slotInfo(QString("%1/slots/%2/info").arg(m_baseDirectory).arg(iterSlots.key()));
        if (slotInfo.open(QIODevice::WriteOnly | QIODevice::Truncate) && slotInfo.isWritable())
        {
            slotInfo.write(QByteArray("#discard\n"));
            slotInfo.write((iterSlots.value().state == Kandas::Connected || iterSlots.value().state == Kandas::Disconnecting) ? "Enabled\n" : "Disabled\n");
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
        char buffer[1024];
        std::cin.getline(buffer, 1024);
        QString command = QString(buffer).simplified();
        QStringList args = command.split(' ');
        QString commandName = args.at(0);
        if (commandName == "dev")
        {
            if (args.count() != 2)
            {
                std::cout << "ERROR: Wrong argument count." << std::endl;
                continue;
            }
            addDevice(args[1]);
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
            setSlot(args[1].toInt(), args[2], args[3].toInt());
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
                "dev [name]                       Adds a device with this name.\n"
                "rmdev [name]                     Removes a device with this name.\n"
                "slot [number] [device] [state]   Adds or changes this slot.\n"
                "rmslot [number]                  Removes this slot.\n"
                "quit | exit | help               Just too obvious.\n\n"
                "Possible states for the 'slot' command are:\n\n"
                "10                               Slot is disconnected.\n"
                "11                               Slot is connected.\n"
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

