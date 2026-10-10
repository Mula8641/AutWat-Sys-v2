#include "display.h"
#include <LiquidCrystal.h>
#include <cstring>
#include "autwat_config.h"
#include "screen_text.h"

namespace {

constexpr uint8_t kRows = 4;

LiquidCrystal lcd(autwat::kPinLcdRs, autwat::kPinLcdEnable, autwat::kPinLcdD4, autwat::kPinLcdD5,
                  autwat::kPinLcdD6, autwat::kPinLcdD7);
char shown[kRows][autwat::kScreenBufferSize];

}  // namespace

void displayBegin() {
    lcd.begin(autwat::kScreenWidth, kRows);
    lcd.clear();
    for (auto& row : shown) row[0] = '\0';
}

void displayShow(const char* line0, const char* line1, const char* line2, const char* line3) {
    const char* const lines[kRows] = {line0, line1, line2, line3};
    for (uint8_t row = 0; row < kRows; ++row) {
        if (strncmp(shown[row], lines[row], autwat::kScreenWidth) != 0) {
            strncpy(shown[row], lines[row], autwat::kScreenWidth);
            shown[row][autwat::kScreenWidth] = '\0';
            lcd.setCursor(0, row);
            lcd.print(shown[row]);
        }
    }
}
