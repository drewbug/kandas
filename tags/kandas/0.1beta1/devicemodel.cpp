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

#include "devicemodel.h"
#include "manager.h"

#include <KIcon>
#include <KIconLoader>
#include <KLocalizedString>

Kandas::Client::DeviceModel::DeviceModel(Kandas::Client::Manager *parent)
    : p(parent)
{
}

Kandas::Client::DeviceModel::~DeviceModel()
{
}

void Kandas::Client::DeviceModel::lateInit()
{
    disconnect(&p->m_interface, SIGNAL(initInfoComplete()), this, SLOT(lateInit()));
    reset();
}

QVariant Kandas::Client::DeviceModel::data(const QModelIndex &index, int role) const
{
    //exceptional error view
    if (p->error())
    {
        return (index.row() == 0 && index.column() == 0) ? p->errorContent(role) : QVariant();
    }
    //normal device view
    int row = index.row();
    if (row < 0 || row >= p->m_devices.count())
        return QVariant();
    //information about connection status
    int connectedCount = 0;
    int slotCount = 0;
    foreach (Kandas::Client::SlotInfo info, p->m_devices.at(row).slotList)
    {
        ++slotCount;
        if (info.state == Kandas::Connected)
            ++connectedCount;
    }
    //data
    switch (role)
    {
        case Qt::DisplayRole:
            return p->m_devices.at(row).device;
        case Qt::DecorationRole:
            if (connectedCount == 0)
                return KIcon("drive-harddisk", KIconLoader::global());
            else
                return KIcon("drive-harddisk", KIconLoader::global(), QStringList() << "emblem-mounted");
        case Kandas::Client::ConnectionStatusRole:
            if (connectedCount == 0)
                return i18n("Not connected");
            else if (connectedCount == slotCount)
                return i18np("%1 slot connected", "%1 slots connected", slotCount);
            else
                return i18n("%1 of %2 slots connected", connectedCount, slotCount);
        case Kandas::Client::ItemIdentifierRole:
            return p->m_devices.at(row).device;
        default:
            return QVariant();
    }
}

QVariant Kandas::Client::DeviceModel::headerData(int, Qt::Orientation, int) const
{
    return QVariant();
}

int Kandas::Client::DeviceModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;
    //error view -> exactly one entry with the error message; device view -> count of devices
    return p->error() ? 1 : p->m_devices.count();
}

void Kandas::Client::DeviceModel::deviceAboutToBeAdded(int deviceIndex)
{
    beginInsertRows(index(deviceIndex).parent(), deviceIndex, deviceIndex);
}

void Kandas::Client::DeviceModel::deviceAboutToBeRemoved(int deviceIndex)
{
    beginRemoveRows(index(deviceIndex).parent(), deviceIndex, deviceIndex);
}

void Kandas::Client::DeviceModel::deviceAdded()
{
    endInsertRows();
}

void Kandas::Client::DeviceModel::deviceRemoved()
{
    endRemoveRows();
}

#include "devicemodel.moc"
