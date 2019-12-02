/****************************************************************************

 This file is part of GLC-Player.
 Copyright (C) 2007-2008 Laurent Ribon (laumaya@users.sourceforge.net)
 Version 2.2.0, packaged on July 2010.

 http://www.glc-player.net

 GLC-Player is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 2 of the License, or
 (at your option) any later version.

 GLC-Player is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with GLC-Player; if not, write to the Free Software
 Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA

*****************************************************************************/

#include "glc_player.h"
#include "FileOpenFilter.h"

#include <QtGui>
#include <QApplication>
#include <QSettings>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    // Add Plugin Path

    #if defined(Q_OS_WIN)
    QCoreApplication::addLibraryPath(QCoreApplication::applicationDirPath());
	#elif defined(Q_OS_MAC)
    QDir plugin(QCoreApplication::applicationDirPath());
    plugin.cdUp();
    plugin.cd("plugins");
    QCoreApplication::addLibraryPath(plugin.path());
	#endif

    // Define Organisation name, domain and application name
	QCoreApplication::setOrganizationName("ribon");
	QCoreApplication::setOrganizationDomain("ribon.com");
	QCoreApplication::setApplicationName("GLC_Player");
    QCoreApplication::setApplicationVersion("2.4.0");

	// The splash screen
	#if !defined(Q_OS_MAC)
	QSplashScreen *pSplash= new QSplashScreen;
	pSplash->setPixmap(QPixmap(":images/Splash.png"));

	pSplash->show();
	#endif
	// Settings
	#if defined(Q_OS_MAC)
	const QString settingsFileName(QDir::homePath() + "/Library/Application Support/" + QCoreApplication::applicationName() + QDir::separator() + "Settings.ini");
	QDir().mkpath(QFileInfo(settingsFileName).path());
	QSettings settings(settingsFileName, QSettings::IniFormat);
	#else
	QSettings settings;
	#endif
	// Chose application language
	QTranslator translator;
	if (settings.contains("currentLanguage"))
	{
		QString currentLanguage(settings.value("currentLanguage").toString());
		translator.load(QString(":/lang/glc_player_") + currentLanguage);
	}
	else
	{
		QString locale = QString(QLocale::system().name()).left(2);
		translator.load(QString(":/lang/glc_player_") + locale);
		settings.setValue("currentLanguage", locale);
	}
	// Set application language
	QCoreApplication::installTranslator(&translator);

    // Test if the system has OpenGL Support
    if (!QGLFormat::hasOpenGL())
    {
    	QString message(QObject::tr("This System has no OpenGL support"));
    	QMessageBox::critical(NULL, QCoreApplication::applicationName(), message);
    	return 1;
    }

    // Test if the system support frame buffer Object
    //const bool frameBufferIsSupported= QGLFramebufferObject::hasOpenGLFramebufferObjects();

    // Create the main Window
    glc_player mainWindow;
    mainWindow.show();

    //app.connect(&app, SIGNAL(lastWindowClosed()), &app, SLOT(quit()));
	#if !defined(Q_OS_MAC)
	pSplash->finish(&mainWindow);
	delete pSplash;
	#endif

    // This filter is installed to intercept the open events sent directly by the Operative System.
    FileOpenFilter *pFileOpenFilter=new FileOpenFilter(&mainWindow);
    app.installEventFilter(pFileOpenFilter);

    return app.exec();
}
