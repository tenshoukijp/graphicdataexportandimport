#include <windows.h>
#include <fstream>
#include <iostream>
#pragma pack(1)

using namespace std;


struct BITMAPHEADER {			// �r�b�g�}�b�v���Ƃ肾���p�̃w�b�_�\���́B
	BITMAPFILEHEADER bfh;
	BITMAPINFOHEADER bih;
};



// bitmap.n6p�̂����A�P��([����Ƃ��O���Ƃ��]��]�̂P��)�f�[�^=��640�h�b�g*�c448���C��
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




MAINMAP_PIXCEL_BMP mpNewBMP, mpKoeiBMP; // mpNewBMP = BMP�t�@�C������̒��ڂ̎󂯎M  mpKoeiBMP = bitmap.n6p�p�̃t�H�[�}�b�g�ɕϊ�����p 

char bufBmpFile[2000000]; // bmp�t�@�C��(640*448��207�F)�̎󂯎M�B

char bufBitMapN6P[860160]; // bitmap.n6p�̎󂯎M


void func(char *outbitmapfile, char *inbmpname, int no) {
	ifstream iBmpFile;
	iBmpFile.open(inbmpname, ios::binary);

	iBmpFile.read( bufBmpFile, sizeof(bufBmpFile) );

	// �r�b�g�}�b�v�w�b�_�[�̃C���X�^���X
	// �o�b�t�@�̐擪��BMP�w�b�_�[���|�C���^�Ƃ��Ĉ����B
	BITMAPHEADER *BitMapHeader = (BITMAPHEADER *)bufBmpFile;

	//��:640 �c:448�����`�F�b�N
	if ( BitMapHeader->bih.biWidth!=MAINMAP_PICTURE_WIDTH || BitMapHeader->bih.biHeight!=MAINMAP_PICTURE_HEIGHT ) {
		cerr << inbmpname << ":" << endl;
		cerr << "�c���T�C�Y����Ă�B ��:640 �c:448 ������" << endl;
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
		// �p���b�g��32�������ށB����̊֌W���m�肽��������Apalette255_inner.act �� palette207_inner.act������
		// palette207_inner.act�̃p���b�g�ƁAkoei�p��bitmap.n6p�p�̃p���b�g�ł́A32����Ă���B(bitmap.n6p�̕���32���l���傫��)
		// ����͒ʏ�A�palette255_inner.act��0�`31�̃p���b�g�͎g��Ȃ��A��������ゾ���g����ȂǂƂ������ݒ肪�A
		// �y�C���^�[�c�[���⌸�F�c�[���ł͈�ʓI�ɂ͕s�\�Ȍ́A�擪��32�������āA palette207_inner.act �Ƃ����`�Œ񋟂��Ă��邽�߂��B
		// �܂��Ō��17�������Ă���B(�Ō�̕��̓p���b�g�̖�肾���Ȃ̂ŁA�l���̂��̂ɂ͉e�����Ȃ���)
		for ( int ipixel=0; ipixel<sizeof(MAINMAP_PIXEL_LINE); ipixel++) {
			mpKoeiBMP.pixel_line[MAINMAP_PICTURE_HEIGHT-iline-1].pixel[ipixel] += 32;
		}
	}

	// ���̃f�[�^��ǂݍ����
	ifstream iBitMapN6P;
	iBitMapN6P.open( outbitmapfile, ios::binary);
	iBitMapN6P.read( bufBitMapN6P, sizeof(bufBitMapN6P));
	iBitMapN6P.close();

	// ����͈̔�(����=0�E�O��=1�E�]��=2)�̂ǂꂩ���㏑��
	memcpy( (char *)(bufBitMapN6P+sizeof(MAINMAP_PIXCEL_BMP)*no), (char *)&mpKoeiBMP, sizeof(mpKoeiBMP) );

	// �f���o��
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

