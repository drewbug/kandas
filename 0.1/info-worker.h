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

#ifndef KANDAS_CONSOLE_INFOWORKER_H
#define KANDAS_CONSOLE_INFOWORKER_H

#include "base-worker.h"

namespace Kandas
{
    namespace Console
    {

        class InfoWorker : public BaseWorker
        {
            public:
                InfoWorker(bool listEnv, bool listDevices, bool listSlots);

                virtual bool execute();
            private:
                void listEnvironment();
                void listDevices();
                void listSlots();

                bool m_listEnv;
                bool m_listDevices;
                bool m_listSlots;
        };

    }
}

#endif // KANDAS_CONSOLE_INFOWORKER_H
