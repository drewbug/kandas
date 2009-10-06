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

#include "info-worker.h"
#include "interface.h"

#include <iostream>
#include <KLocalizedString>

Kandas::Console::InfoWorker::InfoWorker(bool listEnv, bool listDevices, bool listSlots)
    : BaseWorker()
    , m_listEnv(listEnv)
    , m_listDevices(listDevices)
    , m_listSlots(listSlots)
{
}

bool Kandas::Console::InfoWorker::execute()
{
    if (m_listEnv)
    {
        listEnvironment();
    }
    if (m_listDevices)
    {
        if (m_listEnv)
            std::cout << std::endl; //some space between both lists
        if (devicesList().isEmpty())
            Kandas::Console::printMessage(i18n("No drives registered."));
        else
            listDevices();
    }
    if (m_listSlots && slotsList().count() != 0)
    {
        if (m_listDevices || m_listEnv)
            std::cout << std::endl; //some space between both lists
        listSlots();
    }
    //exit immediately
    return true;
}

void Kandas::Console::InfoWorker::listEnvironment()
{
    //print system state
    QString systemStateMessage;
    switch (system())
    {
        case Kandas::SystemUnchecked:
            systemStateMessage = i18nc("System state is unknown", "Unknown.");
            break;
        case Kandas::SystemChecked:
            systemStateMessage = i18nc("System state is alright", "Alright.");
            break;
        case Kandas::NoDriverFound:
            systemStateMessage = i18n("NDAS driver is not loaded.");
            break;
        case Kandas::NoAdminFound:
            systemStateMessage = i18n("NDAS driver is not installed.");
            break;
    }
    Kandas::Console::printMessage(i18n("System state: %1", systemStateMessage));
    //show KaNDAS version
    const QString dVer = interface()->daemonVersion(), iVer = interface()->interfaceVersion();
    Kandas::Console::printMessage(i18n("Driver management: KaNDASd %1, protocol version %2", dVer, iVer));
}

void Kandas::Console::InfoWorker::listDevices()
{
    static const QString deviceHeader = i18n("Drive");
    static const QString serialHeader = i18n("Serial");
    static const QString slotsHeader = i18n("Connection points");
    static const QString stateHeader = i18n("Current state");
    static const int columnPadding = 3;
    static const char paddingCharacterRaw = ' ';
    static const QChar paddingCharacter(paddingCharacterRaw);
    //gather information (note: the slot list is expected to always be shorter than the header!)
    int maxDeviceNameLength = deviceHeader.length();
    int maxSerialLength = serialHeader.length();
    QHash<QString, QList<int> > deviceSlots;
    foreach (Kandas::Console::Device* device, devicesList())
    {
        deviceSlots[device->name] = QList<int>();
        maxDeviceNameLength = qMax(maxDeviceNameLength, device->name.length());
        maxSerialLength = qMax(maxSerialLength, device->serial.length());
    }
    foreach (Kandas::Console::Slot* slot, slotsList())
    {
        QString device = slot->device;
        if (deviceSlots.contains(device))
            deviceSlots[device] << slot->number;
    }
    //output - header
    int deviceNameColumnWidth = maxDeviceNameLength + columnPadding;
    int serialColumnWidth = maxSerialLength + columnPadding;
    int slotsColumnWidth = slotsHeader.length() + columnPadding;
    std::cout << deviceHeader.toUtf8().data()
              << QString(deviceNameColumnWidth - deviceHeader.length(), paddingCharacter).toUtf8().data()
              << serialHeader.toUtf8().data()
              << QString(serialColumnWidth - serialHeader.length(), paddingCharacter).toUtf8().data()
              << slotsHeader.toUtf8().data()
              << QString(columnPadding, paddingCharacter).toUtf8().data() //the width of the slot list column is always defined by the header!
              << stateHeader.toUtf8().data()
              << std::endl;
    //output - lists
    foreach (Kandas::Console::Device* device, devicesList())
    {
        std::cout << device->name.toUtf8().data()
                  << QString(deviceNameColumnWidth - device->name.length(), paddingCharacter).toUtf8().data()
                  << device->serial.toUtf8().data()
                  << QString(serialColumnWidth - device->serial.length(), paddingCharacter).toUtf8().data();
        int slotsColumnContentWidth = 0;
        foreach (int slot, deviceSlots[device->name])
        {
            std::cout << slot << paddingCharacterRaw;
            slotsColumnContentWidth += 1 + QString::number(slot).length();
        }
        std::cout << QString(slotsColumnWidth - slotsColumnContentWidth, paddingCharacter).toUtf8().data();
        switch (device->state)
        {
            case DeviceOffline:
                std::cout << i18n("Offline").toUtf8().data() << std::endl;
                break;
            case DeviceOnline:
                std::cout << i18n("Online").toUtf8().data() << std::endl;
                break;
            case DeviceConnectionError:
                std::cout << i18n("Connection error").toUtf8().data() << std::endl;
                break;
            case DeviceLoginError:
                std::cout << i18n("Login error").toUtf8().data() << std::endl;
                break;
        }
    }
}

void Kandas::Console::InfoWorker::listSlots()
{
    static const QString slotHeader = i18n("Connection point");
    static const QString deviceHeader = i18n("At drive");
    static const QString blockDeviceHeader = i18n("UNIX device");
    static const QString stateHeader = i18n("Current state");
    static const int columnPadding = 3;
    static const char paddingCharacterRaw = ' ';
    static const QChar paddingCharacter(paddingCharacterRaw);
    //gather information
    int maxSlotNumberLength = slotHeader.length();
    int maxDeviceNameLength = deviceHeader.length();
    int maxBlockDeviceNameLength = blockDeviceHeader.length();
    foreach (Kandas::Console::Slot* slot, slotsList())
    {
        maxSlotNumberLength = qMax(maxSlotNumberLength, QString::number(slot->number).length());
        maxDeviceNameLength = qMax(maxDeviceNameLength, slot->device.length());
        maxBlockDeviceNameLength = qMax(maxBlockDeviceNameLength, slot->blockDevice.length());
    }
    //output - header
    int slotNumberColumnWidth = maxSlotNumberLength + columnPadding;
    int deviceNameColumnWidth = maxDeviceNameLength + columnPadding;
    int blockDeviceNameColumnWidth = maxBlockDeviceNameLength + columnPadding;
    std::cout << slotHeader.toUtf8().data()
              << QString(slotNumberColumnWidth - slotHeader.length(), paddingCharacter).toUtf8().data()
              << deviceHeader.toUtf8().data()
              << QString(deviceNameColumnWidth - deviceHeader.length(), paddingCharacter).toUtf8().data()
              << blockDeviceHeader.toUtf8().data()
              << QString(blockDeviceNameColumnWidth - blockDeviceHeader.length(), paddingCharacter).toUtf8().data()
              << stateHeader.toUtf8().data()
              << std::endl;
    //output - slots
    foreach (Kandas::Console::Slot* slot, slotsList())
    {
        std::cout << slot->number
                  << QString(slotNumberColumnWidth - QString::number(slot->number).length(), paddingCharacter).toUtf8().data()
                  << slot->device.toUtf8().data()
                  << QString(deviceNameColumnWidth - slot->device.length(), paddingCharacter).toUtf8().data()
                  << slot->blockDevice.toUtf8().data()
                  << QString(blockDeviceNameColumnWidth - slot->blockDevice.length(), paddingCharacter).toUtf8().data();
        switch (slot->state)
        {
            case Kandas::SlotOffline:
                std::cout << i18n("Offline").toUtf8().data() << std::endl;
                break;
            case Kandas::ConnectedSlot:
                std::cout << i18n("Connected").toUtf8().data() << std::endl;
                break;
            case Kandas::DisconnectedSlot:
                std::cout << i18n("Disconnected").toUtf8().data() << std::endl;
                break;
            case Kandas::ConnectingSlot:
                std::cout << i18n("Connecting").toUtf8().data() << std::endl;
                break;
            case Kandas::DisconnectingSlot:
                std::cout << i18n("Disconnecting").toUtf8().data() << std::endl;
                break;
        }
    }
}
