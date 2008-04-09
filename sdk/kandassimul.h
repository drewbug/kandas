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

#ifndef KANDASSDK_KANDASSIMUL_H
#define KANDASSDK_KANDASSIMUL_H

#include <QHash>
#include <QList>
#include <QString>
#include <kandasd/definitions.h>

namespace Kandas
{

    class Simulator
    {
        public:
            Simulator(const QString &baseDirectory);
            ~Simulator();

            void loop();
        protected:
            void addDevice(const QString &device);
            void removeDevice(const QString &device);
            void setSlot(int slot, const QString &device, int state);
            void removeSlot(int slot);
            void write();
        private:
            QHash<int, Kandas::SlotInfo> m_slots;
            QList<QString> m_devices;
            QString m_baseDirectory;
    };

}

#endif // KANDASSDK_KANDASSIMUL_H
