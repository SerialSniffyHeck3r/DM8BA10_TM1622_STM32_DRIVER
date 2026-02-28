
# TM1622 Spaghetti Driver for STM32

**"Just because it looked cool, I bought it. And then, I made this."**  

---

## What is this?

This repository contains a custom driver written from scratch for the **DM8BA10 LCD** display module, powered by a mysterious **TM1622** controller.  
It’s one of those "*cool looking things you randomly buy from AliExpress*" and then realize... **there’s no documentation for it.** 🤷

---

## Features

-  Pure GPIO bit-banging (SPI-like protocol)
-  Fully reverse-engineered protocol implementation
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
