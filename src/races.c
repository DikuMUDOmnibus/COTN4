#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <assert.h>
#include <unistd.h>
#include <string.h>
#include "merc.h"

void do_eagleeye(CHAR_DATA *ch, char *argument)
{
    CHAR_DATA *victim;
    DESCRIPTOR_DATA *d;
    char      buf [MAX_STRING_LENGTH];
	if (IS_NPC(ch)) return;
	
	if (!IS_RACE(ch, RACE_GRIFFIN))
	{
		send_to_char("#0You are not a GRIFFIN.#n\n\r", ch);
		return;
	}
	send_to_char("#CYou experience the world through your eagle eye.#n\n\r", ch);
	for (d = descriptor_list; d != NULL; d = d->next)
    {
        if (d->connected != CON_PLAYING) continue;
        if ((victim = d->character) == NULL) continue;
        if (IS_NPC(victim) || victim->in_room == NULL) continue;
        if (victim == ch) continue;
        if (!can_see(ch,victim)) continue;
        if (victim->level > 6) continue;
        sprintf(buf,"#n%-14s - %s\n\r", victim->name, victim->in_room->name);
        send_to_char(buf,ch);
	WAIT_STATE(ch, 36);
   }
}

void do_griffinform( CHAR_DATA *ch, char *argument )
{
    char arg [MAX_INPUT_LENGTH];
    char buf [MAX_STRING_LENGTH]; 
    argument = one_argument( argument, arg ); 

    if (IS_NPC(ch)) return;

    if (!IS_RACE(ch, RACE_GRIFFIN))
    {
	send_to_char("#0Huh?#n\n\r",ch);
	return;
    }

    if (IS_POLYAFF(ch, POLY_ZULOFORM))
    {   
        clear_stats(ch);
        REMOVE_BIT(ch->polyaff, POLY_ZULOFORM);
        REMOVE_BIT(ch->affected_by, AFF_POLYMORPH);
        act( "#0You transform into human form.#n", ch, NULL, NULL, TO_CHAR );
        act( "#7$n's #0shrinks back into human form.#n", ch, NULL, NULL, 
TO_ROOM);
        ch->damroll -= 500;
        ch->hitroll -= 500;
        ch->armor   += 300;
	free_string( ch->morph );
        ch->morph = str_dup( "" );
        return;
    }
    else if (IS_AFFECTED(ch,AFF_POLYMORPH))
    {
	send_to_char("#0You cant griffin form when changed.#n\n\r",ch);
	return;
	}
  
  
        if (ch->stance[0] != -1) do_stance(ch,"");
	if (ch->mounted == IS_RIDING) do_dismount(ch,"");
        clear_stats(ch);
        act( "#0You transform into the lion with the head and wings of an eagle.#n", ch, NULL, NULL, TO_CHAR);
        act( "#7$n's #0body shifts into the lion with the head and wings of an eagle.#n", ch, NULL, NULL, TO_ROOM );
        SET_BIT(ch->polyaff, POLY_ZULOFORM);
        SET_BIT(ch->affected_by, AFF_POLYMORPH);
        sprintf(buf, "%s the Griffin", ch->name);
        free_string( ch->morph );
        ch->morph = str_dup( buf );
	ch->damroll += 500;
	ch->hitroll += 500;
        ch->armor   -= 300;
        return;
    }

/*
void do_phase(CHAR_DATA * ch, char *argument)
{
        if (IS_NPC(ch))
                return;
        if (!IS_RACE(ch, RACE_SPECTRE))
        {
                send_to_char("#0Your not a spectre!#n\n\r", ch);
                return;
        }
        if (ch->pcdata->powers[PHASE_COUNTER] > 0)
        {
                send_to_char("Your to tired to phase.\n\r", ch);
                return;
        }
        ch->pcdata->powers[PHASE_COUNTER] = 10;
        act("You phase halfway out of existance.", ch, NULL, NULL, TO_CHAR);
        act("$n phases halfway out of existance.", ch, NULL, NULL, TO_ROOM);
        return;
}
*/

void do_frage(CHAR_DATA * ch, char *argument)
{
        CHAR_DATA *victim;

        if (IS_NPC(ch))
                return;
        if (!IS_RACE(ch, RACE_FENRIS))
        {
                send_to_char("Huh?\n\r", ch);
                return;
        }
        if (ch->mana < 2500)
        {
                send_to_char("This costs 2500 mana.\n\r", ch);
                return;
        }
        if ((victim = ch->fighting) == NULL)
        {
                send_to_char("You aren't fighting anyone.\n\r", ch);
                return;
        }
        if (is_safe(ch, victim)) return;
        WAIT_STATE(ch, 18);
        ch->mana -= 2500;
        act("You enter limbo, steps back into reality behind $N and attack.",
            ch, NULL, victim, TO_CHAR);
        act("$n vanishes and suddenly someone strikes you from behind.", ch,
            NULL, victim, TO_VICT);
        act("$n vanishes and appear behind $N striking $S in the back.", ch,
            NULL, victim, TO_NOTVICT);
        multi_hit(ch, victim, TYPE_UNDEFINED);
//        multi_hit(ch, victim, TYPE_UNDEFINED);
        return;
}

void do_centaurattack(CHAR_DATA * ch, char *argument)
{
        char      arg[MAX_INPUT_LENGTH];
        CHAR_DATA *victim;
        OBJ_DATA *obj;

        one_argument(argument, arg);

        if (IS_NPC(ch))
                return;

        if (!IS_RACE(ch, RACE_CENTAUR))
        {
                send_to_char("#0This is a CENTAUR Ability#n\n\r", ch);
                return;
        }

        if (arg[0] == '\0')
        {
                send_to_char("#0attack whom?#n\n\r", ch);
                return;
        }

        if ((victim = get_char_room(ch, NULL, arg)) == NULL)
        {
                send_to_char("#0They aren't here.#n\n\r", ch);
                return;
        }

        if (victim == ch)
        {
                send_to_char("#0No attacking yourself#n\n\r", ch);
                return;
        }

        if (is_safe(ch, victim)) return;
        if (IS_NPC(victim))
        {
        send_to_char("#cYou attack them with your Hooves!#n\n\r",ch);
        act("#G$n #RAttacks $s opponent with $s his Hooves!!#n", ch, NULL, NULL, TO_ROOM);
                ch->move -= 10000;
                WAIT_STATE(ch, 3 * PULSE_VIOLENCE);
		    multi_hit(ch,victim,gsn_hooves);
		    multi_hit(ch,victim,gsn_hooves);
		    multi_hit(ch,victim,gsn_hooves);
                return;
        }

        send_to_char("#cYou attack them with your Hooves!#n\n\r",ch);
        act("#G$n #RAttacks $s opponent with $s his Hooves!!#n", ch, NULL, NULL, TO_ROOM);
        ch->move -= 10000;
        WAIT_STATE(ch, 3 * PULSE_VIOLENCE);
        if (number_percent() > 25)
		    multi_hit(ch,victim,gsn_hooves);
        if (number_percent() > 50)
		    multi_hit(ch,victim,gsn_hooves);
        if (number_percent() > 75)
		    multi_hit(ch,victim,gsn_hooves);
        return;
}

void do_sblink(CHAR_DATA * ch, char *argument)
{
        CHAR_DATA *victim;

        if (IS_NPC(ch))
                return;
        if (!IS_RACE(ch, RACE_SPECTRE))
        {
                send_to_char("Huh?\n\r", ch);
                return;
        }
        if (ch->mana < 2500)
        {
                send_to_char("This costs 2500 mana.\n\r", ch);
                return;
        }
        if ((victim = ch->fighting) == NULL)
        {
                send_to_char("You aren't fighting anyone.\n\r", ch);
                return;
        }
        WAIT_STATE(ch, 18);
        ch->mana -= 2500;
        act("You enter limbo, steps back into reality behind $N and attack.",
            ch, NULL, victim, TO_CHAR);
        act("$n vanishes and suddenly someone strikes you from behind.", ch,
            NULL, victim, TO_VICT);
        act("$n vanishes and appear behind $N striking $S in the back.", ch,
            NULL, victim, TO_NOTVICT);
        multi_hit(ch, victim, TYPE_UNDEFINED);
        multi_hit(ch, victim, TYPE_UNDEFINED);
        return;
}

void do_hspit(CHAR_DATA *ch, char *argument)
  {
    CHAR_DATA *victim;
    char arg [MAX_INPUT_LENGTH];
    int sn;
    int level;
    int spelltype; 

    one_argument( argument, arg );

   if (!IS_RACE(ch, RACE_HARPIE))
   {
     send_to_char("#0Huh?#n\n\r",ch);
     return;
   }

if ((victim = get_char_room(ch, NULL, arg)) == NULL)
    {
      send_to_char("#0They arent here Jerky#y.#n\n\r", ch);
      return;
    }
    if (ch == victim)
    {
      send_to_char("#0Why would you spit on yourself#y?#n\n\r", ch);
      return;
    }
    if (is_safe(ch, victim)) return;

    if ((sn = skill_lookup( "web" )) < 0) return;

    spelltype = skill_table[sn].target;
    level = ch->spl[spelltype] * 1.00;
    (*skill_table[sn].spell_fun) ( sn, level, ch, victim );
 
    WAIT_STATE( ch, 10 );

    return;
  }

void do_csting(CHAR_DATA *ch, char *argument)
{
    CHAR_DATA *victim;

   if (!IS_RACE(ch, RACE_CHIMERA))
   {
     send_to_char("#0Huh?#n\n\r",ch);
     return;
   }

    if (argument[0] == '\0')
    {
	stc("#0Who do you wish to unstance?#n\n\r", ch);
	return;
    }

    if ( ( victim = get_char_room(ch, NULL, argument) ) == NULL)
    {
	stc("#0They aren't here.#n\n\r", ch);
	return;
    }
    if (is_safe(ch, victim)) return;
    WAIT_STATE(ch, 10);
    do_say(ch,"#GThe sweet smell of #rBLOOD#7!#n");
    do_stance(victim, "");
}

void do_turnhead(CHAR_DATA * ch, char *argument)
{
        char      arg[MAX_INPUT_LENGTH];
        CHAR_DATA *victim;

        one_argument(argument, arg);

        if (IS_NPC(ch))
                return;

        if (!IS_RACE(ch, RACE_MEDUSA) && !IS_RACE(ch, RACE_NYMPH))
        {
                send_to_char("#0This is a MEDUSA or NYMPH Ability#n\n\r", ch);
                return;
        }

        if (arg[0] == '\0')
        {
                send_to_char("#0attack whom?#n\n\r", ch);
                return;
        }

        if ((victim = get_char_room(ch, NULL, arg)) == NULL)
        {
                send_to_char("#0They aren't here.#n\n\r", ch);
                return;
        }

        if (victim == ch)
        {
                send_to_char("#0No attacking yourself#n\n\r", ch);
                return;
        }
        if (is_safe(ch, victim)) return;
        send_to_char("#cYou turn their head and force them to look at you!#n\n\r",ch);
        act("#G$n #RAttacks $s opponent $s by turning his head!#n", ch, NULL, NULL, TO_ROOM);
			  do_look(victim, ch);

        return;
}


void do_spectouch( CHAR_DATA *ch, char *argument )
{
  CHAR_DATA *victim;
  char arg[MAX_INPUT_LENGTH];
  argument = one_argument(argument, arg);

    if (!IS_RACE(ch, RACE_SPECTRE))
    {
	send_to_char( "#0Huh?#n\n\r",ch);
	return;
    }


    if (arg[0] == '\0')
    {
        send_to_char( "Deathtouch whom?\n\r", ch );
        return;
    }


    if ( ( victim = get_char_room( ch, NULL, arg ) ) == NULL )
    {
        send_to_char( "They aren't here.\n\r", ch );
        return;
    }

    if ( victim == ch )
    {
        send_to_char( "How can you deathtouch yourself?\n\r", ch );
        return;
    }


    if ( victim->hit < victim->max_hit )
    {
        act( "$N is hurt and suspicious ... you can't sneak up.",
            ch, NULL, victim, TO_CHAR );
        return;
    }

    if (!IS_SET(ch->affected_by, AFF_ETHEREAL))
    {
      send_to_char("You must be Ethereal to Deathtouch!\n\r",ch);
      return;
    }

    if (is_safe(ch,victim)) return;
    WAIT_STATE(ch,15);
    multi_hit(ch, victim, TYPE_UNDEFINED);
    multi_hit(ch, victim, TYPE_UNDEFINED);
    return;
}



void do_chimeraform( CHAR_DATA *ch, char *argument )
{
    char arg [MAX_INPUT_LENGTH];
    char buf [MAX_STRING_LENGTH]; 
    argument = one_argument( argument, arg ); 

    if (IS_NPC(ch)) return;

    if (!IS_RACE(ch, RACE_CHIMERA))
    {
	send_to_char("#0Huh?#n\n\r",ch);
	return;
    }

    if (IS_POLYAFF(ch, POLY_ZULOFORM))
    {   
        REMOVE_BIT(ch->polyaff, POLY_ZULOFORM);
        REMOVE_BIT(ch->affected_by, AFF_POLYMORPH);
        act( "#0You transform into human form.#n", ch, NULL, NULL, TO_CHAR );
        act( "#7$n's #0shrinks back into human form.#n", ch, NULL, NULL, TO_ROOM);
        ch->damroll -= 500;
        ch->hitroll -= 500;
        ch->armor   += 300;
	free_string( ch->morph );
        ch->morph = str_dup( "" );
        return;
    }
    else if (IS_AFFECTED(ch,AFF_POLYMORPH))
    {
	send_to_char("#0You cant chimera form when changed.#n\n\r",ch);
	return;
	}
  
  
        if (ch->stance[0] != -1) do_stance(ch,"");
	if (ch->mounted == IS_RIDING) do_dismount(ch,"");
        act( "#0You transform into the lion with a snakes tail and second goats head.#n", ch, NULL, NULL, TO_CHAR);
        act( "#7$n's #0body shifts into the lion with a snakes tail and second goats head.#n", ch, NULL, NULL, TO_ROOM );
        SET_BIT(ch->polyaff, POLY_ZULOFORM);
        SET_BIT(ch->affected_by, AFF_POLYMORPH);
        sprintf(buf, "%s the Chimera", ch->name);
        free_string( ch->morph );
        ch->morph = str_dup( buf );
	ch->damroll += 500;
	ch->hitroll += 500;
        ch->armor   -= 300;
        return;
    }

void do_mcharge(CHAR_DATA *ch, char *argument)
{
  CHAR_DATA *victim;
  char arg[MAX_INPUT_LENGTH];

  argument = one_argument(argument, arg);
   
  if (IS_NPC(ch)) return;
  if (!IS_RACE(ch, RACE_MINOTAUR))
  {
    send_to_char("#0Huh?#n\n\r",ch);
    return;
  }
  if (ch->move < 2000)
  {
    send_to_char("This costs 2000 move.\n\r",ch);
    return;
  }
  if (arg[0] == '\0')
  {
  if (ch->fighting == NULL)
  {
  stc("Charge whom?\n\r",ch);
  return;
  }
  else
  {
  victim = ch->fighting;
  }
  }
  else if ( ( victim = get_char_room(ch, NULL, arg) ) == NULL )
  { 
    send_to_char( "They are not here?\n\r", ch );
    return;
  }
  if (is_safe(ch,victim)) return;
  WAIT_STATE(ch, 16);
  ch->move -= 2500;
  one_hit(ch,victim,gsn_headbutt,1);
  one_hit(ch,victim,gsn_hooves,1);
  one_hit(ch,victim,gsn_hooves,1);
  if (number_range(1,4)==1)
  {
    victim->position = POS_STUNNED;
    WAIT_STATE(victim, 8);
    ch->position = POS_STANDING;
    act("You lift $N up by the horns and sends $M flying into the wall.",ch,NULL,victim,TO_CHAR);
    act("$n lifts you up by the horns and send you flying into the wall.",ch,NULL,victim, TO_VICT);
    act("$n lifts $N up by the horns and sends $M flying into the wall.",ch,NULL,victim, TO_ROOM);
  }
  return;
}

void do_satyrattack(CHAR_DATA * ch, char *argument)
{
        char      arg[MAX_INPUT_LENGTH];
        CHAR_DATA *victim;
        OBJ_DATA *obj;

        one_argument(argument, arg);

        if (IS_NPC(ch))
                return;

        if (!IS_RACE(ch, RACE_SATYR))
        {
                send_to_char("#0This is a SATYR Ability#n\n\r", ch);
                return;
        }

        if (arg[0] == '\0')
        {
                send_to_char("#0attack whom?#n\n\r", ch);
                return;
        }

        if ((victim = get_char_room(ch, NULL, arg)) == NULL)
        {
                send_to_char("#0They aren't here.#n\n\r", ch);
                return;
        }

        if (victim == ch)
        {
                send_to_char("#0No attacking yourself#n\n\r", ch);
                return;
        }

        if (is_safe(ch, victim)) return;
        if (IS_NPC(victim))
        {
        send_to_char("#cYou attack them with your Hooves!#n\n\r",ch);
        act("#G$n #RAttacks $s opponent with $s his Hooves!!#n", ch, NULL, NULL, TO_ROOM);
                ch->move -= 10000;
                WAIT_STATE(ch, 3 * PULSE_VIOLENCE);
		    multi_hit(ch,victim,gsn_hooves);
		    multi_hit(ch,victim,gsn_hooves);
		    multi_hit(ch,victim,gsn_hooves);
                return;
        }

        send_to_char("#cYou attack them with your Hooves!#n\n\r",ch);
        act("#G$n #RAttacks $s opponent with $s his Hooves!!#n", ch, NULL, NULL, TO_ROOM);
        ch->move -= 10000;
        WAIT_STATE(ch, 3 * PULSE_VIOLENCE);
        if (number_percent() > 25)
		    multi_hit(ch,victim,gsn_hooves);
        if (number_percent() > 50)
		    multi_hit(ch,victim,gsn_hooves);
        if (number_percent() > 75)
		    multi_hit(ch,victim,gsn_hooves);
        return;
}

void do_spectralhand(CHAR_DATA * ch, char *argument)
{
        char      arg[MAX_INPUT_LENGTH];
        CHAR_DATA *victim;
        OBJ_DATA *obj;

        one_argument(argument, arg);

        if (IS_NPC(ch))
                return;

        if (!IS_RACE(ch, RACE_SPECTRE))
        {
                send_to_char("#0This is a SPECTRE Ability#n\n\r", ch);
                return;
        }

        if (arg[0] == '\0')
        {
                send_to_char("#0attack whom?#n\n\r", ch);
                return;
        }

        if ((victim = get_char_room(ch, NULL, arg)) == NULL)
        {
                send_to_char("#0They aren't here.#n\n\r", ch);
                return;
        }

        if (victim == ch)
        {
                send_to_char("#0No attacking yourself#n\n\r", ch);
                return;
        }
        if (is_safe(ch, victim)) return;

        if (IS_NPC(victim))
        {
        send_to_char("#cYou attack them with your Spectral Hand!#n\n\r",ch);
        act("#G$n #RAttacks $s opponent with $s his Spectral Hand!!#n", ch, NULL, NULL, TO_ROOM);
                ch->move -= 10000;
                WAIT_STATE(ch, 3 * PULSE_VIOLENCE);
		    multi_hit(ch,victim,gsn_malice);
		    multi_hit(ch,victim,gsn_malice);
		    multi_hit(ch,victim,gsn_malice);
                return;
        }

        send_to_char("#cYou attack them with your Spectral Hand!#n\n\r",ch);
        act("#G$n #RAttacks $s opponent with $s his Spectral Hand!!#n", ch, NULL, NULL, TO_ROOM);
        ch->move -= 10000;
        WAIT_STATE(ch, 3 * PULSE_VIOLENCE);
        if (number_percent() > 25)
		    multi_hit(ch,victim,gsn_malice);
        if (number_percent() > 50)
		    multi_hit(ch,victim,gsn_malice);
        if (number_percent() > 75)
		    multi_hit(ch,victim,gsn_malice);
        return;
}

void do_fhowl(CHAR_DATA *ch,char *argument)
{
CHAR_DATA *victim;
char buf[MAX_STRING_LENGTH];
if (IS_NPC(ch)) return;

if (!IS_RACE(ch, RACE_FENRIS))
{
send_to_char("Huh?\n\r",ch);
return;
}
if ((victim=ch->fighting) == NULL)
{
send_to_char("You arent fighting anyone.\n\r",ch);
return;
}

  if (number_range(1,2)==1)
  {
sprintf(buf,"%s FENRIS howls at %s.",
ch->name,victim->name);
act(buf,ch,NULL,NULL,TO_ROOM);
sprintf(buf,"You FENRIS howl at %s.\n\r",
victim->name);
send_to_char(buf,ch);
do_flee(victim,"");
WAIT_STATE(ch,12);
return;
  }
  else
  {
   stc("You roar loudly.\n\r",ch);
   stc("Nothing happens.\n\r",ch);
   act("$n roars loudly.",ch,NULL,NULL,TO_ROOM);
   WAIT_STATE(ch, 12);
   return;
  }
}

void do_gentletouch( CHAR_DATA *ch, char *argument )
{
  char arg[MAX_INPUT_LENGTH];
  CHAR_DATA *victim;
  char buf[MAX_STRING_LENGTH];
  int dam;


  if (IS_NPC(ch)) return;
if (is_safe(ch, victim)) return;
 
  if (!IS_RACE(ch, RACE_NYMPH))
  {
    send_to_char("Huh?\n\r",ch);
    return;
  }

        if ((victim = get_char_room(ch, NULL, arg)) == NULL)
        {
                send_to_char("#0They aren't here.#n\n\r", ch);
                return;
        }

  WAIT_STATE( ch, 12 );
	if (number_range(1,3) == 3) WAIT_STATE(victim, 16);

  if (!IS_NPC(victim)) dam = number_range(4000,8000);
  else dam = number_range(15000,25000);

  sprintf(buf,"Your gentle touch strikes $N with DEVESTATING FORCE! [%d]\n\r",dam);
  act(buf,ch,NULL,victim,TO_CHAR);
  sprintf(buf,"$n's gentle touch strikes you with DEVESTATING FORCE! [%d]\n\r",dam);
  act(buf,ch,NULL,victim,TO_VICT);
  sprintf(buf,"$n's gentletouch strikes $N with DEVESTATING FORCE! [%d]\n\r",dam);
  act(buf,ch,NULL,victim,TO_NOTVICT); 
  hurt_person(ch,victim, dam);
  return;
}


void do_snakebite(CHAR_DATA *ch, char *argument)
{

  char arg[MAX_INPUT_LENGTH];
  CHAR_DATA *victim;
  char buf[MAX_STRING_LENGTH];
  int dam;
  CHAR_DATA *gch;
  CHAR_DATA *gch_next;


  if (IS_NPC(ch)) return;
  if (!IS_RACE(ch, RACE_MEDUSA))
  {
    send_to_char("Huh?\n\r", ch );
    return;
  }

 
  if (ch->mana < 1000)
  {
    send_to_char("You cannot harness the magical energies.\n\r",ch);
    return;
  }



act("You send a plea to the gods, calling for their almighty wrath to smite thos who oppose you.", ch, NULL, NULL, TO_CHAR);  act("$n raises $s hands to the sky and call for the gods to smite $s enemies.", ch, NULL, NULL, TO_ROOM);

  gch_next = ch->in_room->people;
  gch = ch->in_room->people;

  while (gch_next != NULL)
  {


    gch_next = gch->next_in_room;

    if (gch != ch)
    {

 
      if (is_safe(ch, gch)) break;

  if (!IS_NPC(victim)) dam = number_range(4000,8000);
  else dam = number_range(15000,25000);

  sprintf(buf,"Your snake bite strikes $N with DEVESTATING FORCE! [%d]\n\r",dam);
  act(buf,ch,NULL,victim,TO_CHAR);
  sprintf(buf,"$n's snake bite strikes you with DEVESTATING FORCE! [%d]\n\r",dam);
  act(buf,ch,NULL,victim,TO_VICT);
  sprintf(buf,"$n's snake bite strikes $N with DEVESTATING FORCE! [%d]\n\r",dam);
  act(buf,ch,NULL,victim,TO_NOTVICT); 
  hurt_person(ch,victim, dam);

    }


    gch = gch_next;
  }


   ch->mana -= 2000;
   WAIT_STATE(ch, 24);

  return;
}

void do_scharm(CHAR_DATA * ch, char *argument)
{
        CHAR_DATA *victim;
        char      arg[MAX_INPUT_LENGTH];
        int       level, sn, spelltype;

        argument = one_argument(argument, arg);

        if (IS_NPC(ch))
                return;
        if (!IS_RACE(ch, RACE_SATYR))
        {
                send_to_char("#0Huh?#n\n\r", ch);
                return;
        }
        if ((victim = get_char_room(ch, NULL, arg)) == NULL)
        {
                send_to_char("They aren't here.\n\r", ch);
                return;
        }
        if (ch->mana < 1000 || ch->move < 500)
        {
                send_to_char("You are to tired.\n\r", ch);
                return;
        }
				if (IS_IMMUNE(ch,IMM_CHARM))
				{
				send_to_char("They are immune to Charm.\n\r", ch);
        return;
        }

        act("You Satyre charms $N.", ch, NULL, victim, TO_CHAR);
        act("$n  Satyre Charms you.", ch, NULL, victim, TO_VICT);
        act("$n Satyre charms $N  $S.", ch, NULL, victim, TO_NOTVICT);


        if ((sn = skill_lookup("charm")) > 0)
	  { 
        spelltype = skill_table[sn].target;
        level = 200;
        (*skill_table[sn].spell_fun) (sn, level, ch, victim);
	  }

        ch->mana -= 1000;
        ch->move -= 500;
        WAIT_STATE(ch, 12);
        return;
}

void do_vcurse(CHAR_DATA * ch, char *argument)
{
        CHAR_DATA *victim;
        char      arg[MAX_INPUT_LENGTH];
        int       level, sn, spelltype;

        argument = one_argument(argument, arg);

        if (IS_NPC(ch))
                return;
        if (!IS_RACE(ch, RACE_VALKYRIE))
        {
                send_to_char("#0Huh?#n\n\r", ch);
                return;
        }
        if ((victim = get_char_room(ch, NULL, arg)) == NULL)
        {
                send_to_char("They aren't here.\n\r", ch);
                return;
        }
       if (is_safe(ch,victim)) return;
         if (ch->mana < 1000 || ch->move < 500)
        {
                send_to_char("You are to tired.\n\r", ch);
                return;
        }
        act("You Valkyrie curse $N.", ch, NULL, victim, TO_CHAR);
        act("$n Valkyrie curses you.", ch, NULL, victim, TO_VICT);
        act("$n Valkyrie $N  $S.", ch, NULL, victim, TO_NOTVICT);


        if ((sn = skill_lookup("weaken")) > 0)
	  { 
        spelltype = skill_table[sn].target;
        level = 200;
        (*skill_table[sn].spell_fun) (sn, level, ch, victim);
	  }

        if ((sn = skill_lookup("poison")) > 0)
	  {
        spelltype = skill_table[sn].target;
        level = 200;
        (*skill_table[sn].spell_fun) (sn, level, ch, victim);
	  }

        if ((sn = skill_lookup("curse")) > 0)
	  {
        spelltype = skill_table[sn].target;
        level = 200;
        (*skill_table[sn].spell_fun) (sn, level, ch, victim);
        }

        if ((sn = skill_lookup("blindness")) > 0)
	  {
        spelltype = skill_table[sn].target;
        level = 200;
        (*skill_table[sn].spell_fun) (sn, level, ch, victim);
        }

        if ((sn = skill_lookup("faerie fire")) > 0)
	  {
        spelltype = skill_table[sn].target;
        level = 200;
        (*skill_table[sn].spell_fun) (sn, level, ch, victim);
        }

/*        if ((sn = skill_lookup("sleep")) > 0) 
        {
        spelltype = skill_table[sn].target;
        level = 200;
        (*skill_table[sn].spell_fun) (sn, level, ch, victim);
        }
*/
			  victim->position == POS_SLEEPING;

        ch->mana -= 1000;
        ch->move -= 500;
        WAIT_STATE(ch, 14);
        return;
}



void do_fenrisform( CHAR_DATA *ch, char *argument )
{
    char arg [MAX_INPUT_LENGTH];
    char buf [MAX_STRING_LENGTH]; 
    argument = one_argument( argument, arg ); 

    if (IS_NPC(ch)) return;

    if (!IS_RACE(ch, RACE_FENRIS))
    {
	send_to_char("#0Huh?#n\n\r",ch);
	return;
    }

    if (IS_POLYAFF(ch, POLY_ZULOFORM))
    {   
        clear_stats(ch);
        REMOVE_BIT(ch->polyaff, POLY_ZULOFORM);
        REMOVE_BIT(ch->affected_by, AFF_POLYMORPH);
        act( "#0You transform into human form.#n", ch, NULL, NULL, TO_CHAR );
        act( "#7$n's #0shrinks back into human form.#n", ch, NULL, NULL, TO_ROOM);
        ch->damroll -= 500;
        ch->hitroll -= 500;
        ch->armor   += 300;
	free_string( ch->morph );
        ch->morph = str_dup( "" );
        return;
    }
    else if (IS_AFFECTED(ch,AFF_POLYMORPH))
    {
	send_to_char("#0You cant wolf form when changed.#n\n\r",ch);
	return;
	}
  
  
        if (ch->stance[0] != -1) do_stance(ch,"");
	if (ch->mounted == IS_RIDING) do_dismount(ch,"");
        clear_stats(ch);
        act( "#0You transform into the immortal Fenris Wolf.#n", ch, NULL, NULL, TO_CHAR);
        act( "#7$n's #0body shifts into the Fenris Wolf.#n", ch, NULL, NULL, TO_ROOM );
        SET_BIT(ch->polyaff, POLY_ZULOFORM);
        SET_BIT(ch->affected_by, AFF_POLYMORPH);
        sprintf(buf, "%s the huge white wolf", ch->name);
        free_string( ch->morph );
        ch->morph = str_dup( buf );
	ch->damroll += 500;
	ch->hitroll += 500;
        ch->armor   -= 300;
        return;
    }

void do_bite( CHAR_DATA *ch, char *argument )
{
  char arg[MAX_INPUT_LENGTH];
  CHAR_DATA *victim;
  char buf[MAX_STRING_LENGTH];
  int dam;


  if (IS_NPC(ch)) return;
 
  if (!IS_RACE(ch, RACE_FENRIS))
  {
    send_to_char("Huh?\n\r",ch);
    return;
  }
  if ( ( victim = get_char_room( ch, NULL, arg ) ) == NULL )
  {
    if (ch->fighting == NULL)
    {
      send_to_char( "They aren't here.\n\r", ch );
      return;
    }
    else victim = ch->fighting;

  }
if (is_safe(ch, victim)) return;
  WAIT_STATE( ch, 12 );

  if (!IS_NPC(victim)) dam = number_range(4000,8000);
  else dam = number_range(15000,25000);

  sprintf(buf,"You Bite $N with Vicious Malice! [%d]\n\r",dam);
  act(buf,ch,NULL,victim,TO_CHAR);
  sprintf(buf,"$n's The Fenris Wolf bites you with vicious malice! [%d]\n\r",dam);
  act(buf,ch,NULL,victim,TO_VICT);
  sprintf(buf,"$n's The Fenris Wolf bites $N with vicious malice! [%d]\n\r",dam);
  act(buf,ch,NULL,victim,TO_NOTVICT); 
  hurt_person(ch,victim, dam);
  return;
}

void do_griffinattack(CHAR_DATA * ch, char *argument)
{
        char      arg[MAX_INPUT_LENGTH];
        CHAR_DATA *victim;
        OBJ_DATA *obj;

        one_argument(argument, arg);

        if (IS_NPC(ch))
                return;

        if (!IS_RACE(ch, RACE_GRIFFIN))
        {
                send_to_char("#0This is a GRIFFIN Ability#n\n\r", ch);
                return;
        }

        if (arg[0] == '\0')
        {
                send_to_char("#0attack whom?#n\n\r", ch);
                return;
        }

        if ((victim = get_char_room(ch, NULL, arg)) == NULL)
        {
                send_to_char("#0They aren't here.#n\n\r", ch);
                return;
        }

        if (victim == ch)
        {
                send_to_char("#0No attacking yourself#n\n\r", ch);
                return;
        }

      if (is_safe(ch, victim)) return;
        if (IS_NPC(victim))
        {
        send_to_char("#cYou materialize in front of your opponent and GRIFFIN Attack!#n\n\r",ch);
        act("#G$n #RGRIFFIN Attacks $s opponent with $s his wings, beak and clwas!!#n", ch, NULL, NULL, TO_ROOM);
                ch->move -= 10000;
                WAIT_STATE(ch, 3 * PULSE_VIOLENCE);
		    multi_hit(ch,victim,gsn_buffet);
		    multi_hit(ch,victim,gsn_fangs);
		    multi_hit(ch,victim,gsn_claws);
                return;
        }

        send_to_char("#cYou materialize in front of your opponent and GRIFFIN Attack!#n\n\r",ch);
        act("#G$n #RGRIFFIN Attacks $s opponent with $s his wings, beak and clwas!!#n", ch, NULL, NULL, TO_ROOM);
        ch->move -= 10000;
        WAIT_STATE(ch, 3 * PULSE_VIOLENCE);
        if (number_percent() > 25)
		    multi_hit(ch,victim,gsn_buffet);
        if (number_percent() > 50)
		    multi_hit(ch,victim,gsn_fangs);
        if (number_percent() > 75)
		    multi_hit(ch,victim,gsn_claws);
        return;
}

void do_minotaurattack(CHAR_DATA * ch, char *argument)
{
        char      arg[MAX_INPUT_LENGTH];
        CHAR_DATA *victim;
        OBJ_DATA *obj;

        one_argument(argument, arg);

        if (IS_NPC(ch))
                return;

        if (!IS_RACE(ch, RACE_MINOTAUR))
        {
                send_to_char("#0This is a MINOTAUR Ability#n\n\r", ch);
                return;
        }

        if (arg[0] == '\0')
        {
                send_to_char("#0attack whom?#n\n\r", ch);
                return;
        }

        if ((victim = get_char_room(ch, NULL, arg)) == NULL)
        {
                send_to_char("#0They aren't here.#n\n\r", ch);
                return;
        }

        if (victim == ch)
        {
                send_to_char("#0No attacking yourself#n\n\r", ch);
                return;
        }

        if (is_safe(ch, victim)) return;
        if (IS_NPC(victim))
        {
        send_to_char("#cYou materialize in front of your opponent and Minotaur Attack!#n\n\r",ch);
        act("#G$n #RMinotaur Attacks $s opponent with $s looking through his one eye!!#n", ch, NULL, NULL, TO_ROOM);
                ch->move -= 10000;
                WAIT_STATE(ch, 3 * PULSE_VIOLENCE);
		    multi_hit(ch,victim,gsn_punch);
		    multi_hit(ch,victim,gsn_punch);
		    multi_hit(ch,victim,gsn_punch);
                return;
        }

        send_to_char("#cYou materialize in front of your opponent and Minotaur Attack!#n\n\r",ch);
        act("#G$n #RMinotaur Attacks $s opponent with $s looking through his one eye!!#n", ch, NULL, NULL, TO_ROOM);
        ch->move -= 10000;
        WAIT_STATE(ch, 3 * PULSE_VIOLENCE);
        if (number_percent() > 25)
		    multi_hit(ch,victim,gsn_punch);
        if (number_percent() > 50)
		    multi_hit(ch,victim,gsn_punch);
        if (number_percent() > 75)
		    multi_hit(ch,victim,gsn_punch);
        return;
}

void do_cyclopsattack(CHAR_DATA * ch, char *argument)
{
        char      arg[MAX_INPUT_LENGTH];
        CHAR_DATA *victim;
        OBJ_DATA *obj;

        one_argument(argument, arg);

        if (IS_NPC(ch))
                return;

        if (!IS_RACE(ch, RACE_CYCLOPS))
        {
                send_to_char("#0This is a CYCLOPS Ability#n\n\r", ch);
                return;
        }

        if (arg[0] == '\0')
        {
                send_to_char("#0attack whom?#n\n\r", ch);
                return;
        }

        if ((victim = get_char_room(ch, NULL, arg)) == NULL)
        {
                send_to_char("#0They aren't here.#n\n\r", ch);
                return;
        }

        if (victim == ch)
        {
                send_to_char("#0No attacking yourself#n\n\r", ch);
                return;
        }
        if (is_safe(ch, victim)) return;

        if (IS_NPC(victim))
        {
        send_to_char("#cYou materialize in front of your opponent and Cyclops Attack!#n\n\r",ch);
        act("#G$n #RCyclops Attacks $s opponent with $s looking through his one eye!!#n", ch, NULL, NULL, TO_ROOM);
                ch->move -= 10000;
                WAIT_STATE(ch, 3 * PULSE_VIOLENCE);
		    multi_hit(ch,victim,gsn_punch);
		    multi_hit(ch,victim,gsn_punch);
		    multi_hit(ch,victim,gsn_punch);
                return;
        }

        send_to_char("#cYou materialize in front of your opponent and Cyclops Attack!#n\n\r",ch);
        act("#G$n #RCyclops Attacks $s opponent with $s looking through his one eye!!#n", ch, NULL, NULL, TO_ROOM);
        ch->move -= 10000;
        WAIT_STATE(ch, 3 * PULSE_VIOLENCE);
        if (number_percent() > 25)
		    multi_hit(ch,victim,gsn_punch);
        if (number_percent() > 50)
		    multi_hit(ch,victim,gsn_punch);
        if (number_percent() > 75)
		    multi_hit(ch,victim,gsn_punch);
        return;
}

void do_gswoop( CHAR_DATA *ch, char *argument )
{
	char arg[MAX_INPUT_LENGTH];
	CHAR_DATA *victim;
	one_argument( argument, arg );

	if( IS_NPC(ch) ) return;
	if(!IS_IMMORTAL(ch))
	{
		if (!IS_RACE(ch, RACE_GRIFFIN))
		{
			send_to_char("Huh?\n\r", ch);
			return;
		}

	}

	 if ( arg[0] == '\0' )
	 {
		send_to_char( "Swoop to whom?\n\r", ch );
		return;
	 }
	 if ( ch->position == POS_FIGHTING )
	 {
		send_to_char( "No way! You are fighting.\n\r", ch );
		return;
	 }
    if (IS_IMMUNE(victim, IMM_TRAVEL) && victim->fight_timer < 1 )
    {
  send_to_char("I don't think they want you to do that.\n\r",ch);
  return;
    }


	 if ( ( victim = get_char_world( ch, arg ) ) == NULL )
	 {
		send_to_char( "Nobody by that name.\n\r", ch );
		return;
	 }
        if (IS_SET(victim->in_room->room_flags, ROOM_ASTRAL))
        {
                stc("You can't find it's room.\n\r", ch);
                return;
        }

        if (IS_SET(ch->in_room->room_flags, ROOM_ASTRAL))
        {
                stc("Your room is not connected to the astral plane.\n\r",
                    ch);
                return;
        }

	 if( victim == ch )
	 {
		 stc( "Not to yourself.\n\r",ch);
		 return;
	 }



	act( "You disappear up into the sky.", ch, NULL, NULL, TO_CHAR );
	act( "$n disappears up into the sky.", ch, NULL, NULL, TO_ROOM );
	char_from_room( ch );
	char_to_room( ch, get_room_index(victim->in_room->vnum) );
 	

	if (is_safe(ch,victim)) return;

	act("You swoop down from the sky onto $N.",ch,NULL,victim,TO_CHAR);
	act("$n swoops down from the sky onto $N.",ch,NULL,victim,TO_NOTVICT);
	act("$n swoops down from the sky onto you.",ch,NULL,victim,TO_VICT);
	multi_hit(ch, victim, gsn_punch);
  WAIT_STATE(ch,8);
	return;
}

void do_hscream(CHAR_DATA *ch, char *argument)
{
	 CHAR_DATA *vch;
	 CHAR_DATA *vch_next;
         CHAR_DATA *victim;
	 char buf[MAX_STRING_LENGTH];
	 char buf2[MAX_STRING_LENGTH];
	 int dam;
	int rrdam = 0;
        int rrcount = 0;
         int i = 0;
	 int chance = 0;


    if (!IS_RACE(ch, RACE_HARPIE))

         {
            send_to_char ("Huh?\n\r",ch);
            return;
         }


        chance = (number_range (1,20));
        if (chance <= 7) i = 1;
  	else if (chance > 7 && chance <= 22)   i = 2;
  	else if (chance > 22 && chance <= 32)   i = 3;
  	else if (chance > 32 && chance <= 52)   i = 4;
  	else if (chance > 52 && chance <= 90)   i = 5;
  	else if (chance > 90 && chance <= 94)   i = 6;
  	else if (chance > 94 && chance <= 110)  i = 7;
  	else if (chance > 110 && chance <= 125) i = 8;
  	else if (chance > 125 && chance <= 128) i = 9;
  	else if (128 < chance ) i = 10;

   for(i=i;i>0;i--)
   {
	  send_to_char("#0You HARPIE SCREAM#n.\n\r",ch);
     for ( vch = ch->in_room->people; vch!=NULL;vch = vch_next)
      {
      	if(ch->mana <= 100)
      		break;
        vch_next = vch->next_in_room;
        if (can_see(ch,vch))
	// if ( IS_NPC(ch) ? !IS_NPC(vch) : IS_NPC(vch) )
	if (!IS_NPC(ch) && (ch != vch))
	{
          dam = number_range((ch->damcap[DAM_CAP] - 500), 
(ch->damcap[DAM_CAP]+(ch->pRank*125)));
	  sprintf(buf, "%s's HARPIE SCREAMS at you [#G%d#n]\n\r", ch->name, dam);
	  send_to_char( buf, vch );
	if(!IS_SET(ch->act,PLR_BRIEF5)){
	  sprintf(buf2, "You HARPIE SCREAM at %s [#G%d#n]\n\r",vch->name, dam);
	  send_to_char( buf2, ch );
	}
	else{
		rrdam += dam;
		rrcount += 1;
	}
	  set_fighting(ch,vch, TRUE);
	  hurt_person(ch,vch,dam);
	}
      }
    }
	if(IS_SET(ch->act,PLR_BRIEF5)){
		sprintf(buf,"#GYou've dealt #C%d #Gattacks causing #C%d #Gdamage. #0(#y%d #Gaverage#0)#n\n\r",rrcount,rrdam,dam);
		stc(buf,ch);
	}

  WAIT_STATE (ch, 4);
  return;
}

void do_vswoop( CHAR_DATA *ch, char *argument )
{
	char arg[MAX_INPUT_LENGTH];
	CHAR_DATA *victim;
	one_argument( argument, arg );

	if( IS_NPC(ch) ) return;
	if(!IS_IMMORTAL(ch))
	{
		if (!IS_RACE(ch, RACE_VALKYRIE))
		{
			send_to_char("Huh?\n\r", ch);
			return;
		}

	}

	 if ( arg[0] == '\0' )
	 {
		send_to_char( "Swoop to whom?\n\r", ch );
		return;
	 }
	 if ( ch->position == POS_FIGHTING )
	 {
		send_to_char( "No way! You are fighting.\n\r", ch );
		return;
	 }
    if (IS_IMMUNE(victim, IMM_TRAVEL) && victim->fight_timer < 1 )
    {
  send_to_char("I don't think they want you to do that.\n\r",ch);
  return;
    }


	 if ( ( victim = get_char_world( ch, arg ) ) == NULL )
	 {
		send_to_char( "Nobody by that name.\n\r", ch );
		return;
	 }
        if (IS_SET(victim->in_room->room_flags, ROOM_ASTRAL))
        {
                stc("You can't find it's room.\n\r", ch);
                return;
        }

        if (IS_SET(ch->in_room->room_flags, ROOM_ASTRAL))
        {
                stc("Your room is not connected to the astral plane.\n\r",
                    ch);
                return;
        }

	 if( victim == ch )
	 {
		 stc( "Not to yourself.\n\r",ch);
		 return;
	 }



	act( "You disappear up into the sky.", ch, NULL, NULL, TO_CHAR );
	act( "$n disappears up into the sky.", ch, NULL, NULL, TO_ROOM );
	char_from_room( ch );
	char_to_room( ch, get_room_index(victim->in_room->vnum) );
 	

	if (is_safe(ch,victim)) return;

	act("You swoop down from the sky onto $N.",ch,NULL,victim,TO_CHAR);
	act("$n swoops down from the sky onto $N.",ch,NULL,victim,TO_NOTVICT);
	act("$n swoops down from the sky onto you.",ch,NULL,victim,TO_VICT);
	multi_hit(ch, victim, gsn_punch);
	WAIT_STATE(ch,8);
	return;
}

void do_racedetox(CHAR_DATA * ch, char *argument)
{
	char      arg[MAX_INPUT_LENGTH];

        one_argument(argument, arg);

        if (IS_NPC(ch))
                return;

        if (arg[0] == '\0')
        {
        send_to_char("#0Type racedetox continue to become human.#n\n\r", ch);
        return;
			  }
				if (!str_cmp(arg,"continue"))
        {
        send_to_char("#0You are now Human.#n\n\r", ch);
				ch->pcRace = RACE_HUMAN;
        return;
        }
return;
}


void do_cerberusattack(CHAR_DATA * ch, char *argument)
{
        char      arg[MAX_INPUT_LENGTH];
        CHAR_DATA *victim;
        OBJ_DATA *obj;

        one_argument(argument, arg);

        if (IS_NPC(ch))
                return;

        if (!IS_RACE(ch, RACE_CERBERUS))
        {
                send_to_char("#0This is a CERBERUS Ability#n\n\r", ch);
                return;
        }

        if (arg[0] == '\0')
        {
                send_to_char("#0attack whom?#n\n\r", ch);
                return;
        }

        if ((victim = get_char_room(ch, NULL, arg)) == NULL)
        {
                send_to_char("#0They aren't here.#n\n\r", ch);
                return;
        }

        if (victim == ch)
        {
                send_to_char("#0No attacking yourself#n\n\r", ch);
                return;
        }
       if (is_safe(ch, victim)) return;

        if (IS_NPC(victim))
        {
        send_to_char("#cYou materialize in front of your opponent and Cerberus Attack!#n\n\r",ch);
        act("#G$n #RCerberus Attacks $s opponent with $s his three dog heads!!#n", ch, NULL, NULL, TO_ROOM);
                ch->move -= 10000;
                WAIT_STATE(ch, 3 * PULSE_VIOLENCE);
		    multi_hit(ch,victim,gsn_fangs);
		    multi_hit(ch,victim,gsn_fangs);
		    multi_hit(ch,victim,gsn_claws);
                return;
        }

        send_to_char("#cYou materialize in front of your opponent and Cerberus Attack!#n\n\r",ch);
        act("#G$n #RCerberus Attacks $s opponent with $s his three dog heads!!#n", ch, NULL, NULL, TO_ROOM);
        ch->move -= 10000;
        WAIT_STATE(ch, 3 * PULSE_VIOLENCE);
        if (number_percent() > 25)
		    multi_hit(ch,victim,gsn_fangs);
        if (number_percent() > 50)
		    multi_hit(ch,victim,gsn_fangs);
        if (number_percent() > 75)
		    multi_hit(ch,victim,gsn_claws);
        return;
}


void do_cerberusform( CHAR_DATA *ch, char *argument )
{
    char arg [MAX_INPUT_LENGTH];
    char buf [MAX_STRING_LENGTH]; 
    argument = one_argument( argument, arg ); 

    if (IS_NPC(ch)) return;

    if (!IS_RACE(ch, RACE_CERBERUS))
    {
	send_to_char("#0Huh?#n\n\r",ch);
	return;
    }

    if (IS_POLYAFF(ch, POLY_ZULOFORM))
    {   
        REMOVE_BIT(ch->polyaff, POLY_ZULOFORM);
        REMOVE_BIT(ch->affected_by, AFF_POLYMORPH);
        act( "#0You transform into human form.#n", ch, NULL, NULL, TO_CHAR );
        act( "#7$n's #0shrinks back into human form.#n", ch, NULL, NULL, TO_ROOM);
        ch->damroll -= 500;
        ch->hitroll -= 500;
        ch->armor   += 300;
	free_string( ch->morph );
        ch->morph = str_dup( "" );
        return;
    }
    else if (IS_AFFECTED(ch,AFF_POLYMORPH))
    {
	send_to_char("#0You cant wolf form when changed.#n\n\r",ch);
	return;
	}
  
  
        if (ch->stance[0] != -1) do_stance(ch,"");
	if (ch->mounted == IS_RIDING) do_dismount(ch,"");
        act( "#0You transform into the three-headed hound Ceberus.#n", ch, NULL, NULL, TO_CHAR);
        act( "#7$n's #0body shifts into the three headed hound Ceberus.#n", ch, NULL, NULL, TO_ROOM );
        SET_BIT(ch->polyaff, POLY_ZULOFORM);
        SET_BIT(ch->affected_by, AFF_POLYMORPH);
        sprintf(buf, "%s the three headed dog", ch->name);
        free_string( ch->morph );
        ch->morph = str_dup( buf );
	ch->damroll += 500;
	ch->hitroll += 500;
        ch->armor   -= 300;
        return;
    }

void do_bless(CHAR_DATA * ch, char *argument)
{
        CHAR_DATA *victim;
        char      arg[MAX_INPUT_LENGTH];
        int       level, sn, spelltype;

        argument = one_argument(argument, arg);

        if (IS_NPC(ch))
                return;
        if (!IS_RACE(ch, RACE_VALKYRIE))
        {
                send_to_char("#0Huh?#n\n\r", ch);
                return;
        }
        if ((victim = get_char_room(ch, NULL, arg)) == NULL)
        {
                send_to_char("They aren't here.\n\r", ch);
                return;
        }
        if (ch->mana < 1000 || ch->move < 500)
        {
                send_to_char("You are to tired.\n\r", ch);
                return;
        }
        act("You Valkyrie Bless $N.", ch, NULL, victim, TO_CHAR);
        act("$n Valkyrie Blesses you.", ch, NULL, victim, TO_VICT);
        act("$n Valkyrie $N  $S.", ch, NULL, victim, TO_NOTVICT);


        if ((sn = skill_lookup("bless")) > 0)
	  { 
        spelltype = skill_table[sn].target;
        level = 200;
        (*skill_table[sn].spell_fun) (sn, level, ch, victim);
	  }

        if ((sn = skill_lookup("frenzy")) > 0)
	  {
        spelltype = skill_table[sn].target;
        level = 200;
        (*skill_table[sn].spell_fun) (sn, level, ch, victim);
	  }

        if ((sn = skill_lookup("armor")) > 0)
	  {
        spelltype = skill_table[sn].target;
        level = 200;
        (*skill_table[sn].spell_fun) (sn, level, ch, victim);
        }

        if ((sn = skill_lookup("cure poison")) > 0)
	  {
        spelltype = skill_table[sn].target;
        level = 200;
        (*skill_table[sn].spell_fun) (sn, level, ch, victim);
        }

        if ((sn = skill_lookup("shield")) > 0)
	  {
        spelltype = skill_table[sn].target;
        level = 200;
        (*skill_table[sn].spell_fun) (sn, level, ch, victim);
        }

        if ((sn = skill_lookup("stone skin")) > 0) 
        {
        spelltype = skill_table[sn].target;
        level = 200;
        (*skill_table[sn].spell_fun) (sn, level, ch, victim);
        }

        if ((sn = skill_lookup("darkblessing")) > 0) 
        {
        spelltype = skill_table[sn].target;
        level = 200;
        (*skill_table[sn].spell_fun) (sn, level, ch, victim);
        }


        if ((sn = skill_lookup("fly")) > 0) 
        {
        spelltype = skill_table[sn].target;
        level = 200;
        (*skill_table[sn].spell_fun) (sn, level, ch, victim);
        }

        if ((sn = skill_lookup("detect invis")) > 0) 
        {
        spelltype = skill_table[sn].target;
        level = 200;
        (*skill_table[sn].spell_fun) (sn, level, ch, victim);
        }

        if ((sn = skill_lookup("detect hidden")) > 0) 
        {
        spelltype = skill_table[sn].target;
        level = 200;
        (*skill_table[sn].spell_fun) (sn, level, ch, victim);
        }

        if ((sn = skill_lookup("detect magic")) > 0) 
        {
        spelltype = skill_table[sn].target;
        level = 200;
        (*skill_table[sn].spell_fun) (sn, level, ch, victim);
        }

        ch->mana -= 1000;
        ch->move -= 500;
        WAIT_STATE(ch, 12);
        return;
}

void do_chimeraattack(CHAR_DATA * ch, char *argument)
{
        char      arg[MAX_INPUT_LENGTH];
        CHAR_DATA *victim;
        OBJ_DATA *obj;

        one_argument(argument, arg);

        if (IS_NPC(ch))
                return;

        if (!IS_RACE(ch, RACE_CHIMERA))
        {
                send_to_char("#0This is a CHIMERA Ability#n\n\r", ch);
                return;
        }

        if (arg[0] == '\0')
        {
                send_to_char("#0attack whom?#n\n\r", ch);
                return;
        }

        if ((victim = get_char_room(ch, NULL, arg)) == NULL)
        {
                send_to_char("#0They aren't here.#n\n\r", ch);
                return;
        }

        if (victim == ch)
        {
                send_to_char("#0No attacking yourself#n\n\r", ch);
                return;
        }

       if (is_safe(ch, victim)) return;
        if (IS_NPC(victim))
        {
        send_to_char("#cYou materialize in front of your opponent and Chimera Attack!#n\n\r",ch);
        act("#G$n #RChimera Attacks $s opponent with $s two heads and a snakes tail!!#n", ch, NULL, NULL, TO_ROOM);
                ch->move -= 10000;
                WAIT_STATE(ch, 3 * PULSE_VIOLENCE);
		    multi_hit(ch,victim,gsn_fangs);
		    multi_hit(ch,victim,gsn_headbutt);
		    multi_hit(ch,victim,gsn_claws);
                return;
        }

        send_to_char("#cYou materialize in front of your opponent and Chimera Attack!#n\n\r",ch);
        act("#G$n #RChimera Attacks $s opponent with $s two heads and a snakes tail!!#n", ch, NULL, NULL, TO_ROOM);
        ch->move -= 10000;
        WAIT_STATE(ch, 3 * PULSE_VIOLENCE);
        if (number_percent() > 25)
		    multi_hit(ch,victim,gsn_fangs);
        if (number_percent() > 50)
		    multi_hit(ch,victim,gsn_headbutt);
        if (number_percent() > 75)
		    multi_hit(ch,victim,gsn_claws);
        return;
}
void do_mgore(CHAR_DATA * ch, char *argument)
{
        char      arg[MAX_INPUT_LENGTH];
        CHAR_DATA *victim;
        OBJ_DATA *obj;

        one_argument(argument, arg);

        if (IS_NPC(ch))
                return;

        if (!IS_RACE(ch, RACE_MINOTAUR))
        {
                send_to_char("#0This is a Minotaur Ability#n\n\r", ch);
                return;
        }

        if (arg[0] == '\0')
        {
                send_to_char("Gore whom?\n\r", ch);
                return;
        }

        if ((victim = get_char_room(ch, NULL, arg)) == NULL)
        {
                send_to_char("They aren't here.\n\r", ch);
                return;
        }

        if (victim == ch)
        {
                send_to_char("#0No goring yourself#n\n\r", ch);
                return;
        } 
       if (is_safe(ch, victim)) return;

        if (IS_NPC(victim))
        {
         
   		send_to_char("#cYou materialize in front of your opponent and #Rgore him with your sharp horns!#n\n\r", ch);
      act("#G$n #cmaterializes out of thin air, screaming wildly, and #Rgores $s opponent with $s razor sharp horns!!#n", 
			ch, NULL, NULL, TO_ROOM);
                ch->move -= 10000;
                WAIT_STATE(ch, 3 * PULSE_VIOLENCE);
                multi_hit(ch, victim, TYPE_UNDEFINED);
                multi_hit(ch, victim, TYPE_UNDEFINED);
                multi_hit(ch, victim, TYPE_UNDEFINED);
                multi_hit(ch, victim, TYPE_UNDEFINED);
                multi_hit(ch, victim, TYPE_UNDEFINED);
                return;
        }

        send_to_char
                ("#cYou materialize in front of your opponent and #Rgore him with your sharp horns#n\n\r",
                 ch);
act("#G$n #cmaterializes out of thin air, screaming wildly, and #Rgores you with $s razor sharp horns!!#n", 
ch, NULL, NULL, TO_VICT);
        ch->move -= 10000;
        WAIT_STATE(ch, 3 * PULSE_VIOLENCE);
        multi_hit(ch, victim, TYPE_UNDEFINED);
        if (number_percent() > 85)
                multi_hit(ch, victim, TYPE_UNDEFINED);
        if (number_percent() > 95)
                multi_hit(ch, victim, TYPE_UNDEFINED);
        return;
}

/*
void do_dwarfforge( CHAR_DATA *ch, char *argument )
{
    OBJ_INDEX_DATA *pObjIndex;
    OBJ_DATA *obj;
    char arg[MAX_INPUT_LENGTH];
    int vnum = 0;

    argument = one_argument( argument, arg );

    if (IS_NPC(ch)) return;

	if (!IS_RACE(ch, RACE_GULLYDWARF) && !IS_RACE(ch, RACE_HILLDWARF)
	&& !IS_RACE(ch, RACE_MTDWARF) && !IS_RACE(ch, RACE_DUERGARDWARF)
	&& !IS_RACE(ch, RACE_ARCHANGEL))
    {
	    send_to_char("Huh?\n\r",ch);
	    return;
    }

    if (arg[0] == '\0')
    {
	send_to_char("Which Weapon do you wish to Create.\n\r", ch );
	send_to_char("BattleAxe   Warhammer\n\r", ch);
	return;
    }
    if      (!str_cmp(arg,"battleaxe"     )) vnum = 81111;
    else if (!str_cmp(arg,"warhammer"   )) vnum = 81110;
    else
    {
        do_dwarfforge(ch,"");
	return;
    }

    if ( ch->practice < 2500)
    {
	send_to_char("It Costs 2500 Primal to create a Dwarf Forged Weapon.\n\r",ch);
	return;
    }
    

    if ( vnum == 0 || (pObjIndex = get_obj_index( vnum )) == NULL)
    {
	send_to_char("Missing object, please inform Xrakisis.\n\r",ch);
	return;
    }

    ch->practice -= 2500;
    obj = create_object(pObjIndex, 50);
    obj->questowner = str_dup(ch->pcdata->switchname);
    obj_to_char(obj, ch);
    act("$p appears in your hands.",ch,obj,NULL,TO_CHAR);
    act("$p appears in $n's hands.",ch,obj,NULL,TO_ROOM);
    return;
}

*/

void do_trample( CHAR_DATA *ch, char *argument )
{
  char arg[MAX_INPUT_LENGTH];
  CHAR_DATA *victim;
  char buf[MAX_STRING_LENGTH];
  int dam;


  if (IS_NPC(ch)) return;
 
  if (!IS_RACE(ch, RACE_CENTAUR))
  {
    send_to_char("Huh?\n\r",ch);
    return;
  }
  if ( ( victim = get_char_room( ch, NULL, arg ) ) == NULL )
  {
    if (ch->fighting == NULL) 
    {
      send_to_char( "They aren't here.\n\r", ch );
      return;
    }
    else victim = ch->fighting;
     
  }
if (is_safe(ch, victim)) return;

  WAIT_STATE( ch, 12 );

  if (number_range(1,3) == 1)
  {
  do_say(ch,"#rHell Hath No Fury like a Centaur Scorn!#n");
  do_stance(victim, "");
  }

  if (!IS_NPC(victim)) dam = number_range(3500,6500);
  else dam = number_range(15000,25000);

  sprintf(buf,"You Trample $N with your Hooves [%d]\n\r",dam);
  act(buf,ch,NULL,victim,TO_CHAR);
  sprintf(buf,"$n's Tramples you with their Hooves [%d]\n\r",dam);
  act(buf,ch,NULL,victim,TO_VICT);
  sprintf(buf,"$n's tramples $N with their Hooves [%d]\n\r",dam);
  act(buf,ch,NULL,victim,TO_NOTVICT); 
  hurt_person(ch,victim, dam);
  return;
}


/*

void do_ascension(CHAR_DATA *ch, char *argument)
{
  char arg1[MAX_STRING_LENGTH];
  char buf[MSL];
  argument = one_argument( argument, arg1 );

  if (IS_NPC(ch))return;

  if (!IS_NPC(ch) && ch->pcRaceLevel < 200)
  {   
    send_to_char("You must be level 200 to Ascend\n\r",ch);
    return;
  }
  if (ch->level < 3)
  {
    send_to_char("You must be avatar to Ascend.\n\r",ch);
    return;
  }

  if (ch->pcRace == RACE_ARCHDEMON || ch->pcRace == RACE_ARCHANGEL)
  {
    stc("YOu have chosen your path already!\n\r",ch);
    return;
  }

if ( arg1[0] == '\0' )
{
  send_to_char("Ascension  : Type Ascend (race) to choose your Demi-God Race.\n\r\n\r",ch);
  send_to_char("Options are ArchDemon or ArchAngel.\n\r\n\r",ch);
  return;
}

do_clearstats2(ch,"");

if (!str_cmp(arg1,"archdemon"))
{
	ch->pcRace = RACE_ARCHDEMON;
	send_to_char( "You have Become An Arch-Demon Demi-God.\n\r", ch);
	return;
}
else if (!str_cmp(arg1,"archangel"))
{
        ch->pcRace = RACE_ARCHANGEL;
        send_to_char( "You have Become an ArchAngel.\n\r", ch);
        return;
}

}

*/


void do_racepowers(CHAR_DATA * ch, char *argument)
{
        char      buf[MAX_STRING_LENGTH];

	if (IS_NPC(ch))
	return;

	send_to_char("[--------------------[Race Powers]--------------------]\n\r", ch);

        if (IS_RACE(ch, RACE_CHIMERA))
        {
        send_to_char("#R(#0Chimeraattack#R)#0 Powerful attack.#n\n\r", ch);
        send_to_char("#R(#0Fbraeth#R)#0 Breath Fire!#n\n\r", ch);
        send_to_char("#R(#0Chimera#R)#0 Change to your true form.#n\n\r", ch);
        send_to_char("#R(#0Csting#R)#0 De-Stance.#n\n\r", ch);
	  }
        if (IS_RACE(ch, RACE_CERBERUS))
	  {
        send_to_char("#R(#0Cerberusform#R)#0 Change to your true form.#n\n\r", ch);
        send_to_char("#R(#0Cerberusattack#R)#0 Powerful attack.#n\n\r", ch);
        send_to_char("#R(#0Engulf#R)#0 Engulf the room in flames, harming your enemy and preventing fleeing.#n\n\r", ch);
        send_to_char("#R(#0Eternal#R)#0 Call upon eternal flames to strengthen you.#n\n\r", ch);
	  }
        if (IS_RACE(ch, RACE_CYCLOPS))
	  {
        send_to_char("#R(#0Crushingblow#R)#0 Midrounder.#n\n\r", ch);
        send_to_char("#R(#0Cyclopsattack#R)#0 Powerful attack.#n\n\r", ch);
        send_to_char("#R(#0Tremble#R)#0 Destance.#n\n\r", ch);
        send_to_char("#R(#0#R)#0 .#n\n\r", ch);
	  }
        if (IS_RACE(ch, RACE_GRIFFIN))
	  {
        send_to_char("#R(#0Gswoop#R)#0 Swoop to them as they flee.#n\n\r", ch);
        send_to_char("#R(#0Griffinattack#R)#0 Powerful attack.#n\n\r", ch);
        send_to_char("#R(#0Buffet#R)#0 Autoattack.#n\n\r", ch);
        send_to_char("#R(#0Griffinform#R)#0 Your true form.#n\n\r", ch);
	  }
        if (IS_RACE(ch, RACE_HARPIE))
	  {
        send_to_char("#R(#0Hscream#R)#0 Room Attack.#n\n\r", ch);
        send_to_char("#R(#0Pirate#R)#0 Steal something.#n\n\r", ch);
        send_to_char("#R(#0Hspit#R)#0 Midrounder.#n\n\r", ch);
        send_to_char("#R(#0Buffet#R)#0 Autoattack.#n\n\r", ch);
 	  }
        if (IS_RACE(ch, RACE_PHOENIX))
	  {
        send_to_char("#R(#0Sunbolt#R)#0 Shoot flames at your opponent, with a chance to put their eyes out..#n\n\r", ch);
        send_to_char("#R(#0Sunmerge#R)#0 Merge with the sun, become an entity of pure flames. (ethereal).#n\n\r", ch);
        send_to_char("#R(#0Divebomb#R)#0 (Only) while merged with the sun, divebomb your opponent.#n\n\r", ch);
        send_to_char("#R(#0Tear#R)#0 Shed a tear unto someone, revitalizing them.#n\n\r", ch);
        send_to_char("#R(#0Rebirth#R)#0 Burst into flames and experience rebirth from your ashes.#n\n\r", ch);
	  }  
	  if (IS_RACE(ch, RACE_SATYR))
	  {
        send_to_char("#R(#0Scharm#R)#0 Charm.#n\n\r", ch);
        send_to_char("#R(#0Satyrattack#R)#0 Powerful attack.#n\n\r", ch);
	  }
	  if (IS_RACE(ch, RACE_MEDUSA))
	  {
        send_to_char("#R(#0Look#R)#0 When someone looks at you they take 10K damage.#n\n\r", ch);
        send_to_char("#R(#0Snakebite#R)#0 Medusa attack.#n\n\r", ch);
        send_to_char("#R(#0Turnhead#R)#0 force someone to look at you.#n\n\r", ch);
	  }
	  if (IS_RACE(ch, RACE_NYMPH))
	  {
        send_to_char("#R(#0Look#R)#0 When someone looks at you they take 10K damage.#n\n\r", ch);
        send_to_char("#R(#0Gentletouch#R)#0 Nymph attack.#n\n\r", ch);
        send_to_char("#R(#0Turnhead#R)#0 force someone to look at you.#n\n\r", ch);
	  }
	  if (IS_RACE(ch, RACE_TITAN))
	  {
        send_to_char("#R(#0Flameburst#R)#0 Room attack.#n\n\r", ch);
        send_to_char("#R(#0Supermight#R)#0 Strengthen Yourself.#n\n\r", ch);
        send_to_char("#R(#0Titanheal#R)#0 Heal.#n\n\r", ch);
        send_to_char("#R(#0Tearthquakef#R)#0 De-stance.#n\n\r", ch);
	  }
	  if (IS_RACE(ch, RACE_FENRIS))
	  {
	  send_to_char("#R(#0Bite#R)#0 Are you a biter?.#n\n\r", ch);
        send_to_char("#R(#0Fenrisform#R)#0 Assume your true form.#n\n\r", ch);
        send_to_char("#R(#0Fhowl#R)#0 make them flee.#n\n\r", ch);
        send_to_char("#R(#0Frage#R)#0 Powerful attack.#n\n\r", ch);
	  }
	  if (IS_RACE(ch, RACE_SPECTRE))
	  {
        send_to_char("#R(#0Mistform#R)#0 Ethereal.#n\n\r", ch);
        send_to_char("#R(#0Spectalhand#R)#0 Powerful attack.#n\n\r", ch);
        send_to_char("#R(#0Deathtouch#R)#0 Secondary, more reliable attack.#n\n\r", ch);
        send_to_char("#R(#0Sblink#R)#0 Spectre Blink.#n\n\r", ch);
        send_to_char("#R(#0Phase#R)#0 Defense.#n\n\r", ch);
	  }
	  if (IS_RACE(ch, RACE_MINOTAUR))
	  {
        send_to_char("#R(#0Mgore#R)#0 Attack.#n\n\r", ch);
        send_to_char("#R(#0Minotaurattack#R)#0 Chance for three rounds of attacks.#n\n\r", ch);
        send_to_char("#R(#0Mcharge#R)#0 Attack them with a headbut and your hooves.#n\n\r", ch);
	  }
	  if (IS_RACE(ch, RACE_CENTAUR))
	  {
        send_to_char("#R(#0Thunderhammer#R)#0 Area attack.#n\n\r", ch);
        send_to_char("#R(#0Trample#R)#0 De-Stance.#n\n\r", ch);
        send_to_char("#R(#0Centaurattack#R)#0 Powerful Attack.#n\n\r", ch);
	  }
	  if (IS_RACE(ch, RACE_VALKYRIE))
	  {
        send_to_char("#R(#0Bless#R)#0 Spellup yourself or an Ally.#n\n\r", ch);
        send_to_char("#R(#0Vswoop#R)#0 Attack them when they flee.#n\n\r", ch);
        send_to_char("#R(#0Vcurse#R)#0 Negative spellup for enemy.#n\n\r", ch);
	  }

        send_to_char("[--------------------[  level 25  ]-------------------]\n\r", ch);
        if (ch->pcRaceLevel >= 25)
        {
        send_to_char("#R(#0WeaponEnhance#R)#0 Raise the average damage on a weapon.#n\n\r", ch);
        send_to_char("#R(#0ArmorEnhance#R)#0 AC bonus to armor.#n\n\r", ch);
        send_to_char("#R(#0Armorcraft#R)#0 Create armor using a slab and a stone.#n\n\r", ch);
        send_to_char("#R(#0SuperBerserk#R)#0 Knowledge of a Combat Technique passed down from Barbarians.#n\n\r", ch);
        }
	send_to_char("[--------------------[  level 50  ]-------------------]\n\r", ch);
        if (ch->pcRaceLevel >= 50)
        {
        send_to_char("#R(#0Flameblade#R)#0 The Power to surround a blade with Flames.#n\n\r", ch);
        }
        send_to_char("[--------------------[Race Powers]--------------------]\n\r", ch);


	return;
}


void do_gainlevel(CHAR_DATA * ch, char *argument)
{
        char      buf[MAX_STRING_LENGTH];
	int lgainexp = 10000 * ch->pcRaceLevel;
	int lgainquest = 400 * ch->pcRaceLevel;

	if( ch->pcRaceLevel >= 200 )
	{
        send_to_char( "Already Maximum Race Level.\n\r", ch );
        return;
	}
	if( ch->exp < lgainexp )
	{
        sprintf( buf, "Not enough Experience points, you need %d more.\n\r", ( lgainexp - ch->exp ) );
        send_to_char( buf, ch );
        return;
        }
        if( ch->pcdata->quest < lgainquest ) 
	{
        sprintf( buf, "Not enough QP, you need %d more.\n\r", ( lgainquest - ch->pcdata->quest ) );
        send_to_char( buf, ch );
        return;
        }
	if (lgainquest > 35000) lgainquest = 30000;

        ch->pcdata->quest -= lgainquest;
        ch->exp -= lgainexp;
        ch->pcRaceLevel++;
        ch->pcpractice += number_range(2,8);
	if (double_pracs == TRUE)
	ch->pcpractice += number_range(2,8);
	
	sprintf( buf, "#0%s is now #RLevel #R[#0%d#R]#n\n\r", ch->pcdata->switchname, ch->pcRaceLevel );
	send_to_char( buf, ch );
        return;
}



void do_raceself(CHAR_DATA *ch, char *argument)
{
  char arg1[MAX_STRING_LENGTH];
  char buf[MSL];
  argument = one_argument( argument, arg1 );

  if (IS_NPC(ch))return;

  if (!IS_NPC(ch) && ch->pcRace != 0 && ch->pcRace != 1)
  {   
    send_to_char("You already have a race.\n\r",ch);
    return;
  }
  if (ch->level < 3)
  {
    send_to_char("You must be avatar to selfrace.\n\r",ch);
    return;
  }
if ( arg1[0] == '\0' )
{
  send_to_char(" #Y*#0Races of cotn#n									\n\r",ch);
  send_to_char(" #Y*#0Human														\n\r",ch);
  send_to_char(" #Y*#0Chimera                           \n\r",ch);
  send_to_char(" #Y*#0Cerberus                           \n\r",ch);
  send_to_char(" #Y*#0Cyclops                           \n\r",ch);
  send_to_char(" #Y*#0Griffin                           \n\r",ch);
  send_to_char(" #Y*#0Harpie                           \n\r",ch);
  send_to_char(" #Y*#0Phoenix                           \n\r",ch);
  send_to_char(" #Y*#0Satyr                           \n\r",ch);
  send_to_char(" #Y*#0Medusa                           \n\r",ch);
  send_to_char(" #Y*#0Titan                           \n\r",ch);
  send_to_char(" #Y*#0Fenris                           \n\r",ch);
  send_to_char(" #Y*#0Nymph                           \n\r",ch);
  send_to_char(" #Y*#0Spectre                           \n\r",ch);
  send_to_char(" #Y*#0Minotaur                           \n\r",ch);
  send_to_char(" #Y*#0Centaur                           \n\r",ch);
  send_to_char(" #Y*#0Valkyrie                           \n\r",ch);
  send_to_char(" #0Type selfrace (race) to choose your race.#n\n\r",ch);
  return;
}


do_clearstats2(ch,"");

	if (ch->pcdata->pc_eyes == 0)
	ch->pcdata->pc_eyes = number_range(1,10);
        if (ch->pcdata->pc_looks == 0)
	ch->pcdata->pc_looks = number_range(1,4);
        if (ch->pcdata->pc_hair == 0)
	ch->pcdata->pc_hair = number_range(1,11);
        if (ch->pcdata->pc_build == 0)
	ch->pcdata->pc_build = number_range(1,4);

if (!str_cmp(arg1,"chimera"))
{
	ch->pcRace = RACE_CHIMERA;
	send_to_char( "#0You have become a Chimera.#n\n\r", ch);
	ch->height = 70;
	ch->weight = 140;
	return;
}
else if (!str_cmp(arg1,"cerberus"))
{
	ch->pcRace = RACE_CERBERUS;
	send_to_char( "#0You have become a Ceberus.#n\n\r", ch);
	ch->height = 70;
	ch->weight = 130;
      return;
}
else if (!str_cmp(arg1,"cyclops"))
{
	ch->pcRace = RACE_CYCLOPS;
	send_to_char( "#0You have become a Cyclops.#n\n\r", ch);
	ch->height = 550;
	ch->weight = 520;
      return;
}
else if (!str_cmp(arg1,"griffin"))
{
	ch->pcRace = RACE_GRIFFIN;
	send_to_char( "#0You have become a Griffin.#n\n\r", ch);
	ch->height = 145;
	ch->weight = 375;
        return;
}
else if (!str_cmp(arg1,"harpie"))
{
	ch->pcRace = RACE_HARPIE;
	send_to_char( "#0You have become a Harpie.#n\n\r", ch);
	ch->height = 50;
	ch->weight = 210;
      return;
}
else if (!str_cmp(arg1,"phoenix"))
{
	ch->pcRace = RACE_PHOENIX;
	send_to_char( "#0You have become a Phoenix.#n\n\r", ch);
	ch->height = 550;
	ch->weight = 900;
        return;
}
else if (!str_cmp(arg1,"satyr"))
{
	ch->pcRace = RACE_SATYR;
	send_to_char( "#0You have become a Satyr.#n\n\r", ch);
	ch->height = 50;
	ch->weight = 170;
        return;
}
else if (!str_cmp(arg1,"medusa"))
{
	ch->pcRace = RACE_MEDUSA;
	send_to_char( "#0You are now a Medusa.#n\n\r", ch);
	ch->height = 70;
	ch->weight = 140;
      return;
}
else if (!str_cmp(arg1,"titan"))
{
	ch->pcRace = RACE_TITAN;
	send_to_char( "You are a Titan.\n\r", ch);
	ch->height = 90;
	ch->weight = 270;
        return;
}
else if (!str_cmp(arg1,"fenris"))
{
	ch->pcRace = RACE_FENRIS;
	send_to_char( "You are a Fenris.\n\r", ch);
	ch->height = 80;
	ch->weight = 220;
        return;
}
else if (!str_cmp(arg1,"minotaur"))
{
	ch->pcRace = RACE_MINOTAUR;
	send_to_char( "#0You grow in stature, Your skin turns red and Bull Horns sprout from your head.#n\n\r", ch);
	ch->height = 75;
	ch->weight = 190;
        return;
}
else if (!str_cmp(arg1,"centaur"))
{
	ch->pcRace = RACE_CENTAUR;
	send_to_char( "#0You are now a Centaur.#n\n\r", ch);
	ch->height = 80;
	ch->weight = 140;
	return;
}
else if (!str_cmp(arg1,"nymph"))
{
	ch->pcRace = RACE_NYMPH;
	send_to_char( "#0You are now a Nymph.#n\n\r", ch);
	ch->height = 53;
	ch->weight = 130;
	return;
}
else if (!str_cmp(arg1,"spectre"))
{
	ch->pcRace = RACE_SPECTRE;
	send_to_char( "#0You are now a Spectre.#n\n\r", ch);
	ch->height = 60;
	ch->weight = 140;
	return;
}
else if (!str_cmp(arg1,"valkyrie"))
{
	ch->pcRace = RACE_VALKYRIE;
	send_to_char( "#0You are now a Valkyrie.#n\n\r", ch);
	ch->height = 50;
	ch->weight = 210;
	return;
}
  else do_raceself(ch,"");   
  return;
}


void do_armorenhance(CHAR_DATA *ch, char *argument)
{
	 OBJ_DATA *obj;
	 int hitroll;
	 int damroll;
	 int armor;



        if (ch->pcRaceLevel < 25)
          {
             send_to_char("huh?\n\r",ch);
             return;
          }
	if ( ch->practice < 1500)
	  {
		 send_to_char("It costs 1500 primal to enhance a weapon.\n\r",ch);
		 return;
	  }
	if (argument[0] == '\0')
	  {
		 send_to_char("Which item do you wish to forge?\n\r", ch);
		 return;
	  }
	if ( (obj = get_obj_carry(ch, argument, ch)) == NULL)
	  {
		 send_to_char("You are not carrying that item.\n\r", ch);
		 return;
	  }
/*	if (IS_SET(obj->quest,QUEST_RELIC) || IS_SET(obj->quest,QUEST_ARTIFACT))
	  {
		send_to_char("Not on a Relic.\n\r",ch);
		return;
	  }
*/
        if (IS_SET(obj->quest2,QUEST2_RITUAL))
          {
                send_to_char("This item has already been through the Ritual!\n\r",ch);
                return;       
	  }
	if (obj->item_type == ITEM_ARMOR)
          {
//                obj->value[0] += 50;
//                oset_affect(ch,obj,30,APPLY_HITROLL,TRUE);
//                oset_affect(ch,obj,30,APPLY_DAMROLL,TRUE);
                oset_affect(ch,obj,-50,APPLY_AC,TRUE);
        hitroll = 30;
        damroll = 30;
        armor   = -50;
        add_affect( obj, APPLY_HITROLL, hitroll );
        add_affect( obj, APPLY_DAMROLL, damroll );
        add_affect( obj, APPLY_AC, armor );

                SET_BIT(obj->quest2,QUEST2_RITUAL);
                obj->condition = 100;
                obj->toughness = 100;
                obj->resistance = 1;
		    act("You bestow power into $p through the Dark Ritual.", ch, obj, NULL, TO_CHAR);
		    act("$n bestows power into $p through the Dark Ritual.", ch, obj, NULL, TO_ROOM);
                ch->practice -= 1500;
                WAIT_STATE(ch, 8);
                return;
          }
          

}
 
void do_flameblade(CHAR_DATA *ch, char *argument)
{
	 OBJ_DATA *obj;

        if (ch->pcRaceLevel < 50)
          {
             send_to_char("huh?\n\r",ch);
             return;
          }
	if ( ch->practice < 1500)
	  {
		 send_to_char("It costs 1500 primal to flameblade a weapon.\n\r",ch);
		 return;
	  }
	if (argument[0] == '\0')
	  {
		 send_to_char("Which item do you wish to forge?\n\r", ch);
		 return;
	  }
	if ( (obj = get_obj_carry(ch, argument, ch)) == NULL)
	  {
		 send_to_char("You are not carrying that item.\n\r", ch);
		 return;
	  }
/*	if (IS_SET(obj->quest,QUEST_RELIC) || IS_SET(obj->quest,QUEST_ARTIFACT))
	  {
		send_to_char("Not on a Relic.\n\r",ch);
		return;
	  }
*/
        if (IS_SET(obj->quest2,QUEST2_FLAMEBLADE))
          {
                send_to_char("This item has already been through the Ritual!\n\r",ch);
                return;
          }
	if (obj->item_type == ITEM_WEAPON)
	  {
		 obj->level     =  50;
		 SET_BIT(obj->quest2,QUEST2_FLAMEBLADE);
		 obj->condition = 100;
		 obj->toughness = 100;
		 obj->resistance = 1;
		 act(" $p is surrounded with Black Flames.", ch, obj, NULL, TO_CHAR);
		 act("$n stares into  $p and it becomes surrounded with black flames.", ch, obj, NULL, TO_ROOM);
		 if(!IS_SET(obj->weapflags, WEAPON_FLAMING)) SET_BIT(obj->weapflags, WEAPON_FLAMING);
		 ch->practice -= 1500;
		 WAIT_STATE(ch, 8); 
		 return;
	  }
}

void do_elveninfra(CHAR_DATA *ch, char *argument)
{
	 OBJ_DATA *obj;

               if (IS_SET(ch->act, PLR_HOLYLIGHT))
                {
                        REMOVE_BIT(ch->act, PLR_HOLYLIGHT);
                        send_to_char("Your Infravision Fades.\n\r",
                                     ch);
                }
                else
                {
                        SET_BIT(ch->act, PLR_HOLYLIGHT);
                        send_to_char("Your eyes glow red with Infravision.\n\r",
                                     ch);
                }
	 return;
}


void do_weaponenhance(CHAR_DATA *ch, char *argument)
{
	 OBJ_DATA *obj;

        if (ch->pcRaceLevel < 25)
          {
             send_to_char("huh?\n\r",ch);
             return;
          }
	if ( ch->practice < 1500)
	  {
		 send_to_char("It costs 1500 primal to enhance a weapon.\n\r",ch);
		 return;
	  }
	if (argument[0] == '\0')
	  {
		 send_to_char("Which item do you wish to forge?\n\r", ch);
		 return;
	  }
	if ( (obj = get_obj_carry(ch, argument, ch)) == NULL)
	  {
		 send_to_char("You are not carrying that item.\n\r", ch);
		 return;
	  }
/*	if (IS_SET(obj->quest,QUEST_RELIC) || IS_SET(obj->quest,QUEST_ARTIFACT))
	  {
		send_to_char("Not on a Relic.\n\r",ch);
		return;
	  }
*/
        if (IS_SET(obj->quest2,QUEST2_RITUAL))
          {
                send_to_char("This item has already been through the Ritual!\n\r",ch);
                return;
          }
	if (obj->item_type == ITEM_WEAPON)
	  {
		 obj->value[1] += 100;
		 obj->value[2] += 100;
		 obj->level     =  50;
		 oset_affect(ch,obj,50,APPLY_HITROLL,TRUE);
		 oset_affect(ch,obj,50,APPLY_DAMROLL,TRUE);
		 SET_BIT(obj->quest2,QUEST2_RITUAL);
		 obj->condition = 100;
		 obj->toughness = 100;
		 obj->resistance = 1;
		 act("You bestow power into $p through the Dark Ritual.", ch, obj, NULL, TO_CHAR);
		 act("$n bestows power into $p through the Dark Ritual.", ch, obj, NULL, TO_ROOM);
		 if(!IS_SET(obj->weapflags, WEAPON_FLAMING)) SET_BIT(obj->weapflags, WEAPON_FLAMING);
		 ch->practice -= 1500;
		 WAIT_STATE(ch, 8); 
		 return;
	  }
}

void do_mistform(CHAR_DATA * ch, char *argument)
{
        if (IS_NPC(ch))
                return;
        if (!IS_RACE(ch, RACE_SPECTRE))
        {
                send_to_char("You need your race level to 50 to use this.\n\r", ch);
                return;
        }
        if (IS_AFFECTED(ch, AFF_ETHEREAL))
        {
                REMOVE_BIT(ch->affected_by, AFF_ETHEREAL);
                act("You shift back int your solid material form.", ch, NULL, NULL,
                    TO_CHAR);
                act("$n regains $s form and re-enters their solid form.", ch, NULL,
                    NULL, TO_ROOM);
                return;
        }
        if (has_timer(ch))
                return;
        SET_BIT(ch->affected_by, AFF_ETHEREAL);
        act("You shift into the ethereal realm.", ch, NULL, NULL, TO_CHAR);
        act("$n turns ethereal.", ch, NULL, NULL, TO_ROOM);
        return;
}


/*
void do_goblingrenade(CHAR_DATA * ch, char *argument)
{
        CHAR_DATA *vch;
        CHAR_DATA *vch_next;
        int       dam;
        int       level;


        if (IS_NPC(ch))
                return;

	if (!IS_RACE(ch, RACE_GOBLIN) && !IS_RACE(ch, RACE_HOBGOBLIN))
	{
                send_to_char("Huh?\n\r", ch);
                return;
        }

        if (ch->mana < 1150)
        {
                send_to_char("You need more mana.\n\r", ch);
                return;
        }

        level = ch->spl[PURPLE_MAGIC];
        ch->mana -= 1150;

        send_to_char
                ("You pull the pin on a Goblin Grenade.\n\r",
                 ch);
        act("$n drops a Goblin Grenade!", ch, NULL, NULL, TO_ROOM);

        for (vch = ch->in_room->people; vch != NULL; vch = vch_next)
        {
                vch_next = vch->next_in_room;

                if (vch == ch)
                        continue;
                if (vch->trust > 6)
                        continue;
                dam = dice(level, 40);
                if (saves_spell(level, vch))
                        dam /= 2;
                damage(ch, vch, dam, skill_lookup("earthquake"));
        }
        WAIT_STATE(ch, 9);
        return;
}


void do_airshiptravel(CHAR_DATA * ch, char *argument)
{
        char      arg[MAX_INPUT_LENGTH];
        CHAR_DATA *victim;

        argument = one_argument(argument, arg);

        if (IS_NPC(ch))
                return;

        if (!IS_RACE(ch, RACE_GNOME))
        {
                send_to_char("Huh?\n\r", ch);
                return;
        }

        if (arg[0] == '\0')
        {
                send_to_char("Airship Travel to who?\n\r", ch);
                return;
        }

        if ((victim = get_char_world(ch, arg)) == NULL)
        {
                send_to_char("They are not here.\n\r", ch);
                return;
        }

        if (victim == ch)
        {
                send_to_char("You cannot travel to yourself.\n\r", ch);
                return;
        }

        if (victim->in_room == NULL)
        {
                send_to_char("Nothing happens.\n\r", ch);
                return;
        }

        send_to_char("You set off a signal flare and an airship picks you up.\n\r", ch);
        act("$n sets off a signal flare and is picked up by an airship.", ch, NULL, NULL, TO_ROOM);
        char_from_room(ch);
        char_to_room(ch, victim->in_room);
        do_look(ch, "");
        send_to_char("You are dropped off by a Gnome Airship.\n\r", ch);
        act("$n is dropped off by a Gnome Airship.", ch, NULL, NULL, TO_ROOM);
        return;
}
*/

void do_thunderhammer(CHAR_DATA * ch, char *argument)
{

	char buf[MSL];
	char buf2[MSL];
	CHAR_DATA *vch;
	CHAR_DATA *vch_next;
	int dam;
	//int dam2;
	dam = ch->pRank * 865;
	dam *= 3;
//	dam /= 2;
	
	if(IS_NPC(ch)) return;
	
	if (!IS_RACE(ch, RACE_CENTAUR))
	{
		stc("Huh?\n\r",ch);
		return;
	}
	for ( vch = ch->in_room->people; vch!=NULL;vch = vch_next)
	      {
	        vch_next = vch->next_in_room;
	        if (can_see(ch,vch))
	        if ( IS_NPC(ch) ? !IS_NPC(vch) : IS_NPC(vch) )
	        {
		if(is_safe(ch,vch)) continue;
	          sprintf(buf, "#0Your body is struck by lightning! [#C%d#n]\n\r", dam);
	          send_to_char( buf, vch );
	          sprintf(buf2, "#r%s's#0 body is struck by lightning! [#C%d#n]\n\r",vch->short_descr, dam);
	          send_to_char( buf2, ch );
	          set_fighting(ch,vch, TRUE);
	          hurt_person(ch,vch,dam);
	        }
	      }
	
	WAIT_STATE(ch,6);

}

void do_superberserk(CHAR_DATA *ch, char *argument)
{
  char arg [MAX_INPUT_LENGTH];
  CHAR_DATA *vch;
  CHAR_DATA *vch_next;
  CHAR_DATA *mount;
  int number_hit = 0;

  argument = one_argument( argument, arg );
  if (IS_NPC(ch)) return;
  if ( ch->level < skill_table[gsn_berserk].skill_level)
  {
    send_to_char( "You are not wild enough to go berserk.\n\r", ch );
    return;
  }

  WAIT_STATE( ch, 6);
  if ( number_percent( ) > ch->pcdata->learned[gsn_berserk] )
  {
    act("You rant and rave, but nothing much happens.",ch,NULL,NULL,TO_CHAR);
    act("$n gets a wild look in $s eyes, but nothing much happens.",ch,NULL,NULL,TO_ROOM);
    return;
  }
	/*if (!IS_RACE(ch, RACE_MINOTAUR) && !IS_RACE(ch, RACE_OGRE)
	&& !IS_RACE(ch, RACE_HUMAN) && !IS_RACE(ch, RACE_TROLL)
	&& !IS_RACE(ch, RACE_HALFTROLL) && !IS_RACE(ch, RACE_HALFOGRE)
	&& !IS_RACE(ch, RACE_ORC) && !IS_RACE(ch, RACE_HALFORC)
	&& !IS_RACE(ch, RACE_GIANT) && !IS_RACE(ch, RACE_CENTAUR)
	&& !IS_RACE(ch, RACE_ARCHDEMON)
	&& !IS_RACE(ch, RACE_ELF) && !IS_RACE(ch, RACE_WILDELF)
	&& !IS_RACE(ch, RACE_HALFELF) && !IS_RACE(ch, RACE_DROW))
        {
                stc("Huh?\n\r",ch);
                return;
        }
*/

  act("#0You Roar with Rage and #7B#wE#7R#wS#7E#wR#7K#0 into a Barbarian style Frenzy.#n",ch,NULL,NULL,TO_CHAR);
  act("#0$n Roars with Rage and #7B#wE#7R#wS#7E#wR#7K#wS#0 into a Barbarian style Frenzy!#n",ch,NULL,NULL,TO_ROOM);
        for (vch = char_list; vch != NULL; vch = vch_next)
        {
                vch_next = vch->next;
                if (vch->in_room == NULL)
                        continue;
                if (!IS_NPC(vch)) continue;
                if (ch == vch)
                {
                        act("You throw back your head and howl with rage!",
                            ch, NULL, NULL, TO_CHAR);
                        continue;
                }
                if (!IS_NPC(vch) && vch->pcdata->chobj != NULL)
                        continue;
                if (!IS_NPC(vch))
                {
//                        if (vch->in_room == ch->in_room)
                        if (vch->in_room->area == ch->in_room->area)
                                send_to_char
                                        ("You hear a Warcry close by!\n\r",
                                         vch);
                        else if (!CAN_PK(vch))
                                continue;
                }
                if (vch->in_room == ch->in_room && can_see(ch, vch))
                {
                        multi_hit(ch, vch, TYPE_UNDEFINED);
                        if (vch == NULL || vch->position <= POS_STUNNED)
                                continue;
                        multi_hit(ch, vch, TYPE_UNDEFINED);
                        multi_hit(ch, vch, TYPE_UNDEFINED);
                }
        }
return;
}




void do_lessons(CHAR_DATA *ch, char *argument)
{
 char buf[MAX_INPUT_LENGTH];
 int cost;
 int cost2;
 cost = ch->pcdata->powers[BARD_LESSONS] * 100000;
 cost2 = cost - ch->exp;



if (argument[0] == '\0')
{
 send_to_char("Options are: show, improve.\n\r", ch);
 return;
}
if (!str_cmp(argument, "show"))
{
 sprintf(buf, "Your skill is currently at %d.\n\r", ch->pcdata->powers[BARD_LESSONS]);
 send_to_char(buf, ch);
}
else if (!str_cmp(argument, "improve"))
{
 if (ch->exp < cost)
 {
  sprintf(buf, "You need %d more experience.\n\r", cost2);
  send_to_char(buf, ch);
  return;
 }
 if (ch->pcdata->powers[BARD_LESSONS] >= 10)
 {
  send_to_char("You're already as skilled as you can become.\n\r", ch);
  return;
 }
 ch->exp -= cost;
 send_to_char("You learn a little more about the arts.\n\r", ch);
 ch->pcdata->powers[BARD_LESSONS]++;
 return;
}
else 
{
 send_to_char("Try improve or show.\n\r", ch);
 return;
}
return;
}

void do_elfsing(CHAR_DATA *ch, char *argument) 
{
 char buf[MAX_INPUT_LENGTH];
 char arg1[MAX_INPUT_LENGTH];
 char arg2[MAX_INPUT_LENGTH];
  int chance;
  int dam[4];
 CHAR_DATA *victim;
 argument = one_argument(argument, arg1);
 argument = one_argument(argument, arg2);



 if (arg1[0] == '\0')
 {
  send_to_char("Syntax: sing (song) (target)\n\r", ch);
  send_to_char("Possible Songs: \n\r", ch);
  send_to_char("Minuet, Minne, Madrigal, March, Dissonance \n\r", ch);
  send_to_char("Requiem, Paeon, Ballad, Lullaby, Hymnus, Spiritual \n\r", 
ch);
  return;
 }
  if (str_cmp(arg1, "minuet") && str_cmp(arg1, "minne") && str_cmp(arg1, "madrigal") &&
str_cmp(arg1, "march") && str_cmp(arg1, "requiem") && str_cmp(arg1, "paeon") &&
str_cmp(arg1, "ballad") && str_cmp(arg1, "lullaby") && str_cmp(arg1, "hymnnus") &&
str_cmp(arg1, "spiritual") && str_cmp(arg1, "dissonance"))
  {
   send_to_char("Possible Songs: Minuet, Minne, Madrigal, March, Requiem, Paeon,\n\r", ch);
   send_to_char("Ballad, Lullaby, Hymnus, Dissonance, Spiritual\n\r", ch);
   return;
  }
  
  if (!str_cmp(arg2, "self") || !str_cmp(arg2, ch->name))
   victim = ch;
  else if ((victim = get_char_room(ch, NULL, arg2)) == NULL)
  {
   send_to_char("They aren't here.\n\r", ch);
   return;
  }
  
  if (ch->pcdata->powers[SONG_MASTERY] < 1)
   ch->pcdata->powers[SONG_MASTERY] = 1;

  chance = number_range(1, ch->pcdata->powers[BARD_LESSONS]);
  if (chance == 1)
  {
   if (ch->pcdata->powers[SONG_MASTERY] < 10)
   {
    send_to_char("Your skill at singing improves.\n\r", ch);
    ch->pcdata->powers[SONG_MASTERY]++;
   }
  }

  if (!str_cmp(arg1, "minuet"))
  {
   if (IS_NPC(victim))
   { 
    send_to_char("Not on NPCs.\n\r", ch);
    return;
   }
   if (victim->pcdata->songs[SONG_MINUET] > 0)
   {
    send_to_char("They're already affected!\n\r", ch);
    return;
   }
   sprintf(buf, "#nYou sing a wonderful #rminuet#n to %s.\n\r", victim->name);
   send_to_char(buf, ch);
   sprintf(buf, "#n%s sings a wonderful #rminuet#n to you.\n\r", ch->name);
   send_to_char(buf, victim);
   victim->pcdata->songs[SONG_MINUET] += (50 * ch->pcdata->powers[SONG_MASTERY]);
   victim->damroll += (50 * ch->pcdata->powers[SONG_MASTERY]);
   return;
  }
  else if (!str_cmp(arg1, "minne"))
  {
   if (IS_NPC(victim))
   {
    send_to_char("Not on NPCs.\n\r", ch);
    return;
   }
   if (ch->pcdata->powers[BARD_LESSONS] < 1)
   {
    send_to_char("You need to take more lessons.\n\r", ch);
    return;
   }
   if(victim->pcdata->songs[SONG_MINNE] > 0)
   {
    send_to_char("They're already affected!\n\r", ch);
    return;
   }
   sprintf(buf, "#nYou sing a wonderful #Lminne#n to %s.\n\r", victim->name);
   send_to_char(buf, ch);
   sprintf(buf, "#n%s sings a wonderful #Lminne#n to you.\n\r", ch->name);
   send_to_char(buf,victim);
   victim->pcdata->songs[SONG_MINNE] += (50 * ch->pcdata->powers[SONG_MASTERY]);
   victim->armor -= (50 * ch->pcdata->powers[SONG_MASTERY]);
   return;
  }
  else if (!str_cmp(arg1, "madrigal"))
  {
   if (IS_NPC(victim))
   {
    send_to_char("Not on NPCs.\n\r", ch);
   return;
   }
   if (ch->pcdata->powers[BARD_LESSONS] < 2)
   { 
    send_to_char("You need to take more lessons.\n\r", ch);
    return;
   }
   if (victim->pcdata->songs[SONG_MADRIGAL] > 0)
   {
    send_to_char("They're already affected!\n\r", ch);
    return;
   }
   sprintf(buf, "#nYou sing a wonderful #Gmadrigal#n to %s.\n\r", victim->name);
   send_to_char(buf, ch);
   sprintf(buf, "#n%s sings a wonderful #Gmadrigal#n to you.\n\r", ch->name);
   send_to_char(buf, victim);
   victim->pcdata->songs[SONG_MADRIGAL] += (50 * ch->pcdata->powers[SONG_MASTERY]);
   victim->hitroll += (50 * ch->pcdata->powers[SONG_MASTERY]);
   return;
  }
  else if (!str_cmp(arg1, "spiritual"))
  {
   if(ch->pcdata->powers[BARD_LESSONS] < 10)
   {
    send_to_char("You need to take more lessons.\n\r", ch);
    return;
   } 
   dam[0] = ch->pcdata->songs[0] + ch->pcdata->songs[1] + ch->pcdata->songs[2] + ch->pcdata->songs[3];
   dam[1] = ch->pcdata->songs[4] + ch->pcdata->songs[5] + ch->pcdata->songs[6] + ch->pcdata->songs[7];
   dam[2] = dam[0] + dam[1];
   dam[3] = dam[2] * 12.5;
   damage(ch, victim, dam[3], gsn_requiem);
   sprintf(buf, "#yYou channel all your #7inspiration#y into one powerful song, hitting #C%s #yincredibly hard. [#L%d#y]#n\n\r", 
   victim->name, dam[3]);
   send_to_char(buf, ch);
   sprintf(buf, "#C%s #ychannels every bit of their #7inspiration #yinto one powerful song, hitting you hard. [#L%d#y]#n\n\r", ch->name, dam[3]);
   send_to_char(buf, victim);
   WAIT_STATE(ch, 36);
   return;
  }
  else if (!str_cmp(arg1, "lullaby"))
  {
   if (IS_NPC(victim))
   {
    send_to_char("Only on PCs.\n\r", ch);
    return;
   }
   if (ch->pcdata->powers[BARD_LESSONS] < 7)
   {
    send_to_char("You need to take more lessons.\n\r", ch);
    return;
   }
   if (victim->pcdata->songs[SONG_LULLABY] > 0)
   {
    send_to_char("They're already asleep.\n\r", ch);
    return;
   }
   sprintf(buf, "#nYou sing a #0lullaby#n to %s, putting them to sleep.\n\r", victim->name);
   send_to_char(buf, ch);
   sprintf(buf, "#n%s sings a #0lullaby#n to you, putting you to sleep.\n\r", ch->name);
   send_to_char(buf, victim);
   victim->position = POS_STUNNED;
  }   
  else if (!str_cmp(arg1, "requiem"))
  {
   if (IS_NPC(victim))
   {
    send_to_char("Only on PCs.\n\r", ch);
    return;
   }
   if (ch->pcdata->powers[BARD_LESSONS] < 4)
   {
    send_to_char("You need to take more lessons.\n\r", ch);
    return;
   }
   if (victim->pcdata->songs[SONG_REQUIEM] > 0)
   {
    send_to_char("They're already suffering.\n\r", ch);
    return;
   }
   sprintf(buf, "#nYou instill within %s a horrific #rrequiem#n.\n\r", victim->name);
   send_to_char(buf, ch);
   sprintf(buf, "#n%s instills within you a horrific #rrequiem#n.\n\r", ch->name);
   send_to_char(buf, victim);
   victim->pcdata->songs[SONG_REQUIEM] += ch->pcdata->powers[SONG_MASTERY];
   return;
  }
  else if (!str_cmp(arg1, "dissonance"))
  {
   if (ch->pcdata->powers[BARD_LESSONS] < 9)
   {
    send_to_char("You need to take more lessons.\n\r", ch);
    return;
   }
   dam[0] = number_range(1, 100);
   dam[1] = number_range(5, 500);
   dam[2] = number_range(9, 900);
   dam[3] = number_range(13, 1300);
   dam[4] = number_range(17, 1700);
   damage(ch, victim, dam[0], gsn_requiem);
   damage(ch, victim, dam[1], gsn_requiem);
   damage(ch, victim, dam[2], gsn_requiem);
   damage(ch, victim, dam[3], gsn_requiem);
   damage(ch, victim, dam[4], gsn_requiem);
   sprintf(buf, "#nThe #7intrdouction#n to your dissonant rush does #y[#L%d#y]#n damage to #C%s.#n\n\r", dam[0], victim->name);
   send_to_char(buf, ch);
   sprintf(buf, "#nThe #7warmup#n to your dissonant attack does #y[#L%d#y]#n damage to #C%s.#n\n\r", dam[1], victim->name);
   send_to_char(buf, ch); 
   sprintf(buf, "#nThe #7body#n of your dissonant onslaught does #y[#L%d#y]#n damage to #C%s.#n\n\r", dam[2], victim->name);
   send_to_char(buf, ch);
   sprintf(buf, "#nThe #7outro#n to your dissonant song does #y[#L%d#y]#n damage to #C%s.#n\n\r", dam[3], victim->name);
   send_to_char(buf, ch);
   sprintf(buf, "#nThe #7aftermath#n of your attack does #y[#L%d#y]#n damage to #C%s.#n\n\r", dam[4], victim->name);
   send_to_char(buf, ch);
   sprintf(buf, "You take #y[#L%d#y]#n damage from %s's dissonant song #7introduction.#m\n\r", dam[0], ch->name);
   send_to_char(buf, victim);
   sprintf(buf, "The #7warmup#n to %s's dissonant attack does #y[#L%d#y]#n damage to you.\n\r", ch->name, dam[1]);
   send_to_char(buf, victim);
   sprintf(buf, "The #7body#n of %s's dissonant onslaught does #[#L%d#y]#n damage to you.\n\r", ch->name, dam[2]);
   send_to_char(buf, victim);
   sprintf(buf, "%s's dissonant song #7outro#n deals #y[#L%d#y]#n damage to you.\n\r", ch->name, dam[3]);
   send_to_char(buf, victim);
   sprintf(buf, "You take #y[#L%d#y]#n damage from %s's dissonant #7aftermath.#n\n\r", dam[4], ch->name);
   send_to_char(buf, victim);
   WAIT_STATE(ch, 36);
   return;
  }
  else if (!str_cmp(arg1, "paeon"))
  {
   if (IS_NPC(victim))
   {
    send_to_char("Only on PCs.\n\r", ch);
    return;
   }
   if (ch->pcdata->powers[BARD_LESSONS] < 5)
   {
    send_to_char("You need to take more lessons.\n\r", ch);
    return;
   }
   if (victim->pcdata->songs[SONG_PAEON] > 0)
   {
    send_to_char("They're regenerating quickly already.\n\r", ch);
    return;
   }
   sprintf(buf, "#nYou successfully cause %s to regenerate quickly with your #Cpaeon#n.\n\r", victim->name);
   send_to_char(buf, ch);
   sprintf(buf, "#n%s successfully causes you to regenerate quickly with their #Cpaeon#n.\n\r", ch->name);
   send_to_char(buf, victim);
   victim->pcdata->songs[SONG_PAEON] += ch->pcdata->powers[SONG_MASTERY];
   return;
  }
  else if (!str_cmp(arg1, "ballad"))
  {
   if (IS_NPC(victim))
   {
    send_to_char("Only on players.\n\r", ch);
    return;
   }
   if (ch->pcdata->powers[BARD_LESSONS] < 6)
   {
    send_to_char("You need to take more lessons.\n\r", ch);
    return;
   }
   if (victim->pcdata->songs[SONG_BALLAD] > 0)
   {
    send_to_char("They're regenerating mana fiercely already.\n\r", ch);
    return;
   }
   sprintf(buf, "#nYou cause %s to regenerate mana fiercely with your #Pballad#n.\n\r", victim->name);
   send_to_char(buf, ch);
   sprintf(buf, "#n%s causes you to regenerate mana fiercely with their #Pballad#n.\n\r", ch->name);
   send_to_char(buf, victim);
   victim->pcdata->songs[SONG_BALLAD] += ch->pcdata->powers[SONG_MASTERY];
   return;
  }
  else if (!str_cmp(arg1, "hymnus"))
  {
   if (IS_NPC(victim))
   {
    send_to_char("Only PCs.\n\r", ch);
    return;
   }
   if (ch->pcdata->powers[BARD_LESSONS] < 8)
   { 
    send_to_char("You need to take more lessons.\n\r", ch);
    return;
   }
   if (victim->pcdata->songs[SONG_HYMNUS] > 0)
   {
    send_to_char("They are already suffering from it.\n\r", ch);
    return;
   }
   sprintf(buf, "#nYou degrade %s and their ability to regenerate mana with your #7hymnus#n.\n\r", victim->name);
   send_to_char(buf, ch);
   sprintf(buf, "#n%s degrades your ability to regenerate mana with their #7hymnus#n.\n\r", victim->name);
   send_to_char(buf, victim);
   victim->pcdata->songs[SONG_HYMNUS] += ch->pcdata->powers[SONG_MASTERY];
   return;
  }
  else if (!str_cmp(arg1, "march"))
  {
   if (IS_NPC(victim))
   {
    send_to_char("Only on players.\n\r", ch);
    return;
   }
   if (ch->pcdata->powers[BARD_LESSONS] < 3)
   {
    send_to_char("You need to take more lessons.\n\r", ch);
    return;
   }
   if (victim->pcdata->songs[SONG_MARCH] > 0)
   {
    send_to_char("They're already marching!\n\r", ch);
    return;
   }
   sprintf(buf, "#nYou sing a wonderful #ymarch#n to %s\n\r", victim->name);
   send_to_char(buf, ch);
   sprintf(buf, "#n%s sings a wonderful #ymarch#n to you.\n\r", ch->name);
   send_to_char(buf, victim);
   victim->pcdata->songs[SONG_MARCH] += (ch->pcdata->powers[SONG_MASTERY]*50);
   return;
  }
 return; 
}

void do_songs(CHAR_DATA *ch, char *argument)
{
 char uta[MAX_INPUT_LENGTH];
 char buf[MAX_INPUT_LENGTH];
 int song = 0;



 for (song = 0; song < 9; song++)
 {
  switch(song)
  {
   case 0:
    sprintf(uta, "Minuet:   ");
    break;
   case 2:
    sprintf(uta, "Madrigal: ");
    break;
   case 1:
    sprintf(uta, "Minne:    ");
    break;
   case 3:
    sprintf(uta, "March:    ");
    break;
   case 4:
    sprintf(uta, "Requiem:  ");
    break;
   case 5:
    sprintf(uta, "Paeon:    ");
    break;
   case 6:
    sprintf(uta, "Ballad:   ");
    break;
   case 7:
    sprintf(uta, "Lullaby:  ");
    break;
   case 8:
    sprintf(uta, "Hymnus:   ");
    break;
  }
  sprintf(buf, "%s %d\n\r", uta, ch->pcdata->songs[song]);
  send_to_char(buf, ch);
 }
 sprintf(buf, "Mastery:   %d\n", ch->pcdata->powers[SONG_MASTERY]);
 send_to_char(buf, ch);
 return;
}

void do_toughskin (CHAR_DATA * ch, char *argument)
{

  if (IS_NPC (ch))
    return;

  if (!IS_SET (ch->newbits2, NEW2_TOUGHSKIN))
    {
      stc ("Your skin grows stronger.\n\r", ch);
      SET_BIT (ch->newbits2, NEW2_TOUGHSKIN);
    }
  else
    {
      stc ("Your skin looses its leathery texture.\n\r", ch);
      REMOVE_BIT (ch->newbits2, NEW2_TOUGHSKIN);

    }
return;
}

