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

#include "ndasmodel.h"
#include "ndasdevice.h"
#include "ndasslot.h"

Kandas::Client::NdasModel::NdasModel(QObject *parent)
    : QAbstractItemModel(parent)
{
}

Kandas::Client::NdasModel::~NdasModel()
{
    qDeleteAll(m_devices);
}

int Kandas::Client::NdasModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return 1;
}

int Kandas::Client::NdasModel::rowCount(const QModelIndex &parent) const
{
    //root level = device level
    if (!parent.isValid())
        return m_devices.count();
    //slot level
    Kandas::Client::NdasDevice* device = ndasdata_cast<Kandas::Client::NdasDevice*>(parent.internalPointer());
    if (device)
        return device->slotList().count();
    //no structure below slot level
    return 0;
}

QVariant Kandas::Client::NdasModel::data(const QModelIndex &index, int role) const
{
    //dataItem may be a Kandas::Client::NdasDevice or Kandas::Client::NdasSlot instance
    Kandas::Client::NdasData* dataItem = ndasdata_cast<Kandas::Client::NdasData *>(index.internalPointer());
    if (dataItem)
        return dataItem->data(role);
    else
        return QVariant();
}

QVariant Kandas::Client::NdasModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    Q_UNUSED(section)
    Q_UNUSED(orientation)
    Q_UNUSED(role)
    return QVariant();
}

QModelIndex Kandas::Client::NdasModel::index(int row, int column, const QModelIndex &parent) const
{
    //structure on X axis
    if (column != 0)
        return QModelIndex();
    //root level = device level
    if (!parent.isValid())
    {
        if (m_devices.value(row) == 0) //bounds checking!
            return QModelIndex();
        else
            return createIndex(row, 0, (void*) m_devices[row]);
    }
    //slot level
    Kandas::Client::NdasDevice* device = ndasdata_cast<Kandas::Client::NdasDevice*>(parent.internalPointer());
    if (device)
    {
        QList<Kandas::Client::NdasSlot*> slotList = device->slotList();
        if (slotList.value(row) == 0) //bounds checking!
            return QModelIndex();
        else
            return createIndex(row, 0, (void*) slotList[row]);
    }
    //no structure below slot level
    else
        return QModelIndex();
}

QModelIndex Kandas::Client::NdasModel::parent(const QModelIndex &index) const
{
    //root level = device level
    Kandas::Client::NdasDevice* device = ndasdata_cast<Kandas::Client::NdasDevice*>(index.internalPointer());
    if (device)
        return QModelIndex();
    //slot level -> find device for slot
    Kandas::Client::NdasSlot* slot = ndasdata_cast<Kandas::Client::NdasSlot*>(index.internalPointer());
    if (slot)
    {
        for (int row = 0; row < m_devices.count(); ++row)
            if (m_devices[row]->name() == slot->deviceName())
                return createIndex(row, 0, (void*) m_devices[row]);
    }
    return QModelIndex();
}

void Kandas::Client::NdasModel::updateDevice(const QString &deviceName, const QString &serial, Kandas::DeviceState state, bool hasWriteKey)
{
    //check if device exists
    for (int d = 0; d < m_devices.count(); ++d)
    {
        Kandas::Client::NdasDevice* device = m_devices[d];
        if (device->name() == deviceName)
        {
            device->update(serial, state, hasWriteKey);
            const QModelIndex index = createIndex(d, 0, (void*) device);
            emit dataChanged(index, index);
            return;
        }
    }
    //device not found -> insert new device entry
    Kandas::Client::NdasDevice* device = new Kandas::Client::NdasDevice(deviceName, serial, state, hasWriteKey);
    int pos = m_devices.count();
    beginInsertRows(QModelIndex(), pos, pos);
    m_devices << device;
    endInsertRows();
}

void Kandas::Client::NdasModel::updateSlot(int slotNumber, const QString &deviceName, const QString &blockDeviceName, Kandas::SlotState state)
{
    //check if slot exists at the correct device
    for (int d = 0; d < m_devices.count(); ++d)
    {
        Kandas::Client::NdasDevice* device = m_devices[d];
        if (device->name() == deviceName)
        {
            int slotCount = device->slotList().count();
            for (int s = 0; s < slotCount; ++s)
            {
                Kandas::Client::NdasSlot* slot = device->slotList()[s];
                if (slot->number() == slotNumber)
                {
                    //update data
                    slot->update(deviceName, blockDeviceName, state);
                    const QModelIndex index = createIndex(s, 0, (void*) slot);
                    emit dataChanged(index, index);
                    //data update for device necessary
                    const QModelIndex parent = createIndex(d, 0, (void*) device);
                    emit dataChanged(parent, parent);
                    return;
                }
            }
        }
    }
    //check if slot exists at another device
    Kandas::Client::NdasSlot* slot = 0;
    for (int d = 0; d < m_devices.count(); ++d)
    {
        Kandas::Client::NdasDevice* device = m_devices[d];
        if (device->name() != deviceName)
        {
            int slotCount = device->slotList().count();
            for (int s = 0; s < slotCount; ++s)
            {
                slot = device->slotList()[s];
                if (slot->number() == slotNumber)
                {
                    //remove from this device
                    QModelIndex parent = createIndex(d, 0, (void*) device);
                    beginRemoveRows(parent, s, s);
                    device->removeSlot(slot);
                    endRemoveRows();
                    //data update for device necessary
                    emit dataChanged(parent, parent);
                    return;
                }
            }
        }
    }
    if (slot)
    {
        //slot has to be repositioned
        slot->update(deviceName, blockDeviceName, state);
        for (int d = 0; d < m_devices.count(); ++d)
        {
            Kandas::Client::NdasDevice* device = m_devices[d];
            if (device->name() == deviceName)
            {
                //insert slot here
                QModelIndex parent = createIndex(d, 0, (void*) device);
                int d = device->slotList().count();
                beginInsertRows(parent, d, d);
                device->addSlot(slot);
                endInsertRows();
                //data update for device necessary
                emit dataChanged(parent, parent);
                return;
            }
        }
        //could not insert slot -> remove slot
        delete slot;
    }
    else
    {
        //create new slot and insert it
        Kandas::Client::NdasSlot* slot = new Kandas::Client::NdasSlot(slotNumber, deviceName, blockDeviceName, state);
        for (int d = 0; d < m_devices.count(); ++d)
        {
            Kandas::Client::NdasDevice* device = m_devices[d];
            if (device->name() == deviceName)
            {
                //insert slot here
                QModelIndex parent = createIndex(d, 0, (void*) device);
                int d = device->slotList().count();
                beginInsertRows(parent, d, d);
                device->addSlot(slot);
                endInsertRows();
                //data update for device necessary
                emit dataChanged(parent, parent);
                return;
            }
        }
    }
}

void Kandas::Client::NdasModel::removeDevice(const QString &deviceName)
{
    //find device to remove
    for (int d = 0; d < m_devices.count(); ++d)
    {
        Kandas::Client::NdasDevice* device = m_devices[d];
        if (device->name() == deviceName)
        {
            //workaround for a problem in KWidgetItemDelegate: explicitly remove all slots in this device
            QModelIndex parent = createIndex(d, 0, (void*) device);
            int slotCount = device->slotList().count();
            beginRemoveRows(parent, 0, slotCount - 1);
            QList<Kandas::Client::NdasSlot*> slotList = device->slotList();
            foreach (Kandas::Client::NdasSlot* slot, slotList)
            {
                device->removeSlot(slot);
                delete slot;
            }
            endRemoveRows();
            //remove device from model
            beginRemoveRows(QModelIndex(), d, d);
            m_devices.removeAt(d);
            endRemoveRows();
            //delete data item
            delete device;
        }
    }
}

void Kandas::Client::NdasModel::removeSlot(int slotNumber)
{
    //find slot to remove
    for (int d = 0; d < m_devices.count(); ++d)
    {
        Kandas::Client::NdasDevice* device = m_devices[d];
        int slotCount = device->slotList().count();
        for (int s = 0; s < slotCount; ++s)
        {
            Kandas::Client::NdasSlot* slot = device->slotList()[s];
            if (slot->number() == slotNumber)
            {
                //remove from model
                QModelIndex parent = createIndex(d, 0, (void*) device);
                beginRemoveRows(parent, s, s);
                device->removeSlot(slot);
                endRemoveRows();
                //delete data item
                delete slot;
            }
        }
    }
}
