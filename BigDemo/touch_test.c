/* 
 * File:   picaso.c
 * Author: 4DSystems
 *
 * Created on October 2, 2014, 3:09 PM
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <fenv.h>
#include <ctype.h>
#include <termios.h>
#include <sys/stat.h>
#include <picasoSerial.h>
#include <Picaso_Types4D.h>
#include <Picaso_const4D.h>

/*
 * 
 */

#define SERIALPORT   "/dev/ttyAMA0"
static int comspeed;

int errCallback(int ErrCode, unsigned char Errbyte)
{
	printf("Serial 4D Library reports error %s", Error4DText[ErrCode]);
	if (ErrCode == Err4D_NAK)
		printf(" returned data = 0x%02X\n", Errbyte) ;
	else
		printf("\n") ;

    //Abort on error?
    if (Error_Abort4D)
        exit(ErrCode);

	return ErrCode;
}

int main() 
{
    int rc;
    int firstx ;
    int firsty ;
    int x ;
    int y ;
    int state ;
    
    TimeLimit4D = 2000;
    Callback4D = errCallback;
    // abort on detected 4D Serial error
    Error_Abort4D = 1 ;
    comspeed = 9600;
    //WORD *x = 0x001A;
    //char *s = sys_GetModel(&x);
    //x  = &0x001A;
    rc = OpenComm(SERIALPORT, comspeed);
    if (rc != 0)
    {
        printf("Error %d Opening: %s - %s\n", errno, SERIALPORT, strerror(errno));
        exit(EXIT_FAILURE);
    }

	gfx_Cls() ;
	putStr("Touch Tests\n") ;
	printf("Touch Tests.\n") ;
	putStr("Please ensure Touch is only\ndetected in the Blue area") ;
	printf("Detecting touch in Region\n") ;
	touch_Set(TOUCH_ENABLE) ;
	touch_DetectRegion(100,100, 200, 200) ;
	gfx_RectangleFilled(100,100, 200, 200, BLUE) ;
	do {} while (touch_Get(TOUCH_STATUS) != TOUCH_PRESSED);
	touch_Set(TOUCH_REGIONDEFAULT) ;
	gfx_Cls() ;
	putStr("Draw.. Drawing stops\nwhen touch released\n") ;
	printf("Drawing\n") ;

	while(touch_Get(TOUCH_STATUS) != TOUCH_PRESSED)
	{      // we"ll wait for a touch
	}
	firstx = touch_Get(TOUCH_GETX);                          // so we can get the first point
	firsty = touch_Get(TOUCH_GETY);
	while(state != TOUCH_RELEASED)
	{
		state = touch_Get(TOUCH_STATUS);                       // look for any touch activity
		x = touch_Get(TOUCH_GETX);                             // grab the x
		y = touch_Get(TOUCH_GETY);                             // and the y coordinates of the touch
		if (state == TOUCH_PRESSED)                               // if there"s a press
		{
			firstx = x;
			firsty = y;
		}
		if (state == TOUCH_MOVING)                                // if there"s movement
		{
			gfx_Line(firstx, firsty, x, y, BLUE);                 // but lines are much better
			firstx = x;
			firsty = y;
		}
	}
	putStr("Done!\n") ;
	touch_Set(TOUCH_DISABLE);
    	
    return (EXIT_SUCCESS);
}

