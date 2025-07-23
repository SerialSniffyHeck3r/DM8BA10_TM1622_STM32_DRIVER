
/**
  ******************************************************************************
  * @file           : tm1622.c
  * @brief          : HAL BASED DM8BA10 DRIVER FOR STM32
  ******************************************************************************
  LUCKILY I MANAGED TO COOK A FILE OF SPAGHETTI HERE.

  THIS FILE IS A DRIVER TO CONTROL DM8BA10 LCD DISPLAY.

  THIS DISPLAY IS READILY AVAILABLE ON ALIEXPRESS FOR QUITE CHEAP PRICE, AND SUITABLE FOR MANY PROJECTS.
  HOWEVER, ONE BUT VERY CRITICAL DOWNSIDE OF THIS MODULE IS THAT ITS DOCUMENTATION IS VERY LIMITED ONLINE.
  STILL I THOUGHT THIS DISPLAY LOOKS COOL AS HECK SO I PURCHASED THIS WITHOUT EVEN THINKING, AND CREATED THIS DRIVER.

  ENJOY!

  ******************************************************************************
  */



#include "tm1622.h"
#include <string.h>
#include <stdlib.h>

// ======== DIGIT ADDRESS ========================
/*
 * THIS ARRAY IS USED TO DEFINE THE LOCATION OF DIGITS
 * FROM LEFT TO RIGHT, 9 TO 0.
 * WILL BE ACCESSED FROM PutChar FUNCTION
 * LEAVE IT, DO NOT TOUCH
 */
const uint8_t TM1622_DigitAddr[TM1622_NUM_DIGITS] = {
    0x24, 0x20, 0x1C, 0x18, 0x14, 0x10, 0x0C, 0x08, 0x04, 0x00
};
//==================================================





// ======== FONTS SET ===========================


const uint16_t TM1622_FontSet[TM1622_NUM_FONTS] =
{
    0x0000, // space
    0x0220, // !
    0x0060, // "
    0xC621, // #
    0xDA3D, // $
    0xD3AD, // %
    0xBC2A, // &
    0x0020, // '
    0x0082, // (
    0x2100, // )
    0xA3A3, // *
    0x8221, // +
    0x0100, // ,
    0x8001, // -
    0x020D, // .
    0x0180, // /
    0x5DDC, // 0
    0x00C4, // 1
    0x9C59, // 2
    0x185D, // 3
    0xC045, // 4
    0xD81A, // 5
    0xDC1D, // 6
    0x1190, // 7
    0xDC5D, // 8
    0xD85D, // 9
    0x4400, // :
    0x0120, // ;
    0x8082, // <
    0x8809, // =
    0x2101, // >
    0x1251, // ?
    0x5CD9, // @
    0xD455, // A
    0x1A7D, // B
    0x5C18, // C
    0x1A7C, // D
    0xDC18, // E
    0xD410, // F
    0x5C1D, // G
    0xC445, // H
    0x1A38, // I
    0x0C4C, // J
    0xC482, // K
    0x4C08, // L
    0x64C4, // M
    0x6446, // N
    0x5C5C, // O
    0xD451, // P
    0x5C5E, // O
    0xD453, // R
    0xD81D, // S
    0x1230, // T
    0x4C4C, // U
    0x4580, // V
    0x4546, // W
    0x2182, // X
    0xC241, // Y
    0x1998, // Z
    0x0238, // [
    0x2002, // \ //
    0x1A20, // ]
    0x0102, // ^
    0x0808, // _
    0x2000, // `
    0xAE08, // a
    0xCE00, // b
    0x8C00, // c
    0x8E20, // d
    0x8D00, // e
    0x8231, // f
    0xDA20, // g
    0xC600, // h
    0x0200, // i
    0x0E20, // j
    0x02A2, // k
    0x0228, // l
    0x8605, // m
    0x0205, // n
    0x8E00, // o
    0xE400, // p
    0x00C5, // q
    0x8400, // r
    0xDA00, // s
    0xCC00, // t
    0x0E00, // u
    0x0500, // v
    0x0506, // w
    0x2182, // x
    0x2280, // y
    0x8900, // z
    0xB900, // {
    0x4400, // |
    0x009B, // }
    0x8181, // ~
    0x0000, // del
};


const unsigned int custom_char[1] =
{
    0x0071 // degree symbol
};

// ==========================================






//======== DWT DELAY =========================
static inline void TM1622_DelayUs(uint32_t us) {
    uint32_t start = DWT->CYCCNT;
    uint32_t ticks = us * (SystemCoreClock / 1000000U);
    while ((DWT->CYCCNT - start) < ticks) { __NOP(); }
}
//=============================================



//======== GPIO HELPER =============================
#define CS_L()  HAL_GPIO_WritePin(TM1622_CS_PORT,  TM1622_CS_PIN,  GPIO_PIN_RESET)
#define CS_H()  HAL_GPIO_WritePin(TM1622_CS_PORT,  TM1622_CS_PIN,  GPIO_PIN_SET)
#define WR_L()  HAL_GPIO_WritePin(TM1622_WR_PORT,  TM1622_WR_PIN,  GPIO_PIN_RESET)
#define WR_H()  HAL_GPIO_WritePin(TM1622_WR_PORT,  TM1622_WR_PIN,  GPIO_PIN_SET)
#define DT_L()  HAL_GPIO_WritePin(TM1622_DAT_PORT, TM1622_DAT_PIN, GPIO_PIN_RESET)
#define DT_H()  HAL_GPIO_WritePin(TM1622_DAT_PORT, TM1622_DAT_PIN, GPIO_PIN_SET)
//=============================================





//=========== BITS SENDER CORE =====================
static void TM1622_SendBits(uint16_t data, uint8_t bits, TM1622_BitOrder order) {
    uint16_t mask = (order == TM1622_LSB_FIRST) ? 1 : (1 << (bits - 1));
    for (uint8_t i = 0; i < bits; i++) {
        WR_L();
        (data & mask) ? DT_H() : DT_L();
        TM1622_DelayUs(TM1622_DELAY_US);
        WR_H();
        TM1622_DelayUs(TM1622_DELAY_US);
        if (order == TM1622_LSB_FIRST) mask <<= 1;
        else                         mask >>= 1;
    }
}
//===============================================





void TM1622_Init(void) {
    /* DWT 초기화 (HAL_Init 이후) */
    CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
    DWT->CTRL      |= DWT_CTRL_CYCCNTENA_Msk;

    CS_H(); WR_H(); DT_L();
    TM1622_DelayUs(1000);
    TM1622_Command(0x01);  /* SYS_EN */
    TM1622_Command(0x18);  /* RC_INT */
    TM1622_Command(0x03);  /* LCD_ON */
}

void TM1622_Command(uint8_t cmd) {
    CS_L();
    TM1622_SendBits(0b100, 3, TM1622_MSB_FIRST);
    TM1622_SendBits(cmd,   8, TM1622_MSB_FIRST);
    TM1622_SendBits(1,     1, TM1622_MSB_FIRST);
    CS_H();
}

void TM1622_WriteData(uint8_t addr, uint16_t data, uint8_t bits) {
    CS_L();
    TM1622_SendBits(0b101, 3, TM1622_MSB_FIRST);
    TM1622_SendBits(addr,  6, TM1622_MSB_FIRST);
    for (int8_t n = (bits/4)-1; n >= 0; n--) {
        uint16_t nib = (data >> (n*4)) & 0x0F;
        TM1622_SendBits(nib, 4, TM1622_LSB_FIRST);
    }
    CS_H();
}

void TM1622_PutChar(uint8_t pos, uint16_t segMask) {
    if (pos < TM1622_NUM_DIGITS)
        TM1622_WriteData(TM1622_DigitAddr[pos], segMask, 16);
}

void TM1622_ClearAll(void) {
    for (uint8_t a = 0; a < 64; a++)
        TM1622_WriteData(a, 0x0, 4);
}

void TM1622_AllSegments(uint8_t on) {
    for (uint8_t a = 0; a < 64; a++)
        TM1622_WriteData(a, on ? 0xF : 0x0, 4);
}



void TM1622_WriteChar(uint8_t pos, char ch) /// TO USE CUSTOM CHAR, PUT custom_char[YOUR CHAR NUM] to char ch
{
    if (pos >= TM1622_NUM_DIGITS) return;
    if (ch  < 0x20 || ch > 0x7F) ch = ' ';

    uint16_t seg = TM1622_FontSet[(uint8_t)ch - 0x20];
    TM1622_PutChar(pos, seg);
}



void TM1622_WriteStr(uint8_t start_pos, const char* str) {
    uint8_t i = start_pos;
    while (*str && i < TM1622_NUM_DIGITS) {
        char ch = *str++;

        // 폰트 범위 벗어날 경우 공백 처리
        if (ch < 0x20 || ch > 0x7F) ch = ' ';

        TM1622_PutChar(i, TM1622_FontSet[(uint8_t)ch - 0x20]);
        i++;
    }
}

void TM1622_WriteStrAligned(const char* str, TM1622_Align align) {
    uint8_t len = strlen(str);
    uint8_t start = 0;
    if (len < TM1622_NUM_DIGITS) {
        switch (align) {
            case TM1622_ALIGN_LEFT:   start = 0;                                              break;
            case TM1622_ALIGN_CENTER: start = (TM1622_NUM_DIGITS - len) / 2;                   break;
            case TM1622_ALIGN_RIGHT:  start = TM1622_NUM_DIGITS - len;                         break;
        }
    }
    TM1622_WriteStr(start, str);
}

void TM1622_ScrollStr(const char* str, uint16_t delay_ms) {
    uint8_t len = strlen(str);
    char window[TM1622_NUM_DIGITS + 1];
    for (uint8_t offset = 0; offset <= len; offset++) {
        // 버퍼를 공백으로 채우고
        memset(window, ' ', sizeof(window));
        // str[offset]부터 TM1622_NUM_DIGITS만큼 복사
        strncpy(window, str + offset, TM1622_NUM_DIGITS);
        window[TM1622_NUM_DIGITS] = '\0';
        // 화면에 표시
        TM1622_WriteStr(0, window);
        HAL_Delay(delay_ms);
    }
}

void TM1622_ScrollStop(const char* str, uint32_t delay_ms, TM1622_ScrollDir dir) {
    size_t len = strlen(str);
    size_t N   = TM1622_NUM_DIGITS;

    size_t pre  = (dir == TM1622_SCROLL_RTL) ? (N - 1) : ( (len < N) ? (N - len) : 0 );
    size_t post = (dir == TM1622_SCROLL_RTL) ? ( (len < N) ? (N - len) : 0 ) : (N - 1);

    size_t buf_len = pre + len + post;
    char *buf = malloc(buf_len + 1);
    if (!buf) return;

    memset(buf, ' ', pre);
    memcpy(buf + pre, str, len);
    memset(buf + pre + len, ' ', post);
    buf[buf_len] = '\0';


    for (size_t pos = 0; pos + N <= buf_len; pos++) {
        char window[TM1622_NUM_DIGITS + 1];
        memcpy(window, buf + pos, N);
        window[N] = '\0';
        TM1622_WriteStr(0, window);

        HAL_Delay(delay_ms);
    }

    free(buf);
}

/*
 *
    void TM1622_ScrollStop(const char* str, uint32_t delay_ms, TM1622_ScrollDir dir) {
    size_t len = strlen(str);
    size_t N   = TM1622_NUM_DIGITS;

    // 동일한 pre/post padding (양쪽에 빈 칸)
    size_t pad = (len < N) ? (N - len) : 0;
    size_t buf_len = pad + len + pad;

    // 버퍼 생성
    char* buf = malloc(buf_len + 1);
    if (!buf) return;
    memset(buf, ' ', pad);
    memcpy(buf + pad, str, len);
    memset(buf + pad + len, ' ', pad);
    buf[buf_len] = '\0';

    if (dir == TM1622_SCROLL_RTL) {
        // 오른쪽→왼쪽 (기존 로직)
        for (size_t pos = 0; pos + N <= buf_len; pos++) {
            TM1622_WriteStr(0, buf + pos);
            HAL_Delay(delay_ms);
        }
    } else {
        // 왼쪽→오른쪽 (역순으로 읽기)
        for (int pos = (int)(buf_len - N); pos >= 0; pos--) {
            TM1622_WriteStr(0, buf + pos);
            HAL_Delay(delay_ms);
        }
    }

    free(buf);
}
 *
 */

void TM1622_ScrollPause(const char* str,
                       uint32_t scroll_delay,
                       uint32_t pause_ms,
                       TM1622_ScrollDir dir)
{
    size_t len = strlen(str);
    size_t N   = TM1622_NUM_DIGITS;
    size_t pad = N;                       // 좌·우에 N칸씩 패딩
    size_t buf_len = pad + len + pad;
    char *buf = malloc(buf_len + 1);
    if (!buf) return;

    // 1) 버퍼 구성: [빈칸×N][문자열][빈칸×N]
    memset(buf,               ' ', pad);
    memcpy(buf + pad,         str, len);
    memset(buf + pad + len,   ' ', pad);
    buf[buf_len] = '\0';

    // 2) 단일 루프: in → pause → out
    if (dir == TM1622_SCROLL_RTL) {
        // 우→좌: buf[0] → buf[buf_len-N]
        for (size_t pos = 0; pos + N <= buf_len; pos++) {
            TM1622_WriteStr(0, buf + pos);

            // 한 번만 멈춤: 문자열이 완전 왼쪽에 붙는 순간
            if (pos == pad) HAL_Delay(pause_ms);

            HAL_Delay(scroll_delay);
        }
    } else {
        // 좌→우: buf[buf_len-N] → buf[0]
        for (int pos = (int)(buf_len - N); pos >= 0; pos--) {
            TM1622_WriteStr(0, buf + pos);

            // 한 번만 멈춤: 문자열이 완전 오른쪽에 붙는 순간
            if (pos == (int)pad) HAL_Delay(pause_ms);

            HAL_Delay(scroll_delay);
        }
    }

    free(buf);
}


//////////////////////////////////////////
/*
 *
 *
 *
 *
 *
 *

extern TIM_HandleTypeDef htim2;  // CubeMX 생성된 핸들러

static char* _scroll_buf;
static uint16_t _scroll_len;
static uint16_t _scroll_pos;
static TM1622_ScrollDir _scroll_dir;
static TIM_HandleTypeDef* _scroll_tim;

void TM1622_ScrollIT(const char* str, uint32_t interval_ms, TM1622_ScrollDir dir) {
    size_t len = strlen(str), N = TM1622_NUM_DIGITS;
    size_t pre  = (dir == TM1622_SCROLL_RTL) ? (N - 1) : (N - len);
    size_t post = (dir == TM1622_SCROLL_RTL) ? (N - len) : (N - 1);
    _scroll_len = pre + len + post;
    _scroll_buf = malloc(_scroll_len + 1);
    memset(_scroll_buf, ' ', pre);
    memcpy(_scroll_buf + pre, str, len);
    memset(_scroll_buf + pre + len, ' ', post);
    _scroll_buf[_scroll_len] = '\0';
    _scroll_pos = 0;
    _scroll_dir = dir;

    _scroll_tim = &htim2;
    __HAL_TIM_SET_AUTORELOAD(_scroll_tim, interval_ms);
    __HAL_TIM_SET_COUNTER(_scroll_tim, 0);
    HAL_TIM_Base_Start_IT(_scroll_tim);
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
    if (htim == _scroll_tim) {
        if (_scroll_pos <= _scroll_len - TM1622_NUM_DIGITS) {
            char wnd[TM1622_NUM_DIGITS+1];
            memcpy(wnd, _scroll_buf + _scroll_pos, TM1622_NUM_DIGITS);
            wnd[TM1622_NUM_DIGITS] = '\0';
            TM1622_WriteStr(0, wnd);
            _scroll_pos++;
        } else {
            TM1622_StopScroll();
        }
    }
}

void TM1622_StopScroll(void) {
    if (_scroll_tim) HAL_TIM_Base_Stop_IT(_scroll_tim);
    free(_scroll_buf);
    _scroll_buf = NULL;
}

// 오른쪽에서 왼쪽으로
TM1622_ScrollIT("HELLO", 200, TM1622_SCROLL_RTL);
// 왼쪽에서 오른쪽으로
TM1622_ScrollIT("HELLO", 200, TM1622_SCROLL_LTR);

 */

 