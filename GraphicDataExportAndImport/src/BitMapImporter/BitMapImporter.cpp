#include <windows.h>
#include <fstream>
#include <iostream>
#pragma pack(1)

using namespace std;


struct BITMAPHEADER {			// ビットマップ情報とりだし用のヘッダ構造体。
	BITMAPFILEHEADER bfh;
	BITMAPINFOHEADER bih;
};



// bitmap.n6pのうち、１つ([朝廷とか外交とか評定]の１つ)データ=横640ドット*縦448ライン
#define MAINMAP_PICTURE_WIDTH	 640
#define MAINMAP_PICTURE_HEIGHT	 448

struct MAINMAP_PIXEL_LINE
{
	byte pixel[MAINMAP_PICTURE_WIDTH]; 
};

struct MAINMAP_PIXCEL_BMP
{
	MAINMAP_PIXEL_LINE pixel_line[MAINMAP_PICTURE_HEIGHT];
};




MAINMAP_PIXCEL_BMP mpNewBMP, mpKoeiBMP; // mpNewBMP = BMPファイルからの直接の受け皿  mpKoeiBMP = bitmap.n6p用のフォーマットに変換する用 

char bufBmpFile[2000000]; // bmpファイル(640*448の207色)の受け皿。

char bufBitMapN6P[860160]; // bitmap.n6pの受け皿


void func(char *outbitmapfile, char *inbmpname, int no) {
	ifstream iBmpFile;
	iBmpFile.open(inbmpname, ios::binary);

	iBmpFile.read( bufBmpFile, sizeof(bufBmpFile) );

	// ビットマップヘッダーのインスタンス
	// バッファの先頭をBMPヘッダー情報ポインタとして扱う。
	BITMAPHEADER *BitMapHeader = (BITMAPHEADER *)bufBmpFile;

	//横:640 縦:448かをチェック
	if ( BitMapHeader->bih.biWidth!=MAINMAP_PICTURE_WIDTH || BitMapHeader->bih.biHeight!=MAINMAP_PICTURE_HEIGHT ) {
		cerr << inbmpname << ":" << endl;
		cerr << "縦横サイズ違ってる。 横:640 縦:448 を厳守" << endl;
		getchar();
		iBmpFile.close();
		exit(-1);
	}
	// 色空間チェック
	if ( BitMapHeader->bih.biBitCount > 8 ) {
		cerr << inbmpname << ":" << endl;
		cerr << "減色していない。 指定された207色のパレットを使ってoptpixやyukari等で減色すること。" << endl;
		getchar();
		iBmpFile.close();
		exit(-1);
	}

	// 画像内容が始まっている場所を特定し、そこから画像内容をBMP構造体用の格納場所にコピー
	int bfOffBits = BitMapHeader->bfh.bfOffBits;
	memcpy( &mpNewBMP, (bufBmpFile+bfOffBits), sizeof(mpNewBMP) );

	// 左下のラインから情報を読んでいく
	for ( int iline=0; iline < MAINMAP_PICTURE_HEIGHT; iline++ ) {
		// 1ライン分コピペ
		memcpy( (char *)&mpKoeiBMP.pixel_line[MAINMAP_PICTURE_HEIGHT-iline-1], (char *)&mpNewBMP.pixel_line[iline], sizeof(MAINMAP_PIXEL_LINE));
		// パレットに32足しこむ。これの関係が知りたかったら、palette255_inner.act と palette207_inner.actを見よ
		// palette207_inner.actのパレットと、koei用のbitmap.n6p用のパレットでは、32個ずれている。(bitmap.n6pの方が32数値が大きい)
		// これは通常、｢palette255_inner.actの0～31のパレットは使わない、そこから後だけ使え｣などといった設定が、
		// ペインターツールや減色ツールでは一般的には不可能な故、先頭の32個を消して、 palette207_inner.act という形で提供しているためだ。
		// また最後の17個も消している。(最後の方はパレットの問題だけなので、値そのものには影響しないが)
		for ( int ipixel=0; ipixel<sizeof(MAINMAP_PIXEL_LINE); ipixel++) {
			mpKoeiBMP.pixel_line[MAINMAP_PICTURE_HEIGHT-iline-1].pixel[ipixel] += 32;
		}
	}

	// 元のデータを読み込んで
	ifstream iBitMapN6P;
	iBitMapN6P.open( outbitmapfile, ios::binary);
	iBitMapN6P.read( bufBitMapN6P, sizeof(bufBitMapN6P));
	iBitMapN6P.close();

	// 今回の範囲(朝廷=0・外交=1・評定=2)のどれかを上書き
	memcpy( (char *)(bufBitMapN6P+sizeof(MAINMAP_PIXCEL_BMP)*no), (char *)&mpKoeiBMP, sizeof(mpKoeiBMP) );

	// 吐き出し
	ofstream oBitMapN6P;
	oBitMapN6P.open( outbitmapfile, ios::binary);
	oBitMapN6P.write( bufBitMapN6P, sizeof(bufBitMapN6P));
	oBitMapN6P.close();
}


int main(void) {
	func(".\\bitmap.n6p", ".\\bitmap1.bmp", 0);
	func(".\\bitmap.n6p", ".\\bitmap2.bmp", 1);
	func(".\\bitmap.n6p", ".\\bitmap3.bmp", 2);

	return 0;
}

