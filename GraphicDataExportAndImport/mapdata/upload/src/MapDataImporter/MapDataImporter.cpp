#include <windows.h>
#include <fstream>
#include <iostream>
#pragma pack(1)

using namespace std;


struct BITMAPHEADER {			// ビットマップ情報とりだし用のヘッダ構造体。
	BITMAPFILEHEADER bfh;
	BITMAPINFOHEADER bih;
};



// mapdata.n6pのうち、１季節あたりのデータ=横2720ドット*縦672ライン
#define MAINMAP_PICTURE_WIDTH	2720
#define MAINMAP_PICTURE_HEIGHT	 672

struct MAINMAP_PIXEL_LINE
{
	byte pixel[MAINMAP_PICTURE_WIDTH]; 
};

struct MAINMAP_PIXCEL_BMP
{
	MAINMAP_PIXEL_LINE pixel_line[MAINMAP_PICTURE_HEIGHT];
};




MAINMAP_PIXCEL_BMP mpNewBMP, mpKoeiBMP; // mpNewBMP = BMPファイルからの直接の受け皿  mpKoeiBMP = mapdata.n6p用のフォーマットに変換する用 

char bufBmpFile[2000000]; // bmpファイル(2720*672の207色)の受け皿。

char bufMainMapData[7364608]; // mapdata.n6pの受け皿


void func(char *outmapdatafile, char *inbmpname, int no) {
	ifstream iBmpFile;
	iBmpFile.open(inbmpname, ios::binary);

	iBmpFile.read( bufBmpFile, sizeof(bufBmpFile) );

	// ビットマップヘッダーのインスタンス
	// バッファの先頭をBMPヘッダー情報ポインタとして扱う。
	BITMAPHEADER *BitMapHeader = (BITMAPHEADER *)bufBmpFile;

	//横:2720 縦:672かをチェック
	if ( BitMapHeader->bih.biWidth!=MAINMAP_PICTURE_WIDTH || BitMapHeader->bih.biHeight!=MAINMAP_PICTURE_HEIGHT ) {
		cerr << inbmpname << ":" << endl;
		cerr << "縦横サイズ違ってる。 横:2720 縦:672 を厳守" << endl;
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
		// パレットに32足しこむ。これの関係が知りたかったら、palette255_all.act と palette207_mainmap.actを見よ
		// palette207_mainmap.actのパレットと、koei用のmaindata.n6p用のパレットでは、32個ずれている。(mainmapdata.n6pの方が32数値が大きい)
		// これは通常、｢palette255_all.actの0～31のパレットは使わない、そこから後だけ使え｣などといった設定が、
		// ペインターツールや減色ツールでは一般的には不可能な故、先頭の32個を消して、 palette207_mainmap.act という形で提供しているためだ。
		// また最後の17個も消している。(最後の方はパレットの問題だけなので、値そのものには影響しないが)
		for ( int ipixel=0; ipixel<sizeof(MAINMAP_PIXEL_LINE); ipixel++) {
			mpKoeiBMP.pixel_line[MAINMAP_PICTURE_HEIGHT-iline-1].pixel[ipixel] += 32;
		}
	}

	// 元のデータを読み込んで
	ifstream iMapDataN6P;
	iMapDataN6P.open( outmapdatafile, ios::binary);
	iMapDataN6P.read( bufMainMapData, sizeof(bufMainMapData));
	iMapDataN6P.close();

	// 今回の範囲(春=0・夏=1・秋=2・冬=3)のどれかを上書き
	memcpy( (char *)(bufMainMapData+sizeof(MAINMAP_PIXCEL_BMP)*no), (char *)&mpKoeiBMP, sizeof(mpKoeiBMP) );

	// 吐き出し
	ofstream oMapDataN6P;
	oMapDataN6P.open( outmapdatafile, ios::binary);
	oMapDataN6P.write( bufMainMapData, sizeof(bufMainMapData));
	oMapDataN6P.close();
}


int main(void) {
	func(".\\mapdata.n6p", ".\\mainmap1.bmp", 0);
	func(".\\mapdata.n6p", ".\\mainmap2.bmp", 1);
	func(".\\mapdata.n6p", ".\\mainmap3.bmp", 2);
	func(".\\mapdata.n6p", ".\\mainmap4.bmp", 3);

	return 0;
}

