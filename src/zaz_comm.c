/*-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-
 ^    _/_/_/_/                              _/      _/  _/    _/  _/_/_/  ^
 ^   _/        _/_/      _/_/_/  _/  _/_/  _/_/  _/_/  _/    _/  _/    _/ ^
 ^  _/_/_/  _/_/_/_/  _/    _/  _/_/      _/  _/  _/  _/    _/  _/    _/  ^
 ^ _/      _/        _/    _/  _/        _/      _/  _/    _/  _/    _/   ^
 ^_/        _/_/_/    _/_/_/  _/        _/      _/    _/_/    _/_/_/      ^
 ^------------------------------------------------------------------------^
 ^ fearMUD Copyright 2001-2004 by the fearMUD team:                       ^
 ^ Philip Jackson (Zazel): t3hphil@gmail.com                              ^
 ^-----------------------------------------------------------------------*/

#include <sys/types.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include "merc.h"
#include "memwatch.h"


/* 
 * For tells we dont want any chance of players finding an exploit
 * to see someone else's messages.  So we force all the arguments
 * on them.  
 */
void do_zhistory(CHAR_DATA *ch){
	if (IS_NPC(ch))
		return;
	chathist( ch, CHANNEL_TELL, ch->name, NULL, 0 );
	ch->tells = 0; // Clears their tell alert
}

void do_showchannels(CHAR_DATA *ch){
  	char buf[MAX_STRING_LENGTH];
  	char buf2[MAX_STRING_LENGTH];
  	int i;
	load_channels();
  	if (IS_NPC(ch)) return;

  	for (i = 1; i <= MAX_CHANNEL; i++){
    		sprintf(buf2, "Name %s  ID: %d  Min Level: %d\n\r", channel_table[i].name, channel_table[i].id, channel_table[i].minLevel);
    		strcat(buf, buf2);
  	}
  	send_to_char(buf, ch);
  	return;
}



void do_chathist(CHAR_DATA *ch, char *argument ){
        char arg1[MSL];
        char arg2[MSL];
        char arg3[MSL];
        char chann[MSL];
        char buf[MSL];
        int chan = 0;
        int class = 0;
        char *name = NULL; 
        char *search = NULL;
        bool found = FALSE;
        bool permission = FALSE;
        int i = 0;
	//char kingOne[MSL];
	//char kingTwo[MSL];


        // Set the arg variables
        argument = one_argument( argument, arg1 );
        argument = one_argument( argument, arg2 );
        argument = one_argument( argument, arg3 );

        // If the first character in arg1 is nothing, then we want to goto fuxed
        if ( arg1[0] == '\0' ){
                goto fuxed;
        }
        // Players only
        if (IS_NPC(ch)) return;

        /*
         * In jobo's comm system each channel is uniquely identifed but an INT
         * This section takes the argument from the player and assigns int chan
         * the proper number for the channel
         */

         for (i = 1; i <= MAX_CHANNEL; i++){
                 if (!str_cmp(arg1, channel_table[i].name)){
                         chan = channel_table[i].id;
                         found = TRUE;
                         if(ch->level >= channel_table[i].minLevel){
                                 permission = TRUE;
                         }
                 }

         }
/*	 if ( !str_cmp(arg1, "ktalk") && ch->pcdata->kingdom > 0){
		 found = TRUE;
		 permission = TRUE;
		 chan = 23498723;

	 } 

*/

         if (found == FALSE || permission == FALSE){
                 goto fuxed;
         } 

        if (!str_cmp(arg2, "player")){ // if the second argument is player, we want to set the name variable
                name = arg3;
                goto winz;
        } else if (!str_cmp(arg2, "search")){ // if the second argument is search, we want to se the search variable
                search = arg3;
                goto winz;
        } else {
                goto winz; // if there is no arg2, then we already have all the variables needed
        }
        /*      
         * We use fuxed to display the syntax and channel options to the player
         * All this does is display text, and return
         */
        fuxed:
                send_to_char("Syntax: [last/chathist] [channel] [search/player] [search string/player name]\n\r",ch);
                send_to_char("Please choose chat/music/auction/hint/yell/trivia/info/flame/newbie\n\r",ch);
		if (ch->pcdata->kingdom > 0)
			send_to_char("#WAs you have a kingdom you can also do last ktalk#n\n\4",ch);
		if (ch->class > 0)
                        send_to_char("#WAs you have a class you can also do last class#n\n\r",ch);
                if (ch->trust > 6 && ch->trust < 10)
		        send_to_char("#WAs an immortal you may also do immtalk#n\n\r",ch);
                if (ch->trust > 10 && ch->trust < 12)
                        send_to_char("#WAs an administrator you may also do immtalk/admtalk#n\n\r",ch);
                if(ch->trust > 11)
                        send_to_char("#WAs an implimentor you may also do immtalk/admtalk/imptalk#n\n\r",ch);
                return;
        /*
         * Passes the arguments to chathist().
         * Note that name, search, and class are NULL by default
         */
        winz:
                chathist(ch, chan, name, search, class);
                return;
}



void load_channels(){
        int i;
        FILE *fp;

        channel_table[0].name = "";
        channel_table[0].id = 0;

        if ((fp = fopen("../txt/channels.txt", "r")) == NULL){
                log_string(LOG_ERR,"Fatal Error: channels.txt not found!");
                exit(1);
        }

        for (i = 1; i <= MAX_CHANNEL; i++){
                channel_table[i].name = fread_string(fp);
                channel_table[i].id = fread_number(fp);
                channel_table[i].minLevel = fread_number(fp);
        }
  fclose(fp);
}














/* 
 * This function is the meat of our chat logging system.
 * It is responsible for structuring our query, as well as
 * displaying output to the player, after it has passed the
 * initial error checking by do_chathist()
 */
void chathist( CHAR_DATA *ch, int chan, char *name, char *search, int class ){       
        char buf[MSL];
        char query[MSL];
	 char chann[MSL];
	 char channelFinal[MSL];
         int i = 0;
	 int done = 0;
         int numrows = 0;
	 char mes[MSL];
	 char kingOne[MSL];
	 char kingTwo[MSL];
         MYSQL_ROW row = 0;

	/*
	 * The first thing we do is figure out if this is for class chan or not.
	 * If it is a class chan, then we need to set the unique identifer for it
	 * In the case of class chats, the class number is the channel unique identifer
	 * Please note that we use chanName() to convert the number into a unique
	 * identifer string.  If class is NULL notice that it will automatically go
	 * to the else in this statement
	 */
	xprintf(chann, "%d", chan);
	if( chan == CHANNEL_CLASS && (!str_cmp(getClassByNum(class), getClassByNum(ch->class))) && ch->class != 0 ){
		xprintf(channelFinal, getClassByNum(ch->class));
	/*} else if ( ch->pcdata->kingdom > 0 && chan == 23498723)  {
		 strip_color(kingOne,  get_kingdomname(ch->pcdata->kingdom) );
		 xprintf(channelFinal, "king_%s", kingOne); */
	} else {
		xprintf(channelFinal, chanName(chan));
	}
					
	/*
	 * Im sure there is a better way to do this...but its the way i came up with
	 * This is the section which will decide what query is necessary.
	 * It does this based on if variables are NULL or not.
	 */
	if (chann[0] != '\0' && !name && !search){
   		xprintf(query, "select date, name, message from chatHist where date_sub(curdate(), interval 1 day) <= date and channel='%s' order by date asc", channelFinal );
	} else if ( chann[0] != '\0' && name ){
		xprintf(query, "select date, name, message, victim from chatHist where date_sub(curdate(), interval 30 day) <= date and (name='%s' or victim='%s') and channel='%s' order by date asc", name, name, channelFinal);
	} else if ( chann[0] != '\0' && search ){
		xprintf(query, "select date, name, message from chatHist where date_sub(curdate(), interval 30 day) <= date and message like '%%%s%%' and channel='%s'", search, channelFinal);
  	} else { // This should /never/ happen.  all our error checking has been done by now, if you get this message, you fucked up
		send_to_char("something fucked up", ch);
		goto clean;
	}
	// Connect to SQL and if the query isnt set yet, drop out to a clean exit
	sql_connect("localhost", "", "xraMud", SQLUSER, SQLPASS);
	if (!sql_query(query))
		goto clean;
	// We set numrows to the number of results we got, if numrows = 0, then we have chosen a proper channel, but there is no data to show
	// Notice we always do a clean exit, if you havent noticed...there is only one return in this function.
        numrows = sql_num_rows(query);
	if (numrows < 1){
		send_to_char("No chat history available\n\r", ch);
		goto clean;
	}
	/* 
	 * Ok we did good.  We have passed the error checking of do_chathist() we have a query, and it has results
	 * Now we can display the data we got.
	 * First thing we do is display what channel we are doing history for.
	 */
        xprintf(buf, "#R------#C%s #cHistory#R------#n\n\r", capitalize(chanName(chan)));
	send_to_char(buf, ch);
	/* 
	 * We do this for loop the number of times defined in numrows
	 */
        for ( i = numrows - 1; i >=0;  i--){
	       	row = sql_fetch_row(query); // We define an array that is equal to the query....its going to get one row at a time
		/* 
		 * This version of mysql is not able to do subqueries in the way we wanted, so when we did our query
		 * we got 100's or 1000's of rows...so we have to discard the ones we dont want, we just want
		 * the latest 30
		 */
		if ( i - 30 < 5 ){ 
			strcpy( mes, row[2] ); // row[2] is equal to the message, so we dump it into a new char[]
			restore_quote( mes ); // we want to turn all our backticks back into single quotes
			/*
			 * For tells we alternate the color of names, to make it easier to tell which
			 * messages are from you.  
			 */
			if( str_cmp(chanName(chan), chanName(CHANNEL_TELL))){
			        xprintf( buf, "#C[#c%s#C] #R%s#r: #w%s#n\n\r",row[0], row[1], mes);
			} else {

				if ((ch->tells >= i) && ch->tells > 0){
					if (str_cmp(ch->name, row[3])){
					        xprintf( buf, "#Y[#y%s#Y] #C%s#r: #w%s#n\n\r",row[0], row[1], mes);
					} else {
					        xprintf( buf, "#Y[#y%s#Y] #R%s#r: #w%s#n\n\r",row[0], row[1], mes);
					} 
	

				} else {
					if (str_cmp(ch->name, row[3])){
					        xprintf( buf, "#L[#l%s#L] #C%s#r: #w%s#n\n\r",row[0], row[1], mes);
					} else {
					        xprintf( buf, "#L[#l%s#L] #R%s#r: #w%s#n\n\r",row[0], row[1], mes);
					}
	
				}
			}
			send_to_char( buf, ch); // Send one row at a time, notice where this happens in our loop
		}
        }
	clean: // There is no option in this one...we /always/ do a clean exit.  You /must/ close your sql connections properly
	sql_disconnect();
	return;
}
/*void do_zlogshow(CHAR_DATA *ch){
	CHAR_DATA *vch;
        CHAR_DATA *vch_next;
        char buf[MAX_STRING_LENGTH];

        for ( vch = char_list; vch != NULL; vch = vch_next ){
	        vch_next = vch->next;
        	if ( !IS_NPC(vch) && get_trust( vch ) < get_trust( ch ) ) {
		        if (IS_SET(vch->act, PLR_ZLOG)){
				xprintf(buf, "%s\n\r", vch->pcdata->switchname);
				send_to_char(buf,ch);
			}
            	}
        }
	send_to_char("zlog listing complete...\n\r",ch);
	return;
}*/

void do_zlogview(CHAR_DATA *ch, char *argument ){
        char buf[MAX_STRING_LENGTH];
        char query[MAX_INPUT_LENGTH];
	char arg1[MSL];
	char arg2[MSL];
	int done = 0;
	int i = 0;
        int numrows;
	char mes[MSL];
        MYSQL_ROW row;
       
	argument = one_argument( argument, arg1 );
	argument = one_argument( argument, arg2 );

        if (ch->level < 12){
		send_to_char("Imm only",ch);
		return;
	}
        if ( arg1[0] == '\0' ){
		send_to_char("Syntax: zlogview *name* *search string*\n\r", ch);
		return;
  	} else if (arg2[0] != '\0' ){
		xprintf(query, "select date, name, message from log where name='%s' and channel='manual_log' and message like '%%%s%%' order by date asc", arg1, arg2);
	} else {
		xprintf(query, "select date, name, message from log where name='%s' and channel='manual_log' order by date asc", arg1 );
	}
	sql_connect("localhost", "", "xraMud", SQLUSER, SQLPASS);
	if (!sql_query(query))
		goto clean;
        numrows = sql_num_rows(query);
	if (numrows < 1){
		send_to_char("No log available\n\r", ch);
		goto clean;
	}
        send_to_char("#R------#rzlog Hist#R------#n\n\r", ch);
        for ( i = numrows; i >0;  i--){
	       	row = sql_fetch_row(query);
		if ( i - 30 < 5 ){
			strcpy( mes, row[2] );
			restore_quote( mes );
		        xprintf( buf, "[%s] %s: %s#n\n\r",row[0], row[1], mes);
			send_to_char( buf, ch);
		}
        }
	clean:
	sql_disconnect();
	return;
}

/*
 * Our communication logging systems entry point
 * This function is pretty simple, you pass it the info it needs.
 * If something is not applicable, then just pass it in NULL
 * Note that at the very least you need to pass *ch, int channel, and *mes
 */
void log_comm( char *ch, char *victim, int channel, char *mes, int class ){
	char query[MSL];
	char buf[MSL];
	strip_color(buf,mes);
	smash_quote( buf );
	if (victim == NULL && class < 1) // If there is no victim, and no class, then its a normal public channel
		xprintf(query, "insert into chatHist ( name, victim, date, channel, message ) VALUES ('%s', '%s', now(), '%s', '%s')", ch, "NULL", chanName(channel), buf);
	else if (class > 0 && victim == NULL) // if there is a class and no victim, its a class private chat
		xprintf(query, "insert into chatHist ( name, victim, date, channel, message ) VALUES ('%s', '%s', now(), '%s', '%s')", ch, "NULL", getClassByNum(class), buf);
	else if (class == 362872929 && victim[0] != '\0') // we got our selves a kingdom chat...HAX
		xprintf(query, "insert into chatHist ( name, victim, date, channel, message ) VALUES ('%s', '%s', now(), '%s', '%s')", ch, "NULL", victim, buf);
	else  // if its neither of those, its obviously gatta be a tell, tells always have victims
		xprintf(query, "insert into chatHist ( name, victim, date, channel, message ) VALUES ('%s', '%s', now(), '%s', '%s')", ch, victim, chanName(channel), buf);
		// log_string(LOG_GAME, query);

	// Proper connection, execution, and disconnection of SQL
	sql_connect("localhost", "", "xraMud", SQLUSER, SQLPASS);
	sql_execute(query);
	sql_disconnect();
}

// Pass a channel id to *chanName, and he just loves to give you a string back for them
char *chanName(int chan){
	static char buf[MSL];

	buf[0] = '\0';
	if (chan == CHANNEL_KINGDOM) xprintf(buf, "kingdom");
	else if (chan == CHANNEL_CHAT) xprintf(buf, "chat");
	else if (chan == CHANNEL_CLASS) xprintf(buf, "class");
	else if (chan == CHANNEL_IMMTALK) xprintf(buf, "immtalk");
	else if (chan == CHANNEL_MUSIC) xprintf(buf, "music");
	else if (chan == CHANNEL_AUCTION) xprintf(buf, "auction");
	else if (chan == CHANNEL_HINT) xprintf(buf, "hint");
	else if (chan == CHANNEL_YELL) xprintf(buf, "yell");
	else if (chan == CHANNEL_TRIVIA) xprintf(buf, "trivia");
	else if (chan == CHANNEL_HOWL) xprintf(buf, "howl");
	else if (chan == CHANNEL_LOG) xprintf(buf, "log");
	else if (chan == CHANNEL_PRAY) xprintf(buf, "pray");
	else if (chan == CHANNEL_INFO) xprintf(buf, "info");
	else if (chan == CHANNEL_FLAME) xprintf(buf, "flame");
	else if (chan == CHANNEL_TELL) xprintf(buf, "tell");
	else if (chan == CHANNEL_RELIGION) xprintf(buf, "religion");
	else if (chan == CHANNEL_ROLEPLAY) xprintf(buf, "roleplay");
	else if (chan == CHANNEL_NEWBIE) xprintf(buf, "newbie");
	else if (chan == CHANNEL_QUEST) xprintf(buf, "quest");
	else if (chan == CHANNEL_SOCIAL) xprintf(buf, "social");
//	else if (chan == CHANNEL_IMPTALK) xprintf(buf, "imptalk");
//	else if (chan == CHANNEL_ADMTALK) xprintf(buf, "admtalk");
        else xprintf(buf, "none");
	return buf;
}

// when you *getClassByNum, you get a string back
char *getClassByNum(int class){
	static char class2[MSL];
	class2[0] = '\0';
        if (class == CLASS_DEMON) xprintf(class2,"Demon");
        else if (class == CLASS_MAGE) xprintf(class2,"Battlemage");
        else if (class == CLASS_WEREWOLF) xprintf(class2,"Werewolf");
        else if (class == CLASS_VAMPIRE) xprintf(class2,"Vampire");
        else if (class == CLASS_SAMURAI) xprintf(class2,"Samurai");
        else if (class == CLASS_DROW) xprintf(class2,"Drow");
        else if (class == CLASS_MONK) xprintf(class2,"Monk");
        else if (class == CLASS_NINJA) xprintf(class2,"Ninja");
        else if (class == CLASS_LICH) xprintf(class2,"Lich");
        else if (class == CLASS_TANARRI) xprintf(class2,"Tanarri");
        else if (class == CLASS_ANGEL) xprintf(class2,"Angel");
        else if (class == CLASS_UNDEAD_KNIGHT) xprintf(class2,"Undead Knight");
        else if (class == CLASS_CYBORG) xprintf(class2,"Cyborg");
        else if (class == CLASS_DRACONIAN) xprintf(class2,"Draconian");
        else if (class == CLASS_SKYBLADE) xprintf(class2,"Skyblade");
        else if (class == CLASS_GHOUL) xprintf(class2,"Ghoul");
        else if (class == CLASS_THIEF) xprintf(class2,"Thief");
        else if (class == CLASS_PRIEST) xprintf(class2,"Priest");
        else if (class == CLASS_HOBBIT) xprintf(class2,"Hobbit");
        else if (class == CLASS_JEDI) xprintf(class2,"Jedi");
        else if (class == CLASS_SHAPESHIFTER) xprintf(class2,"Shapeshifter");
        else if (class == CLASS_SHADOW) xprintf(class2,"Shadow");
        else if (class == CLASS_GIANT) xprintf(class2,"Giant");
        else if (class == CLASS_FAE) xprintf(class2,"Fae");
        else if (class == CLASS_DRONE) xprintf(class2,"Drone");
        else if (class == CLASS_DROID) xprintf(class2,"Droid");
        else if (class == CLASS_ELEMENTAL) xprintf(class2,"Elemental");
        else if (class == CLASS_PALADIN) xprintf(class2,"Paladin");
        else if (class == CLASS_BLADE_MASTER) xprintf(class2,"Blademaster");
        else if (class == CLASS_DRAGON) xprintf(class2,"Dragon");
        else if (class == CLASS_ANTIPALADIN) xprintf(class2,"Antipaladin");
        else if (class == CLASS_SHINOBI) xprintf(class2,"Shinobi");
        else  xprintf(class2,"None");
	return class2;
}
// Get yourself a class id when ya want to use a name
int getClassByName(char * name){
	int classNum = 0;
		if(!str_cmp(name,"demon"))
			classNum = CLASS_DEMON;
		else if(!str_cmp(name, "battlemage"))
			classNum = CLASS_MAGE;
		else if(!str_cmp(name, "werewolf"))
			classNum = CLASS_WEREWOLF;
		else if(!str_cmp(name, "vampire"))
			classNum = CLASS_VAMPIRE;
		else if(!str_cmp(name, "samurai"))
			classNum = CLASS_SAMURAI;
		else if(!str_cmp(name, "drow"))
			classNum = CLASS_DROW;
		else if(!str_cmp(name, "monk"))
			classNum = CLASS_MONK;
		else if(!str_cmp(name, "ninja"))
			classNum = CLASS_NINJA;
		else if(!str_cmp(name, "lich"))
			classNum = CLASS_LICH;
		else if(!str_cmp(name, "tanarri"))
			classNum = CLASS_TANARRI;
		else if(!str_cmp(name, "angel"))
			classNum = CLASS_ANGEL;
		else if(!str_cmp(name, "undeadknight"))
			classNum = CLASS_UNDEAD_KNIGHT;
		else if(!str_cmp(name, "cyborg"))
			classNum = CLASS_CYBORG;
		else if(!str_cmp(name, "draconian"))
			classNum = CLASS_DRACONIAN;
		else if(!str_cmp(name, "skyblade"))
			classNum = CLASS_SKYBLADE;
		else if(!str_cmp(name, "ghoul"))
			classNum = CLASS_GHOUL;
		else if(!str_cmp(name, "thief"))
			classNum = CLASS_THIEF;
		else if(!str_cmp(name, "priest"))
			classNum = CLASS_PRIEST;
		else if(!str_cmp(name, "hobbit"))
			classNum = CLASS_HOBBIT;
		else if(!str_cmp(name, "jedi"))
			classNum = CLASS_JEDI;
		else if(!str_cmp(name, "shapeshifter"))
			classNum = CLASS_SHAPESHIFTER;
		else if(!str_cmp(name, "shadow"))
			classNum = CLASS_SHADOW;
		else if(!str_cmp(name, "giant"))
			classNum = CLASS_GIANT;
		else if(!str_cmp(name, "fae"))
			classNum = CLASS_FAE;
		else if(!str_cmp(name, "drone"))
			classNum = CLASS_DRONE;
		else if(!str_cmp(name, "droid"))
			classNum = CLASS_DROID;
		else if(!str_cmp(name, "elemental"))
			classNum = CLASS_ELEMENTAL;
		else if(!str_cmp(name, "paladin"))
			classNum = CLASS_PALADIN;
		else if(!str_cmp(name, "dragon"))
			classNum = CLASS_DRAGON;
		else if(!str_cmp(name, "antipaladin"))
			classNum = CLASS_ANTIPALADIN;
              else if(!str_cmp(name, "shinobi"))
			classNum = CLASS_SHINOBI;
		else 
			classNum = 0;

	return classNum;
}




// Old way i had of doing chathist(), im still angry and i cant figure out 
// why it dosent work right...it should, the logic is correct, it just
// causes weird artifacting/random characters to show up in output.
/*void chathist( CHAR_DATA *ch, int chan, char *name )
{       
        char buf[MAX_STRING_LENGTH];
        char query[MAX_INPUT_LENGTH];
        int i = 0;
	int done = 0;
	char * rows[MSL];
        int numrows;
	char mes[MSL];
        MYSQL_ROW row;
	char chatlog[MAX_CHAT_ROWS][MAX_STRING_LENGTH];
        
	if (chan > 0 && !name){
   		xprintf(query, "select date, name, message from chatHist where date_sub(curdate(), interval 1 day) <= date and channel='%d' order by date desc limit %d", chan, MAX_CHAT_ROWS );
	} else if ( chan > 0 && name ){
		xprintf(query, "select date, name, message from chatHist where date_sub(curdate(), interval 30 day) <= date and name='%s' and channel='%d'", name, chan);
	} else {
		send_to_char("something fucked up", ch);
		return;
	}

	sql_connect("localhost", "", "mud", SQLUSER, SQLPASS);
	if (!sql_query(query))
		goto cleanup;

	numrows = sql_num_rows(query);
	if (numrows < 1) {
		send_to_char("No chat history available\n\r", ch);
		goto cleanup;
	}

	send_to_char("#R------#rChat Hist#R------#n\n\r", ch);
	
	
	for (i = 0; i < numrows; i++) {
		row = sql_fetch_row(query);
		xprintf(chatlog[i], "[%s] %s: %s ID=%d\n\r", row[0], row[1], row[2], i);
	}		

	for (i = numrows; i >= 0; i--) {
		send_to_char(chatlog[i], ch);
	}

	cleanup:
	sql_disconnect();

	bail_final:
	return;

}*/







