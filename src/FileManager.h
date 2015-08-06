/**
 *
 *  You can modify and use this source freely
 *  only for the development of application related Live2D.
 *
 *  (c) Live2D Inc. All rights reserved.
 */

#include <d3dx9.h>
#include <string>


class FileManager 
{
public:
	static unsigned char* loadFile(const char* path,int* size);
	static void releaseBuffer(void* ptr);
	static void loadTexture( LPDIRECT3DDEVICE9 g_pD3DDevice, const char * textureFilePath, LPDIRECT3DTEXTURE9* tex) ;

	// 親のディレクトリを return_dir にセットする
	static void getParentDir( const char* path , std::string* return_dir ) ;
};

