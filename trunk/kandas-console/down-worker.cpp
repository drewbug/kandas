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

#include "down-worker.h"
#include "interface.h"

#include <iostream>
#include <QApplication>

Kandas::Console::DownWorker::DownWorker(QVariant target)
    : Kandas::Console::BaseWorker()
    , m_target(target)
{
    setAutoTimeout(true);
}

bool Kandas::Console::DownWorker::execute()
{
    if (m_target.type() == QVariant::Int)
        return disconnectSlot(m_target.toInt());
    else
        return disconnectDevice(m_target.toString());
}

bool Kandas::Console::DownWorker::disconnectDevice(const QString &deviceName)
{
    //check whether device exists
    if (!devicesList().device(deviceName))
    {
        std::cerr << i18n("ERROR: Device \"%1\" is not available.", deviceName).toUtf8().data();
        return true; //do nothing, exit immediately
    }
    //get a list of all slots that need to be down'ed, and send request to KaNDASd
    Kandas::Console::SlotList slotList = slotsList();
    foreach (Kandas::Console::Slot* slot, slotList)
    {
        if (slot->device == deviceName && slot->state == Kandas::ConnectedSlot)
        {
            m_remainingSlots << slot->number;
            interface()->disconnectSlot(slot->number);
        }
    }
    //exit immediately if nothing is to do, else wait for all slots to be connected
    if (m_remainingSlots.count() != 0)
    {
        std::cout << i18np("Waiting for 1 slot to disconnect...", "Waiting for %1 slots to disconnect...", m_remainingSlots.count()).toUtf8().data() << std::endl;
        connect(interface(), SIGNAL(slotInfo(int, const QString &, const QString &, int)), this, SLOT(slotChanged(int, const QString &, const QString &, int)));
        return false;
    }
    else
        return true;
}

bool Kandas::Console::DownWorker::disconnectSlot(int slotNumber)
{
    Kandas::Console::SlotList slotList = slotsList();
    //check whether slot exists
    Kandas::Console::Slot* slot = slotList.slot(slotNumber);
    if (!slot)
    {
        std::cerr << i18n("ERROR: Slot %1 is not available.", slotNumber).toUtf8().data();
        return true; //do nothing, exit immediately
    }
    //check whether update is necessary
    if (slot->state != Kandas::ConnectedSlot)
        return true;
    //issue update
    m_remainingSlots << slotNumber;
    interface()->disconnectSlot(slotNumber);
    std::cout << i18n("Waiting for slot to disconnect...").toUtf8().data() << std::endl;
    connect(interface(), SIGNAL(slotInfo(int, const QString &, const QString &, int)), this, SLOT(slotChanged(int, const QString &, const QString &, int)));
    return false;
}

void Kandas::Console::DownWorker::slotChanged(int slot, const QString &/*device*/, const QString &/*blockDevice*/, int newState)
{
    if (newState == Kandas::DisconnectedSlot && m_remainingSlots.contains(slot))
    {
        m_remainingSlots.removeAll(slot);
        std::cout << i18n("Slot %1 disconnected.", slot).toUtf8().data() << std::endl;
        if (m_remainingSlots.count() == 0)
            qApp->quit();
    }
}

#include "down-worker.moc"
