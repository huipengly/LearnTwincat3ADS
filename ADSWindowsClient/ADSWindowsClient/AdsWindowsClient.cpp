#include <iostream>
#include <windows.h>
//#include <vector>
#include "C:\TwinCAT\ADSApi\TcAdsDll\Include\TcAdsDef.h" //�ṹ��ͳ���������
#include "C:\TwinCAT\ADSApi\TcAdsDll\Include\TcAdsAPI.h" // ADS����������

using namespace std;

long      nErr, nPort;	//����˿ڱ���
AmsAddr   Addr;			//����AMS��ַ����
unsigned long Counter = 0;
char chooseAction;
bool run = true;
double test[10000][7] = { 0 };
double newValue[7] = { 10.1, 10.2, 10.3, 10.4, 10.5, 10.6, 10.7 };
//double newValue[7] = { 11.1, 11.2, 11.3, 11.4, 11.5, 11.6, 11.7 };
unsigned long iValue = 10;
char cArray = 'H';

int Connect()
{
	nPort = AdsPortOpen();//��ADSͨѶ�˿�
	if (nPort == 0)
	{
		return -1;
	}
	AmsNetId AmsId = { 5, 39, 221, 128, 1, 1 };
	Addr.netId = AmsId;
	Addr.port = 0x8888;

	return 0;
}

long Begin()
{
	return AdsSyncReadWriteReq(&Addr, 0x01, 0x01, 0, NULL, 0, NULL);
}

long Read()
{
	return AdsSyncReadWriteReq(&Addr, 0x01, 0x02, sizeof(unsigned long), &Counter, 0, NULL);
}

long Stop()
{
	return AdsSyncReadWriteReq(&Addr, 0x02, 0x01, sizeof(unsigned long), &Counter, 0, NULL);
}

long OverWritten()
{
	//return AdsSyncReadWriteReq(&Addr, 0x03, 0x01, 0, NULL, sizeof(cArray), &cArray);

	return AdsSyncReadWriteReqEx2(nPort, &Addr, 0x01, 0x01, 0, NULL, sizeof(newValue),
		&newValue,
		NULL);
}

long NewStart()
{
	return AdsSyncReadWriteReq(&Addr, 0x02, 0x02, 0, NULL, 0, NULL);
}

void main()
{
//	unsigned long *te = newValue;
//	unsigned long ttt = *te;
	cout << "C for Connect, B for Begin, R for Read" << endl
		<< "S for Stop O for OverWritten, N for NewStart" << endl;
	while (run)
	{
		cin >> chooseAction;
		switch (chooseAction)
		{
		case 'C':
			Connect();
			break;
		case 'B':
			Begin();
			break;
		case 'R':
			Read();
			cout << "Counter = " << Counter << endl;
			break;
		case 'S':
			Stop();
			cout << "Counter stopped value = " << Counter << endl;
			break;
		case 'O':
			//cout << "��������ֵ" << endl;
			//cin >> newValue;
			OverWritten();
			break;
		case 'N':
			NewStart();
			break;
		case 'Z':
			run = false;
			break;
		}
	}

	nErr = AdsPortClose(); //�ر�ADSͨѶ�˿�
	if (nErr) cerr << "Error: AdsPortClose: " << nErr << '\n'; //���ر�ͨѶ�˿ڵĲ����Ƿ�ִ�гɹ�
}
