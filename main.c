#include <windows.h>
#include <stdio.h>
#include <Commctrl.h>
#include <string.h>

#pragma comment(lib, "gdi32.lib")
#pragma comment(lib, "user32.lib")
#pragma comment(lib, "comctl32.lib")

#define GRID_SIZE 9
#define IDC_SOLVE 100
#define IDC_CLEAR 101
#define IDC_FIRST_EDIT 200

typedef struct {
    HWND hwnd;
    HWND editControls[GRID_SIZE][GRID_SIZE];
    BOOL wasEmpty[GRID_SIZE][GRID_SIZE];
    HBRUSH hBrush;
    BOOL solveRunSinceLastClear;
} SudokuApp;

SudokuApp app;

int find_empty(int grid[GRID_SIZE][GRID_SIZE], int *row, int *col) {
    for (*row = 0; *row < GRID_SIZE; (*row)++) {
        for (*col = 0; *col < GRID_SIZE; (*col)++) {
            if (grid[*row][*col] == 0) return 1;
        }
    }
    return 0;
}

int is_valid(int grid[GRID_SIZE][GRID_SIZE], int num, int row, int col) {
    for (int j = 0; j < GRID_SIZE; j++)
        if (grid[row][j] == num && j != col) return 0;
    for (int i = 0; i < GRID_SIZE; i++)
        if (grid[i][col] == num && i != row) return 0;
    int start_row = row - row % 3;
    int start_col = col - col % 3;
    for (int i = 0; i < 3; i++)
        for (int j = 0; j < 3; j++)
            if (grid[i + start_row][j + start_col] == num &&
                (i + start_row != row || j + start_col != col))
                return 0;
    return 1;
}


int solve_sudoku_recursive(int grid[GRID_SIZE][GRID_SIZE]) {
    int row, col;
    if (!find_empty(grid, &row, &col)) return 1;

    for (int num = 1; num <= 9; num++) {
        int temp_grid[GRID_SIZE][GRID_SIZE];
        memcpy(temp_grid, grid, sizeof(temp_grid));
        temp_grid[row][col] = num;

        if (is_valid(temp_grid, num, row, col)) {
             grid[row][col] = num;
             if (solve_sudoku_recursive(grid)) return 1;
             grid[row][col] = 0;
        }
    }
    return 0;
}

LRESULT CALLBACK EditSubclassProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData) {
    switch (uMsg) {
        case WM_CHAR: {
            if ((wParam >= '1' && wParam <= '9') || wParam == ' ') {
                 return DefSubclassProc(hWnd, uMsg, wParam, lParam);
            } else if (wParam == VK_BACK) {
                 return DefSubclassProc(hWnd, uMsg, wParam, lParam);
            } else {
                MessageBeep(0);
                return 0;
            }
        }

        case WM_KEYDOWN: {
            int id = GetDlgCtrlID(hWnd);
            if (id >= IDC_FIRST_EDIT && id < IDC_FIRST_EDIT + GRID_SIZE * GRID_SIZE) {
                int index = id - IDC_FIRST_EDIT;
                int row = index / GRID_SIZE;
                int col = index % GRID_SIZE;
                BOOL handled = FALSE;

                switch (wParam) {
                    case VK_UP:
                        if (row > 0) {
                            SetFocus(app.editControls[row - 1][col]);
                            handled = TRUE;
                        }
                        break;
                    case VK_DOWN:
                        if (row < GRID_SIZE - 1) {
                             SetFocus(app.editControls[row + 1][col]);
                             handled = TRUE;
                        }
                        break;
                    case VK_LEFT:
                        if (col > 0) {
                             SetFocus(app.editControls[row][col - 1]);
                             handled = TRUE;
                        }
                        break;
                    case VK_RIGHT:
                        if (col < GRID_SIZE - 1) {
                             SetFocus(app.editControls[row][col + 1]);
                             handled = TRUE;
                        }
                        break;

                    case VK_BACK: {
                        SetWindowText(hWnd, "");

                        int prevRow = row;
                        int prevCol = col;

                        if (col > 0) {
                            prevCol = col - 1;
                        } else if (row > 0) {
                            prevRow = row - 1;
                            prevCol = GRID_SIZE - 1;
                        } else {
                            return 0;
                        }

                        HWND prevFocus = app.editControls[prevRow][prevCol];
                        SetFocus(prevFocus);
                        SendMessage(prevFocus, EM_SETSEL, 0, -1);

                        handled = TRUE;
                        break;
                    }

                }

                if (handled) {
                    if (wParam != VK_BACK) {
                         HWND currentFocus = GetFocus();
                         if (currentFocus) {
                             SendMessage(currentFocus, EM_SETSEL, 0, -1);
                         }
                    }
                    return 0;
                }
            }
            break;
        }

        case WM_GETDLGCODE: {
            LRESULT lResult = DefSubclassProc(hWnd, uMsg, wParam, lParam);
            lResult &= ~DLGC_WANTARROWS;
            return lResult;
        }

        default:
            break;
    }
    return DefSubclassProc(hWnd, uMsg, wParam, lParam);
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
        case WM_CREATE: {
            app.hwnd = hwnd;

            HFONT hFont = CreateFont(
                20, 0, 0, 0, FW_BOLD,
                FALSE, FALSE, FALSE,
                DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
                DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, "Arial"
            );

            for (int i = 0; i < GRID_SIZE; i++) {
                for (int j = 0; j < GRID_SIZE; j++) {
                    int x = 30 + j * 30 + (j / 3) * 5;
                    int y = 30 + i * 30 + (i / 3) * 5;
                    app.editControls[i][j] = CreateWindow(
                        "EDIT", "",
                        WS_CHILD | WS_VISIBLE | WS_BORDER | ES_CENTER | ES_NUMBER | ES_AUTOHSCROLL, // Added ES_NUMBER
                        x, y, 25, 25,
                        hwnd, (HMENU)(IDC_FIRST_EDIT + i * GRID_SIZE + j),
                        ((LPCREATESTRUCT)lParam)->hInstance, NULL
                    );
                    if (hFont) {
                        SendMessage(app.editControls[i][j], WM_SETFONT, (WPARAM)hFont, TRUE);
                    }
                    SendMessage(app.editControls[i][j], EM_SETLIMITTEXT, 1, 0);
                    SetWindowSubclass(app.editControls[i][j], EditSubclassProc, 0, 0);
                    app.wasEmpty[i][j] = TRUE;
                }
            }

            CreateWindow("BUTTON", "Solve", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
                        30, 340, 80, 30, hwnd, (HMENU)IDC_SOLVE,
                        ((LPCREATESTRUCT)lParam)->hInstance, NULL);
            CreateWindow("BUTTON", "Clear", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
                        120, 340, 80, 30, hwnd, (HMENU)IDC_CLEAR,
                        ((LPCREATESTRUCT)lParam)->hInstance, NULL);

            app.hBrush = CreateSolidBrush(RGB(255, 255, 255)); // White

            app.solveRunSinceLastClear = FALSE;
            break;
        }

        case WM_COMMAND: {
            int wmId = LOWORD(wParam);
            int wmEvent = HIWORD(wParam);

            if (wmEvent == EN_UPDATE && wmId >= IDC_FIRST_EDIT &&
                wmId < IDC_FIRST_EDIT + GRID_SIZE * GRID_SIZE) {
                HWND currentEdit = (HWND)lParam;
                char buffer[2];
                GetWindowText(currentEdit, buffer, 2);

                int id = wmId - IDC_FIRST_EDIT;
                int row = id / GRID_SIZE;
                int col = id % GRID_SIZE;

                if (buffer[0] == ' ') {
                    SetWindowText(currentEdit, "");
                    int nextCol = (col + 1) % GRID_SIZE;
                    int nextRow = (nextCol == 0) ? (row + 1) % GRID_SIZE : row;
                    if (nextRow != row || nextCol != col) {
                         HWND nextFocus = app.editControls[nextRow][nextCol];
                         SetFocus(nextFocus);
                         SendMessage(nextFocus, EM_SETSEL, 0, -1);
                    }
                } else if (buffer[0] >= '1' && buffer[0] <= '9') {
                    int nextCol = (col + 1) % GRID_SIZE;
                    int nextRow = (nextCol == 0) ? (row + 1) % GRID_SIZE : row;
                     if (nextRow != row || nextCol != col) {
                         HWND nextFocus = app.editControls[nextRow][nextCol];
                         SetFocus(nextFocus);
                         SendMessage(nextFocus, EM_SETSEL, 0, -1);
                     }
                     InvalidateRect(currentEdit, NULL, TRUE);
                     UpdateWindow(currentEdit);
                } else if (buffer[0] == '\0') {
                     InvalidateRect(currentEdit, NULL, TRUE);
                     UpdateWindow(currentEdit);
                }
            }

            switch (wmId) {
                case IDC_SOLVE: {
                    int grid[GRID_SIZE][GRID_SIZE] = {0};
                    char buffer[2];
                    BOOL initialGridValid = TRUE;
                    int tempGridForValidation[GRID_SIZE][GRID_SIZE] = {0};

                    for (int i = 0; i < GRID_SIZE; i++) {
                        for (int j = 0; j < GRID_SIZE; j++) {
                            GetWindowText(app.editControls[i][j], buffer, 2);
                            if (buffer[0] >= '1' && buffer[0] <= '9') {
                                int num = atoi(buffer);
                                grid[i][j] = num;
                                tempGridForValidation[i][j] = num;
                                app.wasEmpty[i][j] = FALSE;

                                if (!is_valid(tempGridForValidation, num, i, j)) {
                                    initialGridValid = FALSE;
                                }
                            } else {
                                grid[i][j] = 0;
                                tempGridForValidation[i][j] = 0;
                                app.wasEmpty[i][j] = TRUE;
                            }
                        }
                    }

                    if (!initialGridValid) {
                         MessageBox(hwnd, "The initial puzzle contains conflicting numbers.",
                                 "Invalid Input", MB_OK | MB_ICONWARNING);
                         break;
                    }

                    BOOL solved = solve_sudoku_recursive(grid);

                    app.solveRunSinceLastClear = TRUE;

                    if (solved) {
                        for (int i = 0; i < GRID_SIZE; i++) {
                            for (int j = 0; j < GRID_SIZE; j++) {
                                char numStr[2] = {grid[i][j] + '0', '\0'};
                                char currentText[2];
                                GetWindowText(app.editControls[i][j], currentText, 2);
                                if (strcmp(currentText, numStr) != 0) {
                                    SetWindowText(app.editControls[i][j], numStr);
                                }
                                InvalidateRect(app.editControls[i][j], NULL, TRUE);
                            }
                        }
                         UpdateWindow(hwnd);
                    } else {
                        MessageBox(hwnd, "No solution exists for this puzzle configuration.",
                                 "Solver Result", MB_OK | MB_ICONINFORMATION);
                         for (int i = 0; i < GRID_SIZE; i++) {
                             for (int j = 0; j < GRID_SIZE; j++) {
                                 InvalidateRect(app.editControls[i][j], NULL, TRUE);
                             }
                         }
                         UpdateWindow(hwnd);
                    }
                    break;
                }

                case IDC_CLEAR: {
                    app.solveRunSinceLastClear = FALSE;

                    for (int i = 0; i < GRID_SIZE; i++) {
                        for (int j = 0; j < GRID_SIZE; j++) {
                            SetWindowText(app.editControls[i][j], "");
                            app.wasEmpty[i][j] = TRUE;
                            InvalidateRect(app.editControls[i][j], NULL, TRUE);
                        }
                    }
                     UpdateWindow(hwnd);
                     SetFocus(app.editControls[0][0]);
                    break;
                }
            }
            break;
        }

        case WM_CTLCOLOREDIT: {
            HDC hdc = (HDC)wParam;
            HWND hEdit = (HWND)lParam;
            int id = GetDlgCtrlID(hEdit);

            if (id >= IDC_FIRST_EDIT && id < IDC_FIRST_EDIT + GRID_SIZE * GRID_SIZE) {
                int index = id - IDC_FIRST_EDIT;
                int i = index / GRID_SIZE;
                int j = index % GRID_SIZE;

                COLORREF textColor = RGB(0, 0, 0);
                int textLen = GetWindowTextLength(hEdit);

                if (app.solveRunSinceLastClear && app.wasEmpty[i][j] && textLen > 0) {
                    textColor = RGB(0, 0, 255);
                }

                SetTextColor(hdc, textColor);
                SetBkColor(hdc, RGB(255, 255, 255));
                return (LRESULT)app.hBrush;
            } else {
                return DefWindowProc(hwnd, msg, wParam, lParam);
            }
        }


        case WM_DESTROY: {
             HFONT hFont = (HFONT)SendMessage(app.editControls[0][0], WM_GETFONT, 0, 0);
             if (hFont) {
                 DeleteObject(hFont);
             }
            DeleteObject(app.hBrush);

            for (int i = 0; i < GRID_SIZE; i++) {
                for (int j = 0; j < GRID_SIZE; j++) {
                    if (app.editControls[i][j]) {
                       RemoveWindowSubclass(app.editControls[i][j], EditSubclassProc, 0);
                    }
                }
            }
            PostQuitMessage(0);
            break;
        }

        default:
            return DefWindowProc(hwnd, msg, wParam, lParam);
    }
    return 0;
}


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
                   LPSTR lpCmdLine, int nCmdShow) {
    INITCOMMONCONTROLSEX icex;
    icex.dwSize = sizeof(INITCOMMONCONTROLSEX);
    icex.dwICC = ICC_STANDARD_CLASSES;
    InitCommonControlsEx(&icex);

    WNDCLASS wc = {0};
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = "SudokuSolverClass";
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.style = CS_HREDRAW | CS_VREDRAW;

    if (!RegisterClass(&wc)) {
        MessageBox(NULL, "Window Registration Failed!", "Error", MB_ICONEXCLAMATION | MB_OK);
        return 1;
    }

    int screenWidth = GetSystemMetrics(SM_CXSCREEN);
    int screenHeight = GetSystemMetrics(SM_CYSCREEN);
    int windowWidth = 340;
    int windowHeight = 400;
    int x = (screenWidth - windowWidth) / 2;
    int y = (screenHeight - windowHeight) / 2;

    DWORD dwStyle = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX;
    RECT wr = {0, 0, windowWidth, windowHeight};
    AdjustWindowRect(&wr, dwStyle, FALSE);

    app.hwnd = CreateWindowEx(
        0,
        "SudokuSolverClass",
        "Sudoku Solver",
        dwStyle,
        x, y,
        wr.right - wr.left, wr.bottom - wr.top,
        NULL, NULL, hInstance, NULL
    );

    if (!app.hwnd) {
        MessageBox(NULL, "Window Creation Failed!", "Error", MB_ICONEXCLAMATION | MB_OK);
        return 2;
    }

    ShowWindow(app.hwnd, nCmdShow);
    UpdateWindow(app.hwnd);

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0) > 0) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return (int)msg.wParam;
}
