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

#ifndef KANDAS_CLIENT_NDASDATA_H
#define KANDAS_CLIENT_NDASDATA_H

class QVariant;

namespace Kandas
{
    namespace Client
    {

        class NdasData
        {
            public:
                virtual ~NdasData();
                virtual QVariant data(int role) const = 0;
        };

    }
}

//Rationale for ndasdata_cast: C++ does not allow dynamic_cast for non-polymorphic types, but the dynamic_cast is necessary for type safety. As we can not directly dynamic_cast from void*, we have to reinterpret_cast to some base class we can safely assume. This is Kandas::Client::NdasData. The function ndasdata_cast can be used to cast from void* or NdasData* to NdasDevice* and NdasSlot*.

template<typename T> inline T ndasdata_cast(void *pointer)
{
    return dynamic_cast<T>(reinterpret_cast<Kandas::Client::NdasData *>(pointer));
}

template<> inline Kandas::Client::NdasData *ndasdata_cast<Kandas::Client::NdasData *>(void *pointer)
{
    return reinterpret_cast<Kandas::Client::NdasData *>(pointer);
}

template<typename T> inline T ndasdata_cast(Kandas::Client::NdasData *pointer)
{
    return dynamic_cast<T>(pointer);
}

#endif // KANDAS_CLIENT_NDASDATA_H
