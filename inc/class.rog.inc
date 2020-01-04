|***
 * entropy
 * class.clr.inc
 ***|
 


|***
 * note: primary initilization of all cleric non shared data
 * use: not for you
 ***|
sub set_ROG(bool _debug)
	DEBUG \atset_ROG\ax()
	
	/call set_data ${_debug} maData stSpire															map outer "Spire of the Rake"
	/call set_data ${_debug} maData stSynergy														map outer "Blackguard's Synergy"
	/call set_data ${_debug} maData stEpic1															map outer "Ragebringer"
	/call set_data ${_debug} maData stEpic15														map outer "Fatestealer"
	/call set_data ${_debug} maData stEpic2															map outer "Nightshade, Blade of Entropy"
	/call set_data ${_debug} maData stFade															map outer "Escape"

	/call set_data_map ${_debug} maChr stAlliance												PREGEN|1											"${auxna} alliance spell"
 	/call set_data_map ${_debug} maChr stRest 													PREGEN|1											"${auxna} name of stamina rest disc"
 	/call set_data_map ${_debug} maChr stHiatus													PREGEN|1											"${auxna} name of hiatus stamina recovery disc"
 	/call set_data_map ${_debug} maChr swHiatus													FALSE													"${auxsw} use hiatus rest disc"
	
	/call set_data_map ${_debug} maChr stDichotomic											PREGEN|1											"${auxna} Dichotomic disc family"
	/call set_data_map ${_debug} maChr stAggro 													PREGEN|1											"${auxna} Misdirection disc family"
	/call set_data_map ${_debug} maChr stAssault 												PREGEN|1											"${auxna} Assault disc family"
	/call set_data_map ${_debug} maChr stBleed 													PREGEN|1											"${auxna} Bleed disc family"
	/call set_data_map ${_debug} maChr stPlay	 													PREGEN|1											"${auxna} Knifeplay disc family"
	/call set_data_map ${_debug} maChr stPhantom												PREGEN|1											"${auxna} Phantom Assassin disc family"
	/call set_data_map ${_debug} maChr stHack														PREGEN|1											"${auxna} Jugular Slash disc family"
	/call set_data_map ${_debug} maChr stHiddenBlade										PREGEN|1											"${auxna} Hidden Blade disc family"
	/call set_data_map ${_debug} maChr stBladePoison										PREGEN|1											"${auxna} Toxic Blade disc family"
	/call set_data_map ${_debug} maChr stSneakAttack										PREGEN|1											"${auxna} Sneak Attack disc family"
	/call set_data_map ${_debug} maChr stShadowHunter										PREGEN|1											"${auxna} Shadow-Hunter's Dagger disc family"
	/call set_data_map ${_debug} maChr stDisassociative									PREGEN|1											"${auxna} Disassociative Puncture disc family"
	/call set_data_map ${_debug} maChr stAspBleeder											PREGEN|1											"${auxna} Aspbleeder disc family"
	/call set_data_map ${_debug} maChr stPinPoint 											PREGEN|1											"${auxna} Pinpoint Vulnerability disc family"
	/call set_data_map ${_debug} maChr stMark 													PREGEN|1											"${auxna} Easy Mark disc family"
	/call set_data_map ${_debug} maChr stEradicator 										PREGEN|1											"${auxna} Duelist disc family"
	/call set_data_map ${_debug} maChr stRazor 													PREGEN|1											"${auxna} Razor's Edge disc family"
	/call set_data_map ${_debug} maChr stFrenzy 												PREGEN|1											"${auxna} Frenzied Stabbing disc family"
	/call set_data_map ${_debug} maChr stTwistedChance									PREGEN|1											"${auxna} Twisted Chance disc family"
	/call set_data_map ${_debug} maChr stVision 												PREGEN|1											"${auxna} Thief's Eyes disc family"
	/call set_data_map ${_debug} maChr stTwistedShank										PREGEN|1											"${auxna} Twisted Shank disc family"
	| /call set_data_map ${_debug} maChr stAbsorbingAgent									PREGEN|1											"${auxna} Absorbing Agent disc family"

	/call set_data_map ${_debug} maChr swBlinding												FALSE													"${auxsw} use blinding disc.. or not."
	/call set_data_map ${_debug} maChr stBlinding												PREGEN|1											"${auxna} Blinding Flash disc family"

	/call set_data_map ${_debug} maChr stLegPoison											0															"${auxna} number of summon leg poisons to keep. dont forget the buff tag (\a-wsummonpoison\ax) for legs"

/return TRUE


|***
 * note: stabb'em in the ass
 * use: 
 ***|
sub main_ROG(bool _debug)
	DEBUG \atmain_ROG\ax()

	GETINPUT
	ISMEDEAD
	CHECKTIE
	CHECKREZ

	/if (AUTO) {
		/if (${maEnv.Find[swSoS].Value}) /call set_sos FALSE
	}
	
	/while (${check_combat_status[FALSE]}) {
		GETINPUT
		/if (${maHeal.Find[swHealSelf].Value} && !${Group.Members}) /call check_heal_${Me.Class.ShortName} ${maDebug.Find[self].Value} Self
		/if (!${timer_Check_Cure} && ${maChr.Find[swHealCure].Value}) /call check_cure ${maDebug.Find[cure].Value}
		/if (${is_defensive_active[${maDebug.Find[def].Value}, FALSE, SAFEENV]} != TRUE) /call cast_defensive_cycle ${maDebug.Find[def].Value}
		/if (${check_class_loop[FALSE]}) /return

		/if (VALID) {
			/call do_sneak_attack ${maDebug.Find[class].Value} TARGET
			/call set_combat FALSE
			| /if (MELEE && !${timer_check_stick}) /call stick_to_target ${maDebug.Find[stick].Value} TARGET 
			/if (${is_defensive_active[${maDebug.Find[def].Value}, FALSE, SAFEENV]} != TRUE) /call cast_defensive_cycle ${maDebug.Find[def].Value}
			/call misc_combat_${Me.Class.ShortName} ${maDebug.Find[misc].Value} TARGET
			/if (BURN) /call burn_${Me.Class.ShortName} ${maDebug.Find[burn].Value} TARGET
			/if (MELEE) /call do_melee FALSE TARGET
			/call check_item_cycle ${maDebug.Find[item].Value} TARGET

		}

		/if (AUTO) {	
			CHECKFADE
			/if (${lsClassMelee.Contains[${Me.Class.ShortName}]}) /call cast_stamina_recovery ${maDebug.Find[staminarecover].Value}
		}
	}

/return TRUE



|***
 * note: suprize but seks?
 * use: designed for a first attack
 ***|
sub do_sneak_attack(bool _debug, int _tmpID)
	DEBUG \atdo_sneak_attack\ax(\a-w${_tmpID}\ax)

	/if (!${is_ready[${_debug}, "${maChr.Find[stSneakAttack].Value}"]}) /return FALSE
	CHECKEXIT
	/call target_reset ${_debug} ${_tmpID} all
	
	/if (${Target.ID} && ${Me.Invis} && ${Me.Sneaking}) {
		/call voodoo ${_debug} "${maChr.Find[stSneakAttack].Value}" ${_tmpID} FALSE
		/doability backstab
	}

/return TRUE



|***
 * note: Combat skills, Disciplines and Alternate abilities.
 * use: /call misc_combat_ROG DEBUG
 ***|
sub misc_combat_ROG(bool _debug, int _tmpID)
	DEBUG \atmisc_combat_${Me.Class.ShortName\ax(\a-w${_tmpID}\ax)

	ISMEDEAD
	/if (!${target_reset[ ${_debug}, ${_tmpID}, all]}) /return FALSE

	CHECKEXIT
	GETINPUT
	CHECKTIE

	/if (!${is_defensive_active[${maDebug.Find[def].Value}, FALSE, SAFEENV]}) /call cast_defensive_cycle ${maDebug.Find[def].Value}
	/if (SWARM || PET) /call send_pet ${maDebug.Find[sic].Value} ${_tmpID}	
	/if (AUTO && MELEE) /call check_facing ${maDebug.Find[face].Value} ${_tmpID} FORCE
	
	/if (${is_ready[${_debug}, "${maChr.Find[stAssault].Value}"]}) {
		/call voodoo ${_debug} "${maChr.Find[stAssault].Value}" ${_tmpID} FALSE
	}
	
	/if (${is_ready[${_debug}, "Shadow's Flanking"]}) {
		/call voodoo ${_debug} "Shadow's Flanking" 0 FALSE
	
	}
	/if (${is_ready[${_debug}, "${maChr.Find[stVision].Value}"]} && !${Me.Song[${maChr.Find[stVision].Value}].ID}) {
		/call voodoo ${_debug} "${maChr.Find[stVision].Value}" 0 FALSE
	}
	/if (${is_ready[${_debug}, "${maChr.Find[stTwistedShank].Value}"]}) {
		/call voodoo ${_debug} "${maChr.Find[stTwistedShank].Value}" ${_tmpID} FALSE
	}

	/if (!${target_reset[ ${_debug}, ${_tmpID}, all]}) /return FALSE

	/if (${is_ready[${_debug}, "${maChr.Find[stShadowHunter].Value}"]}) {
		/call voodoo ${_debug} "${maChr.Find[stShadowHunter].Value}" ${_tmpID} FALSE
	}
	/if (${is_ready[${_debug}, "${maChr.Find[stDisassociative].Value}"]}) {
		/call voodoo ${_debug} "${maChr.Find[stDisassociative].Value}" ${_tmpID} FALSE
	}
	/if (${is_ready[${_debug}, "${maChr.Find[stBleed].Value}"]}) {
		/call voodoo ${_debug} "${maChr.Find[stBleed].Value}" ${_tmpID} FALSE
	}

	/if (!${target_reset[ ${_debug}, ${_tmpID}, all]}) /return FALSE

	/if (${is_ready[${_debug}, "${maChr.Find[stPlay].Value}"]}) {
		/if (!${lsZoneNamed.Contains[${Spawn[${_tmpID}].DisplayName}]} || !${is_ready[${_debug}, "${maChr.Find[stFrenzy].Value}"]}) {
			/if (${is_ready[${_debug}, "${maChr.Find[stPlay].Value}"]} && !${Bool[${Me.ActiveDisc}]}) {
				/call voodoo ${_debug} "${maChr.Find[stPlay].Value}" 0 FALSE
			}
		}
	}
	
	/if (!${lsZoneNamed.Contains[${Target.DisplayName}]} && !${is_ready[${_debug}, "${maChr.Find[stFrenzy].Value}"]}) {
		/if (${is_ready[${_debug}, "${maChr.Find[stPlay].Value}"]} && !${Bool[${Me.ActiveDisc}]}) {
			/call voodoo ${_debug} "${maChr.Find[stPlay].Value}" 0 FALSE
		}
	}	
		
	/if (!${target_reset[ ${_debug}, ${_tmpID}, all]}) /return FALSE
	
	/if (${Target.ID} && ${is_ready[${_debug}, "${maChr.Find[stPhantom].Value}"]}) {
		/call voodoo ${_debug} "${maChr.Find[stPhantom].Value}" ${_tmpID} FALSE
	}
	/if (${is_ready[${_debug}, "${maChr.Find[stHiddenBlade].Value}"]}) {
		/call voodoo ${_debug} "${maChr.Find[stHiddenBlade].Value}" 0 FALSE
	}
	/if (${is_ready[${_debug}, "${maChr.Find[stHack].Value}"]}) {
		/call voodoo ${_debug} "${maChr.Find[stHack].Value}" ${_tmpID} FALSE
	}
	/if (${is_ready[${_debug}, "${maChr.Find[stBladePoison].Value}"]}) {
		/call voodoo ${_debug} "${maChr.Find[stBladePoison].Value}" 0 FALSE
	}
	
	/if (!${target_reset[ ${_debug}, ${_tmpID}, all]}) /return FALSE
	
	/if (!AGRO) {
		/if (${is_ready[${_debug}, "${maChr.Find[stAggro].Value}"]}) {
			/call voodoo ${_debug} "${maChr.Find[stAggro].Value}" ${_tmpID} FALSE
		}
	}
	
	/if (${maChr.Find[swBlinding].Value} && ${Target.Level} <= ${Me.Level}) {
		/if (${is_ready[${_debug}, "${maChr.Find[stBlinding].Value}"]}) {
			/call voodoo ${_debug} "${maChr.Find[stBlinding].Value}" ${_tmpID} FALSE
		}
	}
	
/return TRUE



|***
 * note: healing
 * use: auto
 ***|
sub check_heal_ROG(bool _debug, string _type)
	DEBUG \atcheck_heal_ROG\ax(\a-w${_type}\ax)

	CHECKTIE
	ISMEDEAD
	CHECKEXIT
	CHECKREZ
	CHECKFADE

	SETHUD "heal ${_type}"

	/if (!${check_hurt[${_debug}, ${_type}]}) /return FALSE

	/if (${is_ready[${_debug}, "Tumble"]}) {
		/call voodoo ${_debug} "Tumble" 0 FALSE
	}
	
	/call cast_list ${_debug} lsHealBaseLine ${Me.ID}		
	
	SETHUD FALSE

/return



|***
 * note: sets the rogue to invis
 * use: auto
 ***|
sub set_sos(bool _debug)
	DEBUG \atset_sos\ax()

	/if (!${maEnv.Find[swSoS].Value}) /return FALSE
	/if (${Me.Combat} || ${Me.Stunned} || ${Me.State.Equal[FEIGN]}) /return FALSE
	/if (${Me.Casting.ID} || ${Window[TradeWnd].Open} || ${Window[MerchantWnd].Open} || ${Window[BigBankWnd].Open} || ${Me.Mount.ID}) /return FALSE

	ISMEDEAD
	CHECKTIE
	
	/declare _sos bool local FALSE

	/if (${Me.Invis} && ${Me.Sneaking} && !${Me.AbilityReady[hide]} && !${Me.AbilityReady[sneak]}) {
		/return
	}

	/if (!${Me.Invis} && ${Me.AbilityReady[hide]} && ${Me.Sneaking} && !${Me.AbilityReady[sneak]} ) {
		/if (${Me.AbilityReady[hide]}) /doability hide
		/return	
	} else /if (!${Me.Invis} && ${Me.Sneaking} && !${Me.AbilityReady[sneak]}) {
		/if (${Me.AbilityReady[sneak]}) /doability sneak
		/return
	}
	
	/if (!${_sos} && ${Me.AbilityReady[hide]} && ${Me.AbilityReady[sneak]}) {
		/doability hide
		/delay 1
		/doability sneak
	}
	
/return TRUE



|***
 * note: burn routines
 * use: seriously. if you are not buning constantly, you are doing something wrong. wtf you thinking.
 ***| 
sub burn_ROG(bool _debug, int _tmpID)
	DEBUG \atburn_${Me.Class.ShortName\ax(\a-w${_tmpID}\ax)

	ISMEDEAD
	/if (!${target_reset[ ${_debug}, ${_tmpID}, all]}) /return FALSE

	CHECKEXIT
	GETINPUT
	CHECKTIE

	/if (!${is_defensive_active[${maDebug.Find[def].Value}, BURN, SAFEENV]}) /call cast_defensive_cycle ${maDebug.Find[def].Value}
	/if (SWARM || PET) /call send_pet ${maDebug.Find[sic].Value} ${_tmpID}	
	/if (AUTO && MELEE) /call check_facing ${maDebug.Find[face].Value} ${_tmpID} FORCE

	/if (${Me.ActiveDisc.Name.Equal[${maChr.Find[stPlay].Value}]} && ${Me.CombatAbilityReady[${Spell[${maChr.Find[stFrenzy].Value}].RankName}]}) {
		:loopmakestop
		/delay 5
		/if (${Me.ActiveDisc.Name.Equal[${maChr.Find[stPlay].Value}]} && ${Me.CombatAbilityReady[${Spell[${maChr.Find[stFrenzy].Value}].RankName}]}) /stopdisc
		/delay 5
		/if (${Me.ActiveDisc.Name.Equal[${maChr.Find[stPlay].Value}]} && ${Me.CombatAbilityReady[${Spell[${maChr.Find[stFrenzy].Value}].RankName}]}) /goto :loopmakestop
	}
	
	/if (!${target_reset[ ${_debug}, ${_tmpID}, all]}) /return FALSE

	/if (${is_ready[${_debug}, "${maChr.Find[stDichotomic].Value}"]}) {
		/call voodoo ${_debug} "${maChr.Find[stDichotomic].Value}" 0 FALSE
	}

	/if (!AE#) {
		/if (${is_ready[${_debug}, "Focused Rake's Rampage"]}) {
			/call voodoo ${_debug} "Focused Rake's Rampage" 0 FALSE
		}
	} else /if (AE#) {
		/if (${is_ready[${_debug}, "Rake's Rampage"]}) {
			/call voodoo ${_debug} "Rake's Rampage" 0 FALSE
		}
	}

	/if (${is_ready[${_debug}, "${maChr.Find[stPinPoint].Value}"]}) {
		/call voodoo ${_debug} "${maChr.Find[stPinPoint].Value}" ${_tmpID} FALSE
	}
	/if (${is_ready[${_debug}, "Rogue's Fury"]}) {
		/call voodoo ${_debug} "Rogue's Fury" 0 FALSE
	}
	/if (${is_ready[${_debug}, "${maChr.Find[stMark].Value}"]} && ${Me.PctEndurance} > 15) {
		/call voodoo ${_debug} "${maChr.Find[stMark].Value}" ${_tmpID} FALSE
	}
	
	/if (!${target_reset[ ${_debug}, ${_tmpID}, all]}) /return FALSE
	
	/call cast_AA_spire ${maDebug.Find[spire].Value}
	
	/if (${is_ready[${_debug}, "Intensity of the Resolute"]} && ${Raid.Members}) {
		/call voodoo ${_debug} "Intensity of the Resolute" 0 FALSE
	}
  /if (!${Me.ActiveDisc.ID}) {
  	/if (${is_ready[${_debug}, "${maChr.Find[stFrenzy].Value}"]}) {
  		/call voodoo ${_debug} "${maChr.Find[stFrenzy].Value}" 0 FALSE
  	}
  	/delay 2
  }
  /if (${is_ready[${_debug}, "${maChr.Find[stTwistedChance].Value}"]} && !${Me.ActiveDisc.ID}) {
  	/call voodoo ${_debug} "${maChr.Find[stTwistedChance].Value}" 0 FALSE
  }
  /if (${is_ready[${_debug}, "${maChr.Find[stEradicator].Value}"]} && !${Me.ActiveDisc.ID}) {
  	/call voodoo ${_debug} "${maChr.Find[stEradicator].Value}" 0 FALSE
  }
  /if (${is_ready[${_debug}, "${maChr.Find[stRazor].Value}"]} && !${Me.ActiveDisc.ID}) {
  	/call voodoo ${_debug} "${maChr.Find[stRazor].Value}" 0 FALSE
  }
  
  /if (!${target_reset[ ${_debug}, ${_tmpID}, all]}) /return FALSE

  /if (${is_ready[${_debug}, "${maChr.Find[stAspBleeder].Value}"]} && !${Me.ActiveDisc.ID}) {
  	/call voodoo ${_debug} "${maChr.Find[stAspBleeder].Value}" 0 FALSE
  }

	/call cast_glyph ${maDebug.Find[glyph].Value}

/return TRUE



|***
 * note: rogue specific controls
 * use: /chr
 ***|
sub set_control_ROG(string _type, string _verbage, string _silent, bool _debug)
	DEBUG \atset_control_ROG\ax(\a-w${_type}, "${_verbage}", ${_silent}\ax)
	
	/declare _out string local

	| output control map
	/if (${_type.Equal[see]}) {
		/call Bind_command_see maChr ${_verbage}
		/return
		
	| set ae on/off / count
	} else /if (${_type.Equal[ae]}) {
		/if (!${set_control_num_range[${_debug}, stCountAE, "${_verbage}", 0, 99]}) /return FALSE

	| defensive mob agro count
	} else /if (${_type.Equal[def]}) {
		/if (!${set_control_num_range[${_debug}, stCountDefensive, "${_verbage}", 0, 99]}) /return FALSE

	| set AA fade use %	
	} else /if (${_type.Equal[fade]}) {
		/if (!${set_control_num_range[${_debug}, stPctFade, "${_verbage}", 0, 99]}) /return FALSE

	| set spire to be used
	} else /if (${_type.Equal[spire]}) {
		/call set_control_chr_spire ${_debug} ${_verbage} ${_silent}
		/return

	| use blinding disc
	} else /if (${_type.Equal[blind]}) {
		/varset dump ${set_switch_env[${_debug}, swBlinding, TRUE, ${_verbage}]}
		
	| use hiatus disc
	} else /if (${_type.Equal[hiatus]}) {
		/varset dump ${set_switch_env[${_debug}, swHiatus, TRUE, ${_verbage}]}
	
	| leg poisons to keep
	} else /if (${_type.Equal[poison]}) {
		/if (!${set_control_num_range[${_debug}, stLegPoison, "${_verbage}", 0, 99]}) /return FALSE
	
	}

	/if (${_silent.Equal[SILENT]}) /return 
	/varset dump ${set_control_output[${_debug}, maChr, chr, maChrControl]}
	
/return TRUE

