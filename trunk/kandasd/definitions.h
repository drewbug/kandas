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

#ifndef KANDAS_DEFINITIONS_H
#define KANDAS_DEFINITIONS_H

#include <QString>

namespace Kandas
{

    const char InterfaceVersionRaw[] = "0.1pre1";
    const QString InterfaceVersion(InterfaceVersionRaw);

    enum ClientType
    {
        HighImportanceMonitor,
        LowImportanceMonitor,
        DirectModifier
    };

    namespace AutoRefreshIntervals
    {
        const int HighImportanceMonitor = 1;
        const int LowImportanceMonitor = 5;
    };

    enum SlotState
    {
        Undetermined = 0,
        Disconnected = 10,
        Connected,
        Disconnecting = 20,
        Connecting
    };

    enum EnvironmentState
    {
        UnknownEnvironment = 0,
        SaneEnvironment = 1,
        NoDriverFound = 10, //kernel module is not loaded
        NoAdminFound //the ndasadmin program could not be found in $PATH
    };

    struct SlotInfo
    {
        SlotState state;
        QString device;
        SlotInfo(const QString &slotDevice = QString(), SlotState slotState = Kandas::Undetermined) : state(slotState), device(slotDevice) {}
    };

}

#endif // KANDAS_DEFINITIONS_H
