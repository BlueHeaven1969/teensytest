// Character lcd
#ifndef _CLCD_H_
#define _CLCD_H_

#define CLCD_NUM_ROWS     4
#define CLCD_NUM_COLS     20
#define CLCD_POS_CENTERED 32
#define CLCD_POS_RIGHT    33

// PROTOTYPES
void CLCD_Init(void);
void CLCD_PrintLine(uint8_t *text, uint8_t row, uint8_t pos);
void CLCD_ClearScreen(void);
#endif  // _CLCD_H_
