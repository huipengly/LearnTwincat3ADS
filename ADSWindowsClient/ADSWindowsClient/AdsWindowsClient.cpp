#include <iostream>
#include <windows.h>
#include "C:\TwinCAT\ADSApi\TcAdsDll\Include\TcAdsDef.h" //�ṹ��ͳ���������
#include "C:\TwinCAT\ADSApi\TcAdsDll\Include\TcAdsAPI.h" // ADS����������

using namespace std;

long      nErr, nPort;	//����˿ڱ���
AmsAddr   Addr;			//����AMS��ַ����
int Counter = 0;;
char chooseAction;
bool run = true;
int newValue;

int Connect()
{
	nPort = AdsPortOpen();//��ADSͨѶ�˿�
	if (nPort == 0)
	{
		return -1;
	}
	AmsNetId AmsId = { 5, 53, 53, 132, 1, 1 };
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
	return AdsSyncReadWriteReq(&Addr, 0x01, 0x02, sizeof(int), &Counter, 0, NULL);
}

long Stop()
{
	return AdsSyncReadWriteReq(&Addr, 0x02, 0x01, sizeof(int), &Counter, 0, NULL);
}

long OverWritten(int* value)
{
	return AdsSyncReadWriteReq(&Addr, 0x03, 0x01, 0, NULL, sizeof(int), value);
}

long NewStart()
{
	return AdsSyncReadWriteReq(&Addr, 0x02, 0x02, 0, NULL, 0, NULL);
}

void main()
{
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
			cout << "��������ֵ" << endl;
			cin >> newValue;
			OverWritten(&newValue);
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
