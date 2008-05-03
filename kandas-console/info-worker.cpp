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
        listEnvironment();
    if (m_listDevices && devicesList().count() != 0)
    {
        if (m_listEnv)
            std::cout << std::endl; //some space between both lists
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
    std::cout << i18n("System state:").toUtf8().data() << ' ';
    switch (system())
    {
        case Kandas::SystemUnchecked:
            std::cout << i18n("Could not be determined").toUtf8().data() << std::endl;
            break;
        case Kandas::SystemChecked:
            std::cout << i18nc("System state is alright", "Alright").toUtf8().data() << std::endl;
            break;
        case Kandas::NoDriverFound:
            std::cout << i18n("NDAS driver not installed or not running").toUtf8().data() << std::endl;
            break;
        case Kandas::NoAdminFound:
            std::cout << i18n("NDAS administration tool not installed.").toUtf8().data() << std::endl;
            break;
    }
    //show KaNDAS version
    std::cout << i18n("NDAS management: KaNDASd %1", interface()->daemonVersion()).toUtf8().data() << std::endl;
}

void Kandas::Console::InfoWorker::listDevices()
{
    static const QString deviceHeader = i18n("Device");
    static const QString slotsHeader = i18n("Associated slots");
    static const int columnPadding = 3;
    static const char paddingCharacterRaw = ' ';
    static const QChar paddingCharacter(paddingCharacterRaw);
    //gather information
    int maxDeviceNameLength = deviceHeader.length();
    QHash<QString, QList<int> > deviceSlots;
    foreach (QString device, devicesList())
    {
        deviceSlots[device] = QList<int>();
        maxDeviceNameLength = qMax(maxDeviceNameLength, device.length());
    }
    QHashIterator<int, Kandas::SlotInfo> iterSlots(slotsList());
    while (iterSlots.hasNext())
    {
        QString device = iterSlots.next().value().device;
        if (deviceSlots.contains(device))
            deviceSlots[device] << iterSlots.key();
    }
    //output - header
    int deviceNameColumnWidth = maxDeviceNameLength + columnPadding;
    std::cout << deviceHeader.toUtf8().data()
              << QString(deviceNameColumnWidth - deviceHeader.length(), paddingCharacter).toUtf8().data()
              << slotsHeader.toUtf8().data()
              << std::endl;
    //output - lists
    QHashIterator<QString, QList<int> > iterDevices(deviceSlots);
    while (iterDevices.hasNext())
    {
        QString device = iterDevices.next().key();
        std::cout << device.toUtf8().data()
                  << QString(deviceNameColumnWidth - device.length(), paddingCharacter).toUtf8().data();
        foreach (int slot, iterDevices.value())
            std::cout << slot << paddingCharacterRaw;
        std::cout << std::endl;
    }
}

void Kandas::Console::InfoWorker::listSlots()
{
    static const QString slotHeader = i18n("Slot");
    static const QString deviceHeader = i18n("At device");
    static const QString stateHeader = i18n("Current state");
    static const QString integerToString("%1");
    static const int columnPadding = 3;
    static const char paddingCharacterRaw = ' ';
    static const QChar paddingCharacter(paddingCharacterRaw);
    //gather information
    int maxSlotNumberLength = slotHeader.length();
    int maxDeviceNameLength = deviceHeader.length();
    QHashIterator<int, Kandas::SlotInfo> iterSlots(slotsList());
    while (iterSlots.hasNext())
    {
        iterSlots.next();
        maxSlotNumberLength = qMax(maxSlotNumberLength, integerToString.arg(iterSlots.key()).length());
        maxDeviceNameLength = qMax(maxDeviceNameLength, iterSlots.value().device.length());
    }
    //output - header
    int slotNumberColumnWidth = maxSlotNumberLength + columnPadding;
    int deviceNameColumnWidth = maxDeviceNameLength + columnPadding;
    std::cout << slotHeader.toUtf8().data()
              << QString(slotNumberColumnWidth - slotHeader.length(), paddingCharacter).toUtf8().data()
              << deviceHeader.toUtf8().data()
              << QString(deviceNameColumnWidth - deviceHeader.length(), paddingCharacter).toUtf8().data()
              << stateHeader.toUtf8().data()
              << std::endl;
    //output - slots
    iterSlots.toFront();
    while (iterSlots.hasNext())
    {
        Kandas::SlotInfo info = iterSlots.next().value();
        std::cout << iterSlots.key()
                  << QString(slotNumberColumnWidth - integerToString.arg(iterSlots.key()).length(), paddingCharacter).toUtf8().data()
                  << info.device.toUtf8().data()
                  << QString(deviceNameColumnWidth - info.device.length(), paddingCharacter).toUtf8().data();
        switch (info.state)
        {
            case Kandas::Unknown:
                std::cout << i18n("Undetermined").toUtf8().data() << std::endl;
                break;
            case Kandas::Connected:
                std::cout << i18n("Connected").toUtf8().data() << std::endl;
                break;
            case Kandas::Disconnected:
                std::cout << i18n("Disconnected").toUtf8().data() << std::endl;
                break;
            case Kandas::Connecting:
                std::cout << i18n("Connecting").toUtf8().data() << std::endl;
                break;
            case Kandas::Disconnecting:
                std::cout << i18n("Disconnecting").toUtf8().data() << std::endl;
                break;
        }
    }
}
