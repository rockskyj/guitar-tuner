#include <string.h>
#include "stm32f429i_discovery_lcd.h"
#include "syscall.h"
#include "metronome.h"
#include "ui.h"
#include "main.h"

static uint8_t sound[8][2] = {"A", "B", "C", "D", "E", "F", "G", "H"};
static uint8_t hz_str[3] = "Hz";
static uint8_t frequency_str[4];
static uint8_t bpm_text_str[4] = "BPM";
static uint8_t beat_text_str[5] = "BEAT";
static uint8_t bpm_str[4];
static uint8_t beat_count_str[2];

extern int metronome_bpm;
extern int metronome_beat_count;

extern int mode;

static uint8_t* itoa(int value, uint8_t* result, int base)
{
	if (base < 2 || base > 36) {
		*result = '\0';
		return result;
	}
	uint8_t *ptr = result, *ptr1 = result, tmp_char;
	int tmp_value;

	do {
		tmp_value = value;
		value /= base;
		*ptr++ = "zyxwvutsrqponmlkjihgfedcba9876543210123456789abcdefghijklmnopqrstuvwxyz" [35 + (tmp_value - value * base)];
	} while (value);

	if (tmp_value < 0) *ptr++ = '-';
	*ptr-- = '\0';
	while (ptr1 < ptr) {
		tmp_char = *ptr;
		*ptr-- = *ptr1;
		*ptr1++ = tmp_char;
	}
	return result;
}

void ui_bfclear()
{
	memset((void *) LCD_FRAME_BUFFER + BUFFER_OFFSET, 0xff, BUFFER_OFFSET);
	//memset((void *) LCD_FRAME_BUFFER, 0xff, BUFFER_OFFSET);
}


void ui_draw_beat(int color)
{

    switch(color){
        case GREEN:
            LCD_SetColors(LCD_COLOR_GREEN, LCD_COLOR_GREEN);
            break;
        case RED:
            LCD_SetColors(LCD_COLOR_RED, LCD_COLOR_RED);
            break;
    }

    LCD_DrawFullCircle(200, 40, 10);


    sleep(50);
    
    // clear the circle
    LCD_SetColors(LCD_COLOR_WHITE, LCD_COLOR_WHITE);
    LCD_DrawFullCircle(200, 40, 10);
}

void ui_draw_tuner()
{
	int hz = 332;

	itoa(hz, frequency_str, 10);
	strcat((char *)frequency_str, (char *)hz_str);

	LCD_SetColors(LCD_COLOR_MAGENTA , LCD_COLOR_WHITE);
    
	LCD_DrawLine(20, 200, 200, LCD_DIR_HORIZONTAL);
	LCD_DrawLine(20, 180, 40, LCD_DIR_VERTICAL);
	LCD_DrawLine(220, 180, 40, LCD_DIR_VERTICAL);

	LCD_DisplayStringLine(LCD_LINE_2, sound[3]);
	LCD_DisplayStringLine(LCD_LINE_3, frequency_str);
}

void ui_draw_metronome()
{
	itoa(metronome_bpm, bpm_str, 10);
    
	metronome_beat_count = metronome_beat_count % BEATLIMIT;
    
	itoa(metronome_beat_count, beat_count_str, 10);
    
	LCD_SetColors(LCD_COLOR_BLACK , LCD_COLOR_BLACK);
	LCD_DrawFullEllipse(100, 220, 50, 35);
    
    
	LCD_DrawLine(100 + 50, 40, 180, LCD_DIR_VERTICAL);
	LCD_DrawLine(100 + 50 -1, 40, 180, LCD_DIR_VERTICAL);
	LCD_DrawLine(100 + 50 -2, 40, 180, LCD_DIR_VERTICAL);
	LCD_DrawLine(100 + 50 -3, 40, 180, LCD_DIR_VERTICAL);
	LCD_DrawLine(100 + 50 -4, 40, 180, LCD_DIR_VERTICAL);
        
	LCD_SetColors(LCD_COLOR_MAGENTA , LCD_COLOR_WHITE);
        
	LCD_DisplayStringLine(LCD_LINE_2, bpm_text_str);
	LCD_DisplayStringLine(LCD_LINE_3, bpm_str);
        
	LCD_DisplayStringLine(LCD_LINE_5, beat_text_str);
	LCD_DisplayStringLine(LCD_LINE_6, beat_count_str);
}

void ui_task()
{
	/* Clear the screen */
	LCD_Clear(LCD_COLOR_WHITE);

	while(1) {
		int cur_mode = mode;

		switch(mode) {
		    case TUNER_MODE:
			ui_draw_tuner();
			break;
		    case METRONOME_MODE:
			ui_draw_metronome();
			break;
		}

		if(cur_mode != mode) {
			/* Mode switched */
			LCD_Clear(LCD_COLOR_WHITE); //Clear the screen
			sleep(200);
		}
	}
}

void ui_init()
{

    LCD_Init();

    LCD_LayerInit();

    LTDC_Cmd(ENABLE);

    LCD_DisplayOn();

    LCD_SetLayer(LCD_FOREGROUND_LAYER);
    LCD_SetTransparency(0x00);

    LCD_SetLayer(LCD_BACKGROUND_LAYER);
}
