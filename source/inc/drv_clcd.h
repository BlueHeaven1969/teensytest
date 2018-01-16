// Character lcd
#ifndef _DRV_CLCD_H_
#define _DRV_CLCD_H_

#define CLCD_NUM_ROWS     4
#define CLCD_NUM_COLS     20
#define CLCD_POS_CENTERED 32
#define CLCD_POS_RIGHT    33

// Display commands
#define CLCD_CMD_DISPLAY_ON      0x41
#define CLCD_CMD_DISPLAY_OFF     0x42
#define CLCD_CMD_CUR_SET         0x45
#define CLCD_CMD_CUR_HOME        0x46
#define CLCD_CMD_CUR_UL_ON       0x47
#define CLCD_CMD_CUR_UL_OFF      0x48
#define CLCD_CMD_CUR_LEFT        0x49
#define CLCD_CMD_CUR_RIGHT       0x4A
#define CLCD_CMD_CUR_BL_ON       0x4B
#define CLCD_CMD_CUR_BL_OFF      0x4C
#define CLCD_CMD_BACKSPACE       0x4E
#define CLCD_CMD_CLS             0x51
#define CLCD_CMD_SET_CONTRAST    0x52
#define CLCD_CMD_SET_BRIGHT      0x53
#define CLCD_CMD_LOAD_CUST_CHAR  0x54
#define CLCD_CMD_DISPLAY_LEFT    0x55
#define CLCD_CMD_DISPLAY_RIGHT   0x56

// PROTOTYPES
void CLCD_Init(void);
void CLCD_PrintLine(uint8_t *text, uint8_t row, uint8_t pos);
void CLCD_ClearScreen(void);
#endif  // _CLCD_H_
