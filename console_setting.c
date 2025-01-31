#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include "console_setting.h"

//برای ویندوز ترمینال کاربرد ندارد
//اما برای cmd قدمی استفاده میشود
void setConsoleSizeAndLockLOGIN() {
    // تنظیم سایز پنجره‌ی کنسول
    system("mode con: cols=50 lines=30");

    
    HWND hwnd = GetConsoleWindow();

    // غیرفعال کردن تغییر سایز پنجره
    LONG style = GetWindowLong(hwnd, GWL_STYLE);
    style &= ~(WS_SIZEBOX | WS_MAXIMIZEBOX);  // حذف دکمه‌ی تغییر سایز و بزرگ‌نمایی
    SetWindowLong(hwnd, GWL_STYLE, style);
    
    // اعمال تنظیمات جدید
    SetWindowPos(hwnd, NULL, 100, 100, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
}
void setConsoleSizeAndLockGAME() {
    // تنظیم سایز پنجره‌ی کنسول
    system("mode con: cols=118 lines=15");

    
    HWND hwnd = GetConsoleWindow();

    // غیرفعال کردن تغییر سایز پنجره
    LONG style = GetWindowLong(hwnd, GWL_STYLE);
    style &= ~(WS_SIZEBOX | WS_MAXIMIZEBOX);  // حذف دکمه‌ی تغییر سایز و بزرگ‌نمایی
    SetWindowLong(hwnd, GWL_STYLE, style);
    
    
    SetWindowPos(hwnd, NULL, 100, 100, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
}

#include <windows.h>

