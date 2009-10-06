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

#include "up-worker.h"
#include "interface.h"

#include <iostream>
#include <QApplication>

Kandas::Console::UpWorker::UpWorker(QVariant target, bool readOnly)
    : Kandas::Console::BaseWorker()
    , m_readOnly(readOnly)
    , m_target(target)
{
    setAutoTimeout(true);
}

bool Kandas::Console::UpWorker::execute()
{
    if (m_target.type() == QVariant::Int)
        return connectSlot(m_target.toInt());
    else
        return connectDevice(m_target.toString());
}

bool Kandas::Console::UpWorker::connectDevice(const QString &deviceName)
{
    //check whether device exists
    if (!devicesList().device(deviceName))
    {
        Kandas::Console::printError(i18n("Drive \"%1\" is not available."));
        return true; //do nothing, exit immediately
    }
    //get a list of all slots that need to be up'ed, and send request to KaNDASd
    Kandas::Console::SlotList slotList = slotsList();
    foreach (Kandas::Console::Slot* slot, slotList)
    {
        if (slot->device == deviceName && slot->state == Kandas::DisconnectedSlot)
        {
            m_remainingSlots << slot->number;
            interface()->connectSlot(slot->number, m_readOnly);
        }
    }
    //exit immediately if nothing is to do, else wait for all slots to be connected
    if (m_remainingSlots.count() != 0)
    {
        Kandas::Console::printMessage(i18n("Establishing connection..."));
        connect(interface(), SIGNAL(slotInfo(int, const QString &, const QString &, int)), this, SLOT(slotChanged(int, const QString &, const QString &, int)));
        return false;
    }
    else
        return true;
}

bool Kandas::Console::UpWorker::connectSlot(int slotNumber)
{
    Kandas::Console::SlotList slotList = slotsList();
    //check whether slot exists
    Kandas::Console::Slot* slot = slotList.slot(slotNumber);
    if (!slot)
    {
        Kandas::Console::printMessage(i18n("Connection point %1 is not available.", slotNumber));
        return true; //do nothing, exit immediately
    }
    //check whether update is necessary
    if (slot->state != Kandas::DisconnectedSlot)
        return true;
    //issue update
    m_remainingSlots << slotNumber;
    interface()->connectSlot(slotNumber, m_readOnly);
    Kandas::Console::printMessage(i18n("Establishing connection..."));
    connect(interface(), SIGNAL(slotInfo(int, const QString &, const QString &, int)), this, SLOT(slotChanged(int, const QString &, const QString &, int)));
    return false;
}

void Kandas::Console::UpWorker::slotChanged(int slot, const QString &/*device*/, const QString &/*blockDevice*/, int newState)
{
    if (newState == Kandas::ConnectedSlot && m_remainingSlots.contains(slot))
    {
        m_remainingSlots.removeAll(slot);
        Kandas::Console::printMessage(i18n("Connection to point %1 established.", slot));
        if (m_remainingSlots.count() == 0)
            qApp->quit();
    }
}

#include "up-worker.moc"
