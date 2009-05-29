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

#include "ndassystemmodel.h"
#include "ndasmodel.h" //for the user-defined roles

#include <KIcon>
#include <KLocalizedString>

Kandas::Client::NdasSystemModel::NdasSystemModel(QObject *parent)
    : QStandardItemModel(parent)
    , m_item(new QStandardItem)
    , m_state(Kandas::SystemChecked)
{
    appendRow(m_item);
    //start with no specific content in the item, as the model is assumed to be hidden in the SystemChecked state
}

bool Kandas::Client::NdasSystemModel::clean() const
{
    return m_errorMessage.isEmpty() && m_state == Kandas::SystemChecked;
}

QString Kandas::Client::NdasSystemModel::connectionError() const
{
    return m_errorMessage;
}

Kandas::SystemState Kandas::Client::NdasSystemModel::state() const
{
    return m_state;
}

void Kandas::Client::NdasSystemModel::setConnectionError(const QString &errorMessage)
{
    if (m_errorMessage == errorMessage)
        return;
    m_errorMessage = errorMessage;
    if (!errorMessage.isEmpty())
        m_state = Kandas::SystemUnchecked; //logical consequence of error in connection to KaNDASd
    updateContents();
}

void Kandas::Client::NdasSystemModel::setState(Kandas::SystemState state)
{
    if (m_state == state)
        return;
    m_state = state;
    updateContents();
}

void Kandas::Client::NdasSystemModel::updateContents()
{
    if (!m_errorMessage.isEmpty())
    {
        m_item->setData(m_errorMessage, Qt::DisplayRole);
        m_item->setData(i18n("Check your system configuration."), Kandas::Client::SecondDisplayRole);
        m_item->setData(KIcon("dialog-error"), Qt::DecorationRole);
        return;
    }
    switch (m_state)
    {
        case Kandas::SystemChecked:
            break; //do not set any specific content, as the model is assumed to be hidden in this state
        case Kandas::SystemUnchecked:
            m_item->setData(i18n("Performing system check..."), Qt::DisplayRole);
            m_item->setData(QString(), Kandas::Client::SecondDisplayRole);
            m_item->setData(KIcon("dialog-information"), Qt::DecorationRole);
            break;
        case Kandas::NoDriverFound:
            m_item->setData(i18n("NDAS driver is not loaded."), Qt::DisplayRole);
            m_item->setData(i18n("Check your system configuration."), Kandas::Client::SecondDisplayRole);
            m_item->setData(KIcon("dialog-error"), Qt::DecorationRole);
            break;
        case Kandas::NoAdminFound:
            m_item->setData(i18n("NDAS driver is not installed."), Qt::DisplayRole);
            m_item->setData(i18n("Check your system configuration."), Kandas::Client::SecondDisplayRole);
            m_item->setData(KIcon("dialog-error"), Qt::DecorationRole);
            break;
    }
}
