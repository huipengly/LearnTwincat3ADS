// DH_Model_Example.cpp : �������̨Ӧ�ó������ڵ㡣
// ����1��Matrix4d��Vector3d ��ʹ�÷���
// ����2������ĳ�����ԭʼ�������ͬ

#include <iostream>
#include <math.h>
#include <stdio.h>           //��standard input & output"����׼���������
#include "complex"          //��������
#include "Eigen/Dense"     //�����ռ�

#include <windows.h>
//#include <vector>
#include "C:\TwinCAT\ADSApi\TcAdsDll\Include\TcAdsDef.h" //�ṹ��ͳ���������
#include "C:\TwinCAT\ADSApi\TcAdsDll\Include\TcAdsAPI.h" // ADS����������

using namespace std;
#define PI 3.1415926535897932384626433832795

// ƽ��
Eigen::Matrix4d translate(Eigen::Vector3d xyz)               //  ƽ���������任��ƽ�����ӣ� 
{
	// Identity()����Ϊ��λ�Խ��������Ͻǵ����½ǶԽ���ֵΪ1������Ϊ0�� Matrix4d��Ӧ4��4�о���Vector3d��Ӧ3ά����

	Eigen::Matrix4d T = Eigen::Matrix4d::Identity(4, 4);

	T(0, 3) = xyz(0);
	T(1, 3) = xyz(1);
	T(2, 3) = xyz(2);

	return T;
}

// ��ת
Eigen::Matrix4d rotation(char axis, double radian)           //  ��ת�������任����ת���ӣ�
{
	Eigen::Matrix4d T = Eigen::Matrix4d::Identity(4, 4);

	double ct = cos(radian);
	double st = sin(radian);

	switch (axis)
	{
	case 'x':
	case 'X':
		T << 1, 0, 0, 0,
			0, ct, -st, 0,
			0, st, ct, 0,
			0, 0, 0, 1;
		break;
	case 'y':
	case 'Y':
		T << ct, 0, st, 0,
			0, 1, 0, 0,
			-st, 0, ct, 0,
			0, 0, 0, 1;
		break;
	case 'z':
	case 'Z':
		T << ct, -st, 0, 0,
			st, ct, 0, 0,
			0, 0, 1, 0,
			0, 0, 0, 1;
		break;
	}

	return T;
}

// ΢��
Eigen::VectorXd tr2diff(Eigen::MatrixXd T1, Eigen::MatrixXd T2)     //λ��֮��
{
	Eigen::VectorXd diff(6);
	Eigen::VectorXd vCross = Eigen::VectorXd::Zero(3);

	for (int i = 0; i<3; i++)
	{
		Eigen::Vector3d vT1, vT2;
		vT1 << T1(0, i), T1(1, i), T1(2, i);
		vT2 << T2(0, i), T2(1, i), T2(2, i);

		vCross += vT1.cross(vT2);      //��ȡ������
	}

	for (int i = 0; i<3; i++)
	{
		diff(i) = T2(i, 3) - T1(i, 3);
		diff(i + 3) = 0.5*vCross(i);
	}

	return diff;
}

// ����
Eigen::MatrixXd  DH_Foward(Eigen::MatrixXd DH, Eigen::VectorXd angle)    //���˶�ѧ
{
	Eigen::Matrix4d T_60 = Eigen::Matrix4d::Identity(4, 4);

	for (int i = 0; i<DH.rows(); i++)
	{
		Eigen::Vector3d xyz;
		xyz << DH(i, 1), 0, DH(i, 3);

		DH(i, 2) = DH(i, 2) + angle(i);

		T_60 = T_60 * rotation('z', DH(i, 2)) * translate(xyz) * rotation('x', DH(i, 0));     //  DHģ��

	}

	return T_60;
}

// �ſ˱�n
Eigen::MatrixXd DH_Jacobn(Eigen::MatrixXd DH, Eigen::VectorXd angle)      //��е��ĩ���ſ˱Ⱦ���
{
	Eigen::MatrixXd Jacobn = Eigen::MatrixXd::Identity(6, 3);    //Identity()�������ڻ�е���ڵѿ����ռ�����ɶ��������������ڲ����۹ؽ�����
	Eigen::Matrix4d T_i = Eigen::Matrix4d::Identity(4, 4);
	for (int i = DH.rows() - 1; i >= 0; i--)
	{
		Eigen::Vector3d xyz;
		xyz << DH(i, 1), 0, DH(i, 3);
		DH(i, 2) = DH(i, 2) + angle(i);
		T_i = rotation('z', DH(i, 2)) * translate(xyz) * rotation('x', DH(i, 0)) * T_i;

		Eigen::Vector3d n, o, a, p;
		a << T_i(0, 0), T_i(1, 0), T_i(2, 0);
		o << T_i(0, 1), T_i(1, 1), T_i(2, 1);
		n << T_i(0, 2), T_i(1, 2), T_i(2, 2);
		p << T_i(0, 3), T_i(1, 3), T_i(2, 3);

		Jacobn(0, i) = -a(0) * p(1) + a(1) * p(0);
		Jacobn(1, i) = -o(0) * p(1) + o(1) * p(0);
		Jacobn(2, i) = -n(0) * p(1) + n(1) * p(0);
		Jacobn(3, i) = a(2);
		Jacobn(4, i) = o(2);
		Jacobn(5, i) = n(2);

	}


	return Jacobn;
}

// �ſ˱�0
Eigen::MatrixXd DH_Jacob0(Eigen::MatrixXd DH, Eigen::VectorXd angle)     //��е�ۻ����ſ˱Ⱦ���
{
	Eigen::MatrixXd Jacob0 = DH_Jacobn(DH, angle);         //Jacob0��6*3����
	Eigen::MatrixXd T = DH_Foward(DH, angle);
	Eigen::MatrixXd Transform(6, 6);
	Transform << T(0, 0), T(0, 1), T(0, 2), 0, 0, 0,
		T(1, 0), T(1, 1), T(1, 2), 0, 0, 0,
		T(2, 0), T(2, 1), T(2, 2), 0, 0, 0,
		0, 0, 0, T(0, 0), T(0, 1), T(0, 2),
		0, 0, 0, T(1, 0), T(1, 1), T(1, 2),
		0, 0, 0, T(2, 0), T(2, 1), T(2, 2);
	Jacob0 = Transform * Jacob0;           //Jacob0�õ��Ļ���6*3����

	return Jacob0;
}

// ����
Eigen::VectorXd DH_Inverse(Eigen::MatrixXd DH, Eigen::MatrixXd T, Eigen::VectorXd ref)  //�˶�ѧ���
{

	Eigen::VectorXd q = ref;                    //ref Ϊ�ο��� 

	double nm = 1.0;
	double mini = 1e-12f;
	int count = 0;
	int limitNum = 1000;

	double qlimit[3] = { 170, 120, 170 };       //�¼���Ĺؽڽ�����

	while (nm > mini)
	{
		Eigen::VectorXd e = tr2diff(DH_Foward(DH, q), T);
		cout << "rows and cols number of e is:" << e.rows() << "      " << e.cols() << endl;

		Eigen::MatrixXd Jacob0 = DH_Jacob0(DH, q);
		cout << "rows and cols number of jacob0.pinv() is:" << Jacob0.pinv().rows() << "     " << Jacob0.pinv().cols() << endl;

		Eigen::VectorXd dq = (Jacob0.pinv()) *e; //  Jacob0.pinv()��3*6����e��6*1����

		q = q + dq;

		nm = dq.norm();       //norm,��dq��ʸ������,��ʸ��dq�ĳ���

		count++;
		if (count > limitNum)
		{
			std::cout << "�����޷�����\n" << endl;
			break;
		}
	}
	std::cout << "count= " << count << std::endl;
	for (int i = 0; i<q.rows(); i++)
	{
		if (abs(q(i)) > qlimit[i] * PI / 180)
		{
			cout << "�ؽ� " << i + 1 << "  ����" << endl;
		}
	}

	return q;
}

using namespace std;

long      nErr, nPort;	//����˿ڱ���
AmsAddr   Addr;			//����AMS��ַ����
unsigned long Counter = 0;
char chooseAction;
bool run = true;
//double test[10000][7] = { 0 };
double Value1[7] = { 0, 0, 0, 0, 0, 0, 0 };
double Value2[7] = { 60, 0, 0, 0, 0, 10, 15 };
double Value3[7] = { 0, 0, 0, 0, 0, 25, 25 };
double newValue[7] = { 11.1, 11.2, 11.3, 11.4, 11.5, 11.6, 11.7 };
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

struct CartesianCoordinates
{
	double x;
	double y;
	double z;
	double a;		//��z����ת�Ƕ�
	double b;		//��y��
	double c;		//��x��
};

CartesianCoordinates cartesian_test = {-9, 600, 0, 0, 0, 0 };
bool status = false;

void send_xy();
void send_xyz();

int main()
{
	if (!Connect())
	{
		cout << "Connect success!" << endl;
	};
	//�����˶�����
	while (1)
	{
		send_xy();
	}

	nErr = AdsPortClose(); //�ر�ADSͨѶ�˿�
	if (nErr) cerr << "Error: AdsPortClose: " << nErr << '\n'; //���ر�ͨѶ�˿ڵĲ����Ƿ�ִ�гɹ�

	cout << "Send finished!" << endl;
	cout << "press any key to close..." << endl;
	getchar();
	return 0;
}

void send_xy()
{
	cout << "please input target's cartesian coordinates, use x y:" << endl;
	cin >> cartesian_test.x >> cartesian_test.y;
	AdsSyncReadWriteReq(&Addr, 0x02, 0x01, sizeof(bool), &status, sizeof(CartesianCoordinates), &cartesian_test);
	getchar();
}

void send_xyz()
{
	cout << "please input target's cartesian coordinates, use x y z:" << endl;
	cin >> cartesian_test.x >> cartesian_test.y >> cartesian_test.z;
	AdsSyncReadWriteReq(&Addr, 0x02, 0x01, sizeof(bool), &status, sizeof(CartesianCoordinates), &cartesian_test);
	getchar();
}