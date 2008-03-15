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
#include <QApplication>
#include <QHash>
#include <QList>
#include <KLocalizedString>
#include <kandasd/definitions.h>

//TODO: Abstract the whole initiation thing into a Kandas::Console::Worker class.
//TODO: Find out whether there is a KaNDASd instance running. If no, immediately exit.

namespace Kandas
{
    namespace Console
    {

        class InfoWorkerPrivate
        {
            public:
                InfoWorkerPrivate(bool listDevices, bool listSlots, InfoWorker *parent);
                ~InfoWorkerPrivate();

                bool m_listDevices, m_listSlots, m_clean;
                OrgKdeKandasInterface *m_interface;

                Kandas::EnvironmentState m_environment;
                QList<QString> m_devices;
                QHash<int, Kandas::SlotInfo> m_slots;
        };

    }
}

Kandas::Console::InfoWorkerPrivate::InfoWorkerPrivate(bool listDevices, bool listSlots, InfoWorker *parent)
    : m_listDevices(listDevices)
    , m_listSlots(listSlots)
    , m_clean(true)
    , m_interface(new OrgKdeKandasInterface("org.kde.kandas", "/", QDBusConnection::systemBus(), parent))
    , m_environment(Kandas::UnknownEnvironment)
{
    if (m_interface->engineVersion().value() == "") //no KaNDASd instance running
    {
        std::cerr << i18n("ERROR: KaNDASd is not running.").toUtf8().data() << std::endl;
        m_clean = false;
    }
}

Kandas::Console::InfoWorkerPrivate::~InfoWorkerPrivate()
{
    delete m_interface;
}

Kandas::Console::InfoWorker::InfoWorker(bool listDevices, bool listSlots)
    : p(new InfoWorkerPrivate(listDevices, listSlots, this))
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

Kandas::Console::InfoWorker::~InfoWorker()
{
    p->m_interface->unregisterClient();
    delete p;
}

bool Kandas::Console::InfoWorker::clean() const
{
    return p->m_clean;
}

void Kandas::Console::InfoWorker::initEnvironment(int state)
{
    switch (state)
    {
        case Kandas::SaneEnvironment: p->m_environment = Kandas::SaneEnvironment; break;
        case Kandas::NoDriverFound: p->m_environment = Kandas::NoDriverFound; break;
        case Kandas::NoAdminFound: p->m_environment = Kandas::NoAdminFound; break;
        default: p->m_environment = Kandas::UnknownEnvironment; break;
    }
}

void Kandas::Console::InfoWorker::initDevice(const QString &device)
{
    p->m_devices << device;
}

void Kandas::Console::InfoWorker::initSlot(int slot, const QString &device, int state)
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

void Kandas::Console::InfoWorker::executeJobs()
{
    if (p->m_listDevices)
        listDevices();
    if (p->m_listSlots)
    {
        if (p->m_listDevices)
            std::cout << std::endl << std::endl; //some space between both lists
        listSlots();
    }
    qApp->quit();
}

void Kandas::Console::InfoWorker::listDevices()
{
    if (p->m_devices.count() == 0)
        return;
    static const QString deviceHeader = i18n("Device");
    static const QString slotsHeader = i18n("Associated slots");
    static const int columnPadding = 3;
    static const char paddingCharacterRaw = ' ';
    static const QChar paddingCharacter(paddingCharacterRaw);
    //gather information
    int maxDeviceNameLength = deviceHeader.length();
    QHash<QString, QList<int> > deviceSlots;
    foreach (QString device, p->m_devices)
    {
        deviceSlots[device] = QList<int>();
        maxDeviceNameLength = qMax(maxDeviceNameLength, device.length());
    }
    QHashIterator<int, Kandas::SlotInfo> iterSlots(p->m_slots);
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
    if (p->m_slots.count() == 0)
        return;
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
    QHashIterator<int, Kandas::SlotInfo> iterSlots(p->m_slots);
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
            case Kandas::Undetermined:
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

#include "info-worker.moc"
