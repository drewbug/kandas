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

#ifndef KANDAS_CONSOLE_DOWNWORKER_H
#define KANDAS_CONSOLE_DOWNWORKER_H

#include "base-worker.h"

#include <QVariant>

namespace Kandas
{
    namespace Console
    {

        class DownWorker : public BaseWorker
        {
            Q_OBJECT
            public:
                DownWorker(QVariant target);

                virtual bool execute();
            private slots:
                void slotChanged(int slot, const QString &device, int newState);
            private:
                bool disconnectDevice(const QString &device);
                bool disconnectSlot(int slot);

                QVariant m_target;
                QList<int> m_remainingSlots;
        };

    }
}

#endif // KANDAS_CONSOLE_DOWNWORKER_H
