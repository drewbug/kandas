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

#include "slotmodel.h"
#include "manager.h"

#include <KIcon>
#include <KIconLoader>
#include <KLocalizedString>

Kandas::Client::SlotModel::SlotModel(Kandas::Client::Manager *parent)
    : p(parent)
    , m_currentDevice(-1)
{
}

Kandas::Client::SlotModel::~SlotModel()
{
}

QVariant Kandas::Client::SlotModel::data(const QModelIndex &index, int role) const
{
    if (m_currentDevice < 0 || m_currentDevice >= p->m_devices.count())
        return QVariant();
    int row = index.row();
    if (row < 0 || row >= p->m_devices.at(m_currentDevice).slotList.count())
        return QVariant();
    Kandas::Client::SlotInfo info(p->m_devices.at(m_currentDevice).slotList.at(row));
    switch (role)
    {
        case Qt::DisplayRole:
            return i18n("Slot %1", info.slot);
        case Qt::DecorationRole:
            if (info.state == Kandas::Connected || info.state == Kandas::Disconnecting)
                return KIcon("network-wired", KIconLoader::global(), QStringList() << "emblem-mounted");
            else if (info.state == Kandas::Undetermined)
                return KIcon("unknown");
            else
                return KIcon("network-wired", KIconLoader::global());
        case Kandas::Client::ConnectionStatusRole:
            switch (info.state)
            {
                case Kandas::Undetermined:
                    return i18n("State could not be determined. Check your installation.");
                case Kandas::Connected:
                    return i18n("Connected");
                case Kandas::Disconnected:
                    return i18n("Disconnected");
                case Kandas::Connecting:
                    return i18n("Connecting");
                case Kandas::Disconnecting:
                    return i18n("Disconnecting");
            }
        case Kandas::Client::ItemIdentifierRole:
            return info.slot;
        default:
            return QVariant();
    }
}

QVariant Kandas::Client::SlotModel::headerData(int, Qt::Orientation, int) const
{
    return QVariant();
}

int Kandas::Client::SlotModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid() || m_currentDevice < 0 || m_currentDevice >= p->m_devices.count())
        return 0;
    return p->m_devices.at(m_currentDevice).slotList.count();
}

void Kandas::Client::SlotModel::changeSelectedDevice(int index)
{
    if (m_currentDevice != index)
    {
        m_currentDevice = index;
        reset();
    }
}

void Kandas::Client::SlotModel::deviceAdded(int deviceIndex)
{
    if (m_currentDevice <= deviceIndex) //device added before selected one - increment index
        ++m_currentDevice;
}

void Kandas::Client::SlotModel::deviceRemoved(int deviceIndex)
{
    if (m_currentDevice == deviceIndex) //selected device removed
        changeSelectedDevice(-1);
    else if (m_currentDevice <= deviceIndex) //device removed before selected one - decrement index
        --m_currentDevice;
}

void Kandas::Client::SlotModel::slotAboutToBeAdded(int deviceIndex, int slotIndex)
{
    if (m_currentDevice == deviceIndex)
        beginInsertRows(index(slotIndex).parent(), slotIndex, slotIndex);
}

void Kandas::Client::SlotModel::slotAdded(int deviceIndex)
{
    if (m_currentDevice == deviceIndex)
        endInsertRows();
}

void Kandas::Client::SlotModel::slotChanged(int deviceIndex, int slotIndex)
{
    if (m_currentDevice == deviceIndex)
        emit dataChanged(index(slotIndex), index(slotIndex));
}

void Kandas::Client::SlotModel::slotAboutToBeRemoved(int deviceIndex, int slotIndex)
{
    if (m_currentDevice == deviceIndex)
        beginRemoveRows(index(slotIndex).parent(), slotIndex, slotIndex);
}

void Kandas::Client::SlotModel::slotRemoved(int deviceIndex)
{
    if (m_currentDevice == deviceIndex)
        endRemoveRows();
}

#include "slotmodel.moc"
