/***************************************************************************
 *   Copyright 2008-2009 Stefan Majewsky <majewsky@gmx.net>
4 *
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

#ifndef KANDAS_CONSOLE_UPWORKER_H
#define KANDAS_CONSOLE_UPWORKER_H

#include "base-worker.h"

#include <QVariant>

namespace Kandas
{
    namespace Console
    {

        class UpWorker : public BaseWorker
        {
            Q_OBJECT
            public:
                UpWorker(QVariant target, bool readOnly);

                virtual bool execute();
            private slots:
                void slotChanged(int slot, const QString &device, const QString &blockDevice, int newState);
            private:
                bool connectDevice(const QString &device);
                bool connectSlot(int slot);

                bool m_readOnly;
                QVariant m_target;
                QList<int> m_remainingSlots;
        };

    }
}

#endif // KANDAS_CONSOLE_UPWORKER_H
