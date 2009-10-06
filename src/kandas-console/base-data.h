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

#ifndef KANDAS_CONSOLE_BASEDATA_H
#define KANDAS_CONSOLE_BASEDATA_H

#include <kandasd/definitions.h>
#include <QList>

namespace Kandas
{
    namespace Console
    {

        struct Device
        {
            QString name, serial;
            Kandas::DeviceState state;
            bool hasWriteKey;
        };

        struct Slot
        {
            int number;
            QString device, blockDevice;
            Kandas::SlotState state;
        };

        class DeviceList : public QList<Kandas::Console::Device*>
        {
            public:
                Kandas::Console::Device* device(const QString &name)
                {
                    foreach (Kandas::Console::Device* theDevice, *this)
                        if (theDevice->name == name)
                            return theDevice;
                    return 0;
                }
        };

        class SlotList : public QList<Kandas::Console::Slot*>
        {
            public:
                Kandas::Console::Slot* slot(int number)
                {
                    foreach (Kandas::Console::Slot* theSlot, *this)
                        if (theSlot->number == number)
                            return theSlot;
                    return 0;
                }
        };

    }
}

#endif // KANDAS_CONSOLE_BASEDATA_H
