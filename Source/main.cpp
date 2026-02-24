#include <Windows.h>
#include <CommCtrl.h>
#include <string>
#include "resource.h"
#include "MemoryManager.h"
#pragma comment(lib, "comctl32.lib")

HINSTANCE g_hInst;
HWND g_hMainWnd, hStatusBar, hBtnConnect;
HWND hWoodEdit, hGoldEdit, hIronEdit, hSkillEdit, hArcaneEdit, hPrimalEdit, hDivineEdit, hUnholyEdit;
HWND hWoodBtn, hGoldBtn, hIronBtn, hSkillBtn, hArcaneBtn, hPrimalBtn, hDivineBtn, hUnholyBtn;
HWND hFrzWood, hFrzGold, hFrzIron, hFrzArcane, hFrzPrimal, hFrzDivine, hFrzUnholy;
HWND hGodMode, hInfHealth, hInfMana, hOneHit, hMaxXP;
MemoryManager* g_memMgr = nullptr;
bool g_connected = false;
bool g_features[5] = {false};

HBRUSH g_hBrushBg, g_hBrushPanel, g_hBrushBtn, g_hBrushEdit;
HFONT g_hFontTitle, g_hFontLabel, g_hFontBtn;

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
void CreateModernUI(HWND hwnd);
void ConnectToGame();
void ApplyResource(int type, const std::wstring& val);
void UpdateStatus(const std::wstring& msg, COLORREF clr);
void DrawGradientRect(HDC hdc, RECT* rc, COLORREF c1, COLORREF c2, bool vertical);

int WINAPI WinMain(_In_ HINSTANCE hInst, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int nShow)
{
    g_hInst = hInst;
    INITCOMMONCONTROLSEX icc = {sizeof(INITCOMMONCONTROLSEX), ICC_WIN95_CLASSES};
    InitCommonControlsEx(&icc);
    
    g_hBrushBg = CreateSolidBrush(RGB(18, 18, 20));
    g_hBrushPanel = CreateSolidBrush(RGB(28, 28, 32));
    g_hBrushBtn = CreateSolidBrush(RGB(45, 45, 52));
    g_hBrushEdit = CreateSolidBrush(RGB(35, 35, 40));
    
    g_hFontTitle = CreateFontW(28, 0, 0, 0, FW_BOLD, 0, 0, 0, DEFAULT_CHARSET, 0, 0, CLEARTYPE_QUALITY, 0, L"Segoe UI");
    g_hFontLabel = CreateFontW(15, 0, 0, 0, FW_MEDIUM, 0, 0, 0, DEFAULT_CHARSET, 0, 0, CLEARTYPE_QUALITY, 0, L"Segoe UI");
    g_hFontBtn = CreateFontW(14, 0, 0, 0, FW_SEMIBOLD, 0, 0, 0, DEFAULT_CHARSET, 0, 0, CLEARTYPE_QUALITY, 0, L"Segoe UI");
    
    WNDCLASSEXW wc = {sizeof(WNDCLASSEXW)};
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInst;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = g_hBrushBg;
    wc.lpszClassName = L"DisciplesDominationTrainer";
    if (!RegisterClassExW(&wc)) return 0;
    
    g_hMainWnd = CreateWindowExW(0, L"DisciplesDominationTrainer", 
        L"DISCIPLES: DOMINATION - Enhancement Suite v2.1",
        WS_OVERLAPPED|WS_CAPTION|WS_SYSMENU|WS_MINIMIZEBOX,
        CW_USEDEFAULT, CW_USEDEFAULT, 920, 720, NULL, NULL, hInst, NULL);
    
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
    DeleteObject(g_hFontTitle);
    DeleteObject(g_hFontLabel);
    DeleteObject(g_hFontBtn);
    if (g_memMgr) delete g_memMgr;
    return (int)msg.wParam;
}

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
        
        // Header gradient
        SetRect(&rc, 0, 0, 920, 80);
        DrawGradientRect(hdc, &rc, RGB(25, 25, 30), RGB(15, 15, 18), false);
        
        // Divider line
        HPEN hPen = CreatePen(PS_SOLID, 2, RGB(60, 120, 200));
        SelectObject(hdc, hPen);
        MoveToEx(hdc, 0, 80, NULL);
        LineTo(hdc, 920, 80);
        DeleteObject(hPen);
        
        EndPaint(hwnd, &ps);
        return 0;
    }
    
    case WM_CTLCOLORSTATIC:
    case WM_CTLCOLORBTN: {
        HDC hdcStatic = (HDC)wp;
        SetTextColor(hdcStatic, RGB(220, 220, 230));
        SetBkColor(hdcStatic, RGB(18, 18, 20));
        return (LRESULT)g_hBrushBg;
    }
    
    case WM_CTLCOLOREDIT: {
        HDC hdcEdit = (HDC)wp;
        SetTextColor(hdcEdit, RGB(255, 255, 255));
        SetBkColor(hdcEdit, RGB(35, 35, 40));
        return (LRESULT)g_hBrushEdit;
    }
    
    case WM_COMMAND:
        if (LOWORD(wp) == 5001) ConnectToGame();
        else if (LOWORD(wp) >= 6001 && LOWORD(wp) <= 6008) {
            if (!g_connected) { UpdateStatus(L"[!] Please connect to game first", RGB(255, 100, 100)); return 0; }
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
            switch(feat) {
                case 0: g_memMgr->SetGodMode(g_features[0]); break;
                case 1: g_memMgr->SetInfiniteHealth(g_features[1]); break;
                case 2: g_memMgr->SetInfiniteMana(g_features[2]); break;
                case 3: g_memMgr->SetOneHitKill(g_features[3]); break;
                case 4: g_memMgr->SetMaxExperience(g_features[4]); break;
            }
            UpdateStatus(g_features[feat] ? L"[+] Feature enabled" : L"[-] Feature disabled", 
                g_features[feat] ? RGB(100, 255, 150) : RGB(200, 200, 200));
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

void CreateModernUI(HWND hwnd)
{
    // === HEADER SECTION ===
    // Title - centered and prominent
    HWND hTitle = CreateWindowW(L"STATIC", L"DISCIPLES: DOMINATION", 
        WS_CHILD | WS_VISIBLE | SS_CENTER,
        0, 18, 920, 35, hwnd, NULL, g_hInst, NULL);
    SendMessage(hTitle, WM_SETFONT, (WPARAM)g_hFontTitle, TRUE);

    // Subtitle
    HWND hSubtitle = CreateWindowW(L"STATIC", L"Enhancement Suite v2.1", 
        WS_CHILD | WS_VISIBLE | SS_CENTER,
        0, 50, 920, 20, hwnd, NULL, g_hInst, NULL);
    SendMessage(hSubtitle, WM_SETFONT, (WPARAM)g_hFontLabel, TRUE);

    // Divider line (drawn in WM_PAINT)

    // === CONNECTION SECTION ===
    // Connect button - centered and large
    hBtnConnect = CreateWindowW(L"BUTTON", L"CONNECT TO GAME", 
        WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
        310, 85, 300, 45, hwnd, (HMENU)(INT_PTR)5001, g_hInst, NULL);
    SendMessage(hBtnConnect, WM_SETFONT, (WPARAM)g_hFontLabel, TRUE);

    // === RESOURCES GROUP ===
    HWND hGrpRes = CreateWindowW(L"BUTTON", L"  RESOURCES  ", 
        WS_CHILD | WS_VISIBLE | BS_GROUPBOX,
        15, 145, 890, 295, hwnd, NULL, g_hInst, NULL);
    SendMessage(hGrpRes, WM_SETFONT, (WPARAM)g_hFontLabel, TRUE);

    const wchar_t* resLabels[] = {
        L"Wood:", L"Gold:", L"Iron:", L"Skill Points:",
        L"Arcane Flux:", L"Primal Essence:", L"Divine Essence:", L"Unholy Essence:"
    };

    // Perfect alignment coordinates
    int labelX = 40;       // Label position
    int labelWidth = 160;  // Label width
    int editX = 210;       // Edit box position  
    int editWidth = 140;   // Edit box width
    int btnX = 365;        // Button position
    int btnWidth = 90;     // Button width
    int lockX = 470;       // Lock checkbox position
    int lockWidth = 80;    // Lock width

    int y = 175;           // Starting Y position
    int rowHeight = 35;    // Height of each row

    HWND resEdits[] = {hWoodEdit, hGoldEdit, hIronEdit, hSkillEdit, hArcaneEdit, hPrimalEdit, hDivineEdit, hUnholyEdit};
    HWND resBtns[] = {hWoodBtn, hGoldBtn, hIronBtn, hSkillBtn, hArcaneBtn, hPrimalBtn, hDivineBtn, hUnholyBtn};
    HWND resFrz[] = {hFrzWood, hFrzGold, hFrzIron, hFrzArcane, hFrzPrimal, hFrzDivine, hFrzUnholy};

    for (int i = 0; i < 8; i++) {
        // Label with right alignment
        HWND lbl = CreateWindowW(L"STATIC", resLabels[i], 
            WS_CHILD | WS_VISIBLE | SS_RIGHT,
            labelX, y + 7, labelWidth, 20, hwnd, NULL, g_hInst, NULL);
        SendMessage(lbl, WM_SETFONT, (WPARAM)g_hFontLabel, TRUE);

        // Edit box - perfectly centered with borders
        HWND edt = CreateWindowExW(WS_EX_CLIENTEDGE, L"EDIT", L"99999", 
            WS_CHILD | WS_VISIBLE | ES_NUMBER | ES_CENTER,
            editX, y + 2, editWidth, 30, hwnd, NULL, g_hInst, NULL);
        SendMessage(edt, WM_SETFONT, (WPARAM)g_hFontLabel, TRUE);

        // SET button - aligned perfectly
        HWND btn = CreateWindowW(L"BUTTON", L"SET", 
            WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
            btnX, y + 2, btnWidth, 30, hwnd, (HMENU)(INT_PTR)(6001 + i), g_hInst, NULL);
        SendMessage(btn, WM_SETFONT, (WPARAM)g_hFontBtn, TRUE);

        // Lock checkbox (all except Skill Points)
        if (i < 7) {
            HWND frz = CreateWindowW(L"BUTTON", L"Lock", 
                WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX,
                lockX, y + 7, lockWidth, 20, hwnd, (HMENU)(INT_PTR)(7001 + i), g_hInst, NULL);
            SendMessage(frz, WM_SETFONT, (WPARAM)g_hFontLabel, TRUE);
            resFrz[i] = frz;
        }

        // Store handles
        switch(i) {
            case 0: hWoodEdit = edt; hWoodBtn = btn; hFrzWood = resFrz[i]; break;
            case 1: hGoldEdit = edt; hGoldBtn = btn; hFrzGold = resFrz[i]; break;
            case 2: hIronEdit = edt; hIronBtn = btn; hFrzIron = resFrz[i]; break;
            case 3: hSkillEdit = edt; hSkillBtn = btn; break;
            case 4: hArcaneEdit = edt; hArcaneBtn = btn; hFrzArcane = resFrz[i]; break;
            case 5: hPrimalEdit = edt; hPrimalBtn = btn; hFrzPrimal = resFrz[i]; break;
            case 6: hDivineEdit = edt; hDivineBtn = btn; hFrzDivine = resFrz[i]; break;
            case 7: hUnholyEdit = edt; hUnholyBtn = btn; break;
        }

        y += rowHeight;
    }

    // === COMBAT & EXPERIENCE GROUP ===
    HWND hGrpCombat = CreateWindowW(L"BUTTON", L"  COMBAT & EXPERIENCE  ", 
        WS_CHILD | WS_VISIBLE | BS_GROUPBOX,
        15, 455, 890, 175, hwnd, NULL, g_hInst, NULL);
    SendMessage(hGrpCombat, WM_SETFONT, (WPARAM)g_hFontLabel, TRUE);

    const wchar_t* featLabels[] = {
        L"God Mode (Invincibility)",
        L"Infinite Squad Health",
        L"Unlimited Mana/Energy",
        L"One-Hit Kill",
        L"Maximum Experience Multiplier"
    };

    y = 485;
    int featRowHeight = 32;

    for (int i = 0; i < 5; i++) {
        HWND chk = CreateWindowW(L"BUTTON", featLabels[i], 
            WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX,
            40, y, 420, 25, hwnd, (HMENU)(INT_PTR)(8001 + i), g_hInst, NULL);
        SendMessage(chk, WM_SETFONT, (WPARAM)g_hFontLabel, TRUE);

        switch(i) {
            case 0: hGodMode = chk; break;
            case 1: hInfHealth = chk; break;
            case 2: hInfMana = chk; break;
            case 3: hOneHit = chk; break;
            case 4: hMaxXP = chk; break;
        }

        y += featRowHeight;
    }

    // === STATUS BAR ===
    // Bottom status bar - sleek and informative
    hStatusBar = CreateWindowW(L"STATIC", L"[*] System ready - Waiting for connection...",
        WS_CHILD | WS_VISIBLE | SS_CENTER,
        15, 645, 890, 28, hwnd, NULL, g_hInst, NULL);
    SendMessage(hStatusBar, WM_SETFONT, (WPARAM)g_hFontLabel, TRUE);
}

void ConnectToGame()
{
    const wchar_t* names[] = {
        L"Disciples_Domination.exe", L"DisciplesDomination.exe", 
        L"Disciples.exe", L"Domination.exe"
    };
    
    for (int i = 0; i < 4; i++) {
        if (g_memMgr->AttachToProcess(names[i])) {
            g_connected = true;
            UpdateStatus(L"[+] Successfully connected to Disciples: Domination!", RGB(100, 255, 150));
            EnableWindow(hBtnConnect, FALSE);
            SetTimer(g_hMainWnd, 1, 100, NULL);
            return;
        }
    }
    UpdateStatus(L"[!] Disciples: Domination not found - Please launch the game", RGB(255, 100, 100));
}

void ApplyResource(int type, const std::wstring& val)
{
    int v = _wtoi(val.c_str());
    if (g_memMgr->SetResourceValue(type, v)) {
        const wchar_t* names[] = {L"Wood", L"Gold", L"Iron", L"Skills", L"Arcane", L"Primal", L"Divine", L"Unholy"};
        std::wstring m = L"[+] " + std::wstring(names[type]) + L" set to " + val;
        UpdateStatus(m, RGB(100, 255, 150));
    } else {
        UpdateStatus(L"[!] Failed to write memory - Check game state", RGB(255, 100, 100));
    }
}

void UpdateStatus(const std::wstring& msg, COLORREF clr)
{
    SetWindowTextW(hStatusBar, msg.c_str());
    InvalidateRect(hStatusBar, NULL, TRUE);
}

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
            fillRect.top = rc->top + i;
            fillRect.bottom = fillRect.top + 1;
        } else {
            fillRect.left = rc->left + i;
            fillRect.right = fillRect.left + 1;
        }
        FillRect(hdc, &fillRect, hBrush);
        DeleteObject(hBrush);
    }
}
