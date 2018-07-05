/***************************************************************************
 *  Original Diku Mud copyright (C) 1990, 1991 by Sebastian Hammer,        *
 *  Michael Seifert, Hans Henrik St{rfeldt, Tom Madsen, and Katja Nyboe.   *
 *                                                                         *
 *  Merc Diku Mud improvments copyright (C) 1992, 1993 by Michael          *
 *  Chastain, Michael Quan, and Mitchell Tse.                              *
 *                                                                         *
 *  In order to use any part of this Merc Diku Mud, you must comply with   *
 *  both the original Diku license in 'license.doc' as well the Merc       *
 *  license in 'license.txt'.  In particular, you may not remove either of *
 *  these copyright notices.                                               *
 *                                                                         *
 *  Dystopia Mud improvements copyright (C) 2000, 2001 by Brian Graversen  *
 *                                                                         *
 *  Much time and thought has gone into this software and you are          *
 *  benefitting.  We hope that you share your changes too.  What goes      *
 *  around, comes around.                                                  *
 ***************************************************************************/
/***************************************************************************
 *                                 _/                            _/        *
 *      _/_/_/  _/_/      _/_/_/  _/    _/_/    _/    _/    _/_/_/         *
 *     _/    _/    _/  _/        _/  _/    _/  _/    _/  _/    _/          *
 *    _/    _/    _/  _/        _/  _/    _/  _/    _/  _/    _/           *
 *   _/    _/    _/    _/_/_/  _/    _/_/      _/_/_/    _/_/_/            *
 ***************************************************************************
 * Mindcloud Copyright 2001-2003 by Jeff Boschee (Zarius),                 *
 * Addition credits are in the help file CODECREDITS                       *
 * All Rights Reserved.                                                    *
 ***************************************************************************/

#include <sys/types.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "merc.h"
#include "interp.h"


/*
 * Local functions.
 */
bool is_in args((char *, char *));
bool all_in args((char *, char *));
char     *socialc
args((CHAR_DATA * ch, char *argument, char *you, char *them));
char     *socialv
args((CHAR_DATA * ch, char *argument, char *you, char *them));
char     *socialn
args((CHAR_DATA * ch, char *argument, char *you, char *them));
bool add_buf args((BUFFER * buffer, char *string));
void chatperform args((CHAR_DATA * ch, CHAR_DATA * victim, char *msg));
// void clear_sigils args((CHAR_DATA *ch));
// void clear_sigils_container args((OBJ_DATA *container, int sigs[8]));


/* Trace's Bounty code */
void do_bounty(CHAR_DATA * ch, char *argument)
{
        char      arg1[MAX_INPUT_LENGTH];
        char      arg2[MAX_INPUT_LENGTH];
        char      buf[MAX_STRING_LENGTH];
        CHAR_DATA *victim;

        argument = one_argument(argument, arg1);
        argument = one_argument(argument, arg2);

        if (IS_NPC(ch))
                return;

        if (arg1[0] == '\0' || arg2[0] == '\0')
        {
                send_to_char
                        ("Place a bounty on who's head?\n\rSyntax:  Bounty <victim> <amount>\n\r",
                         ch);
                return;
        }

        if ((victim = get_char_world(ch, arg1)) == NULL)
        {
                send_to_char("They are currently not logged in!", ch);
                return;
        }

        if (IS_NPC(victim))
        {
                send_to_char("You cannot put a bounty on NPCs!", ch);
                return;
        }
        if (victim == ch)
        {
                stc("Not on yourself\n\r", ch);
                return;
        }

        if (victim->level >= 7)
        {
                send_to_char("You can't put a bounty on an immortal.", ch);
                return;
        }

        if (is_number(arg2))
        {
                int       amount;

                amount = atoi(arg2);
                if (amount < 100)
                {
                        stc("Needs to be at least 1000 Bones, less than that you gotta be kidding.\n\r", ch);
                        return;
                }
                if (ch->bones < amount)
                {
                        send_to_char("You don't have that many Bones!", ch);
                        return;
                }
                ch->bones -= amount;
                victim->pcdata->bounty += amount;
                sprintf(buf, "You have placed a %d Bones bounty on %s.\n\r",
                        amount, victim->name);
                send_to_char(buf, ch);
                sprintf(buf, "%s spends %d Bones on a bounty.\n\r", ch->name,
                        amount);
                do_info(ch, buf);
                sprintf(buf,
                        "%s now has a bounty of %d Bones on their head.\n\r",
                        victim->name, victim->pcdata->bounty);
                do_info(ch, buf);
                return;
        }
}

void room_message(ROOM_INDEX_DATA * room, char *message)
{
        CHAR_DATA *rch;

        if ((rch = room->people) == NULL)
                return;

        act(message, rch, NULL, NULL, TO_ROOM);
        act(message, rch, NULL, NULL, TO_CHAR);
}

void do_roleplay(CHAR_DATA * ch, char *argument)
{
        char      buf[MAX_STRING_LENGTH];

        /*
         * Communication storage system, Ye Jobo
         */
        if (strlen(argument) > 1 && !IS_SET(ch->act, PLR_SILENCE)
            && !IS_NPC(ch))
        {
                sprintf(buf, "#G%-12s#n : #R%s#n.", ch->name, argument);
                update_channel_storage(buf, STORE_ROLEPLAY);
        }
        talk_channel(ch, argument, CHANNEL_ROLEPLAY, 0, "roleplay");
        return;
}

void do_scroll(CHAR_DATA * ch, char *argument)
{
        char      arg[MAX_INPUT_LENGTH];
        char      buf[100];
        int       lines;

        one_argument(argument, arg);

        if (IS_NPC(ch))
                return;

        if (arg[0] == '\0')
        {
                if (ch->pcdata->pagelen == 0)
                        send_to_char("You do not page long messages.\n\r",
                                     ch);
                else
                {
                        sprintf(buf,
                                "You currently display %d lines per page.\n\r",
                                ch->pcdata->pagelen + 2);
                        send_to_char(buf, ch);
                }
                return;
        }

        if (!is_number(arg))
        {
                send_to_char
                        ("You must provide a number. Use 0 to disable.\n\r",
                         ch);
                return;
        }

        lines = atoi(arg);

        if (lines == 0)
        {
                send_to_char("Paging disabled.\n\r", ch);
                ch->pcdata->pagelen = 0;
                return;
        }

        if (lines < 10 || lines > 50)
        {
                send_to_char
                        ("You must provide a number between 10 and 50.\n\r",
                         ch);
                return;
        }

        sprintf(buf, "Scroll set to %d lines.\n\r", lines);
        send_to_char(buf, ch);
        ch->pcdata->pagelen = lines - 2;
}


void do_chat(CHAR_DATA * ch, char *argument)
{
        char      buf[MAX_STRING_LENGTH];

        /*
         * Communication storage system, Ye Jobo 
         */
        if (strlen(argument) > 1 && !IS_SET(ch->act, PLR_SILENCE))
        {
                sprintf(buf, "#G%-12s#n : #R%s#n.", ch->name, argument);
                update_channel_storage(buf, STORE_CHAT);
        }
        talk_channel(ch, argument, CHANNEL_CHAT, 0, "chat");
        //mud_message(ch, argument);
        return;
}

void do_flame(CHAR_DATA * ch, char *argument)
{
        char      buf[MAX_STRING_LENGTH];

        /*
         * Communication storage system, Ye Jobo
         */
        if (strlen(argument) > 1 && !IS_SET(ch->act, PLR_SILENCE)
            && !IS_NPC(ch))
        {
                sprintf(buf, "#G%-12s#n : #R%s#n.", ch->name, argument);
                update_channel_storage(buf, STORE_FLAME);
        }
        talk_channel(ch, argument, CHANNEL_FLAME, 0, "bitch");
        return;
}

void do_qtalk(CHAR_DATA * ch, char *argument)
{
        talk_channel(ch, argument, CHANNEL_QUEST, 0, "quest");
        if ((!str_cmp(argument, "what mob?"))
            || (!str_cmp(argument, "who was the thief?"))
            || (!str_cmp(argument, "who is the thief?"))
            || (!str_cmp(argument, "what item?"))
            || (!str_cmp(argument, "where are you?"))
            || (!str_cmp(argument, "who stole the item?"))
            || (!str_cmp(argument, "where is the thief?")))
                ask_quest_question(ch, argument);
        return;
}

void do_trivia(CHAR_DATA * ch, char *argument)
{
        char      buf[MAX_STRING_LENGTH];

        /*
         * Communication storage system, Ye Jobo
         */
        if (strlen(argument) > 1 && !IS_SET(ch->act, PLR_SILENCE)
            && !IS_NPC(ch))
        {
                sprintf(buf, "#G%-12s#n : #R%s#n.", ch->name, argument);
                update_channel_storage(buf, STORE_TRIVIA);
        }
        talk_channel(ch, argument, CHANNEL_TRIVIA, 0, "trivia");
        return;
}

/*
 * Alander's new channels.
 */
void do_sing(CHAR_DATA * ch, char *argument)
{
        talk_channel(ch, argument, CHANNEL_MUSIC, 0, "sing");

        return;
}


void do_yell(CHAR_DATA * ch, char *argument)
{

        talk_channel(ch, argument, CHANNEL_YELL, 0, "yell");

        return;
}



void do_immtalk(CHAR_DATA * ch, char *argument)
{
        talk_channel(ch, argument, CHANNEL_IMMTALK, 0, "immtalk");
        return;
}

void do_monktalk(CHAR_DATA * ch, char *argument)
{
        int       class = ch->class;

        if (IS_NPC(ch) || (!IS_IMMORTAL(ch) && !IS_CLASS(ch, CLASS_MONK)))
        {
                send_to_char("Huh?\n\r", ch);
                return;
        }
        ch->class = CLASS_MONK;
        talk_channel(ch, argument, CHANNEL_CLASS, CC_MONK, "monktalk");
        ch->class = class;
        return;
}

void do_miktalk(CHAR_DATA * ch, char *argument)
{
        int       class = ch->class;

        if (IS_NPC(ch) || (!IS_IMMORTAL(ch) && !IS_CLASS(ch, CLASS_NINJA)))
        {
                send_to_char("Huh?\n\r", ch);
                return;
        }
        ch->class = CLASS_NINJA;
        talk_channel(ch, argument, CHANNEL_CLASS, CC_NINJA, "ninjatalk");
        ch->class = class;
        return;
}

void do_hightalk(CHAR_DATA * ch, char *argument)
{
        int       class = ch->class;

        if (IS_NPC(ch) || (!IS_IMMORTAL(ch) && !IS_CLASS(ch, CLASS_SAMURAI)))
        {
                send_to_char("Huh?\n\r", ch);
                return;
        }
        ch->class = CLASS_SAMURAI;
        talk_channel(ch, argument, CHANNEL_CLASS, CC_SAMURAI, "samtalk");
        ch->class = class;
        return;
}
void do_knighttalk(CHAR_DATA * ch, char *argument)
{
        int       class = ch->class;

        if (IS_NPC(ch)
            || (!IS_IMMORTAL(ch) && !IS_CLASS(ch, CLASS_UNDEAD_KNIGHT)))
        {
                send_to_char("Huh?\n\r", ch);
                return;
        }
        ch->class = CLASS_UNDEAD_KNIGHT;
        talk_channel(ch, argument, CHANNEL_CLASS, CC_KNIGHT, "knighttalk");
        ch->class = class;
        return;
}

void do_drool(CHAR_DATA * ch, char *argument)
{
        int       class = ch->class;

        if (IS_NPC(ch) || (!IS_IMMORTAL(ch) && !IS_CLASS(ch, CLASS_DRONE)))
        {
                send_to_char("Huh?\n\r", ch);
                return;
        }
        ch->class = CLASS_DRONE;
        talk_channel(ch, argument, CHANNEL_CLASS, CC_DRONE, "drool");
        ch->class = class;
        return;
}

void do_sign(CHAR_DATA * ch, char *argument)
{
        int       class = ch->class;

        if (IS_NPC(ch) || (!IS_IMMORTAL(ch) && !IS_CLASS(ch, CLASS_DROW)))
        {
                send_to_char("Huh?\n\r", ch);
                return;
        }
        ch->class = CLASS_DROW;
        talk_channel(ch, argument, CHANNEL_CLASS, CC_DROW, "sign");
        ch->class = class;
        return;
}

void do_ktalk(CHAR_DATA * ch, char *argument)
{

        if (IS_NPC(ch) || ch->pcdata->kingdom == 0)
        {
                send_to_char("Huh?\n\r", ch);
                return;
        }
        talk_channel(ch, argument, CHANNEL_KINGDOM, 0, "ktalk");
        return;
}

void do_reltalk(CHAR_DATA * ch, char *argument)
{

        if (IS_NPC(ch) || ch->pcdata->religion == 0)
        {
                send_to_char("Huh?\n\r", ch);
                return;
        }
        talk_channel(ch, argument, CHANNEL_RELIGION, 0, "reltalk");
        return;
}

void do_vamptalk(CHAR_DATA * ch, char *argument)
{
        int       class = ch->class;

        if (IS_NPC(ch) || (!IS_IMMORTAL(ch) && !IS_CLASS(ch, CLASS_VAMPIRE)))

        {
                send_to_char("Huh?\n\r", ch);
                return;
        }
        ch->class = CLASS_VAMPIRE;
        talk_channel(ch, argument, CHANNEL_CLASS, CC_VAMPIRE, "vamptalk");
        ch->class = class;
        return;
}

void do_magetalk(CHAR_DATA * ch, char *argument)
{
        int       class = ch->class;

        if (IS_NPC(ch) || (!IS_IMMORTAL(ch) && !IS_CLASS(ch, CLASS_MAGE)))
        {
                send_to_char("Huh?\n\r", ch);
                return;
        }
        ch->class = CLASS_MAGE;
        talk_channel(ch, argument, CHANNEL_CLASS, CC_MAGE, "magetalk");
        ch->class = class;
        return;
}

void do_lichtalk(CHAR_DATA * ch, char *argument)
{
        int       class = ch->class;

        if (IS_NPC(ch) || (!IS_IMMORTAL(ch) && !IS_CLASS(ch, CLASS_LICH)))
        {
                send_to_char("Huh?\n\r", ch);
                return;
        }
        ch->class = CLASS_LICH;
        talk_channel(ch, argument, CHANNEL_CLASS, CC_LICH, "lichtalk");
        ch->class = class;
        return;
}

void do_tantalk(CHAR_DATA * ch, char *argument)
{
        int       class = ch->class;

        if (IS_NPC(ch) || (!IS_IMMORTAL(ch) && !IS_CLASS(ch, CLASS_TANARRI)))
        {
                send_to_char("Huh?\n\r", ch);
                return;
        }
        ch->class = CLASS_TANARRI;
        talk_channel(ch, argument, CHANNEL_CLASS, CC_TANARRI, "tantalk");
        ch->class = class;
        return;
}

void do_faetalk(CHAR_DATA * ch, char *argument)
{
        int       class = ch->class;

        if (IS_NPC(ch) || (!IS_IMMORTAL(ch) && !IS_CLASS(ch, CLASS_FAE)))
        {
                send_to_char("Huh?\n\r", ch);
                return;
        }
        ch->class = CLASS_FAE;
        talk_channel(ch, argument, CHANNEL_CLASS, CC_FAE, "faetalk");
        ch->class = class;
        return;
}

void do_rumble(CHAR_DATA * ch, char *argument)
{
        int       class = ch->class;

        if (IS_NPC(ch) || (!IS_IMMORTAL(ch) && !IS_CLASS(ch, CLASS_GIANT)))
        {
                send_to_char("Huh?\n\r", ch);
                return;
        }
        ch->class = CLASS_GIANT;
        talk_channel(ch, argument, CHANNEL_CLASS, CC_GIANT, "rumble");
        ch->class = class;
        return;
}

void do_hobtalk(CHAR_DATA * ch, char *argument)
{
        int       class = ch->class;

        if (IS_NPC(ch) || (!IS_IMMORTAL(ch) && !IS_CLASS(ch, CLASS_HOBBIT)))
        {
                send_to_char("Huh?\n\r", ch);
                return;
        }
        ch->class = CLASS_HOBBIT;
        talk_channel(ch, argument, CHANNEL_CLASS, CC_HOBBIT, "hobtalk");
        ch->class = class;
        return;
}

void do_bladetalk(CHAR_DATA * ch, char *argument)
{
        int       class = ch->class;

        if (IS_NPC(ch) || (!IS_IMMORTAL(ch) && !IS_CLASS(ch, CLASS_SKYBLADE)))
        {
                send_to_char("Huh?\n\r", ch);
                return;
        }
        ch->class = CLASS_SKYBLADE;
        talk_channel(ch, argument, CHANNEL_CLASS, CC_SKYBLADE, "bladetalk");
        ch->class = class;
        return;
}

void do_monk(CHAR_DATA * ch, char *argument)
{
        int       class = ch->class;

        if (IS_NPC(ch) || (!IS_IMMORTAL(ch) && !IS_CLASS(ch, CLASS_MONK)))
        {
                send_to_char("Huh?\n\r", ch);
                return;
        }
        ch->class = CLASS_MONK;
        talk_channel(ch, argument, CHANNEL_CLASS, CC_MONK, "monktalk");
        ch->class = class;
        return;
}
void do_zomtalk( CHAR_DATA *ch, char *argument )
{

	if (IS_NPC(ch) || (!IS_IMMORTAL(ch) && !IS_CLASS(ch, CLASS_ZOMBIE)))
	{send_to_char("Huh?\n\r", ch);
	return;}
	talk_channel(ch, argument, CHANNEL_CLASS, CC_ZOMBIE, "moan");
	return;
}

void do_droar( CHAR_DATA *ch, char *argument )
{
   if (IS_NPC(ch) || (!IS_IMMORTAL(ch) && !IS_CLASS(ch, CLASS_DRAGON)))
   {
       send_to_char("huh?\n\r",ch);
       return;
   }
   talk_channel(ch, argument, CHANNEL_CLASS, CC_DRAGON,  "droar");
   return;
}
void do_angeltalk(CHAR_DATA * ch, char *argument)
{
        int       class = ch->class;

        if (IS_NPC(ch) || (!IS_IMMORTAL(ch) && !IS_CLASS(ch, CLASS_ANGEL)))
        {
                send_to_char("Huh?\n\r", ch);
                return;
        }
        ch->class = CLASS_ANGEL;
        talk_channel(ch, argument, CHANNEL_CLASS, CC_ANGEL, "angeltalk");
        ch->class = class;
        return;
}

void do_pray(CHAR_DATA * ch, char *argument)
{
        int       class = ch->class;

        if (IS_NPC(ch) || (!IS_IMMORTAL(ch) && !IS_CLASS(ch, CLASS_DEMON)))
        {
                send_to_char("Huh?\n\r", ch);
                return;
        }
        ch->class = CLASS_DEMON;
        talk_channel(ch, argument, CHANNEL_CLASS, CC_DEMON, "demontalk");
        ch->class = class;
        return;
}

void do_howl(CHAR_DATA * ch, char *argument)
{
        int       class = ch->class;

        if (IS_NPC(ch) || (!IS_IMMORTAL(ch) && !IS_CLASS(ch, CLASS_WEREWOLF)))
        {
                send_to_char("Huh?\n\r", ch);
                return;
        }
        ch->class = CLASS_WEREWOLF;
        talk_channel(ch, argument, CHANNEL_CLASS, CC_WW, "howl");
        ch->class = class;
        return;
}



void do_say(CHAR_DATA * ch, char *argument)
{
        char      name[80];
        char      poly[MAX_STRING_LENGTH];
        char      speak[10];
        char      speaks[10];
        char      endbit[2];
        char      secbit[2];
        CHAR_DATA *to;
        bool      is_ok;

        if (RTIMER(ch->in_room, RTIMER_SILENCE) != 0)
        {
                send_to_char
                        ("Something prevents you from speaking in this room.\n\r",
                         ch);
                return;
        }
        if (IS_HEAD(ch, LOST_TONGUE))
        {
                send_to_char("You can't speak without a tongue!\n\r", ch);
                return;
        }
        if (IS_EXTRA(ch, GAGGED))
        {
                send_to_char("You can't speak with a gag on!\n\r", ch);
                return;
        }

        if (strlen(argument) > MAX_INPUT_LENGTH)
        {
                send_to_char("Line too long.\n\r", ch);
                return;
        }

        if (argument[0] == '\0')
        {
                send_to_char("Say what?\n\r", ch);
                return;
        }

        endbit[0] = argument[strlen(argument) - 1];
        endbit[1] = '\0';

        if (strlen(argument) > 1)
                secbit[0] = argument[strlen(argument) - 2];
        else
                secbit[0] = '\0';
        secbit[1] = '\0';

        if (IS_BODY(ch, CUT_THROAT))
        {
                sprintf(speak, "rasp");
                sprintf(speaks, "rasps");
        }
        else if (!IS_NPC(ch) &&
                 (IS_SET(ch->special, SPC_WOLFMAN)
                  || IS_POLYAFF(ch, POLY_WOLF) || (IS_CLASS(ch, CLASS_VAMPIRE)
                                                   && ch->pcdata->
                                                   stats[UNI_RAGE] > 0)))
        {
                if (number_percent() > 50)
                {
                        sprintf(speak, "growl");
                        sprintf(speaks, "growls");
                }
                else
                {
                        sprintf(speak, "snarl");
                        sprintf(speaks, "snarls");
                }
        }
        else if (!IS_NPC(ch) && IS_POLYAFF(ch, POLY_BAT))
        {
                sprintf(speak, "squeak");
                sprintf(speaks, "squeaks");
        }
        else if (!IS_NPC(ch) && IS_POLYAFF(ch, POLY_SERPENT))
        {
                sprintf(speak, "hiss");
                sprintf(speaks, "hisses");
        }
        else if (!IS_NPC(ch) && IS_POLYAFF(ch, POLY_FROG))
        {
                sprintf(speak, "croak");
                sprintf(speaks, "croaks");
        }
        else if (!IS_NPC(ch) && IS_POLYAFF(ch, POLY_RAVEN))
        {
                sprintf(speak, "squark");
                sprintf(speaks, "squarks");
        }
        else if (IS_NPC(ch) && ch->pIndexData->vnum == MOB_VNUM_FROG)
        {
                sprintf(speak, "croak");
                sprintf(speaks, "croaks");
        }
        else if (IS_NPC(ch) && ch->pIndexData->vnum == MOB_VNUM_RAVEN)
        {
                sprintf(speak, "squark");
                sprintf(speaks, "squarks");
        }
        else if (IS_NPC(ch) && ch->pIndexData->vnum == MOB_VNUM_CAT)
        {
                sprintf(speak, "purr");
                sprintf(speaks, "purrs");
        }
        else if (IS_NPC(ch) && ch->pIndexData->vnum == MOB_VNUM_DOG)
        {
                sprintf(speak, "bark");
                sprintf(speaks, "barks");
        }
        else if (!str_cmp(endbit, "!"))
        {
                sprintf(speak, "exclaim");
                sprintf(speaks, "exclaims");
        }
        else if (!str_cmp(endbit, "?"))
        {
                sprintf(speak, "ask");
                sprintf(speaks, "asks");
        }
        else if (secbit[0] != '\0' && str_cmp(secbit, ".")
                 && !str_cmp(endbit, "."))
        {
                sprintf(speak, "state");
                sprintf(speaks, "states");
        }
        else if (secbit[0] != '\0' && !str_cmp(secbit, ".")
                 && !str_cmp(endbit, "."))
        {
                sprintf(speak, "mutter");
                sprintf(speaks, "mutters");
        }
        else if (!IS_NPC(ch) && ch->pcdata->condition[COND_DRUNK] > 10)
        {
                sprintf(speak, "slur");
                sprintf(speaks, "slurs");
        }
        else
        {
                sprintf(speak, "say");
                sprintf(speaks, "says");
        }
        sprintf(poly, "#BYou %s '#W$T#B'#n.", speak);

        argument = makedrunk(argument, ch);

        act(poly, ch, NULL, argument, TO_CHAR);

        if (!IS_NPC(ch) && IS_SET(ch->act, PLR_SILENCE))
                return;

        sprintf(poly, "#B$n %s '#W$T#B'#n.", speaks);
        if (ch->in_room->vnum != ROOM_VNUM_IN_OBJECT)
        {
                act(poly, ch, NULL, argument, TO_ROOM);
                // Mobprog
                if (!IS_NPC(ch))
                {
                        CHAR_DATA *mob, *mob_next;
                        OBJ_DATA *obj, *obj_next;

                        for (mob = ch->in_room->people; mob != NULL;
                             mob = mob_next)
                        {
                                mob_next = mob->next_in_room;
                                if (IS_NPC(mob)
                                    && HAS_TRIGGER_MOB(mob, TRIG_SPEECH)
                                    && mob->position ==
                                    mob->pIndexData->default_pos)
                                        p_act_trigger(argument, mob, NULL,
                                                      NULL, ch, NULL, NULL,
                                                      TRIG_SPEECH);
                                if (IS_NPC(mob)
                                    && HAS_TRIGGER_MOB(mob, TRIG_REGEX))
                                        mp_regex_trigger(argument, mob, ch,
                                                         NULL, NULL);
                                for (obj = mob->carrying; obj; obj = obj_next)
                                {
                                        obj_next = obj->next_content;
                                        if (HAS_TRIGGER_OBJ(obj, TRIG_SPEECH))
                                                p_act_trigger(argument, NULL,
                                                              obj, NULL, ch,
                                                              NULL, NULL,
                                                              TRIG_SPEECH);
                                }
                        }
                        for (obj = ch->in_room->contents; obj; obj = obj_next)
                        {
                                obj_next = obj->next_content;
                                if (HAS_TRIGGER_OBJ(obj, TRIG_SPEECH))
                                        p_act_trigger(argument, NULL, obj,
                                                      NULL, ch, NULL, NULL,
                                                      TRIG_SPEECH);
                        }

                        if (HAS_TRIGGER_ROOM(ch->in_room, TRIG_SPEECH))
                                p_act_trigger(argument, NULL, NULL,
                                              ch->in_room, ch, NULL, NULL,
                                              TRIG_SPEECH);

                }
                //end mobprog
                room_text(ch, strlower(argument));
                mob_text(ch, strlower(argument));
                return;
        }

        to = ch->in_room->people;
        for (; to != NULL; to = to->next_in_room)
        {
                is_ok = FALSE;

                if (to->desc == NULL || !IS_AWAKE(to))
                        continue;

                if (ch == to)
                        continue;

                if (!IS_NPC(ch) && ch->pcdata->chobj != NULL &&
                    ch->pcdata->chobj->in_room != NULL &&
                    !IS_NPC(to) && to->pcdata->chobj != NULL &&
                    to->pcdata->chobj->in_room != NULL &&
                    ch->in_room == to->in_room)
                        is_ok = TRUE;
                else
                        is_ok = FALSE;

                if (!IS_NPC(ch) && ch->pcdata->chobj != NULL &&
                    ch->pcdata->chobj->in_obj != NULL &&
                    !IS_NPC(to) && to->pcdata->chobj != NULL &&
                    to->pcdata->chobj->in_obj != NULL &&
                    ch->pcdata->chobj->in_obj == to->pcdata->chobj->in_obj)
                        is_ok = TRUE;
                else
                        is_ok = FALSE;

                if (!is_ok)
                        continue;

                if (IS_NPC(ch))
                        sprintf(name, "%s", ch->short_descr);
                else if (!IS_NPC(ch) && IS_AFFECTED(ch, AFF_POLYMORPH))
                        sprintf(name, "%s", ch->morph);
                else if (!IS_NPC(ch) && IS_AFFECTED(ch, AFF_SHIFT))
                        sprintf(name, "%s", ch->morph);


                else
                        sprintf(name, "%s", ch->name);
                name[0] = UPPER(name[0]);
                sprintf(poly, "%s %s '%s'.\n\r", name, speaks, argument);
                send_to_char(poly, to);
        }
        // Mobprog
        if (!IS_NPC(ch))
        {
                CHAR_DATA *mob, *mob_next;
                OBJ_DATA *obj, *obj_next;

                for (mob = ch->in_room->people; mob != NULL; mob = mob_next)
                {
                        mob_next = mob->next_in_room;
                        if (IS_NPC(mob) && HAS_TRIGGER_MOB(mob, TRIG_SPEECH)
                            && mob->position == mob->pIndexData->default_pos)
                                p_act_trigger(argument, mob, NULL, NULL, ch,
                                              NULL, NULL, TRIG_SPEECH);
                        if (IS_NPC(mob) && HAS_TRIGGER_MOB(mob, TRIG_REGEX))
                                mp_regex_trigger(argument, mob, ch, NULL,
                                                 NULL);
                        for (obj = mob->carrying; obj; obj = obj_next)
                        {
                                obj_next = obj->next_content;
                                if (HAS_TRIGGER_OBJ(obj, TRIG_SPEECH))
                                        p_act_trigger(argument, NULL, obj,
                                                      NULL, ch, NULL, NULL,
                                                      TRIG_SPEECH);
                        }
                }
                for (obj = ch->in_room->contents; obj; obj = obj_next)
                {
                        obj_next = obj->next_content;
                        if (HAS_TRIGGER_OBJ(obj, TRIG_SPEECH))
                                p_act_trigger(argument, NULL, obj, NULL, ch,
                                              NULL, NULL, TRIG_SPEECH);
                }

                if (HAS_TRIGGER_ROOM(ch->in_room, TRIG_SPEECH))
                        p_act_trigger(argument, NULL, NULL, ch->in_room, ch,
                                      NULL, NULL, TRIG_SPEECH);

        }
        //end mobprog
        room_text(ch, strlower(argument));
        mob_text(ch, strlower(argument));
        return;
}

void room_text(CHAR_DATA * ch, char *argument)
{
        CHAR_DATA *vch;
        CHAR_DATA *vch_next;
        CHAR_DATA *mob;
        OBJ_DATA *obj;
        ROOMTEXT_DATA *rt;
        bool      mobfound;
        bool      hop;

        char      arg[MAX_INPUT_LENGTH];
        char      arg1[MAX_INPUT_LENGTH];
        char      arg2[MAX_INPUT_LENGTH];

        for (rt = ch->in_room->roomtext; rt != NULL; rt = rt->next)
        {
                if (!strcmp(argument, rt->input)
                    || is_in(argument, rt->input)
                    || all_in(argument, rt->input))
                {
                        if (rt->name != NULL && rt->name != '\0'
                            && str_cmp(rt->name, "all")
                            && str_cmp(rt->name, "|all*"))
                                if (!is_in(ch->name, rt->name))
                                        continue;
                        mobfound = TRUE;
                        if (rt->mob != 0)
                        {
                                mobfound = FALSE;
                                for (vch = char_list; vch != NULL;
                                     vch = vch_next)
                                {
                                        vch_next = vch->next;
                                        if (vch->in_room == NULL)
                                                continue;
                                        if (!IS_NPC(vch))
                                                continue;
                                        if (vch->in_room == ch->in_room
                                            && vch->pIndexData->vnum ==
                                            rt->mob)
                                        {
                                                mobfound = TRUE;
                                                break;
                                        }
                                }
                        }
                        if (!mobfound)
                                continue;
                        hop = FALSE;
                        switch (rt->type % RT_RETURN)
                        {
                        case RT_SAY:
                                break;
                        case RT_LIGHTS:
                                do_changelight(ch, "");
                                break;
                        case RT_LIGHT:
                                REMOVE_BIT(ch->in_room->room_flags,
                                           ROOM_DARK);
                                break;
                        case RT_DARK:
                                SET_BIT(ch->in_room->room_flags, ROOM_DARK);
                                break;
                        case RT_OBJECT:
                                if (get_obj_index(rt->power) == NULL)
                                        return;
                                obj = create_object(get_obj_index(rt->power),
                                                    ch->level);
                                if (IS_SET(rt->type, RT_TIMER))
                                        obj->timer = 1;
                                if (CAN_WEAR(obj, ITEM_TAKE))
                                        obj_to_char(obj, ch);
                                else
                                        obj_to_room(obj, ch->in_room);
                                if (!str_cmp(rt->choutput, "copy"))
                                        act(rt->output, ch, obj, NULL,
                                            TO_CHAR);
                                else
                                        act(rt->choutput, ch, obj, NULL,
                                            TO_CHAR);
                                if (!IS_SET(rt->type, RT_PERSONAL))
                                        act(rt->output, ch, obj, NULL,
                                            TO_ROOM);
                                hop = TRUE;
                                break;
                        case RT_MOBILE:
                                if (get_mob_index(rt->power) == NULL)
                                        return;
                                mob = create_mobile(get_mob_index(rt->power));
                                char_to_room(mob, ch->in_room);
                                if (!str_cmp(rt->choutput, "copy"))
                                        act(rt->output, ch, NULL, mob,
                                            TO_CHAR);
                                else
                                        act(rt->choutput, ch, NULL, mob,
                                            TO_CHAR);
                                if (!IS_SET(rt->type, RT_PERSONAL))
                                        act(rt->output, ch, NULL, mob,
                                            TO_ROOM);
                                hop = TRUE;
                                break;
                        case RT_SPELL:
                                (*skill_table[rt->power].spell_fun) (rt->
                                                                     power,
                                                                     number_range
                                                                     (20, 30),
                                                                     ch, ch);
                                break;
                        case RT_PORTAL:
                                if (get_obj_index(OBJ_VNUM_PORTAL) == NULL)
                                        return;
                                obj = create_object(get_obj_index
                                                    (OBJ_VNUM_PORTAL), 0);
                                obj->timer = 1;
                                obj->value[0] = rt->power;
                                obj->value[1] = 1;
                                obj_to_room(obj, ch->in_room);
                                break;
                        case RT_TELEPORT:
                                if (get_room_index(rt->power) == NULL)
                                        return;
                                if (!str_cmp(rt->choutput, "copy"))
                                        act(rt->output, ch, NULL, NULL,
                                            TO_CHAR);
                                else
                                        act(rt->choutput, ch, NULL, NULL,
                                            TO_CHAR);
                                if (!IS_SET(rt->type, RT_PERSONAL))
                                        act(rt->output, ch, NULL, NULL,
                                            TO_ROOM);
                                char_from_room(ch);
                                char_to_room(ch, get_room_index(rt->power));
                                act("$n appears in the room.", ch, NULL, NULL,
                                    TO_ROOM);
                                do_look(ch, "auto");
                                hop = TRUE;
                                break;
                        case RT_ACTION:
                                sprintf(arg, "%s", argument);
                                argument = one_argument(arg, arg1);
                                argument = one_argument(arg, arg2);
                                if ((mob =
                                     get_char_room(ch, NULL, arg2)) == NULL)
                                        continue;
                                interpret(mob, rt->output);
                                break;
                        case RT_OPEN_LIFT:
                                open_lift(ch);
                                break;
                        case RT_CLOSE_LIFT:
                                close_lift(ch);
                                break;
                        case RT_MOVE_LIFT:
                                move_lift(ch, rt->power);
                                break;
                        default:
                                break;
                        }
                        if (hop && IS_SET(rt->type, RT_RETURN))
                                return;
                        else if (hop)
                                continue;
                        if (!str_cmp(rt->choutput, "copy")
                            && !IS_SET(rt->type, RT_ACTION))
                                act(rt->output, ch, NULL, NULL, TO_CHAR);
                        else if (!IS_SET(rt->type, RT_ACTION))
                                act(rt->choutput, ch, NULL, NULL, TO_CHAR);
                        if (!IS_SET(rt->type, RT_PERSONAL)
                            && !IS_SET(rt->type, RT_ACTION))
                                act(rt->output, ch, NULL, NULL, TO_ROOM);
                        if (IS_SET(rt->type, RT_RETURN))
                                return;
                }
        }
        return;
}

char     *strlower(char *ip)
{
        static char buffer[MAX_INPUT_LENGTH];
        int       pos;

        for (pos = 0; pos < (MAX_INPUT_LENGTH - 1) && ip[pos] != '\0'; pos++)
        {
                buffer[pos] = tolower(ip[pos]);
        }
        buffer[pos] = '\0';
        return buffer;
}

bool is_in(char *arg, char *ip)
{
        char     *lo_arg;
        char      cmp[MAX_INPUT_LENGTH];
        int       fitted;

        if (ip[0] != '|')
                return FALSE;
        cmp[0] = '\0';
        lo_arg = strlower(arg);
        do
        {
                ip += strlen(cmp) + 1;
                fitted = sscanf(ip, "%[^*]", cmp);
                if (strstr(lo_arg, cmp) != NULL)
                {
                        return TRUE;
                }
        }
        while (fitted > 0);
        return FALSE;
}

bool all_in(char *arg, char *ip)
{
        char     *lo_arg;
        char      cmp[MAX_INPUT_LENGTH];
        int       fitted;

        if (ip[0] != '&')
                return FALSE;
        cmp[0] = '\0';
        lo_arg = strlower(arg);
        do
        {
                ip += strlen(cmp) + 1;
                fitted = sscanf(ip, "%[^*]", cmp);
                if (strstr(lo_arg, cmp) == NULL)
                {
                        return FALSE;
                }
        }
        while (fitted > 0);
        return TRUE;
}

void do_tell(CHAR_DATA * ch, char *argument)
{
        char      arg[MAX_INPUT_LENGTH];
        char      buf[MSL];
        char      poly[MAX_STRING_LENGTH];
        CHAR_DATA *victim;
        int       position;
        int       i;

        if (IS_EXTRA(ch, GAGGED))
        {
                send_to_char("Your message didn't get through.\n\r", ch);
                return;
        }

        argument = one_argument(argument, arg);

        if (arg[0] == '\0' || argument[0] == '\0')
        {
                send_to_char("Tell whom what?\n\r", ch);
                return;
        }

        /*
         * Can tell to PC's anywhere, but NPC's only in same room.
         * -- Furey
         */
        if ((victim = get_char_world(ch, arg)) == NULL
            || (IS_NPC(victim) && victim->in_room != ch->in_room))
        {
                send_to_char("They aren't here.\n\r", ch);
                return;
        }

        if (!IS_IMMORTAL(ch) && !IS_AWAKE(victim))
        {
                act("$E can't hear you.", ch, 0, victim, TO_CHAR);
                return;
        }

        if (!IS_NPC(victim) && victim->desc == NULL)
        {
                act("$E is currently link dead.", ch, 0, victim, TO_CHAR);
                sprintf(buf, "#R%s tells you '#W%s#R'#n\n\r",
                        PERS(ch, victim), argument);
                buf[0] = UPPER(buf[0]);
                add_buf(victim->pcdata->buffer, buf);
                victim->tells++;
//		log_comm(ch->name, victim->name, CHANNEL_TELL, argument, 0);

                return;
        }

        if (IS_SET(victim->deaf, CHANNEL_TELL) && !IS_IMMORTAL(ch))
        {
                if (IS_NPC(victim) || IS_NPC(ch)
                    || strlen(victim->pcdata->marriage) < 2
                    || str_cmp(ch->name, victim->pcdata->marriage))
                {
                        act("$E can't hear you.", ch, 0, victim, TO_CHAR);
                        return;
                }
        }

        if (IS_SET(victim->extra, EXTRA_AFK))
        {
                if (IS_NPC(victim))
                {
                        act("$E is AFK, and not receiving tells.", ch, NULL,
                            victim, TO_CHAR);
                        return;
                }

                act("$E is AFK, but your tell will go through when they return.", ch, NULL, victim, TO_CHAR);
                sprintf(buf, "#R%s tells you '#W%s#R'#n\n\r",
                        PERS(ch, victim), argument);
                buf[0] = UPPER(buf[0]);
                add_buf(victim->pcdata->buffer, buf);
//		log_comm(ch->name, victim->name, CHANNEL_TELL, argument, 0);
                victim->tells++;
                return;
        }

        if (IS_SET(victim->act2, PLR_AUTOSTORE) && (victim->fighting != NULL))
        {
                if (IS_NPC(victim))
                {
                        act("$E is fighting, and not receiving tells.", ch,
                            NULL, victim, TO_CHAR);
                        return;
                }

                act("$E is fighting, but your tell will go through when $E finishes.", ch, NULL, victim, TO_CHAR);
                sprintf(buf, "#R%s tells you '#W%s#R'#n\n\r",
                        PERS(ch, victim), argument);
                buf[0] = UPPER(buf[0]);
                add_buf(victim->pcdata->buffer, buf);
//		log_comm(ch->name, victim->name, CHANNEL_TELL, argument, 0);

                victim->tells++;
                return;
        }

        if (!IS_NPC(ch) && !IS_NPC(victim))
        {
                for (i = 0; i < MAX_IGNORE; i++)
                {
                        if (victim->pcdata->ignore[i] == ch->pcdata->playerid)
                        {
                                send_to_char("They are ignoring you.\n\r",
                                             ch);
                                return;
                        }
                }
        }

        sprintf(poly, "#RYou tell $N '#W$t#R'#n");
        act(poly, ch, argument, victim, TO_CHAR);

        if (!IS_NPC(ch) && IS_SET(ch->act, PLR_SILENCE))
                return;

        position = victim->position;
        victim->position = POS_STANDING;

        sprintf(poly, "#R$n tells you '#W$t#R'#n");
        act(poly, ch, argument, victim, TO_VICT);

        if (!IS_NPC(ch) && !IS_NPC(victim))
        {
                update_history(victim, ch, poly, argument, TRUE);
        }

        victim->position = position;
        victim->reply = ch;

        if (!IS_NPC(ch) && IS_NPC(victim)
            && HAS_TRIGGER_MOB(victim, TRIG_SPEECH))
                p_act_trigger(argument, victim, NULL, NULL, ch, NULL, NULL,
                              TRIG_SPEECH);

        if (!IS_NPC(ch) && IS_NPC(victim)
            && HAS_TRIGGER_MOB(victim, TRIG_REGEX))
                mp_regex_trigger(argument, victim, ch, NULL, NULL);

        if (IS_SET(victim->act, ACT_INTELLIGENT))
                chatperform(victim, ch, argument);
//	log_comm(ch->name, victim->name, CHANNEL_TELL, argument, 0);
        return;
}



void do_whisper(CHAR_DATA * ch, char *argument)
{
        char      arg[MAX_INPUT_LENGTH];
        CHAR_DATA *victim;

        if (IS_EXTRA(ch, GAGGED))
        {
                send_to_char("Not with a gag on!\n\r", ch);
                return;
        }

        argument = one_argument(argument, arg);

        if (arg[0] == '\0' || argument[0] == '\0')
        {
                send_to_char("Syntax: whisper <person> <message>\n\r", ch);
                return;
        }

        if ((victim = get_char_world(ch, arg)) == NULL
            || (victim->in_room != ch->in_room))
        {
                send_to_char("They aren't here.\n\r", ch);
                return;
        }

        if (!IS_AWAKE(victim))
        {
                act("$E cannot hear you.", ch, 0, victim, TO_CHAR);
                return;
        }

        if (!IS_NPC(victim) && victim->desc == NULL)
        {
                act("$E is currently link dead.", ch, 0, victim, TO_CHAR);
                return;
        }

        act("You whisper to $N '$t'.", ch, argument, victim, TO_CHAR);
        act("$n whispers to you '$t'.", ch, argument, victim, TO_VICT);
        act("$n whispers something to $N.", ch, NULL, victim, TO_NOTVICT);

        return;
}



void do_reply(CHAR_DATA * ch, char *argument)
{
        char      poly[MAX_STRING_LENGTH];
        char      buf[MSL];
        CHAR_DATA *victim;
        int       position;
        int       i;

        if (IS_EXTRA(ch, GAGGED))
        {
                send_to_char("Your message didn't get through.\n\r", ch);
                return;
        }

        if ((victim = ch->reply) == NULL)
        {
                send_to_char("They aren't here.\n\r", ch);
                return;
        }

        if (!IS_IMMORTAL(ch) && !IS_AWAKE(victim))
        {
                act("$E can't hear you.", ch, 0, victim, TO_CHAR);
                return;
        }

        if (!IS_NPC(victim) && victim->desc == NULL)
        {
                act("$E is currently link dead.", ch, 0, victim, TO_CHAR);
                sprintf(buf, "#C%s replies to you '#W%s#C'#n\n\r",
                        PERS(ch, victim), argument);
                buf[0] = UPPER(buf[0]);
                add_buf(victim->pcdata->buffer, buf);
                victim->tells++;
                return;
        }

        if (!IS_NPC(ch) && !IS_NPC(victim))
        {
                for (i = 0; i < MAX_IGNORE; i++)
                {
                        if (victim->pcdata->ignore[i] == ch->pcdata->playerid)
                        {
                                send_to_char("They are ignoring you.\n\r",
                                             ch);
                                return;
                        }
                }
        }

        if (IS_SET(victim->extra, EXTRA_AFK))
        {
                if (IS_NPC(victim))
                {
                        act("$E is AFK, and not receiving tells.", ch, NULL,
                            victim, TO_CHAR);
                        return;
                }

                act("$E is AFK, but your tell will go through when $E returns.", ch, NULL, victim, TO_CHAR);
                sprintf(buf, "#C%s replies to you '#W%s#C'#n\n\r",
                          PERS(ch, victim), argument);
                buf[0] = UPPER(buf[0]);
                add_buf(victim->pcdata->buffer, buf);
                victim->tells++;
                return;
        }

        sprintf(poly, "#CYou reply to $N '#W$t#C'#n.");
        act(poly, ch, argument, victim, TO_CHAR);

        if (!IS_NPC(ch) && IS_SET(ch->act, PLR_SILENCE))
                return;

        position = victim->position;
        victim->position = POS_STANDING;

        sprintf(poly, "#C$n replies to you '#W$t#C'#n.");
        act(poly, ch, argument, victim, TO_VICT);

        if (!IS_NPC(ch) && !IS_NPC(victim))
        {
                update_history(victim, ch, poly, argument, TRUE);
        }

        victim->position = position;
        victim->reply = ch;
//	log_comm(ch->name, victim->name, CHANNEL_TELL, argument, 0);
        return;
}



void do_emote(CHAR_DATA * ch, char *argument)
{
        char      buf[MAX_STRING_LENGTH];
        char     *plast;

        char      name[80];
        char      poly[MAX_INPUT_LENGTH];
        CHAR_DATA *to;
        bool      is_ok;


        if (IS_HEAD(ch, LOST_TONGUE) || IS_HEAD(ch, LOST_HEAD)
            || IS_EXTRA(ch, GAGGED))
        {
                send_to_char("You can't show your emotions.\n\r", ch);
                return;
        }

        if (argument[0] == '\0')

        {
                send_to_char("Pose what?\n\r", ch);
                return;
        }

        for (plast = argument; *plast != '\0'; plast++)
                ;

        strcpy(buf, argument);
        if (isalpha(plast[-1]))
                strcat(buf, ".");
        MOBtrigger = FALSE;
        act("$n $T", ch, NULL, buf, TO_CHAR);

        if (ch->in_room->vnum != ROOM_VNUM_IN_OBJECT)
        {
                act("$n $T", ch, NULL, buf, TO_ROOM);
                return;
        }

        to = ch->in_room->people;
        for (; to != NULL; to = to->next_in_room)
        {
                is_ok = FALSE;

                if (to->desc == NULL || !IS_AWAKE(to))
                        continue;

                if (ch == to)
                        continue;

                if (!IS_NPC(ch) && ch->pcdata->chobj != NULL &&
                    ch->pcdata->chobj->in_room != NULL &&
                    !IS_NPC(to) && to->pcdata->chobj != NULL &&
                    to->pcdata->chobj->in_room != NULL &&
                    ch->in_room == to->in_room)
                        is_ok = TRUE;
                else
                        is_ok = FALSE;

                if (!IS_NPC(ch) && ch->pcdata->chobj != NULL &&
                    ch->pcdata->chobj->in_obj != NULL &&
                    !IS_NPC(to) && to->pcdata->chobj != NULL &&
                    to->pcdata->chobj->in_obj != NULL &&
                    ch->pcdata->chobj->in_obj == to->pcdata->chobj->in_obj)
                        is_ok = TRUE;
                else
                        is_ok = FALSE;

                if (!is_ok)
                        continue;

                if (IS_NPC(ch))
                        sprintf(name, "%s", ch->short_descr);
                else if (!IS_NPC(ch) && IS_AFFECTED(ch, AFF_POLYMORPH))
                        sprintf(name, "%s", ch->morph);
                else if (!IS_NPC(ch) && IS_AFFECTED(ch, AFF_SHIFT))
                        sprintf(name, "%s", ch->morph);

                else
                        sprintf(name, "%s", ch->name);
                name[0] = UPPER(name[0]);
                sprintf(poly, "%s %s\n\r", name, buf);
                send_to_char(poly, to);
        }
        MOBtrigger = TRUE;
        return;
}



void do_xemote(CHAR_DATA * ch, char *argument)
{
        char      buf[MAX_STRING_LENGTH];
        char      buf2[MAX_STRING_LENGTH];
        char      oldarg[MAX_STRING_LENGTH];
        char     *plast;

        char      name[80];
        char      you[80];
        char      them[80];
        char      poly[MAX_INPUT_LENGTH];
        char      arg[MAX_INPUT_LENGTH];
        CHAR_DATA *to;
        CHAR_DATA *victim;
        bool      is_ok;

        argument = one_argument(argument, arg);

        if (IS_HEAD(ch, LOST_TONGUE) || IS_HEAD(ch, LOST_HEAD)
            || IS_EXTRA(ch, GAGGED))
        {
                send_to_char("You can't show your emotions.\n\r", ch);
                return;
        }

        if (strlen(argument) > MAX_INPUT_LENGTH)
        {
                send_to_char("Line too long.\n\r", ch);
                return;
        }

        if (argument[0] == '\0' || arg[0] == '\0')
        {
                send_to_char("Syntax: emote <person> <sentence>\n\r", ch);
                return;
        }

        if ((victim = get_char_room(ch, NULL, arg)) == NULL)
        {
                send_to_char("They aren't here.\n\r", ch);
                return;
        }
        if (IS_NPC(ch))
        {
                if (ch->short_descr != NULL)
                        strcpy(you, ch->short_descr);
                else
                        return;
        }
        else
        {
                if (ch->name != NULL)
                        strcpy(you, ch->name);
                else
                        return;
        }
        if (IS_NPC(victim))
        {
                if (victim->short_descr != NULL)
                        strcpy(you, victim->short_descr);
                else
                        return;
        }
        else
        {
                if (victim->name != NULL)
                        strcpy(you, victim->name);
                else
                        return;
        }

/*
oldarg = argument;
*/
        strcpy(oldarg, argument);
        strcpy(buf, argument);
        for (plast = argument; *plast != '\0'; plast++)
                ;

        if (isalpha(plast[-1]))
                strcat(buf, ".");
        argument = socialc(ch, buf, you, them);

        strcpy(buf, argument);
        strcpy(buf2, "You ");
        buf[0] = LOWER(buf[0]);
        strcat(buf2, buf);
        capitalize(buf2);
        act(buf2, ch, NULL, victim, TO_CHAR);

        if (ch->in_room->vnum != ROOM_VNUM_IN_OBJECT)
        {
                strcpy(buf, oldarg);
                for (plast = argument; *plast != '\0'; plast++)
                        ;
                if (isalpha(plast[-1]))
                        strcat(buf, ".");

                argument = socialn(ch, buf, you, them);

                strcpy(buf, argument);
                strcpy(buf2, "$n ");
                buf[0] = LOWER(buf[0]);
                strcat(buf2, buf);
                capitalize(buf2);
                act(buf2, ch, NULL, victim, TO_NOTVICT);

                strcpy(buf, oldarg);
                for (plast = argument; *plast != '\0'; plast++)
                        ;
                if (isalpha(plast[-1]))
                        strcat(buf, ".");

                argument = socialv(ch, buf, you, them);

                strcpy(buf, argument);
                strcpy(buf2, "$n ");
                buf[0] = LOWER(buf[0]);
                strcat(buf2, buf);
                capitalize(buf2);
                act(buf2, ch, NULL, victim, TO_VICT);
                return;
        }

        to = ch->in_room->people;
        for (; to != NULL; to = to->next_in_room)
        {
                is_ok = FALSE;

                if (to->desc == NULL || !IS_AWAKE(to))
                        continue;

                if (ch == to)
                        continue;

                if (!IS_NPC(ch) && ch->pcdata->chobj != NULL &&
                    ch->pcdata->chobj->in_room != NULL &&
                    !IS_NPC(to) && to->pcdata->chobj != NULL &&
                    to->pcdata->chobj->in_room != NULL &&
                    ch->in_room == to->in_room)
                        is_ok = TRUE;
                else
                        is_ok = FALSE;

                if (!IS_NPC(ch) && ch->pcdata->chobj != NULL &&
                    ch->pcdata->chobj->in_obj != NULL &&
                    !IS_NPC(to) && to->pcdata->chobj != NULL &&
                    to->pcdata->chobj->in_obj != NULL &&
                    ch->pcdata->chobj->in_obj == to->pcdata->chobj->in_obj)
                        is_ok = TRUE;
                else
                        is_ok = FALSE;

                if (!is_ok)
                        continue;

                if (IS_NPC(ch))
                        sprintf(name, "%s", ch->short_descr);
                else if (!IS_NPC(ch) && IS_AFFECTED(ch, AFF_POLYMORPH))
                        sprintf(name, "%s", ch->morph);
                else if (!IS_NPC(ch) && IS_AFFECTED(ch, AFF_SHIFT))
                        sprintf(name, "%s", ch->morph);

                else
                        sprintf(name, "%s", ch->name);
                name[0] = UPPER(name[0]);
                sprintf(poly, "%s %s\n\r", name, buf);
                send_to_char(poly, to);
        }
        return;
}



void do_bug(CHAR_DATA * ch, char *argument)
{
        append_file(ch, BUG_FILE, argument);
        send_to_char("Ok.  Thanks.\n\r", ch);
        return;
}



void do_idea(CHAR_DATA * ch, char *argument)
{
        send_to_char("Please use board 2 for ideas.\n\r", ch);
        return;
}



void do_typo(CHAR_DATA * ch, char *argument)
{
        send_to_char
                ("Please post a personal note about any typo's to Jobo.\n\r",
                 ch);
        return;
}



void do_rent(CHAR_DATA * ch, char *argument)
{
        send_to_char("There is no rent here.  Just save and quit.\n\r", ch);
        return;
}



void do_qui(CHAR_DATA * ch, char *argument)
{
        send_to_char("If you want to QUIT, you have to spell it out.\n\r",
                     ch);
        return;
}

void do_quit(CHAR_DATA * ch, char *argument)
{
        DESCRIPTOR_DATA *d;
        char      buf[MAX_STRING_LENGTH];
        OBJ_DATA *obj;
        OBJ_DATA *obj_next;
        CHAR_DATA *mount;
        CHAR_DATA *gch;
        int       i;

        if (IS_NPC(ch))
                return;

        if (ch->position == POS_FIGHTING)
        {
                send_to_char("No way! You are fighting.\n\r", ch);
                return;
        }

        if (ch->pcdata->logout_counter > 0)
        {
                stc("Can't quit within 45 secs of logging in.\n\r", ch);
                return;
        }

        if (ch->position != POS_STANDING)
        {
                ch->position = POS_STANDING;
        }
        if (auction_info.high_bidder == ch || auction_info.owner == ch)
        {
                send_to_char("You still have a stake in the auction!\n\r",
                             ch);
                return;
        }
        if (ch->fight_timer > 0)
        {
                send_to_char("Not until your fight timer expires.\n\r", ch);
                return;
        }

        if (ch->in_room != NULL)
        {
                if (IS_SET(ch->in_room->room_flags, ROOM_ARENA))
                {
                        send_to_char("Your in the arena.\n\r", ch);
                        return;
                }
        }

        if (in_fortress(ch))
        {
                send_to_char("Your in The Forbidden Fortress.\n\r", ch);
                return;
        }

        call_all(ch);   // wippy-doodle-doo

        for (gch = char_list; gch; gch = gch->next)
        {
                if (IS_NPC(gch))
                        continue;
                if (gch->desc)
                {
                        if (gch->desc->connected != CON_PLAYING
                            && !(gch->desc->connected >= CON_NOTE_TO
                                 && gch->desc->connected <= CON_NOTE_FINISH))
                                continue;
                }
                if (gch->challenger == ch)
                {
                        gch->challenger = NULL;
                        send_to_char("Your victim leaves the game.\n\r", gch);
                }
        }

//	clear_sigils(ch);

        /*
         * ripping out arties 
         */
        for (i = 0; i < MAX_WEAR; i++)
        {
                if ((obj = get_eq_char(ch, i)) != NULL)
                {
                        if (IS_SET(obj->quest, QUEST_ARTIFACT))
                                unequip_char(ch, obj);
                }
        }
        for (obj = ch->carrying; obj != NULL; obj = obj_next)
        {
                obj_next = obj->next_content;

                if (IS_SET(obj->quest, QUEST_ARTIFACT))
                {
                        obj_from_char(obj);
                        sprintf(buf, "%s has returned to the game",
                                obj->short_descr);
                        do_info(ch, buf);
                        sprintf(buf, "%d", obj->pIndexData->vnum);
                        extract_obj(obj);
                        do_randomload(ch, buf);
                }
        }

        if (ch->level > 6); /* do nothing */
        else if (IS_SET(ch->pcdata->jflags, JFLAG_SETLOGOUT))
                logout_message(ch);
        else if (ch->pcdata->obj_vnum == 0)
        {
                switch (number_range(0, 11))
                {
                default:
                        sprintf(buf, "#R%s #whas fled from #R%s#w.#n",
                                ch->pcdata->switchname, MUDNAME);
                        break;
                case 0:
                        sprintf(buf,
                                "#w%s gives everyone the finger while leaving.",
                                ch->pcdata->switchname);
                        break;
                case 1:
                        sprintf(buf,
                                "#w%s is surrounded by a #Ggreen #wforceshield, then #pdisappears!#n.",
                                ch->pcdata->switchname);
                        break;
                case 2:
                        sprintf(buf, "#w%s spits in your mouth, then runs!",
                                ch->pcdata->switchname);
                        break;
                case 3:
                        sprintf(buf,
                                "#w%s says, 'I'll be back to claim the rest of my status.'.",
                                ch->pcdata->switchname);
                        break;
                case 4:
                        sprintf(buf, "#w%s gets kicked out the door.",
                                ch->pcdata->switchname);
                        break;
                case 5:
                        sprintf(buf, "#w%s slams the door on the way out.",
                                ch->pcdata->switchname);
                        break;
                case 6:
                        sprintf(buf,
                                "#wWhile leaving, %s pulls down their pants and wiggles their barenaked ass at you.",
                                ch->pcdata->switchname);
                        break;
                case 7:
                        sprintf(buf,
                                "#w%s runs to a toilet and jumps in and is gone.",
                                ch->pcdata->switchname);
                        break;
                case 8:
                        sprintf(buf,
                                "#w%s screams #RSHAZZAM #w and runs away!",
                                ch->pcdata->switchname);
                        break;
                case 9:
                        sprintf(buf,
                                "#0%s gets hit on the #BAss #0by the door on the way out.",
                                ch->pcdata->switchname);
                        break;
                case 10:
                        sprintf(buf,
                                "#w%s gives a twist to their #RRosy #wnose and flies up the chimney and to their sleigh.",
                                ch->pcdata->switchname);
                        break;
                case 11:
                        sprintf(buf,
                                "#w%s jumps off a bridge and breaks every bone in their body and sinks below the water.",
                                ch->pcdata->switchname);
                        break;
                }
                leave_info(buf);
        }

        if (IS_SET(ch->extra, EXTRA_OSWITCH))
                do_humanform(ch, "");
        if (ch->gladiator != NULL)
                ch->gladiator = NULL;   /* set player to bet on to NULL */
        if (ch->challenger != NULL)
                ch->challenger = NULL;
        if (ch->challenged != NULL)
                ch->challenged = NULL;
        if ((mount = ch->mount) != NULL)
                do_dismount(ch, "");

        /*
         * it's in close_socket as well, maybe that's enough... 
         */
        if (ch->desc && ch->desc->out_compress)
                compressEnd(ch->desc);  // free up the mccp shit

        switch (number_range(1, 10))
        {
        case 1:
                send_to_char
                        ("      Honesty is the best policy, but insanity is a better defence \n\r",
                         ch);
                break;
        case 2:
                send_to_char
                        ("      Some people wish to get what they deserve, while others fear the same \n\r",
                         ch);
                break;
        case 3:
                send_to_char
                        ("      A wise man gets more use from his enemies than a fool from his friends \n\r",
                         ch);
                break;
        case 4:
                send_to_char
                        ("      The best days to drink beer are days that end in the letter, 'Y' \n\r",
                         ch);
                break;
        case 5:
                send_to_char
                        ("      Pain is only weakness leaving the body \n\r",
                         ch);
                break;
        case 6:
                send_to_char
                        ("      Trans corpus meum mortuum. - Over my dead body \n\r",
                         ch);
                break;
        case 7:
                send_to_char("                      \\=/, \n\r", ch);
                send_to_char("                      |  @___oo  \n\r", ch);
                send_to_char("            /\\  /\\   / (___,,,}  \n\r", ch);
                send_to_char("          ) /^\\) ^\\/ _)  \n\r", ch);
                send_to_char("          )   /^\\/   _)  \n\r", ch);
                send_to_char("          )   _ /  / _)  \n\r", ch);
                send_to_char
                        ("       /\\  )/\\/ ||  | )_)             See you later, alligator\n\r",
                         ch);
                send_to_char("      <  >      |(,,) )__)  \n\r", ch);
                send_to_char("       ||      /    \\)___)\\  \n\r", ch);
                send_to_char("       | \\____(      )___) )___  \n\r", ch);
                send_to_char("        \\______(_______;;; __;;; \n\r", ch);
                break;
        case 8:
                send_to_char
                        ("      To HELL with the Prime Directive.... FIRE!!! - Kirk \n\r",
                         ch);
                break;
        case 9:
                send_to_char
                        ("      You, in the red uniform, go see what that noise is! \n\r",
                         ch);
                break;
        case 10:
                send_to_char
                        ("      C.O.B.O.L - Completely Obsolete Boring Old Language \n\r",
                         ch);
                break;
        }

        if (ch->pcdata->in_progress)
                free_note(ch->pcdata->in_progress);

        d = ch->desc;
        save_char_obj(ch);
        if (ch->pcdata->obj_vnum != 0)
                act("$n slowly fades out of existance.", ch, NULL, NULL,
                    TO_ROOM);
        else
                act("$n has left the game.", ch, NULL, NULL, TO_ROOM);

        if (d != NULL)
                close_socket2(d, FALSE);

        if (ch->in_room != NULL)
                char_from_room(ch);
        char_to_room(ch, get_room_index(ROOM_VNUM_DISCONNECTION));

        //sprintf( log_buf,  );
        log_string(LOG_CONNECT, "%s has quit.", ch->name);
        if (ch->pcdata->chobj != NULL)
                extract_obj(ch->pcdata->chobj);

        /*
         * After extract_char the ch is no longer valid!
         */
        extract_char(ch, TRUE);
        return;
}

/*
void clear_sigils(CHAR_DATA *ch)
{
  OBJ_DATA *obj;
  OBJ_DATA *obj_next;
  int sigs[8], i;

  for( i = 0; i < 8; i++ )
    sigs[i] = 0;

  for( obj = ch->carrying; obj != NULL; obj = obj_next )
  {
    obj_next = obj->next_content;
    if( obj->pIndexData->vnum >= 11059 && obj->pIndexData->vnum <= 11066 && obj->item_type == ITEM_SIGIL )
    {
      if( ++sigs[obj->pIndexData->vnum - 11059] > 4 )
      {
        obj_from_char(obj);
        extract_obj(obj);
      }
    }
    else if( obj->item_type == ITEM_CONTAINER )
    {
      clear_sigils_container(obj, sigs);
    }
  }
}

void clear_sigils_container( OBJ_DATA *container, int sigs[8] )
{
  OBJ_DATA *obj;
  OBJ_DATA *obj_next;

  if( container->item_type != ITEM_CONTAINER ) return;
  for( obj = container->contains; obj != NULL; obj = obj_next )
  {
    obj_next = obj->next_content;
    if( obj->pIndexData->vnum >= 11059 && obj->pIndexData->vnum <= 11066 && obj->item_type == ITEM_SIGIL )
    {
      if( ++sigs[obj->pIndexData->vnum - 11059] > 4 )
      {
        obj_from_char(obj);
        extract_obj(obj);
      }
    }
    else if( obj->item_type == ITEM_CONTAINER )
      clear_sigils_container(obj, sigs);
  }
}
*/
void do_save(CHAR_DATA * ch, char *argument)
{
        if (IS_NPC(ch))
                return;

        if (ch->level < 2)
        {
                send_to_char
                        ("You must kill at least 5 mobs before you can save.\n\r",
                         ch);
                return;
        }
        save_char_obj(ch);
        send_to_char("Saved.\n\r", ch);
        return;
}



void do_autosave(CHAR_DATA * ch, char *argument)
{
        if (IS_NPC(ch))
                return;
        if (ch->level < 2)
                return;
        save_char_obj(ch);
        return;
}



void do_follow(CHAR_DATA * ch, char *argument)
{
        char      arg[MAX_INPUT_LENGTH];
        CHAR_DATA *victim;

        one_argument(argument, arg);


        if (arg[0] == '\0')
        {
                send_to_char("Follow whom?\n\r", ch);
                return;
        }
        if ((victim = get_char_room(ch, NULL, arg)) == NULL)
        {
                send_to_char("They aren't here.\n\r", ch);
                return;
        }
        if (IS_AFFECTED(ch, AFF_CHARM) && ch->master != NULL)
        {
                act("But you'd rather follow $N!", ch, NULL, ch->master,
                    TO_CHAR);
                return;
        }
        if (victim == ch)
        {
                if (ch->master == NULL)
                {
                        send_to_char("You already follow yourself.\n\r", ch);
                        return;
                }
                stop_follower(ch, FALSE);
                return;
        }

        if (ch->master != NULL)
                stop_follower(ch, FALSE);

        add_follower(ch, victim);
        return;
}



void add_follower(CHAR_DATA * ch, CHAR_DATA * master)
{
        if (ch->master != NULL)
        {
                bug("Add_follower: non-null master.", 0);
                return;
        }

        ch->master = master;
        ch->leader = NULL;

        if (can_see(master, ch))
                act("$n now follows you.", ch, NULL, master, TO_VICT);

        act("You now follow $N.", ch, NULL, master, TO_CHAR);

        return;
}

void stop_follower(CHAR_DATA * ch, bool isDead)
{
        if (ch->master == NULL)
        {
                bug("Stop_follower: null master.", 0);
                return;
        }

        if (IS_AFFECTED(ch, AFF_CHARM))
        {
                REMOVE_BIT(ch->affected_by, AFF_CHARM);
                affect_strip(ch, gsn_charm_person);
        }

        if (can_see(ch->master, ch))
                act("$n stops following you.", ch, NULL, ch->master, TO_VICT);
        act("You stop following $N.", ch, NULL, ch->master, TO_CHAR);

        ch->master = NULL;
        ch->leader = NULL;

        /*
         * kill of those golems   ---- bad, if the golem dies, then this is called, so it will die twice.. silly 
         */
        if (IS_NPC(ch) && !isDead)
        {
                if (ch->pIndexData->vnum >= MOB_VNUM_FIRE
                    && ch->pIndexData->vnum <= MOB_VNUM_CLAY)
                {
                        act("$n crumbles to dust.", ch, NULL, NULL, TO_ROOM);
                        extract_char(ch, TRUE);
                }
        }
        return;
}



void die_follower(CHAR_DATA * ch)
{
        CHAR_DATA *fch;
        CHAR_DATA *fch_next;

        if (ch->master != NULL)
                stop_follower(ch, TRUE);

        ch->leader = NULL;

        for (fch = char_list; fch != NULL; fch = fch_next)
        {
                fch_next = fch->next;

                if (fch->leader == ch)
                        fch->leader = fch;
                if (fch->master == ch)
                        stop_follower(fch, FALSE);
        }

        return;
}



void do_order(CHAR_DATA * ch, char *argument)
{

        char      arg[MAX_INPUT_LENGTH];
        CHAR_DATA *victim;
        CHAR_DATA *och;
        CHAR_DATA *och_next;
        bool      found;
        bool      fAll;

        argument = one_argument(argument, arg);

        if (arg[0] == '\0' || argument[0] == '\0')
        {
                send_to_char("Order whom to do what?\n\r", ch);
                return;
        }

        if (IS_AFFECTED(ch, AFF_CHARM))
        {
                send_to_char("You feel like taking, not giving, orders.\n\r",
                             ch);
                return;
        }

        if (IS_SET(ch->in_room->room_flags, ROOM_ORDER)
            || IS_SET(ch->in_room->room_flags, ROOM_SAFE))
        {
                stc("You can't order things around here.\n\r", ch);
                return;
        }

        if (!str_cmp(arg, "all"))
        {
                send_to_char("Ordering 'all' has been disabled.\n\r", ch);
                return;
        }
        else
        {
                fAll = FALSE;
                if ((victim = get_char_room(ch, NULL, arg)) == NULL)
                {
                        send_to_char("They aren't here.\n\r", ch);
                        return;
                }

                if (victim == ch)
                {
                        send_to_char("Aye aye, right away!\n\r", ch);
                        return;
                }

                if ((!IS_AFFECTED(victim, AFF_CHARM) || victim->master != ch))

                {
                        send_to_char("Do it yourself!\n\r", ch);
                        return;
                }

                if (IS_CLASS(ch, CLASS_VAMPIRE)
                    && IS_CLASS(victim, CLASS_VAMPIRE)
                    && ((ch->pcdata->stats[UNI_GEN] > 2)
                        || ch->pcdata->kingdom != victim->pcdata->kingdom))
                {
                        act("$N ignores your order.", ch, NULL, victim,
                            TO_CHAR);
                        act("You ignore $n's order.", ch, NULL, victim,
                            TO_VICT);
                        return;
                }

        }

        found = FALSE;
        for (och = ch->in_room->people; och != NULL; och = och_next)
        {
                och_next = och->next_in_room;
                if (och == ch)
                        continue;

                if ((IS_AFFECTED(och, AFF_CHARM)
                     && och->master == ch
                     && (fAll || och == victim))
                    || (ch->pcdata->stats[UNI_GEN] == 2
                        && (fAll || och == victim)
                        && ch->pcdata->kingdom == och->pcdata->kingdom))
                {
                        found = TRUE;
                        act("$n orders you to '$t'.", ch, argument, och,
                            TO_VICT);
                        interpret(och, argument);
                }
                else if (!IS_NPC(ch) && !IS_NPC(och)
                         && (fAll || och == victim)
                         && IS_CLASS(ch, CLASS_VAMPIRE)
                         && IS_CLASS(och, CLASS_VAMPIRE)
                         && ch->pcdata->stats[UNI_GEN] <
                         och->pcdata->stats[UNI_GEN]
                         && ch->pcdata->kingdom == och->pcdata->kingdom)
                {
                        found = TRUE;
                        act("$n orders you to '$t'.", ch, argument, och,
                            TO_VICT);
                        interpret(och, argument);
                }
        }

        if (found)
        {
                send_to_char("Ok.\n\r", ch);
                if (!IS_NPC(victim))
                        ch->wait = victim->wait;
        }
        else
                send_to_char("You have no followers here.\n\r", ch);
        WAIT_STATE(ch, 12);
        return;
}



void do_command(CHAR_DATA * ch, char *argument)
{
        char      arg1[MAX_INPUT_LENGTH];
        char      arg2[MAX_INPUT_LENGTH];
        char      buf[MAX_STRING_LENGTH];
        char      buffy[MAX_STRING_LENGTH];
        CHAR_DATA *victim;
        int       awe;

        argument = one_argument(argument, arg1);
        argument = one_argument(argument, arg2);

        if (IS_NPC(ch))
                return;
        if (!IS_CLASS(ch, CLASS_VAMPIRE)
            && !IS_CLASS(ch, CLASS_UNDEAD_KNIGHT))
        {
                send_to_char("Huh?\n\r", ch);
                return;
        }
        if (ch->power[DISC_VAMP_DOMI] < 1 && IS_CLASS(ch, CLASS_VAMPIRE))
        {
                send_to_char
                        ("You must obtain at least level 1 in Dominate to use Command.\n\r",
                         ch);
                return;
        }
        if (IS_CLASS(ch, CLASS_UNDEAD_KNIGHT)
            && ch->pcdata->powers[NECROMANCY] < 4)
        {
                send_to_char
                        ("You need level 4 necromancry to use command.\n\r",
                         ch);
                return;
        }



        if (ch->spl[RED_MAGIC] < 1)
        {
                send_to_char("Your mind is too weak.\n\r", ch);
                return;
        }
        if (arg1[0] == '\0' || arg2[0] == '\0')
        {
                send_to_char("Command whom to do what?\n\r", ch);
                return;
        }
        if ((victim = get_char_room(ch, NULL, arg1)) == NULL)
        {
                send_to_char("They aren't here.\n\r", ch);
                return;
        }
        if (victim->shop_fun || victim->quest_fun)
        {
                send_to_char("That mobile is protected by the gods.\n\r", ch);
                return;
        }
        if (IS_AFFECTED(victim, AFF_CHARM))
        {
                send_to_char("Their mind is controlled by someone else.\n\r",
                             ch);
                return;
        }
        if (!IS_NPC(victim) && IS_CLASS(victim, CLASS_DRONE))
        {
                if (IS_SET(victim->newbits2, NEW2_MENTALBLOCK))
                {
                        send_to_char
                                ("A strange power blocks your attempt to control them.\n\r",
                                 ch);
                        sprintf(buf,
                                  "You sense that %s is trying to control your actions.\n\r",
                                  ch->name);
                        send_to_char(buf, victim);
                        return;
                }
        }
        if (IS_SET(ch->in_room->room_flags, ROOM_ORDER)
            || IS_SET(ch->in_room->room_flags, ROOM_SAFE))
        {
                stc("You can't order things around here.\n\r", ch);
                return;
        }
        if (victim == ch)
        {
                send_to_char("How can you command yourself??\n\r", ch);
                return;
        }

        if (ch->power[DISC_VAMP_DOMI] > 2)
                awe = 50;
        else if (ch->power[DISC_VAMP_DOMI] > 3)
                awe = 75;
        else if (ch->power[DISC_VAMP_DOMI] > 4)
                awe = 100;
        else
                awe = 25;

        if (IS_EXTRA(ch, EXTRA_AWE))
        {
                awe += 75;
        }

        if (!IS_NPC(victim) && victim->level != 3)
        {
                send_to_char("You can only command other avatars.\n\r", ch);
                return;
        }


        if (ch->power[DISC_VAMP_DOMI] > 1)
        {
                sprintf(buffy, "%s %s", arg2, argument);
                if (IS_NPC(victim))
                        sprintf(buf, "I think %s wants to %s",
                                victim->short_descr, buffy);
                else if (!IS_NPC(victim)
                         && IS_AFFECTED(victim, AFF_POLYMORPH))
                        sprintf(buf, "I think %s wants to %s", victim->morph,
                                buffy);
                else
                        sprintf(buf, "I think %s wants to %s", victim->name,
                                buffy);
                do_say(ch, buf);
        }
        else
        {
                if (IS_NPC(victim))
                        sprintf(buf, "I think %s wants to %s",
                                victim->short_descr, arg2);
                else if (!IS_NPC(victim)
                         && IS_AFFECTED(victim, AFF_POLYMORPH))
                        sprintf(buf, "I think %s wants to %s", victim->morph,
                                arg2);
                else
                        sprintf(buf, "I think %s wants to %s", victim->name,
                                arg2);
                do_say(ch, buf);
        }

        if (IS_NPC(victim)
            && (victim->level >= awe * ch->spl[RED_MAGIC] * 2
                || victim->level > 500))
        {
                act("You shake off $N's suggestion.", victim, NULL, ch,
                    TO_CHAR);
                act("$n shakes off $N's suggestion.", victim, NULL, ch,
                    TO_NOTVICT);
                act("$n shakes off your suggestion.", victim, NULL, ch,
                    TO_VICT);
                act("$s mind is too strong to overcome.", victim, NULL, ch,
                    TO_VICT);
                return;
        }

        else if (victim->spl[BLUE_MAGIC] >= (ch->spl[RED_MAGIC] / 2))
        {
                act("You shake off $N's suggestion.", victim, NULL, ch,
                    TO_CHAR);
                act("$n shakes off $N's suggestion.", victim, NULL, ch,
                    TO_NOTVICT);
                act("$n shakes off your suggestion.", victim, NULL, ch,
                    TO_VICT);
                act("$s mind is too strong to overcome.", victim, NULL, ch,
                    TO_VICT);
                return;
        }

        act("You blink in confusion.", victim, NULL, NULL, TO_CHAR);
        act("$n blinks in confusion.", victim, NULL, NULL, TO_ROOM);
        strcpy(buf, "Yes, you're right, I do...");
        do_say(victim, buf);
        if (ch->power[DISC_VAMP_DOMI] > 1)
                interpret(victim, buffy);
        else
                interpret(victim, arg2);
        WAIT_STATE(ch, 4);
        return;

}



void do_group(CHAR_DATA * ch, char *argument)
{
        char      buf[MAX_STRING_LENGTH];
        char      arg[MAX_INPUT_LENGTH];
        CHAR_DATA *victim;

        one_argument(argument, arg);

        if (arg[0] == '\0')
        {
                CHAR_DATA *gch;
                CHAR_DATA *leader;


                leader = (ch->leader != NULL) ? ch->leader : ch;
                sprintf(buf, "%s's group:\n\r", PERS(leader, ch));
                send_to_char(buf, ch);

                for (gch = char_list; gch != NULL; gch = gch->next)
                {
                        if (is_same_group(gch, ch))
                        {
                                sprintf(buf,
                                        "[%-16s] %4d/%4d hp %4d/%4d mana %4d/%4d mv %7.0f xp\n\r",
                                        capitalize(PERS(gch, ch)),
                                        gch->hit, gch->max_hit,
                                        gch->mana, gch->max_mana,
                                        gch->move, gch->max_move, gch->exp);
                                send_to_char(buf, ch);
                        }
                }
                return;
        }

        if ((victim = get_char_room(ch, NULL, arg)) == NULL)
        {
                send_to_char("They aren't here.\n\r", ch);
                return;
        }

        if (ch->master != NULL || (ch->leader != NULL && ch->leader != ch))
        {
                send_to_char("But you are following someone else!\n\r", ch);
                return;
        }

        if (victim->master != ch && ch != victim)
        {
                act("$N isn't following you.", ch, NULL, victim, TO_CHAR);
                return;
        }

        if (is_same_group(victim, ch) && ch != victim)
        {
                victim->leader = NULL;
                act("$n removes $N from $s group.", ch, NULL, victim,
                    TO_NOTVICT);
                act("$n removes you from $s group.", ch, NULL, victim,
                    TO_VICT);
                act("You remove $N from your group.", ch, NULL, victim,
                    TO_CHAR);
                return;
        }

        victim->leader = ch;
        act("$N joins $n's group.", ch, NULL, victim, TO_NOTVICT);
        act("You join $n's group.", ch, NULL, victim, TO_VICT);
        act("$N joins your group.", ch, NULL, victim, TO_CHAR);
        return;
}


void do_gtell(CHAR_DATA * ch, char *argument)
{
        char      buf[MAX_STRING_LENGTH];
        DESCRIPTOR_DATA *d;
        CHAR_DATA *gch;

        if (argument[0] == '\0')
        {
                send_to_char("Tell your group what?\n\r", ch);
                return;
        }

        if (IS_SET(ch->act, PLR_NO_TELL))
        {
                send_to_char("Your message didn't get through!\n\r", ch);
                return;
        }

        /*
         * Note use of send_to_char, so gtell works on sleepers.
         */
        sprintf(buf, "#G%s tells the group #R'#G%s#R'\n\r#n", ch->name,
                argument);
//    for ( gch = char_list; gch != NULL; gch = gch->next )
        for (d = descriptor_list; d != NULL; d = d->next)
        {
                if (d->character != NULL)
                        gch = d->character;
                else
                        continue;
                if (is_same_group(gch, ch))
                        send_to_char(buf, gch);
        }

        return;
}



/*
 * It is very important that this be an equivalence relation:
 * (1) A ~ A
 * (2) if A ~ B then B ~ A
 * (3) if A ~ B  and B ~ C, then A ~ C
 */
bool is_same_group(CHAR_DATA * ach, CHAR_DATA * bch)
{
        if (ach->leader != NULL)
                ach = ach->leader;
        if (bch->leader != NULL)
                bch = bch->leader;
        return ach == bch;
}

void do_changelight(CHAR_DATA * ch, char *argument)
{
        if (IS_SET(ch->in_room->room_flags, ROOM_DARK))
        {
                REMOVE_BIT(ch->in_room->room_flags, ROOM_DARK);
                act("The room is suddenly filled with light!", ch, NULL, NULL,
                    TO_CHAR);
                act("The room is suddenly filled with light!", ch, NULL, NULL,
                    TO_ROOM);
                return;
        }
        SET_BIT(ch->in_room->room_flags, ROOM_DARK);
        act("The lights in the room suddenly go out!", ch, NULL, NULL,
            TO_CHAR);
        act("The lights in the room suddenly go out!", ch, NULL, NULL,
            TO_ROOM);
        return;
}

void open_lift(CHAR_DATA * ch)
{
        ROOM_INDEX_DATA *location;
        int       in_room;

        in_room = ch->in_room->vnum;
        location = get_room_index(in_room);

        if (is_open(ch))
                return;

        act("The doors open.", ch, NULL, NULL, TO_CHAR);
        act("The doors open.", ch, NULL, NULL, TO_ROOM);
        move_door(ch);
        if (is_open(ch))
                act("The doors close.", ch, NULL, NULL, TO_ROOM);
        if (!same_floor(ch, in_room))
                act("The lift judders suddenly.", ch, NULL, NULL, TO_ROOM);
        if (is_open(ch))
                act("The doors open.", ch, NULL, NULL, TO_ROOM);
        move_door(ch);
        open_door(ch, FALSE);
        char_from_room(ch);
        char_to_room(ch, location);
        open_door(ch, TRUE);
        move_door(ch);
        open_door(ch, TRUE);
        thru_door(ch, in_room);
        char_from_room(ch);
        char_to_room(ch, location);
        return;
}


void close_lift(CHAR_DATA * ch)
{
        ROOM_INDEX_DATA *location;
        int       in_room;

        in_room = ch->in_room->vnum;
        location = get_room_index(in_room);

        if (!is_open(ch))
                return;
        act("The doors close.", ch, NULL, NULL, TO_CHAR);
        act("The doors close.", ch, NULL, NULL, TO_ROOM);
        open_door(ch, FALSE);
        move_door(ch);
        open_door(ch, FALSE);
        char_from_room(ch);
        char_to_room(ch, location);
        return;
}

void move_lift(CHAR_DATA * ch, int to_room)
{
        ROOM_INDEX_DATA *location;
        int       in_room;

        in_room = ch->in_room->vnum;
        location = get_room_index(in_room);

        if (is_open(ch))
                act("The doors close.", ch, NULL, NULL, TO_CHAR);
        if (is_open(ch))
                act("The doors close.", ch, NULL, NULL, TO_ROOM);
        if (!same_floor(ch, to_room))
                act("The lift judders suddenly.", ch, NULL, NULL, TO_CHAR);
        if (!same_floor(ch, to_room))
                act("The lift judders suddenly.", ch, NULL, NULL, TO_ROOM);
        move_door(ch);
        open_door(ch, FALSE);
        char_from_room(ch);
        char_to_room(ch, location);
        open_door(ch, FALSE);
        thru_door(ch, to_room);
        return;
}

bool same_floor(CHAR_DATA * ch, int cmp_room)
{
        OBJ_DATA *obj;
        OBJ_DATA *obj_next;

        for (obj = ch->in_room->contents; obj != NULL; obj = obj_next)
        {
                obj_next = obj->next_content;
                if (obj->item_type != ITEM_PORTAL)
                        continue;
                if (obj->pIndexData->vnum == 30001)
                        continue;
                if (obj->value[0] == cmp_room)
                        return TRUE;
                else
                        return FALSE;
        }
        return FALSE;
}

bool is_open(CHAR_DATA * ch)
{
        OBJ_DATA *obj;
        OBJ_DATA *obj_next;

        for (obj = ch->in_room->contents; obj != NULL; obj = obj_next)
        {
                obj_next = obj->next_content;
                if (obj->item_type != ITEM_PORTAL)
                        continue;
                if (obj->pIndexData->vnum == 30001)
                        continue;
                if (obj->value[2] == 0)
                        return TRUE;
                else
                        return FALSE;
        }
        return FALSE;
}

void move_door(CHAR_DATA * ch)
{
        OBJ_DATA *obj;
        OBJ_DATA *obj_next;
        ROOM_INDEX_DATA *pRoomIndex;

        for (obj = ch->in_room->contents; obj != NULL; obj = obj_next)
        {
                obj_next = obj->next_content;
                if (obj->item_type != ITEM_PORTAL)
                        continue;
                if (obj->pIndexData->vnum == 30001)
                        continue;
                pRoomIndex = get_room_index(obj->value[0]);
                char_from_room(ch);
                char_to_room(ch, pRoomIndex);
                return;
        }
        return;
}

void thru_door(CHAR_DATA * ch, int doorexit)
{
        OBJ_DATA *obj;
        OBJ_DATA *obj_next;

        for (obj = ch->in_room->contents; obj != NULL; obj = obj_next)
        {
                obj_next = obj->next_content;
                if (obj->item_type != ITEM_PORTAL)
                        continue;
                if (obj->pIndexData->vnum == 30001)
                        continue;
                obj->value[0] = doorexit;
                return;
        }
        return;
}

void open_door(CHAR_DATA * ch, bool be_open)
{
        OBJ_DATA *obj;
        OBJ_DATA *obj_next;

        for (obj = ch->in_room->contents; obj != NULL; obj = obj_next)
        {
                obj_next = obj->next_content;
                if (obj->item_type != ITEM_PORTAL)
                        continue;
                if (obj->pIndexData->vnum == 30001)
                        continue;
                if (obj->value[2] == 0 && !be_open)
                        obj->value[2] = 3;
                else if (obj->value[2] == 3 && be_open)
                        obj->value[2] = 0;
                return;
        }
        return;
}

void do_speak(CHAR_DATA * ch, char *argument)
{
        char      arg[MAX_INPUT_LENGTH];
        char      buf[MAX_INPUT_LENGTH];

        argument = one_argument(argument, arg);

        if (IS_NPC(ch))
                return;

        if (arg[0] == '\0')
        {
                if (IS_SPEAKING(ch, LANG_DARK))
                        send_to_char("You are speaking the Dark tongue.\n\r",
                                     ch);
                else if (IS_SPEAKING(ch, DIA_OLDE))
                        send_to_char("You are speaking Olde Worlde.\n\r", ch);
                else if (IS_SPEAKING(ch, DIA_BAD))
                        send_to_char("You are speaking very badly.\n\r", ch);
                else
                        send_to_char
                                ("You are speaking the common language.\n\r",
                                 ch);
                strcpy(buf, "You can speak the following languages:");
                strcat(buf, " Common Olde Bad");
                if (CAN_SPEAK(ch, LANG_DARK))
                        strcat(buf, " Dark");
                strcat(buf, ".\n\r");
                send_to_char(buf, ch);
                return;
        }

        if (!str_cmp(arg, "dark"))
        {
                if (!CAN_SPEAK(ch, LANG_DARK))
                {
                        send_to_char("You cannot speak the Dark tongue.\n\r",
                                     ch);
                        return;
                }
                if (IS_SPEAKING(ch, LANG_DARK))
                {
                        send_to_char
                                ("But you are already speaking the Dark tongue!\n\r",
                                 ch);
                        return;
                }
                ch->pcdata->language[0] = LANG_DARK;
                send_to_char("Ok.\n\r", ch);
                return;
        }
        else if (!str_cmp(arg, "common"))
        {
                if (ch->pcdata->language[0] == LANG_COMMON)
                {
                        send_to_char
                                ("But you are already speaking the common tongue!\n\r",
                                 ch);
                        return;
                }
                ch->pcdata->language[0] = LANG_COMMON;
                send_to_char("Ok.\n\r", ch);
                return;
        }
        else if (!str_cmp(arg, "olde"))
        {
                if (ch->pcdata->language[0] == DIA_OLDE)
                {
                        send_to_char
                                ("But you are already speaking Olde Worlde!\n\r",
                                 ch);
                        return;
                }
                ch->pcdata->language[0] = DIA_OLDE;
                send_to_char("Ok.\n\r", ch);
                return;
        }
        else if (!str_cmp(arg, "bad"))
        {
                if (ch->pcdata->language[0] == DIA_BAD)
                {
                        send_to_char
                                ("But you are already speaking badly!\n\r",
                                 ch);
                        return;
                }
                ch->pcdata->language[0] = DIA_BAD;
                send_to_char("Ok.\n\r", ch);
                return;
        }
/*
    else if (!str_cmp(arg,"common"))
    {
	if (ch->pcdata->language[0] < LANG_DARK)
	{
	    send_to_char("But you are already speaking the common tongue!\n\r",ch);
	    return;
	}
	ch->pcdata->language[0] = LANG_COMMON;
	if (CAN_SPEAK(ch,DIA_OLDE)) ch->pcdata->language[0] = DIA_OLDE;
	else if (CAN_SPEAK(ch,DIA_BAD)) ch->pcdata->language[0] = DIA_BAD;
	send_to_char("Ok.\n\r",ch);
	return;
    }
*/
        else
        {
                strcpy(buf, "You can speak the following languages:");
                strcat(buf, " Common Olde Bad");
                if (CAN_SPEAK(ch, LANG_DARK))
                        strcat(buf, " Dark");
                strcat(buf, ".\n\r");
                send_to_char(buf, ch);
                return;
        }

        return;
}

char     *badlang(CHAR_DATA * ch, char *argument)
{
        char      buf[MAX_STRING_LENGTH];
        char     *pName;
        int       iSyl;
        int       length;

        struct spk_type
        {
                char     *old;
                char     *new;
        };

        static const struct spk_type spk_table[] = {
                {" ", " "},
                {"my name is", "i calls meself"},
                {"are not", "aint"},
                {"have", "'av"},
                {"my", "me"},
                {"hello", "oy"},
                {"hi ", "oy "},
                {"i am", "im"},
                {"it is", "tis"},
                {"the ", "@"},
                {" the", " da"},
                {"thank", "fank"},
                {"that", "dat"},
                {"with", "wiv"},
                {"they", "day"},
                {"this", "dis"},
                {"then", "den"},
                {"there", "ver"},
                {"their", "ver"},
                {"thing", "fing"},
                {"think", "fink"},
                {"was", "woz"},
                {"would", "wud"},
                {"what ", "#"},
                {"what", "wot"},
                {"where", "weer"},
                {"when", "wen"},
                {"are", "is"},
                {"you", "ya"},
                {"your ", "&"},
                {"your", "yer"},
                {"dead", "ded"},
                {"kill", "stomp"},
                {"food", "nosh"},
                {"blood", "blud"},
                {"vampire", "sucker"},
                {"kindred", "suckers"},
                {"fire", "hot"},
                {"dwarf", "stunty"},
                {"dwarves", "stunties"},
                {"goblin", "gobbo"},
                {"death", "def"},
                {"immune", "mune"},
                {"immunit", "munit"},
                {"childer", "nippers"},
                {"childe", "nipper"},
                {"child", "nipper"},
                {"tradition", "wassname"},
                {"generation", "batch"},
                {"founded", "made"},
                {"sired", "nipped"},
                {"sire", "dad"},
                {"lineage", "istory"},
                {"road", "path"},
                {"recognize", "dats"},
                {"recognize", "dats"},
                {"decapitate", "headchop"},
                {"decap", "chop"},
                {"recites", "sez"},
                {"recite", "sez"},
                {"a", "a"}, {"b", "b"}, {"c", "c"}, {"d", "d"},
                {"e", "e"}, {"f", "f"}, {"g", "g"}, {"h", "h"},
                {"i", "i"}, {"j", "j"}, {"k", "k"}, {"l", "l"},
                {"m", "m"}, {"n", "n"}, {"o", "o"}, {"p", "p"},
                {"q", "q"}, {"r", "r"}, {"s", "s"}, {"t", "t"},
                {"u", "u"}, {"v", "v"}, {"w", "w"}, {"x", "x"},
                {"y", "y"}, {"z", "z"}, {",", ","}, {".", "."},
                {";", ";"}, {":", ":"}, {"(", "("}, {")", ")"},
                {")", ")"}, {"-", "-"}, {"!", "!"}, {"?", "?"},
                {"1", "1"}, {"2", "2"}, {"3", "3"}, {"4", "4"},
                {"5", "5"}, {"6", "6"}, {"7", "7"}, {"8", "8"},
                {"9", "9"}, {"0", "0"}, {"%", "%"}, {"", ""}
        };
        buf[0] = '\0';

        if (argument[0] == '\0')
                return argument;

        for (pName = str_dup(argument); *pName != '\0'; pName += length)
        {
                for (iSyl = 0; (length = strlen(spk_table[iSyl].old)) != 0;
                     iSyl++)
                {
                        if (!str_prefix(spk_table[iSyl].old, pName))
                        {
                                if (!str_cmp(spk_table[iSyl].new, ",") ||
                                    !str_cmp(spk_table[iSyl].new, "!") ||
                                    !str_cmp(spk_table[iSyl].new, "?"))
                                {
                                        if (strlen(buf) < MAX_INPUT_LENGTH)
                                                switch (number_range(1, 20))
                                                {
                                                default:
                                                        break;
                                                case 1:
                                                        strcat(buf,
                                                               ", ya stupid fucker");
                                                        break;
                                                case 2:
                                                        strcat(buf,
                                                               ", ya idiot");
                                                        break;
                                                case 3:
                                                        strcat(buf,
                                                               ", ya damn fool");
                                                        break;
                                                case 4:
                                                        strcat(buf,
                                                               ", ya fick mug");
                                                        break;
                                                case 5:
                                                        strcat(buf,
                                                               ", ya fuckin' jerk");
                                                        break;
                                                case 6:
                                                        strcat(buf,
                                                               ", asshole");
                                                        break;
                                                case 7:
                                                        strcat(buf,
                                                               ", dickwipe");
                                                        break;
                                                }
                                }
                                else if (!str_cmp(spk_table[iSyl].new, "@"))
                                {
                                        if (strlen(buf) >= MAX_INPUT_LENGTH)
                                                strcat(buf, "da ");
                                        else
                                                switch (number_range(1, 10))
                                                {
                                                default:
                                                        strcat(buf, "da ");
                                                        break;
                                                case 1:
                                                        strcat(buf,
                                                               "da bloody ");
                                                        break;
                                                case 2:
                                                        strcat(buf,
                                                               "da fuckin' ");
                                                        break;
                                                case 3:
                                                        strcat(buf,
                                                               "da goddamn ");
                                                        break;
                                                case 4:
                                                        strcat(buf,
                                                               "da flippin' ");
                                                        break;
                                                case 5:
                                                        strcat(buf,
                                                               "da stupid ");
                                                        break;
                                                case 6:
                                                        strcat(buf,
                                                               "da stinkin' ");
                                                        break;
                                                }
                                        break;
                                }
                                else if (!str_cmp(spk_table[iSyl].new, "#"))
                                {
                                        if (strlen(buf) >= MAX_INPUT_LENGTH)
                                                strcat(buf, "wot ");
                                        else
                                                switch (number_range(1, 10))
                                                {
                                                default:
                                                        strcat(buf, "wot ");
                                                        break;
                                                case 1:
                                                        strcat(buf,
                                                               "wot da fuck ");
                                                        break;
                                                case 2:
                                                        strcat(buf,
                                                               "wot, i sez, ");
                                                        break;
                                                case 3:
                                                        strcat(buf,
                                                               "wot da hell ");
                                                        break;
                                                case 4:
                                                        strcat(buf,
                                                               "wot da flip ");
                                                        break;
                                                }
                                        break;
                                }
                                else if (!str_cmp(spk_table[iSyl].new, "&"))
                                {
                                        if (strlen(buf) >= MAX_INPUT_LENGTH)
                                                strcat(buf, "yer ");
                                        else
                                                switch (number_range(1, 10))
                                                {
                                                default:
                                                        strcat(buf, "yer ");
                                                        break;
                                                case 1:
                                                        strcat(buf,
                                                               "yer bloody ");
                                                        break;
                                                case 2:
                                                        strcat(buf,
                                                               "yer fuckin' ");
                                                        break;
                                                case 3:
                                                        strcat(buf,
                                                               "yer damn ");
                                                        break;
                                                case 4:
                                                        strcat(buf,
                                                               "yer flippin' ");
                                                        break;
                                                case 5:
                                                        strcat(buf,
                                                               "yer stupid ");
                                                        break;
                                                case 6:
                                                        strcat(buf,
                                                               "yer stinkin' ");
                                                        break;
                                                }
                                        break;
                                }
                                strcat(buf, spk_table[iSyl].new);
                                break;
                        }
                }

                if (length == 0)
                        length = 1;
        }

        argument[0] = '\0';
        strcpy(argument, buf);
        argument[0] = UPPER(argument[0]);
        return argument;
}

char     *oldelang(CHAR_DATA * ch, char *argument)
{
        char      buf[MAX_STRING_LENGTH];
        char     *pName;
        int       iSyl;
        int       length;

        struct spk_type
        {
                char     *old;
                char     *new;
        };

        static const struct spk_type spk_table[] = {
                {" ", " "},
                {"have", "hath"},
                {"hello", "hail"},
                {"hi ", "hail "},
                {" hi", " hail"},
                {"are", "art"},
                {"your", "thy"},
                {"you", "thou"},
                {"i think", "methinks"},
                {"do ", "doth "},
                {" do", " doth"},
                {"it was", "twas"},
                {"before", "ere"},
                {"his", "$s"},
                {"a", "a"}, {"b", "b"}, {"c", "c"}, {"d", "d"},
                {"e", "e"}, {"f", "f"}, {"g", "g"}, {"h", "h"},
                {"i", "i"}, {"j", "j"}, {"k", "k"}, {"l", "l"},
                {"m", "m"}, {"n", "n"}, {"o", "o"}, {"p", "p"},
                {"q", "q"}, {"r", "r"}, {"s", "s"}, {"t", "t"},
                {"u", "u"}, {"v", "v"}, {"w", "w"}, {"x", "x"},
                {"y", "y"}, {"z", "z"}, {",", ","}, {".", "."},
                {";", ";"}, {":", ":"}, {"(", "("}, {")", ")"},
                {")", ")"}, {"-", "-"}, {"!", "!"}, {"?", "?"},
                {"1", "1"}, {"2", "2"}, {"3", "3"}, {"4", "4"},
                {"5", "5"}, {"6", "6"}, {"7", "7"}, {"8", "8"},
                {"9", "9"}, {"0", "0"}, {"%", "%"}, {"", ""}
        };
        buf[0] = '\0';

        if (argument[0] == '\0')
                return argument;

        for (pName = str_dup(argument); *pName != '\0'; pName += length)
        {
                for (iSyl = 0; (length = strlen(spk_table[iSyl].old)) != 0;
                     iSyl++)
                {
                        if (!str_prefix(spk_table[iSyl].old, pName))
                        {
                                strcat(buf, spk_table[iSyl].new);
                                break;
                        }
                }

                if (length == 0)
                        length = 1;
        }

        argument[0] = '\0';
        strcpy(argument, buf);
        argument[0] = UPPER(argument[0]);

        return argument;
}

char     *darktongue(CHAR_DATA * ch, char *argument)
{
        char      buf[MAX_STRING_LENGTH];
        char     *pName;
        int       iSyl;
        int       length;

        struct spk_type
        {
                char     *old;
                char     *new;
        };

/*
    static const struct spk_type spk_table[] =
    {
	{ " ",		" "		},
	{ "a", "i" }, { "b", "t" }, { "c", "x" }, { "d", "j" },
	{ "e", "u" }, { "f", "d" }, { "g", "k" }, { "h", "z" },
	{ "i", "o" }, { "j", "s" }, { "k", "f" }, { "l", "h" },
	{ "m", "b" }, { "n", "c" }, { "o", "e" }, { "p", "r" },
	{ "q", "l" }, { "r", "v" }, { "s", "w" }, { "t", "q" },
	{ "u", "a" }, { "v", "n" }, { "w", "y" }, { "x", "g" },
	{ "y", "m" }, { "z", "p" }, { ",", "," }, { ".", "." },
	{ ";", ";" }, { ":", ":" }, { "(", "(" }, { ")", ")" },
	{ ")", ")" }, { "-", "-" }, { "!", "!" }, { "?", "?" },
	{ "", "" }
    };
*/
        static const struct spk_type spk_table[] = {
                {" ", " "},
                {"a", "i"}, {"b", "t"}, {"c", "x"}, {"d", "j"},
                {"e", "u"}, {"f", "d"}, {"g", "k"}, {"h", "r"},
                {"i", "o"}, {"j", "s"}, {"k", "f"}, {"l", "r"},
                {"m", "b"}, {"n", "z"}, {"o", "e"}, {"p", "p"},
                {"q", "l"}, {"r", "l"}, {"s", "l"}, {"t", "s"},
                {"u", "a"}, {"v", "n"}, {"w", "y"}, {"x", "g"},
                {"y", "m"}, {"z", "p"}, {",", ","}, {".", "."},
                {";", ";"}, {":", ":"}, {"(", "("}, {")", ")"},
                {")", ")"}, {"-", "-"}, {"!", "!"}, {"?", "?"},
                {"", ""}
        };
        buf[0] = '\0';

        if (argument[0] == '\0')
                return argument;

        for (pName = str_dup(argument); *pName != '\0'; pName += length)
        {
                for (iSyl = 0; (length = strlen(spk_table[iSyl].old)) != 0;
                     iSyl++)
                {
                        if (!str_prefix(spk_table[iSyl].old, pName))
                        {
                                strcat(buf, spk_table[iSyl].new);
                                break;
                        }
                }

                if (length == 0)
                        length = 1;
        }

        argument[0] = '\0';
        strcpy(argument, buf);
        argument[0] = UPPER(argument[0]);

        return argument;
}

char     *beasttalk(CHAR_DATA * ch, char *argument)
{
        char      buf[MAX_STRING_LENGTH];
        char     *pName;
        int       iSyl;
        int       length;

        struct spk_type
        {
                char     *old;
                char     *new;
        };

        static const struct spk_type spk_table[] = {
                {" ", " "},
                {"what ", "#"},
                {"your ", "&"},
                {"the ", "@"},
                {"my ", "$"},
                {"'", "'"},
                {"a", "a"}, {"b", "b"}, {"c", "c"}, {"d", "d"},
                {"e", "e"}, {"f", "f"}, {"g", "g"}, {"h", "h"},
                {"i", "i"}, {"j", "j"}, {"k", "k"}, {"l", "l"},
                {"m", "m"}, {"n", "n"}, {"o", "o"}, {"p", "p"},
                {"q", "q"}, {"r", "r"}, {"s", "s"}, {"t", "t"},
                {"u", "u"}, {"v", "v"}, {"w", "w"}, {"x", "x"},
                {"y", "y"}, {"z", "z"}, {",", ","}, {".", "."},
                {";", ";"}, {":", ":"}, {"(", "("}, {")", ")"},
                {")", ")"}, {"-", "-"}, {"!", "!"}, {"?", "?"},
                {"1", "1"}, {"2", "2"}, {"3", "3"}, {"4", "4"},
                {"5", "5"}, {"6", "6"}, {"7", "7"}, {"8", "8"},
                {"9", "9"}, {"0", "0"}, {"%", "%"}, {"", ""}
        };
        buf[0] = '\0';

        if (argument[0] == '\0')
                return argument;

        for (pName = str_dup(argument); *pName != '\0'; pName += length)
        {
                for (iSyl = 0; (length = strlen(spk_table[iSyl].old)) != 0;
                     iSyl++)
                {
                        if (!str_prefix(spk_table[iSyl].old, pName))
                        {
                                if (!str_cmp(spk_table[iSyl].new, ",") ||
                                    !str_cmp(spk_table[iSyl].new, "!") ||
                                    !str_cmp(spk_table[iSyl].new, "?"))
                                {
                                        if (strlen(buf) < MAX_INPUT_LENGTH)
                                                switch (number_range(1, 20))
                                                {
                                                default:
                                                        break;
                                                case 1:
                                                        strcat(buf,
                                                               ", you stupid fucker");
                                                        break;
                                                case 2:
                                                        strcat(buf,
                                                               ", you idiot");
                                                        break;
                                                case 3:
                                                        strcat(buf,
                                                               ", you damn fool");
                                                        break;
                                                case 4:
                                                        strcat(buf,
                                                               ", you little shit");
                                                        break;
                                                case 5:
                                                        strcat(buf,
                                                               ", you fucking jerk");
                                                        break;
                                                case 6:
                                                        strcat(buf,
                                                               ", asshole");
                                                        break;
                                                case 7:
                                                        strcat(buf,
                                                               ", cock sucker");
                                                        break;
                                                case 8:
                                                        strcat(buf,
                                                               ", fuckwit");
                                                        break;
                                                }
                                }
                                else if (!str_cmp(spk_table[iSyl].new, "@"))
                                {
                                        if (strlen(buf) >= MAX_INPUT_LENGTH)
                                                strcat(buf, "the ");
                                        else
                                                switch (number_range(1, 10))
                                                {
                                                default:
                                                        strcat(buf, "the ");
                                                        break;
                                                case 1:
                                                        strcat(buf,
                                                               "the bloody ");
                                                        break;
                                                case 2:
                                                        strcat(buf,
                                                               "the fucking ");
                                                        break;
                                                case 3:
                                                        strcat(buf,
                                                               "the goddamn ");
                                                        break;
                                                case 4:
                                                        strcat(buf,
                                                               "the stupid ");
                                                        break;
                                                case 5:
                                                        strcat(buf,
                                                               "the stinking ");
                                                        break;
                                                }
                                        break;
                                }
                                else if (!str_cmp(spk_table[iSyl].new, "#"))
                                {
                                        if (strlen(buf) >= MAX_INPUT_LENGTH)
                                                strcat(buf, "what ");
                                        else
                                                switch (number_range(1, 5))
                                                {
                                                default:
                                                        strcat(buf, "what ");
                                                        break;
                                                case 1:
                                                        strcat(buf,
                                                               "what the fuck ");
                                                        break;
                                                case 2:
                                                        strcat(buf,
                                                               "what the hell ");
                                                        break;
                                                }
                                        break;
                                }
                                else if (!str_cmp(spk_table[iSyl].new, "&"))
                                {
                                        if (strlen(buf) >= MAX_INPUT_LENGTH)
                                                strcat(buf, "your ");
                                        else
                                                switch (number_range(1, 10))
                                                {
                                                default:
                                                        strcat(buf, "your ");
                                                        break;
                                                case 1:
                                                        strcat(buf,
                                                               "your bloody ");
                                                        break;
                                                case 2:
                                                        strcat(buf,
                                                               "your fucking ");
                                                        break;
                                                case 3:
                                                        strcat(buf,
                                                               "your damn ");
                                                        break;
                                                case 4:
                                                        strcat(buf,
                                                               "your goddamn ");
                                                        break;
                                                case 5:
                                                        strcat(buf,
                                                               "your stupid ");
                                                        break;
                                                case 6:
                                                        strcat(buf,
                                                               "your stinking ");
                                                        break;
                                                }
                                        break;
                                }
                                else if (!str_cmp(spk_table[iSyl].new, "$"))
                                {
                                        if (strlen(buf) >= MAX_INPUT_LENGTH)
                                                strcat(buf, "my ");
                                        else
                                                switch (number_range(1, 10))
                                                {
                                                default:
                                                        strcat(buf, "my ");
                                                        break;
                                                case 1:
                                                        strcat(buf,
                                                               "my bloody ");
                                                        break;
                                                case 2:
                                                        strcat(buf,
                                                               "my fucking ");
                                                        break;
                                                case 3:
                                                        strcat(buf,
                                                               "my damn ");
                                                        break;
                                                case 4:
                                                        strcat(buf,
                                                               "my goddamn ");
                                                        break;
                                                case 5:
                                                        strcat(buf,
                                                               "my stupid ");
                                                        break;
                                                case 6:
                                                        strcat(buf,
                                                               "my stinking ");
                                                        break;
                                                }
                                        break;
                                }
                                strcat(buf, spk_table[iSyl].new);
                                if (!str_cmp(spk_table[iSyl].new, " ")
                                    && strlen(buf) < MAX_INPUT_LENGTH)
                                {
                                        switch (number_range(1, 20))
                                        {
                                        default:
                                                break;
                                        case 1:
                                                strcat(buf, "*snarl* ");
                                                break;
                                        case 2:
                                                strcat(buf, "*growl* ");
                                                break;
                                        }
                                }
                                break;
                        }
                }

                if (length == 0)
                        length = 1;
        }

        argument[0] = '\0';
        strcpy(argument, buf);
        argument[0] = UPPER(argument[0]);

        return argument;
}

char     *beastchat(CHAR_DATA * ch, char *argument)
{
        char      buf[MAX_STRING_LENGTH];
        char     *pName;
        int       iSyl;
        int       length;

        struct spk_type
        {
                char     *old;
                char     *new;
        };

        static const struct spk_type spk_table[] = {
                {" ", " "},
                {"what ", "#"},
                {"your ", "&"},
                {"the ", "@"},
                {"my ", "$"},
                {"'", "'"},
                {"a", "a"}, {"b", "b"}, {"c", "c"}, {"d", "d"},
                {"e", "e"}, {"f", "f"}, {"g", "g"}, {"h", "h"},
                {"i", "i"}, {"j", "j"}, {"k", "k"}, {"l", "l"},
                {"m", "m"}, {"n", "n"}, {"o", "o"}, {"p", "p"},
                {"q", "q"}, {"r", "r"}, {"s", "s"}, {"t", "t"},
                {"u", "u"}, {"v", "v"}, {"w", "w"}, {"x", "x"},
                {"y", "y"}, {"z", "z"}, {",", ","}, {".", "."},
                {";", ";"}, {":", ":"}, {"(", "("}, {")", ")"},
                {")", ")"}, {"-", "-"}, {"!", "!"}, {"?", "?"},
                {"1", "1"}, {"2", "2"}, {"3", "3"}, {"4", "4"},
                {"5", "5"}, {"6", "6"}, {"7", "7"}, {"8", "8"},
                {"9", "9"}, {"0", "0"}, {"%", "%"}, {"", ""}
        };
        buf[0] = '\0';

        if (argument[0] == '\0')
                return argument;

        for (pName = str_dup(argument); *pName != '\0'; pName += length)
        {
                for (iSyl = 0; (length = strlen(spk_table[iSyl].old)) != 0;
                     iSyl++)
                {
                        if (!str_prefix(spk_table[iSyl].old, pName))
                        {
                                if (!str_cmp(spk_table[iSyl].new, ",") ||
                                    !str_cmp(spk_table[iSyl].new, "!") ||
                                    !str_cmp(spk_table[iSyl].new, "?"))
                                {
                                        if (strlen(buf) < MAX_INPUT_LENGTH)
                                                switch (number_range(1, 20))
                                                {
                                                default:
                                                        break;
                                                case 1:
                                                        strcat(buf,
                                                               ", you stupid fuckers");
                                                        break;
                                                case 2:
                                                        strcat(buf,
                                                               ", you idiots");
                                                        break;
                                                case 3:
                                                        strcat(buf,
                                                               ", you sad spods");
                                                        break;
                                                case 4:
                                                        strcat(buf,
                                                               ", you bunch of shits");
                                                        break;
                                                case 5:
                                                        strcat(buf,
                                                               ", you fucking jerks");
                                                        break;
                                                case 6:
                                                        strcat(buf,
                                                               ", assholes");
                                                        break;
                                                case 7:
                                                        strcat(buf,
                                                               ", cock suckers");
                                                        break;
                                                case 8:
                                                        strcat(buf,
                                                               ", fuckwits");
                                                        break;
                                                }
                                }
                                else if (!str_cmp(spk_table[iSyl].new, "@"))
                                {
                                        if (strlen(buf) >= MAX_INPUT_LENGTH)
                                                strcat(buf, "the ");
                                        else
                                                switch (number_range(1, 10))
                                                {
                                                default:
                                                        strcat(buf, "the ");
                                                        break;
                                                case 1:
                                                        strcat(buf,
                                                               "the bloody ");
                                                        break;
                                                case 2:
                                                        strcat(buf,
                                                               "the fucking ");
                                                        break;
                                                case 3:
                                                        strcat(buf,
                                                               "the goddamn ");
                                                        break;
                                                case 4:
                                                        strcat(buf,
                                                               "the stupid ");
                                                        break;
                                                case 5:
                                                        strcat(buf,
                                                               "the stinking ");
                                                        break;
                                                }
                                        break;
                                }
                                else if (!str_cmp(spk_table[iSyl].new, "#"))
                                {
                                        if (strlen(buf) >= MAX_INPUT_LENGTH)
                                                strcat(buf, "what ");
                                        else
                                                switch (number_range(1, 5))
                                                {
                                                default:
                                                        strcat(buf, "what ");
                                                        break;
                                                case 1:
                                                        strcat(buf,
                                                               "what the fuck ");
                                                        break;
                                                case 2:
                                                        strcat(buf,
                                                               "what the hell ");
                                                        break;
                                                }
                                        break;
                                }
                                else if (!str_cmp(spk_table[iSyl].new, "&"))
                                {
                                        if (strlen(buf) >= MAX_INPUT_LENGTH)
                                                strcat(buf, "your ");
                                        else
                                                switch (number_range(1, 10))
                                                {
                                                default:
                                                        strcat(buf, "your ");
                                                        break;
                                                case 1:
                                                        strcat(buf,
                                                               "your bloody ");
                                                        break;
                                                case 2:
                                                        strcat(buf,
                                                               "your fucking ");
                                                        break;
                                                case 3:
                                                        strcat(buf,
                                                               "your damn ");
                                                        break;
                                                case 4:
                                                        strcat(buf,
                                                               "your goddamn ");
                                                        break;
                                                case 5:
                                                        strcat(buf,
                                                               "your stupid ");
                                                        break;
                                                case 6:
                                                        strcat(buf,
                                                               "your stinking ");
                                                        break;
                                                }
                                        break;
                                }
                                else if (!str_cmp(spk_table[iSyl].new, "$"))
                                {
                                        if (strlen(buf) >= MAX_INPUT_LENGTH)
                                                strcat(buf, "my ");
                                        else
                                                switch (number_range(1, 10))
                                                {
                                                default:
                                                        strcat(buf, "my ");
                                                        break;
                                                case 1:
                                                        strcat(buf,
                                                               "my bloody ");
                                                        break;
                                                case 2:
                                                        strcat(buf,
                                                               "my fucking ");
                                                        break;
                                                case 3:
                                                        strcat(buf,
                                                               "my damn ");
                                                        break;
                                                case 4:
                                                        strcat(buf,
                                                               "my goddamn ");
                                                        break;
                                                case 5:
                                                        strcat(buf,
                                                               "my stupid ");
                                                        break;
                                                case 6:
                                                        strcat(buf,
                                                               "my stinking ");
                                                        break;
                                                }
                                        break;
                                }
                                strcat(buf, spk_table[iSyl].new);
/*
		if (!str_cmp(spk_table[iSyl].new," ") && strlen(buf) < MAX_INPUT_LENGTH)
		{
		    switch (number_range(1,20))
		    {
			default: break;
			case 1: strcat( buf, "*snarl* " ); break;
			case 2: strcat( buf, "*growl* " ); break;
		    }
		}
		break;
*/
                        }
                }

                if (length == 0)
                        length = 1;
        }

        argument[0] = '\0';
        strcpy(argument, buf);
        argument[0] = UPPER(argument[0]);

        return argument;
}

char     *socialc(CHAR_DATA * ch, char *argument, char *you, char *them)
{
        char      buf[MAX_STRING_LENGTH];
        char     *pName;
        int       iSyl;
        int       length;

        struct spk_type
        {
                char     *old;
                char     *new;
        };

        static const struct spk_type spk_table[] = {
                {" ", " "},
                {"you are", "$E is"},
                {"you.", "$M."},
                {"you,", "$M,"},
                {"you ", "$M "},
                {" you", " $M"},
                {"your ", "$S "},
                {" your", " $S"},
                {"yours.", "theirs."},
                {"yours,", "theirs,"},
                {"yours ", "theirs "},
                {" yours", " theirs"},
                {"begins", "begin"},
                {"caresses", "caress"},
                {"gives", "give"},
                {"glares", "glare"},
                {"grins", "grin"},
                {"licks", "lick"},
                {"looks", "look"},
                {"loves", "love"},
                {"plunges", "plunge"},
                {"presses", "press"},
                {"pulls", "pull"},
                {"runs", "run"},
                {"slaps", "slap"},
                {"slides", "slide"},
                {"smashes", "smash"},
                {"squeezes", "squeeze"},
                {"stares", "stare"},
                {"sticks", "stick"},
                {"strokes", "stroke"},
                {"tugs", "tug"},
                {"thinks", "think"},
                {"thrusts", "thrust"},
                {"whistles", "whistle"},
                {"wraps", "wrap"},
                {"winks", "wink"},
                {"wishes", "wish"},
                {" winks", " wink"},
                {" his", " your"},
                {"his ", "your "},
                {" her", " your"},
                {"her ", "your "},
                {" him", " your"},
                {"him ", "your "},
                {"the", "the"},
                {" he", " you"},
                {"he ", "you "},
                {" she", " you"},
                {"she ", "you "},
                {"a", "a"}, {"b", "b"}, {"c", "c"}, {"d", "d"},
                {"e", "e"}, {"f", "f"}, {"g", "g"}, {"h", "h"},
                {"i", "i"}, {"j", "j"}, {"k", "k"}, {"l", "l"},
                {"m", "m"}, {"n", "n"}, {"o", "o"}, {"p", "p"},
                {"q", "q"}, {"r", "r"}, {"s", "s"}, {"t", "t"},
                {"u", "u"}, {"v", "v"}, {"w", "w"}, {"x", "x"},
                {"y", "y"}, {"z", "z"}, {",", ","}, {".", "."},
                {";", ";"}, {":", ":"}, {"(", "("}, {")", ")"},
                {")", ")"}, {"-", "-"}, {"!", "!"}, {"?", "?"},
                {"1", "1"}, {"2", "2"}, {"3", "3"}, {"4", "4"},
                {"5", "5"}, {"6", "6"}, {"7", "7"}, {"8", "8"},
                {"9", "9"}, {"0", "0"}, {"%", "%"}, {"", ""}
        };
        buf[0] = '\0';

        if (argument[0] == '\0')
                return argument;

        for (pName = argument; *pName != '\0'; pName += length)
        {
                for (iSyl = 0; (length = strlen(spk_table[iSyl].old)) != 0;
                     iSyl++)
                {
                        if (!str_prefix(spk_table[iSyl].old, pName))
                        {
                                strcat(buf, spk_table[iSyl].new);
                                break;
                        }
                }


                if (length == 0)
                        length = 1;
        }

        argument[0] = '\0';
        strcpy(argument, buf);
        argument[0] = UPPER(argument[0]);

        return argument;
}

char     *socialv(CHAR_DATA * ch, char *argument, char *you, char *them)
{
        char      buf[MAX_STRING_LENGTH];
        char     *pName;
        int       iSyl;
        int       length;

        struct spk_type
        {
                char     *old;
                char     *new;
        };

        static const struct spk_type spk_table[] = {
                {" ", " "},
                {" his", " $s"},
                {"his ", "$s "},
                {" her", " $s"},
                {"her ", "$s "},
                {" him", " $m"},
                {"him ", "$m "},
                {" he", " $e"},
                {"he ", "$e "},
                {" she", " $e"},
                {"she ", "$e "},
                {"a", "a"}, {"b", "b"}, {"c", "c"}, {"d", "d"},
                {"e", "e"}, {"f", "f"}, {"g", "g"}, {"h", "h"},
                {"i", "i"}, {"j", "j"}, {"k", "k"}, {"l", "l"},
                {"m", "m"}, {"n", "n"}, {"o", "o"}, {"p", "p"},
                {"q", "q"}, {"r", "r"}, {"s", "s"}, {"t", "t"},
                {"u", "u"}, {"v", "v"}, {"w", "w"}, {"x", "x"},
                {"y", "y"}, {"z", "z"}, {",", ","}, {".", "."},
                {";", ";"}, {":", ":"}, {"(", "("}, {")", ")"},
                {")", ")"}, {"-", "-"}, {"!", "!"}, {"?", "?"},
                {"1", "1"}, {"2", "2"}, {"3", "3"}, {"4", "4"},
                {"5", "5"}, {"6", "6"}, {"7", "7"}, {"8", "8"},
                {"9", "9"}, {"0", "0"}, {"%", "%"}, {"", ""}
        };
        buf[0] = '\0';

        if (argument[0] == '\0')
                return argument;

        for (pName = argument; *pName != '\0'; pName += length)
        {
                for (iSyl = 0; (length = strlen(spk_table[iSyl].old)) != 0;
                     iSyl++)
                {
                        if (!str_prefix(spk_table[iSyl].old, pName))
                        {
                                strcat(buf, spk_table[iSyl].new);
                                break;
                        }
                }

                if (length == 0)
                        length = 1;
        }

        argument[0] = '\0';
        strcpy(argument, buf);
        argument[0] = UPPER(argument[0]);

        return argument;
}

char     *socialn(CHAR_DATA * ch, char *argument, char *you, char *them)
{
        char      buf[MAX_STRING_LENGTH];
        char     *pName;
        int       iSyl;
        int       length;

        struct spk_type
        {
                char     *old;
                char     *new;
        };

        static const struct spk_type spk_table[] = {
                {" ", " "},
                {"you are", "$N is"},
                {"you.", "$N."},
                {"you,", "$N,"},
                {"you ", "$N "},
                {" you", " $N"},
                {"your.", "$N's."},
                {"your,", "$N's,"},
                {"your ", "$N's "},
                {" your", " $N's"},
                {"yourself", "$Mself"},
                {" his", " $s"},
                {"his ", "$s "},
                {" her", " $s"},
                {"her ", "$s "},
                {" him", " $m"},
                {"him ", "$m "},
                {" he", " $e"},
                {"he ", "$e "},
                {" she", " $e"},
                {"she ", "$e "},
                {"a", "a"}, {"b", "b"}, {"c", "c"}, {"d", "d"},
                {"e", "e"}, {"f", "f"}, {"g", "g"}, {"h", "h"},
                {"i", "i"}, {"j", "j"}, {"k", "k"}, {"l", "l"},
                {"m", "m"}, {"n", "n"}, {"o", "o"}, {"p", "p"},
                {"q", "q"}, {"r", "r"}, {"s", "s"}, {"t", "t"},
                {"u", "u"}, {"v", "v"}, {"w", "w"}, {"x", "x"},
                {"y", "y"}, {"z", "z"}, {",", ","}, {".", "."},
                {";", ";"}, {":", ":"}, {"(", "("}, {")", ")"},
                {")", ")"}, {"-", "-"}, {"!", "!"}, {"?", "?"},
                {"1", "1"}, {"2", "2"}, {"3", "3"}, {"4", "4"},
                {"5", "5"}, {"6", "6"}, {"7", "7"}, {"8", "8"},
                {"9", "9"}, {"0", "0"}, {"%", "%"}, {"", ""}
        };
        buf[0] = '\0';

        if (argument[0] == '\0')
                return argument;

        for (pName = argument; *pName != '\0'; pName += length)
        {
                for (iSyl = 0; (length = strlen(spk_table[iSyl].old)) != 0;
                     iSyl++)
                {
                        if (!str_prefix(spk_table[iSyl].old, pName))
                        {
                                strcat(buf, spk_table[iSyl].new);
                                break;
                        }
                }

                if (length == 0)
                        length = 1;
        }

        argument[0] = '\0';
        strcpy(argument, buf);
        argument[0] = UPPER(argument[0]);

        return argument;
}



void do_telepath(CHAR_DATA * ch, char *argument)
{
        int       class = ch->class;

        if (IS_NPC(ch)
            || (!IS_IMMORTAL(ch) && !IS_CLASS(ch, CLASS_SHAPESHIFTER)))
        {
                send_to_char("Huh?\n\r", ch);
                return;
        }
        ch->class = CLASS_SHAPESHIFTER;
        talk_channel(ch, argument, CHANNEL_CLASS, CC_SHIFTER, "telepath");
        ch->class = class;
        return;
}

void do_chitter(CHAR_DATA * ch, char *argument)
{
        int       class = ch->class;

        if (IS_NPC(ch) || (!IS_IMMORTAL(ch) && !IS_CLASS(ch, CLASS_DROID)))
        {
                send_to_char("Huh?\n\r", ch);
                return;
        }
        ch->class = CLASS_DROID;
        talk_channel(ch, argument, CHANNEL_CLASS, CC_DROID, "droidtalk");
        ch->class = class;
        return;
}


void do_transmit(CHAR_DATA * ch, char *argument)
{
        int       class = ch->class;

        if (IS_NPC(ch) || (!IS_IMMORTAL(ch) && !IS_CLASS(ch, CLASS_CYBORG)))
        {
                send_to_char("Huh?\n\r", ch);
                return;
        }
        ch->class = CLASS_CYBORG;
        talk_channel(ch, argument, CHANNEL_CLASS, CC_CYBORG, "borgtalk");
        ch->class = class;
}

void hint_channel(char *msg)
{
        char      buf[MAX_STRING_LENGTH];
        DESCRIPTOR_DATA *d;

        sprintf(buf, "\n\r#0[#RHINT#0] #W%s\n\r", msg); /* Add color if you wish */

        for (d = descriptor_list; d != NULL; d = d->next)
        {
                CHAR_DATA *victim;

                victim = d->original ? d->original : d->character;

                if (d->connected == CON_PLAYING &&
                    !IS_SET(victim->deaf, CHANNEL_HINT))
                {
                        send_to_char(buf, victim);
                }
        }

        return;
}
void do_gsocial(CHAR_DATA * ch, char *argument)
{
        char      command[MAX_INPUT_LENGTH];
        CHAR_DATA *victim;
        int       cmd;
        bool      found;
        char      arg[MAX_INPUT_LENGTH];
        DESCRIPTOR_DATA *d;
        char      buf[MAX_STRING_LENGTH];
        int       counter;
        int       count;
        char      buf2[MAX_STRING_LENGTH];

        argument = one_argument(argument, command);

        if (command[0] == '\0')
        {
                send_to_char("Something is wrong\n\r", ch);
        }

        found = FALSE;
        for (cmd = 0; social_table[cmd].name[0] != '\0'; cmd++)
        {
                if (command[0] == social_table[cmd].name[0]
                    && !str_prefix(command, social_table[cmd].name))
                {
                        found = TRUE;
                        break;
                }
        }

        if (!found)
        {
                send_to_char("What kind of social is that?!?!\n\r", ch);
                return;
        }


        if (!IS_NPC(ch) && IS_SET(ch->deaf, CHANNEL_SOCIAL))
        {
                send_to_char
                        ("But you have the social channel turned off!\n\r",
                         ch);
                return;
        }

        switch (ch->position)
        {
        case POS_DEAD:
                send_to_char("Lie still; you are DEAD!\n\r", ch);
                return;
        case POS_INCAP:
        case POS_MORTAL:
                send_to_char("You are hurt far too bad for that.\n\r", ch);
                return;
        case POS_STUNNED:
                send_to_char("You are too stunned for that.\n\r", ch);
                return;
        }

        one_argument(argument, arg);
        victim = NULL;
        if (arg[0] == '\0')
        {
                sprintf(buf, "#W[#0Social#W]#R:#n %s",
                        social_table[cmd].char_no_arg);
                act(buf, ch, NULL, NULL, TO_CHAR);
                sprintf(buf, "#W[#0Social#W]#R:#n %s",
                        social_table[cmd].others_no_arg);
                for (d = descriptor_list; d != NULL; d = d->next)
                {
                        CHAR_DATA *vch;

                        vch = d->original ? d->original : d->character;
                        if (d->connected == CON_PLAYING &&
                            d->character != ch &&
                            !IS_SET(vch->deaf, CHANNEL_SOCIAL))
                        {
                                act(buf, ch, NULL, vch, TO_VICT);
                        }
                }
        }
        else if ((victim = get_char_world(ch, arg)) == NULL)
        {
                send_to_char("They aren't here.\n\r", ch);
                return;
        }
        else if (victim == ch)
        {
                sprintf(buf, "#W[#0Social#W]#R:#n %s",
                        social_table[cmd].char_auto);
                act(buf, ch, NULL, NULL, TO_CHAR);
                sprintf(buf, "#W[#0Social#W]#R:#n %s",
                        social_table[cmd].others_auto);
                for (d = descriptor_list; d != NULL; d = d->next)
                {
                        CHAR_DATA *vch;

                        vch = d->original ? d->original : d->character;
                        if (d->connected == CON_PLAYING &&
                            d->character != ch &&
                            !IS_SET(vch->deaf, CHANNEL_SOCIAL))
                        {
                                act(buf, ch, NULL, vch, TO_VICT);
                        }
                }
        }
        else
        {
                sprintf(buf, "#W[#0Social#W]#R:#n %s",
                        social_table[cmd].char_found);
                act(buf, ch, NULL, victim, TO_CHAR);
                sprintf(buf, "#W[#0Social#W]#R:#n %s",
                        social_table[cmd].vict_found);
                act(buf, ch, NULL, victim, TO_VICT);

                sprintf(buf, "#W[#0Social#W]#R:#n %s",
                        social_table[cmd].others_found);
                for (counter = 0; buf[counter + 1] != '\0'; counter++)
                {
                        if (buf[counter] == '$' && buf[counter + 1] == 'N')
                        {
                                strcpy(buf2, buf);
                                buf2[counter] = '\0';
                                strcat(buf2, victim->name);
                                for (count = 0; buf[count] != '\0'; count++)
                                {
                                        buf[count] = buf[count + counter + 2];
                                }
                                strcat(buf2, buf);
                                strcpy(buf, buf2);

                        }
                        else if (buf[counter] == '$'
                                 && buf[counter + 1] == 'E')
                        {
                                switch (victim->sex)
                                {
                                default:
                                        strcpy(buf2, buf);
                                        buf2[counter] = '\0';
                                        strcat(buf2, "it");
                                        for (count = 0; buf[count] != '\0';
                                             count++)
                                        {
                                                buf[count] =
                                                        buf[count + counter +
                                                            2];
                                        }
                                        strcat(buf2, buf);
                                        strcpy(buf, buf2);
                                        break;
                                case 1:
                                        strcpy(buf2, buf);
                                        buf2[counter] = '\0';
                                        strcat(buf2, "it");
                                        for (count = 0; buf[count] != '\0';
                                             count++)
                                        {
                                                buf[count] =
                                                        buf[count + counter +
                                                            2];
                                        }
                                        strcat(buf2, buf);
                                        strcpy(buf, buf2);
                                        break;
                                case 2:
                                        strcpy(buf2, buf);
                                        buf2[counter] = '\0';
                                        strcat(buf2, "it");
                                        for (count = 0; buf[count] != '\0';
                                             count++)
                                        {
                                                buf[count] =
                                                        buf[count + counter +
                                                            2];
                                        }
                                        strcat(buf2, buf);
                                        strcpy(buf, buf2);
                                        break;
                                }
                        }
                        else if (buf[counter] == '$'
                                 && buf[counter + 1] == 'M')
                        {
                                buf[counter] = '%';
                                buf[counter + 1] = 's';
                                switch (victim->sex)
                                {
                                default:
                                        strcpy(buf2, buf);
                                        buf2[counter] = '\0';
                                        strcat(buf2, "it");
                                        for (count = 0; buf[count] != '\0';
                                             count++)
                                        {
                                                buf[count] =
                                                        buf[count + counter +
                                                            2];
                                        }
                                        strcat(buf2, buf);
                                        strcpy(buf, buf2);
                                        break;
                                case 1:
                                        strcpy(buf2, buf);
                                        buf2[counter] = '\0';
                                        strcat(buf2, "him");
                                        for (count = 0; buf[count] != '\0';
                                             count++)
                                        {
                                                buf[count] =
                                                        buf[count + counter +
                                                            2];
                                        }
                                        strcat(buf2, buf);
                                        strcpy(buf, buf2);
                                        break;
                                case 2:
                                        strcpy(buf2, buf);
                                        buf2[counter] = '\0';
                                        strcat(buf2, "her");
                                        for (count = 0; buf[count] != '\0';
                                             count++);
                                        {
                                                buf[count] =
                                                        buf[count + counter +
                                                            2];
                                        }
                                        strcat(buf2, buf);
                                        strcpy(buf, buf2);
                                        break;
                                }
                        }
                        else if (buf[counter] == '$'
                                 && buf[counter + 1] == 'S')
                        {
                                switch (victim->sex)
                                {
                                default:
                                        strcpy(buf2, buf);
                                        buf2[counter] = '\0';
                                        strcat(buf2, "its");
                                        for (count = 0; buf[count] != '\0';
                                             count++)
                                        {
                                                buf[count] =
                                                        buf[count + counter +
                                                            2];
                                        }
                                        strcat(buf2, buf);
                                        strcpy(buf, buf2);
                                        break;
                                case 1:
                                        strcpy(buf2, buf);
                                        buf2[counter] = '\0';
                                        strcat(buf2, "his");
                                        for (count = 0; buf[count] != '\0';
                                             count++)
                                        {
                                                buf[count] =
                                                        buf[count + counter +
                                                            2];
                                        }
                                        strcat(buf2, buf);
                                        strcpy(buf, buf2);
                                        break;
                                case 2:
                                        strcpy(buf2, buf);
                                        buf2[counter] = '\0';
                                        strcat(buf2, "hers");
                                        for (count = 0; buf[count] != '\0';
                                             count++)
                                        {
                                                buf[count] =
                                                        buf[count + counter +
                                                            2];
                                        }
                                        strcat(buf2, buf);
                                        strcpy(buf, buf2);
                                        break;
                                }
                        }

                }
                for (d = descriptor_list; d != NULL; d = d->next)
                {
                        CHAR_DATA *vch;

                        vch = d->original ? d->original : d->character;
                        if (d->connected == CON_PLAYING &&
                            d->character != ch &&
                            d->character != victim &&
                            !IS_SET(vch->deaf, CHANNEL_SOCIAL))
                        {
                                act(buf, ch, NULL, vch, TO_VICT);
                        }
                }
        }
        return;
}
