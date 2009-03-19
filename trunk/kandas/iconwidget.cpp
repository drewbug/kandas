/***************************************************************************
 *   Copyright (C) 2009 Stefan Majewsky <majewsky@gmx.net>
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

#include "iconwidget.h"

Kandas::Client::IconWidget::IconWidget(const QSize& size, QWidget* parent)
    : QLabel(parent)
    , m_size(size)
    , m_active(false)
{
    setMinimumSize(size);
    setMaximumSize(size);
    //initialize pixmap
    setActive(m_active, true);
}

void Kandas::Client::IconWidget::setActive(bool active, bool force)
{
    if (m_active != active || force)
    {
        m_active = active;
        setPixmap(m_icon.pixmap(m_size, active ? QIcon::Active : QIcon::Normal));
    }
}

void Kandas::Client::IconWidget::setIcon(const QIcon& icon)
{
    m_icon = icon;
    setActive(m_active, true);
}

void Kandas::Client::IconWidget::enterEvent(QEvent* event)
{
    Q_UNUSED(event)
    setActive(isEnabled());
}

void Kandas::Client::IconWidget::leaveEvent(QEvent* event)
{
    Q_UNUSED(event)
    setActive(false);
}

#include "iconwidget.moc"
