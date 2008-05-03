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

#ifndef KANDAS_CONSOLE_BASEWORKER_H
#define KANDAS_CONSOLE_BASEWORKER_H

#include <QHash>
#include <QList>
#include <QObject>
#include <KLocalizedString>
#include <kandasd/definitions.h>

class OrgKandasInterface;

namespace Kandas
{
    namespace Console
    {

        const char Description[] = I18N_NOOP("Command line management for NDAS devices");
        const char VersionRaw[] = "trunk";
        const QString Version(VersionRaw);

        class BaseWorkerPrivate;

        class BaseWorker : public QObject
        {
            Q_OBJECT
            public:
                BaseWorker();
                virtual ~BaseWorker();

                bool clean() const;
                virtual bool execute() = 0; //returns true if the 
            protected:
                OrgKandasInterface *interface() const;

                Kandas::SystemState system() const;
                QList<QString> devicesList() const;
                QHash<int, Kandas::SlotInfo> slotsList() const;
            private slots:
                void systemInfo(int state);
                void deviceInfo(const QString &device);
                void slotInfo(int slot, const QString &device, int state);
                void executeJobs();
            private:
                BaseWorkerPrivate *p;
        };

    }
}

#endif // KANDAS_CONSOLE_BASEWORKER_H
