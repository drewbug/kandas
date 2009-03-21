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

#include "connectmenu.h"
#include "manager.h"

#include <KAction>
#include <KIcon>
#include <KLocalizedString>

Kandas::Client::ConnectMenu::ConnectMenu(Kandas::Client::Manager *manager, const QVariant &target)
    : m_manager(manager)
    , m_target(target)
    , m_readAct(new KAction(KIcon("media-playback-start"), i18n("Read access"), this))
    , m_readWriteAct(new KAction(KIcon("media-record"), i18n("Read/write access"), this))
{
    addAction(m_readAct);
    addAction(m_readWriteAct);
    connect(this, SIGNAL(aboutToHide()), this, SLOT(handleHidden()));
    connect(m_readAct, SIGNAL(triggered()), this, SLOT(handleHidden()));
    connect(m_readWriteAct, SIGNAL(triggered()), this, SLOT(handleHidden()));
    popup(QCursor::pos());
}

void Kandas::Client::ConnectMenu::handleHidden()
{
    deleteLater();
    //determine read mode
    bool readOnly;
    if (sender() == m_readAct)
        readOnly = true;
    else if (sender() == m_readWriteAct)
        readOnly = false;
    else
        return; //user clicked outside of menu -> abort action
    //invoke connection
    if (m_target.type() == QVariant::Int)
        m_manager->connectSlot(m_target.value<int>(), readOnly);
    else
        m_manager->connectDevice(m_target.value<QString>(), readOnly);
}

#include "connectmenu.moc"
