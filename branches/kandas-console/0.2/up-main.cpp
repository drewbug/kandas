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

#include "up-worker.h"

#include <iostream>
#include <KAboutData>
#include <KApplication>
#include <KCmdLineArgs>
#include <KLocalizedString>

int main(int argc, char **argv)
{
    KAboutData about("kandas-up", "kandas", ki18nc("The application's name", "KaNDAS-up"), Kandas::Console::VersionRaw, ki18n(Kandas::Console::Description), KAboutData::License_GPL, ki18n("Copyright 2008-2009 Stefan Majewsky"), KLocalizedString(), "http://code.google.com/p/kandas/", "majewsky@gmx.net");
    about.addAuthor(ki18n("Stefan Majewsky"), ki18n("Original author and current maintainer"), "majewsky@gmx.net");
    KCmdLineArgs::init(argc, argv, &about, KCmdLineArgs::CmdLineArgNone);

    KCmdLineOptions options;
    options.add("r");
    options.add("readonly", ki18n("Connect slot read-only"));
    options.add("d");
    options.add("device", ki18n("Slot argument is a device name; connect all slots of this device"));
    options.add("+slot", ki18n("The slot to connect to"));
    KCmdLineArgs::addCmdLineOptions(options);

    KApplication app(false);

    KCmdLineArgs *args = KCmdLineArgs::parsedArgs();
    if (args->count() < 1)
    {
        std::cerr << i18n("ERROR: KaNDAS-up called without slot ID or device name. Use the --help switch for details.").toUtf8().data() << std::endl;
        return 1;
    }
    //read command line options
    bool readOnly = args->isSet("readonly");
    QVariant target(args->arg(0));
    if (!args->isSet("device"))
        target.convert(QVariant::Int);
    args->clear();

    Kandas::Console::UpWorker worker(target, readOnly);
    if (worker.clean())
        return app.exec();
    else
        return 1;
}
