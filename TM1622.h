// tm1622.h
#ifndef __TM1622_H__
#define __TM1622_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "stm32f1xx_hal.h"



/* === 사용자 설정 핀 매핑 === */
#ifndef TM1622_CS_PORT
#define TM1622_CS_PORT   GPIOC
#endif
#ifndef TM1622_CS_PIN
#define TM1622_CS_PIN    GPIO_PIN_11
#endif

#ifndef TM1622_WR_PORT
#define TM1622_WR_PORT   GPIOC
#endif
#ifndef TM1622_WR_PIN
#define TM1622_WR_PIN    GPIO_PIN_12
#endif

#ifndef TM1622_DAT_PORT
#define TM1622_DAT_PORT  GPIOC
#endif
#ifndef TM1622_DAT_PIN
#define TM1622_DAT_PIN   GPIO_PIN_10
#endif

/* 지연시간 (µs) */
#ifndef TM1622_DELAY_US
#define TM1622_DELAY_US  2
#endif

/* 비트 송신 순서 */
typedef enum {
    TM1622_MSB_FIRST = 0,
    TM1622_LSB_FIRST = 1
} TM1622_BitOrder;

typedef enum {
    TM1622_ALIGN_LEFT   = 0,
    TM1622_ALIGN_CENTER = 1,
    TM1622_ALIGN_RIGHT  = 2
} TM1622_Align;

typedef enum {
  TM1622_SCROLL_RTL = 0,
  TM1622_SCROLL_LTR = 1
} TM1622_ScrollDir;


/* 자리수 */
#define TM1622_NUM_DIGITS 10
#define TM1622_NUM_FONTS 192

/* 자리별 RAM 주소 */
extern const uint8_t TM1622_DigitAddr[TM1622_NUM_DIGITS];





/* === 공개 API === */
void TM1622_Init(void);
void TM1622_Command(uint8_t cmd);
void TM1622_WriteData(uint8_t addr, uint16_t data, uint8_t bits);

void TM1622_PutChar(uint8_t pos, uint16_t segMask);
void TM1622_ClearAll(void);
void TM1622_AllSegments(uint8_t on);

void TM1622_WriteChar(uint8_t pos, char ch);   // ← 새 함수
void TM1622_WriteStr(uint8_t start_pos, const char* str);

void TM1622_WriteStrAligned(const char* str, TM1622_Align align);
void TM1622_ScrollStr(const char* str, uint16_t delay_ms);
void TM1622_ScrollStop(const char* str, uint32_t delay_ms, TM1622_ScrollDir dir);

void TM1622_ScrollPause(const char* str, uint32_t scroll_delay, uint32_t pause_ms, TM1622_ScrollDir dir);

#ifdef __cplusplus
}
#endif

#endif /* __TM1622_H__ */
