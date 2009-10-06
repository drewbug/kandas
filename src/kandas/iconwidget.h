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

#ifndef KANDAS_CLIENT_ICONWIDGET_H
#define KANDAS_CLIENT_ICONWIDGET_H

#include <QIcon>
#include <QLabel>
#include <QPersistentModelIndex>

namespace Kandas
{
    namespace Client
    {

        class IconWidget : public QLabel
        {
            Q_OBJECT
            public:
                IconWidget(const QSize &size, QWidget *parent = 0);
                ~IconWidget();

                bool hasIndex() const;

                void setActive(bool active, bool force = false);
                void setAction(const QString &action);
                void setIcon(const QIcon &icon);
                void setIndex(const QPersistentModelIndex &index);
            Q_SIGNALS:
                void triggered(const QPersistentModelIndex &index, const QString &action);
            protected:
                virtual void enterEvent(QEvent *event);
                virtual void leaveEvent(QEvent *event);
                virtual void mousePressEvent(QMouseEvent *event);
            private:
                QIcon m_icon;
                QSize m_size;
                bool m_active;
                QString m_action;
                QPersistentModelIndex* m_index;
        };

    }
}

#endif // KANDAS_CLIENT_ICONWIDGET_H