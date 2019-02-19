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

#ifdef LINUX

#include "Thumbnailer.hpp"

#include <kfileitem.h>
#include <previewjob.h>

#include <QBuffer>
#include <QByteArray>
#include <QEventLoop>
#include <QFileInfo>
#include <QSize>
#include <QString>
#include <QPixmap>

std::string get_thumbnail( std::string path ){
	using namespace KIO;
	
	//Initialize job
	auto url = QUrl::fromLocalFile( QFileInfo(QString::fromLocal8Bit( path.c_str() )).absoluteFilePath() );
	KFileItem file( url );
	auto list = QList<KFileItem>() << file;
	KIO::PreviewJob job( list, QSize( 512, 512 ) );
	job.setIgnoreMaximumSize( true );
	
	//Set thumb getter
	QEventLoop loop;
	QByteArray array;
	QBuffer buffer( &array );
	
	QObject::connect( &job, &KIO::PreviewJob::gotPreview, [&](const KFileItem&, const QPixmap& result){
		//convert pixmap to BMP
		result.save( &buffer, "BMP" ); //TODO: transparency?
		loop.quit();
	} );
	QObject::connect( &job, &KIO::PreviewJob::failed, [&](const KFileItem&){
		loop.quit();
	} );
	
	//Wait for thumbnail to finish processing
	job.start();
	loop.exec();
	
	return std::string( array.constData(), array.size() );
}


#endif

