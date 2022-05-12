#include <windows.h>
#include <fstream>
#include <iostream>
#pragma pack(1)

using namespace std;


struct BITMAPHEADER {			// �r�b�g�}�b�v���Ƃ肾���p�̃w�b�_�\���́B
	BITMAPFILEHEADER bfh;
	BITMAPINFOHEADER bih;
};


char bufBmpFileTemplate[2000]; // palette256_all.bmp�̎󂯎M
char bufBmpFile[2000000];      // �o�͂���r�b�g�}�b�v
char bufKoeiBmpData[2000000];  // bitmap.n6p�������̕��������̂��́B�v�Z���₷�����邾���̂��߁B
char bufBitMapN6P[7364608];	   // bitmap.n6p�̎󂯎M


void CpyBufFromBitMapN6P(char *inputdatafile) {
	// ���̃f�[�^��ǂݍ����
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

	// mapdata.n6p�̒�����A�ΏۂƂ���̈悾����bufKoeiBmpData�ɃR�s�[
	memcpy( bufKoeiBmpData, bufBitMapN6P+iStartAddress, iWidth*iHeight );

	// �r�b�g�}�b�v�w�b�_�[�̃C���X�^���X
	// �o�b�t�@�̐擪��BMP�w�b�_�[���|�C���^�Ƃ��Ĉ����B
	BITMAPHEADER *BitMapHeader = (BITMAPHEADER *)bufBmpFile;

	//���Əc���w��T�C�Y�ŏ㏑��
	BitMapHeader->bih.biWidth  = iWidth;
	BitMapHeader->bih.biHeight = iHeight;

	int bfOffBits = BitMapHeader->bfh.bfOffBits;

	// ���ۂ̃r�b�g�}�b�v���n�܂�X�^�[�g�|�C���g
	char *pBitMapStart = bufBmpFile + bfOffBits;

	// �P���C���P�ʂŋt�ɃR�s�[���Ă䂭�B
	for ( int iline=0; iline < iHeight; iline++ ) {
		// 1���C�����R�s�y
		memcpy( pBitMapStart+(iHeight-iline-1)*iWidth, bufKoeiBmpData+(iline*iWidth), iWidth);
	}

	// memcpy( pBitMapStart, bufKoeiBmpData, iWidth*iHeight ); // ���܂�܃R�s�[���Ƌt�ɂȂ�B
															   // �t�Ɍ����΁ABMP���㉺���]�����摜�ɂ��Ă����΁A�܂�܃R�s�[�ł��悢�̂��B

	// �f���o��
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

