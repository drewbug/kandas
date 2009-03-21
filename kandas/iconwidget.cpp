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

#include "iconwidget.h"

#include <QMouseEvent>

Kandas::Client::IconWidget::IconWidget(const QSize &size, QWidget *parent)
    : QLabel(parent)
    , m_size(size)
    , m_active(false)
    , m_index(0)
{
    qRegisterMetaType<QPersistentModelIndex>("QPersistentModelIndex");
    setMinimumSize(size);
    setMaximumSize(size);
    //initialize pixmap
    setActive(m_active, true);
}

Kandas::Client::IconWidget::~IconWidget()
{
    delete m_index;
}

bool Kandas::Client::IconWidget::hasIndex() const
{
    return m_index != 0;
}

void Kandas::Client::IconWidget::setActive(bool active, bool force)
{
    if (m_active != active || force)
    {
        m_active = active;
        setPixmap(m_icon.pixmap(m_size, active ? QIcon::Active : QIcon::Normal));
    }
}

void Kandas::Client::IconWidget::setAction(const QString &action)
{
    m_action = action;
}

void Kandas::Client::IconWidget::setIcon(const QIcon &icon)
{
    m_icon = icon;
    setActive(m_active, true);
}

void Kandas::Client::IconWidget::setIndex(const QPersistentModelIndex &index)
{
    delete m_index;
    m_index = new QPersistentModelIndex(index);
}

void Kandas::Client::IconWidget::enterEvent(QEvent *event)
{
    Q_UNUSED(event)
    setActive(isEnabled());
}

void Kandas::Client::IconWidget::leaveEvent(QEvent *event)
{
    Q_UNUSED(event)
    setActive(false);
}

void Kandas::Client::IconWidget::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton && isEnabled() && m_index)
        emit triggered(*m_index, m_action);
}

#include "iconwidget.moc"
