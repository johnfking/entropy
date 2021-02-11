|***
 * entropy
 * class.clr.inc
 ***|



|***
 * note: primary initilization of all zerker non shared data
 * use: not for you
 ***|
sub class_init(bool _debug)
  DEBUG \atclass_init\ax()

  /if (${maDebug.Find[init].Value}) {
    /declare _boottimestart int local ${MacroQuest.Running}
  }

  /call set_data ${_debug} maData stSpire                             map outer "Spire of the Juggernaut"
  /call set_data ${_debug} maData stSynergy                           map outer "Fury's Synergy"
  /call set_data ${_debug} maData stFade                              map outer "Self Preservation"

  /call set_data_map ${_debug} maChr stRest                           PREGEN|1                      "${auxna} name of stamina rest disc"
  /call set_data_map ${_debug} maChr swHiatus                         FALSE                         "${auxsw} use hiatus rest disc"

  /call set_data_map ${_debug} maChr stRageVolley                     PREGEN|1                      "${auxna} "
  /call set_data_map ${_debug} maChr stCryOfChaos                     PREGEN|1                      "${auxna} "
  /call set_data_map ${_debug} maChr stInspiredAnger                  PREGEN|1                      "${auxna} "
  /call set_data_map ${_debug} maChr stOverpoweringFrenzy             PREGEN|1                      "${auxna} "
  /call set_data_map ${_debug} maChr stUnsettlingScream               PREGEN|1                      "${auxna} "
  /call set_data_map ${_debug} maChr stAxeThrow                       PREGEN|1                      "${auxna} "
  /call set_data_map ${_debug} maChr stDivertiveStrike                PREGEN|1                      "${auxna} "
  /call set_data_map ${_debug} maChr stSharedBloodlust                PREGEN|1                      "${auxna} "
  /call set_data_map ${_debug} maChr stBattleFrenzy                   PREGEN|1                      "${auxna} "
  /call set_data_map ${_debug} maChr stCleavingRage                   PREGEN|1                      "${auxna} "
  /call set_data_map ${_debug} maChr stKickintheShins                 PREGEN|1                      "${auxna} "
  /call set_data_map ${_debug} maChr stAxeofRalos                     PREGEN|1                      "${auxna} "
  /call set_data_map ${_debug} maChr stVengefulFlurry                 PREGEN|1                      "${auxna} "
  /call set_data_map ${_debug} maChr stViciousSpiral                  PREGEN|1                      "${auxna} "
  /call set_data_map ${_debug} maChr stCryHavoc                       PREGEN|1                      "${auxna} "
  /call set_data_map ${_debug} maChr stAugmentedFrenzy                PREGEN|1                      "${auxna} "
  /call set_data_map ${_debug} maChr stFesteringRage                  PREGEN|1                      "${auxna} "
  /call set_data_map ${_debug} maChr stResolve                        PREGEN|1                      "${auxna} "
  /call set_data_map ${_debug} maChr stDichotomic                     PREGEN|1                      "${auxna} "
  /call set_data_map ${_debug} maChr stDisconcerting                  PREGEN|1                      "${auxna} "
  /call set_data_map ${_debug} maChr stSappingStrikes                 PREGEN|1                      "${auxna} "
  /call set_data_map ${_debug} maChr stPhantomAssailant               PREGEN|1                      "${auxna} "
  /call set_data_map ${_debug} maChr stOpenWound                      PREGEN|1                      "${auxna} "
  | /call set_data_map ${_debug} maChr stConditionedRetaliation	        PREGEN|1			    		        "${auxna} "

  | class order lists
  /call set_data_list ${_debug} maChr lsOrderClassBase                EMPTY                                     "${auxli} class base loop start"
  /call set_data_list ${_debug} maChr lsOrderClassPre                 EMPTY                                     "${auxli} class pre combat"
  /call set_data_list ${_debug} maChr lsOrderClass                    item|melee|burn|misc|debuff|dot|nuke      "${auxli} class active combat order"
  /call set_data_list ${_debug} maChr lsOrderClassPost                staminarecover                            "${auxli} class post combat"

  | AAs
  /call set_data_map ${_debug} maChr stAAUnity                        FALSE                         "${auxna} class aa unity buff"
  /call set_data_map ${_debug} maChr stAADrawntoBlood                 PREGEN|1                      "${auxna} Drawn to Blood aa"
  /call set_data_map ${_debug} maChr stAABattleLeap                   PREGEN|1                      "${auxna} Battle Leap aa"
  /call set_data_map ${_debug} maChr stAARecklessAbandon              PREGEN|1                      "${auxna} Reckless Abandon aa"
  /call set_data_map ${_debug} maChr stAABindingAxe                   PREGEN|1                      "${auxna} Binding Axe aa"
  /call set_data_map ${_debug} maChr stAARampage                      PREGEN|1                      "${auxna} Rampage aa"
  /call set_data_map ${_debug} maChr stAAVehementRage                 PREGEN|1                      "${auxna} Vehement Rage aa"
  /call set_data_map ${_debug} maChr stAACommunionofBlood             PREGEN|1                      "${auxna} Communion of Blood aa"
  /call set_data_map ${_debug} maChr stAABraxisHowl                   PREGEN|1                      "${auxna} Braxi's Howl aa"
  /call set_data_map ${_debug} maChr stAACryofBattle                  PREGEN|1                      "${auxna} Cry of Battle aa"
  /call set_data_map ${_debug} maChr stAAFuriousRampage               PREGEN|1                      "${auxna} Furious Rampage aa"
  /call set_data_map ${_debug} maChr stAASilentStrikes                PREGEN|1                      "${auxna} Silent Strikes aa"
  /call set_data_map ${_debug} maChr stAAFocusedFuriousRampage        PREGEN|1                      "${auxna} Focused Furious Rampage aa"
  /call set_data_map ${_debug} maChr stAAJuggernautSurge              PREGEN|1                      "${auxna} Juggernaut Surge aa"
  /call set_data_map ${_debug} maChr stAABlindingFury                 PREGEN|1                      "${auxna} Blinding Fury aa"
  /call set_data_map ${_debug} maChr stAABloodPact                    PREGEN|1                      "${auxna} Blood Pact aa"
  /call set_data_map ${_debug} maChr stAADesperation                  PREGEN|1                      "${auxna} Desperation aa"
  /call set_data_map ${_debug} maChr stAABloodfury                    PREGEN|1                      "${auxna} Bloodfury aa"
  /call set_data_map ${_debug} maChr stAAUntamedRage                  PREGEN|1                      "${auxna} Untamed Rage aa"
  /call set_data_map ${_debug} maChr stAARecklessAbandon              PREGEN|1                      "${auxna} Reckless Abandon aa"
  /call set_data_map ${_debug} maChr DistractionAttack                PREGEN|1                      "${auxna} Distraction Attack aa"
  /call set_data_map ${_debug} maChr stAASavageSpirit									PREGEN|1											"${auxna} Savage Spirit aa"
  /call set_data_map ${_debug} maChr stAABloodSustenance							PREGEN|1											"${auxna} Blood Sustenance aa"

  /if (${maDebug.Find[init].Value}) {
    OUT \agset_${Me.Class.ShortName}\ax${sep}\a-w${Math.Calc[((${MacroQuest.Running}-${_boottimestart}) / 10) / 60]}s\ax
    /mqp
  }

/return TRUE



|***
 * note: i have one purpose,  otherwise i'm useless
 * use:
 ***|
sub class_main(bool _debug)
  DEBUG \atclass_main\ax()

  GETINPUT
  ISMEDEAD
  CHECKTIE
  CHECKREZ

  /if (AUTO && !ENGINE3) {
    /call class_cycle lsOrderClassBase
  } else /if (AUTO && ENGINE3) {
    /if (${SubDefined[e3_pre_${Me.Class.ShortName}]}) {
      /call e3_pre_${Me.Class.ShortName} ${maDebug.Find[e3].Value}
    }
  }

  /while (${check_combat_status[${maDebug.Find[status].Value}]}) {
    /call class_cycle lsOrderClassPre
    /if (!${check_class_loop[${maDebug.Find[status].Value}]}) /return FALSE

    /if (VALID && !ENGINE3) {
      /call do_sneak_attack ${maDebug.Find[class].Value} GETASSIST
      /call set_combat_control ${maDebug.Find[decision].Value} GETASSIST
      /call class_cycle lsOrderClass
    } else /if (VALID && ENGINE3) {
      /if (${SubDefined[e3_combat_${Me.Class.ShortName}]}) {
        /call e3_combat_${Me.Class.ShortName} ${maDebug.Find[e3].Value}
      }
    }

    /if (AUTO && !ENGINE3) {
      /call class_cycle lsOrderClassPost
    } else /if (AUTO && !ENGINE3) {
      /if (${SubDefined[e3_post_${Me.Class.ShortName}]}) {
        /call e3_post_${Me.Class.ShortName} ${maDebug.Find[e3].Value}
      }
    }
  }

/return TRUE



|***
 * note: suprize but slap?
 * use: designed for a first attack
 ***|
sub do_sneak_attack(bool _debug, int _tmpID)
  DEBUG \atdo_sneak_attack\ax(\a-w${_tmpID}\ax)

  /if (${Select[${Me.CombatState},combat,active,debuffed]}) {
    /return FALSE
  }

  /if (!${is_ready[${_debug}, "${maChr.Find[stKickintheShins].Value}"]}) {
    /return FALSE
  }

  CHECKEXIT

  /if (${Target.Distance} <= 49 && ${Target.Distance} <= ENV) {
    /call voodoo ${_debug} "${maChr.Find[stKickintheShins].Value}" ${_tmpID} FALSE
  }

/return TRUE



|***
 * note:
 * use:
 ***|
sub misc_combat(bool _debug, int _tmpID)
  DEBUG \atmisc_combat\ax(\a-w${_tmpID}\ax)

  ISMEDEAD
  /if (${target[isdead, ${_tmpID}]}) {
    /return FALSE
  }

  CHECKEXIT
  GETINPUT
  CHECKTIE
  CHECKDEFENSE
  BUFFNOW
  BUFFTEMP

  /if (AUTO && MELEE) {
    /call check_facing ${maDebug.Find[face].Value} ${_tmpID} FORCE
  }

  /if (SWARM || PET) {
    /call send_pet ${maDebug.Find[sic].Value} ${_tmpID}
  }

  /if (ENGINE2) {
    /call cast_cycle_miscdps ${maDebug.Find[agro].Value} ${_tmpID}
    /return TRUE
  }

  | Self Buffs to keep up at all times (Sapping Strikes, up 2m of every 2.5m)
  /if (${is_ready[${_debug}, "${maChr.Find[stSappingStrikes].Value}"]}) {
    /call voodoo ${_debug} "${maChr.Find[stSappingStrikes].Value}" 0 FALSE
  }
  
  | Self buffs to keep up at all times (Magnified Frenzy, requires target)
  /if (${is_ready[${_debug}, "${maChr.Find[stAugmentedFrenzy].Value}"]} && !${Me.Buff[${maChr.Find[stAugmentedFrenzy].Value}].ID}) {
  	/if (${Me.PctHPs} > 90) {
      /call voodoo ${_debug} "${maChr.Find[stAABloodfury].Value}" 0 FALSE
      /delay 1
      /if (${Me.PctHPs} > 90) {
      		/call voodoo ${_debug} "${maChr.Find[stAABloodfury].Value}" 0 FALSE
      		/delay 1
      }
    }
		/call voodoo ${_debug} "${maChr.Find[stAugmentedFrenzy].Value}" 0 FALSE
	}
	

  | Use Drawn to Blood if Frenzied Resolve active
  /if (${Me.ActiveDisc[Frenzied Resolve].ID} && ${Spawn[id ${_tmpID}].Distance} > ${Spawn[id ${_tmpID}].MaxRangeTo}*.8 && ${Spawn[id ${_tmpID}].Distance} < 60) {
    /if (${is_ready[${_debug}, "${maChr.Find[stAADrawntoBlood].Value}"]}) {
      /call voodoo ${_debug} "${maChr.Find[stAADrawntoBlood].Value}" 0 FALSE
    }
  }
  
  | Get under 90% with Bloodfury
  | /if (${Me.PctHPs} > 90 && !${Me.Song[${maChr.Find[stOpenWound].Value}].ID}) {
  |  /if (${is_ready[${_debug}, "${maChr.Find[stAABloodfury].Value}"]}) {
  |    /call voodoo ${_debug} "${maChr.Find[stAABloodfury].Value}" 0 FALSE
  |  }
  | }

  | Vindicating Frenzy
  /if (${is_ready[${_debug}, "${maChr.Find[stOverpoweringFrenzy].Value}"]}) {
  	/if (${Me.PctHPs} > 90) {
      /call voodoo ${_debug} "${maChr.Find[stAABloodfury].Value}" 0 FALSE
      /delay 1
      /if (${Me.PctHPs} > 90) {
      		/call voodoo ${_debug} "${maChr.Find[stAABloodfury].Value}" 0 FALSE
      		/delay 1
      }
    }
    /call voodoo ${_debug} "${maChr.Find[stOverpoweringFrenzy].Value}" ${_tmpID} FALSE
  }
  
  | Vindicating Volley
  /if (${is_ready[${_debug}, "${maChr.Find[stRageVolley].Value}"]}) {
    /call voodoo ${_debug} "${maChr.Find[stRageVolley].Value}" ${_tmpID} FALSE
  }

  | Axe of Derakor
  /if (${is_ready[${_debug}, "${maChr.Find[stAxeofRalos].Value}"]}) {
    /call voodoo ${_debug} "${maChr.Find[stAxeofRalos].Value}" ${_tmpID} FALSE
  }

  | Vindicating Axe Throw
  /if (${is_ready[${_debug}, "${maChr.Find[stAxeThrow].Value}"]}) {
    /call voodoo ${_debug} "${maChr.Find[stAxeThrow].Value}" ${_tmpID} FALSE
  }
  
  | Shared Atavism
  /if (!${Me.Song[${maChr.Find[stSharedBloodlust].Value}].ID}) {
    /if (${is_ready[${_debug}, "${maChr.Find[stSharedBloodlust].Value}"]}) {
      /call voodoo ${_debug} "${maChr.Find[stSharedBloodlust].Value}" 0 FALSE
    }
  }
  
  | Seething Rage
  /if (${is_ready[${_debug}, "${maChr.Find[stFesteringRage].Value}"]}) {
    /call voodoo ${_debug} "${maChr.Find[stFesteringRage].Value}" 0 FALSE
  }
  
  | swarm pet
  /if (SWARM) {
    /if (${is_ready[${_debug}, "${maChr.Find[stPhantomAssailant].Value}"]}) {
      /call voodoo ${_debug} "${maChr.Find[stPhantomAssailant].Value}" ${_tmpID} FALSE
    }
  }
  
  | Binding Axe
  /if (${is_ready[${_debug}, "${maChr.Find[stAABindingAxe].Value}"]}) {
    /call voodoo ${_debug} "${maChr.Find[stAABindingAxe].Value}" ${_tmpID} FALSE
  }

  /if (AE# && ${Me.Song[Strike of Savagery].ID}) {
    /if (${is_ready[${_debug}, "${maChr.Find[stAARampage].Value}"]}) {
      /call voodoo ${_debug} "${maChr.Find[stAARampage].Value}" 0 FALSE
    }
  }
  
  | Vehement Rage
  /if (!${Me.Song[${maChr.Find[stAARecklessAbandon].Value}].ID}) {
    /if (${is_ready[${_debug}, "${maChr.Find[stAAVehementRage].Value}"]}) {
      /call voodoo ${_debug} "${maChr.Find[stAAVehementRage].Value}" 0 FALSE
    }
  }

  | endurance regeneration
  /if (${Me.PctHPs} > 75 && ${Me.PctEndurance} < 50) {
    /if (${is_ready[${_debug}, "${maChr.Find[stAACommunionofBlood].Value}"]}) {
      /call voodoo ${_debug} "${maChr.Find[stAACommunionofBlood].Value}" 0 FALSE
    }
  }

  | Braxi and Dicho
  /if (${is_ready[${_debug}, "${maChr.Find[stAABraxisHowl].Value}"]}) {
    | Zerker dicho
    /if (${is_ready[${_debug}, "${maChr.Find[stDichotomic].Value}"]} && ${Me.CurrentEndurance} > ${Spell[${maChr.Find[stDichotomic].Value}].EnduranceCost}*2) {
      /call voodoo ${_debug} "${maChr.Find[stAABraxisHowl].Value}" 0 FALSE
      /call voodoo ${_debug} "${maChr.Find[stDichotomic].Value}" ${_tmpID} FALSE
    }
  }

  | Vicious Revolution
  /if (AE#) {
    /if (${is_ready[${_debug}, "${maChr.Find[stViciousSpiral].Value}"]}) {
      /call voodoo ${_debug} "${maChr.Find[stViciousSpiral].Value}" 0 FALSE
    }
  }
	
  | Rabbit Punch
  | /if (${Target.Distance} <= 49 && ${Target.Distance} <= ENV) {
  |   /call voodoo ${_debug} "${maChr.Find[stKickintheShins].Value}" ${_tmpID} FALSE
  | }
  
  | to agro or not ?
  /if (!AGRO) {
    /if (${is_ready[${_debug}, "${maChr.Find[DistractionAttack].Value}"]} && ${Me.PctAggro} > 96) {
      /call voodoo ${_debug} "${maChr.Find[DistractionAttack].Value}" 0 FALSE
    }
    /if (${is_ready[${_debug}, "${maChr.Find[stDivertiveStrike].Value}"]} && ${Me.PctAggro} > 80) {
      /call voodoo ${_debug} "${maChr.Find[stDivertiveStrike].Value}" ${_tmpID} FALSE
    }
  }


/return TRUE



|***
 * note: if you are using this.. and people find out.. you will get laughed at. zerkers are glass cannons..
 * use: /heal self
 ***|
sub check_heal(bool _debug, string _type)
  DEBUG \atcheck_heal\ax(\a-w${_type}\ax)

  CHECKTIE
  ISMEDEAD
  CHECKEXIT
  CHECKREZ
  CHECKFADE

  SETHUD "heal ${_type}"

  /if (${Me.PctHPs} > ${maHeal.Find[stHealPoint${Me.Class.ShortName}].Value}*.PCTHO) /return FALSE

  /call cast_cycle_heal ${_debug} ${hurtID} healsingle

  SETHUD EMPTY

/return TRUE



|***
 * note: Berzerker burn routines
 * use: if you're not burning. you are doing it wrong..
 *  ramp selection
 *  savage spirit
 *  berserking
 *  jugg surge
 *  spire
 *  if no ancient blinding
 *  blood pact
 *  mashing skills
 *  no berserking use cleaving anger and alt act 3710
 *  no cleaving use vengful and glyph
 *  no vengful use frenzied and drawn
 ***|
sub burn(bool _debug, int _tmpID)
  DEBUG \atburn\ax(\aw${_tmpID}\ax)

  ISMEDEAD
  /if (${target[isdead, ${_tmpID}]}) {
    /return FALSE
  }

  CHECKEXIT
  GETINPUT
  CHECKTIE
  CHECKDEFENSE

  /if (SWARM || PET) {
    /call send_pet ${maDebug.Find[sic].Value} ${_tmpID}
  }
  /if (AUTO && MELEE) {
    /call check_facing ${maDebug.Find[face].Value} ${_tmpID} FORCE
  }

  /if (ENGINE2) {
    /call cast_cycle_burn ${maDebug.Find[burn].Value} ${_tmpID}
    /return TRUE
  }

|**
/if (!${is_ready[${_debug}, "${maChr.Find[stInspiredAnger].Value}"]} && !${is_ready[${_debug}, "${maChr.Find[stVengefulFlurry].Value}"]} && !${is_ready[${_debug}, "${maChr.Find[stCleavingRage].Value}"]} && !${is_ready[${_debug}, "${maChr.Find[stResolve].Value}"]} && !${Me.Buff[${maChr.Find[stResolve].Value}].ID}) {
    /if (${is_ready[${_debug}, "${maChr.Find[stDisconcerting].Value}"]}) {
      /call voodoo ${_debug} "${maChr.Find[stDisconcerting].Value}" 0 FALSE
    }
  }

  /if (${is_ready[${_debug}, "${maChr.Find[stInspiredAnger].Value}"]} || ${is_ready[${_debug}, "${maChr.Find[stVengefulFlurry].Value}"]} || ${is_ready[${_debug}, "${maChr.Find[stCleavingRage].Value}"]} || ${is_ready[${_debug}, "${maChr.Find[stResolve].Value}"]}) {
    :loopmakestop
    /delay 5
    /if (${Me.ActiveDisc.Name.Equal[${maChr.Find[stDisconcerting].Value}]}) /stopdisc
    /delay 5
    /if (${Me.ActiveDisc.Name.Equal[${maChr.Find[stDisconcerting].Value}]}) /goto :loopmakestop
  }
  **|
  
  | Hit Mangling/Reckless Abandon/Savage Spirit
  /if (${is_ready[${_debug}, "${maChr.Find[stDisconcerting].Value}"]}) {
    /if (${Me.ActiveDisc.ID} && !${Me.ActiveDisc.Name.Find[stDisconcerting]}) {
      /stopdisc
    }
    /if (${is_ready[${_debug}, "${maChr.Find[stAARecklessAbandon].Value}"]}) {
  			/call voodoo ${_debug} "${maChr.Find[stAARecklessAbandon].Value}" 0 FALSE
  	}
    /call voodoo ${_debug} "${maChr.Find[stDisconcerting].Value}" 0 FALSE
  	/if (${is_ready[${_debug}, "${maChr.Find[stAASavageSpirit].Value}"]}) {
    	/call voodoo ${_debug} "${maChr.Find[stAASavageSpirit].Value}" 0 FALSE
  	}
  }
  
  | Brutal/berserking and savage spirit
  /if (!${Me.ActiveDisc.Name.Find[${maChr.Find[stDisconcerting].Value}]} && !${is_ready[${_debug}, "${maChr.Find[stDisconcerting].Value}"]}) {
    /if (!${Me.ActiveDisc.Name.Find[${maChr.Find[stInspiredAnger].Value}]} && ${is_ready[${_debug}, "${maChr.Find[stInspiredAnger].Value}"]}) {
        /stopdisc
      }
      /call voodoo ${_debug} "${maChr.Find[stInspiredAnger].Value}" 0 FALSE
      /if (${is_ready[${_debug}, "${maChr.Find[stAASavageSpirit].Value}"]}) {
      	/call voodoo ${_debug} "${maChr.Find[stAASavageSpirit].Value}" 0 FALSE
    	}
    }


  | Intensity of the Resolute AA 4 hour reuse
  /call cast_AA_intensity ${maDebug.Find[intensity].Value}

  /if (${is_ready[${_debug}, "${maChr.Find[stCryOfChaos].Value}"]} && !${Me.Song[${maChr.Find[stCryOfChaos].Value}].ID}) {
    /delay ${Math.Rand[8]}s ${Me.Song[${maChr.Find[stCryOfChaos].Value}].ID}
    /if (!${Me.Song[${maChr.Find[stCryOfChaos].Value}].ID}) {
      /if (${Raid.Members}) {
        /if (${is_ready[${_debug}, "${maChr.Find[stAACryofBattle].Value}"]}) {
          /call voodoo ${_debug} "${maChr.Find[stAACryofBattle].Value}" 0 FALSE
        }
      }
      /if (${is_ready[${_debug}, "${maChr.Find[stCryOfChaos].Value}"]} && ${Me.CurrentEndurance} > ${Spell[${maChr.Find[stCryOfChaos].Value}].EnduranceCost}) {
        /call voodoo ${_debug} "${maChr.Find[stCryOfChaos].Value}" 0 FALSE
      }
    }
  }
  
  | Blood Sustenance
  /if (${is_ready[${_debug}, "${maChr.Find[stAABloodSustenance].Value}"]}) {
    /call voodoo ${_debug} "${maChr.Find[stAABloodSustenance].Value}" 0 FALSE
  }

  | juggernaut surge
  /if (${is_ready[${_debug}, "${maChr.Find[stAAJuggernautSurge].Value}"]}) {
    /call voodoo ${_debug} "${maChr.Find[stAAJuggernautSurge].Value}" 0 FALSE
  }
  
  | blood pact
  /if (${is_ready[${_debug}, "${maChr.Find[stAABloodPact].Value}"]}) {
    /call voodoo ${_debug} "${maChr.Find[stAABloodPact].Value}" 0 FALSE
  }
  
  | Blinding
  /if (${is_ready[${_debug}, "${maChr.Find[stAABlindingFury].Value}"]}) {
    /call voodoo ${_debug} "${maChr.Find[stAABlindingFury].Value}" 0 FALSE
  }

  | Spire control
  /call cast_AA_spire ${maDebug.Find[spire].Value}

  | AA rampage choice
  /if (AE# && ${SpawnCount[npc radius ENV PS4]} >= AE#) {
    /if (${is_ready[${_debug}, "${maChr.Find[stAAFuriousRampage].Value}"]}) {
      /call voodoo ${_debug} "${maChr.Find[stAAFuriousRampage].Value}" 0 FALSE
    }
    /if (${is_ready[${_debug}, "${maChr.Find[stAASilentStrikes].Value}"]}) {
      /call voodoo ${_debug} "${maChr.Find[stAASilentStrikes].Value}" 0 FALSE
    }
  } else /if (!AE#) {
    /if (${is_ready[${_debug}, "${maChr.Find[stAAFocusedFuriousRampage].Value}"]}) {
      /call voodoo ${_debug} "${maChr.Find[stAAFocusedFuriousRampage].Value}" 0 FALSE
    }
    /if (${is_ready[${_debug}, "${maChr.Find[stAASilentStrikes].Value}"]}) {
      /call voodoo ${_debug} "${maChr.Find[stAASilentStrikes].Value}" 0 FALSE
    }
  }

	| Desperation
  /if (${is_ready[${_debug}, "${maChr.Find[stAADesperation].Value}"]} && !${Me.Song[${maChr.Find[stCryOfChaos].Value}].ID}) {
  	/if (${Me.PctHPs} > 90) {
      /call voodoo ${_debug} "${maChr.Find[stAABloodfury].Value}" 0 FALSE
      /delay 1
      /if (${Me.PctHPs} > 90) {
      		/call voodoo ${_debug} "${maChr.Find[stAABloodfury].Value}" 0 FALSE
      		/delay 1
      }
    }
    /call voodoo ${_debug} "${maChr.Find[stAADesperation].Value}" 0 FALSE
  }

	| Untamed Rage
  /if (${is_ready[${_debug}, "${maChr.Find[stAAUntamedRage].Value}"]}) {
    /call voodoo ${_debug} "${maChr.Find[stAAUntamedRage].Value}" 0 FALSE
  }

  | /if (${is_ready[${_debug}, ${maChr.Find[stUnsettlingScream].Value}]} && ${Me.CurrentEndurance} > ${Spell[${maChr.Find[stUnsettlingScream].Value}].EnduranceCost}) {
  |   /call voodoo ${_debug} "${maChr.Find[stUnsettlingScream].Value}" ${_tmpID} FALSE
  | }

  | no berserking use cleaving anger and reckless abandon
  /if (${is_ready[${_debug}, "${maChr.Find[stCleavingRage].Value}"]} && !${Me.ActiveDisc.ID} && !${is_ready[${_debug}, "${maChr.Find[stInspiredAnger].Value}"]}) {
    /call voodoo ${_debug} "${maChr.Find[stCleavingRage].Value}" 0 FALSE
  }

	| Reckless Abandon if avail
  /if (${is_ready[${_debug}, "${maChr.Find[stAARecklessAbandon].Value}"]}) {
    /call voodoo ${_debug} "${maChr.Find[stAARecklessAbandon].Value}" 0 FALSE
  }

	| Vehement Rage if avail
  /if (${is_ready[${_debug}, "${maChr.Find[stAAVehementRage].Value}"]}) {
    /call voodoo ${_debug} "${maChr.Find[stAAVehementRage].Value}" 0 FALSE
  }

  | no cleaving use vengful and glyph
  /if (${is_ready[${_debug}, "${maChr.Find[stVengefulFlurry].Value}"]} && !${Me.ActiveDisc.ID} && ${is_ready[${_debug}, "${maChr.Find[stResolve].Value}"]} && !${is_ready[${_debug}, "${maChr.Find[stCleavingRage].Value}"]} && !${is_ready[${_debug}, "${maChr.Find[stInspiredAnger].Value}"]}) {
    /call voodoo ${_debug} "${maChr.Find[stVengefulFlurry].Value}" 0 FALSE
  }

  /call cast_glyph ${maDebug.Find[glyph].Value}

  | no vengful use frenzied
  /if (!${Me.ActiveDisc.ID} && !${is_ready[${_debug}, "${maChr.Find[stInspiredAnger].Value}"]} && !${is_ready[${_debug}, "${maChr.Find[stVengefulFlurry].Value}"]} && !${is_ready[${_debug}, "${maChr.Find[stCleavingRage].Value}"]}) {
  	/if (${Me.PctHPs} > 90) {
      /call voodoo ${_debug} "${maChr.Find[stAABloodfury].Value}" 0 FALSE
      /delay 1
      /if (${Me.PctHPs} > 90) {
      		/call voodoo ${_debug} "${maChr.Find[stAABloodfury].Value}" 0 FALSE
      		/delay 1
      }
    }
    /if (${is_ready[${_debug}, "${maChr.Find[stResolve].Value}"]}) {
      /call voodoo ${_debug} "${maChr.Find[stResolve].Value}" 0 FALSE
    }
  }


/return TRUE



|***
 * note: class control
 * use: /chr
 ***|
sub set_control(string _type, string _verbage, string _verbage2, bool _debug)
  DEBUG \atset_control\ax(\a-w${_type}, "${_verbage}", ${_verbage2}\ax)

  /call set_control_shared ${_type} "${_verbage}" "${_verbage2}" ${_debug}
  /if (${Macro.Return.Equal[SKIP]}) /return FALSE

  /if (${_verbage2.Equal[SILENT]}) /return
  /invoke ${set_control_output[${_debug}, maChr, chr, maChrControl]}

/return TRUE

