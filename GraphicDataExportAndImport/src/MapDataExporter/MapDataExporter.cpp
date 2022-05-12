#include <windows.h>
#include <fstream>
#include <iostream>
#pragma pack(1)

using namespace std;


struct BITMAPHEADER {			// ビットマップ情報とりだし用のヘッダ構造体。
	BITMAPFILEHEADER bfh;
	BITMAPINFOHEADER bih;
};


char bufBmpFileTemplate[2000]; // palette256_all.bmpの受け皿
char bufBmpFile[2000000];      // 出力するビットマップ
char bufKoeiBmpData[2000000];  // mapdataから特定の部分だけのもの。計算しやすくするだけのため。
char bufMapData[7364608];	   // mapdata.n6pの受け皿


void CpyBufFromMapDataN6P(char *inputdatafile) {
	// 元のデータを読み込んで
	ifstream iMapDataN6P;
	iMapDataN6P.open( inputdatafile, ios::binary);
	iMapDataN6P.read( bufMapData, sizeof(bufMapData));
	iMapDataN6P.close();
}

void CpyBufFromTemplate256BMP(char *inputbmpfile) {
	ifstream iTemplateBmpFile;
	iTemplateBmpFile.open(inputbmpfile, ios::binary);

	iTemplateBmpFile.read( bufBmpFileTemplate, sizeof(bufBmpFileTemplate) );

	iTemplateBmpFile.close();
}

void func(char *outbmpfile, int iStartAddress=0, int iWidth=16, int iHeight=16 ) {
	if ( !outbmpfile ) { return; }

	memcpy( bufBmpFile, bufBmpFileTemplate, sizeof(bufBmpFileTemplate) );

	// mapdata.n6pの中から、対象とする領域だけをbufKoeiBmpDataにコピー
	memcpy( bufKoeiBmpData, bufMapData+iStartAddress, iWidth*iHeight );

	// ビットマップヘッダーのインスタンス
	// バッファの先頭をBMPヘッダー情報ポインタとして扱う。
	BITMAPHEADER *BitMapHeader = (BITMAPHEADER *)bufBmpFile;

	//横と縦を指定サイズで上書き
	BitMapHeader->bih.biWidth  = iWidth;
	BitMapHeader->bih.biHeight = iHeight;

	int bfOffBits = BitMapHeader->bfh.bfOffBits;

	// 実際のビットマップが始まるスタートポイント
	char *pBitMapStart = bufBmpFile + bfOffBits;

	// １ライン単位で逆にコピーしてゆく。
	for ( int iline=0; iline < iHeight; iline++ ) {
		// 1ライン分コピペ
		memcpy( pBitMapStart+(iHeight-iline-1)*iWidth, bufKoeiBmpData+(iline*iWidth), iWidth);
	}

	// memcpy( pBitMapStart, bufKoeiBmpData, iWidth*iHeight ); // ←まんまコピーだと逆になる。

	// 吐き出し
	ofstream oMapDataN6P;
	oMapDataN6P.open( outbmpfile, ios::binary);
	oMapDataN6P.write( bufBmpFile, bfOffBits+iWidth*iHeight);
	oMapDataN6P.close();

}


int main(int argc, char *argv[]) {

	CpyBufFromMapDataN6P(".\\mapdata.n6p");
	CpyBufFromTemplate256BMP(".\\palette256_all.bmp");

	if ( argv[3] ) {
		func( argv[1], atoi(argv[2]), atoi(argv[3]), atoi(argv[4]) );
	} else {
		func(".\\test1.bmp", 1828840, 2720, 672 ); 
	}
	return 0;
}

