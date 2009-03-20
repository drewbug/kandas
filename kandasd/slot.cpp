/***************************************************************************
 *   Copyright (C) 2009 Stefan Majewsky <majewsky@gmx.net>
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

#include "slot.h"
#include "engine.h"

#include <QFile>

Kandas::Daemon::Slot::Slot(const QString &device, int slotNumber)
    : m_number(slotNumber)
    , m_device(device)
    , m_state(Kandas::SlotOffline)
{
    static const QString slotInfoPath = QString("%1/slots/%2/info").arg(Kandas::Daemon::Engine::InformationSourceDirectory);
    static const QString blockDeviceInfoPath = QString("%1/slots/%2/devname").arg(Kandas::Daemon::Engine::InformationSourceDirectory);
    //get name of block device
    QFile blockDeviceInfoFile(blockDeviceInfoPath.arg(slotNumber));
    QString blockDeviceNameTemplate("/dev/%1");
    if (blockDeviceInfoFile.exists() && blockDeviceInfoFile.open(QIODevice::ReadOnly) && blockDeviceInfoFile.isReadable())
        m_blockDevice = blockDeviceNameTemplate.arg(QString::fromUtf8(blockDeviceInfoFile.readAll()));
    blockDeviceInfoFile.close();
    //read info file
    const int bufferSize = 1024; char buffer[bufferSize];
    QFile slotInfoFile(slotInfoPath.arg(slotNumber));
    if (!slotInfoFile.exists() || !slotInfoFile.open(QIODevice::ReadOnly) || !slotInfoFile.isReadable())
        return;
    slotInfoFile.readLine(buffer, bufferSize); //skip first line (human-readable captions)
    if (slotInfoFile.readLine(buffer, bufferSize) == -1)
        return;
    const QString line = QString::fromUtf8(buffer).simplified();
    slotInfoFile.close();
    //get state from info file
    m_state = (line.section(' ', 0, 0) == "Enabled") ? Kandas::ConnectedSlot : Kandas::DisconnectedSlot;
}

int Kandas::Daemon::Slot::number() const
{
    return m_number;
}

QString Kandas::Daemon::Slot::deviceName() const
{
    return m_device;
}

QString Kandas::Daemon::Slot::blockDeviceName() const
{
    return m_blockDevice;
}

Kandas::SlotState Kandas::Daemon::Slot::state() const
{
    return m_state;
}

void Kandas::Daemon::Slot::setState(Kandas::SlotState state)
{
    m_state = state;
}

void Kandas::Daemon::Slot::setPreviousState(Kandas::SlotState state)
{
    //stay in transitional state if target state has not yet been reached
    bool case1 = (m_state == Kandas::ConnectedSlot && state == Kandas::DisconnectingSlot);
    bool case2 = (m_state == Kandas::DisconnectedSlot && state == Kandas::ConnectingSlot);
    if (case1 || case2)
        m_state = state;
}

//BEGIN Kandas::Daemon::SlotList

Kandas::Daemon::SlotList::SlotList()
{
}

Kandas::Daemon::SlotList::SlotList(const Kandas::Daemon::SlotList& other)
    : QList<Kandas::Daemon::Slot*>(other)
{
}

bool Kandas::Daemon::SlotList::contains(int slotNumber) const
{
    foreach (Kandas::Daemon::Slot* slot, *this)
        if (slot->number() == slotNumber)
            return true;
    return false;
}

Kandas::Daemon::Slot* Kandas::Daemon::SlotList::slot(int slotNumber) const
{
    foreach (Kandas::Daemon::Slot* slot, *this)
        if (slot->number() == slotNumber)
            return slot;
    return 0;
}

//END Kandas::Daemon::SlotList
