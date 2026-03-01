
# DM8BA10 Display Driver for STM32

**"Just because it looked cool, I bought it. And then, I made this."**  

## STM32용 DM8BA10 디스플레이 드라이버: 한국어 설명

Aliexpress에서 '그저 멋있어 보여서' 충동적으로 구매한 10자리 16-Segment LCD 디스플레이인 DM8BA10용 드라이버를 STM32로 포팅한 프로젝트입니다.
소프트웨어 방식의 유사 SPI 비트 뱅잉 방식으로 구현하였으며, 벤더에서 제공하는 Arduino용 드라이버를 STM32로 포팅한 뒤 일부 유용한 기능을 추가하였습니다. 

---

## What is this?

This repository contains a custom driver written from scratch for the **DM8BA10 LCD** display module, powered by a mysterious **TM1622** controller.  
It’s one of those "*cool looking things you randomly buy from AliExpress*" and then realize... **there’s no documentation for it.** 

---

## Features

-  Pure GPIO bit-banging (SPI-like protocol)
-  Digit-wise segment addressing (10 digits supported)
-  Built-in font table for ASCII characters (and a custom ° symbol!)
-  String printing with alignment (`left`, `center`, `right`)
-  Scrolling text (RTL & LTR)
-  Timer-based interrupt scrolling (commented section)
-  Clear all / All-on segment control

---

## How to use?

- Put TM1622.C and TM1622.H into your project file and link 'em
- Enjoy!

##  Requirements

- STM32 microcontroller (tested with STM32F103, but I am pretty sure this will work on different nucleos since this driver uses standard HAL library)
- HAL library (CubeMX)
- A patient and slightly masochistic attitude
