#include <Windows.h>
#include <CommCtrl.h>
#include <string>
#include "resource.h"
#include "MemoryManager.h"
#pragma comment(lib, "comctl32.lib")

// ─── Colour palette ──────────────────────────────────────────────────────────
#define CLR_BG        RGB(18,  18,  22)
#define CLR_PANEL     RGB(26,  26,  32)
#define CLR_ROW_ALT   RGB(23,  23,  28)
#define CLR_BTN       RGB(42,  42,  54)
#define CLR_EDIT      RGB(30,  30,  38)
#define CLR_ACCENT    RGB(212, 160, 23)   // gold
#define CLR_ACCENT2   RGB(155, 110,  8)   // darker gold
#define CLR_DIVIDER   RGB(55,  55,  70)
#define CLR_TEXT      RGB(220, 220, 230)
#define CLR_SUCCESS   RGB(80,  200, 120)
#define CLR_ERROR     RGB(220,  80,  80)
#define WIN_W 920
#define WIN_H 680

// ─── Globals ─────────────────────────────────────────────────────────────────
HINSTANCE g_hInst;
HWND g_hMainWnd, hStatusBar, hBtnConnect;
HWND hWoodEdit, hGoldEdit, hIronEdit, hSkillEdit, hArcaneEdit, hPrimalEdit, hDivineEdit, hUnholyEdit;
HWND hWoodBtn, hGoldBtn, hIronBtn, hSkillBtn, hArcaneBtn, hPrimalBtn, hDivineBtn, hUnholyBtn;
HWND hFrzWood, hFrzGold, hFrzIron, hFrzArcane, hFrzPrimal, hFrzDivine, hFrzUnholy;
HWND hGodMode, hInfHealth, hInfMana, hOneHit, hMaxXP;
MemoryManager* g_memMgr = nullptr;
bool g_connected = false;
bool g_features[5] = {false};
COLORREF g_statusColor = CLR_TEXT;

HBRUSH g_hBrushBg, g_hBrushPanel, g_hBrushBtn, g_hBrushEdit, g_hBrushAccent, g_hBrushRowAlt;
HFONT g_hFontTitle, g_hFontLabel, g_hFontBtn, g_hFontSmall;

// ─── Forward declarations ─────────────────────────────────────────────────────
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
void CreateModernUI(HWND hwnd);
void ConnectToGame();
void ApplyResource(int type, const std::wstring& val);
void UpdateStatus(const std::wstring& msg, COLORREF clr);
void DrawGradientRect(HDC hdc, RECT* rc, COLORREF c1, COLORREF c2, bool vertical);
void DrawOwnerButton(LPDRAWITEMSTRUCT dis, bool isAccent);

// ─── WinMain ─────────────────────────────────────────────────────────────────
int WINAPI WinMain(_In_ HINSTANCE hInst, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int nShow)
{
    g_hInst = hInst;
    INITCOMMONCONTROLSEX icc = {sizeof(INITCOMMONCONTROLSEX), ICC_WIN95_CLASSES};
    InitCommonControlsEx(&icc);

    g_hBrushBg     = CreateSolidBrush(CLR_BG);
    g_hBrushPanel  = CreateSolidBrush(CLR_PANEL);
    g_hBrushBtn    = CreateSolidBrush(CLR_BTN);
    g_hBrushEdit   = CreateSolidBrush(CLR_EDIT);
    g_hBrushAccent = CreateSolidBrush(CLR_ACCENT);
    g_hBrushRowAlt = CreateSolidBrush(CLR_ROW_ALT);

    g_hFontTitle = CreateFontW(30, 0, 0, 0, FW_BOLD,     0, 0, 0, DEFAULT_CHARSET, 0, 0, CLEARTYPE_QUALITY, 0, L"Segoe UI");
    g_hFontLabel = CreateFontW(15, 0, 0, 0, FW_MEDIUM,   0, 0, 0, DEFAULT_CHARSET, 0, 0, CLEARTYPE_QUALITY, 0, L"Segoe UI");
    g_hFontBtn   = CreateFontW(14, 0, 0, 0, FW_SEMIBOLD, 0, 0, 0, DEFAULT_CHARSET, 0, 0, CLEARTYPE_QUALITY, 0, L"Segoe UI");
    g_hFontSmall = CreateFontW(12, 0, 0, 0, FW_NORMAL,   0, 0, 0, DEFAULT_CHARSET, 0, 0, CLEARTYPE_QUALITY, 0, L"Segoe UI");

    WNDCLASSEXW wc = {sizeof(WNDCLASSEXW)};
    wc.lpfnWndProc   = WndProc;
    wc.hInstance     = hInst;
    wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = g_hBrushBg;
    wc.lpszClassName = L"DisciplesDominationTrainer";
    if (!RegisterClassExW(&wc)) return 0;

    g_hMainWnd = CreateWindowExW(0, L"DisciplesDominationTrainer",
        L"DISCIPLES: DOMINATION — Enhancement Suite v2.1",
        WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
        CW_USEDEFAULT, CW_USEDEFAULT, WIN_W, WIN_H, NULL, NULL, hInst, NULL);

    if (!g_hMainWnd) return 0;
    ShowWindow(g_hMainWnd, nShow);
    UpdateWindow(g_hMainWnd);

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    DeleteObject(g_hBrushBg);
    DeleteObject(g_hBrushPanel);
    DeleteObject(g_hBrushBtn);
    DeleteObject(g_hBrushEdit);
    DeleteObject(g_hBrushAccent);
    DeleteObject(g_hBrushRowAlt);
    DeleteObject(g_hFontTitle);
    DeleteObject(g_hFontLabel);
    DeleteObject(g_hFontBtn);
    DeleteObject(g_hFontSmall);
    if (g_memMgr) delete g_memMgr;
    return (int)msg.wParam;
}

// ─── Window procedure ─────────────────────────────────────────────────────────
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp)
{
    switch (msg) {
    case WM_CREATE:
        CreateModernUI(hwnd);
        g_memMgr = new MemoryManager();
        break;

    case WM_PAINT: {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);
        RECT rc;

        // Header: vertical gradient (top → bottom)
        SetRect(&rc, 0, 0, WIN_W, 85);
        DrawGradientRect(hdc, &rc, RGB(28, 28, 38), RGB(18, 18, 24), true);

        // Top accent bar (3 px gold stripe at very top)
        HBRUSH hAccentBrush = CreateSolidBrush(CLR_ACCENT);
        SetRect(&rc, 0, 0, WIN_W, 3);
        FillRect(hdc, &rc, hAccentBrush);
        DeleteObject(hAccentBrush);

        // Bottom header divider
        HPEN hPen = CreatePen(PS_SOLID, 1, CLR_DIVIDER);
        SelectObject(hdc, hPen);
        MoveToEx(hdc, 0, 85, NULL);
        LineTo(hdc, WIN_W, 85);
        DeleteObject(hPen);

        // Status bar background strip
        SetRect(&rc, 0, WIN_H - 50, WIN_W, WIN_H);
        FillRect(hdc, &rc, g_hBrushPanel);
        HPEN hPen2 = CreatePen(PS_SOLID, 1, CLR_DIVIDER);
        SelectObject(hdc, hPen2);
        MoveToEx(hdc, 0, WIN_H - 50, NULL);
        LineTo(hdc, WIN_W, WIN_H - 50);
        DeleteObject(hPen2);

        // Status indicator dot
        HBRUSH hDotBrush = CreateSolidBrush(g_statusColor);
        SetRect(&rc, 20, WIN_H - 33, 32, WIN_H - 21);
        FillRect(hdc, &rc, hDotBrush);
        DeleteObject(hDotBrush);

        EndPaint(hwnd, &ps);
        return 0;
    }

    case WM_CTLCOLORSTATIC: {
        HDC hdcCtl = (HDC)wp;
        HWND hCtl  = (HWND)lp;
        // Alternating stripe statics get a slightly different background
        if (GetWindowLongPtrW(hCtl, GWLP_USERDATA) == 1) {
            SetBkColor(hdcCtl, CLR_ROW_ALT);
            return (LRESULT)g_hBrushRowAlt;
        }
        SetTextColor(hdcCtl, CLR_TEXT);
        SetBkColor(hdcCtl, CLR_BG);
        return (LRESULT)g_hBrushBg;
    }
    case WM_CTLCOLORBTN: {
        HDC hdcCtl = (HDC)wp;
        SetTextColor(hdcCtl, CLR_TEXT);
        SetBkColor(hdcCtl, CLR_BG);
        return (LRESULT)g_hBrushBg;
    }

    case WM_CTLCOLOREDIT: {
        HDC hdcEdit = (HDC)wp;
        SetTextColor(hdcEdit, RGB(255, 255, 255));
        SetBkColor(hdcEdit, CLR_EDIT);
        return (LRESULT)g_hBrushEdit;
    }

    case WM_DRAWITEM: {
        LPDRAWITEMSTRUCT dis = (LPDRAWITEMSTRUCT)lp;
        if (dis->CtlType == ODT_BUTTON) {
            bool isConnect = (dis->CtlID == 5001);
            DrawOwnerButton(dis, isConnect);
            return TRUE;
        }
        break;
    }

    case WM_COMMAND:
        if (LOWORD(wp) == 5001) ConnectToGame();
        else if (LOWORD(wp) >= 6001 && LOWORD(wp) <= 6008) {
            if (!g_connected) { UpdateStatus(L"Please connect to game first", CLR_ERROR); return 0; }
            wchar_t txt[256];
            int idx = LOWORD(wp) - 6001;
            HWND eds[] = {hWoodEdit, hGoldEdit, hIronEdit, hSkillEdit, hArcaneEdit, hPrimalEdit, hDivineEdit, hUnholyEdit};
            if (idx < 8) { GetWindowTextW(eds[idx], txt, 256); ApplyResource(idx, txt); }
        }
        else if (LOWORD(wp) >= 7001 && LOWORD(wp) <= 7007)
            g_memMgr->SetFreeze(LOWORD(wp) - 7001, SendMessage((HWND)lp, BM_GETCHECK, 0, 0) == BST_CHECKED);
        else if (LOWORD(wp) >= 8001 && LOWORD(wp) <= 8005) {
            int feat = LOWORD(wp) - 8001;
            g_features[feat] = (SendMessage((HWND)lp, BM_GETCHECK, 0, 0) == BST_CHECKED);
            if (!g_connected) return 0;
            switch (feat) {
                case 0: g_memMgr->SetGodMode(g_features[0]); break;
                case 1: g_memMgr->SetInfiniteHealth(g_features[1]); break;
                case 2: g_memMgr->SetInfiniteMana(g_features[2]); break;
                case 3: g_memMgr->SetOneHitKill(g_features[3]); break;
                case 4: g_memMgr->SetMaxExperience(g_features[4]); break;
            }
            UpdateStatus(g_features[feat] ? L"Feature enabled" : L"Feature disabled",
                g_features[feat] ? CLR_SUCCESS : CLR_TEXT);
        }
        break;

    case WM_TIMER:
        if (g_connected && g_memMgr) g_memMgr->Update();
        break;

    case WM_DESTROY:
        KillTimer(hwnd, 1);
        PostQuitMessage(0);
        break;

    default:
        return DefWindowProc(hwnd, msg, wp, lp);
    }
    return 0;
}

// ─── UI construction ──────────────────────────────────────────────────────────
void CreateModernUI(HWND hwnd)
{
    // === HEADER ===
    HWND hTitle = CreateWindowW(L"STATIC", L"DISCIPLES: DOMINATION",
        WS_CHILD | WS_VISIBLE | SS_CENTER,
        0, 14, WIN_W, 36, hwnd, NULL, g_hInst, NULL);
    SendMessage(hTitle, WM_SETFONT, (WPARAM)g_hFontTitle, TRUE);

    HWND hSubtitle = CreateWindowW(L"STATIC", L"Enhancement Suite  v2.1",
        WS_CHILD | WS_VISIBLE | SS_CENTER,
        0, 52, WIN_W, 20, hwnd, NULL, g_hInst, NULL);
    SendMessage(hSubtitle, WM_SETFONT, (WPARAM)g_hFontSmall, TRUE);

    // === CONNECT BUTTON (owner-draw → gold style) ===
    hBtnConnect = CreateWindowW(L"BUTTON", L"CONNECT TO GAME",
        WS_CHILD | WS_VISIBLE | BS_OWNERDRAW,
        310, 92, 300, 40, hwnd, (HMENU)(INT_PTR)5001, g_hInst, NULL);
    SendMessage(hBtnConnect, WM_SETFONT, (WPARAM)g_hFontLabel, TRUE);

    // === RESOURCES GROUP ===
    HWND hGrpRes = CreateWindowW(L"BUTTON", L"  RESOURCES  ",
        WS_CHILD | WS_VISIBLE | BS_GROUPBOX,
        15, 148, WIN_W - 30, 285, hwnd, NULL, g_hInst, NULL);
    SendMessage(hGrpRes, WM_SETFONT, (WPARAM)g_hFontLabel, TRUE);

    const wchar_t* resLabels[] = {
        L"Wood:", L"Gold:", L"Iron:", L"Skill Points:",
        L"Arcane Flux:", L"Primal Essence:", L"Divine Essence:", L"Unholy Essence:"
    };

    // Left column: label + edit + SET + Lock
    const int labelX  = 40,  labelW  = 160;
    const int editX   = 210, editW   = 140;
    const int btnX    = 358, btnW    = 82;
    const int lockX   = 450, lockW   = 72;

    int y          = 174;
    const int rowH = 33;

    HWND resFrz[] = {hFrzWood, hFrzGold, hFrzIron, hFrzArcane, hFrzPrimal, hFrzDivine, hFrzUnholy};

    for (int i = 0; i < 8; i++) {
        // Subtle alternating row stripe (drawn as child static with background colour)
        if (i % 2 == 1) {
            HWND hStripe = CreateWindowW(L"STATIC", L"",
                WS_CHILD | WS_VISIBLE | SS_LEFT,
                16, y - 1, WIN_W - 32, rowH, hwnd, NULL, g_hInst, NULL);
            SetWindowLongPtrW(hStripe, GWLP_USERDATA, 1); // mark as stripe
            // Override background: handled by WM_CTLCOLORSTATIC returning g_hBrushRowAlt
            (void)hStripe;
        }

        HWND lbl = CreateWindowW(L"STATIC", resLabels[i],
            WS_CHILD | WS_VISIBLE | SS_RIGHT,
            labelX, y + 8, labelW, 18, hwnd, NULL, g_hInst, NULL);
        SendMessage(lbl, WM_SETFONT, (WPARAM)g_hFontLabel, TRUE);

        HWND edt = CreateWindowExW(WS_EX_CLIENTEDGE, L"EDIT", L"99999",
            WS_CHILD | WS_VISIBLE | ES_NUMBER | ES_CENTER,
            editX, y + 3, editW, 27, hwnd, NULL, g_hInst, NULL);
        SendMessage(edt, WM_SETFONT, (WPARAM)g_hFontLabel, TRUE);

        // Owner-draw SET button
        HWND btn = CreateWindowW(L"BUTTON", L"SET",
            WS_CHILD | WS_VISIBLE | BS_OWNERDRAW,
            btnX, y + 3, btnW, 27, hwnd, (HMENU)(INT_PTR)(6001 + i), g_hInst, NULL);
        SendMessage(btn, WM_SETFONT, (WPARAM)g_hFontBtn, TRUE);

        if (i < 7) {
            HWND frz = CreateWindowW(L"BUTTON", L"Lock",
                WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX,
                lockX, y + 8, lockW, 18, hwnd, (HMENU)(INT_PTR)(7001 + i), g_hInst, NULL);
            SendMessage(frz, WM_SETFONT, (WPARAM)g_hFontSmall, TRUE);
            resFrz[i] = frz;
        }

        switch (i) {
            case 0: hWoodEdit   = edt; hWoodBtn   = btn; hFrzWood   = resFrz[i]; break;
            case 1: hGoldEdit   = edt; hGoldBtn   = btn; hFrzGold   = resFrz[i]; break;
            case 2: hIronEdit   = edt; hIronBtn   = btn; hFrzIron   = resFrz[i]; break;
            case 3: hSkillEdit  = edt; hSkillBtn  = btn; break;
            case 4: hArcaneEdit = edt; hArcaneBtn = btn; hFrzArcane = resFrz[i]; break;
            case 5: hPrimalEdit = edt; hPrimalBtn = btn; hFrzPrimal = resFrz[i]; break;
            case 6: hDivineEdit = edt; hDivineBtn = btn; hFrzDivine = resFrz[i]; break;
            case 7: hUnholyEdit = edt; hUnholyBtn = btn; break;
        }

        y += rowH;
    }

    // === COMBAT & EXPERIENCE GROUP ===
    // Two-column layout: 3 items left | 2 items right
    HWND hGrpCombat = CreateWindowW(L"BUTTON", L"  COMBAT & EXPERIENCE  ",
        WS_CHILD | WS_VISIBLE | BS_GROUPBOX,
        15, 448, WIN_W - 30, 140, hwnd, NULL, g_hInst, NULL);
    SendMessage(hGrpCombat, WM_SETFONT, (WPARAM)g_hFontLabel, TRUE);

    const wchar_t* featLabels[] = {
        L"God Mode (Invincibility)",
        L"Infinite Squad Health",
        L"Unlimited Mana / Energy",
        L"One-Hit Kill",
        L"Maximum Experience Multiplier"
    };
    HWND* featHwnds[] = {&hGodMode, &hInfHealth, &hInfMana, &hOneHit, &hMaxXP};

    // Column positions
    const int colW       = (WIN_W - 30) / 2;
    const int col1X      = 40;
    const int col2X      = col1X + colW;
    const int featStartY = 475;
    const int featRowH   = 32;

    for (int i = 0; i < 5; i++) {
        int col = (i < 3) ? 0 : 1;
        int row = (i < 3) ? i : (i - 3);
        int fx  = (col == 0) ? col1X : col2X;
        int fy  = featStartY + row * featRowH;

        HWND chk = CreateWindowW(L"BUTTON", featLabels[i],
            WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX,
            fx, fy, colW - 20, 24, hwnd, (HMENU)(INT_PTR)(8001 + i), g_hInst, NULL);
        SendMessage(chk, WM_SETFONT, (WPARAM)g_hFontLabel, TRUE);
        *featHwnds[i] = chk;
    }

    // === STATUS BAR ===
    hStatusBar = CreateWindowW(L"STATIC", L"System ready — Waiting for connection...",
        WS_CHILD | WS_VISIBLE | SS_LEFT,
        42, WIN_H - 38, WIN_W - 58, 22, hwnd, NULL, g_hInst, NULL);
    SendMessage(hStatusBar, WM_SETFONT, (WPARAM)g_hFontSmall, TRUE);
}

// ─── ConnectToGame ────────────────────────────────────────────────────────────
void ConnectToGame()
{
    const wchar_t* names[] = {
        L"Disciples_Domination.exe", L"DisciplesDomination.exe",
        L"Disciples.exe", L"Domination.exe"
    };

    for (int i = 0; i < 4; i++) {
        if (g_memMgr->AttachToProcess(names[i])) {
            g_connected = true;
            UpdateStatus(L"Connected to Disciples: Domination", CLR_SUCCESS);
            EnableWindow(hBtnConnect, FALSE);
            SetTimer(g_hMainWnd, 1, 100, NULL);
            return;
        }
    }
    UpdateStatus(L"Game not found — please launch Disciples: Domination first", CLR_ERROR);
}

// ─── ApplyResource ────────────────────────────────────────────────────────────
void ApplyResource(int type, const std::wstring& val)
{
    int v = _wtoi(val.c_str());
    if (g_memMgr->SetResourceValue(type, v)) {
        const wchar_t* names[] = {L"Wood", L"Gold", L"Iron", L"Skills", L"Arcane", L"Primal", L"Divine", L"Unholy"};
        std::wstring m = std::wstring(names[type]) + L" set to " + val;
        UpdateStatus(m, CLR_SUCCESS);
    } else {
        UpdateStatus(L"Failed to write memory — check game state", CLR_ERROR);
    }
}

// ─── UpdateStatus ─────────────────────────────────────────────────────────────
void UpdateStatus(const std::wstring& msg, COLORREF clr)
{
    g_statusColor = clr;
    SetWindowTextW(hStatusBar, msg.c_str());
    // Repaint the status strip (dot + text area)
    RECT rc = {0, WIN_H - 50, WIN_W, WIN_H};
    InvalidateRect(g_hMainWnd, &rc, FALSE);
    InvalidateRect(hStatusBar, NULL, TRUE);
}

// ─── DrawGradientRect ─────────────────────────────────────────────────────────
void DrawGradientRect(HDC hdc, RECT* rc, COLORREF c1, COLORREF c2, bool vertical)
{
    int r1 = GetRValue(c1), g1 = GetGValue(c1), b1 = GetBValue(c1);
    int r2 = GetRValue(c2), g2 = GetGValue(c2), b2 = GetBValue(c2);
    int steps = vertical ? (rc->bottom - rc->top) : (rc->right - rc->left);

    for (int i = 0; i < steps; i++) {
        int r = r1 + (r2 - r1) * i / steps;
        int g = g1 + (g2 - g1) * i / steps;
        int b = b1 + (b2 - b1) * i / steps;
        HBRUSH hBrush = CreateSolidBrush(RGB(r, g, b));
        RECT fillRect = *rc;
        if (vertical) {
            fillRect.top    = rc->top + i;
            fillRect.bottom = fillRect.top + 1;
        } else {
            fillRect.left  = rc->left + i;
            fillRect.right = fillRect.left + 1;
        }
        FillRect(hdc, &fillRect, hBrush);
        DeleteObject(hBrush);
    }
}

// ─── DrawOwnerButton ──────────────────────────────────────────────────────────
void DrawOwnerButton(LPDRAWITEMSTRUCT dis, bool isAccent)
{
    HDC hdc    = dis->hDC;
    RECT rc    = dis->rcItem;
    bool pressed = (dis->itemState & ODS_SELECTED) != 0;
    bool disabled = (dis->itemState & ODS_DISABLED) != 0;

    // Background
    if (disabled) {
        HBRUSH hBr = CreateSolidBrush(RGB(35, 35, 42));
        FillRect(hdc, &rc, hBr);
        DeleteObject(hBr);
    } else if (isAccent) {
        // Gold gradient for the Connect button
        COLORREF top = pressed ? CLR_ACCENT2 : RGB(230, 175, 35);
        COLORREF bot = pressed ? RGB(120, 85, 5) : CLR_ACCENT2;
        DrawGradientRect(hdc, &rc, top, bot, true);
    } else {
        // Dark fill for SET buttons; lighten slightly on press
        COLORREF fill = pressed ? RGB(55, 55, 70) : CLR_BTN;
        HBRUSH hBr = CreateSolidBrush(fill);
        FillRect(hdc, &rc, hBr);
        DeleteObject(hBr);
    }

    // Border — gold for accent button, subtle grey for regular
    COLORREF borderClr = isAccent ? (disabled ? RGB(90, 65, 5) : CLR_ACCENT2)
                                  : CLR_DIVIDER;
    HPEN hPen = CreatePen(PS_SOLID, 1, borderClr);
    HPEN hOld = (HPEN)SelectObject(hdc, hPen);
    SelectObject(hdc, GetStockObject(NULL_BRUSH));
    Rectangle(hdc, rc.left, rc.top, rc.right, rc.bottom);
    SelectObject(hdc, hOld);
    DeleteObject(hPen);

    // Text
    wchar_t text[128] = {};
    GetWindowTextW(dis->hwndItem, text, 128);

    SetBkMode(hdc, TRANSPARENT);
    if (disabled)
        SetTextColor(hdc, RGB(90, 90, 100));
    else if (isAccent)
        SetTextColor(hdc, RGB(30, 20, 0));   // dark on gold
    else
        SetTextColor(hdc, CLR_TEXT);

    HFONT hOldFont = (HFONT)SelectObject(hdc, isAccent ? g_hFontLabel : g_hFontBtn);
    DrawTextW(hdc, text, -1, &rc, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
    SelectObject(hdc, hOldFont);

    // Focus rect
    if (dis->itemState & ODS_FOCUS) {
        InflateRect(&rc, -3, -3);
        DrawFocusRect(hdc, &rc);
    }
}
