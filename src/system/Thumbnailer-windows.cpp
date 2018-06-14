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


#ifdef WIN32

#define _WIN32_IE _WIN32_IE_IE70

#include <Shobjidl.h>

#include "Thumbnailer.hpp"

#include <string>
#include <iostream>
#include <memory>
#include <stdexcept>

#include <boost/filesystem.hpp>

#include <wincodec.h>
#include <Windows.h>
#include <Winerror.h>

//Automatically free IUnknown instances
template<class T>
class ComPtr{
	private:
		T* ptr{ nullptr }; //NOTE: We can't use unique_ptr<> as we need to be able to pass a pointer to the pointer
		
		void free(){
			if( ptr )
				ptr->Release();
			ptr = nullptr;
		}
		
	public:
		ComPtr( T* ptr=nullptr ) : ptr(ptr) { }
		
		T** init(){
			free();
			return &ptr;
		}
		
		T* get(){ return ptr; }
		T* operator->(){ return get(); }
		operator bool() const { return ptr != nullptr; }
};

//Throw exceptions on HResult errors
void ComCheck( const char* const message, HRESULT code ){
	if( code != S_OK )
		throw std::runtime_error( "ComException, code " + std::to_string(code) + " while " + message );
}

//Encode Bitmap to BMP string
static std::string WriteBitmap( HBITMAP bitmap ) {
    // (1) Retrieve properties from the source HBITMAP.
    BITMAP bm_info = { 0 };
    if( !GetObject( bitmap, sizeof(bm_info), &bm_info ) )
		throw std::runtime_error( "Failed to get Bitmap info" );
	
	//Create and determine buffer size
	size_t buffer_size = bm_info.bmWidth*bm_info.bmHeight*3*2;
	auto buffer = std::make_unique<BYTE[]>( buffer_size );

    // (2) Create an IWICImagingFactory instance.
    ComPtr<IWICImagingFactory> factory;
	ComCheck( "creating WIC factory", CoCreateInstance(
		CLSID_WICImagingFactory, nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS( factory.init() ) ) );

    // (3) Create an IWICBitmap instance from the HBITMAP.
    ComPtr<IWICBitmap> wic_bitmap;
    ComCheck( "creating IWICBitmap", factory->CreateBitmapFromHBITMAP(
			bitmap, nullptr, WICBitmapIgnoreAlpha, wic_bitmap.init()
		) );

    // (4) Create an IWICStream instance, and attach it to a filename.
    ComPtr<IWICStream> stream;
    ComCheck( "createing IWICStream", factory->CreateStream( stream.init() ) );
	ComCheck( "initialize mem-stream", stream->InitializeFromMemory( buffer.get(), buffer_size ) );

    // (5) Create an IWICBitmapEncoder instance, and associate it with the stream.
    ComPtr<IWICBitmapEncoder> encoder;
    ComCheck( "creating BMP encoder", factory->CreateEncoder( GUID_ContainerFormatBmp, nullptr, encoder.init() ) );
    ComCheck( "initializing encoder", encoder->Initialize( stream.get(), WICBitmapEncoderNoCache ) );

    // (6) Create an IWICBitmapFrameEncode instance, and initialize it
    // in compliance with the source HBITMAP.
    ComPtr<IWICBitmapFrameEncode> frame;
	ComCheck( "creating frame", encoder->CreateNewFrame( frame.init(), nullptr ) );
    ComCheck( "initializing frame", frame->Initialize( nullptr ) );
    ComCheck( "setting frame size", frame->SetSize( bm_info.bmWidth, bm_info.bmHeight ) );
	
	GUID pixel_format = GUID_WICPixelFormat24bppBGR;
	ComCheck( "setting pixel format", frame->SetPixelFormat( &pixel_format ) );

    // (7) Write bitmap data to the frame.
    ComCheck( "writing frame", frame->WriteSource( wic_bitmap.get(), nullptr ) );

    // (8) Commit frame and data to stream.
    ComCheck( "frame commit", frame->Commit() );
    ComCheck( "encoder commit", encoder->Commit() );

	//Find size of buffer
	//TODO: Seek size does not work
	ULARGE_INTEGER final_buffer_size { 0, 0 };
	LARGE_INTEGER seek_amount { 0, 0 };
	ComCheck( "getting stream end pos", stream->Seek( seek_amount, STREAM_SEEK_CUR, &final_buffer_size ) );
	
//	return { (char*)buffer.get(), final_buffer_size.QuadPart };
	return { (char*)buffer.get(), buffer_size };
}


static std::wstring s2ws( const std::string& s ){
    int slength = s.length() + 1;
    auto len = MultiByteToWideChar( CP_UTF8, 0, s.c_str(), slength, 0, 0 );
    auto buf = std::make_unique<wchar_t[]>( len );
    MultiByteToWideChar( CP_UTF8, 0, s.c_str(), slength, buf.get(), len );
    return { buf.get() };
}


std::string get_thumbnail( std::string path ){
	HBITMAP bitmap;
	std::string out = "";
	
	try{
		//TODO: Encoding conversion with boost and path::wstring() ?
		//NOTE: Path must be absolute and use backslashes
		auto absolute = boost::filesystem::system_complete( { path } );
		path = absolute.string();
		auto wpath = s2ws( path );
		
		//Get thumbnail
		ComPtr<IShellItem> item;
		ComPtr<IShellItemImageFactory> factory;
		ComCheck( "creating ShellItem", SHCreateItemFromParsingName( wpath.c_str(), nullptr, IID_PPV_ARGS(item.init()) ) );		
		ComCheck( "creating Shell factory", item->QueryInterface( IID_PPV_ARGS( factory.init() ) ) );
		
		tagSIZE size{ 512, 512 };
		ComCheck( "extracting thumbnail", factory->GetImage( size, SIIGBF_RESIZETOFIT | SIIGBF_THUMBNAILONLY, &bitmap ) );
		
		//Encode image
		out = WriteBitmap( bitmap );
	}
	catch( std::exception& e ){
		std::cout << "ERROR while getting thumbnail: " << e.what() << '\n';
		std::cout << "File path: " << path.c_str() << '\n';
	}
	
	DeleteObject( bitmap );
	return out;
}

#endif

