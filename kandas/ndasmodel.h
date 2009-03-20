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

#ifndef KANDAS_CLIENT_NDASMODEL_H
#define KANDAS_CLIENT_NDASMODEL_H

#include <QAbstractItemModel>

namespace Kandas
{
    namespace Client
    {

        class NdasDevice;
        class NdasSlot;

        enum NdasRole
        {
        };

        class NdasModel : public QAbstractItemModel
        {
            public:
                NdasModel(QObject *parent = 0);
                virtual ~NdasModel();

                virtual int columnCount(const QModelIndex &parent = QModelIndex()) const;
                virtual int rowCount(const QModelIndex &parent = QModelIndex()) const;
                virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;

                virtual QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const;
                virtual QModelIndex parent(const QModelIndex &index) const;
            private:
                QList<Kandas::Client::NdasDevice*> m_devices;
        };

    }
}

#endif // KANDAS_CLIENT_NDASMODEL_H
