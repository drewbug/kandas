/***************************************************************************
 *   Copyright 2008-2009 Stefan Majewsky <majewsky@gmx.net>
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

namespace Kandas
{

    enum DeviceState
    {
        DeviceOffline = 0,
        DeviceOnline = 1,
        DeviceConnectionError = 10,
        DeviceLoginError = 11
    };

    enum SlotState
    {
        SlotOffline = 0,
        DisconnectedSlot = 1,
        ConnectedSlot,
        DisconnectingSlot = 10,
        ConnectingSlot
    };

    enum SystemState
    {
        SystemUnchecked = 0,
        SystemChecked = 1,
        NoDriverFound = 10, //kernel module is not loaded
        NoAdminFound //the ndasadmin program could not be found in $PATH
    };

}

#endif // KANDAS_DEFINITIONS_H
