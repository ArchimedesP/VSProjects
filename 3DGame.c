#include <stdio.h>
#include <Windows.h>
#include <Math.h>

#include "Drawing.h"
#include "Util.h"


struct frameStruct frame = { 0 };

static BITMAPINFO frame_bitmapInfo;	// bitmap info
static HBITMAP frame_hBitmap = 0;	// bitmap handle
static HDC frame_dc = 0;	// frame device context

int keys[128] = { 0 };
int running = 1;


struct {
	double x;
	double y;
	double z;
	double angleX;
	double angleY;
	int fov;
	int speed;
	int turnSpeed;
} plr = { 0, 0, 0, 210, 0, 90, 3, 3 };


struct {
	int numPixels_x;
	int numPixels_y;
} render = { 100, 100 };

#define mX1 0
#define mY1 1
#define mZ1 2

#define mX2 3
#define mY2 4
#define mZ2 5

#define mX3 6
#define mY3 7
#define mZ3 8


#define map_polygonAmount 100
int map_vertices[map_polygonAmount][10] = { 0 };
// [polygon][x1, y1, z1, x2, y2, z2, x3, y3, z3]


int map_GetEnd() {

	int iMapEnd = 0;

	for (int p = 0; p < map_polygonAmount; p++) {

		int polygonExists = 0;

		for (int v = 0; v < 9; v++) {

			if (map_vertices[p][v] == 0) {
				continue;
			};

			polygonExists = 1;
			break;

		};

		if (!polygonExists) {

			iMapEnd = p;
			break;

		};

	};

	return iMapEnd;

};


void map_AddPolygon(int p, int x1, int y1, int z1, int x2, int y2, int z2, int x3, int y3, int z3, int color) {

	map_vertices[p][mX1] = x1;
	map_vertices[p][mY1] = y1;
	map_vertices[p][mZ1] = z1;

	map_vertices[p][mX2] = x2;
	map_vertices[p][mY2] = y2;
	map_vertices[p][mZ2] = z2;

	map_vertices[p][mX3] = x3;
	map_vertices[p][mY3] = y3;
	map_vertices[p][mZ3] = z3;

	map_vertices[p][9] = color;

	return;

};


				// index of polygon; use a constant num to keep switching it out, use map_GetEnd() to add new
int map_AddRect(int p, int x1, int y1, int z1, int width, int height, int angX, int angY, int angZ, int color) {



	map_AddPolygon(
		p,

		x1,
		y1,
		z1,

		x1 + width,
		y1 + 0,
		z1 + 0,

		x1 + 0,
		y1 + height,
		z1 + width,

		color
	);		


	return;

};

LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);


int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow) {


	wchar_t CLASS_NAME[] = L"Window Class";

	WNDCLASS wc = {  };
	wc.hInstance = hInstance;
	wc.lpfnWndProc = WindowProc;
	wc.lpszClassName = CLASS_NAME;

	RegisterClass(&wc);

	frame_bitmapInfo.bmiHeader.biSize = sizeof(frame_bitmapInfo.bmiHeader);
	frame_bitmapInfo.bmiHeader.biPlanes = 1;
	frame_bitmapInfo.bmiHeader.biBitCount = 32;
	frame_bitmapInfo.bmiHeader.biCompression = BI_RGB;
	frame_dc = CreateCompatibleDC(frame_dc);

	DWORD style = WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_CAPTION | WS_SYSMENU;

	RECT rect;
	rect.left = 250;
	rect.top = 150;
	rect.right = rect.left + 600;
	rect.bottom = rect.top + 500;

	HWND hWnd = CreateWindowEx(
		0,
		CLASS_NAME,
		L"Title",
		style,
		rect.left,
		rect.top,
		rect.right - rect.left,
		rect.bottom - rect.top,
		NULL,
		NULL,
		hInstance,
		NULL
	);

	ShowWindow(hWnd, nCmdShow);


	// create map

	map_AddRect(0, 300, 50, 200, 200, 100, 0, 0, 0, -1);

	//map_AddPolygon(map_GetEnd(), 100, 0, 100, 100, 30, 200, 220, 0, 120, -1);


	/*
	map_vertices[1][mX1] = 300;
	map_vertices[1][mY1] = 40;
	map_vertices[1][mZ1] = 300;

	map_vertices[1][mX2] = 500;
	map_vertices[1][mY2] = 40;
	map_vertices[1][mZ2] = 300;

	map_vertices[1][mX3] = 400;
	map_vertices[1][mY3] = 40;
	map_vertices[1][mZ3] = 400;
	*/

	//map_AddPolygon(300, 40, 300, 500, 40, 300, 400, 40, 400, -1);

	/*
	map_vertices[2][mX1] = 500;
	map_vertices[2][mY1] = 5;
	map_vertices[2][mZ1] = 100;

	map_vertices[2][mX2] = 500;
	map_vertices[2][mY2] = 10;
	map_vertices[2][mZ2] = 200;

	map_vertices[2][mX3] = 420;
	map_vertices[2][mY3] = 5;
	map_vertices[2][mZ3] = 320;
	*/

	int tempX = 0;
	int tempY = 0;
	int tempZ = 0;

	plr.x = frame.width / 2;
	plr.z = frame.height / 2;


	//int angleA = 0;

	MSG msg = { };

	while (running == 1) {

		PeekMessage(&msg, NULL, 0u, 0u, PM_REMOVE);

		TranslateMessage(&msg);
		DispatchMessage(&msg);

		for (int i = 0; i < 128; i++) {
			keys[i] = GetKeyState(i) / 10;
		};

		if (keys[27]) {	// esc
			SendMessage(hWnd, WM_CLOSE, NULL, NULL);
		};

		if (GetAsyncKeyState(VK_LEFT)) {
			plr.angleX += plr.turnSpeed;
		};

		if (GetAsyncKeyState(VK_RIGHT)) {
			plr.angleX -= plr.turnSpeed;
		};

		if (GetAsyncKeyState(VK_UP)) {
			plr.angleY += plr.turnSpeed;
		};

		if (GetAsyncKeyState(VK_DOWN)) {
			plr.angleY -= plr.turnSpeed;
		};

		if (plr.angleY < -90) {
			plr.angleY = -90;
		};

		if (plr.angleY > 90) {
			plr.angleY = 90;
		};

		if (keys[87]) {	// W
			plr.x += 1.0 * plr.speed * cos(plr.angleX * 3.14 / 180);
			plr.z += 1.0 * plr.speed * sin(plr.angleX * 3.14 / 180);
		};

		if (keys[83]) {	// S
			plr.x -= plr.speed * cos(plr.angleX * 3.14 / 180);
			plr.z -= plr.speed * sin(plr.angleX * 3.14 / 180);
		};

		if (keys[65]) {	// A
			plr.x += plr.speed * cos((plr.angleX + 90) * 3.14 / 180);
			plr.z += plr.speed * sin((plr.angleX + 90) * 3.14 / 180);
		};

		if (keys[68]) {	// D
			plr.x -= plr.speed * cos((plr.angleX + 90) * 3.14 / 180);
			plr.z -= plr.speed * sin((plr.angleX + 90) * 3.14 / 180);
		};


		if (keys[69]) {	// E
			plr.y += plr.speed;
		};

		if (keys[81]) { // Q
			plr.y -= plr.speed;
		};

		if (keys[88]) {	// X
			tempX += 2;
		};

		if (keys[89]) { // Y
			tempY += 2;
		};

		if (keys[90]) {	// Z
			tempZ += 2;
		};

		map_AddRect(0, 300, 50, 200, 200, 100, tempX, tempY, tempZ, -1);


		for (int i = 0; i < frame.width * frame.height; i++) {
			frame.pixels[i] = 0;
		};

		for (int i = 0; i < map_polygonAmount; i++) {

			if (!map_vertices[i][0]) {
				continue;
			};

			int x1 = map_vertices[i][mX1];
			int y1 = map_vertices[i][mY1];
			int z1 = map_vertices[i][mZ1];

			int x2 = map_vertices[i][mX2];
			int y2 = map_vertices[i][mY2];
			int z2 = map_vertices[i][mZ2];

			int x3 = map_vertices[i][mX3];
			int y3 = map_vertices[i][mY3];
			int z3 = map_vertices[i][mZ3];

			frame_DrawLine(&frame, x1, z1, x2, z2, 0x0000FF00);
			frame_DrawLine(&frame, x2, z2, x3, z3, 0x00FF0000);
			frame_DrawLine(&frame, x3, z3, x1, z1, 0x000000FF);

			frame_DrawCircleFilled(&frame, x1, z1, (y1 / 5 + 1), -1);
			frame_DrawCircleFilled(&frame, x2, z2, (y2 / 5 + 1), -1);
			frame_DrawCircleFilled(&frame, x3, z3, (y3 / 5 + 1), -1);

			frame_DrawLine(&frame, plr.x, plr.z, x1, z1, 0x00222222);
			frame_DrawLine(&frame, plr.x, plr.z, x2, z2, 0x00222222);
			frame_DrawLine(&frame, plr.x, plr.z, x3, z3, 0x00222222);

			frame_DrawText(&frame, x1, z1, "v0", 2, 2, -1);
			frame_DrawText(&frame, x2, z2, "v1", 2, 2, -1);
			frame_DrawText(&frame, x3, z3, "v2", 2, 2, -1);

		};


		double render_pixWidth = (frame.width) / plr.fov;
		double render_pixHeight = (frame.height) / plr.fov;



		for (int p = 0; p < map_polygonAmount; p++) {

			if (!map_vertices[p][0]) {
				continue;
			};

			int frame_x[3] = { 0 };
			int frame_y[3] = { 0 };

			int numOut = 0;


			for (int v = 0; v < 3; v++) {

				int deltaX = plr.x - map_vertices[p][v * 3];
				int deltaY = map_vertices[p][v * 3 + 1] - plr.y;
				int deltaZ = plr.z - map_vertices[p][v * 3 + 2];

				double angleX = DeltasToDegrees(deltaX, deltaZ);
				int distX = sqrt(deltaX * deltaX + deltaZ * deltaZ);
				double angleY = DeltasToDegrees(distX, deltaY);

				angleX -= plr.angleX;
				angleX = NormalizeAngle(angleX);

				if (angleX > 0) {
					angleX -= 180;
				};

				angleY -= plr.angleY;

				frame_x[v] = frame.width - (render_pixWidth * angleX + frame.width / 2);
				frame_y[v] = render_pixHeight * angleY + frame.height / 2;

				int angleOut_left = (angleX < plr.fov / -2);
				int angleOut_right = (angleX > plr.fov / 2);
				int angleOut_top = (angleY > plr.fov / 2);
				int angleOut_behind = (angleX < plr.fov * -1) || (angleX > plr.fov);

				int text_color = -1;

				if (angleOut_behind) {

					angleX += (180) * (angleOut_left)
						+(-180) * (angleOut_right);

					frame_x[v] = (render_pixWidth * angleX + frame.width / 2);

					angleY = (angleY - 180) * (angleY <= 0)
						+ (180 - angleY) * (angleY > 0);


					frame_y[v] = render_pixHeight * angleY + frame.height / 2;

					frame_DrawCircleFilled(&frame, 50, 130, 10, 0x0000FF00);
					text_color = 0x0000FF00;


				};

				// (debug) display angleOut flags
				if (angleOut_top) {
					frame_DrawCircleFilled(&frame, 50, 150, 10, 0x00FF0000);
					frame_DrawText(&frame, 50, 150, "Top", 2, -1, -1);
				};

				if (angleOut_behind) {
					frame_DrawCircleFilled(&frame, 50, 170, 10, 0x00FF0000);
					frame_DrawText(&frame, 50, 170, "Behind", 2, -1, -1);
				};

				if (angleOut_left) {
					frame_DrawCircleFilled(&frame, 50, 190, 10, 0x00FF0000);
					frame_DrawText(&frame, 50, 190, "Left", 2, -1, -1);
				};

				if (angleOut_right) {
					frame_DrawCircleFilled(&frame, 50, 210, 10, 0x00FF0000);
					frame_DrawText(&frame, 50, 210, "Right", 2, -1, -1);
				};

				if (angleOut_left + angleOut_right + angleOut_top) {
					frame_DrawCircleFilled(&frame, 50, 240, 10, 0x00FF00FF);
					frame_DrawText(&frame, 50, 240, "MarkedOut", 2, -1, -1);
					numOut++;
				};

				// (debug) display vertice stats
				char text[string_maxLength] = { 0 };

				text_AppendString(&text, "v:");
				text_AppendInt(&text, v);

				text_AppendString(&text, " aX:");
				text_AppendInt(&text, angleX);

				text_AppendString(&text, " aY:");
				text_AppendInt(&text, angleY);

				text_AppendString(&text, " dXYZ:");
				text_AppendInt(&text, deltaX);
				text_AppendString(&text, ", ");
				text_AppendInt(&text, deltaY);
				text_AppendString(&text, ", ");
				text_AppendInt(&text, deltaZ);

				text_AppendString(&text, " diX:");
				text_AppendInt(&text, distX);

				frame_DrawText(&frame, 15, frame.height - 20 * (v + 1), &text, 2, -1, text_color);

			};

			if (numOut == 3) {
				continue;
			};

			frame_DrawLine(&frame, frame_x[0], frame_y[0], frame_x[1], frame_y[1], -1);
			frame_DrawLine(&frame, frame_x[1], frame_y[1], frame_x[2], frame_y[2], -1);
			frame_DrawLine(&frame, frame_x[2], frame_y[2], frame_x[0], frame_y[0], -1);

			frame_DrawCircleFilled(&frame, frame_x[0], frame_y[0], 3, 0x000000FF);
			frame_DrawText(&frame, frame_x[0] + 20, frame_y[0] + 20, "v0", 2, -1, 0x000000FF);

			frame_DrawCircleFilled(&frame, frame_x[1], frame_y[1], 3, 0x00FF0000);
			frame_DrawText(&frame, frame_x[1] + 20, frame_y[1] + 20, "v1", 2, -1, 0x00FF0000);

			frame_DrawCircleFilled(&frame, frame_x[2], frame_y[2], 3, 0x000000FF);
			frame_DrawText(&frame, frame_x[2] + 20, frame_y[2] + 20, "v2", 2, -1, 0x000000FF);


		};


		// (debug) display map layout & player
		frame_DrawLineA(&frame, plr.x, plr.z, 300, plr.angleX + plr.fov / 2, 0x00333333);
		frame_DrawLineA(&frame, plr.x, plr.z, 300, plr.angleX - plr.fov / 2, 0x00333333);

		frame_DrawLineA(&frame, plr.x, plr.z, 10, plr.angleX + plr.fov / 2, 0x00F3C335);
		frame_DrawLineA(&frame, plr.x, plr.z, 10, plr.angleX - plr.fov / 2, 0x00F3C335);
		frame_DrawLineA(&frame, plr.x, plr.z, 500, plr.angleX, 0x00111111);

		frame_DrawLine(&frame, frame.width / 2 - 5, frame.height / 2, frame.width / 2 + 5, frame.height / 2, 0x00FF0000);
		frame_DrawLine(&frame, frame.width / 2, frame.height / 2 - 5, frame.width / 2, frame.height / 2 + 5, 0x00FF0000);


		// (debug) display stats
		char text[string_maxLength] = { 0 };

		text_AppendString(&text, "angleX:");
		text_AppendDouble(&text, plr.angleX);
		frame_DrawText(&frame, 10, 60, text, 2, 2, -1);

		text_ClearString(&text);
		text_AppendString(&text, "angleY:");
		text_AppendInt(&text, plr.angleY);
		frame_DrawText(&frame, 10, 40, text, 2, 2, -1);

		text_ClearString(&text);
		text_AppendString(&text, "pos:");
		text_AppendDouble(&text, plr.x);
		text_AppendString(&text, " ");
		text_AppendDouble(&text, plr.y);
		text_AppendString(&text, " ");
		text_AppendDouble(&text, plr.z);
		frame_DrawText(&frame, 10, 20, text, 2, 2, -1);




		InvalidateRect(hWnd, NULL, 0);
		UpdateWindow(hWnd);

		Sleep(20);


	};


	return 0;

}




LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {


	switch (uMsg) {

	case (WM_CLOSE): {
		DestroyWindow(hWnd);
		break;
	};

	case (WM_DESTROY): {
		PostQuitMessage(0);
		running = 0;
		break;
	};

	case (WM_SIZE): {

		frame_bitmapInfo.bmiHeader.biWidth = LOWORD(lParam);
		frame_bitmapInfo.bmiHeader.biHeight = HIWORD(lParam);

		if (frame_hBitmap) {
			DeleteObject(frame_hBitmap);
		};

		frame_hBitmap = CreateDIBSection(NULL, &frame_bitmapInfo, DIB_RGB_COLORS, &frame.pixels, 0, 0);

		SelectObject(frame_dc, frame_hBitmap);


		frame.width = LOWORD(lParam);
		frame.height = HIWORD(lParam);


	};

	case (WM_PAINT): {

		static PAINTSTRUCT ps;
		static HDC hdc;
		hdc = BeginPaint(hWnd, &ps);

		BitBlt(
			hdc,
			ps.rcPaint.left,
			ps.rcPaint.top,
			ps.rcPaint.right - ps.rcPaint.left,
			ps.rcPaint.bottom - ps.rcPaint.top,
			frame_dc,
			ps.rcPaint.left,
			ps.rcPaint.top,
			SRCCOPY
		);


		EndPaint(hWnd, &ps);


	};


	default:
		return DefWindowProc(hWnd, uMsg, wParam, lParam);

	};


};
