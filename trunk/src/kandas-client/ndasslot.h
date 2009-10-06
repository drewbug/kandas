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

#ifndef KANDAS_CLIENT_NDASSLOT_H
#define KANDAS_CLIENT_NDASSLOT_H

#include "ndasdata.h"

#include <QString>
#include <kandas-daemon/definitions.h>

namespace Kandas
{
    namespace Client
    {

        class NdasSlot : public Kandas::Client::NdasData
        {
            public:
                NdasSlot(int number, const QString &deviceName, const QString &blockDeviceName, Kandas::SlotState state);
                virtual ~NdasSlot();
                void update(const QString &deviceName, const QString &blockDeviceName, Kandas::SlotState state);

                int number() const;
                QString deviceName() const;
                QString blockDeviceName() const;
                Kandas::SlotState state() const;

                virtual QVariant data(int role) const;
            private:
                int m_number;
                QString m_deviceName, m_blockDeviceName;
                Kandas::SlotState m_state;
        };

    }
}

#endif // KANDAS_CLIENT_NDASSLOT_H
