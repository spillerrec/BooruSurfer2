/*	This file is part of BooruSurfer2.

	BooruSurfer2 is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	BooruSurfer2 is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with BooruSurfer2.  If not, see <http://www.gnu.org/licenses/>.
*/

//find_package(KF5KIO)
//target_link_libraries(yourapp KF5::KIOCore KF5::KIOFileWidgets KF5::KIOWidgets KF5::KIONTLM)


#ifdef LINUX

#include "Thumbnailer.hpp"
#include "ThumbGetter.hpp"

#include <kfileitem.h>
#include <previewjob.h>

#include <QObject>
#include <QString>
#include <QSize>
#include <QEventLoop>
#include <QPixmap>
#include <QByteArray>
#include <QBuffer>

#include <string>
#include <iostream>
#include <QFileInfo>

//*
std::string get_thumbnail( std::string path ){
	using namespace KIO;
	//Initialize job
	QUrl url( "file://" + QFileInfo(QString::fromLocal8Bit( path.c_str() )).absoluteFilePath() );
	KFileItem file( url );
	auto list = QList<KFileItem>() << file;
	KIO::PreviewJob job( list, QSize( 512, 512 ) );
	
	//Set thumb getter
	ThumbGetter getter;
	QObject::connect(&job, &KIO::PreviewJob::gotPreview, &getter, &ThumbGetter::setImage);
	
	//Wait for thumbnail to finish processing
	QEventLoop loop;
	QObject::connect(&job, &KIO::PreviewJob::failed, [&](const KFileItem){ loop.quit(); });
	QObject::connect(&getter, &ThumbGetter::done, [&](){ loop.quit(); });
	job.start();
	loop.exec();
	
	//convert pixmap to BMP
	QByteArray array;
	QBuffer buffer( &array );
	getter.pixmap.save( &buffer, "BMP" ); //TODO: transparency?
	return std::string( array.constData(), array.size() );
}


/*/
#include <QApplication>
#include <mutex>


std::string get_thumbnail( std::string path ){
	std::cout << "Get thumbnail of: " << path.c_str() << "\n";
	using namespace KIO;
	//Initialize job
	std::cout << "Test1\n";
	QUrl url( QString::fromLocal8Bit( path.c_str() ) );
	KFileItem file( url );
	auto list = QList<KFileItem>() << file;
	std::cout << "Test2\n";
	KIO::PreviewJob job( list, QSize( 512, 512 ) );
	std::cout << "Test3.0\n";
	//return "";
	
	//Set thumb getter
	std::mutex pixmap_working;
	pixmap_working.lock();
	std::cout << "Test3.4\n";
	QPixmap pixmap;
	
	std::cout << "Test3.5\n";
	QObject::connect( &job, &KIO::PreviewJob::gotPreview, [&](const KFileItem&, const QPixmap& result){
		std::cout << "Pixmap recieved\n";
		pixmap = result;
		pixmap_working.unlock();
	} );
	QObject::connect( &job, &KIO::PreviewJob::failed, [&](const KFileItem&){
		std::cout << "Pixmap failed\n";
		pixmap_working.unlock();
	} );
	QObject::connect( &job, &KIO::PreviewJob::result, [&](KJob*){
		std::cout << "Job done\n";
	} );
	
	std::cout << "Test4\n";
	
	//Wait for thumbnail to finish processing
	job.start();
	std::cout << "Test5\n";
	while(true)
		QApplication::processEvents();
	std::cout << "Test6\n";
	
	//convert pixmap to BMP
	QByteArray array;
	QBuffer buffer( &array );
	std::cout << "Test7\n";
	pixmap.save( &buffer, "BMP" ); //TODO: transparency?
	std::cout << "Test8\n";
	
	std::cout << "Thumbnail lenght is: " << array.size() << "\n";
	return std::string( array.constData(), array.size() );
}//*/


#endif

