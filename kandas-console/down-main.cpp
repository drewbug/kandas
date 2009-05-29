/***************************************************************************
 *   Copyright 2008-2009 Stefan Majewsky <majewsky@gmx.net>
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

#include "down-worker.h"

#include <iostream>
#include <KAboutData>
#include <KApplication>
#include <KCmdLineArgs>

int main(int argc, char **argv)
{
    KAboutData about("kandas-down", "kandas", ki18n("KaNDAS-down"), Kandas::Console::VersionRaw, ki18n(Kandas::Console::Description), KAboutData::License_GPL, ki18n("Copyright 2008-2009 Stefan Majewsky"), KLocalizedString(), "http://code.google.com/p/kandas/", "majewsky@gmx.net");
    about.addAuthor(ki18n("Stefan Majewsky"), ki18n("Maintainer"), "majewsky@gmx.net");
    KCmdLineArgs::init(argc, argv, &about, KCmdLineArgs::CmdLineArgNone);

    KCmdLineOptions options;
    options.add("d");
    options.add("drive", ki18n("Argument is a drive name"));
    options.add("+point", ki18n("Connection point (or drive if the -d switch is given)"));
    KCmdLineArgs::addCmdLineOptions(options);

    KApplication app(false);

    KCmdLineArgs *args = KCmdLineArgs::parsedArgs();
    if (args->count() < 1)
    {
        Kandas::Console::printError(i18n("Missing argument. Use the --help switch for details."));
        return 1;
    }
    //read command line options
    QVariant target(args->arg(0));
    if (!args->isSet("drive"))
        target.convert(QVariant::Int);
    args->clear();

    Kandas::Console::DownWorker worker(target);
    if (worker.clean())
        return app.exec();
    else
        return 1;
}
