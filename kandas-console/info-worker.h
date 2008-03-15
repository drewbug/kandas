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

class OrgKdeKandasInterface;

#include <QObject>

namespace Kandas
{
    namespace Console
    {

        class InfoWorkerPrivate;

        class InfoWorker : public QObject
        {
            Q_OBJECT
            public:
                InfoWorker(bool listDevices, bool listSlots);
                ~InfoWorker();
                bool clean() const;
            private slots:
                void initEnvironment(int state);
                void initDevice(const QString &device);
                void initSlot(int slot, const QString &device, int state);
                void executeJobs();
                void listDevices();
                void listSlots();
            private:
                InfoWorkerPrivate *p;
        };

    }
}

#endif // KANDAS_CONSOLE_INFOWORKER_H
