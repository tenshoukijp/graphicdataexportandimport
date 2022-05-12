#include <windows.h>
#include <fstream>
#include <iostream>
#pragma pack(1)

using namespace std;


struct BITMAPHEADER {			// �r�b�g�}�b�v���Ƃ肾���p�̃w�b�_�\���́B
	BITMAPFILEHEADER bfh;
	BITMAPINFOHEADER bih;
};



// mapdata.n6p�̂����A�P�G�߂�����̃f�[�^=��2720�h�b�g*�c672���C��
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




MAINMAP_PIXCEL_BMP mpNewBMP, mpKoeiBMP; // mpNewBMP = BMP�t�@�C������̒��ڂ̎󂯎M  mpKoeiBMP = mapdata.n6p�p�̃t�H�[�}�b�g�ɕϊ�����p 

char bufBmpFile[2000000]; // bmp�t�@�C��(2720*672��207�F)�̎󂯎M�B

char bufMainMapData[7364608]; // mapdata.n6p�̎󂯎M


void func(char *outmapdatafile, char *inbmpname, int no) {
	ifstream iBmpFile;
	iBmpFile.open(inbmpname, ios::binary);

	iBmpFile.read( bufBmpFile, sizeof(bufBmpFile) );

	// �r�b�g�}�b�v�w�b�_�[�̃C���X�^���X
	// �o�b�t�@�̐擪��BMP�w�b�_�[���|�C���^�Ƃ��Ĉ����B
	BITMAPHEADER *BitMapHeader = (BITMAPHEADER *)bufBmpFile;

	//��:2720 �c:672�����`�F�b�N
	if ( BitMapHeader->bih.biWidth!=MAINMAP_PICTURE_WIDTH || BitMapHeader->bih.biHeight!=MAINMAP_PICTURE_HEIGHT ) {
		cerr << inbmpname << ":" << endl;
		cerr << "�c���T�C�Y����Ă�B ��:2720 �c:672 ������" << endl;
		getchar();
		iBmpFile.close();
		exit(-1);
	}
	// �F��ԃ`�F�b�N
	if ( BitMapHeader->bih.biBitCount > 8 ) {
		cerr << inbmpname << ":" << endl;
		cerr << "���F���Ă��Ȃ��B �w�肳�ꂽ207�F�̃p���b�g���g����optpix��yukari���Ō��F���邱�ƁB" << endl;
		getchar();
		iBmpFile.close();
		exit(-1);
	}

	// �摜���e���n�܂��Ă���ꏊ����肵�A��������摜���e��BMP�\���̗p�̊i�[�ꏊ�ɃR�s�[
	int bfOffBits = BitMapHeader->bfh.bfOffBits;
	memcpy( &mpNewBMP, (bufBmpFile+bfOffBits), sizeof(mpNewBMP) );

	// �����̃��C���������ǂ�ł���
	for ( int iline=0; iline < MAINMAP_PICTURE_HEIGHT; iline++ ) {
		// 1���C�����R�s�y
		memcpy( (char *)&mpKoeiBMP.pixel_line[MAINMAP_PICTURE_HEIGHT-iline-1], (char *)&mpNewBMP.pixel_line[iline], sizeof(MAINMAP_PIXEL_LINE));
		// �p���b�g��32�������ށB����̊֌W���m�肽��������Apalette255_all.act �� palette207_mainmap.act������
		// palette207_mainmap.act�̃p���b�g�ƁAkoei�p��maindata.n6p�p�̃p���b�g�ł́A32����Ă���B(mainmapdata.n6p�̕���32���l���傫��)
		// ����͒ʏ�A�palette255_all.act��0�`31�̃p���b�g�͎g��Ȃ��A��������ゾ���g����ȂǂƂ������ݒ肪�A
		// �y�C���^�[�c�[���⌸�F�c�[���ł͈�ʓI�ɂ͕s�\�Ȍ́A�擪��32�������āA palette207_mainmap.act �Ƃ����`�Œ񋟂��Ă��邽�߂��B
		// �܂��Ō��17�������Ă���B(�Ō�̕��̓p���b�g�̖�肾���Ȃ̂ŁA�l���̂��̂ɂ͉e�����Ȃ���)
		for ( int ipixel=0; ipixel<sizeof(MAINMAP_PIXEL_LINE); ipixel++) {
			mpKoeiBMP.pixel_line[MAINMAP_PICTURE_HEIGHT-iline-1].pixel[ipixel] += 32;
		}
	}

	// ���̃f�[�^��ǂݍ����
	ifstream iMapDataN6P;
	iMapDataN6P.open( outmapdatafile, ios::binary);
	iMapDataN6P.read( bufMainMapData, sizeof(bufMainMapData));
	iMapDataN6P.close();

	// ����͈̔�(�t=0�E��=1�E�H=2�E�~=3)�̂ǂꂩ���㏑��
	memcpy( (char *)(bufMainMapData+sizeof(MAINMAP_PIXCEL_BMP)*no), (char *)&mpKoeiBMP, sizeof(mpKoeiBMP) );

	// �f���o��
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

