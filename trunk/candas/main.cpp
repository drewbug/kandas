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

#include "engine.h"

#include <iostream>
#include <KAboutData>
#include <KApplication>
#include <KCmdLineArgs>
#include <KLocalizedString>

static const char description[] = I18N_NOOP("Command line management for NDAS devices");
static const char version[] = "0.1";

int main(int argc, char **argv)
{
    KAboutData about("candas", "candas", ki18nc("The application's name", "CaNDAS"), version, ki18n(description),
        KAboutData::License_GPL, ki18n("(C) 2008 Stefan Majewsky"));
    about.addAuthor(ki18n("Stefan Majewsky"), ki18n("Original author and current maintainer"), "majewsky@gmx.net");
    KCmdLineArgs::init(argc, argv, &about, KCmdLineArgs::CmdLineArgNone);

    KCmdLineOptions options;
    options.add("r");
    options.add("readonly", ki18n("Connect devices and slots without write access"));
    options.add("!+state <devices or slots>", ki18n("Show the state of the given devices or slots"));
    options.add("!+up <devices or slots>", ki18n("Connect the given devices or slots"));
    options.add("!+down <devices or slots>", ki18n("Disconnect the given devices or slots"));
    KCmdLineArgs::addCmdLineOptions(options);

    KApplication app(false);

    KCmdLineArgs *args = KCmdLineArgs::parsedArgs();
    //read command line options
    QList<QString> upTargets, downTargets, stateTargets; QList<QString> *currentList = 0;
    bool readOnly = false;
    for (int i = 0; i < args->count(); ++i)
    {
        QString arg = args->arg(i);
        if (arg == "--readonly" || arg == "-r")
            readOnly = true;
        else if (arg == "up")
            currentList = &upTargets;
        else if (arg == "down")
            currentList = &downTargets;
        else if (arg == "state")
            currentList = &stateTargets;
        else
        {
            if (currentList == 0)
                std::cerr << i18n("Error: Unknown option \"%1\"", arg).toUtf8().data() << std::endl;
            else
                *currentList << arg;
        }
    }
    args->clear();

    Kandas::Console::Engine engine(upTargets, downTargets, stateTargets, readOnly);
    return app.exec();
}
