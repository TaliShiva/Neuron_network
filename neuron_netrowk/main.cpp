#include <windows.h>
#include <math.h>
#include <random>
#include <iostream>
#include <vector>
#include <cmath>
#include <ctime>
using namespace std;

#define pi 3.14

int function = 1;

double scale;
double scale2;

int ssize = 20;//кол-во нейронов

double mis;
int present = 10;
//int distribution = 1;
double mistake = 0.4;//велинична, на сколько максимально может отклонится точки от графика

double step = 0.0005;//шаг


inline double tan(double x)//функция активации
{
	return tanh(3.5*x);
}

inline double diff(double x)
{
	return 3.5*(1 - x * x);
}

inline double sinfunc(double x)
{
	return sin(2 * pi*x / scale)*x / scale;
}

inline double cube(double x){
	auto cock = (pow(x/ 120, 3) + x/scale2 + 5);
	return cock/scale2;
}


double cosfunc(double x)
{
	return cos(2 * pi*x / scale);
}


double parabola(double x)
{
	return pow(x / scale, 2.0);

}

double y;


double WhatFunction(int f, double scale, double i)
{
	if (f == 1)
		return scale * cosfunc(i);
	if (f == 2)
		return scale * sinfunc(i);
	if (f == 3)
		return scale * parabola(i);
	if (f == 4)
		return scale * cube(i);
}





LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
	PSTR szCmdLine, int iCmdShow)
{
	static TCHAR szAppName[] = TEXT("SineWave");
	HWND hwnd;
	MSG msg;

	WNDCLASSEX wndclass;
	wndclass.cbSize = sizeof(wndclass);
	wndclass.style = CS_HREDRAW | CS_VREDRAW;
	wndclass.lpfnWndProc = WndProc;
	wndclass.cbClsExtra = 0;
	wndclass.cbWndExtra = 0;
	wndclass.hInstance = hInstance;
	wndclass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndclass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wndclass.lpszMenuName = TEXT("MYMENU");
	wndclass.lpszClassName = (szAppName);
	wndclass.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
	RegisterClassEx(&wndclass);
	hwnd = CreateWindow(szAppName, TEXT("Нейронная сеть"),
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT,
		1000, 750,
		NULL, NULL, hInstance, NULL);
	ShowWindow(hwnd, iCmdShow);
	UpdateWindow(hwnd);
	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return msg.wParam;
}


LRESULT CALLBACK WndProc(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
	srand(time(NULL));
	static int cxClient, cyClient;
	HDC hdc;
	PAINTSTRUCT ps;
	int i, r;
	HPEN hPen, hPenOld;
	HBRUSH hBrush, hBrushOld;

	mt19937 gen(1529);

	uniform_real_distribution<> distr(-mistake, mistake);

	switch (iMsg)
	{

	case WM_CREATE:
		break;

	case WM_SIZE:
		cxClient = LOWORD(lParam);
		cyClient = HIWORD(lParam);
		break;

	case WM_PAINT:
	{

		double *xx = new double[ssize];
		double *yy = new double[ssize];

		hdc = BeginPaint(hwnd, &ps);

		SetMapMode(hdc, MM_ISOTROPIC);
		SetWindowExtEx(hdc, cxClient, cyClient, 0);
		SetViewportExtEx(hdc, cxClient / 2, -cyClient / 2, 0);
		SetViewportOrgEx(hdc, cxClient / 2, cyClient / 2, 0);

		MoveToEx(hdc, -cxClient, 0, NULL);
		LineTo(hdc, cxClient, 0);
		MoveToEx(hdc, 0, -cyClient, NULL);
		LineTo(hdc, 0, cyClient);

		scale = cxClient;
		scale2 = cyClient / 2;
		r = cxClient;

		hPen = CreatePen(PS_SOLID, 0, RGB(255, 0, 0));
		SelectObject(hdc, hPen);

		MoveToEx(hdc, -r, WhatFunction(function, scale2, -r), NULL);



		for (i = -r; i < r; i++)
		{
			y = WhatFunction(function, scale2, i);
			LineTo(hdc, i, (int)y);
		}


		for (int i = 0; i < ssize; i++)
		{
			xx[i] = rand() % r * 2 - r;

			mis = 0;

			if (rand() % 100 < present)
			{
				mis = distr(gen);

			}

			yy[i] = WhatFunction(function, scale2, xx[i]) + scale2 * mis;

			hPen = CreatePen(PS_SOLID, 0, RGB(0, 0, 255));
			hPenOld = (HPEN)SelectObject(hdc, hPen);

			hBrush = CreateSolidBrush(RGB(0, 0, 255));
			hBrushOld = (HBRUSH)SelectObject(hdc, hBrush);

			Ellipse(hdc, xx[i] - 4.0, yy[i] - 4.0, xx[i] + 4.0, yy[i] + 4.0);

			xx[i] /= scale;
			yy[i] /= scale2;

			SelectObject(hdc, hBrushOld);
			DeleteObject(hBrush);

			SelectObject(hdc, hPenOld);
			DeleteObject(hPen);

		}

		for (int t = 0; t < 100; t++)//эпохи
		{

			double* v = new double[ssize];//веса для дуг от входных нейронов
			double* w = new double[ssize];//веса для дуг к выходным нейронам
			double* v0 = new double[ssize];//веса нейронов смещения
			double *z = new double[ssize];//хранение промежуточных значений
			double* delta_v = new double[ssize];//ошибки для соответствующих дуг
			double* delta_w = new double[ssize];
			double* delta_v0 = new double[ssize];
			double w0;//нейрон смещения для выхода
			double delta;//ошибка
			double f;//выходное значение

			for (int i = 0; i < ssize; i++)
			{
				v[i] = (rand() % r * 2 - r) / scale;
				w[i] = (rand() % r * 2 - r) / scale;//назначения весов дуг графа случайными значениями
				v0[i] = (rand() % r * 2 - r) / scale;
			}
			w0 = (rand() % r * 2 - r) / scale;

			for (int u = 0; u < 10000; u++)//итерации для тренировки
			{
				for (int h = 0; h < ssize; h++)//для каждого элемента выборки
				{
					for (int i = 0; i < ssize; i++)
					{
						z[i] = xx[h] * v[i] + v0[i];//преобразование задаваемое нейронной сетью
						z[i] = tan(z[i]);
					}

					f = 0;
					for (int i = 0; i < ssize; i++)
						f += w[i] * z[i];//суммируем значения в выходном нейроне
					f += w0;
					f = tan(f);

					delta = (f - yy[h])*diff(f);

					w0 -= step	 * delta;//меня дугу для выходного нейрона смещения
					for (int i = 0; i < ssize; i++)
					{
						delta_w[i] = -step * delta*z[i];//считаем дельты для весов дуг между выходом и внутренним слоем
						delta_v[i] = -step * delta*w[i] * xx[h] * diff(z[i]);//между входом и внутренним слоем
						delta_v0[i] = -step * delta*w[i] * diff(z[i]);//для нейронов смещения
					}

					for (int i = 0; i < ssize; i++)
					{
						v[i] += delta_v[i];//изменение весов дуг
						w[i] += delta_w[i];
						v0[i] += delta_v0[i];
					}

				}
			}

			f = w0;
			for (int i = 0; i < ssize; i++)
			{
				z[i] = (-r / scale)*v[i] + v0[i];
				z[i] = tan(z[i]);
			}
			for (int i = 0; i < ssize; i++)
				f += z[i] * w[i];
			f = tan(f);
			f *= scale2;//поправка для отрисовки

			MoveToEx(hdc, -r, f, NULL);


			hPen = CreatePen(PS_SOLID, 0, RGB(30, 225, 0));
			SelectObject(hdc, hPen);

			for (int j = -r; j <= r; j++)//отрисовка
			{
				f = w0;
				for (int i = 0; i < ssize; i++)
				{
					z[i] = (j / scale)*v[i] + v0[i];
					z[i] = tan(z[i]);
				}
				for (int i = 0; i < ssize; i++)
					f += z[i] * w[i];
				f = tan(f);
				f *= scale2;
				LineTo(hdc, j, (int)f);
			}
		}
		return 0;
	}
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}
	return DefWindowProc(hwnd, iMsg, wParam, lParam);


}