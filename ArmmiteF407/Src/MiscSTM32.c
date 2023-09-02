/***********************************************************************************************************************
MMBasic

MiscMX470.c

Handles the a few miscellaneous functions for the MX470 version.

Copyright 2011 - 2019 Geoff Graham.  All Rights Reserved.

This file and modified versions of this file are supplied to specific individuals or organisations under the following
provisions:

- This file, or any files that comprise the MMBasic source (modified or not), may not be distributed or copied to any other
  person or organisation without written permission.

- Object files (.o and .hex files) generated using this file (modified or not) may not be distributed or copied to any other
  person or organisation without written permission.

- This file is provided in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

************************************************************************************************************************/


#include "MMBasic_Includes.h"
#include "Hardware_Includes.h"

extern void SetBacklight(int intensity);
//extern void CallCFuncT5(void);                                      // this is implemented in CFunction.c
//extern unsigned int CFuncT5;                                        // we should call the CFunction T5 interrupt function if this is non zero
extern void ConfigSDCard(char *p);
volatile MMFLOAT VCC=3.3;
extern volatile int ConsoleTxBufHead;
extern volatile int ConsoleTxBufTail;
#define GenSPI hspi2
extern SPI_HandleTypeDef GenSPI;
extern void initKeyboard(void);
extern void MIPS16 InitTouch(void);
extern int CurrentSPISpeed;
extern char LCDAttrib;
extern RTC_HandleTypeDef hrtc;
extern TIM_HandleTypeDef htim1;
extern void setterminal(void);


///////////////////////////////////////////////////////////////////////////////////////////////
// constants and functions used in the OPTION LIST command
char *LCDList[] = {"","VGA","SSD1963_5ER_16", "SSD1963_7ER_16",  //0-3
		"SSD1963_4_16", "SSD1963_5_16", "SSD1963_5A_16", "SSD1963_7_16", "SSD1963_7A_16", "SSD1963_8_16",  //4-9 SSD P16 displays
		"USER",//10
		"ST7735","ILI9431_I","ST7735S","","ILI9481IPS","ILI9163", "GC9A01", "ST7789","ILI9488", "ILI9481", "ILI9341", "",      //11-22 SPI
		  "ILI9341_16", "ILI9486_16", "", "IPS_4_16", ""    //23-27 P16 displays
		 };
const char *OrientList[] = {"", "LANDSCAPE", "PORTRAIT", "RLANDSCAPE", "RPORTRAIT"};
const char *CaseList[] = {"", "LOWER", "UPPER"};
const char *KBrdList[] = {"", "US", "FR", "GR", "IT", "BE", "UK", "ES" };

void PRet(void){
	MMPrintString("\r\n");
}


void PO(char *s) {
    MMPrintString("OPTION "); MMPrintString(s); MMPrintString(" ");
}

void PInt(int n) {
    char s[20];
    IntToStr(s, (int64_t)n, 10);
    MMPrintString(s);
}

void PIntComma(int n) {
    MMPrintString(", "); PInt(n);
}

void PO2Str(char *s1, const char *s2) {
    PO(s1); MMPrintString((char *)s2); PRet();
}


void PO2Int(char *s1, int n) {
    PO(s1); PInt(n); PRet();
}

void PO3Int(char *s1, int n1, int n2) {
    PO(s1); PInt(n1); PIntComma(n2); PRet();
}
void PIntH(unsigned int n) {
    char s[20];
    IntToStr(s, (int64_t)n, 16);
    MMPrintString(s);
}
void PIntHC(unsigned int n) {
    MMPrintString(", "); PIntH(n);
}

void PFlt(MMFLOAT flt){
	   char s[20];
	   FloatToStr(s, flt, 4,4, ' ');
	    MMPrintString(s);
}
void PFltComma(MMFLOAT n) {
    MMPrintString(", "); PFlt(n);
}
void PPinName(int n){
	char s[3]="Px";
	int pn=0,pp=1;
	if(PinDef[n].sfr==GPIOA)s[1]='A';
	else if(PinDef[n].sfr==GPIOB)s[1]='B';
	else if(PinDef[n].sfr==GPIOC)s[1]='C';
	else if(PinDef[n].sfr==GPIOD)s[1]='D';
	else if(PinDef[n].sfr==GPIOE)s[1]='E';
	while(PinDef[n].bitnbr!=pp){pp<<=1;pn++;}
	if(s[1]!='x'){
		MMPrintString(s);
		PInt(pn);
	} else PInt(n);
}
void PPinNameComma(int n) {
    MMPrintString(", "); PPinName(n);

}
///////////////////////////////////////////////////////////////////////////////////////////////

void printoptions(void){
//	LoadOptions();

    MMPrintString("\rARMmite F407 MMBasic Version " VERSION "\r\n");

    if(Option.Autorun == true) PO2Str("AUTORUN", "ON");
    if(Option.Baudrate != CONSOLE_BAUDRATE) PO2Int("BAUDRATE", Option.Baudrate);
    if(Option.Restart > 0) PO2Int("RESTART", Option.Restart);
   // if(Option.Invert == 2) PO2Str("CONSOLE", "AUTO");
    if(Option.ColourCode == true) PO2Str("COLOURCODE", "ON");
    if(Option.Listcase != CONFIG_TITLE) PO2Str("CASE", CaseList[(int)Option.Listcase]);
    if(Option.Tab != 2) PO2Int("TAB", Option.Tab);
    if(Option.Height != 24 || Option.Width != 80) PO3Int("DISPLAY", Option.Height, Option.Width);

    if(Option.DISPLAY_TYPE >= SPI_PANEL_START && Option.DISPLAY_TYPE <=  SPI_PANEL_END){
   	    PO("LCDPANEL"); MMPrintString((char *)LCDList[(int)Option.DISPLAY_TYPE]); MMPrintString(", "); MMPrintString((char *)OrientList[(int)Option.DISPLAY_ORIENTATION]);
        PPinNameComma(Option.LCD_CD); PPinNameComma(Option.LCD_Reset); PPinNameComma(Option.LCD_CS); PRet();
    }
    //if(Option.DISPLAY_TYPE >= SSD_PANEL_START && Option.DISPLAY_TYPE <= SSD_PANEL_END) {
   //     PO("LCDPANEL"); MMPrintString(LCDList[(int)Option.DISPLAY_TYPE]); MMPrintString(", "); MMPrintString((char *)OrientList[(int)Option.DISPLAY_ORIENTATION]);
   //     PRet();
   // }
    if((Option.DISPLAY_TYPE >= SSD_PANEL_START && Option.DISPLAY_TYPE <= SSD_PANEL_END) || (Option.DISPLAY_TYPE>= P16_PANEL_START && Option.DISPLAY_TYPE <= P16_PANEL_END)){
        PO("LCDPANEL"); MMPrintString(LCDList[(int)Option.DISPLAY_TYPE]); MMPrintString(", "); MMPrintString((char *)OrientList[(int)Option.DISPLAY_ORIENTATION]);
        PRet();
    }
    if(Option.DISPLAY_TYPE == USER ){
    	PO("LCDPANEL"); MMPrintString(LCDList[(int)Option.DISPLAY_TYPE]);
        PIntComma(HRes);
        PIntComma(VRes);
        PRet();
    }
    if(Option.TOUCH_CS) {
        PO("TOUCH"); PPinName(Option.TOUCH_CS); PPinNameComma(Option.TOUCH_IRQ);
        if(Option.TOUCH_Click) PPinNameComma(Option.TOUCH_Click);
        PRet();
        if(Option.TOUCH_XZERO != TOUCH_NOT_CALIBRATED ) {
            MMPrintString("GUI CALIBRATE "); PInt(Option.TOUCH_SWAPXY); PIntComma(Option.TOUCH_XZERO); PIntComma(Option.TOUCH_YZERO);
           PIntComma(Option.TOUCH_XSCALE * 10000); PIntComma(Option.TOUCH_YSCALE * 10000); PRet();
        }
    }
   // if(Option.DefaultBrightness!=100){
    if(HRes != 0){
    	MMPrintString("DEFAULT BACKLIGHT ");
    	if(Option.DefaultBrightness>100){
    		PInt(Option.DefaultBrightness-101);MMPrintString(",R \r\n");
    	}else{
    		PInt(Option.DefaultBrightness);PRet();
    	}
    }

    if(Option.DISPLAY_CONSOLE == true) PO2Str("LCDPANEL", "CONSOLE");
    if(Option.MaxCtrls != 201) PO2Int("CONTROLS", Option.MaxCtrls - 1);
    if(!Option.SerialPullup) PO2Str("SERIAL PULLUPS", "OFF");
    if(!Option.SerialConDisabled) PO2Str("SERIAL CONSOLE", "ON");
    if(Option.KeyboardConfig != NO_KEYBOARD) PO2Str("KEYBOARD", KBrdList[(int)Option.KeyboardConfig]);
    if(Option.RTC_Calibrate)PO2Int("RTC CALIBRATE", Option.RTC_Calibrate);
    if(Option.ProgFlashSize !=0x20000){PO("PROG_FLASH_SIZE");PIntH( Option.ProgFlashSize); PRet();}  //LIBRARY
    if(Option.FLASH_CS != 35){ PO("FLASH_CS"); PInt(Option.FLASH_CS);PRet();}
    if(Option.DefaultFont != 1){ PO("DefaultFont"); PInt(Option.DefaultFont);PRet();}
   // PO3Int("DISPLAY", Option.Height, Option.Width);
    return;

}
void touchdisable(void){
	// if(!(Option.DISPLAY_TYPE==ILI9341 || Option.DISPLAY_TYPE==ILI9481)){
    if(!(Option.DISPLAY_TYPE >= SPI_PANEL_START && Option.DISPLAY_TYPE <= SPI_PANEL_END)){
        HAL_SPI_DeInit(&GenSPI);
		ExtCurrentConfig[SPI2_OUT_PIN]=EXT_DIG_IN;
		ExtCurrentConfig[SPI2_INP_PIN]=EXT_DIG_IN;
		ExtCurrentConfig[SPI2_CLK_PIN]=EXT_DIG_IN;
        ExtCfg(SPI2_OUT_PIN, EXT_NOT_CONFIG, 0);
        ExtCfg(SPI2_INP_PIN, EXT_NOT_CONFIG, 0);
        ExtCfg(SPI2_CLK_PIN, EXT_NOT_CONFIG, 0);
        CurrentSPISpeed=NONE_SPI_SPEED;
    }
	if(Option.TOUCH_CS)ExtCurrentConfig[Option.TOUCH_CS]=EXT_DIG_IN;
	if(Option.TOUCH_IRQ)ExtCurrentConfig[Option.TOUCH_IRQ]=EXT_DIG_IN;
	if(Option.TOUCH_CS)ExtCfg(Option.TOUCH_CS, EXT_NOT_CONFIG, 0);
	if(Option.TOUCH_IRQ)ExtCfg(Option.TOUCH_IRQ, EXT_NOT_CONFIG, 0);
    if(Option.TOUCH_Click!=false){
		ExtCurrentConfig[Option.TOUCH_Click]=EXT_DIG_IN;
        ExtCfg(Option.TOUCH_Click, EXT_NOT_CONFIG, 0);
    }
    Option.TOUCH_Click = Option.TOUCH_CS = Option.TOUCH_IRQ = false;
}



void MIPS16 OtherOptions(void) {
	char *tp, *ttp;

	tp = checkstring(cmdline, "RESET");
	if(tp) {
        ResetAllOptions();
		goto saveandreset;
	}

    tp = checkstring(cmdline, "KEYBOARD");
	if(tp) {
    	//if(CurrentLinePtr) error("Invalid in a program");
		if(checkstring(tp, "DISABLE")){
			Option.KeyboardConfig = NO_KEYBOARD;
			HAL_NVIC_DisableIRQ(EXTI15_10_IRQn);
			ExtCurrentConfig[KEYBOARD_CLOCK]=EXT_DIG_IN;
			ExtCurrentConfig[KEYBOARD_DATA]=EXT_DIG_IN;
	        ExtCfg(KEYBOARD_CLOCK, EXT_NOT_CONFIG, 0);
	        ExtCfg(KEYBOARD_DATA, EXT_NOT_CONFIG, 0);
	        SaveOptions();
	        return;
		}
        else if(checkstring(tp, "US"))	Option.KeyboardConfig = CONFIG_US;
		else if(checkstring(tp, "FR"))	Option.KeyboardConfig = CONFIG_FR;
		else if(checkstring(tp, "GR"))	Option.KeyboardConfig = CONFIG_GR;
		else if(checkstring(tp, "IT"))	Option.KeyboardConfig = CONFIG_IT;
		else if(checkstring(tp, "BE"))	Option.KeyboardConfig = CONFIG_BE;
		else if(checkstring(tp, "UK"))	Option.KeyboardConfig = CONFIG_UK;
		else if(checkstring(tp, "ES"))	Option.KeyboardConfig = CONFIG_ES;
		SaveOptions();
        initKeyboard();
        return;
	}

    tp = checkstring(cmdline, "SERIAL PULLUP");
	if(tp) {
        if(checkstring(tp, "DISABLE"))
            Option.SerialPullup = 0;
        else if(checkstring(tp, "ENABLE"))
            Option.SerialPullup = 1;
        else error("Invalid Command");
        goto saveandreset;
	}
	tp=checkstring(cmdline, "RTC CALIBRATE");
	if(tp){
		Option.RTC_Calibrate=getint(tp,-511,512);
		int up=RTC_SMOOTHCALIB_PLUSPULSES_RESET;
		int calibrate= -Option.RTC_Calibrate;
		if(Option.RTC_Calibrate>0){
			up=RTC_SMOOTHCALIB_PLUSPULSES_SET;
			calibrate=512-Option.RTC_Calibrate;
		}
		HAL_RTCEx_SetSmoothCalib(&hrtc, RTC_SMOOTHCALIB_PERIOD_32SEC, up, calibrate);
		SaveOptions();
		return;
	}

	tp = checkstring(cmdline, "CONTROLS");
    if(tp) {
    	Option.MaxCtrls = getint(tp, 0, 1000) + 1;
        goto saveandreset;
    }

    tp = checkstring(cmdline, "ERROR");
	if(tp) {
		if(checkstring(tp, "CONTINUE")) {
            OptionFileErrorAbort = false;
            return;
        }
		if(checkstring(tp, "ABORT")) {
            OptionFileErrorAbort = true;
            return;
        }
	}

    tp = checkstring(cmdline, "LCDPANEL");
    if(tp) {
       //if(CurrentLinePtr) error("Invalid in a program");      G.A.
        if((ttp = checkstring(tp, "CONSOLE"))) {
            if(HRes == 0) error("LCD Panel not configured");
            if(ScrollLCD == (void (*)(int ))DisplayNotSet) error("No Support on this LCD Panel");
            if(!DISPLAY_LANDSCAPE) error("Landscape only");
            Option.Height = VRes/gui_font_height; Option.Width = HRes/gui_font_width;
            skipspace(ttp);
            Option.DefaultFC = WHITE;
            Option.DefaultBC = BLACK;
           // Option.DefaultBrightness = 100;
            if(!(*ttp == 0 || *ttp == '\'')) {
                getargs(&ttp, 7, ",");                              // this is a macro and must be the first executable stmt in a block
                if(argc > 0) {
                    if(*argv[0] == '#') argv[0]++;                  // skip the hash if used
                   // MMPrintString("SetFont() ");PIntH((int)((getint(argv[0], 1, FONT_BUILTIN_NBR) - 1) << 4) | 1);PRet();
                    SetFont((((getint(argv[0], 1, FONT_BUILTIN_NBR) - 1) << 4) | 1));
                    Option.DefaultFont = (gui_font>>4)+1;
                }
                if(argc > 2) Option.DefaultFC = getint(argv[2], BLACK, WHITE);
                if(argc > 4) Option.DefaultBC = getint(argv[4], BLACK, WHITE);
                if(Option.DefaultFC == Option.DefaultBC) error("Same colours");
                if(argc > 6) {
                	if(Option.DefaultBrightness>100){
                		Option.DefaultBrightness = 101+getint(argv[6], 0, 100);
                	}else{
                		Option.DefaultBrightness = 101+getint(argv[6], 0, 100);
                	}
                	SetBacklight(Option.DefaultBrightness);
                }
            } else {
                SetFont((Option.DefaultFont-1)<<4 | 0x1);   //B7-B4 is fontno.-1 , B3-B0 is scale
            }

            Option.DISPLAY_CONSOLE = 1;
           // Option.ColourCode = true;  //On by default so done change it
            SaveOptions();
            //PromptFont = Option.DefaultFont;
            PromptFont = gui_font;
            PromptFC = Option.DefaultFC;
            PromptBC = Option.DefaultBC;
            ClearScreen(Option.DefaultBC);
            return;
        }
        else if(checkstring(tp, "NOCONSOLE")) {
            Option.Height = SCREENHEIGHT; Option.Width = SCREENWIDTH;
            Option.DISPLAY_CONSOLE = 0;
           // Option.ColourCode = false;   //Don't change as on by default
            Option.DefaultFC = WHITE;
            Option.DefaultBC = BLACK;
            //SetFont((Option.DefaultFont = 0x01));
            Option.DefaultFont=0x01;
            SetFont(((Option.DefaultFont-1)<<4) | 1);
            // Option.DefaultBrightness = 100;
            setterminal();
            SaveOptions();
            ClearScreen(Option.DefaultBC);
            return;
        }
        else if(checkstring(tp, "DISABLE")) {
        	  if(Option.DISPLAY_TYPE >= SPI_PANEL_START && Option.DISPLAY_TYPE<=SPI_PANEL_END){

        	            	if(Option.LCD_CS){
        	            	  ExtCurrentConfig[Option.LCD_CS]=EXT_DIG_IN;
        	            	  ExtCfg(Option.LCD_CS, EXT_NOT_CONFIG, 0);
        	            	}
        	            	ExtCurrentConfig[Option.LCD_CD]=EXT_DIG_IN;
        	            	ExtCfg(Option.LCD_CD, EXT_NOT_CONFIG, 0);
        	            	ExtCurrentConfig[Option.LCD_Reset]=EXT_DIG_IN;
        	            	ExtCfg(Option.LCD_Reset, EXT_NOT_CONFIG, 0);
        	            	Option.LCD_CS = Option.LCD_CD  = Option.LCD_Reset  = 0;
        	            	SaveOptions();

        	 }

        	touchdisable();
        	SaveOptions();
            Option.Height = SCREENHEIGHT; Option.Width = SCREENWIDTH;
            if(Option.DISPLAY_CONSOLE){
               setterminal();
            }

            Option.DISPLAY_CONSOLE = Option.DISPLAY_TYPE = Option.DISPLAY_ORIENTATION = Option.SSDspeed = LCDAttrib = HRes = 0;
            Option.DefaultFC = WHITE; Option.DefaultBC = BLACK; Option.DefaultFont = 0x01;// Option.DefaultBrightness = 100;
            DrawRectangle = (void (*)(int , int , int , int , int )) DisplayNotSet;
            DrawBitmap =  (void (*)(int , int , int , int , int , int , int , unsigned char *)) DisplayNotSet;
            ScrollLCD = (void (*)(int ))DisplayNotSet;
            DrawBuffer = (void (*)(int , int , int , int , char * )) DisplayNotSet;
            ReadBuffer = (void (*)(int , int , int , int , char * )) DisplayNotSet;
        }
        else {
            if(Option.DISPLAY_TYPE) error("Display already configured");
            if(!Option.DISPLAY_TYPE)ConfigDisplaySSD(tp);
            if(!Option.DISPLAY_TYPE) ConfigDisplaySPI(tp);          // if it is not an SSD1963 then try for a SPI type
        }
        SaveOptions();
        return;
    }
    tp = checkstring(cmdline, "TOUCH");
    if(tp) {
    	//if(CurrentLinePtr) error("Invalid in a program");        //G.A.
		if(checkstring(tp, "DISABLE")) {
			touchdisable();
		} else {
            ConfigTouch(tp);
			InitTouch();
        }
        SaveOptions();
        return;
    }
    tp = checkstring(cmdline, "SERIAL CONSOLE");
	if(tp) {
		if(checkstring(tp, "OFF"))		    {
			if(!CurrentLinePtr)MMPrintString("Reset the processor to connect with the USB/CDC port\r\n");
	    	while(ConsoleTxBufTail != ConsoleTxBufHead){}
			Option.SerialConDisabled = true;
			if(!CurrentLinePtr)SaveOptions();    // G.A.
	        return;
		}
		if(checkstring(tp, "ON"))           {
			if(!CurrentLinePtr)MMPrintString("Reset the processor to connect with the serial port J6\r\n");
	    	while(ConsoleTxBufTail != ConsoleTxBufHead){}
			Option.SerialConDisabled = false;
			if(!CurrentLinePtr)SaveOptions();    // G.A.
	        return;
		}
	}
	tp = checkstring(cmdline, "LIST");
    if(tp) {
    	printoptions();
    	return;
    }

    tp = checkstring(cmdline, "FLASH_CS");
           int i;
           if(tp) {
           	i=getint(tp,0,100);
           	if (i!=35 && i!=77 && i!=0) error("Only 0,35 or 77 are valid");
           	Option.FLASH_CS = i ;
           	goto saveandreset;

    }

    tp = checkstring(cmdline, "RESTART");
    if(tp) {
         	Option.Restart = getint(tp, 0, 2) ;
         	SaveOptions();
         	return;
    }

	error("Unrecognised option");

saveandreset:
    // used for options that require the cpu to be reset
   // dont' reset if called in a program G.A.
  if(!CurrentLinePtr) {
    SaveOptions();
    _excep_code = RESTART_NOAUTORUN;                            // otherwise do an automatic reset
	while(ConsoleTxBufTail != ConsoleTxBufHead);
	uSec(10000);
    SoftReset();                                                // this will restart the processor
  }
}





