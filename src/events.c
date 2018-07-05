/***************************************************************************
*   Original Diku Mud copyright (C) 1990, 1991 by Sebastian Hammer,        *
*   Michael Seifert, Hans Henrik Stærfeldt, Tom Madsen, and Katja Nyboe.   *
*                                                                          *
*   Merc Diku Mud improvements copyright (C) 1992, 1993 by Michael         *
*   Chastain, Michael Quan, and Mitchell Tse.                              *
*                                                                          *
*       ROM 2.4 is copyright 1993-1995 Russ Taylor                         *
*       ROM has been brought to you by the ROM consortium                  *
*           Russ Taylor (rtaylor@pacinfo.com)                              *
*           Gabrielle Taylor (gtaylor@pacinfo.com)                         *
*           Brian Moore (rom@rom.efn.org)                                  *
*       ROT 2.0 is copyright 1996-1999 by Russ Walsh                       *
*       DRM 1.0a is copyright 2000-2002 by Joshua Chance Blackwell         *
*        SD 2.0 is copyright 2004-2006 by Patrick Mylund Nielsen           *
***************************************************************************/

#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <string.h>
#include <time.h>
#include "merc.h"
#include "interp.h"
#include <unistd.h>

#define WOLLA(fun)   void fun (void)



WOLLA ( dailyevents )
{ 
/* Basic events that have their own chance check */

      if ( its_christmas || special_day )
      {
         global_exp = TRUE;
         global_qp = TRUE;
         global_dt = TRUE;
         double_pracs = TRUE;
 	 global_cp = TRUE;
      }
      else
      {
        event_doublexp     (  );
        event_doublebones     (  );
	event_dt	(  );
        event_doublepracs  (  );
	event_cp	(  );
      }
      
      
/* Happy Moment */
    if ( !its_christmas )
    {
     if ( bigchance ( 3 ) )
	 happymoment (  );
    }
    else
    {
     if ( chance ( 4 ) )
         merrychristmas (  );
    }

}

WOLLA ( event_doublexp )
{
    if ( (!global_exp) && ( bigchance ( 2 ) ) ) 
    { 
	global_exp = TRUE;
	do_gmessage ( "#R[#7Event#R] #CIt is time for doublexp! Get killing!#n\n\r" );
	return;
    }
    else if ( (global_exp) && ( chance ( 25 ) ) ) 
    { 
	global_exp = FALSE;
	do_gmessage ( "#R[#7Event#R] #CDoublexp is over.#n\n\r" );
	return;
    }
    return;
} 

WOLLA ( event_cp )
{
    if ( (!double_bones) && ( bigchance ( 2 ) ) )
    {
        global_cp = TRUE;
        do_gmessage ( "#R[#7Event#R] #CIt is time for double CP!#n\n\r" );
        return;
    }
    else if ( (double_bones) && ( chance ( 25 ) ) )
    {
        global_cp = FALSE;
        do_gmessage ( "#R[#7Event#R] #CDouble CP is over.#n\n\r" );
        return;
    }
    return;
}


WOLLA ( event_doublebones )
{
    if ( (!double_bones) && ( bigchance ( 2 ) ) ) 
    { 
	double_bones = TRUE;
	do_gmessage ( "#R[#7Event#R] #CIt is time for double bones!#n\n\r" );
	return;
    }
    else if ( (double_bones) && ( chance ( 25 ) ) ) 
    { 
	double_bones = FALSE;
	do_gmessage ( "#R[#7Event#R] #CDouble bones is over.#n\n\r" );
	return;
    }
    return;
} 

WOLLA ( event_dt )
{
    if ( (!global_dt) && ( bigchance ( 2 ) ) ) 
    { 
	global_dt = TRUE;
	do_gmessage ( "#R[#7Event#R] #CIt is time for double stance gain! Get fighting!#n\n\r" );
	return;
    }
    else if ( (global_dt) && ( chance ( 25 ) ) ) 
    { 
	global_dt = FALSE;
	do_gmessage ( "#R[#7Event#R] #CDouble stance gain is over.#n\n\r" );
	return;
    }
    return;
} 

WOLLA ( event_doublepracs )
{
    if ( (!double_pracs) && ( bigchance ( 2 ) ) ) 
    { 
	double_pracs = TRUE;
	do_gmessage ( "#R[#7Event#R] #CIt is time for double practice gain!#n\n\r" );
	return;
    }
    else if ( (double_pracs) && ( chance ( 25 ) ) ) 
    { 
	double_pracs = FALSE;
	do_gmessage ( "#R[#7Event#R] #CDouble practice gain is over.#n\n\r" );
	return;
    }
    return;
} 

WOLLA ( happymoment )
{
    CHAR_DATA *ch;
    DESCRIPTOR_DATA *d;
    long  happymomentbonus;

    for ( d = descriptor_list; d != NULL; d = d->next )
    {
     if ( d->connected == CON_PLAYING && ( ch = ( d->original ? d->original : d->character ) ) != NULL )
     {
   {
    send_to_char ( "#DI#st#s'#ss #sH#sa#sp#sp#sy #sM#so#sm#se#sn#st#s!#s!#s!#n\n\r", d->character );
    send_to_char ( "#sI#st#s'#ss #sH#sa#sp#sp#sy #sM#so#sm#se#sn#st#s!#s!#s!#n\n\r", d->character );
    send_to_char ( "#sI#st#s'#ss #sH#sa#sp#sp#sy #sM#so#sm#se#sn#st#s!#s!#s!#n\n\r", d->character );
    send_to_char ( "#sI#st#s'#ss #sH#sa#sp#sp#sy #sM#so#sm#se#sn#st#s!#s!#s!#n\n\r", d->character );
    send_to_char ( "#sI#st#s'#ss #sH#sa#sp#sp#sy #sM#so#sm#se#sn#st#s!#s!#s!#n\n\r", d->character );
    send_to_char ( "#sI#st#s'#ss #sH#sa#sp#sp#sy #sM#so#sm#se#sn#st#s!#s!#s!#n\n\r\n\r", d->character );

     if ( ( d->character->level < LEVEL_HERO ) )
     {
       if ( chance ( 70 ) )
       {
         happymomentbonus = number_range ( 100, 10000 );
         gain_exp ( d->character, happymomentbonus );
         printf_to_char ( d->character, "Free #Yexp#n!!! #R(#R%ld#R)#n (If you want to avoid this, type noexp)\n\r", happymomentbonus );
       }
       else send_to_char ( "Sorry, you didn't get any #Yexp#n this time.\n\r", d->character );
      }

    if ( chance ( 30 ) )
    {
     happymomentbonus = number_range ( 1, 5 );
     d->character->bones += happymomentbonus;
     printf_to_char ( d->character, "Your #Ybones#n amount increases by #R(#R%ld#R)#n!!\n\r",happymomentbonus );
    }
    else send_to_char ( "Sorry, you didn't get any #YBones#n this time.\n\r", d->character );

    if ( chance ( 40 ) )
    {
     happymomentbonus = number_range ( 10, 1000 );
     d->character->practice += happymomentbonus;
     printf_to_char ( d->character, "Your #YPrimal#n amount increases by #R(#R%ld#R)#n!!\n\r", happymomentbonus );
    }
    else send_to_char ( "Sorry, you didn't get any #YPrimal#n this time.\n\r", d->character );

    if ( chance ( 10 ) )
    {
     happymomentbonus = number_range ( 1, 100 );
     d->character->max_hit += happymomentbonus;
     printf_to_char ( d->character, "Your #Yhp#n amount increases by #R(#R%ld#R)#n!!\n\r", happymomentbonus );
    }
    else send_to_char ( "Sorry, you didn't get any #Yhps#n this time.\n\r", d->character );

    if ( chance ( 12 ) )
    {
     happymomentbonus = number_range ( 1, 200 );
     d->character->max_mana += happymomentbonus;
     printf_to_char ( d->character, "Your #Ymana#n amount increases by #R(#R%ld#R)#n!!\n\r", happymomentbonus );
    }
    else send_to_char ( "Sorry, you didn't get any #Ymana#n this time.\n\r", d->character );

    if ( chance ( 18 ) )
    {
     happymomentbonus = number_range ( 1, 300 );
     d->character->max_move += happymomentbonus;
     printf_to_char ( d->character, "Your #Ymove#n amount increases by #R(#R%ld#R)#n!!\n\r", happymomentbonus );
    }
    else send_to_char ( "Sorry, you didn't get any #Ymoves#n this time.\n\r", d->character );

    if ( chance ( 35 ) )
    {
     happymomentbonus = number_range ( 1, 50 );
     d->character->pcpractice += happymomentbonus;
     printf_to_char ( d->character, "Your #Ypractice#n amount increases by #R(#R%ld#R)#n!!\n\r", happymomentbonus );
    }
    else send_to_char ( "Sorry, you didn't get any #Ypractices#n this time.\n\r", d->character );

    if ( chance ( 35 ) )
    {
     happymomentbonus = number_range ( 1, 10 );
     d->character->practice += happymomentbonus;
     printf_to_char ( d->character, "Your #Ytrain#n amount increases by #R(#R%ld#R)#n!!\n\r", happymomentbonus );
    }
    else send_to_char ( "Sorry, you didn't get any #Ytrains#n this time.\n\r", d->character );

    d->character->hit = d->character->max_hit;
    d->character->mana = d->character->max_mana;
    d->character->move = d->character->max_move;
    send_to_char ( "#sThe gods have restored you!#n\n\r", d->character );

     if ( d->character->level >= 35 )
     {
        save_char_obj ( d->character );
        send_to_char ( "Your character has been backupped. Please remember to do this frequently.\n\r", d->character );
     }
   }
  }
 }
}

WOLLA ( merrychristmas )
{
    CHAR_DATA *ch;
    DESCRIPTOR_DATA *d;
    long  happymomentbonus;

    for ( d = descriptor_list; d != NULL; d = d->next )
    {
     if ( d->connected == CON_PLAYING && ( ch = ( d->original ? d->original : d->character ) ) != NULL )
     {
//   if ( !IS_SET ( d->character->comm, COMM_AFK ) )
//   {
    send_to_char ( "#7   _ __ __                             __\n\r", d->character );
    send_to_char ( "  / )  )  )                          /   ) /                 /\n\r", d->character );
    send_to_char ( "   /  /  /   _   o_   o_   _       /      /         o    ---/---\n\r", d->character );
    send_to_char ( "  /  /  /   /_) /  ) /  ) / )  /  |      /_   o_           /  _ _   __\n\r", d->character );
    send_to_char ( " /  /   )__(___/  (_/  (_/ (__/   |     /  ) /  ) /  /)   /  / ) ) /  |  /)\n\r", d->character );
    send_to_char ( "                          ___/_    )___/  (_/  (_/(_/__)_/(_/ / (_/)_/(_/__)#n\n\r", d->character );

     if ( ( d->character->level < 4 ))
     {
       if ( chance ( 70 ) )
       {
         happymomentbonus = number_range ( 100, 20000 );
         gain_exp ( d->character, happymomentbonus );
         printf_to_char ( d->character, "Free #Yexp#n!!! #R(#R%ld#R)#n (If you want to avoid this, type noexp)\n\r", happymomentbonus );
       }
       else send_to_char ( "Sorry, you didn't get any #Yexp#n this time.\n\r", d->character );
      }

    if ( chance ( 30 ) )
    {
     happymomentbonus = number_range ( 1, 5 );
     d->character->bones += happymomentbonus;
     printf_to_char ( d->character, "Your #Ybones#n amount increases by #R(#R%ld#R)#n!!\n\r",happymomentbonus );
    }
    else send_to_char ( "Sorry, you didn't get any #YBones#n this time.\n\r", d->character );

    if ( chance ( 40 ) )
    {
     happymomentbonus = number_range ( 10, 1000 );
     d->character->practice += happymomentbonus;
     printf_to_char ( d->character, "Your #YPrimal#n amount increases by #R(#R%ld#R)#n!!\n\r", happymomentbonus );
    }
    else send_to_char ( "Sorry, you didn't get any #YPrimal#n this time.\n\r", d->character );

    if ( chance ( 10 ) )
    {
     happymomentbonus = number_range ( 1, 20 );
     d->character->max_hit += happymomentbonus;
     printf_to_char ( d->character, "Your #Yhp#n amount increases by #R(#R%ld#R)#n!!\n\r", happymomentbonus );
    }
    else send_to_char ( "Sorry, you didn't get any #Yhps#n this time.\n\r", d->character );

    if ( chance ( 12 ) )
    {
     happymomentbonus = number_range ( 1, 30 );
     d->character->max_mana += happymomentbonus;
     printf_to_char ( d->character, "Your #Ymana#n amount increases by #R(#R%ld#R)#n!!\n\r", happymomentbonus );
    }
    else send_to_char ( "Sorry, you didn't get any #Ymana#n this time.\n\r", d->character );

    if ( chance ( 18 ) )
    {
     happymomentbonus = number_range ( 1, 40 );
     d->character->max_move += happymomentbonus;
     printf_to_char ( d->character, "Your #Ymove#n amount increases by #R(#R%ld#R)#n!!\n\r", happymomentbonus );
    }
    else send_to_char ( "Sorry, you didn't get any #Ymoves#n this time.\n\r", d->character );

    if ( chance ( 35 ) )
    {
     happymomentbonus = number_range ( 1, 20 );
     d->character->pcpractice += happymomentbonus;
     printf_to_char ( d->character, "Your #Ypractice#n amount increases by #R(#R%ld#R)#n!!\n\r", happymomentbonus );
    }
    else send_to_char ( "Sorry, you didn't get any #Ypractices#n this time.\n\r", d->character );

    if ( chance ( 35 ) )
    {
     happymomentbonus = number_range ( 1, 5 );
     d->character->practice += happymomentbonus;
     printf_to_char ( d->character, "Your #Ytrain#n amount increases by #R(#R%ld#R)#n!!\n\r", happymomentbonus );
    }
    else send_to_char ( "Sorry, you didn't get any #Ytrains#n this time.\n\r", d->character );

    d->character->hit = d->character->max_hit;
    d->character->mana = d->character->max_mana;
    d->character->move = d->character->max_move;
    send_to_char ( "#sThe gods have restored you!#n\n\r", d->character );
   }
  
 }
}
