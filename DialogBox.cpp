#import  "ComplexNumber.tlb" no_namespace, raw_interfaces_only
#include "Debug/complexnumber.tlh"
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include "MyDialog.h"
#include "ClassFactoryDllForChemistry.h"
#include <math.h>
#include "resource1.h"

BOOL CALLBACK MyDlgProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
void DisableChemistryAndMathematics(HWND hDlg);
void DisablePhysicsAndMathematics(HWND hDlg);
void DisablePhysicsAndChemistry(HWND hDlg);
void EnableMathematics(HWND hDlg);
void EnableChemistry(HWND hDlg);
void EnablePhysics(HWND hDlg);
void SavePhysicsResult(HWND hDlg, double result);
void SaveChemistryResult(HWND hDlg, float efficiency);
void SaveMathematicsResult(HWND hDlg, float iResultOfAddReal, float iResultOfAddImg, float iResultOfSubReal, float iResultOfSubImg, float iResultOfMulReal, float iResultOfMulImg, float iResultOfDivReal, float iResultOfDivImg);
void InitializeValues(HWND hDlg);

IChemistry *pIChemistry = NULL;

//All TCHAR Arrays taken in a common struct for input
struct MYINPUT
{
	TCHAR gravity[255], m1[255], m2[255], distance[255];
	TCHAR resultofphysics[255];
	TCHAR iT1[255], iT2[255], iQ1[255], iQ2[255], iW[255];
	TCHAR iRExp1[255], iRExp2[255], iIExp1[255], iIExp2[255];
};
MYINPUT in;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszcmdLine, int iCmdShow)
{
	WNDCLASSEX wndclass;
	HWND hwnd;
	MSG msg;
	TCHAR szAppName[] = TEXT("Science-Project");
	HRESULT hr;
	//COM Initialization
	hr = CoInitialize(NULL);

	if (FAILED(hr))
	{
		MessageBox(NULL, TEXT("COM Library cannot be initialized.\n Program will now exit."), TEXT("Program Error"), MB_OK);
		exit(0);
	}
	wndclass.cbSize = sizeof(WNDCLASSEX);
	wndclass.style = CS_HREDRAW | CS_VREDRAW;
	wndclass.cbClsExtra = 0;
	wndclass.cbWndExtra = 0;
	wndclass.lpfnWndProc = WndProc;
	wndclass.hInstance = hInstance;
	wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndclass.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(MYICON));
	wndclass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wndclass.lpszClassName = szAppName;
	wndclass.lpszMenuName = NULL;
	wndclass.hIconSm = LoadIcon(hInstance, MAKEINTRESOURCE(MYICON));

	RegisterClassEx(&wndclass);

	hwnd = CreateWindow(szAppName, TEXT("Science Project"),
		WS_OVERLAPPED,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		1925,
		1100,
		NULL,
		NULL,
		hInstance,
		NULL);

	ShowWindow(hwnd, iCmdShow);
	UpdateWindow(hwnd);

	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	CoUninitialize();
	return((int)msg.wParam);
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
	HINSTANCE hInst;

	HDC hdc1, hdc2;
	PAINTSTRUCT ps;
	RECT rc;
	static HBITMAP temp1;
	BITMAP temp2;
	HINSTANCE hInstant = NULL;

	switch (iMsg)
	{
	case WM_CREATE:
		hInstant = ((LPCREATESTRUCT)lParam)->hInstance;
		temp1 = LoadBitmap(hInstant, MAKEINTRESOURCE(MY_BITMAP));
		break;

	case WM_PAINT:
		hdc1 = BeginPaint(hwnd, &ps);
		GetClientRect(hwnd, &rc);

		hdc2 = CreateCompatibleDC(hdc1);
		SelectObject(hdc2, temp1);
		GetObject(temp1, sizeof(BITMAP), &temp2);
		BitBlt(hdc1, 50, 1, temp2.bmWidth, temp2.bmHeight, hdc2, 0, 0, SRCCOPY);
		DeleteDC(hdc2);
		EndPaint(hwnd, &ps);
		break;

	case WM_KEYDOWN:
		switch (wParam)
		{
			//space bar ascii value
		case 0x20:
			hInst = (HINSTANCE)GetWindowLong(hwnd, GWL_HINSTANCE);
			DialogBox(hInst, MAKEINTRESOURCE(ID_DATAENTRY), hwnd, reinterpret_cast<DLGPROC>(MyDlgProc));
			break;
		}
		//esc ascii value
	case 27:
		DestroyWindow(hwnd);
		break;

	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	}
	return(DefWindowProc(hwnd, iMsg, wParam, lParam));
}

BOOL CALLBACK MyDlgProc(HWND hDlg, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
	//for phyisics
	HWND hwndOfThatContent = 0;
	TCHAR str[255];
	HMODULE hLib = NULL;
	typedef float(*pfnMagnitudeOfForce)(float, float, float, float);
	pfnMagnitudeOfForce pfn = NULL;
	double result = 0;
	BOOL temp = 0;

	//for chemistry class factory
	void SafeInterfaceRelease(void);
	HRESULT hr;
	double iEfficiency = 0;
	float efficiency = 0;

	//for mathematics
	float iResultOfAddReal = 0, iResultOfAddImg = 0, iResultOfSubReal = 0, iResultOfSubImg = 0,
		iResultOfMulReal = 0, iResultOfMulImg = 0, iResultOfDivReal = 0, iResultOfDivImg = 0;
	CLSID clsidComplex;

	HDC hdc;
	HBRUSH hbrBkgnd = NULL;
	switch (iMsg)
	{
	case WM_INITDIALOG:
		SendDlgItemMessage(hDlg, ID_RBPHYSICS, BM_SETCHECK, 1, 0);
		DisableChemistryAndMathematics(hDlg);
		InitializeValues(hDlg);
		return(TRUE);
		break;

	case WM_CTLCOLORDLG:
		hdc = HDC(wParam);
		SetBkColor(hdc, RGB(30, 30, 30));
		if (hbrBkgnd == NULL)
		{
			hbrBkgnd = CreateSolidBrush(RGB(30, 30, 30));
		}
		return (INT_PTR)hbrBkgnd;
		break;

	case WM_CTLCOLORSTATIC:
		hdc = HDC(wParam);
		SetTextColor(hdc, RGB(30, 148, 148));
		SetBkColor(hdc, RGB(30, 30, 30));
		if (hbrBkgnd == NULL)
		{
			hbrBkgnd = CreateSolidBrush(RGB(30, 30, 30));
		}
		return (INT_PTR)hbrBkgnd;
		break;

	case WM_CTLCOLOREDIT:
		hdc = HDC(wParam);
		SetTextColor(hdc, RGB(255, 255, 255));
		SetBkColor(hdc, RGB(60, 60, 60));
		return(INT_PTR)CreateSolidBrush(RGB(60, 60, 60));
		break;

	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case ID_RBPHYSICS:
			DisableChemistryAndMathematics(hDlg);
			EnablePhysics(hDlg);
			break;

		case ID_RBCHEMISTRY:
			DisablePhysicsAndMathematics(hDlg);
			EnableChemistry(hDlg);
			break;

		case ID_RBMATHS:
			DisablePhysicsAndChemistry(hDlg);
			EnableMathematics(hDlg);
			break;

			//THIS CASE INDICATES CALCULATION FOR PHYSICS
		case ID_PHYPBCONTINUE:
			hLib = LoadLibrary(TEXT("ExplicitDllForPhysics.dll"));

			pfn = (pfnMagnitudeOfForce)GetProcAddress(hLib, "MagnitudeOfForce");
			if (hLib == NULL)
			{
				MessageBox(hDlg, TEXT("Cannot Load Dll\n"), TEXT("Error"), MB_OK);
				DestroyWindow(hDlg);
			}
			GetDlgItemText(hDlg, ID_ETG, (in.gravity), 255);
			GetDlgItemText(hDlg, ID_ETM1, (in.m1), 255);
			GetDlgItemText(hDlg, ID_ETM2, (in.m2), 255);
			GetDlgItemText(hDlg, ID_ETDSQUARE, (in.distance), 255);

			if (GetDlgItemText(hDlg, ID_ETG, (in.gravity), 255) == NULL || GetDlgItemText(hDlg, ID_ETM1, (in.m1), 255) == NULL
				|| GetDlgItemText(hDlg, ID_ETM2, (in.m2), 255) == NULL || GetDlgItemText(hDlg, ID_ETDSQUARE, (in.distance), 255) == NULL)
			{
				MessageBox(NULL, TEXT("Values cannot be Null"), TEXT("Input Error"), MB_OK | MB_ICONWARNING);
			}
			else
			{
				result = pfn(_wtof(in.gravity), _wtof(in.m1), _wtof(in.m2), _wtof(in.distance));
				swprintf_s(str, TEXT("Force of Gravitation : %f"), result);
				SetDlgItemText(hDlg, ID_ETFG, str);
				SavePhysicsResult(hDlg, result);
			}
			break;

		case ID_PHYDETAIL:
			MessageBox(hDlg, TEXT("INTRODUCTION:\n\nThe universe has a lot of forces, a lot of pushes and pulls."
				" We're always pushing or pulling something, even if only the ground."
				" But it turns out that in physics, there are really only four fundamental forces from which everything else is derived: the strong force, the weak force, the electromagnetic force, and the gravitational force.\n"
				"The gravitational force is a force that attracts any two objects with mass.We call the gravitational force attractive because it always tries to pull masses together, it never pushes them apart."
				"In fact, every object, including you, is pulling on every other object in the entire universe!This is called Newton's Universal Law of Gravitation."
				" Admittedly, you don't have a very large mass and so, you're not pulling on those other objects much."
				" And objects that are really far apart from each other dont pull on each other noticeably either."
				"But the force is there and we can calculate it.\n\nUniversal Gravitation Equation\n\n"
				"This equation describes the force between any two objects in the universe :\nIn the equation :\n\n"
				"1. F is the force of gravity(measured in Newtons, N)\n\n"
				"2. G is the gravitational constant of the universe and is always the same number\n\n"
				"3. M is the mass of one object(measured in kilograms, kg)\n\n"
				"4. m is the mass of the other object(measured in kilograms, kg)\n\n"
				"5. r is the distance those objects are apart(measured in meters, m)\n\n"
				"So if you know how massive two objects are and how far they are apart, you can figure out the force between them. "),
				TEXT("What is Gravitational Force?"), MB_ICONINFORMATION | MB_OK);
			break;

		case ID_PHYCLEAR:
			SetDlgItemText(hDlg, ID_ETG, 0);
			SetDlgItemText(hDlg, ID_ETM1, 0);
			SetDlgItemText(hDlg, ID_ETM2, 0);
			SetDlgItemText(hDlg, ID_ETDSQUARE, 0);
			SetDlgItemText(hDlg, ID_ETFG, 0);
			break;

			//THIS CASE INDICATES CALCULATION FOR CHEMISTRY
		case ID_PBCHMCONTINUE:
			hr = CoCreateInstance(CLSID_ChemistryOperation, NULL, CLSCTX_INPROC_SERVER, IID_IChemistry, (void**)&pIChemistry);
			if (FAILED(hr))
			{
				MessageBox(hDlg, TEXT("IChemistry Interface can not be obtained"), TEXT("Error"), MB_OK);
				DestroyWindow(hDlg);
			}

			//using Tempreature
			GetDlgItemText(hDlg, ID_ETT1, in.iT1, 255);
			GetDlgItemText(hDlg, ID_ETT2, in.iT2, 255);
			if (GetDlgItemText(hDlg, ID_ETT1, in.iT1, 255) == NULL || GetDlgItemText(hDlg, ID_ETT2, in.iT2, 255) == NULL
				|| GetDlgItemText(hDlg, ID_ETQ1, in.iQ1, 255) == NULL || GetDlgItemText(hDlg, ID_ETQ2, in.iQ2, 255) == NULL
				|| GetDlgItemText(hDlg, ID_ETW, in.iW, 255) == NULL)
			{
				MessageBox(NULL, TEXT("Values cannot be Null"), TEXT("Input Error"), MB_OK | MB_ICONWARNING);
			}
			else
			{
				pIChemistry->EfficiencyInCarnotCycleByTempreature(_wtof(in.iT1), _wtof(in.iT2), &efficiency);
				swprintf_s(str, TEXT("%f"), efficiency);
				SetDlgItemText(hDlg, ID_ETTEMP, str);

				//Using Heat
				GetDlgItemText(hDlg, ID_ETQ1, in.iQ1, 255);
				GetDlgItemText(hDlg, ID_ETQ2, in.iQ2, 255);
				pIChemistry->EfficiencyInCarnotCycleByHeat(_wtof(in.iQ1), _wtof(in.iQ2), &efficiency);
				swprintf_s(str, TEXT("%f"), efficiency);
				SetDlgItemText(hDlg, ID_ETTHEAT, str);

				//using work
				GetDlgItemText(hDlg, ID_ETW, in.iW, 255);
				pIChemistry->EfficiencyInCarnotCycleByWork(_wtof(in.iW), _wtof(in.iQ2), &efficiency);
				swprintf_s(str, TEXT("%f"), efficiency);
				SetDlgItemText(hDlg, ID_ETWORK, str);
				SaveChemistryResult(hDlg, efficiency);
				pIChemistry->Release();
				pIChemistry = NULL;

			}
			break;

		case ID_PBCHMCLEAR:
			SetDlgItemText(hDlg, ID_ETT1, 0);
			SetDlgItemText(hDlg, ID_ETT2, 0);
			SetDlgItemText(hDlg, ID_ETQ1, 0);
			SetDlgItemText(hDlg, ID_ETQ2, 0);
			SetDlgItemText(hDlg, ID_ETW, 0);
			SetDlgItemText(hDlg, ID_ETTEMP, 0);
			SetDlgItemText(hDlg, ID_ETTHEAT, 0);
			SetDlgItemText(hDlg, ID_ETWORK, 0);
			break;

		case ID_PBCHMDETAIL:
			MessageBox(hDlg, TEXT("INTRODUCTION:"
				"\n\nIn the early 19th century, steam engines came to play an increasingly important role in industry and transportation."
				"However, a systematic set of theories of the conversion of thermal energy to motive power by steam engines had not yet been developed."
				"Nicolas Léonard Sadi Carnot (1796-1832), a French military engineer, published Reflections on the Motive Power of Fire in 1824."
				"The book proposed a generalized theory of heat engines, as well as an idealized model of a thermodynamic system for a heat engine that is now known as thea Carnot cycle."
				"Carnot developed the foundation of the second law of thermodynamics, and is often described as the 'Father of thermodynamics'."
				"\n\nThe Carnot cycle consists of the following four processes:\n\n"
				"1.	A reversible isothermal gas expansion process.In this process, the ideal gas in the system absorbs qin amount heat from a heat source at a high temperature Th, expands and does work on surroundings.\n\n"
				"2.	A reversible adiabatic gas expansion process.In this process, the system is thermally insulated.The gas continues to expand and do work on surroundings, which causes the system to cool to a lower temperature, Tl.\n\n"
				"3.	A reversible isothermal gas compression process.In this process, surroundings do work to the gas at Tl, and causes a loss of heat, qout.\n\n4.	A reversible adiabatic gas compression process."
				"In this process, the system is thermally insulated.Surroundings continue to do work to the gas, which causes the temperature to rise back to Th."),
				TEXT("What is Carnot Cycle?"), MB_ICONINFORMATION | MB_OK);
			break;

			//THIS CASE INDICATES CALCULATION FOR MATHEMATICS
		case ID_PBOPERATE:

			GetDlgItemText(hDlg, ID_ETREALE1, (in.iRExp1), 255);
			GetDlgItemText(hDlg, ID_ETREALE2, (in.iRExp2), 255);
			GetDlgItemText(hDlg, ID_ETIMGE1, (in.iIExp1), 255);
			GetDlgItemText(hDlg, ID_ETIMGE2, (in.iIExp2), 255);

			if (GetDlgItemText(hDlg, ID_ETREALE1, (in.iRExp1), 255) == NULL || GetDlgItemText(hDlg, ID_ETREALE2, (in.iRExp2), 255) == NULL
				|| GetDlgItemText(hDlg, ID_ETIMGE1, (in.iIExp1), 255) == NULL || GetDlgItemText(hDlg, ID_ETIMGE2, (in.iIExp2), 255) == NULL)
			{
				MessageBox(NULL, TEXT("Values cannot be Null"), TEXT("Input Error"), MB_OK | MB_ICONWARNING);
			}
			else
			{
				IComplexNumber *app;
				hr = CLSIDFromProgID(L"ManagedServerForInterop.ComplexNumber", &clsidComplex);

				if (FAILED(hr))
				{
					MessageBox(hDlg, str, TEXT("Error in HR"), MB_OK);
					DestroyWindow(hDlg);
				}

				hr = CoCreateInstance(clsidComplex, NULL, CLSCTX_INPROC_SERVER, __uuidof(IComplexNumber), (VOID**)&app);
				if (FAILED(hr))
				{
					MessageBox(hDlg, str, TEXT("Error in HR"), MB_OK);
					DestroyWindow(hDlg);
				}
				//Addition Operation in Complex number
				hr = app->AdditionForRealPart(_wtof(in.iRExp1), _wtof(in.iRExp2), &iResultOfAddReal);

				if (FAILED(hr))
				{
					MessageBox(hDlg, str, TEXT("Error in HR"), MB_OK);
					DestroyWindow(hDlg);
				}
				swprintf_s(str, TEXT("%f"), iResultOfAddReal);
				SetDlgItemText(hDlg, ID_ETADDREAL, str);

				hr = app->AdditionForImgPart(_wtof(in.iIExp1), _wtof(in.iIExp2), &iResultOfAddImg);
				if (FAILED(hr))
				{
					MessageBox(hDlg, str, TEXT("Error in HR"), MB_OK);
					DestroyWindow(hDlg);
				}
				swprintf_s(str, TEXT("%f"), iResultOfAddImg);
				SetDlgItemText(hDlg, ID_ETADDIMG, str);

				//Subtraction Opearation in Complex number
				hr = app->SubtractionForReadPart(_wtof(in.iRExp1), _wtof(in.iRExp2), &iResultOfSubReal);
				if (FAILED(hr))
				{
					MessageBox(hDlg, str, TEXT("Error in HR"), MB_OK);
					DestroyWindow(hDlg);
				}
				swprintf_s(str, TEXT("%f"), iResultOfSubReal);
				SetDlgItemText(hDlg, ID_ETSUBREAL, str);

				hr = app->SubtractionForImgPart(_wtof(in.iIExp1), _wtof(in.iIExp2), &iResultOfSubImg);
				if (FAILED(hr))
				{
					MessageBox(hDlg, str, TEXT("Error in HR"), MB_OK);
					DestroyWindow(hDlg);
				}
				swprintf_s(str, TEXT("%f"), iResultOfSubImg);
				SetDlgItemText(hDlg, ID_ETSUBIMG, str);

				//Multiplication Operation in Complex Number
				hr = app->MultiplicationForRealPart(_wtof(in.iRExp1), _wtof(in.iRExp2), _wtof(in.iIExp1), _wtof(in.iIExp2), &iResultOfMulReal);
				if (FAILED(hr))
				{
					MessageBox(hDlg, str, TEXT("Error in HR"), MB_OK);
					DestroyWindow(hDlg);
				}
				swprintf_s(str, TEXT("%f"), iResultOfMulReal);
				SetDlgItemText(hDlg, ID_ETMULREAL, str);

				hr = app->MultiplicationForImgPart(_wtof(in.iRExp1), _wtof(in.iIExp2), _wtof(in.iIExp1), _wtof(in.iRExp2), &iResultOfMulImg);
				if (FAILED(hr))
				{
					MessageBox(hDlg, str, TEXT("Error in HR"), MB_OK);
					DestroyWindow(hDlg);
				}
				swprintf_s(str, TEXT("%f"), iResultOfMulImg);
				SetDlgItemText(hDlg, ID_ETMULIMG, str);

				//Division Operation in Complex Number
				hr = app->DivisionForRealPart(_wtof(in.iRExp1), _wtof(in.iRExp2), _wtof(in.iIExp1), _wtof(in.iIExp2), &iResultOfDivReal);
				if (FAILED(hr))
				{
					MessageBox(hDlg, str, TEXT("Error in HR"), MB_OK);
					DestroyWindow(hDlg);
				}
				swprintf_s(str, TEXT("%f"), iResultOfDivReal);
				SetDlgItemText(hDlg, ID_ETDIVREAL, str);

				hr = app->DivisionForImgPart(_wtof(in.iRExp1), _wtof(in.iRExp2), _wtof(in.iIExp1), _wtof(in.iIExp2), &iResultOfDivImg);
				if (FAILED(hr))
				{
					MessageBox(hDlg, str, TEXT("Error in HR"), MB_OK);
					DestroyWindow(hDlg);
				}
				swprintf_s(str, TEXT("%f"), iResultOfDivImg);
				SetDlgItemText(hDlg, ID_ETDIVIMG, str);
				SaveMathematicsResult(hDlg, iResultOfAddReal, iResultOfAddImg, iResultOfSubReal, iResultOfSubImg, iResultOfMulReal, iResultOfMulImg, iResultOfDivReal, iResultOfDivImg);
			}
			break;

		case ID_COMPLEXCLEAR:
			SetDlgItemText(hDlg, ID_ETREALE1, 0);
			SetDlgItemText(hDlg, ID_ETREALE2, 0);
			SetDlgItemText(hDlg, ID_ETADDREAL, 0);
			SetDlgItemText(hDlg, ID_ETSUBREAL, 0);
			SetDlgItemText(hDlg, ID_ETIMGE1, 0);
			SetDlgItemText(hDlg, ID_ETIMGE2, 0);
			SetDlgItemText(hDlg, ID_ETADDIMG, 0);
			SetDlgItemText(hDlg, ID_ETSUBIMG, 0);
			SetDlgItemText(hDlg, ID_ETMULREAL, 0);
			SetDlgItemText(hDlg, ID_ETMULIMG, 0);
			SetDlgItemText(hDlg, ID_ETDIVREAL, 0);
			SetDlgItemText(hDlg, ID_ETDIVIMG, 0);
			break;

		case ID_PBCPLXDETAIL:
			MessageBox(hDlg, TEXT("A complex number is a number that can be expressed in the form a + bi,"
				" where a and b are real numbers, and i is a solution of the equation x2 = −1."
				"Because no real number satisfies this equation, i is called an imaginary number."
				"For the complex number a + bi, a is called the real part, and b is called the imaginary part."
				"Despite the historical nomenclature imaginary,"
				"complex numbers are regarded in the mathematical sciences as just as real as the real numbers,"
				"and are fundamental in many aspects of the scientific description of the natural world."
				"A complex number is a number of the form a + bi, where a and b are real numbers and i is an indeterminate satisfying i2 = −1. For example, 2 + 3i is a complex number."),
				TEXT("What are Complex Numbers?"), MB_ICONINFORMATION | MB_OK);
			break;

		case ID_CANCEL:
			int flag = 1;
			FreeLibrary(hLib);
			if (flag == 1)
			{
				MessageBox(hDlg, TEXT("Are you sure you want to exit?"), TEXT("End Task"), MB_ICONQUESTION | MB_OK);
				EndDialog(hDlg, 0);
				PostQuitMessage(0);
			}
			break;
		}
		break;
		return(TRUE);
	}
	return(FALSE);
}
void safeInterfaceRelease(void)
{
	if (pIChemistry)
	{
		pIChemistry->Release();
		pIChemistry = NULL;
	}
}

void DisableChemistryAndMathematics(HWND hDlg)
{
	//Chemistry
	EnableWindow(GetDlgItem(hDlg, ID_ETT1), FALSE);
	EnableWindow(GetDlgItem(hDlg, ID_ETT2), FALSE);
	EnableWindow(GetDlgItem(hDlg, ID_ETQ1), FALSE);
	EnableWindow(GetDlgItem(hDlg, ID_ETQ2), FALSE);
	EnableWindow(GetDlgItem(hDlg, ID_ETW), FALSE);
	EnableWindow(GetDlgItem(hDlg, ID_ETTEMP), FALSE);
	EnableWindow(GetDlgItem(hDlg, ID_ETTHEAT), FALSE);
	EnableWindow(GetDlgItem(hDlg, ID_ETWORK), FALSE);
	EnableWindow(GetDlgItem(hDlg, ID_PBCHMCONTINUE), FALSE);
	EnableWindow(GetDlgItem(hDlg, ID_PBCHMCLEAR), FALSE);
	EnableWindow(GetDlgItem(hDlg, ID_PBCHMDETAIL), FALSE);
	//maths
	EnableWindow(GetDlgItem(hDlg, ID_ETREALE1), FALSE);
	EnableWindow(GetDlgItem(hDlg, ID_ETREALE2), FALSE);
	EnableWindow(GetDlgItem(hDlg, ID_ETADDREAL), FALSE);
	EnableWindow(GetDlgItem(hDlg, ID_ETSUBREAL), FALSE);
	EnableWindow(GetDlgItem(hDlg, ID_ETIMGE1), FALSE);
	EnableWindow(GetDlgItem(hDlg, ID_ETIMGE2), FALSE);
	EnableWindow(GetDlgItem(hDlg, ID_ETADDIMG), FALSE);
	EnableWindow(GetDlgItem(hDlg, ID_ETSUBIMG), FALSE);
	EnableWindow(GetDlgItem(hDlg, ID_ETMULREAL), FALSE);
	EnableWindow(GetDlgItem(hDlg, ID_ETMULIMG), FALSE);
	EnableWindow(GetDlgItem(hDlg, ID_ETDIVREAL), FALSE);
	EnableWindow(GetDlgItem(hDlg, ID_ETDIVIMG), FALSE);
	EnableWindow(GetDlgItem(hDlg, ID_PBOPERATE), FALSE);
	EnableWindow(GetDlgItem(hDlg, ID_COMPLEXCLEAR), FALSE);
	EnableWindow(GetDlgItem(hDlg, ID_PBCPLXDETAIL), FALSE);
}
void DisablePhysicsAndMathematics(HWND hDlg)
{
	//physics
	EnableWindow(GetDlgItem(hDlg, ID_ETG), FALSE);
	EnableWindow(GetDlgItem(hDlg, ID_ETM1), FALSE);
	EnableWindow(GetDlgItem(hDlg, ID_ETM2), FALSE);
	EnableWindow(GetDlgItem(hDlg, ID_ETDSQUARE), FALSE);
	EnableWindow(GetDlgItem(hDlg, ID_ETFG), FALSE);
	EnableWindow(GetDlgItem(hDlg, ID_PHYPBCONTINUE), FALSE);
	EnableWindow(GetDlgItem(hDlg, ID_PHYCLEAR), FALSE);
	EnableWindow(GetDlgItem(hDlg, ID_PHYDETAIL), FALSE);
	//maths
	EnableWindow(GetDlgItem(hDlg, ID_ETREALE1), FALSE);
	EnableWindow(GetDlgItem(hDlg, ID_ETREALE2), FALSE);
	EnableWindow(GetDlgItem(hDlg, ID_ETADDREAL), FALSE);
	EnableWindow(GetDlgItem(hDlg, ID_ETSUBREAL), FALSE);
	EnableWindow(GetDlgItem(hDlg, ID_ETIMGE1), FALSE);
	EnableWindow(GetDlgItem(hDlg, ID_ETIMGE2), FALSE);
	EnableWindow(GetDlgItem(hDlg, ID_ETADDIMG), FALSE);
	EnableWindow(GetDlgItem(hDlg, ID_ETSUBIMG), FALSE);
	EnableWindow(GetDlgItem(hDlg, ID_ETMULREAL), FALSE);
	EnableWindow(GetDlgItem(hDlg, ID_ETMULIMG), FALSE);
	EnableWindow(GetDlgItem(hDlg, ID_ETDIVREAL), FALSE);
	EnableWindow(GetDlgItem(hDlg, ID_ETDIVIMG), FALSE);
	EnableWindow(GetDlgItem(hDlg, ID_PBOPERATE), FALSE);
	EnableWindow(GetDlgItem(hDlg, ID_COMPLEXCLEAR), FALSE);
	EnableWindow(GetDlgItem(hDlg, ID_PBCPLXDETAIL), FALSE);
}
void DisablePhysicsAndChemistry(HWND hDlg)
{
	//physics
	EnableWindow(GetDlgItem(hDlg, ID_ETG), FALSE);
	EnableWindow(GetDlgItem(hDlg, ID_ETM1), FALSE);
	EnableWindow(GetDlgItem(hDlg, ID_ETM2), FALSE);
	EnableWindow(GetDlgItem(hDlg, ID_ETDSQUARE), FALSE);
	EnableWindow(GetDlgItem(hDlg, ID_ETFG), FALSE);
	EnableWindow(GetDlgItem(hDlg, ID_PHYPBCONTINUE), FALSE);
	EnableWindow(GetDlgItem(hDlg, ID_PHYCLEAR), FALSE);
	EnableWindow(GetDlgItem(hDlg, ID_PHYDETAIL), FALSE);
	//chemistry
	EnableWindow(GetDlgItem(hDlg, ID_ETT1), FALSE);
	EnableWindow(GetDlgItem(hDlg, ID_ETT2), FALSE);
	EnableWindow(GetDlgItem(hDlg, ID_ETQ1), FALSE);
	EnableWindow(GetDlgItem(hDlg, ID_ETQ2), FALSE);
	EnableWindow(GetDlgItem(hDlg, ID_ETW), FALSE);
	EnableWindow(GetDlgItem(hDlg, ID_ETTEMP), FALSE);
	EnableWindow(GetDlgItem(hDlg, ID_ETTHEAT), FALSE);
	EnableWindow(GetDlgItem(hDlg, ID_ETWORK), FALSE);
	EnableWindow(GetDlgItem(hDlg, ID_PBCHMCONTINUE), FALSE);
	EnableWindow(GetDlgItem(hDlg, ID_PBCHMCLEAR), FALSE);
	EnableWindow(GetDlgItem(hDlg, ID_PBCHMDETAIL), FALSE);
}

void EnablePhysics(HWND hDlg)
{
	EnableWindow(GetDlgItem(hDlg, ID_ETG), TRUE);
	EnableWindow(GetDlgItem(hDlg, ID_ETM1), TRUE);
	EnableWindow(GetDlgItem(hDlg, ID_ETM2), TRUE);
	EnableWindow(GetDlgItem(hDlg, ID_ETDSQUARE), TRUE);
	EnableWindow(GetDlgItem(hDlg, ID_ETFG), TRUE);
	EnableWindow(GetDlgItem(hDlg, ID_PHYPBCONTINUE), TRUE);
	EnableWindow(GetDlgItem(hDlg, ID_PHYCLEAR), TRUE);
	EnableWindow(GetDlgItem(hDlg, ID_PHYDETAIL), TRUE);
}

void EnableChemistry(HWND hDlg)
{
	EnableWindow(GetDlgItem(hDlg, ID_ETT1), TRUE);
	EnableWindow(GetDlgItem(hDlg, ID_ETT2), TRUE);
	EnableWindow(GetDlgItem(hDlg, ID_ETQ1), TRUE);
	EnableWindow(GetDlgItem(hDlg, ID_ETQ2), TRUE);
	EnableWindow(GetDlgItem(hDlg, ID_ETW), TRUE);
	EnableWindow(GetDlgItem(hDlg, ID_ETTEMP), TRUE);
	EnableWindow(GetDlgItem(hDlg, ID_ETTHEAT), TRUE);
	EnableWindow(GetDlgItem(hDlg, ID_ETWORK), TRUE);
	EnableWindow(GetDlgItem(hDlg, ID_PBCHMCONTINUE), TRUE);
	EnableWindow(GetDlgItem(hDlg, ID_PBCHMCLEAR), TRUE);
	EnableWindow(GetDlgItem(hDlg, ID_PBCHMDETAIL), TRUE);
}

void EnableMathematics(HWND hDlg)
{
	EnableWindow(GetDlgItem(hDlg, ID_ETREALE1), TRUE);
	EnableWindow(GetDlgItem(hDlg, ID_ETREALE2), TRUE);
	EnableWindow(GetDlgItem(hDlg, ID_ETADDREAL), TRUE);
	EnableWindow(GetDlgItem(hDlg, ID_ETSUBREAL), TRUE);
	EnableWindow(GetDlgItem(hDlg, ID_ETIMGE1), TRUE);
	EnableWindow(GetDlgItem(hDlg, ID_ETIMGE2), TRUE);
	EnableWindow(GetDlgItem(hDlg, ID_ETADDIMG), TRUE);
	EnableWindow(GetDlgItem(hDlg, ID_ETSUBIMG), TRUE);
	EnableWindow(GetDlgItem(hDlg, ID_ETMULREAL), TRUE);
	EnableWindow(GetDlgItem(hDlg, ID_ETMULIMG), TRUE);
	EnableWindow(GetDlgItem(hDlg, ID_ETDIVREAL), TRUE);
	EnableWindow(GetDlgItem(hDlg, ID_ETDIVIMG), TRUE);
	EnableWindow(GetDlgItem(hDlg, ID_PBOPERATE), TRUE);
	EnableWindow(GetDlgItem(hDlg, ID_COMPLEXCLEAR), TRUE);
	EnableWindow(GetDlgItem(hDlg, ID_PBCPLXDETAIL), TRUE);
}

void SavePhysicsResult(HWND hDlg, double result)
{
	FILE *fptr;
	fopen_s(&fptr, "physics.txt", "w");
	fprintf_s(fptr, "\t\t####################################################\n\n");
	fprintf_s(fptr, "\t\tRESULTS OF THE OPERATION OF TOPIC - PHYSICS\n\n");
	fprintf_s(fptr, "\t\t####################################################\n\n");
	fprintf_s(fptr, "\t\tForce of Gravitation : %f\n\n", result);
	fprintf_s(fptr, "\t\t####################################################\n\n");
	fclose(fptr);
	MessageBox(NULL, TEXT("Result Stored in File Successfully"), TEXT("Success"), MB_ICONINFORMATION | MB_OK);
}

void SaveChemistryResult(HWND hDlg, float efficiency)
{
	FILE *fptr;

	fopen_s(&fptr, "chemistry.txt", "w");
	fprintf_s(fptr, "\t\t####################################################\n\n");
	fprintf_s(fptr, "\t\tRESULTS OF THE OPERATIONS OF TOPIC - CHEMISTRY\n\n");
	fprintf_s(fptr, "\t\t####################################################\n\n");
	pIChemistry->EfficiencyInCarnotCycleByTempreature(_wtof(in.iT1), _wtof(in.iT2), &efficiency);
	fprintf_s(fptr, "\t\tEfficiency in carnot cycle is using tempreature is: %f\n\n", efficiency);
	pIChemistry->EfficiencyInCarnotCycleByHeat(_wtof(in.iQ1), _wtof(in.iQ2), &efficiency);
	fprintf_s(fptr, "\t\tEfficiency in carnot cycle is using Heat is: %f\n\n", efficiency);
	pIChemistry->EfficiencyInCarnotCycleByWork(_wtof(in.iW), _wtof(in.iQ2), &efficiency);
	fprintf_s(fptr, "\t\tEfficiency in carnot cycle is using work is: %f\n\n", efficiency);
	fprintf_s(fptr, "\t\t####################################################\n\n");
	fclose(fptr);
	MessageBox(NULL, TEXT("Result Stored in File Successfully"), TEXT("Success"), MB_ICONINFORMATION | MB_OK);
}

void SaveMathematicsResult(HWND hDlg, float iResultOfAddReal, float iResultOfAddImg, float iResultOfSubReal, float iResultOfSubImg,
	float iResultOfMulReal, float iResultOfMulImg, float iResultOfDivReal, float iResultOfDivImg)
{
	FILE *fptr;

	fopen_s(&fptr, "mathematics.txt", "w");
	fprintf_s(fptr, "\t\t####################################################\n\n");
	fprintf_s(fptr, "\t\tRESULTS OF THE OPERATIONS OF TOPIC - COMPLEX NUMBERS\n\n");
	fprintf_s(fptr, "\t\t####################################################\n\n");
	fprintf_s(fptr, "\t\tResult for Addition is (%f) + i(%f)\n\n", iResultOfAddReal, iResultOfAddImg);
	fprintf_s(fptr, "\t\tResult for Subtraction is (%f) + i(%f)\n\n", iResultOfSubReal, iResultOfSubImg);
	fprintf_s(fptr, "\t\tResult for Multiplication is (%f) + i(%f)\n\n", iResultOfMulReal, iResultOfMulImg);
	fprintf_s(fptr, "\t\tResult for Division is (%f) + i(%f)\n\n", iResultOfDivReal, iResultOfDivImg);
	fprintf_s(fptr, "\t\t####################################################\n\n");
	fclose(fptr);
	MessageBox(NULL, TEXT("Result Stored in File Successfully"), TEXT("Success"), MB_ICONINFORMATION | MB_OK);
}
void InitializeValues(HWND hDlg)
{
	TCHAR str[255];
	//Initialize values for Physics
	swprintf_s(str, TEXT("%f"), 120.0);
	SetDlgItemText(hDlg, ID_ETG, str);
	swprintf_s(str, TEXT("%f"), 510.0);
	SetDlgItemText(hDlg, ID_ETM1, str);
	swprintf_s(str, TEXT("%f"), 107.0);
	SetDlgItemText(hDlg, ID_ETM2, str);
	swprintf_s(str, TEXT("%f"), 120.0);
	SetDlgItemText(hDlg, ID_ETDSQUARE, str);
	swprintf_s(str, TEXT("Force of Gravitation : %f"), 454.750000);
	SetDlgItemText(hDlg, ID_ETFG, str);

	//Initialize values for Chemistry
	swprintf_s(str, TEXT("%f"), 121.0);
	SetDlgItemText(hDlg, ID_ETT1, str);
	swprintf_s(str, TEXT("%f"), 126.0);
	SetDlgItemText(hDlg, ID_ETT2, str);
	swprintf_s(str, TEXT("%f"), 122.0);
	SetDlgItemText(hDlg, ID_ETQ1, str);
	swprintf_s(str, TEXT("%f"), 332.0);
	SetDlgItemText(hDlg, ID_ETQ2, str);
	swprintf_s(str, TEXT("%f"), 162.0);
	SetDlgItemText(hDlg, ID_ETW, str);
	swprintf_s(str, TEXT("%f"), 0.039683);
	SetDlgItemText(hDlg, ID_ETTEMP, str);
	swprintf_s(str, TEXT("%f"), 0.632530);
	SetDlgItemText(hDlg, ID_ETTHEAT, str);
	swprintf_s(str, TEXT("%f"), 0.487952);
	SetDlgItemText(hDlg, ID_ETWORK, str);

	//Initialize values for Mathematics
	swprintf_s(str, TEXT("%f"), 121.0);
	SetDlgItemText(hDlg, ID_ETREALE1, str);
	swprintf_s(str, TEXT("%f"), 126.0);
	SetDlgItemText(hDlg, ID_ETREALE2, str);
	swprintf_s(str, TEXT("%f"), 122.0);
	SetDlgItemText(hDlg, ID_ETIMGE1, str);
	swprintf_s(str, TEXT("%f"), 332.0);
	SetDlgItemText(hDlg, ID_ETIMGE2, str);
	swprintf_s(str, TEXT("%f"), 247.000000);
	SetDlgItemText(hDlg, ID_ETADDREAL, str);
	swprintf_s(str, TEXT("%f"), 454.000000);
	SetDlgItemText(hDlg, ID_ETADDIMG, str);
	swprintf_s(str, TEXT("%f"), -5.000000);
	SetDlgItemText(hDlg, ID_ETSUBREAL, str);
	swprintf_s(str, TEXT("%f"), -210.000000);
	SetDlgItemText(hDlg, ID_ETSUBIMG, str);
	swprintf_s(str, TEXT("%f"), -25258.000000);
	SetDlgItemText(hDlg, ID_ETMULREAL, str);
	swprintf_s(str, TEXT("%f"), 55544.000000);
	SetDlgItemText(hDlg, ID_ETMULIMG, str);
	swprintf_s(str, TEXT("%f"), 0.442109);
	SetDlgItemText(hDlg, ID_ETDIVREAL, str);
	swprintf_s(str, TEXT("%f"), 0.123577);
	SetDlgItemText(hDlg, ID_ETDIVIMG, str);
}
