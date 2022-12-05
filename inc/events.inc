|***
 * entropy
 * events.inc
 * all shared events
 *
 * Most of this is commented on purpose. Verant/Soe/DB are all well known for using the same emotes in more then one expansion.
 * if you want to use something in particular, just uncomment the related section and restart the mac
 * this file is not updated very often, so your changes should be relatively safe
 *
 ***|



|***
 * note: many events require movement to a mob to echo a response.
 * use:
 ***|
sub Event_SPEAKLINE(string _target, string _verbage)
  /if (!${maEntropy.Find[swEventsInc].Value}) {
    /return FALSE
  }

  /if (${Spawn[${_target}].Distance} > 50) /echo I need to Move Closer To ${_target}

  /invoke ${target[clear]}
  /stick off

  /target ${_target}
  /delay 1s ${Target.DisplayName.Equal[${_target}]}

  /navto "${_target}"
  /delay 10s !${Navigation.Active}
  /delay 1s

  /say ${_verbage}

/return



|***
 * note: advanced pulling summon correction
 * use : auto
 ***|

#event AP_IN_PROGRESS               "You are stunned!"
#event AP_IN_PROGRESS               "You have been summoned!"
sub Event_AP_IN_PROGRESS(string _line)
  /if (!AUTO) /return
  /if (HOME && PULL) {
    | /if (${Me.Stunned}) {
    |   /delay 3s !${Me.Stunned}
    | }
    /if (${Math.Distance[${Me.Y},${Me.X},${Me.Z}:${maData.Find[stCampY].Value},${maData.Find[stCampX].Value},${maData.Find[stCampZ].Value}]} < ${maHome.Find[stHomeVariance].Value}) /return
    /nav locyxz ${maData.Find[stCampY].Value} ${maData.Find[stCampX].Value} ${maData.Find[stCampZ].Value} |dist=${maHome.Find[stHomeVariance].Value} log=off
    | /delay 5s !${Me.Moving}
  }

/return


| rez accept marker clear
#event REZ_ACCEPT                 "You regain some experience from resurrection#.#"
sub REZ_ACCEPT(string _line, bool _debug)
  EDEBUG REZ_ACCEPT
  /varset swIsMeDead FALSE
/return

#event BIND_RETURN                "Returning to Bind Location. Please Wait#.#"




|***
 * note: cast return events
 * use: not for you
 ***|
#event CAST_BEGIN                  "You begin casting#*#"
#event CAST_BEGIN                  "You begin singing#*#"
#event CAST_BEGIN                  "Your #1# begins to glow.#*#"
#event CAST_BEGIN                  "Your summon#*#"
#event CAST_BEGIN                  "Your ability has failed. Timer has been reset#.#"
sub Event_CAST_BEGIN(string _line, bool _debug)
  EDEBUG SUCCESS
  /invoke ${maData.Add[stCastReturn,SUCCESS]}
/return


| #event CAST_BLOCKED            "Your ability has failed. Timer has been reset."
| sub Event_CAST_BLOCKED(string _line, bool _debug)
|   EDEBUG BLOCKED
|   /invoke ${maData.Add[stCastReturn,BLOCKED]}
| /return

#event CAST_OVERWRITTEN            "Your#*#has been overwritten#*#"
sub Event_CAST_OVERWRITTEN(string _line, bool _debug)
  EDEBUG OVERWRITTEN
  /invoke ${maData.Add[stCastReturn,OVERWRITTEN]}
/return

#event CAST_CANNOTSEE              "You cannot see your target#*#"
sub Event_CAST_CANNOTSEE(string _line, bool _debug)
  EDEBUG CANNOTSEE
  /invoke ${maData.Add[stCastReturn,CANNOTSEE]}
/return

#event CAST_COLLAPSE               "Your gate is too unstable, and collapses#*#"
sub Event_CAST_COLLAPSE(string _line, bool _debug)
  EDEBUG COLLAPSE
  /invoke ${maData.Add[stCastReturn,COLLAPSE]}
/return

#event CAST_COMPONENTS             "You are missing some required components#*#"
#event CAST_COMPONENTS             "Your ability to use this item has been disabled because you do not have at least a gold membership#*#"
sub Event_CAST_COMPONENTS(string _line, bool _debug)
  EDEBUG COMPONENT
  /invoke ${maData.Add[stCastReturn,COMPONENT]}
/return

#event CAST_DISTRACTED             "You need to play a#*#instrument for this song#*#"
#event CAST_DISTRACTED             "You are too distracted to cast a spell now#*#"
#event CAST_DISTRACTED             "You can't cast spells while invulnerable#*#"
#event CAST_DISTRACTED             "You *CANNOT* cast spells, you have been silenced#*#"
#event CAST_DISTRACTED             "You do not have sufficient focus to maintain that ability."
sub Event_CAST_DISTRACTED(string _line, bool _debug)
  EDEBUG DISTRACTED
  /invoke ${maData.Add[stCastReturn,DISTRACTED]}
/return

#event CAST_FDFAIL                 "#1# has fallen to the ground.#*#"
sub Event_CAST_FDFAIL(string _line, string _name, bool _debug)
  EDEBUG FDFAIL
  /invoke ${maData.Add[stCastReturn,FDFAIL]}
/return

#event CAST_FIZZLE                 "Your spell fizzles#*#"
#event CAST_FIZZLE                 "You miss a note, bringing your song to a close#*#"
sub Event_CAST_FIZZLE(string _line, bool _debug)
  EDEBUG FIZZLE
  /invoke ${maData.Add[stCastReturn,FIZZLE]}
/return

#event CAST_BLOCKED               "Your #*# did not take hold on #*#. (Blocked by #*#.)"
sub Event_CAST_BLOCKED(string _line, bool _debug)
  EDEBUG BLOCKED
  /invoke ${maData.Add[stCastReturn,BLOCKED]}
/return

#event CAST_IMMUNE                 "Your target has no mana to affect#*#"
#event CAST_IMMUNE                 "Your target is immune to changes in its attack speed#*#"
#event CAST_IMMUNE                 "Your target is immune to changes in its run speed#*#"
#event CAST_IMMUNE                 "Your target is immune to snare spells#*#"
#event CAST_IMMUNE                 "Your target is immune to the stun portion of this effect#*#"
#event CAST_IMMUNE                 "Your target cannot be mesmerized#*#"
#event CAST_IMMUNE                 "Your target looks unaffected#*#"
sub Event_CAST_IMMUNE(string _line, bool _debug)
  EDEBUG IMMUNE
  /invoke ${maData.Add[stCastReturn,IMMUNE]}
/return

#event CAST_INTERRUPTED            "Your spell is interrupted#*#"
#event CAST_INTERRUPTED            "Your #*# spell is interrupted#*#"
#event CAST_INTERRUPTED            "Your casting has been interrupted#*#"
sub Event_CAST_INTERRUPTED(string _line, bool _debug)
  EDEBUG INTERRUPTED
  /invoke ${maData.Add[stCastReturn,INTERRUPTED]}
/return

#event CAST_NOMOUNT                "You can only summon a mount on dry land#*#"
#event CAST_NOMOUNT                "You need to be in a more open area to summon a mount#*#"
#event CAST_NOMOUNT                "You can not summon a mount here#*#"
#event CAST_NOMOUNT                "You must have both the Horse Models and your current Luclin Character Model enabled to summon a mount#*#"
sub Event_CAST_NOMOUNT(string _line, bool _debug)
  EDEBUG NOMOUNT
  /invoke ${maData.Add[stCastReturn,NOMOUNT]}
/return

#event CAST_NOTARGET               "You must first select a target for this spell#*#"
#event CAST_NOTARGET               "This spell only works on#*#"
#event CAST_NOTARGET               "You must first target a group member#*#"
sub Event_CAST_NOTARGET(string _line, bool _debug)
  EDEBUG NOTARGET
  /invoke ${maData.Add[stCastReturn,NOTARGET]}
/return

#event CAST_NOTREADY               "Spell recast time not yet met#*#"
sub Event_CAST_NOTREADY(string _line, bool _debug)
  EDEBUG NOTREADY
  /invoke ${maData.Add[stCastReturn,NOTREADY]}
/return

#event CAST_OUTDOORS               "This spell does not work here#*#"
#event CAST_OUTDOORS               "You can only cast this spell in the outdoors#*#"
sub Event_CAST_OUTDOORS(string _line, bool _debug)
  EDEBUG NOTOUTDOORS
  /invoke ${maData.Add[stCastReturn,NOTOUTDOORS]}
/return

#event CAST_OUTOFMANA              "Insufficient Mana to cast this spell#*#"
sub Event_CAST_OUTOFMANA(string _line, bool _debug)
  EDEBUG OUTOFMANA
  /invoke ${maData.Add[stCastReturn,OUTOFMANA]}
/return

#event CAST_OUTOFRANGE             "Your target is out of range, get closer#*#"
sub Event_CAST_OUTOFRANGE(string _line, bool _debug)
  EDEBUG OUTOFRANGE
  /invoke ${maData.Add[stCastReturn,OUTOFRANGE]}
/return

#event CAST_RECOVER                "You haven't recovered yet#*#"
#event CAST_RECOVER                "Spell recovery time not yet met#*#"
sub Event_CAST_RECOVER(string _line, bool _debug)
  EDEBUG RECOVER
  /invoke ${maData.Add[stCastReturn,RECOVER]}
/return

#event CAST_RESISTED               "Your target resisted the #1# spell#*#"
#event CAST_RESISTED               "#*# resisted your #1#!"
sub Event_CAST_RESISTED(string _line, string _name, bool _debug)
  EDEBUG RESISTED
  /invoke ${maData.Add[stCastReturn,RESISTED]}
/return

#event CAST_RESISTEDYOU            "You resist the #1# spell#*#"
#event CAST_RESISTEDYOU            "You resist #*#"
sub Event_CAST_RESISTEDYOU(string _line, bool _debug)
  EDEBUG RESISTEDYOU
  /invoke ${maData.Add[stCastReturn,RESISTEDYOU]}
/return

#event CAST_STANDING               "You must be standing to cast a spell#*#"
sub Event_CAST_STANDING(string _line, bool _debug)
  EDEBUG RESTART
  /invoke ${maData.Add[stCastReturn,RESTART]}
/return

#event CAST_STUNNED                "You can't cast spells while stunned#*#"
#event CAST_STUNNED                "You are stunned#*#"
sub Event_CAST_STUNNED(string _line, bool _debug)
  EDEBUG STUNNED
  /invoke ${maData.Add[stCastReturn,STUNNED]}
/return

#event CAST_TAKEHOLD               "Your spell did not take hold#*#"
#event CAST_TAKEHOLD               "Your spell would not have taken hold#*#"
#event CAST_TAKEHOLD               "Your spell is too powerfull for your intended target#*#"
| #event CAST_TAKEHOLD               "This pet may not be made invisible#*#"
sub Event_CAST_TAKEHOLD(string _line, bool _debug)
  EDEBUG TAKEHOLD
  /invoke ${maData.Add[stCastReturn,TAKEHOLD]}
/return

#event CAST_REZFAIL               "You were unable to restore the corpse to life, but you may have success with a later attempt#*#"
sub Event_CAST_REZFAIL(string _line, bool _debug)
  EDEBUG REZFAIL
  /invoke ${maData.Add[stCastReturn,REZFAIL]}
/return

#event CAST_RESET               "Your ability failed. Timer has been reset#*#"
sub Event_CAST_RESET(string _line, bool _debug)
  EDEBUG RESET
  /invoke ${maData.Add[stCastReturn,RESET]}
/return


|***
 * use: macro ending events
 ***|
| #event End_Macro "You have entered #*#"
sub Event_End_Macro(string _line)

/return



|***
 * use: correct range distance when too close
 ***|
| #event COMBAT_DISTANCE                  "Your Target is too close to use a ranged weapon#*#"
sub Event_COMBAT_DISTANCE(string _line)
  /doevents flush COMBAT_DISTANCE
  /if (PULL || AGRO) {
    /return
  }
  /call set_combat FALSE
/return


|***
 * use: catch when a zone happens
 ***|
#event Zone "You have entered #*#"
| #event Zone "|${Me.DisplayName}| creates a mistic portal."
#event Zone "LOADING, PLEASE WAIT..."
sub Event_Zone(string _line)
  EDEBUG zoning
  /if (${_line.Find[Drunken Monkey]}) {
    /return FALSE
  }
  /call check_mode FALSE RESET
  /call check_zone_id ${maDebug.Find[zone].Value}
  /call Bind_control_home clear

/return FALSE


|***
 * use: end macro on a camp
 ***|
#event Camp "It will take you about 30 seconds to prepare your camp."
sub Event_Camp(string _line)
  EDEBUG camping
  /endmacro
/return


|***
 * note: raid and group invite accept controls
 * use: EVENTS
 ***|
#event Join "#1# invites you to join a #2#."
sub Event_Join(string _line, string _sender, string _groupraid)

  /doevents flush Join
  
  /if (!${is_lsSafeNames[FALSE, ${_sender}]}) {
    /return FALSE
  }

  | group invites
  /if (${_groupraid.Equal[group]}) {
    /if (!${maEnv.Find[swTakeGroupInvite].Value}) {
      /return
    } else /if (${maEnv.Find[swTakeGroupInvite].Value}) {
      OUT Accepting ${_groupraid} invite${sep}\a-w${_sender}\ax
      /invite
    }
  }

  | raid invites
  /if (${_groupraid.Equal[raid]}) {
    /delay 1s
    /if (!${maEnv.Find[swTakeRaidInvite].Value}) {
      OUT Declining ${_groupraid} invite from${sep}\a-w${_sender}\ax
      /raiddecline
      /if (${Window[ConfirmationDialogbox].Child[CD_TextOutPut].Text.Find["join a raid"]}) {
        /nomodkey /notify ConfirmationDialogBox No_Button leftmouseup
      }
    } else /if (${maEnv.Find[swTakeRaidInvite].Value}) {
      OUT Accepting ${_groupraid} invite from${sep}\a-w${_sender}\ax
      /raidaccept
      /if (${Window[ConfirmationDialogbox].Child[CD_TextOutPut].Text.Find["join a raid"]}) {
        /nomodkey /notify ConfirmationDialogBox Yes_Button leftmouseup
      }
    }
  }

/return



|***
 * note: time to live events
 * use: /ttl
 ***|
#event TTLstart "#2# is #1# by YOUR #4# for #3# points of non-melee damage.#*#"
#event TTLstart "#2# has taken #3# damage from your #5#.#*#"
#event TTLstart "You #1# #2# for #3# points of damage.#*#"
#event TTLstart "You #1# #2# for #3# points of #4# damage by #5#.#*#"
| #event TTLstart "You try to #*#, but miss"
#event TTLstart "|${Me.Pet.DisplayName}| #1# #2# for #3# points of damage.#*#"
#event TTLstart "|${Me.Pet.DisplayName}| #1# #2# for #3# points of #4# damage by #5#.#*#"
#event TTLstart "|${Me}|`s pet #1# #2# for #3# points of damage.#*#"
#event TTLstart "|${Me}|`s pet #1# #2# for #3# points of #4# damage by #5#.#*#"
sub Event_TTLstart(string Line, string YHType, string _mob, int YHDamage, string YHSpellDamageType, string YHSpellName)
  /if (${maTTL.Find[swTTL].Value}) {
    /call control_ttl ${maDebug.Find[ttl].Value} start "${_mob.Lower}"
  }
/return


#event TTLend "You have slain #1#!"
#event TTLend "#1# has been slain by #*#"
sub Event_TTLend(string _line, string _mob)
  /if (${maTTL.Find[swTTL].Value}) {
    /call control_ttl ${maDebug.Find[ttl].Value} end "${_mob.Lower}"
  }
/return



|***
 * note: Tells for specific things.. i guess?
 * use: /t uselesstoon IwantThisBuff
 ***|
#event Talk "#1# tells you,#*# '#2#'"
sub Event_Talk(string line, string _sender, string _chatText)

  | tell realys
  /if (${maEntropy.Find[swRelayTell].Value} && !${_sender.Find[Banker]} && !${_sender.Find[${Me}]} && !${_sender.Find[${maTie.Find[stTieToon].Value}]} && !${_sender.Find[${Me.Pet.DisplayName}]} && ${lsGrain.Contains[25]}) {
    | relay to tie toon if its opne of your toons
    /if (${DanNet.Peers[all].Find[${_sender}]} && ${Bool[${maTie.Find[stTieToon].Value}]}) {
      /tell ${maTie.Find[stTieToon].Value} ->[${_sender}] told me::${_chatText}

      | relay to dannet
      /dgtell all RELAY to ${maTie.Find[stTieToon].Value} :: ${_chatText}

      | is beep set?
      /if (${maEntropy.Find[swRelayTellBeep].Value}) {
        /beep
      }
    }
  }


  | Tells for buffs.
  /if (${maBuff.Find[swBuffBeg].Value}) {
    
    /if (!${Spawn[${_sender}].ID} || ${_sender.Equal[${Me.DisplayName}]}) {
      /return FALSE
    }

    /declare _count int local 0
    /for _count 1 to 50

      | buff has no name
      /if (!${Bool[${maBuff.Find[stBuff${_count}].Value}]}) {
        /continue
      | buff is turned off
      } else /if (!${maBuff.Find[swBuff${_count}].Value}) {
        /continue
      } else /if (${maBuff.Find[stBuff${_count}Alias].Value.NotEqual[${_chatText}]}) {
        /continue
      }

      /call Bind_command_requestbuff "${_sender}" Add "${_chatText}"
      /return TRUE

    /next _count

    /return FALSE
  }


  | take a tell to reform your group.
  /if (${_chatText.Equal[makemeleader]}) /makeleader ${_sender}
  /if (${_chatText.Equal[makememasterlooter]}) /delegategmasterlooter ${_sender}

  | raid chat
  /if (${_chatText.Equal[raidinviteme]} && ${Me.DisplayName.Equal[${Raid.Leader}]} && !${Raid.Locked}) /raidinvite ${_sender}
  /if (${_chatText.Equal[makemeraidleader]} && ${Me.DisplayName.Equal[${Raid.Leader}]} && !${Raid.Locked}) /makeraidleader ${_sender}
  /if (${_chatText.Equal[makemedzleader]}) /dzmakeleader ${_sender}
  /if (${_chatText.Equal[makemetaskleader]}) /taskmakeleader ${_sender}

  | misc stuff
  /if (${_chatText.Equal[taketask]}) {
    | ????
  }


/return






| 16th anniversary .. http://special.eqresource.com/pubcrawl.php
|***
#event 16anniStein "#*# Visigothe says, #1#"
sub Event_16anniStein(string line, string _verbage)
  /if (!${Spawn[npc Galdorin].ID}) /return
  /if (${Spawn[npc Galdorin].ID} && ${Target.DisplayName.NotEqual[Galdorin Visigothe]} && ${Spawn[npc Galdorin].Distance} <= 30) {
    /target ID ${Spawn[npc Galdorin].ID}
    /delay 3 ${Target.ID} == ${Spawn[npc Galdorin].ID}
  } else /if (${Spawn[npc Galdorin].Distance} > 30) {
    VOUT ${onotice} ${sep} Must be next to Galdorin Visigothe in Abysmal Sea.
    /return
  }
  /say ${_verbage}
/return
**|

|**
#event 16stein1 "Galdorin Visigothe says, 'My stinky stein has rough dirty lips,'"
#event 16stein2 "Galdorin Visigothe says, 'but she loves a deep carouse.'"
#event 16stein3 "Galdorin Visigothe says, 'Beer or ale are her great trips.'"
#event 16stein4 "Galdorin Visigothe says, 'No matter how many vows'"
#event 16stein5 "Galdorin Visigothe says, 'I make or break, my drinking glass'"
#event 16stein6 "Galdorin Visigothe says, 'reminds me of my lovely Brasse.'"

sub Event_16stein1(string line)
  /say My stinky stein has rough dirty lips,
/return

sub Event_16stein2(string line)
  /say but she loves a deep carouse.
/return

sub Event_16stein3(string line)
  /say Beer or ale are her great trips.
/return

sub Event_16stein4(string line)
  /say No matter how many vows
/return

sub Event_16stein5(string line)
  /say I make or break, my drinking glass
/return

sub Event_16stein6(string line)
  /say reminds me of my lovely Brasse.
/return

****|


|********************************************************************************************
 ** The Ruins of Kunark (April 2000)
 **|


|********************************************************************************************
 ** The Scars of Velious (December 2000)
 **|


|********************************************************************************************
 ** The Shadows of Luclin (December 2001)
 **|


|********************************************************************************************
 ** The Planes of Power (October 2002)
 **|


|********************************************************************************************
 ** The Legacy of Ykesha (February 2003)
 **|


|********************************************************************************************
 ** Lost Dungeons of Norrath (September 2003)
 **|


|********************************************************************************************
 ** Gates of Discord (February 2004)
 **|


|********************************************************************************************
 ** Omens of War (September 2004)
 **|


#Event mask1 "You feel a gaze of deadly power focusing on you.#.#"
sub Event_mask1()
  /if (!${maEntropy.Find[swEventsInc].Value}) /return FALSE

  /if (!${cast_data[FALSE, "Mirrored Mask"]}) {
    /return FALSE
  }

  /call set_stop ${maDebug.Find[stop].Value} SILENT
  /call cast FALSE ${Me.ID} FALSE

/return TRUE






|********************************************************************************************
 ** Dragons of Norrath (February 2005)
 **|


|********************************************************************************************
 ** Depths of Darkhollow (September 2005)



#Event hatchet1 "#*#Hatchet hefts a weighted throwing axe in his offhand#*#"
sub Event_hatchet1
  /if (!${maEntropy.Find[swEventsInc].Value}) /return FALSE
  /echo I NEED TO RUN AWAY
  /declare running            it    local
  /declare distanceNow        float   local
  /declare distanceBefore     float   local
  /declare distanceModifier   int     local
  /declare MoveToID           int     local ${Spawn[Hatchet].ID}
  /declare StopDistance       int     local 200
  /declare distanceTimer      timer 15
  /varset distanceBefore      ${Spawn[${MoveToID}].Distance}
  /varset distanceModifier    1
  /varset running             0
  /stick off
  :moveAway
   /squelch /face fast away id ${MoveToID}

   /if (${Spawn[${MoveToID}].Distance} >= ${StopDistance}) {
     /keypress forward
     /echo I am far from Hatchet!!!
     /return
   }
   /if (${distanceTimer} == 0) {
     /if (${Me.Sneaking}) {
       /varset distanceModifier 2
     } else {
       /varset distanceModifier 1
     }
     /varset distanceNow ${Spawn[${MoveToID}].Distance}
     /if (${Math.Calc[${distanceBefore}-${distanceNow}]} < ${Math.Calc[10/${distanceModifier}]}) {
       /keypress forward
       /if (${Math.Rand[99]} > 50) {
         /keypress strafe_right hold
       } else {
         /keypress strafe_left hold
       }
       /delay 1s
       /keypress strafe_right
       /keypress strafe_left
       /keypress forward hold
     }
     /varset distanceBefore ${Spawn[${MoveToID}].Distance}
     /varset distanceTimer 15
     }
      /if (${running}==0) {
        /keypress forward
        /if (${Spawn[${MoveToID}].Distance} <= ${StopDistance}) {
          /varset running 1
          /keypress forward hold
        }
      } else {
        /if (${Spawn[${MoveToID}].Distance} >= ${StopDistance}) {
          /varset running 0
          /keypress forward
        }
     /goto :moveAway
   }
/return


#Event hatchet2 "#*#Hatchet lifts his axe high#*#"
sub Event_hatchet2
  /if (!${maEntropy.Find[swEventsInc].Value}) /return FALSE
  /echo I NEED TO RUN BETWEEN HIS LEGS
  /declare running              int local
  /declare distanceNow          float local
  /declare distanceBefore       float local
  /declare distanceModifier     int local
  /declare MoveToID             int local ${Spawn[Hatchet].ID}
  /declare StopDistance         int local 6
  /declare distanceTimer        timer 15
  /varset distanceBefore        ${Spawn[${MoveToID}].Distance}
  /varset distanceModifier 1
  /varset running 0

  :moveToSpawn
   /squelch /face fast nolook id ${MoveToID}

   /if (${Spawn[${MoveToID}].Distance} < ${StopDistance}) {
     /keypress forward
     /echo I am at Hatchet!!!
     /return
   }
   /if (${distanceTimer} == 0) {
     /if (${Me.Sneaking}) {
       /varset distanceModifier 2
     } else {
       /varset distanceModifier 1
     }
     /varset distanceNow ${Spawn[${MoveToID}].Distance}
     /if (${Math.Calc[${distanceBefore}-${distanceNow}]} < ${Math.Calc[10/${distanceModifier}]}) {
       /keypress forward
       /if (${Math.Rand[99]} > 50) {
         /keypress strafe_right hold
       } else {
         /keypress strafe_left hold
       }
       /delay 1s
       /keypress strafe_right
       /keypress strafe_left
       /keypress forward hold
     }
     /varset distanceBefore ${Spawn[${MoveToID}].Distance}
     /varset distanceTimer 15
     }
      /if (${running} == 0) {
        /keypress forward
        /if (${Spawn[${MoveToID}].Distance} >= ${StopDistance}) {
          /varset running 1
          /keypress forward hold
        }
      } else {
        /if (${Spawn[${MoveToID}].Distance} < ${StopDistance}) {
          /varset running 0
          /keypress forward
        }
   }
  /goto :moveToSpawn
/return


#Event hatchet3 "#*#Hatchet levels his axe at your upper body#*#"
#Event hatchet3 "#*#Breakneck hoists an arm back#*#"
sub Event_hatchet3
  /if (!${maEntropy.Find[swEventsInc].Value}) /return FALSE
  /echo I NEED TO DUCK
  /if (${Me.Class.Name.Equal[BARD]}) /twist off
 :TopDuck
  /if (${Me.State.NotEqual[DUCK]}) /keypress DUCK
  /doevents
  /delay 5
  /if (${Me.State.Equal[DUCK]}) /goto :TopDuck
  /if (${Me.Class.Name.Equal[BARD]}) /twist
/return

sub Event_hatchet4
  /echo RUN AWAY FROM THE NAMED!!!
  /if (${Stick.Status[ON]}) /stick off
  /mqp
/return


#Event hatchet4 "#*#Hatchet locks eyes with you#*#"
#Event hatchet4 "#*#Breakneck locks eyes with you and snorts#*#"
#Event hatchet5 "#*#Hatchet the torturer levels#*#"
sub Event_hatchet5
  /echo HE IS CHARGING SOMEONE
/return


#Event hatchet6 "#*#You outdistance the reach#*#"
#Event hatchet6 "#*#Breakneck's mallet passes harmlessly overhead.#*#"
#Event hatchet6 "#*#Breakneck, Master at Arms pauses to catch his breath#*#"
#Event hatchet6 "#*#By darting between#*#"
#Event hatchet6 "#*#You outdistance the reach#*#"
#Event hatchet6 "#*#Hatchet's axe passes#*#"
sub Event_hatchet6
  /echo DANGER IS GONE!
  /if (${Me.State.Equal[DUCK]}) /keypress DUCK
/return


#Event Aelfric1 "Touched tenderly.#*#"
sub Event_Aelfric1
  /call event_speakline "Aelfric the Flautist" "Where will you be?"
/return


#Event Aelfric2 "Where will you be?#*#"
sub Event_Aelfric2
  /call Event_SPEAKLINE "Aelfric the Flautist" "Dreaming with me."
/return


#Event Aelfric3 "Dreaming with me.#*#"
sub Event_Aelfric3
  /call Event_SPEAKLINE "Aelfric the Flautist" "Please,"
/return


#Event Aelfric4 "Please,#*#"
sub Event_Aelfric4
  /call Event_SPEAKLINE "Aelfric the Flautist" "everybody, hear the music."
/return


#Event Britton1 "When she and I split ways,#*#"
sub Event_Britton1
  /call Event_SPEAKLINE "Britton Harmony" "it felt like the end of my days."
/return


#Event Britton2 "it felt like the end of my days.#*#"
sub Event_Britton2
  /call Event_SPEAKLINE "Britton Harmony" "Until I suddenly,"
/return


#Event Britton3 "Until I suddenly,#*#"
sub Event_Britton3
  /call Event_SPEAKLINE "Britton Harmony" "Suddenly realized"
/return


#Event Britton4 "suddenly realized#*#"
sub Event_Britton4
  /call Event_SPEAKLINE "Britton Harmony" "this life was better off alone."
/return


#Event Britton5 "this life was better off alone.#*#"
sub Event_Britton5
  /call Event_SPEAKLINE "Britton Harmony" "Solitude was the best gift you ever gave me."
/return


#Event Randal1  "Ol' Nilipus hailed from Misty Thicket#*#"
sub Event_Randal1
  /call Event_SPEAKLINE "Randal Reedsea" "Where'er he smelled Jumjum he'd pick it."
/return


#Event Randal2 "Where'er he smelled Jumjum he'd pick it#*#"
sub Event_Randal2
  /call Event_SPEAKLINE "Randal Reedsea" "The halflings grew cross"
/return


#Event Randal3 "The halflings grew cross#*#"
sub Event_Randal3
  /call Event_SPEAKLINE "Randal Reedsea" "When their profits were lost,"
/return


#Event Randal4 "When their profits were lost,#*#"
sub Event_Randal4
  /call Event_SPEAKLINE "Randal Reedsea" "screamin', 'Where is that brownie? I'll kick it!'"
/return


#Event Seth1 "Another night, in eternal darkness.#*#"
sub Event_Seth1
  /call Event_SPEAKLINE "Seth Kimble" "Time bleeds like a wound that's lost all meaning."
/return


#Event Seth2 "Time bleeds like a wound that's lost all meaning.#*#"
sub Event_Seth2
  /call Event_SPEAKLINE "Seth Kimble" "It's a long winter in the swirling chaotic void."
/return


#Event Seth3 "It's a long winter in the swirling chaotic void.#*#"
sub Event_Seth3
  /call Event_SPEAKLINE "Seth Kimble" "This is my torture,"
/return


#Event Seth4 "This is my torture,#*#"
sub Event_Seth4
  /call Event_SPEAKLINE "Seth Kimble" "my pain and suffering!"
/return


#Event Seth5 "my pain and suffering!#*#"
sub Event_Seth5
  /call Event_SPEAKLINE "Seth Kimble" "Pinch me, O'Death. . ."
/return


#Event queenlocation "#*#Sendaii begins to look in your direction#*#"
sub Event_queenlocation
  /echo Queen is heading right for me
/return


#Event queenae "#*#Sendaii glances to her #1# as she adjusts her footing.#*#"
sub Event_queenae(string line, string Direction)
  /echo Sendaii is going to AE ${Direction}
/return
 **|


|********************************************************************************************
 ** Prophecy of Ro (February 2006)
 **|


|********************************************************************************************
 ** The Serpent's Spine (September 2006)
 **|


|********************************************************************************************
 ** The Buried Sea (February 2007)
 **|


|********************************************************************************************
 ** Secrets of Faydwer (November 2007)


#Event Sleeper "#*#You can't concentrate!#*#"
sub Event_Sleeper
  /echo I got STUNNED!!!!!!
  /if (!${Defined[SleeperCastTimer]}) {
    /declare SleeperCastTimer int outer ${Macro.RunTime}
    /echo Declaring SleeperCastTimer at 10s
  } else {
    /varset SleeperCastTimer ${Math.Calc[${Macro.RunTime}+10]}
    /echo Reseting SleeperCastTimer at 10s
  }
/return


#Event UnstableCharge "#*#An unstable charge courses through you.#*#"
sub Event_UnstableCharge
  /echo CHARGE ON ME...CURE!!!
/return

#Event Front "#*#The mighty prismatic dragon prepares a terrible tail strike!#*#"
sub Event_Front
   /if (${lstClassMelee.Find[${Me.Class.Name}]} || ${lstClassMelee.Find[${Me.Class.ShortName}]}) /squelch /stick ${StickSetting} front
   /return


#Event Back "#*#Kerafyrm tenses for a charge that will overwhelm his foes!#*#"
#Event Back "#*#Kerafyrm exhales a blast of magical prismatic energy!#*#"
sub Event_Back
   /if (${lstClassMelee.Find[${Me.Class.Name}]} || ${lstClassMelee.Find[${Me.Class.ShortName}]}) /squelch /stick ${StickSetting} behind
/return

 **|



|********************************************************************************************
 ** Seeds of Destruction (October 2008)


#Event QueenAE  "#*#Queen Malarian anchors her feet to the ground and prepares to unleash a wave of burning acid#*#"
sub Event_QueenAE
  /if (!${maEntropy.Find[swEventsInc].Value}) /return FALSE
  /echo Getting away from Queen
  /if (${Spawn[Queen Malarian].Distance}>30) /return
  /return
  /target clear
  /play QueenAE smart
  /delay 2s ${Me.Moving}
  :MovingFromQueen
   /delay 2s !${AdvPath.Playing}
  /if (${AdvPath.Playing}) /goto :MovingFromQueen
/return


#Event QueenAdd "#*#You see the queen glare at you#*#"
sub Event_QueenAdd
  /if (!${maEntropy.Find[swEventsInc].Value}) /return FALSE
  /echo I NEED to RUN AWAY!
  /target clear
  /play QueenAdd smart
  /delay 1s ${Me.Moving}
  :MoveToQueenSafeSpot
   /delay 1s !${Me.Moving}
  /if (${Me.Moving}) /goto :MoveToQueenSafeSpot
  :CheckQueenAE
   /delay 1s ${Me.Buff[Wave of Infestation].ID}
  /if (!${Me.Buff[Wave of Infestation].ID}) /goto :CheckQueenAE
  :CureQueenAE
   /delay 1s !${Me.Buff[Wave of Infestation].ID}
  /if (${Me.Buff[Wave of Infestation].ID}) /goto :CureQueenAE
  /play RunToQueenCamp
  /delay 1s ${Me.Moving}
  :BackToQueenCamp
   /delay 1s !${Me.Moving}
  /if (${Me.Moving}) /goto :BackToQueenCamp
/return


#Event BrektRock "#*#A shadow passes over you#*#"
sub Event_BrektRock
  /if (!${maEntropy.Find[swEventsInc].Value}) /return FALSE
  /echo GET AWAY FROM THE RAID NOW!!
/return


#Event BrektFeet "#*#Lord Brekt prepares to unleash his fury on the mortals at his feet#*#"
Sub_Event_BrektFeet
  /if (!${maEntropy.Find[swEventsInc].Value}) /return FALSE
  /echo GET AWAY FROM BREKT'S FEET
/return


#Event BrektDT "#*#You are corrupted by discordant energy#*#"
Sub_Event_BrekDT
  /if (!${maEntropy.Find[swEventsInc].Value}) /return FALSE
  /echo Possible got the DT
  /delay 1s ${Me.Buff[Discordant Doom].ID}
  /if (${Me.Buff[Discordant Doom].ID}) /echo I NEED A CURE!!!
/return


#Event Mindblight "#*#The maw in the #1# gurgles and coughs up a drop of ooze.#*#"
sub Event_Mindblight(Line, Direction)
  /if (!${maEntropy.Find[swEventsInc].Value}) /return FALSE
  /if (${Direction.Equal[southwest]}) {
    /if (${Spawn[npc =The Mindblight].ID}) /return
    /echo Moving NE
    /moveto loc 201 -79
    /delay 2s ${MoveTo.Moving}
    /delay 20s !${MoveTo.Moving}
    /return
  }
  /if (${Direction.Equal[northwest]}) {
    /if (${Spawn[npc =The Mindblight].ID}) /return
    /echo Moving SE
    /moveto loc 94 -86
    /delay 2s ${MoveTo.Moving}
    /delay 20s !${MoveTo.Moving}
    /return
  }
  /if (${Direction.Equal[southeast]}) {
    /if (${Spawn[npc =The Mindblight].ID}) /return
    /echo Moving NW
    /moveto loc 215 71
    /delay 2s ${MoveTo.Moving}
    /delay 20s !${MoveTo.Moving}
    /return
  }
  /if (${Direction.Equal[northeast]}) {
    /if (${Spawn[npc =The Mindblight].ID}) /return
    /echo Moving SW
    /moveto loc 92 82
    /delay 2s ${MoveTo.Moving}
    /delay 20s !${MoveTo.Moving}
    /return
  }
/return


#Event mindshear "#*#You get the feeling that someone is watching you#*#"
sub Event_mindshear
  /echo i need to move away from the raid
/return


 **|



|********************************************************************************************
 ** Underfoot (December 2009)


#Event CV2Corrupt "#*#Seething Corruption BURNS within you, #1#!#*#"
sub Event_CV2Corrupt(string Line, string Name)
  /if (!${maEntropy.Find[swEventsInc].Value}) /return FALSE
  /if (!${Name.Find[${Me.Name}]}) /return
  /echo I NEED to RUN AWAY!
  /target clear
  /play SeethCorrupt smart
  /delay 1s ${Me.Moving}
  :MoveCorruptSafeSpot
   /delay 1s !${Me.Moving}
  /if (${Me.Moving}) /goto :MoveCorruptSafeSpot
  :CheckCorruptAE
   /delay 1s ${Me.Song[Seething Corruption].ID}
  /if (!${Me.Song[Seething Corruption].ID}) /goto :CheckCorruptAE
  :WaitCorruptAE
   /delay 1s !${Me.Song[Seething Corruption].ID}
  /if (${Me.Song[Seething Corruption].ID}) /goto :WaitCorruptAE
  /play RunToCorruptCamp
  /delay 1s ${Me.Moving}
  :BackToCorruptCamp
   /delay 1s !${Me.Moving}
  /if (${Me.Moving}) /goto :BackToCorruptCamp
/return


#Event Beast "#*#Grunkuck looks at #1# with glee!#*#"
#Event Beast "#*#Grunkuck looks at #1# with strange intent#*#"
sub Event_Beast(line, person)
  /if (!${maEntropy.Find[swEventsInc].Value}) /return FALSE
  /if (${person.Equal[${Me}]}) {
    /echo say I got beast ONE!!
    /target clear
    /play path1 smart
    /delay 1s ${Me.Moving}
    /delay 15s
    /play path1 reverse smart
    /delay 1s ${Me.Moving}
    :BackToCamp
      /delay 1s !${Me.Moving}
    /if (${Me.Moving}) /goto :BackToCamp
  }
/return



#Event LCWhip "#*#A whip wraps around your limbs!#*#"
sub Event_LCWhip
  /if (!${maEntropy.Find[swEventsInc].Value}) /return FALSE
  /echo I NEED to RUN AWAY!
  /target clear
  /play LCWhip smart
  /delay 1s ${Me.Moving}
  :MoveToLCSafeSpot
   /delay 1s !${Me.Moving}
  /if (${Me.Moving}) /goto :MoveToLCSafeSpot
  :CheckLCAE
   /delay 1s ${Me.Buff[Bullwhip Grab].ID}
  /if (!${Me.Buff[Bullwhip Grab].ID}) /goto :CheckLCAE
  :CureLCAE
   /delay 1s !${Me.Buff[Bullwhip Grab].ID}
  /if (${Me.Buff[Bullwhip Grab].ID}) /goto :CureLCAE
  /play LCWhip reverse smart
  /delay 1s ${Me.Moving}
  :BackToLCCamp
   /delay 1s !${Me.Moving}
  /if (${Me.Moving}) /goto :BackToLCCamp
/return

 **|


|********************************************************************************************
 ** House of Thule (October 2010)
 **|


|********************************************************************************************
 ** Veil of Alaris (November 2011)


#Event SpineFreeze "#*#Spinerfreezer's eye all focus upon you#*#"
sub Event_Spinefreeze
  /if (!${maEntropy.Find[swEventsInc].Value}) /return FALSE
  /echo I NEED TO RUN AWAY!!
/return


#Event RunAway "#*#The Tentacle of Terror Mentor points at you#*#"
sub Event_Runaway
  /if (!${maEntropy.Find[swEventsInc].Value}) /return FALSE
  /echo I NEED TO RUN AWAY!!
/return


#Event CVSisterRun "#*##1# lashes out with debilitating energy#*#"
sub Event_CVSisterRun(string Line, string Name)
  /if (!${maEntropy.Find[swEventsInc].Value}) /return FALSE
  /echo I NEED TO RUN AWAY FROM ${Name}!!
/return


#Event GolemAEArg "#*#prepares for a massive strike at anyone not hidden from sight#*#"
sub Event_GolemAEArg
  /if (!${maEntropy.Find[swEventsInc].Value}) /return FALSE
  /if (!${Spawn[Avatar of Steel].LineOfSight}) /return
  /echo I NEED to RUN AWAY!
  /target clear
  /play RunFromGolem smart
  /delay 1s ${Me.Moving}
  :RunGolemSafeSpot
   /delay 1s !${Me.Moving}
  /if (${Me.Moving}) /goto :RunGolemSafeSpot
/return


#Event GolemSteelAdd "#*#drops a piece of steel from its body#*#"
sub Event_GolemSteelAdd
  /if (!${maEntropy.Find[swEventsInc].Value}) /return FALSE
  /echo Add UP!!!
  |**
  /return
  /if (!${Spawn[a steel pattern].ID}) /return
  /echo Killing this mofo steel pattern!!
  /target id ${Spawn[a steel pattern].ID}
  /play SteelAdd smart reverse
  /delay 1s ${Me.Moving}
  :RunGolemAddSpot
   /delay 1s !${Me.Moving}
  /if (${Me.Moving}) /goto :RunGolemAddSpot
/return


#Event RTCure "Seek out the purity stone to remove the filth that surrounds you."
sub Event_RTCure
  /if (!${maEntropy.Find[swEventsInc].Value}) /return FALSE
  /echo I NEED TO BE CURED!
/return


#Event RTCureDone "You have been cleansed."
sub Event_RTCureDone
  /if (!${maEntropy.Find[swEventsInc].Value}) /return FALSE
  /echo CURED
/return


#Event RTJump "Jump from this tower as the ultimate sacrifice to Sholoth!"
sub Event_RTJump
  /if (!${maEntropy.Find[swEventsInc].Value}) /return FALSE
  /echo I NEED TO JUMP!
/return


#Event RTJumpDone "You have made the proper sacrifice."
sub Event_RTJumpDone
  /if (!${maEntropy.Find[swEventsInc].Value}) /return FALSE
  /echo I JUMPED
/return


#Event RTSludge "Your feet are stuck in a pool of sludge.#*#"
sub Event_RTSludge
  /if (!${maEntropy.Find[swEventsInc].Value}) /return FALSE
  /echo MOVE ME MAFUKKA in SLUDGE!!!
/return

 **|

|********************************************************************************************
 ** Rain of Fear (November 2012)


| ** Kael Raid
#Event DainBanish "#*#Dain Frostreaver IV shouts 'You will be banished, #1#, #2#, and #3#!#*#"
sub Event_DainBanish(string Line, string PersonOne, string PersonTwo, string PersonThree)
   /if (!${maEntropy.Find[swEventsInc].Value}) /return FALSE
  /declare sPersonOne     string local ${PersonOne}
  /declare sPersonTwo     string local ${PersonTwo}
  /declare sPersonThree   string local ${PersonThree}

  /if (${sPersonOne.Equal[${Me.CleanName}]} || ${sPersonTwo.Equal[${Me.CleanName}]} || ${sPersonThree.Equal[${Me.CleanName}]}) {
    /echo Running out of arena
    /play RunFromArena smart
    /delay 10s ${Me.Moving}
    :MoveFromArena
     /delay 1s !${Me.Moving}
    /if (${Me.Moving} || ${AdvPath.Waypoints}>1) /goto :MoveFromArena
  }
/return


| ** Crystal Caverns Raid
#Event danela "#*#A Ry`Gorr miner throws a barrel of dynamite at#1##*#"
sub Event_danela(string Line, string Person, string Person2)
   /if (!${maEntropy.Find[swEventsInc].Value}) /return FALSE
  /if (${Line.Find[${Me.CleanName}]}) {
    /echo I GOT BARREL!!
    /play Danela smart
    /delay 10s ${AdvPath.Waypoints}<1
    /play off
    /delay 3s !${Me.Moving}
    /play Danela reverse smart
    /delay 10s ${AdvPath.Waypoints}<1
  }
/return


| ** Chapterhouse Raid .. fuck this place
|#Event chapterhouse "Your bones are burning#*#"
sub Event_chapterhouse()
  /if (!${maEntropy.Find[swEventsInc].Value}) /return FALSE
  /echo I NEED to RUN AWAY!
  /target clear
  /echo running to pool
  /play ChapterPool smart
  /delay 1s ${Me.Moving}
  :MoveChaptSafeSpot
   /delay 1s !${Me.Moving}
  /if (${Me.Moving}) /goto :MoveChaptSafeSpot
  :WaitChaptDot
   /delay 1s !${Me.Buff[Scorching Bones].ID}
  /if (${Me.Buff[Scorching Bones].ID}) /goto :WaitChaptDot
  /echo Running back to camp
  /play ChapterReturn smart
  /delay 1s ${Me.Moving}
  :BackToChaptCamp
   /delay 1s !${Me.Moving}
  /if (${Me.Moving}) /goto :BackToChaptCamp
/return


#Event ChaptDoT   "#*##1# begins to burn!#*#"
sub Event_ChaptDot(string line, string Name)
  /if (!${swEventsInc}) /echo ${Name} got the DoT!
/return


| ** Breeding Grounds Raid Events
#Event zalikor "#*#You have been corrupted by the fear crystal#*#"
sub Event_Zalikor()
  /if (!${maEntropy.Find[swEventsInc].Value}) /return FALSE
  /if (!${Zone.ShortName.Equal[breedinggrounds_raid]}) /return
  /echo I need to RUN!!!
  /if (${Me.Casting.ID}) /call Interrupt
  :TopOfDrake
   /play Zalikor smart slow normal
   /delay 1s ${Me.Moving}
   :stillmoving
     /delay 5 ${AdvPath.NextWaypoint}
    /if (${AdvPath.State}==3 || ${AdvPath.NextWaypoint}) /goto :stillmoving
/return


#Event AuraNuke "#*#Zalikor directs his fiery breath toward the aura of #1#."
sub Event_AuraNuke((string line, string AuraName)
  /if (!${maEntropy.Find[swEventsInc].Value}) /return FALSE
  /echo ${AuraName}
  /declare RunAway string local FALSE
  /if (${AuraName.Equal[fire]} && ${Math.Distance[1109,-570]}<=50) {
    /echo I have to run!!
    /varset RunAway TRUE
  } else /if (${AuraName.Equal[power]} && ${Math.Distance[619,-391]}<=50) {
    /varset RunAway TRUE
  } else /if (${AuraName.Equal[magic]} && ${Math.Distance[633,-558]}<=50) {
      /varset RunAway TRUE
  }
  /if (${RunAway.Equal[TRUE]}) {
    /play ${AuraName} smart slow
    /delay 1s ${Me.Moving}
    /delay 14s
    /play ${AuraName} reverse smart slow
    /delay 1s ${Me.Moving}
    /delay 4s !${Me.Moving}
  }
/return


#Event eggs "#*#The ground beneath your feet begins to heat as Zalikor flies away#*#"
sub Event_eggs()
  /if (!${maEntropy.Find[swEventsInc].Value}) /return FALSE
  /if (!${Zone.ShortName.Equal[breedinggrounds_raid]}) /return
  /varset DoLeash 0
  /echo I need to up top!!!
  /if (${Me.Casting.ID}) /call Interrupt
  :TopOfEggs
   /play eggs smart slow normal
   /delay 1s ${Me.Moving}
   :stillmoving
    /delay 5 ${AdvPath.NextWaypoint}
    /if (${AdvPath.State}==3 || ${AdvPath.NextWaypoint}) /goto :stillmoving
/return


| ** Xorbb Raid
#Event FindleFireBall1 "#*#You feel an inner warmth as Findlewill points at you#*#"
sub Event_FindleFireBall1()
  /if (!${maEntropy.Find[swEventsInc].Value}) /return FALSE
  /echo I NEED to RUN! Have Fireball!
  /target clear
  :TopOfFireBallSub
   /echo running south
   /play FindleFireBall smart
   /delay 1s ${Me.Moving}
   :MoveFireSafeSpot
    /delay 1s !${Me.Moving}
   /if (${Me.Moving}) /goto :MoveFireSafeSpot
   :WaitFireBallDot
    /delay 1s ${Me.Buff[Engulfing Flames].ID}
    /if (!${Me.Ducking} && !${Me.Moving}) /keypress DUCK
   /if (!${Me.Buff[Engulfing Flames].ID}) /goto :WaitFireBallDot
   /echo I have the fireball DoT on me now!
   :WaitFireBallDotFade
    /if (!${Me.Ducking}) /keypress DUCK
    /delay 1s ${Me.Buff[Engulfing Flames].ID}
   /if (${Me.Buff[Engulfing Flames].ID}) /goto :WaitFireBallDotFade
   :FireBallHere
    /if (${Me.Buff[Engulfing Flames].ID} && !${Me.Ducking}) /keypress DUCK
    /delay 1s ${Spawn[npc "a fireball"].ID} && ${Spawn[npc "a fireball"].Distance}<40
   /if (${Spawn[npc "a fireball"].ID} && ${Spawn[npc "a fireball"].Distance}<40) /goto :FireBallHere
   /if (${Me.Ducking}) /keypress DUCK
   /echo Running back.
   /play FindleFireBall smart reverse
   /delay 1s ${Me.Moving}
   :BackToFireCamp
    /delay 1s !${Me.Moving}
   /if (${Me.Moving}) /goto :BackToFireCamp
  /if (${Me.Buff[Engulfing Flames].ID}) /goto :TopOfFireBallSub
  /doevents flush FindleFireBall1
/return


#Event FindleFireBall2  "#*#Findlewill begins casting a fireball #1##*#"
sub Event_FindleFireBall2(string Line, string Name)
  /if (!${maEntropy.Find[swEventsInc].Value}) /return FALSE
  /if (${Spawn[Findlewill].Distance}>40) /return
  /echo I NEED to run away from Findle!
  /target clear
  /echo Running away from Findle
  /play RunFromFindle smart
  /delay 1s ${Me.Moving}
  :MoveAwayFindleSpot
   /delay 1s !${Me.Moving}
  /if (${Me.Moving}) /goto :MoveAwayFindleSpot
  /if (${Spawn[pc ${Name}].NearestSpawn[npc Findlewill].Distance}<40) /delay 2s ${Spawn[pc ${Name}].NearestSpawn[npc Findlewill].Distance}>40
  /doevents flush FindleFireBall2
/return


#Event XorbbEye "#*#You are consumed by a sense of power watching and hating you#*#"
sub Event_XorbbEye()
  /if (!${maEntropy.Find[swEventsInc].Value}) /return FALSE
  /echo I NEED to RUN! Have Fire Emote!
  /target clear
  /echo Breaking LoS
  /play XorbbEye smart
  /delay 1s ${Me.Moving}
  :MoveEyeSafeSpot
   /delay 1s !${Me.Moving}
  /if (${Me.Moving}) /goto :MoveEyeSafeSpot
  /delay 4s
  /echo Running back.
  /play XorbbEye smart reverse
  /delay 1s ${Me.Moving}
  :BackToEyeCamp
   /delay 1s !${Me.Moving}
  /if (${Me.Moving}) /goto :BackToEyeCamp
  /doevents flush XorbbEye
/return


#Event Xorbb3 "King Xorbb shouts '|${Me.CleanName}|'"
sub Event_Xorbb3()
  /if (!${maEntropy.Find[swEventsInc].Value}) /return FALSE
  /echo I need to face away from him for a bit
  /call set_stop ${maDebug.Find[stop].Value} SILENT
  /call target set ${Spawn[npc King Xorbb].ID}
  /squelch /face away
  /if (${lsClassMelee.Find[${Me.Class.Name}]}) {
    /delay 16s
  }
/return



| ** Chelsith Raid
#Event SlaveCourage "#*#a frightened slave looks courageous#*#"
sub Event_SlaveCourage()
  /if (!${maEntropy.Find[swEventsInc].Value}) /return FALSE
/return


#Event ChelsMove "#*#The crowd in the #1# section boos. They want the action brought closer!#*#"
sub Event_ChelsMove(string Line, string Area)
  /if (!${maEntropy.Find[swEventsInc].Value}) /return FALSE
  /echo Move mob to ${Area}
/return


#Event KilsevRage "#*#Kilsev begins to focus his rage at #1##*#"
sub Event_KilsevRage(string Line, string Person)
  /if (!${maEntropy.Find[swEventsInc].Value}) /return FALSE
 /if (${Person.Equal[${Me.CleanName}]}) {
    /echo I HAVE TO KITE!!!
    /if (!${Defined[KilsevRaged]}) {
      /declare KilsevRaged int outer 1
    } else {
      /varset KilsevRaged 1
    }

    :TopOfKilsevRaged
     /doevents
     /delay 5 !${KilsevRaged}
    /if (${KilsevRaged}) /goto :TopOfKilsevRaged
    /varset VyzhClose 0
  }
/return


#Event KilsevRageOff "#*#Kilsev's rage subsides#*#"
sub Event_KilsevRageOff()
  /if (!${maEntropy.Find[swEventsInc].Value}) /return FALSE
  /if (!${Defined[KilsevRaged]}) {
    /declare KilsevRaged int outer 0
  } else {
    /varset KilsevRaged 0
  }
  /echo Rage off!
/return


#Event VyzhAE   "#*#Vyzh`dra draws power from the crowd's energy#*#"
sub Event_VyzhAE()
  /if (!${maEntropy.Find[swEventsInc].Value}) /return FALSE
  /if (!${Defined[VyzhClose]}) {
    /declare VyzhClose int outer 0
  } else {
    /varset VyzhClose 0
  }
  :TopOfVyzhAE
   /if (${Spawn[Vyzh`dra the Godslayer].ID}) {
     /call DoLeashPerson
   }
   /doevents
   /delay 1
  /if (${Spawn[Vyzh`dra the Godslayer].Distance}<=150 || ${VyzhClose}) /goto :TopOfVyzhAE
  /echo Vyzh AE all done!!
  /varset VyzhClose 0
/return


#Event VyzhGetClose "#*#Vyzh`dra commands his tigers to attack#*#"
sub Event_VyzhGetClose()
  /if (!${maEntropy.Find[swEventsInc].Value}) /return FALSE
  /echo Oh shit move back!1
  /moveto loc ${Spawn[Vyzh`dra the Godslayer].Y} ${Spawn[Vyzh`dra the Godslayer].X}
  /if (!${Defined[VyzhClose]}) {
    /declare VyzhClose int outer 1
  } else {
    /varset VyzhClose 1
  }
  /delay 3s ${Spawn[Vyzh`dra the Godslayer].Distance}<20
/return


#Event ChelsStopDPS "#*#The crowd boos. They think the fight is going too fast#*#"
sub Event_ChelsStopDPS()
  /if (!${maEntropy.Find[swEventsInc].Value}) /return FALSE
  /echo Stop DPS ${CureList.Find[${Me.Class}]} ${Me.Class}
  /if (!${CureList.Find[${Me.Class}]}) {
    /target myself
    /delay 16s
  }
/return


| ** Temple of Veeshan Raid

#Event AaryonarGaze "Aaryonar turns a dreadful gaze upon YOU!"
sub Event_AaryonarGaze()
  /if (!${maEntropy.Find[swEventsInc].Value}) /return FALSE
  /declare CasterClasses string local "|DRU|CLR|SHM|ENC|NEC|MAG|WIZ|"
  /echo OH SHIT HE BE LOOKIN AT ME
  /if (${Target.CleanName.Equal[Aaryonar]}) {
    /melee off
    /stick off
    /face away fast
  } else /if (${CasterClasses.Find[${Me.Class.ShortName}]} && ${Target.CleanName.NotEqual[Aaryonar]}) {
    /target Aaryonar
    /delay 1s ${Target.CleanName.Equal[Aaryonar]}
    /melee off
    /stick off
    /face away fast
  }
  :AaryonarGaze
   /if (${Target.CleanName.Equal[Aaryonar]} && !${CasterClasses.Find[${Me.Class.ShortName}]}) {
     /doevents
     /delay 1s ${Target.ID}
     /goto :AaryonarGaze
   }
   /melee on
   /echo Gaze is off me!
/return



#Event AaryonarGaze "Aaryonar turns a dreadful gaze upon YOU!"
sub Event_AaryonarGaze()
  /if (!${maEntropy.Find[swEventsInc].Value}) /return FALSE
  /call set_stop NOECHO
  /call clear_combat
  /face away "Aaryonar" fast nolook
  /delay 3s
  /while (${Me.Buff[Dreadgaze].ID}) {
    /delay 1s
  }
/return




#Event AaryonarGazeOff "Aaryonar turns his gaze away from you."
sub Event_AaryonarGazeOff()
  /if (!${maEntropy.Find[swEventsInc].Value}) /return FALSE
  /echo He cant handle looking at my junk!
  /if (${Target.CleanName.Equal[Aaryonar]}) /target clear
/return


#Event ZlaAE "Zlexak has spotted you!#*#"
#Event ZlaAE "You are covered in filth#*#"
sub Event_ZlaAE()
  /if (!${maEntropy.Find[swEventsInc].Value}) /return FALSE
  /echo I NEED to RUN AWAY!
  /target clear
  /echo running away
  /play ZlaxRun smart
  /delay 1s ${Me.Moving}
  :MoveZlaxSafeSpot
   /delay 1s !${Me.Moving}
  /if (${Me.Moving}) /goto :MoveZlaxSafeSpot
  /if (${CureList.Find[${Me.Class}]}) /return
  :WaitZlaxDot
   /delay 1s ${Me.Buff[Eye of Zlexak].ID} || ${Me.Buff[Fog of Filth I].ID} || ${Me.Buff[Fog of Filth II].ID} || ${Me.Buff[Fog of Filth III].ID} || ${Me.Buff[Fog of Filth IV].ID}
  /if (${Me.Buff[Eye of Zlexak].ID} || ${Me.Buff[Fog of Filth I].ID} || ${Me.Buff[Fog of Filth II].ID} || ${Me.Buff[Fog of Filth III].ID} || ${Me.Buff[Fog of Filth IV].ID}) /goto :WaitZlaxDot
  /echo Running back to camp
  /play ZlaxRun smart reverse
  /delay 1s ${Me.Moving}
  :BackToZlax
   /delay 1s !${Me.Moving}
  /if (${Me.Moving}) /goto :BackToZlax
/return


| ** Other

#Event Naggy "Your skin begins to glow#*#"
sub Event_Naggy()
  /echo I GOT NAGGY EMOTE!!
/return

 **|






|********************************************************************************************
 ** Call of the Forsaken (October 2013)



| ** Dead Hills 1 Raid
|#Event PrimeCloud  "#*#cloud of disease begins to form around #1##*#"
sub Event_PrimeCloud(string Line, string Person)
  /if (!${maEntropy.Find[swEventsInc].Value}) /return FALSE
  /echo ${Line.Arg[1]} - ${Line.Arg[2]} - ${Line.Arg[3]} - ${Line.Arg[4]} - ${Line.Arg[5]} - ${Line.Arg[6]} - ${Line.Arg[7]} - ${Line.Arg[8]} - ${Line.Arg[9]}
  /echo ${Line}
  /if (${Line.Find[${Me.CleanName}]}) {
    /echo I HAVE TO KITE!!!
    /if (!${maEntropy.Find[swEventsInc].Value}) /return FALSE
    /if(${DoLeash}) /varset DoLeash 0

    /if (!${Defined[PrimeCloud]}) {
      /declare PrimeCloud int outer 1
    } else {
      /varset PrimeCloud 1
    }

    :TopOfPrimeCloud
     /if (!${Advpath.Playing}) /play PrimeKit loop smart
     /delay 3s !${Advpath.Playing}
    /if (${PrimeCloud}) /goto :TopOfPrimeCloud
  }
  /doevents flush PrimeCloud
/return


#Event PrimeDoT "#*#Your flesh begins to bubble and ooze#*#"
sub Event_PrimeDoT()
  /if (!${maEntropy.Find[swEventsInc].Value}) /return FALSE
  /echo I NEED A CURE!!
/return


|#Event Prime1 "#*#Your flesh begins to bubble and ooze.#*#"

|#Event CurePrime   "#1# #*#xulous prime#*#"
sub Event_CurePrime(string Line string Person)
  /if (!${maEntropy.Find[swEventsInc].Value}) /return FALSE
  /if (!${Person.Find[You]} && (${Me.Class.Name.Equal[Ranger]} || ${Me.Class.Name.Equal[BeastLord]}) ) {
    /delay 3s ${Me.SpellReady[${DiseaseSpell}]}
    /squelch /stick off
    /call CastSpell "${DiseaseSpell}" 0 ${Spawn[pc ${Person}].ID}
  }
  /doevents flush CurePrime
/return


| ** West Karna Raid
#Event BurningPrison  "#*#You are frozen in place#*#"
#Event BurningPrison  "#*#You are chilled to the bone#*#"
sub Event_BurningPrison()
  /if (!${maEntropy.Find[swEventsInc].Value}) /return FALSE
  /if (${Me.Buff[Burning Prison].ID}) {
    /if (${Me.AltAbilityReady[Battle Leap]}) {
      /alt activate ${Me.AltAbility[Battle Leap].ID}
      /delay 5 !${Me.AltAbilityReady[Battle Leap]}
    } else /if (${FindItem[Steamjet Pack].Timer}) {
      /call CastItemClicky "Steamjet Pack"
    } else {
      /rewind
      /delay 5
    }
    /if (${Me.Casting.ID}) /call Interrupt
  }
/return


| Neriak Raids
#Event NeriakShock "#*#The golem glows and begins to hum, turning its head toward you.#*#"
sub Event_NeriakShock()
  /if (!${maEntropy.Find[swEventsInc].Value}) /return FALSE
  /echo I need to run away from the raid shock emote
  /if (!${maEntropy.Find[swEventsInc].Value}) /return FALSE
  /if (${DoLeash}) /leash off
  /play shock smart
  /delay 10s ${AdvPath.Waypoints} < 1
  /delay 6s ${Me.Buff[Sonic Shock].ID}
  /play shock smart reverse
  /delay 10s ${AdvPath.Waypoints} < 1
/return


#Event NeriakChill "#*#Take that one's essence, my pet!#*#"
sub Event_NeriakChill(string line)
  /if (!${maEntropy.Find[swEventsInc].Value}) /return FALSE
  /if (${line.Find[${Me}]}) {
    /echo I need to run away from the raid got essense emote
    /if (!${maEntropy.Find[swEventsInc].Value}) /return FALSE
    /if (${DoLeash}) /leash off
    /play shock smart
    /delay 10s ${AdvPath.Waypoints} < 1
    /delay 6s ${Me.Buff[Grave Chill].ID}
    /play shock smart reverse
    /delay 10s ${AdvPath.Waypoints} < 1
  }
/return


#Event NeriakCold "#*#Sooo cooooold#*#"
sub Event_NeriakCold()
  /if (!${maEntropy.Find[swEventsInc].Value}) /return FALSE
  /if (${lstClassMelee.Find[${Me.Class.Name}]}) {
    /echo moving back!
    /squelch /stick 120 moveback
    /delay 12s
    /squelch /stick off
  }
/return


#Event NeriakPulse "#*#An energy pulse moves toward you.#*#"
sub Event_NeriakPulse()
  /if (!${maEntropy.Find[swEventsInc].Value}) /return FALSE
  /echo I need to run away from the raid from pulse!
  /if (${DoLeash}) /leash off
  /play pulse smart
  /delay 15s ${AdvPath.Waypoints} < 1
  /delay 45s ${SpawnCount[npc "An energy pulse" radius 50]}
  /play pulse2 smart
  /delay 45s ${SpawnCount[npc "An energy pulse" radius 50]}
  /play pulse3 smart
  /delay 45s ${SpawnCount[npc "An energy pulse" radius 50]}
  /play pulse smart reverse
  /delay 10s ${AdvPath.Waypoints} < 1
/return


#Event NeriakSpear "#*#you are pinned to the ground with a spear#*#"
sub Event_NeriakSpear()
  /if (!${maEntropy.Find[swEventsInc].Value}) /return FALSE
  /stand
  /moveto loc 440.50 -668
  /delay 1s
  /delay 6s !${Me.Moving}
  /delay 6s
  /moveto loc 454.06 -504
  /delay 1s
  /delay 6s !${Me.Moving}
  /doevents flush NeriakSpear
/return



| ** Bixie Raids
#Event Bixie2Red "#*#red cloud forms above#*#"
sub Event_Bixie2Red(string line)
  /if (!${maEntropy.Find[swEventsInc].Value}) /return FALSE
  /if (${line.Find[${Me}]}) {
    /declare tempx float local ${Me.X}
    /declare tempy float local ${Me.Y}
    /moveto loc -932 -2525
    :loop
     /doevents Bixie2Safe
    /if (!${Macro.Return.Find[Safe]}) /goto :loop
    /moveto loc ${tempy} ${tempx}
    /delay 1s
    /delay 6s !${Me.Moving}
  }
/return


#Event Bixie2Yellow "#*#yellow cloud forms above#*#"
sub Event_Bixie2Yellow(string line)
  /if (!${maEntropy.Find[swEventsInc].Value}) /return FALSE
  /if (${line.Find[${Me}]}) {
    /declare tempx float local ${Me.X}
    /declare tempy float local ${Me.Y}
    /moveto loc -614 -2444
    :loop
     /doevents Bixie2Safe
    /if (!${Macro.Return.Find[Safe]}) /goto :loop
    /moveto loc ${tempy} ${tempx}
    /delay 2s
    /delay 6s !${Me.Moving}
  }
/return


#Event Bixie2Safe "#*#spines rains down around#*#"
sub Event_Bixie2Safe(string line)
  /if (!${maEntropy.Find[swEventsInc].Value}) /return FALSE
 /if (${line.Find[${Me}]}) /return Safe
/return


#Event SlaveThokWhip "#*#Slave Driver Thokk coils up his whip and aims for your head#*#"
sub Event_SlaveThokWhip()
  /if (!${maEntropy.Find[swEventsInc].Value}) /return FALSE
  /echo I NEED TO DUCK
  /if (${Me.Class.Name.Equal[BARD]}) /twist off
  :TopDuck
    /if (${Me.State.NotEqual[DUCK]}) /keypress DUCK
    /doevents
    /delay 10 ${Me.State.Equal[DUCK]}
  /if (${Me.State.Equal[DUCK]}) /goto :TopDuck
  /if (${Me.Class.Name.Equal[BARD]}) /twist
/return


#Event SlaveThokStand "#*#Slave Driver Thokk's whip passes harmlessly overhead#*#"
sub Event_SlaveThokStand()
  /if (!${maEntropy.Find[swEventsInc].Value}) /return FALSE
  /echo Danger Gone!
  /if (${Me.State.Equal[DUCK]}) /keypress DUCK
/return


#Event DSOn "#*#The mark of Nife flashes before your eyes#*#"
#Event DSOn "#*#Warchanter Neezzee begins to cast a spell. <Chaotic Rune>#*#"
sub Event_DSOn()
  /if (!${maEntropy.Find[swEventsInc].Value}) /return FALSE
  /echo I have DS on
  /if (${lstClassMelee.Find[${Me.Class.Name}]} || ${lstClassMelee.Find[${Me.Class.ShortName}]}) {
    /target myself
    /delay 1s ${Target.ID}==${Me.ID}
    :TopDS
      /doevents
      /delay 10 ${Target.ID}!=${Me.ID}
    /if (${Target.ID}==${Me.ID}) /goto :TopDS
  }
/return


#Event DSOff "#*#beats on the drum#*#"
#Event DSOff "#*#The mystic symbol fades#*#"
sub Event_DSOff()
  /if (!${maEntropy.Find[swEventsInc].Value}) /return FALSE
  /echo DS Gone!
  /if (${lstClassMelee.Find[${Me.Class.Name}]} || ${lstClassMelee.Find[${Me.Class.ShortName}]}) /target clear
/return


| ** Tower of Rot Raid
#Event ToRClay "#*#Lord Kyle Bayle raises his ancient claymore high overhead#*#"
sub Event_ToRClay()
  /if (!${maEntropy.Find[swEventsInc].Value}) /return FALSE

  /if (${lstClassMelee.Find[${Me.Class.Name}]} || ${lstClassMelee.Find[${Me.Class.ShortName}]}) {
    /target myself
    /delay 1s ${Target.ID}==${Me.ID}
    :TopClay
      /doevents
      /call Leash
      /delay 10 ${Target.ID}!=${Me.ID}
    /if (${Target.ID}==${Me.ID}) /goto :TopClay
  }
/return


#Event ToRClaySafe "#*#Lord Kyle Bayle swings his ancient claymore with frightening force#*#"
sub Event_ToRClaySafe()
  /if (!${maEntropy.Find[swEventsInc].Value}) /return FALSE
  /if (${lstClassMelee.Find[${Me.Class.Name}]} || ${lstClassMelee.Find[${Me.Class.ShortName}]}) /target clear
/return


#Event ToRSplashAcid "#*#A splash of acid strikes you in the face#*#"
sub Event_ToRSplashAcid()
  /if (!${maEntropy.Find[swEventsInc].Value}) /return FALSE
  /echo I got splash emote!
  /target myself
  /if (${Me.Class.Name.Equal[BARD]}) /twist off
  /delay 3s
  /if (!${Me.Sitting}) /sit
  /delay 10s ${Me.Sitting}
  /delay 1s ${Me.Sitting}
  /if (${Me.Class.Name.Equal[BARD]}) /twist
/return


#Event ToRWither "#*#Poison withers your body, you become weak and vulnerable#*#"
sub Event_ToRWither()
  /if (!${maEntropy.Find[swEventsInc].Value}) /return FALSE
  /gsay I need WITHER cure!
/return


 **|




|********************************************************************************************
 ** The Darkened Sea (October 2014)


#Event AM4 "#*#Electricity begins to form all around your body#*#"
sub Event_AM4()
  /if (!${maEntropy.Find[swEventsInc].Value}) /return FALSE
  /if (!${Zone.ShortName.Equal[arxmentis_raid]}) /return
  | /echo I have to run away from raid!!!
  /declare tempx float local ${Me.X}
  /declare tempy float local ${Me.Y}
  | =/moveto loc 104 1.06
  /moveto loc -151 284
  /delay 1s ${Me.Moving}
  /call create_timer AM4Timer 13s
  :loop
    /doevents AM1Safe
    /if (!${AM4Timer}) /goto :endtimer
  /if (!${Macro.Return.Find[safe]}) /goto :loop
  :EndTimer
    /moveto loc ${tempy} ${tempx}
    /delay 1s ${Me.Moving}
    /delay 6s !${Me.Moving}
/return


#Event AM1 "#*#You feel faint claws scratching#*#"
#Event AM1 "#*#Fire begins to form around your feet."
sub Event_AM1()
  /if (!${maEntropy.Find[swEventsInc].Value}) /return FALSE
  /if (!${Zone.ShortName.Equal[arxmentis_raid]}) /return
  /declare tempx float local ${Me.X}
  /declare tempy float local ${Me.Y}
  /moveto loc 104 1.06
  /delay 1s ${Me.Moving}
  /call create_timer AM1Timer 13s
  :loop
   /doevents AM1Safe
   /if (!${AM1Timer}) /goto :endtimer
   /if (!${Macro.Return.Find[safe]}) /goto :loop
  :EndTimer
  /moveto loc ${tempy} ${tempx}
  /delay 1s ${Me.Moving}
  /delay 6s !${Me.Moving}
/return


#Event AM1Safe "#*#Dark claws rake at your body#*#"
sub Event_AM1Safe()
/return safe


#Event VitioWeapon "#*#weapons out to his sides#*#"
sub Event_VitioWeapon()
  /if (!${maEntropy.Find[swEventsInc].Value}) /return FALSE
  /declare tempx float local ${Me.X}
  /declare tempy float local ${Me.Y}
  /call create_timer VitioWeaponTimer 15s
  /moveto loc 252.65 -36.91
  /delay 1s ${Me.Moving}
  /delay 10s !${VitioWeaponTimer}
  /moveto loc ${tempy} ${tempx}
  /delay 1s ${Me.Moving}
  /delay 6s !${Me.Moving}
/return


#Event VitioClose   "#*#begins to focus the power of the citadel#*#"
sub Event_VitioClose()
  /if (!${maEntropy.Find[swEventsInc].Value}) /return FALSE
  | /echo Get close to him!
  /declare tempx float local ${Me.X}
  /declare tempy float local ${Me.Y}
  /call create_timer VitioWeaponTimer 10s
  /moveto loc ${Spawn[Praetor Vitio].Y} ${Spawn[Praetor Vitio].X}
  /delay 1s ${Me.Moving}
  /delay 10s !${VitioWeaponTimer}
  /moveto loc ${tempy} ${tempx}
  /delay 1s ${Me.Moving}
  /delay 6s !${Me.Moving}
/return


#Event AMGolemLT "#*#a mortuis vifens gains health as he drains the life force from those standing nearby#*#"
sub Event_AMGolemLT()
  /if (!${maEntropy.Find[swEventsInc].Value}) /return FALSE
  | /echo need to get away from golem!
/return


#Event AMRunAwayBlob "#*#|${Me.CleanName}| has attracted the attention of the frigium letum#*#"
#Event AMRunAwayBlob "#*#a phasma frigasia points a bony finger at you!#*#"
sub Event_AMRunAwayBlob()
  /if (!${maEntropy.Find[swEventsInc].Value}) /return FALSE
  | /echo Got Blob emote!
  /if (${Spawn[Principal Ex-Vicarum Nomia].Distance}>150) /return
  /target clear
  /play runfromboss3 smart
  /delay 2s ${Me.Moving}
  :MovingFromBoss3
   /delay 2s !${AdvPath.Playing}
  /if (${AdvPath.Playing}) /goto :MovingFromBoss3
/return


#Event AMRunAwayFive "#*#Heads up, |${Me.CleanName}|!#*#"
sub Event_AMRunAwayFive()
  /if (!${maEntropy.Find[swEventsInc].Value}) /return FALSE
  | /echo Got Heads up Emote!!
  /declare tempx float local ${Me.X}
  /declare tempy float local ${Me.Y}
  /target clear
  /moveto loc -3.51 -7.61
  /delay 2s ${Me.Moving}
  /delay 12s
  /moveto loc ${tempy} ${tempx}
  /delay 1s
/return


#Event AMRunAwayFiveOne "#*#You feel energy gathering. Only motion can set if free#*#"
sub Event_AMRunAwayFiveOne()
  /if (!${maEntropy.Find[swEventsInc].Value}) /return FALSE
  | /echo Got Gathering Emote!!
  /declare tempx float local ${Me.X}
  /declare tempy float local ${Me.Y}
  /target clear
  /moveto loc 57.78 37.32
  /delay 2s ${Me.Moving}
  /delay 12s !${Me.Moving}
  /moveto loc ${tempy} ${tempx}
  /delay 2s ${Me.Moving}
  /delay 12s !${Me.Moving}
/return
 **|



|********************************************************************************************
 ** The Broken Mirror (November 2015)


#Event CureAkkapanDot "#*#Terrible fumes engulf you. Run from it#*#"
sub Event_CureAkkapanDot()
  /if (!${maEntropy.Find[swEventsInc].Value}) /return FALSE
  /declare _rnd int local 0
  /declare _tempTie bool local ${swSetTie}

  /if (${swSetTie}) {
    /varset _tempTie TRUE
    /varset swSetTie FALSE
  }

  /varset _rnd ${Int[${Math.Calc[${Math.Rand[5]}+1]}]}
  :loopmovetospot
    /if (${_rnd} == 1) /moveto loc 2570 1007
    /if (${_rnd} == 2) /moveto loc 2557 1185
    /if (${_rnd} == 3) /moveto loc 2785 1127
    /if (${_rnd} == 4) /moveto loc 2752 1003
    /if (${_rnd} == 5) /moveto loc 2645 961
    /delay 2s ${Me.Moving}
    /delay 6s !${Me.Moving}
    /if (${Me.Buff[Pulsing Fumes].ID}) /goto :loopmovetospot

  /if (${_tempTie}) /varset swSetTie TRUE
/return



| ** Grummus Raid
| ** ONE OFF
| #Event CurePyroignitionDot "#*#Flames erupt where you stand#*#"
sub Event_CurePyroignitionDot()
  /if (!${maEntropy.Find[swEventsInc].Value}) /return FALSE
  /declare _rnd int local 0
  /declare _tempTie bool local ${swSetTie}

  /if (${swSetTie}) {
    /varset _tempTie ${swSetTie}
    /call Event_SetVar NA swSetTie Movement FALSE
  }

  /varset _rnd ${Int[${Math.Calc[${Math.Rand[3]}+1]}]}
  :loopmovetospot
    /if (${_rnd} == 1) /moveto loc 1321 -1337
    /if (${_rnd} == 2) /moveto loc 1464 -1349
    /if (${_rnd} == 3) /moveto loc 1690 -1494
    /delay 2s ${Me.Moving}
    /delay 12s !${Me.Moving}
    /if (${Me.Buff[Pyroignition].ID}) {
      /delay 1s
      /goto :loopmovetospot
    }

  /if (${_tempTie}) {
    /call Event_SetVar NA swSetTie Movement TRUE
  }
/return


| ** Wither and Decay Raid
#Event DecayRaid "#*#Decay spews a pile of living vomit in |${Me.CleanName}|'s direction.#*#"
sub Event_DecayRaid()
  /if (!${maEntropy.Find[swEventsInc].Value}) /return FALSE
  /declare tempx float local ${Me.X}
  /declare tempy float local ${Me.Y}
  /moveto loc -84.35 -88.09
  /delay 1s ${Me.Moving}
  /delay 10s !${Me.Moving}
  /delay 15s ${Spawn[Living Vomit].Height} == 2.92
  /moveto loc ${tempy} ${tempx}
  /delay 1s ${Me.Moving}
  /delay 10s !${Me.Moving}
/return


| ** Plane of Hate Revisted Raid
#Event InnyDT "#*#You feel a sense of doom upon you#*#"
sub Event_InnyDT()
  /if (!${maEntropy.Find[swEventsInc].Value}) /return FALSE
  | /echo Got Inny DT emote!
  /if (${Spawn[Innoruuk].Distance}>150) /return
  /play CureInnyDT
  /target clear
  :startLoop
  /delay 2s ${Me.Moving}
  :MovingToCureInnyDT
   /delay 2s !${AdvPath.Playing} || !${Me.Song[Doom].ID}
   /delay 1s !${AdvPath.Playing}
   /if (!${Me.Song[Doom].ID}) {
     /echo No longer have DT
     /play off
     /return
   }
  /if (${AdvPath.Playing}) /goto :MovingToCureInnyDT
  /if (${Me.Song[Doom].ID}) /goto :startLoop
/return


| **  Demiplane of Life Raid
| #Event AnashtiSulDT "#*#The power of Sul runs through you, sparking in your hands.#*#"
| sub Event_AnashtiSulDT()
|  /if (!${swEventsInc}) /goto :Event_AnashtiSulDT
|  /echo Got DT emote!
|  :Event_AnashtiSulDT
| /return


| ** Anashti Raid
| ** ONE OFF
sub cure_Anashti_DT()
  /if (!${maEntropy.Find[swEventsInc].Value}) /return FALSE
  /if (${lstClassMelee.Find[${Me.Class.ShortName}]} && ${Me.Buff[Holistic Health].Duration.TotalSeconds} > 15) /return
  /call set_stop
  /declare tempx float local ${Me.X}
  /declare tempy float local ${Me.Y}
  /moveto loc ${Spawn[Lamplight].Y} ${Spawn[Lamplight].X}
  /delay 2s ${Me.Moving}
  /delay 12s !${Me.Moving}
  /moveto loc ${tempy} ${tempx}
  /delay 2s ${Me.Moving}
  /delay 12s !${Me.Moving}
/return


| ** Plane of Health
| ** ONE OFF
sub CureDeathStrikeDT()
| =[Tue May 24 03:45:19 2016] Your Location is 2595.22, 977.81, -35.55
| =[Tue May 24 03:45:25 2016] Your Location is 2579.40, 1083.14, -42.38
| =[Tue May 24 03:45:30 2016] Your Location is 2623.95, 1185.62, -46.88
| =[Tue May 24 03:45:36 2016] Your Location is 2741.26, 1138.35, -42.89
| =[Tue May 24 03:45:42 2016] Your Location is 2732.88, 1013.57, -33.54

  /if (!${Defined[spot]}) {
    /echo Making spot!
    /declare spot int outer 0
  } else {
    /varcalc spot ${spot} + 1
  }
  /if (${spot}==0) {
    /echo Moving from Spot ${spot}
    /moveto loc 2538.74 880.09
    /delay 3s
  } else /if (${spot}==1 || ${spot}==2) {
    /echo Moving from Spot ${spot}
    /moveto loc 2533.38 1149.97
    /delay 3s
  } else /if (${spot}==3 || ${spot}==4) {
    /echo Moving from Spot ${spot}
    /moveto loc 2786.41 1072.15
    /delay 3s
  }
  /call MoveToSpot
/return


sub MoveToSpot()
  /echo Spot is ${spot}
  /if (${spot}==0) {
    /echo Moving from Spot ${spot}
    /moveto loc 2579.40 1083.14
    /delay 3s
  } else /if (${spot}==1) {
    /echo Moving from Spot ${spot}
    /moveto loc 2623.95 1185.62
    /delay 3s
  } else /if (${spot}==2) {
    /echo Moving from Spot ${spot}
    /moveto loc 2741.26 1138.35
    /delay 3s
  } else /if (${spot}==3) {
    /echo Moving from Spot ${spot}
    /moveto loc 2732.88 1013.57
    /delay 3s
  } else /if (${spot}==4) {
    /echo Moving from Spot ${spot}
    /moveto loc 2595.22 977.81
    /delay 3s
    /varset spot 0
  }
/return

 **|



|***
 *  DES: cure debufs for Damsel of Decay
 *  USE: auto
 *  DPS classes if they get Withering Physicality XIII they need to use Distillate of Immunization XV (64 Counters to cycle it twice)
 *  DPS classes if they get Withering Limbs XIII they need to use a Distillate of Immunization XIII (56 Counters to Cycle once) I was Lazy but this could be Level VII through XIII, and not cycle it twice.
 *  Healer classes If they get Withering Physicality XIII they need to use Distillate of Immunization XIII
 *  Healer classes If they get Withering Faith XIII they need to use Distillate of Immunization XV
 *  Tank classes If they get Withering Physicality XIII they need to use Distillate of Immunization XIII

sub CureDamselofDecay()
  /if (!${maEntropy.Find[swEventsInc].Value}) /return FALSE
  ISMEDEAD
  | DPS classes BST,RNG,MON,ROG,ENC,WIZ,MAG,NEC,SHD,PAL,BRD,BST
  /if (${Select[${Me.Class.ShortName},BST,RNG,MON,ROG,ENC,WIZ,MAG,NEC,SHD,PAL,BRD,BST]} && (${Me.Buff[Withering Physicality XIII].ID} || ${Me.Buff[Withering Limbs XIII].ID})) {
    /if (${Me.Buff[Withering Physicality XIII].ID} && ${Me.ItemReady[Distillate of Immunization XIII]}) /call core_cast2 "Distillate of Immunization XIII" item 0 FALSE
    /if (${Me.Buff[Withering Limbs XIII].ID} && ${Me.ItemReady[Distillate of Immunization XIII]}) /call core_cast2 "Distillate of Immunization XIII" item 0 FALSE
  }

  | healers CLR,SHM,DRU
  /if (${Select[${Me.Class.ShortName},CLR,SHM,DRU]} && (${Me.Buff[Withering Physicality XIII].ID} || ${Me.Buff[Withering Faith XIII].ID})) {
    /if (${Me.Buff[Withering Physicality XIII].ID} && ${Me.ItemReady[Distillate of Immunization XIII]}) /call core_cast2 "Distillate of Immunization XIII" item 0 FALSE
    /if (${Me.Buff[Withering Faith XIII].ID} && ${Me.ItemReady[Distillate of Immunization XV]}) /call core_cast2 "Distillate of Immunization XV" item 0 FALSE
  }

  | TANKS WAR,SHD,PAL
  /if (${Select[${Me.Class.ShortName},WAR,SHD,PAL]} && ${Me.Buff[Withering Physicality XIII].ID}) {
    /if (${Me.Buff[Withering Physicality XIII].ID} && ${Me.ItemReady[Distillate of Immunization XIII]}) /call core_cast2 "Distillate of Immunization XIII" item 0 FALSE
  }
/return
 ***|



|***
sub setAkkapanCamp()
| =[Tue May 24 03:45:19 2016] Your Location is 2595.22, 977.81, -35.55
| =[Tue May 24 03:45:25 2016] Your Location is 2579.40, 1083.14, -42.38
| =[Tue May 24 03:45:30 2016] Your Location is 2623.95, 1185.62, -46.88
| =[Tue May 24 03:45:36 2016] Your Location is 2741.26, 1138.35, -42.89
| =[Tue May 24 03:45:42 2016] Your Location is 2732.88, 1013.57, -33.54

  /if (!${Defined[spot]}) {
    /echo Making spot!
    /declare spot int outer 0
  } else {
    /varcalc spot ${spot} + 1
  }
  /call MoveToSpot
/return
 **|







|********************************************************************************************
 ** Ring of Scale (December 12, 2017)
 **|














|********************************************************************************************
 ** Empires of Kunark (November 16, 2017)



| **Talendor (VP 2 group raid)
#Event Nexona "#*#tiny dark flame on |${Me.DisplayName}|#*#"
#Event Nexona "#*#tiny dark flame on #*#, |${Me.DisplayName}|#*#"
#Event Nexona "#*#tiny dark flame on #*#, #*#, and |${Me.DisplayName}|#*#"
sub Event_Nexona()
  /if (!${maEntropy.Find[swEventsInc].Value}) /return FALSE
  ISMEDEAD
  /echo Got SILENCE Emote!!
  /declare tempx float local ${Me.X}
  /declare tempy float local ${Me.Y}
  /target clear
  /moveto loc 229.40 -956.84
  /delay 2s ${Me.Moving}
  /delay 15s
  /moveto loc ${tempy} ${tempx}
  /delay 1s
/return


 **|





|********************************************************************************************
 ** The Burning Lands (December 11, 2018)
 **|


| POFIRE
| Guardian of Doomfire
#Event Guardian_of_Doomfire "#*#Guardian of Doomfire shouts, 'I doom both you, #1# and you, #2#!'#*#"
sub Event_Guardian_of_Doomfire(string Line, string PersonOne, string PersonTwo)
  /if (!${maEntropy.Find[swEventsInc].Value}) /return FALSE
  OUT GUARDIAN EMOTE
  /nav recordwaypoint tmpcamp "tmp camp"
  /delay 1s
  OUT RUNNING AWAY
  /if (${PersonOne.Equal[${Me.DisplayName}]}) /nav loc -1195 -266 -198
  /if (${PersonTwo.Equal[${Me.DisplayName}]}) /nav loc -1043 -180 -190
  /delay 25s
  OUT RETURNING TO CAMP
  /nav waypoint tmpcamp
/return


| General Reparm
#Event Reparm "|${Me.DisplayName}| is engulfed in expanding flames."
sub Event_Reparm()
  /if (!${maEntropy.Find[swEventsInc].Value}) /return FALSE
  OUT REPARM EMOTE
  /nav recordwaypoint tmpcamp "tmp camp"
  /delay 1s
  OUT RUNNING AWAY
  /nav loc -920 -1500 -185
  /delay 25s
  OUT RETURNING TO CAMP
  /nav waypoint tmpcamp
/return





|********************************************************************************************
 ** Torment of Velious (December 18, 2019)
 **|



| Griklor the Restless roars and points at Name.
| is the trigger emote to stick
| there are two success messages to end stick and return to where you started would be perfect
| Griklor the Restless tries to control his mount as shards of deadly ice strike it.
| Griklor's griffon's wings are shredded by deadly ice and Griklor falls to the ground.
| are the two success messages

#Event Griklor "#*#Griklor the Restless roars and points at |${Me.DisplayName}|.#*#"
sub Event_Griklor()
  /if (!${maEntropy.Find[swEventsInc].Value}) /return FALSE
  OUT GRIKLOR EMOTE
  /nav recordwaypoint tmpcamp "tmp camp"
  /delay 1s
  /declare x int local 0
  /for x 1 to 26
    /if (${Me.Dead}) /return
    OUT FOLLOWING GRIKLOR
    /if (!${Navigation.Active} && ${Spawn[Griklor the Restless].Distance} > 5) {
      /nav id ${Spawn[Griklor the Restless].ID} |log=${maEntropy.Find[swNavLogSpew].Value}
    }
    /delay 4
    /doevents Grik_Success
  /next x
  OUT RETURNING TO CAMP
  /nav waypoint tmpcamp
  /return
  
#Event Grik_Success "#*#Griklor the Restless tries to control his mount as shards of deadly ice strike it.#*#"
sub Event_Grik_Success
/return


| Icebound Avatar mission
#Event IceAoW "#*#The ice encrusted Avatar of War shouts that |${Me.DisplayName}| must bend the knee#*#"
sub Event_IceAoW()
  /if (!${maEntropy.Find[swEventsInc].Value}) /return FALSE

  /if (${Me.State.NotEqual[duck]}) {
    /keypress DUCK
  }

  /delay 11s

  /if (${Me.State.Equal[duck]}) {
    /keypress DUCK
  }

/return TRUE


| Icebound Avatar mission
#Event IceAoWUnduck "#*#The Avatar of War nods, accepting the subservience of those that gave it#*#"
sub Event_IceAoWUnduck()
  /if (!${maEntropy.Find[swEventsInc].Value}) /return FALSE

  /if (!${Defined[AoW_Event_Acceptance]}) {
    /declare  AoW_Event_Acceptance bool outer
  }
  /varset AoW_Event_Acceptance TRUE
}
/return TRUE


#Event IceAoW "#*#The ice encrusted Avatar of War shouts that |${Me.DisplayName}| must bend the knee#*#"
sub Event_IceAoW()

  /if (!${maEntropy.Find[swEventsInc].Value}) /return FALSE
  /call clear_combat
  /call set_stop ${maDebug.Find[stop].Value} SILENT

  /echo ${Me.Animation} ?? 24)
  /while (!${Select[${Me.Animation},24]} && !${Me.Ducking}) {
    /keypress DUCK
    /delay 1s
    /if (${Select[${Me.Animation},24]} || ${Me.Ducking}) {
      /break
    }
  }

  /delay 10s
  /doevents IceAoWUnduck

  /if (!${Me.Dead} && ${AoW_Event_Acceptance}) {
    /varset AoW_Event_Acceptance FALSE
    /if (${Select[${Me.Animation},24]} || ${Me.Ducking}) {
     /keypress DUCK
    }
  }

/return TRUE



| Till Death do us Part
#Event Tofs2 "#*#The usher requires the attention of #1##*#"
| #Event Tofs2 "#*#Honored guest. I require your presence.#*#"
sub Event_Tofs2(string Line)
  /if (!${maEntropy.Find[swEventsInc].Value}) {
    /return FALSE
  }

  /if (!${Line.Find[${Me.CleanName}]}) {
    | /echo Not My Time
    /return FALSE
  }

  /call clear_combat ${maDebug.Find[clear].Value} SILENT
  /call set_stop ${maDebug.Find[clear].Value} SILENT

  OUT ive been picked
  /if (!${Spawn[npc a golem usher].ID}) {
    /return FALSE
  }

  /nav id ${Spawn[npc a golem usher].ID} |log=off distance=20
  /delay 1s ${Navigation.Active}
  /delay 5s !${Navigation.Active}

  :target
  CHECKDEAD
  /target id ${Spawn[npc a golem usher].ID}
  /if (${Target.DisplayName.Equal[a golem usher]}) {
    /say show
  } else {
    /goto :target
  }

  /delay 2s

  /if (${Me.Song[Groom's Side].ID}) {
    OUT You are heading to the Bride's camp
    /nav locyxz 420 -110 ${Me.Z} |log=${maEntropy.Find[swNavLogSpew].Value} distance=10
  } else /if (${Me.Song[Bride's Side].ID}) {
    OUT You are heading to the Groom's camp
    /nav locyxz 411 -211 ${Me.Z} |log=${maEntropy.Find[swNavLogSpew].Value} distance=10
  }

/return TRUE






|********************************************************************************************
 ** Claws of Veeshan (2020)
 **|
 
 
| Aaryonar
| yxz

| purple
| 1931, -1614, 160

| brown
| 1708, -1718, 159

| fire
| 1656, -1926, 158

| white
| 1812, -1838, 155



| Tantor

#Event Tantor "Tantor roars, pointing its trunk at |${Me.DisplayName}|."
Sub Event_Tantor()
  /if (!${maEntropy.Find[swEventsInc].Value}) /return FALSE
  OUT TANTOR EMOTE
  /call chase_tail
  OUT Returning to our camp
  /nav waypoint tmpcamp
/return


sub chase_tail()
  /nav recordwaypoint tmpcamp "tmp camp"
  /delay 1s
  OUT RUNNING AWAY FROM TANTOR
  /nav loc -443 -1091 -43
  /delay 2s
  /while (${Me.Moving} && !${Me.Stunned}) {
    /delay 1s
  }
  /declare _count int local 0
  /face fast heading 270
  /circle on 200
  /delay 2s
  /while (${Me.Moving} && ${_count} < 90) {
    /delay 1s
    /varcalc _count ${_count} + 1
    /doevents Tantor_Success
    /if (${Bool[${Macro.Return}]}) /break   
  }
/return
  

#Event Tantor_Success "Tantor gives up the chase." 
Sub Event_Tantor_Success
/return TRUE
  

#Event TrunkRock "Tantor grabs a rock with its trunk and turns toward |${Me.DisplayName}|."
sub Event_TrunkRock()
	/if (!${maEntropy.Find[swEventsInc].Value}) /return FALSE
	OUT ROCK EMOTE
	/nav recordwaypoint tmpcamp "tmp camp"
	/delay 1s
	OUT RUNNING AWAY
	/nav loc -824 -1079 -29
	/delay 7s
  /if (${Me.State.NotEqual[duck]}) {
    OUT DUCK, DUCK, DUCK, GOOSE
	  /keypress DUCK
	}
	/delay 18s
	/if (${Me.State.Equal[duck]}) {
    /keypress DUCK
	  OUT UN-GOOSING
	}
	OUT RETURNING TO CAMP
	/nav waypoint tmpcamp
/return




|********************************************************************************************
 ** Torment of Luclin (2021)
 **|
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
|********************************************************************************************
 * this is the start of the event bind. these are custom events done by expansion.
 * these will not work for everyone. hence the custom part.
 * note: seriously, i just explained it
 * use: varries... suck it
 ***|
#Bind event /event
sub Bind_event(string _action, string _option)


  /if (${_action.Left[1].Equal[-]}) {
    /lua run HELP event ${_action.Right[1]}
    /return TRUE
  }

  | we build the bind outputs on the fly for this control
  /invoke ${maControl.Clear}


  | some default never changing options
  /if (${_action.Equal[ui]}) {
    /lua run ${MacroQuest.Path[macros]}\entropy\lua\event.lua
    /lua run ${MacroQuest.Path[macros]}\entropy\lua\eventbuttons.lua
    /return TRUE
  } else /if (${_action.Equal[banner]}) {
    /nav id ${Spawn[banner].ID}
    /return TRUE
  } else /if (${_action.Equal[campfire]}) {
    /nav id ${Spawn[campfire].ID}
    /return TRUE
  }

  /invoke ${maControl.Add[ui,option]}
  /invoke ${maControl.Add[banner,option]}
  /invoke ${maControl.Add[campfire,option]}


| ---  start of ToL events
  /if (${Zone.ShortName.Equal[maidentwo_raid]}) {    
 
    | gorgana huge chain aura-
    /if (${_action.Equal[hide]}) {
      /if (${Me.Snared}) {
        /call Bind_control_gtfo FORCE
      }
      /declare _pathlength int local 99999
      /declare _swap string local
      /declare _liloc listiterator local
      /vardata _liloc ls28MaidenHideLocYX.First.Clone
      /while (!${_liloc.IsEnd}) {        
        /if (${Navigation.PathLength[locyx ${_liloc.Value}]} < ${_pathlength}) {
          /varset _pathlength ${Navigation.PathLength[locyx ${_liloc.Value}]}
          /varset _swap ${_liloc.Value}
        }
        /invoke ${_liloc.Advance}
      }
      /nav locyx ${_swap}
      /delay 5
      /while (${Me.Moving}) {
       /delay 1
      }
      /return TRUE
    }
    
    | gorgana move to purple arura    
    /if (${_action.Equal[aura]}) {
      /declare _aura spawn local
      /vardata _aura NearestSpawn[${maEvent.Find[st28MaidenAura].Value}]
      /if (!${_aura.ID}) {
        /return FALSE
      }
      /nav id ${_aura.ID}
      /return TRUE
    }

    /invoke ${maControl.Add[hide,option]}
    /invoke ${maControl.Add[aura,option]}
    
    
  | Bloodfalls Close the Gate raid
  } else /if (${Zone.ShortName.Equal[bloodfalls_raid]}) {
    
    | hide from T emote
    /if (${_action.Equal[hide]}) {
      /nav locyx ${maEvent.Find[st28BloodfallsHideLocYX].Value}
      /return TRUE
    }
    
    | various cures
    /if (${_action.Equal[cure]}) {
    
      | Bugs :: damage shield
      /if (${Me.Buff[Buggs].ID}) {
        /call remove_spa ${_debug} 59
        /if (${cast_data[${_debug}, "${maEvent.Find[st28BloodfallsBuggs].Value}"]}) {
          /call cast ${_debug} ${Me.ID} FALSE
        }
      | Chased! :: Sow
      } else /if (${Me.Buff[Chased].ID}) {
        /call remove_spa ${_debug} 3
        /if (${cast_data[${_debug}, "${maEvent.Find[st28BloodfallsChased].Value}"]}) {
          /call cast ${_debug} ${Me.ID} FALSE
        }
      | Drowning :: EB
      } else /if (${Me.Buff[Drowning].ID}) {
        /call remove_spa ${_debug} 57
        /if (${cast_data[${_debug}, "${maEvent.Find[st28BloodfallsDrowning].Value}"]}) {
          /call cast ${_debug} ${Me.ID} FALSE
        }
      | Falling :: Levi
      } else /if (${Me.Buff[Falling].ID}) {
        /call remove_spa ${_debug} 57
        /if (${cast_data[${_debug}, "${maEvent.Find[st28BloodfallsFalling].Value}"]}) {
          /call cast ${_debug} ${Me.ID} FALSE
        }
      | Monster :: invis
      } else /if (${Me.Buff[Monster].ID}) {
        /if (${cast_data[${_debug}, "${maEvent.Find[st28BloodfallsMonster].Value}"]}) {
          /call cast ${_debug} ${Me.ID} FALSE
        }
      | Curse of Shadows :: dispell
      } else /if (${Me.Buff[Curse of Shadows].ID}) {
        /if (${cast_data[${_debug}, "${maEvent.Find[st28BloodfallsCurse].Value}"]}) {
          /call cast ${_debug} ${Me.ID} FALSE
        }
      }
      /return TRUE
    }
    
    /invoke ${maControl.Add[hide,option]}
    /invoke ${maControl.Add[cure,option]}
 
 
  | Aetn Vex Thal Raid
  } else /if (${Zone.ShortName.Equal[vexthaltwo_raid]}) {
 
    | 3 stage cure
    /if (${_action.Equal[cure]}) {
      /call cast_cycle_cure ${_debug} ${Me.ID} FALSE FALSE
    | run to room for emote
    } else /if (${_action.Equal[hide]}) {
      /nav locyx ${maEvent.Find[st28AetnHideLocYX].Value}
    }

    /invoke ${maControl.Add[hide,option]}
    /invoke ${maControl.Add[cure,option]}

    
  | Oubliette of Light Raid
  } else /if (${Zone.ShortName.Equal[basilica_raid]}) {

    | center of purple aura    
    /if (${_action.Equal[aura]}) {
      /declare _aura spawn local
      /vardata _aura NearestSpawn[${maEvent.Find[st28OublietteAura].Value}]
      /if (!${_aura.ID}) {
        /return FALSE
      }
      /nav id ${_aura.ID}
      /return TRUE
    }

    /invoke ${maControl.Add[aura,option]}


  | Maidens Eye
  } else /if (${Zone.ShortName.Equal[umbraltwo_raid]}) {

    /if (${_action.Equal[home]}) {
      /nav locyx -165 129

    | hide hide for virul emote (the green one) on the darth vader event
    } else /if (${_action.Equal[hide]}) {

      /declare _pathlength int local 99999
      /declare _swap string local
      /declare _liloc listiterator local
      /vardata _liloc ls28UmbralHideLocYX.First.Clone
      /while (!${_liloc.IsEnd}) {        
        /if (${Navigation.PathLength[locyx ${_liloc.Value}]} < ${_pathlength}) {
          /varset _pathlength ${Navigation.PathLength[locyx ${_liloc.Value}]}
          /varset _swap ${_liloc.Value}
        }
        /invoke ${_liloc.Advance}
      }
      /nav locyx ${_swap}
      /delay 5
      /while (${Me.Moving}) {
       /delay 1
      }
      /return TRUE
    }

    /invoke ${maControl.Add[home,option]}
    /invoke ${maControl.Add[hide,option]}

  }
  
 
 
 | ---  start of NoS events
  /if (${Zone.ShortName.Equal[idontexistyet]}) {    

  | Shar Vhal
  | Padul Caverns
  








  } 
  

  
  
| ---  start of Anni events
  | Karnor anni raid
  /if (${Zone.ShortName.Equal[karnor_raid]}) {
    
    | ae mez run to east room away from raid
    /if (${_action.Equal[hide]}) {
      /nav locyx ${maEvent.Find[stKarnorHideLocYX].Value}
    }
    
    /invoke ${maControl.Add[hide,option]}
    
  }
  

  /invoke ${set_control_output[FALSE, maEvent, "event", maControl]}
    
/return TRUE
 
 
 