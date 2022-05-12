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
char bufKoeiBmpData[2000000];  // bitmap.n6pから特定の部分だけのもの。計算しやすくするだけのため。
char bufBitMapN6P[7364608];	   // bitmap.n6pの受け皿


void CpyBufFromBitMapN6P(char *inputdatafile) {
	// 元のデータを読み込んで
	ifstream iBitMapN6P;
	iBitMapN6P.open( inputdatafile, ios::binary);
	iBitMapN6P.read( bufBitMapN6P, sizeof(bufBitMapN6P));
	iBitMapN6P.close();
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
	memcpy( bufKoeiBmpData, bufBitMapN6P+iStartAddress, iWidth*iHeight );

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
															   // 逆に言えば、BMPを上下反転した画像にしておけば、まんまコピーでもよいのだ。

	// 吐き出し
	ofstream oBitMapN6P;
	oBitMapN6P.open( outbmpfile, ios::binary);
	oBitMapN6P.write( bufBmpFile, bfOffBits+iWidth*iHeight);
	oBitMapN6P.close();

}


int main(int argc, char *argv[]) {

	CpyBufFromBitMapN6P( ".\\bitmap.n6p");
	CpyBufFromTemplate256BMP(".\\palette256_inner.bmp");

	if ( argv[1] ) {
		func( argv[1], atoi(argv[2]), atoi(argv[3]), atoi(argv[4]) );
	}
	return 0;
}

