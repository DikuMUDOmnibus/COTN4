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

#if defined(macintosh)
#include <types.h>
#else
#include <sys/types.h>
#endif
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "merc.h"
#include "globals.h"
#include "proto.h"

#define WOLLA(fun)   void fun (void)

WOLLA ( dailyevents )
{ 
/* Basic events that have their own chance check */

      if ( its_christmas || special_day )
      {
         global_exp = TRUE;
         double_bones = TRUE;
         double_damage = TRUE;
         double_stance = TRUE;
         double_pracs = TRUE;
      }
      else
      {
        event_doublexp     (  );
        event_doublebones     (  );
        event_doubledamage (  );
        event_doublestance (  );
        event_doublepracs  (  );
      }
      
/* Random pill and angel/demon token spreading */
        random_spread (  );
      
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

/* Auto Freezetag Game */
    if ( bigchance ( 3 ) )
	auto_tag (  );

/* Stock Market Crash */
    if ( bigchance ( 1 ) )
	event_marketcrash (  );
}

WOLLA ( event_doublexp )
{
    if ( (!global_exp) && ( bigchance ( 2 ) ) ) 
    { 
	global_exp = TRUE;
	do_gmessage ( "{R[{WEvent{R] {CIt is time for doublexp! Get killing!{x\n\r" );
	return;
    }
    else if ( (global_exp) && ( chance ( 25 ) ) ) 
    { 
	global_exp = FALSE;
	do_gmessage ( "{R[{WEvent{R] {CDoublexp is over.{x\n\r" );
	return;
    }
    return;
} 

WOLLA ( event_doublebones )
{
    if ( (!double_bones) && ( bigchance ( 2 ) ) ) 
    { 
	double_bones = TRUE;
	do_gmessage ( "{R[{WEvent{R] {CIt is time for double bones!{x\n\r" );
	return;
    }
    else if ( (double_bones) && ( chance ( 25 ) ) ) 
    { 
	double_bones = FALSE;
	do_gmessage ( "{R[{WEvent{R] {CDouble bones is over.{x\n\r" );
	return;
    }
    return;
} 

WOLLA ( event_doubledamage )
{
    if ( (!double_damage) && ( bigchance ( 2 ) ) ) 
    { 
	double_damage = TRUE;
	do_gmessage ( "{R[{WEvent{R] {CIt is time for double damage! Get smashing!{x\n\r" );
	return;
    }
    else if ( (double_damage) && ( chance ( 25 ) ) ) 
    { 
	double_damage = FALSE;
	do_gmessage ( "{R[{WEvent{R] {CDouble damage is over.{x\n\r" );
	return;
    }
    return;
} 

WOLLA ( event_doublestance )
{
    if ( (!double_stance) && ( bigchance ( 2 ) ) ) 
    { 
	double_stance = TRUE;
	do_gmessage ( "{R[{WEvent{R] {CIt is time for double stance gain! Get fighting!{x\n\r" );
	return;
    }
    else if ( (double_stance) && ( chance ( 25 ) ) ) 
    { 
	double_stance = FALSE;
	do_gmessage ( "{R[{WEvent{R] {CDouble stance gain is over.{x\n\r" );
	return;
    }
    return;
} 

WOLLA ( event_doublepracs )
{
    if ( (!double_pracs) && ( bigchance ( 2 ) ) ) 
    { 
	double_pracs = TRUE;
	do_gmessage ( "{R[{WEvent{R] {CIt is time for double practice gain!{x\n\r" );
	return;
    }
    else if ( (double_pracs) && ( chance ( 25 ) ) ) 
    { 
	double_pracs = FALSE;
	do_gmessage ( "{R[{WEvent{R] {CDouble practice gain is over.{x\n\r" );
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
    send_to_char ( "{*I{*t{*'{*s {*H{*a{*p{*p{*y {*M{*o{*m{*e{*n{*t{*!{*!{*!{x\n\r", d->character );
    send_to_char ( "{*I{*t{*'{*s {*H{*a{*p{*p{*y {*M{*o{*m{*e{*n{*t{*!{*!{*!{x\n\r", d->character );
    send_to_char ( "{*I{*t{*'{*s {*H{*a{*p{*p{*y {*M{*o{*m{*e{*n{*t{*!{*!{*!{x\n\r", d->character );
    send_to_char ( "{*I{*t{*'{*s {*H{*a{*p{*p{*y {*M{*o{*m{*e{*n{*t{*!{*!{*!{x\n\r", d->character );
    send_to_char ( "{*I{*t{*'{*s {*H{*a{*p{*p{*y {*M{*o{*m{*e{*n{*t{*!{*!{*!{x\n\r", d->character );
    send_to_char ( "{*I{*t{*'{*s {*H{*a{*p{*p{*y {*M{*o{*m{*e{*n{*t{*!{*!{*!{x\n\r\n\r", d->character );

     if ( ( d->character->level < HERO ) && !IS_SET ( d->character->act2, PLR2_NO_EXP ) )
     {
       if ( chance ( 70 ) )
       {
         happymomentbonus = number_range ( 100, 10000 );
         gain_exp ( d->character, happymomentbonus );
         printf_to_char ( d->character, "Free {Yexp{x!!! {r({R%ld{r){x (If you want to avoid this, type noexp)\n\r", happymomentbonus );
       }
       else send_to_char ( "Sorry, you didn't get any {Yexp{x this time.\n\r", d->character );
      }

    if ( chance ( 60 ) )
    {
     happymomentbonus = number_range ( 1, 300 );
     d->character->platinum += happymomentbonus;
     printf_to_char ( d->character, "Your {Yplatinum{x amount increases by {r({R%ld{r){x!!\n\r", happymomentbonus );
    }
    else send_to_char ( "Sorry, you didn't get any {Yplatinum{x this time.\n\r", d->character );

    if ( chance ( 30 ) )
    {
     happymomentbonus = number_range ( 1, 5 );
     d->character->bones += happymomentbonus;
     printf_to_char ( d->character, "Your {Yiqp{x amount increases by {r({R%ld{r){x!!\n\r", happymomentbonus );
    }
    else send_to_char ( "Sorry, you didn't get any {Yiqps{x this time.\n\r", d->character );

    if ( chance ( 40 ) )
    {
     happymomentbonus = number_range ( 10, 1000 );
     d->character->bones += happymomentbonus;
     printf_to_char ( d->character, "Your {Yaqp{x amount increases by {r({R%ld{r){x!!\n\r", happymomentbonus );
    }
    else send_to_char ( "Sorry, you didn't get any {Yaqps{x this time.\n\r", d->character );

    if ( chance ( 10 ) )
    {
     happymomentbonus = number_range ( 1, 100 );
     d->character->max_hit += happymomentbonus;
     printf_to_char ( d->character, "Your {Yhp{x amount increases by {r({R%ld{r){x!!\n\r", happymomentbonus );
    }
    else send_to_char ( "Sorry, you didn't get any {Yhps{x this time.\n\r", d->character );

    if ( chance ( 12 ) )
    {
     happymomentbonus = number_range ( 1, 200 );
     d->character->max_mana += happymomentbonus;
     printf_to_char ( d->character, "Your {Ymana{x amount increases by {r({R%ld{r){x!!\n\r", happymomentbonus );
    }
    else send_to_char ( "Sorry, you didn't get any {Ymana{x this time.\n\r", d->character );

    if ( chance ( 18 ) )
    {
     happymomentbonus = number_range ( 1, 300 );
     d->character->max_move += happymomentbonus;
     printf_to_char ( d->character, "Your {Ymove{x amount increases by {r({R%ld{r){x!!\n\r", happymomentbonus );
    }
    else send_to_char ( "Sorry, you didn't get any {Ymoves{x this time.\n\r", d->character );

    if ( chance ( 35 ) )
    {
     happymomentbonus = number_range ( 1, 50 );
     d->character->practice += happymomentbonus;
     printf_to_char ( d->character, "Your {Ypractice{x amount increases by {r({R%ld{r){x!!\n\r", happymomentbonus );
    }
    else send_to_char ( "Sorry, you didn't get any {Ypractices{x this time.\n\r", d->character );

    if ( chance ( 35 ) )
    {
     happymomentbonus = number_range ( 1, 10 );
     d->character->train += happymomentbonus;
     printf_to_char ( d->character, "Your {Ytrain{x amount increases by {r({R%ld{r){x!!\n\r", happymomentbonus );
    }
    else send_to_char ( "Sorry, you didn't get any {Ytrains{x this time.\n\r", d->character );

    d->character->hit = d->character->max_hit;
    d->character->mana = d->character->max_mana;
    d->character->move = d->character->max_move;
    send_to_char ( "{*The gods have restored you!{x\n\r", d->character );

     if ( d->character->level >= 35 )
     {
        save_char_obj ( d->character );
        send_to_char ( "Your character has been backupped. Please remember to do this frequently.\n\r", d->character );
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
    send_to_char ( "{W   _ __ __                             __\n\r", d->character );
    send_to_char ( "  / )  )  )                          /   ) /                 /\n\r", d->character );
    send_to_char ( "   /  /  /   _   o_   o_   _       /      /         o    ---/---\n\r", d->character );
    send_to_char ( "  /  /  /   /_) /  ) /  ) / )  /  |      /_   o_           /  _ _   __\n\r", d->character );
    send_to_char ( " /  /   )__(___/  (_/  (_/ (__/   |     /  ) /  ) /  /)   /  / ) ) /  |  /)\n\r", d->character );
    send_to_char ( "                          ___/_    )___/  (_/  (_/(_/__)_/(_/ / (_/)_/(_/__){x\n\r", d->character );

     if ( ( d->character->level < HERO ) && !IS_SET ( d->character->act2, PLR2_NO_EXP ) )
     {
       if ( chance ( 70 ) )
       {
         happymomentbonus = number_range ( 100, 20000 );
         gain_exp ( d->character, happymomentbonus );
         printf_to_char ( d->character, "Free {Yexp{x!!! {r({R%ld{r){x (If you want to avoid this, type noexp)\n\r", happymomentbonus );
       }
       else send_to_char ( "Sorry, you didn't get any {Yexp{x this time.\n\r", d->character );
      }

    if ( chance ( 60 ) )
    {
     happymomentbonus = number_range ( 1, 150 );
     d->character->platinum += happymomentbonus;
     printf_to_char ( d->character, "Your {Yplatinum{x amount increases by {r({R%ld{r){x!!\n\r", happymomentbonus );
    }
    else send_to_char ( "Sorry, you didn't get any {Yplatinum{x this time.\n\r", d->character );

    if ( chance ( 30 ) )
    {
     happymomentbonus = number_range ( 1, 2 );
     d->character->questpoints += happymomentbonus;
     printf_to_char ( d->character, "Your {Yiqp{x amount increases by {r({R%ld{r){x!!\n\r", happymomentbonus );
    }
    else send_to_char ( "Sorry, you didn't get any {Yiqps{x this time.\n\r", d->character );

    if ( chance ( 40 ) )
    {
     happymomentbonus = number_range ( 10, 350 );
     d->character->pcdata->questpoints += happymomentbonus;
     printf_to_char ( d->character, "Your {Yaqp{x amount increases by {r({R%ld{r){x!!\n\r", happymomentbonus );
    }
    else send_to_char ( "Sorry, you didn't get any {Yaqps{x this time.\n\r", d->character );

    if ( chance ( 10 ) )
    {
     happymomentbonus = number_range ( 1, 20 );
     d->character->max_hit += happymomentbonus;
     printf_to_char ( d->character, "Your {Yhp{x amount increases by {r({R%ld{r){x!!\n\r", happymomentbonus );
    }
    else send_to_char ( "Sorry, you didn't get any {Yhps{x this time.\n\r", d->character );

    if ( chance ( 12 ) )
    {
     happymomentbonus = number_range ( 1, 30 );
     d->character->max_mana += happymomentbonus;
     printf_to_char ( d->character, "Your {Ymana{x amount increases by {r({R%ld{r){x!!\n\r", happymomentbonus );
    }
    else send_to_char ( "Sorry, you didn't get any {Ymana{x this time.\n\r", d->character );

    if ( chance ( 18 ) )
    {
     happymomentbonus = number_range ( 1, 40 );
     d->character->max_move += happymomentbonus;
     printf_to_char ( d->character, "Your {Ymove{x amount increases by {r({R%ld{r){x!!\n\r", happymomentbonus );
    }
    else send_to_char ( "Sorry, you didn't get any {Ymoves{x this time.\n\r", d->character );

    if ( chance ( 35 ) )
    {
     happymomentbonus = number_range ( 1, 20 );
     d->character->practice += happymomentbonus;
     printf_to_char ( d->character, "Your {Ypractice{x amount increases by {r({R%ld{r){x!!\n\r", happymomentbonus );
    }
    else send_to_char ( "Sorry, you didn't get any {Ypractices{x this time.\n\r", d->character );

    if ( chance ( 35 ) )
    {
     happymomentbonus = number_range ( 1, 5 );
     d->character->train += happymomentbonus;
     printf_to_char ( d->character, "Your {Ytrain{x amount increases by {r({R%ld{r){x!!\n\r", happymomentbonus );
    }
    else send_to_char ( "Sorry, you didn't get any {Ytrains{x this time.\n\r", d->character );

    d->character->hit = d->character->max_hit;
    d->character->mana = d->character->max_mana;
    d->character->move = d->character->max_move;
    send_to_char ( "{*The gods have restored you!{x\n\r", d->character );
   }
 }
}
