/*
 * Copyright (C) 2008-2011 TrinityCore <http://www.trinitycore.org/>
 * Copyright (C) 2005-2009 MaNGOS <http://getmangos.com/>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 2 of the License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include "Common.h"
#include "DatabaseEnv.h"
#include "DBCStores.h"
#include "ObjectMgr.h"
#include "Player.h"
#include "Item.h"
#include "GameObject.h"
#include "Opcodes.h"
#include "Chat.h"
#include "MapManager.h"
#include "Language.h"
#include "World.h"
#include "GameEventMgr.h"
#include "SpellMgr.h"
#include "PoolMgr.h"
#include "AccountMgr.h"
#include "WaypointManager.h"
#include "Util.h"
#include <cctype>
#include <iostream>
#include <fstream>
#include <map>
#include "OutdoorPvPMgr.h"
#include "Transport.h"
#include "TargetedMovementGenerator.h"                      // for HandleNpcUnFollowCommand
#include "CreatureGroups.h"
#include <sstream>
#include "ace/INET_Addr.h"

//mute player for some times
bool ChatHandler::HandleMuteCommand(const char* args)
{
    char* nameStr;
    char* delayStr;
    extractOptFirstArg((char*)args, &nameStr, &delayStr);
    if (!delayStr)
        return false;

    char *mutereason = strtok(NULL, "\r");
    std::string mutereasonstr = "No reason";
    if (mutereason != NULL)
         mutereasonstr = mutereason;

    Player* target;
    uint64 target_guid;
    std::string target_name;
    if (!extractPlayerTarget(nameStr, &target, &target_guid, &target_name))
        return false;

    uint32 account_id = target ? target->GetSession()->GetAccountId() : sObjectMgr->GetPlayerAccountIdByGUID(target_guid);

    // find only player from same account if any
    if (!target)
        if (WorldSession* session = sWorld->FindSession(account_id))
            target = session->GetPlayer();

    uint32 notspeaktime = (uint32) atoi(delayStr);

    // must have strong lesser security level
    if (HasLowerSecurity (target, target_guid, true))
        return false;

    time_t mutetime = time(NULL) + notspeaktime*60;

    if (target)
        target->GetSession()->m_muteTime = mutetime;

    LoginDatabase.PExecute("UPDATE account SET mutetime = " UI64FMTD " WHERE id = '%u'", uint64(mutetime), account_id);

    if (target)
        ChatHandler(target).PSendSysMessage(LANG_YOUR_CHAT_DISABLED, notspeaktime, mutereasonstr.c_str());

    std::string nameLink = playerLink(target_name);

    PSendSysMessage(LANG_YOU_DISABLE_CHAT, nameLink.c_str(), notspeaktime, mutereasonstr.c_str());

    return true;
}

//unmute player
bool ChatHandler::HandleUnmuteCommand(const char* args)
{
    Player* target;
    uint64 target_guid;
    std::string target_name;
    if (!extractPlayerTarget((char*)args, &target, &target_guid, &target_name))
        return false;

    uint32 account_id = target ? target->GetSession()->GetAccountId() : sObjectMgr->GetPlayerAccountIdByGUID(target_guid);

    // find only player from same account if any
    if (!target)
        if (WorldSession* session = sWorld->FindSession(account_id))
            target = session->GetPlayer();

    // must have strong lesser security level
    if (HasLowerSecurity (target, target_guid, true))
        return false;

    if (target)
    {
        if (target->CanSpeak())
        {
            SendSysMessage(LANG_CHAT_ALREADY_ENABLED);
            SetSentErrorMessage(true);
            return false;
        }

        target->GetSession()->m_muteTime = 0;
    }

    LoginDatabase.PExecute("UPDATE account SET mutetime = '0' WHERE id = '%u'", account_id);

    if (target)
        ChatHandler(target).PSendSysMessage(LANG_YOUR_CHAT_ENABLED);

    std::string nameLink = playerLink(target_name);

    PSendSysMessage(LANG_YOU_ENABLE_CHAT, nameLink.c_str());
    return true;
}

bool ChatHandler::HandleGUIDCommand(const char* /*args*/)
{
    uint64 guid = m_session->GetPlayer()->GetSelection();

    if (guid == 0)
    {
        SendSysMessage(LANG_NO_SELECTION);
        SetSentErrorMessage(true);
        return false;
    }

    PSendSysMessage(LANG_OBJECT_GUID, GUID_LOPART(guid), GUID_HIPART(guid));
    return true;
}

 //move item to other slot
bool ChatHandler::HandleItemMoveCommand(const char* args)
{
    if (!*args)
        return false;
    uint8 srcslot, dstslot;

    char* pParam1 = strtok((char*)args, " ");
    if (!pParam1)
        return false;

    char* pParam2 = strtok(NULL, " ");
    if (!pParam2)
        return false;

    srcslot = (uint8)atoi(pParam1);
    dstslot = (uint8)atoi(pParam2);

    if (srcslot == dstslot)
        return true;

    if (!m_session->GetPlayer()->IsValidPos(INVENTORY_SLOT_BAG_0, srcslot, true))
        return false;

    if (!m_session->GetPlayer()->IsValidPos(INVENTORY_SLOT_BAG_0, dstslot, false))
        return false;

    uint16 src = ((INVENTORY_SLOT_BAG_0 << 8) | srcslot);
    uint16 dst = ((INVENTORY_SLOT_BAG_0 << 8) | dstslot);

    m_session->GetPlayer()->SwapItem(src, dst);

    return true;
}

//demorph player or unit
bool ChatHandler::HandleDeMorphCommand(const char* /*args*/)
{
    Unit* target = getSelectedUnit();
    if (!target)
        target = m_session->GetPlayer();

    if (target->GetTypeId() == TYPEID_PLAYER)
    {	
		Player* target = getSelectedPlayer();
		if (!m_session->GetPlayer()->CanUseCommandOnPlayer(target))
		{
			PSendSysMessage("You can't use commands on this player!");
			return true;
		}
    }

    target->DeMorph();
    return true;
}

//kick player
bool ChatHandler::HandleKickPlayerCommand(const char *args)
{
    Player* target = NULL;
    std::string playerName;
    if (!extractPlayerTarget((char*)args, &target, NULL, &playerName))
        return false;

    if (m_session && target == m_session->GetPlayer())
    {
        SendSysMessage(LANG_COMMAND_KICKSELF);
        SetSentErrorMessage(true);
        return false;
    }

    // check online security
    if (HasLowerSecurity(target, 0))
        return false;

    if (sWorld->getBoolConfig(CONFIG_SHOW_KICK_IN_WORLD))
        sWorld->SendWorldText(LANG_COMMAND_KICKMESSAGE, playerName.c_str());
    else
        PSendSysMessage(LANG_COMMAND_KICKMESSAGE, playerName.c_str());

    target->GetSession()->KickPlayer();
    return true;
}

//show info of player
bool ChatHandler::HandlePInfoCommand(const char* args)
{

    Player* target; // Prototype for target to allow for std::string pName = target? target->GetName() : args;
    // Added check for m_session to allow using command from CLI. Else m_session->GetPlayer()->GetSelectedPlayer() crashes the server.
    if(m_session)
        {	
        target = m_session->GetPlayer()->GetSelectedPlayer();
	if (!target && !*args)
		return false;
        }
     else
        if (!*args)
            return false;

	std::string pName = target ? target->GetName() : args;
	std::string onlineStatus;
	uint32 accountLevel;

	//												      0		   1     2 
    QueryResult result = CharacterDatabase.PQuery("SELECT account, guid, name FROM characters WHERE name = '%s'", pName.c_str());
    if (!result)
		return false;
    Field* fields = result->Fetch();
	uint32 accID = fields[0].GetUInt32();
	uint32 playerGUID = fields[1].GetUInt64();
	pName = fields[2].GetString();
	sLog->outString("ChatHandler::HandlePInfoCommand(): Character DB query is fine.");

    //												   0		 1		2		 3
	QueryResult result1 = LoginDatabase.PQuery("SELECT username, email, last_ip, last_login FROM account WHERE id = %u", accID);
	if (!result1) 
		return false; 
	Field* accFields = result1->Fetch();
	std::string account = accFields[0].GetString();
	std::string email = accFields[1].GetString();
	std::string lastIP = accFields[2].GetString();
	std::string lastLogin = accFields[3].GetString();
	
	uint32 ip = inet_addr(lastIP.c_str());
	QueryResult ip2nation = WorldDatabase.PQuery("SELECT c.country FROM ip2nationCountries c, ip2nation i WHERE i.ip < %u AND c.code = i.country ORDER BY i.ip DESC LIMIT 0,1", ip);
	if(ip2nation)
	{
		Field* ipnation = ip2nation->Fetch();
		lastIP = lastIP + " (" + ipnation[0].GetString() + ")";
	}
    
    // Return online status
    QueryResult result2 = LoginDatabase.PQuery("SELECT online FROM account WHERE id=%u", accID);
    if (!result2)
        return false;
    Field* fields2 = result2->Fetch();
    if (fields2[0].GetUInt32()==0)
        onlineStatus = "Offline";
    if (fields2[0].GetUInt32()==1)
	{
		QueryResult result2_1 = CharacterDatabase.PQuery("SELECT name FROM characters WHERE account=%u AND online=1", accID);
		if (!result2_1)
		{
			onlineStatus = "Online in Character Screen";
		}
		else
		{
		Field* fields2_1 = result2_1->Fetch();
                // Format string for each console vs. in-game.
                if(m_session)
		    onlineStatus = "Online on |Hplayer:" + fields2_1[0].GetString() + "|h[" + fields2_1[0].GetString() + "]|h";
		else
                    onlineStatus = "Online on [" + fields2_1[0].GetString() + "]";
                }
	}
        
    // Return GMLevel
        QueryResult result3 = LoginDatabase.PQuery("SELECT gmlevel FROM account_access WHERE id=%u", accID);
        if (!result3)
            accountLevel=0;
        else
        {
        Field* fields3 = result3->Fetch();
            accountLevel = fields3[0].GetUInt32();
        }

	//Return Vote points and number of different IPs
		QueryResult votes = LoginDatabase.PQuery("SELECT votes, count(distinct ip) FROM auth.account INNER JOIN vote.votelog ON auth.account.username = vote.votelog.user WHERE auth.account.id=%u", accID);
		uint32 numVotes;
		uint32 numIP;
		if (!votes)
			numIP = numVotes = 0;
		Field *votePoint = votes->Fetch();
		numVotes = votePoint[0].GetUInt32();
		numIP = votePoint[1].GetUInt32();

	//Get play time
	//Define time constants
	const int YEAR_SEC = 31536000;
	const int MONTH_SEC = 2592000;
	const int DAY_SEC = 86400;
	const int HOUR_SEC = 3600;
	const int MIN_SEC = 60;

	// Fetch time played in seconds
	QueryResult played = CharacterDatabase.PQuery("SELECT sum(totaltime) FROM characters WHERE account=%u", accID);
	if(!played)
		return false;
	Field *playtime = played->Fetch();
	int time_played = playtime[0].GetInt();

	//convert time from seconds to string
    int temp;
    int years, days, hours, minutes;
    std::stringstream ss;
    
    years = time_played / YEAR_SEC;
    temp = time_played % YEAR_SEC;
    
    days = temp / DAY_SEC;
    temp = temp % DAY_SEC;
    
    hours = temp / HOUR_SEC;
    temp = temp % HOUR_SEC;
    
    minutes = temp / MIN_SEC;
    temp = temp % MIN_SEC;

    
	// Only including time if needed using correct plurality (is that a word?)
    if(years != 0)
    {
        ss << years << " year";
        if(years > 1)
            ss << "s";
        ss << " ";
    }
    
    if(days != 0)
    {
        ss << days << " day";
        if(days > 1)
            ss << "s";
        ss << " ";
    }
    
    if(hours != 0)
    {
        ss << hours << " hour";
        if(hours > 1)
            ss << "s";
        ss << " ";
    }
    
    if(minutes != 0)
    {
        ss << minutes << " minute";
        if(minutes > 1)
            ss << "s";
        ss << " ";
    }
    
    if(temp != 0)
    {
        ss << temp << " second";
        if(temp > 1)
            ss << "s";
    }
    
	// If they haven't played at all.
    if(days == 0 && hours == 0 && minutes == 0 && temp == 0)
        ss << "not played at all";

	// Finalize time string
	std::string pTime = ss.str();
           
	email = email.empty() ? "No Email Specified" : accFields[1].GetString();

    // Format output for console vs. game.
    if(m_session)
    {
	PSendSysMessage("|Hplayer:%s|h[%s]|h (GUID: %u) last logged in at %s, from %s Currently: %s", pName.c_str(), pName.c_str(), playerGUID, lastLogin.c_str(), lastIP.c_str(), onlineStatus.c_str());
	PSendSysMessage("Account: %s (ID: %u) Account Level: %u" , account.c_str(), accID, accountLevel);
	PSendSysMessage("Email: %s", email.c_str());
	PSendSysMessage("Number of votes: %u Number of distinct IPs: %u", numVotes, numIP);
	PSendSysMessage("Time played on account: %s", pTime.c_str());
    }
    else
    {
        PSendSysMessage("[%s] (GUID: %u) last logged in at %s, from %s Currently: %s", pName.c_str(), playerGUID, lastLogin.c_str(), lastIP.c_str(), onlineStatus.c_str());
        PSendSysMessage("Account: %s (ID: %u) Account Level: %u" , account.c_str(), accID, accountLevel);
        PSendSysMessage("Email: %s", email.c_str());
        PSendSysMessage("Number of votes: %u Number of distinct IPs: %u", numVotes, numIP);
        PSendSysMessage("Time played on account: %s", pTime.c_str());
    }

    return true;
}

//rename characters
bool ChatHandler::HandleCharacterRenameCommand(const char* args)
{
    Player* target;
    uint64 target_guid;
    std::string target_name;
    if (!extractPlayerTarget((char*)args, &target, &target_guid, &target_name))
        return false;

	if (!m_session->GetPlayer()->IsAdmin())
		target = m_session->GetPlayer();

    if (target)
    {
        PSendSysMessage(LANG_RENAME_PLAYER, GetNameLink(target).c_str());
        target->SetAtLoginFlag(AT_LOGIN_RENAME);
    }
    else
    {
        // check offline security
        if (HasLowerSecurity(NULL, target_guid))
            return false;

        std::string oldNameLink = playerLink(target_name);

        PSendSysMessage(LANG_RENAME_PLAYER_GUID, oldNameLink.c_str(), GUID_LOPART(target_guid));
        CharacterDatabase.PExecute("UPDATE characters SET at_login = at_login | '1' WHERE guid = '%u'", GUID_LOPART(target_guid));
    }

    return true;
}

// customize characters
bool ChatHandler::HandleCharacterCustomizeCommand(const char* args)
{
    Player* target;
    uint64 target_guid;
    std::string target_name;
    if (!extractPlayerTarget((char*)args, &target, &target_guid, &target_name))
        return false;

    if (target)
    {
        if (!m_session->GetPlayer()->CanUseCommandOnPlayer(target))
		{
			PSendSysMessage("You can't use commands on this player!");
			return true;
		}
		
		PSendSysMessage(LANG_CUSTOMIZE_PLAYER, GetNameLink(target).c_str());
        target->SetAtLoginFlag(AT_LOGIN_CUSTOMIZE);
        CharacterDatabase.PExecute("UPDATE characters SET at_login = at_login | '8' WHERE guid = '%u'", target->GetGUIDLow());
    }
    else
    {
        std::string oldNameLink = playerLink(target_name);

        PSendSysMessage(LANG_CUSTOMIZE_PLAYER_GUID, oldNameLink.c_str(), GUID_LOPART(target_guid));
        CharacterDatabase.PExecute("UPDATE characters SET at_login = at_login | '8' WHERE guid = '%u'", GUID_LOPART(target_guid));
    }

    return true;
}

bool ChatHandler::HandleCharacterChangeFactionCommand(const char * args)
{
    Player* target;
    uint64 target_guid;
    std::string target_name;

    if (!extractPlayerTarget((char*)args, &target, &target_guid, &target_name))
        return false;

    if (target)
    {
        if (!m_session->GetPlayer()->CanUseCommandOnPlayer(target))
		{
			PSendSysMessage("You can't use commands on this player!");
			return true;
		}
		
		// TODO : add text into database
        PSendSysMessage(LANG_CUSTOMIZE_PLAYER, GetNameLink(target).c_str());
        target->SetAtLoginFlag(AT_LOGIN_CHANGE_FACTION);
        CharacterDatabase.PExecute("UPDATE characters SET at_login = at_login | '64' WHERE guid = %u", target->GetGUIDLow());
    }
    else
    {
        std::string oldNameLink = playerLink(target_name);

        // TODO : add text into database
        PSendSysMessage(LANG_CUSTOMIZE_PLAYER_GUID, oldNameLink.c_str(), GUID_LOPART(target_guid));
        CharacterDatabase.PExecute("UPDATE characters SET at_login = at_login | '64' WHERE guid = %u", GUID_LOPART(target_guid));
    }

    return true;
}

bool ChatHandler::HandleCharacterChangeRaceCommand(const char * args)
{
    Player* target;
    uint64 target_guid;
    std::string target_name;
    if (!extractPlayerTarget((char*)args, &target, &target_guid, &target_name))
        return false;

    if (target)
    {
		if (!m_session->GetPlayer()->CanUseCommandOnPlayer(target))
		{
			PSendSysMessage("You can't use commands on this player!");
			return true;
		}

        // TODO : add text into database
        PSendSysMessage(LANG_CUSTOMIZE_PLAYER, GetNameLink(target).c_str());
        target->SetAtLoginFlag(AT_LOGIN_CHANGE_RACE);
        CharacterDatabase.PExecute("UPDATE characters SET at_login = at_login | '128' WHERE guid = %u", target->GetGUIDLow());
    }
    else
    {
        std::string oldNameLink = playerLink(target_name);

        // TODO : add text into database
        PSendSysMessage(LANG_CUSTOMIZE_PLAYER_GUID, oldNameLink.c_str(), GUID_LOPART(target_guid));
        CharacterDatabase.PExecute("UPDATE characters SET at_login = at_login | '128' WHERE guid = %u", GUID_LOPART(target_guid));
    }

    return true;
}

bool ChatHandler::HandleCharacterReputationCommand(const char* args)
{
    Player* target;
    if (!extractPlayerTarget((char*)args, &target))
        return false;

	if (!m_session->GetPlayer()->CanUseCommandOnPlayer(target))
	{
		PSendSysMessage("You can't use commands on this player!");
		return true;
	}

    LocaleConstant loc = GetSessionDbcLocale();

    FactionStateList const& targetFSL = target->GetReputationMgr().GetStateList();
    for (FactionStateList::const_iterator itr = targetFSL.begin(); itr != targetFSL.end(); ++itr)
    {
        FactionEntry const *factionEntry = sFactionStore.LookupEntry(itr->second.ID);
        char const* factionName = factionEntry ? factionEntry->name[loc] : "#Not found#";
        ReputationRank rank = target->GetReputationMgr().GetRank(factionEntry);
        std::string rankName = GetTrinityString(ReputationRankStrIndex[rank]);
        std::ostringstream ss;
        if (m_session)
            ss << itr->second.ID << " - |cffffffff|Hfaction:" << itr->second.ID << "|h[" << factionName << " " << localeNames[loc] << "]|h|r";
        else
            ss << itr->second.ID << " - " << factionName << " " << localeNames[loc];

        ss << " " << rankName << " (" << target->GetReputationMgr().GetReputation(factionEntry) << ")";

        if (itr->second.Flags & FACTION_FLAG_VISIBLE)
            ss << GetTrinityString(LANG_FACTION_VISIBLE);
        if (itr->second.Flags & FACTION_FLAG_AT_WAR)
            ss << GetTrinityString(LANG_FACTION_ATWAR);
        if (itr->second.Flags & FACTION_FLAG_PEACE_FORCED)
            ss << GetTrinityString(LANG_FACTION_PEACE_FORCED);
        if (itr->second.Flags & FACTION_FLAG_HIDDEN)
            ss << GetTrinityString(LANG_FACTION_HIDDEN);
        if (itr->second.Flags & FACTION_FLAG_INVISIBLE_FORCED)
            ss << GetTrinityString(LANG_FACTION_INVISIBLE_FORCED);
        if (itr->second.Flags & FACTION_FLAG_INACTIVE)
            ss << GetTrinityString(LANG_FACTION_INACTIVE);

        SendSysMessage(ss.str().c_str());
    }
    return true;
}

bool ChatHandler::HandleLookupEventCommand(const char* args)
{
    if (!*args)
        return false;

    std::string namepart = args;
    std::wstring wnamepart;

    // converting string that we try to find to lower case
    if (!Utf8toWStr(namepart, wnamepart))
        return false;

    wstrToLower(wnamepart);

    bool found = false;
    uint32 count = 0;
    uint32 maxResults = sWorld->getIntConfig(CONFIG_MAX_RESULTS_LOOKUP_COMMANDS);

    GameEventMgr::GameEventDataMap const& events = sGameEventMgr->GetEventMap();
    GameEventMgr::ActiveEvents const& activeEvents = sGameEventMgr->GetActiveEventList();

    for (uint32 id = 0; id < events.size(); ++id)
    {
        GameEventData const& eventData = events[id];

        std::string descr = eventData.description;
        if (descr.empty())
            continue;

        if (Utf8FitTo(descr, wnamepart))
        {
            if (maxResults && count++ == maxResults)
            {
                PSendSysMessage(LANG_COMMAND_LOOKUP_MAX_RESULTS, maxResults);
                return true;
            }

            char const* active = activeEvents.find(id) != activeEvents.end() ? GetTrinityString(LANG_ACTIVE) : "";

            if (m_session)
                PSendSysMessage(LANG_EVENT_ENTRY_LIST_CHAT, id, id, eventData.description.c_str(), active);
            else
                PSendSysMessage(LANG_EVENT_ENTRY_LIST_CONSOLE, id, eventData.description.c_str(), active);

            if (!found)
                found = true;
        }
    }

    if (!found)
        SendSysMessage(LANG_NOEVENTFOUND);

    return true;
}

bool ChatHandler::HandleCombatStopCommand(const char* args)
{
    Player* target;
    if (!extractPlayerTarget((char*)args, &target))
        return false;

	if (!m_session->GetPlayer()->CanUseCommandOnPlayer(target))
	{
		PSendSysMessage("You can't use commands on this player!");
		return true;
	}

    // check online security
    if (HasLowerSecurity(target, 0))
        return false;

    target->CombatStop();
    target->getHostileRefManager().deleteReferences();
    return true;
}

bool ChatHandler::HandleLookupPlayerIpCommand(const char* args)
{

    if (!*args)
        return false;

    std::string ip = strtok ((char*)args, " ");
    char* limit_str = strtok (NULL, " ");
    int32 limit = limit_str ? atoi (limit_str) : -1;

    LoginDatabase.EscapeString (ip);

    QueryResult result = LoginDatabase.PQuery ("SELECT id, username FROM account WHERE last_ip = '%s'", ip.c_str ());

    return LookupPlayerSearchCommand (result, limit);
}

bool ChatHandler::HandleLookupPlayerAccountCommand(const char* args)
{
    if (!*args)
        return false;

    std::string account = strtok ((char*)args, " ");
    char* limit_str = strtok (NULL, " ");
    int32 limit = limit_str ? atoi (limit_str) : -1;

    if (!AccountMgr::normalizeString (account))
        return false;

    LoginDatabase.EscapeString (account);

    QueryResult result = LoginDatabase.PQuery ("SELECT id, username FROM account WHERE username = '%s'", account.c_str ());

    return LookupPlayerSearchCommand (result, limit);
}

bool ChatHandler::HandleLookupPlayerEmailCommand(const char* args)
{

    if (!*args)
        return false;

    std::string email = strtok ((char*)args, " ");
    char* limit_str = strtok (NULL, " ");
    int32 limit = limit_str ? atoi (limit_str) : -1;

    LoginDatabase.EscapeString (email);

    QueryResult result = LoginDatabase.PQuery ("SELECT id, username FROM account WHERE email = '%s'", email.c_str ());

    return LookupPlayerSearchCommand (result, limit);
}

bool ChatHandler::LookupPlayerSearchCommand(QueryResult result, int32 limit)
{
    if (!result)
    {
        PSendSysMessage(LANG_NO_PLAYERS_FOUND);
        SetSentErrorMessage(true);
        return false;
    }

    int i = 0;
    uint32 count = 0;
    uint32 maxResults = sWorld->getIntConfig(CONFIG_MAX_RESULTS_LOOKUP_COMMANDS);
    do
    {
        if (maxResults && count++ == maxResults)
        {
            PSendSysMessage(LANG_COMMAND_LOOKUP_MAX_RESULTS, maxResults);
            return true;
        }

        Field* fields = result->Fetch();
        uint32 acc_id = fields[0].GetUInt32();
        std::string acc_name = fields[1].GetString();

        QueryResult chars = CharacterDatabase.PQuery("SELECT guid, name FROM characters WHERE account = '%u'", acc_id);
        if (chars)
        {
            PSendSysMessage(LANG_LOOKUP_PLAYER_ACCOUNT, acc_name.c_str(), acc_id);

            uint64 guid = 0;
            std::string name;

            do
            {
                Field* charfields = chars->Fetch();
                guid = charfields[0].GetUInt64();
                name = charfields[1].GetString();

                PSendSysMessage(LANG_LOOKUP_PLAYER_CHARACTER, name.c_str(), guid);
                ++i;

            } while (chars->NextRow() && (limit == -1 || i < limit));
        }
    } while (result->NextRow());

    if (i == 0)                                                // empty accounts only
    {
        PSendSysMessage(LANG_NO_PLAYERS_FOUND);
        SetSentErrorMessage(true);
        return false;
    }

    return true;
}

/// Triggering corpses expire check in world
bool ChatHandler::HandleServerCorpsesCommand(const char* /*args*/)
{
    sObjectAccessor->RemoveOldCorpses();
    return true;
}

bool ChatHandler::HandleRepairitemsCommand(const char* args)
{
    Player* target;
    if (!extractPlayerTarget((char*)args, &target))
        return false;

	if (!m_session->GetPlayer()->CanUseCommandOnPlayer(target))
	{
		PSendSysMessage("You can't use commands on this player!");
		return true;
	}

    // check online security
    if (HasLowerSecurity(target, 0))
        return false;

    // Repair items
    target->DurabilityRepairAll(false, 0, false);

    PSendSysMessage(LANG_YOU_REPAIR_ITEMS, GetNameLink(target).c_str());
    if (needReportToTarget(target))
        ChatHandler(target).PSendSysMessage(LANG_YOUR_ITEMS_REPAIRED, GetNameLink().c_str());
    return true;
}

bool ChatHandler::HandleWaterwalkCommand(const char* args)
{
    if (!*args)
        return false;

    Player* player = getSelectedPlayer();

    if (!player)
    {
        PSendSysMessage(LANG_NO_CHAR_SELECTED);
        SetSentErrorMessage(true);
        return false;
    }

	if (!m_session->GetPlayer()->CanUseCommandOnPlayer(player))
	{
		PSendSysMessage("You can't use commands on this player!");
		return true;
	}

    if (strncmp(args, "on", 3) == 0)
        player->SetMovement(MOVE_WATER_WALK);               // ON
    else if (strncmp(args, "off", 4) == 0)
        player->SetMovement(MOVE_LAND_WALK);                // OFF
    else
    {
        SendSysMessage(LANG_USE_BOL);
        return false;
    }

    PSendSysMessage(LANG_YOU_SET_WATERWALK, args, GetNameLink(player).c_str());
    if (needReportToTarget(player))
        ChatHandler(player).PSendSysMessage(LANG_YOUR_WATERWALK_SET, args, GetNameLink().c_str());
    return true;
}

bool ChatHandler::HandleCreatePetCommand(const char* /*args*/)
{
    Player* player = m_session->GetPlayer();
    Creature *creatureTarget = getSelectedCreature();

    if (!creatureTarget || creatureTarget->isPet() || creatureTarget->GetTypeId() == TYPEID_PLAYER)
    {
        PSendSysMessage(LANG_SELECT_CREATURE);
        SetSentErrorMessage(true);
        return false;
    }

    CreatureTemplate const* cInfo = sObjectMgr->GetCreatureTemplate(creatureTarget->GetEntry());
    // Creatures with family 0 crashes the server
    if (cInfo->family == 0)
    {
        PSendSysMessage("This creature cannot be tamed. (family id: 0).");
        SetSentErrorMessage(true);
        return false;
    }

    if (player->GetPetGUID())
    {
        PSendSysMessage("You already have a pet");
        SetSentErrorMessage(true);
        return false;
    }

    // Everything looks OK, create new pet
    Pet* pet = new Pet(player, HUNTER_PET);

    if (!pet)
      return false;

    if (!pet->CreateBaseAtCreature(creatureTarget))
    {
        delete pet;
        PSendSysMessage("Error 1");
        return false;
    }

    creatureTarget->setDeathState(JUST_DIED);
    creatureTarget->RemoveCorpse();
    creatureTarget->SetHealth(0); // just for nice GM-mode view

    pet->SetUInt64Value(UNIT_FIELD_CREATEDBY, player->GetGUID());
    pet->SetUInt32Value(UNIT_FIELD_FACTIONTEMPLATE, player->getFaction());

    if (!pet->InitStatsForLevel(creatureTarget->getLevel()))
    {
        sLog->outError("InitStatsForLevel() in EffectTameCreature failed! Pet deleted.");
        PSendSysMessage("Error 2");
        delete pet;
        return false;
    }

    // prepare visual effect for levelup
    pet->SetUInt32Value(UNIT_FIELD_LEVEL, creatureTarget->getLevel()-1);

    pet->GetCharmInfo()->SetPetNumber(sObjectMgr->GeneratePetNumber(), true);
    // this enables pet details window (Shift+P)
    pet->InitPetCreateSpells();
    pet->SetFullHealth();

    pet->GetMap()->Add(pet->ToCreature());

    // visual effect for levelup
    pet->SetUInt32Value(UNIT_FIELD_LEVEL, creatureTarget->getLevel());

    player->SetMinion(pet, true);
    pet->SavePetToDB(PET_SAVE_AS_CURRENT);
    player->PetSpellInitialize();

    return true;
}

bool ChatHandler::HandlePetLearnCommand(const char* args)
{
    if (!*args)
        return false;

    Player *plr = m_session->GetPlayer();
    Pet *pet = plr->GetPet();

    if (!pet)
    {
        PSendSysMessage("You have no pet");
        SetSentErrorMessage(true);
        return false;
    }

    uint32 spellId = extractSpellIdFromLink((char*)args);

    if (!spellId || !sSpellStore.LookupEntry(spellId))
        return false;

    // Check if pet already has it
    if (pet->HasSpell(spellId))
    {
        PSendSysMessage("Pet already has spell: %u", spellId);
        SetSentErrorMessage(true);
        return false;
    }

    // Check if spell is valid
    SpellEntry const* spellInfo = sSpellStore.LookupEntry(spellId);
    if (!spellInfo || !SpellMgr::IsSpellValid(spellInfo))
    {
        PSendSysMessage(LANG_COMMAND_SPELL_BROKEN, spellId);
        SetSentErrorMessage(true);
        return false;
    }

    pet->learnSpell(spellId);

    PSendSysMessage("Pet has learned spell %u", spellId);
    return true;
}

bool ChatHandler::HandlePetUnlearnCommand(const char *args)
{
    if (!*args)
        return false;

    Player *plr = m_session->GetPlayer();
    Pet *pet = plr->GetPet();

    if (!pet)
    {
        PSendSysMessage("You have no pet");
        SetSentErrorMessage(true);
        return false;
    }

    uint32 spellId = extractSpellIdFromLink((char*)args);

    if (pet->HasSpell(spellId))
        pet->removeSpell(spellId, false);
    else
        PSendSysMessage("Pet doesn't have that spell");

    return true;
}

bool ChatHandler::HandleLookupTitleCommand(const char* args)
{
    if (!*args)
        return false;

    // can be NULL in console call
    Player* target = getSelectedPlayer();

    // title name have single string arg for player name
    char const* targetName = target ? target->GetName() : "NAME";

    std::string namepart = args;
    std::wstring wnamepart;

    if (!Utf8toWStr(namepart, wnamepart))
        return false;

    // converting string that we try to find to lower case
    wstrToLower(wnamepart);

    uint32 counter = 0;                                     // Counter for figure out that we found smth.
    uint32 maxResults = sWorld->getIntConfig(CONFIG_MAX_RESULTS_LOOKUP_COMMANDS);

    // Search in CharTitles.dbc
    for (uint32 id = 0; id < sCharTitlesStore.GetNumRows(); id++)
    {
        CharTitlesEntry const *titleInfo = sCharTitlesStore.LookupEntry(id);
        if (titleInfo)
        {
            int loc = GetSessionDbcLocale();
            std::string name = titleInfo->name[loc];
            if (name.empty())
                continue;

            if (!Utf8FitTo(name, wnamepart))
            {
                loc = 0;
                for (; loc < TOTAL_LOCALES; ++loc)
                {
                    if (loc == GetSessionDbcLocale())
                        continue;

                    name = titleInfo->name[loc];
                    if (name.empty())
                        continue;

                    if (Utf8FitTo(name, wnamepart))
                        break;
                }
            }

            if (loc < TOTAL_LOCALES)
            {
                if (maxResults && counter == maxResults)
                {
                    PSendSysMessage(LANG_COMMAND_LOOKUP_MAX_RESULTS, maxResults);
                    return true;
                }

                char const* knownStr = target && target->HasTitle(titleInfo) ? GetTrinityString(LANG_KNOWN) : "";

                char const* activeStr = target && target->GetUInt32Value(PLAYER_CHOSEN_TITLE) == titleInfo->bit_index
                    ? GetTrinityString(LANG_ACTIVE)
                    : "";

                char titleNameStr[80];
                snprintf(titleNameStr, 80, name.c_str(), targetName);

                // send title in "id (idx:idx) - [namedlink locale]" format
                if (m_session)
                    PSendSysMessage(LANG_TITLE_LIST_CHAT, id, titleInfo->bit_index, id, titleNameStr, localeNames[loc], knownStr, activeStr);
                else
                    PSendSysMessage(LANG_TITLE_LIST_CONSOLE, id, titleInfo->bit_index, titleNameStr, localeNames[loc], knownStr, activeStr);

                ++counter;
            }
        }
    }
    if (counter == 0)                                       // if counter == 0 then we found nth
        SendSysMessage(LANG_COMMAND_NOTITLEFOUND);
    return true;
}

bool ChatHandler::HandleCharacterTitlesCommand(const char* args)
{
    if (!*args)
        return false;

    Player* target;
    if (!extractPlayerTarget((char*)args, &target))
        return false;

	if (!m_session->GetPlayer()->CanUseCommandOnPlayer(target))
	{
		PSendSysMessage("You can't use commands on this player!");
		return true;
	}

    LocaleConstant loc = GetSessionDbcLocale();
    char const* targetName = target->GetName();
    char const* knownStr = GetTrinityString(LANG_KNOWN);

    // Search in CharTitles.dbc
    for (uint32 id = 0; id < sCharTitlesStore.GetNumRows(); id++)
    {
        CharTitlesEntry const *titleInfo = sCharTitlesStore.LookupEntry(id);
        if (titleInfo && target->HasTitle(titleInfo))
        {
            std::string name = titleInfo->name[loc];
            if (name.empty())
                continue;

            char const* activeStr = target && target->GetUInt32Value(PLAYER_CHOSEN_TITLE) == titleInfo->bit_index
                ? GetTrinityString(LANG_ACTIVE)
                : "";

            char titleNameStr[80];
            snprintf(titleNameStr, 80, name.c_str(), targetName);

            // send title in "id (idx:idx) - [namedlink locale]" format
            if (m_session)
                PSendSysMessage(LANG_TITLE_LIST_CHAT, id, titleInfo->bit_index, id, titleNameStr, localeNames[loc], knownStr, activeStr);
            else
                PSendSysMessage(LANG_TITLE_LIST_CONSOLE, id, titleInfo->bit_index, name.c_str(), localeNames[loc], knownStr, activeStr);
        }
    }
    return true;
}
