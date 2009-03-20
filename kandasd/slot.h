/***************************************************************************
 *   Copyright 2009 Stefan Majewsky <majewsky@gmx.net>
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

#ifndef KANDAS_DAEMON_SLOT_H
#define KANDAS_DAEMON_SLOT_H

#include "definitions.h"
#include <QList>

namespace Kandas
{
    namespace Daemon
    {

        class Slot
        {
            public:
                Slot(const QString &device, int slotNumber);

                int number() const;
                QString deviceName() const;
                QString blockDeviceName() const; //something like /dev/ndas-12345678-0
                Kandas::SlotState state() const;

                void setState(Kandas::SlotState state);
                void setPreviousState(Kandas::SlotState state); //used to keep transitional states when appropriate
            private:
                int m_number;
                QString m_device, m_blockDevice;
                Kandas::SlotState m_state;
        };

        class SlotList : public QList<Kandas::Daemon::Slot*>
        {
            public:
                SlotList();
                SlotList(const Kandas::Daemon::SlotList& other);

                //helper functions (similar to QList::contains and QList::value) that look for slots with a given slot number
                bool contains(int slotNumber) const;
                Kandas::Daemon::Slot* slot(int slotNumber) const;
        };

    }
}

#endif // KANDAS_DAEMON_SLOT_H
