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

#include "device.h"
#include "slotmanager.h"

#include <QHash>
#include <QPushButton>
#include <QVBoxLayout>

namespace Kandas
{
    namespace Client
    {

        class DevicePrivate
        {
            public:
                DevicePrivate(const QString &name, Device *device)
                    : m_name(name)
                    , m_slotMgr(0)
                {
                    device->setLayout(&m_layout);
                }
                ~DevicePrivate() {}

                QString m_name;
                QVBoxLayout m_layout;
                //members for single-slot mode
                SlotManager *m_slotMgr; //is 0 when in multi-slot mode or when no slots have been added
                int m_singleSlot;
                //members for multi-slot mode
                QHash<int, SlotManager *> m_slotMgrs;
        };

    }
}

Kandas::Client::Device::Device(const QString &name, QWidget *parent)
    : QGroupBox(name, parent)
    , p(new DevicePrivate(name, this))
{
    addSlot(1, 10);
    addSlot(2, 11);
    addSlot(3, 20);
}

Kandas::Client::Device::~Device()
{
    delete p;
}

void Kandas::Client::Device::addSlot(int slot, int state)
{
    if (p->m_slotMgr == 0)
    {
        if (p->m_slotMgrs.count() == 0)
        {
            //everything empty - enter single-slot mode with first slot
            p->m_slotMgr = new Kandas::Client::SlotManager(slot, state, this);
            p->m_singleSlot = slot;
            p->m_layout.addWidget(p->m_slotMgr);
        }
        else
        {
            //multi-slot mode - add another slot (if slot has already been added, change it only)
            if (p->m_slotMgrs.contains(slot))
                p->m_slotMgrs[slot]->update(state);
            else
            {
                Kandas::Client::SlotManager *mgr = new Kandas::Client::SlotManager(slot, state, this);
                p->m_slotMgrs[slot] = mgr;
                p->m_layout.addWidget(mgr);
            }
        }
    }
    else
    {
        //single-slot mode - if slot has already been added, change it; if a new slot is to be added, enter multi-slot mode
        if (p->m_singleSlot == slot)
            p->m_slotMgr->update(state);
        else
        {
            p->m_slotMgrs[p->m_singleSlot] = p->m_slotMgr;
            p->m_slotMgr = 0;
            p->m_singleSlot = 0;
            Kandas::Client::SlotManager *mgr = new Kandas::Client::SlotManager(slot, state, this);
            p->m_slotMgrs[slot] = mgr;
            p->m_layout.addWidget(mgr);
        }
    }
}

bool Kandas::Client::Device::changeSlot(int slot, int state)
{
    if (p->m_slotMgr == 0)
    {
        //multi-slot mode
        if (p->m_slotMgrs.contains(slot))
        {
            p->m_slotMgrs[slot]->update(state);
            return true;
        }
        else
            return false;
    }
    else
    {
        //single-slot mode
        if (p->m_singleSlot == slot)
        {
            p->m_slotMgr->update(state);
            return true;
        }
        else
            return false;
    }
}

bool Kandas::Client::Device::removeSlot(int slot)
{
    if (p->m_slotMgr == 0)
    {
        //multi-slot mode - search for slot
        if (p->m_slotMgrs.contains(slot))
        {
            Kandas::Client::SlotManager *mgr = p->m_slotMgrs.take(slot);
            p->m_layout.removeWidget(mgr);
            delete mgr;
            //check if only one slot is left, and enter single-slot mode if necessary
            if (p->m_slotMgrs.count() == 1)
            {
                QMutableHashIterator<int, Kandas::Client::SlotManager *> iterSlotMgrs(p->m_slotMgrs);
                iterSlotMgrs.next();
                p->m_slotMgr = iterSlotMgrs.value();
                p->m_singleSlot = iterSlotMgrs.key();
                iterSlotMgrs.remove();
            }
            return true;
        }
        else //slot manager not found in hash
            return false;
    }
    else
    {
        //single-slot mode - remove last slot
        if (p->m_singleSlot == slot)
        {
            Kandas::Client::SlotManager *mgr = p->m_slotMgr;
            p->m_slotMgr = 0;
            p->m_singleSlot = 0;
            p->m_layout.removeWidget(mgr);
            delete mgr;
            return true;
        }
        else
            return false;
    }
}

#include "device.moc"
