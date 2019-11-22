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
	
	/call set_data FALSE maData stSpire																	map outer "Spire of the Rake"
	/call set_data FALSE maData stSynergy																map outer "Blackguard's Synergy"
	/call set_data FALSE maData stEpic1																	map outer "Ragebringer"
	/call set_data FALSE maData stEpic15																map outer "Fatestealer"
	/call set_data FALSE maData stEpic2																	map outer "Nightshade, Blade of Entropy"
	/call set_data FALSE maData stFade																	map outer "Escape"

	/call set_data_map FALSE maChr stAlliance														PREGEN|1											"${auxna} alliance spell"
 	/call set_data_map FALSE maChr stRest 															PREGEN|1											"${auxna} name of stamina rest disc"

	/call set_data_map FALSE maChr stDichotomic													PREGEN|1											"${auxna} Dichotomic disc family"
	/call set_data_map FALSE maChr stAggro 															PREGEN|1											"${auxna} Misdirection disc family"
	/call set_data_map FALSE maChr stAssault 														PREGEN|1											"${auxna} Assault disc family"
	/call set_data_map FALSE maChr stBleed 															PREGEN|1											"${auxna} Bleed disc family"
	/call set_data_map FALSE maChr stPlay	 															PREGEN|1											"${auxna} Knifeplay disc family"
	/call set_data_map FALSE maChr stPhantom														PREGEN|1											"${auxna} Phantom Assassin disc family"
	/call set_data_map FALSE maChr stHack																PREGEN|1											"${auxna} Jugular Slash disc family"
	/call set_data_map FALSE maChr stHiddenBlade												PREGEN|1											"${auxna} Hidden Blade disc family"
	/call set_data_map FALSE maChr stBladePoison												PREGEN|1											"${auxna} Toxic Blade disc family"
	/call set_data_map FALSE maChr stSneakAttack												PREGEN|1											"${auxna} Sneak Attack disc family"
	/call set_data_map FALSE maChr stShadowHunter												PREGEN|1											"${auxna} Shadow-Hunter's Dagger disc family"
	/call set_data_map FALSE maChr stDisassociative											PREGEN|1											"${auxna} Disassociative Puncture disc family"
	/call set_data_map FALSE maChr stAspBleeder													PREGEN|1											"${auxna} Aspbleeder disc family"
	/call set_data_map FALSE maChr stPinPoint 													PREGEN|1											"${auxna} Pinpoint Vulnerability disc family"
	/call set_data_map FALSE maChr stMark 															PREGEN|1											"${auxna} Easy Mark disc family"
	/call set_data_map FALSE maChr stEradicator 												PREGEN|1											"${auxna} Duelist disc family"
	/call set_data_map FALSE maChr stRazor 															PREGEN|1											"${auxna} Razor's Edge disc family"
	/call set_data_map FALSE maChr stFrenzy 														PREGEN|1											"${auxna} Frenzied Stabbing disc family"
	/call set_data_map FALSE maChr stTwistedChance											PREGEN|1											"${auxna} Twisted Chance disc family"
	/call set_data_map FALSE maChr stVision 														PREGEN|1											"${auxna} Thief's Eyes disc family"
	/call set_data_map FALSE maChr stTwistedShank												PREGEN|1											"${auxna} Twisted Shank disc family"
	/call set_data_map FALSE maChr stAbsorbingAgent											PREGEN|1											"${auxna} Absorbing Agent disc family"

	/call set_data_map FALSE maChr swBlinding														FALSE													"${auxsw} use blinding disc.. or not."
	/call set_data_map FALSE maChr stBlinding														PREGEN|1											"${auxna} Blinding Flash disc family"

	| keep on cooldown
	| /call timer_set FALSE tClick_Poison																	string		outer 30s 	timer_click_poison
	                                           		              				        	
	| /call sql_control FALSE ${chrID} clickPoisonBuff										string 		outer "|Bite of the Shissar Poison X|Tallon's Tactic Poison IV|Tallon's Tactic Poison V|Vallon's Tactic Poison IV|Vallon's Tactic Poison V|Mana Poison Coat|Hero's Toxin Poison I|Hero's Toxin Poison II"
	| /call sql_control FALSE ${chrID} clickPoisonItem										string 		outer "Consigned Bite of the Shissar XVIII"
	| /call sql_control FALSE ${chrID} clickPoisonDispenser								string 		outer "Selrach's Shadowscale Leggings"
	| /call sql_control FALSE ${chrID} clickPoisonCount										int		 		outer 60
                                             		              				        	                                             		              			        	
	
/return


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
		| /if (!${timer_click_poison}) /call click_poison FALSE
		/if (${maEnv.Find[swSoS].Value}) /call set_sos FALSE
	}
	
	/while (${check_combat_status[FALSE]}) {
		GETINPUT
		/if (${maHeal.Find[swHealSelf].Value} && !${Group.Members}) /call check_heal_${Me.Class.ShortName} FALSE Self
		/if (!${timer_Check_Cure} && ${maChr.Find[swHealCure].Value}) /call check_cure FALSE
		/if (${is_defensive_active[FALSE, FALSE, SAFEENV]} != TRUE) /call cast_defensive_cycle FALSE
		/if (${check_class_loop[FALSE]}) /return

		/if (VALID) {
			/call do_sneak_attack FALSE
			/call set_combat FALSE
			/if (MELEE && !${timer_Check_Behind}) /call stick_to_target FALSE TARGET 
			/if (${is_defensive_active[FALSE, FALSE, SAFEENV]} != TRUE) /call cast_defensive_cycle FALSE
			/call misc_combat_${Me.Class.ShortName} FALSE
			/if (BURN) /call burn_${Me.Class.ShortName} FALSE
			/if (MELEE) /call do_melee FALSE TARGET
			/call check_item_cycle FALSE TARGET

		}

		/if (AUTO) {	
			CHECKFADE
			/if (${lsClassMelee.Contains[${Me.Class.ShortName}]}) /call cast_stamina_recovery
		}
	}

/return TRUE



|***
 * note: suprize but seks?
 * use: designed for a first attack
 ***|
sub do_sneak_attack(bool _debug)
	DEBUG \atdo_sneak_attack\ax()

	/if (!${is_ready[FALSE, "${maChr.Find[stSneakAttack].Value}"]}) /return
	CHECKEXIT
	/if (${is_target_dead[FALSE, TARGET]}) /return
	
	/if (${Target.ID} && ${Me.Invis} && ${Me.Sneaking}) {
		/call voodoo FALSE "${maChr.Find[stSneakAttack].Value}" TARGET FALSE
		/doability backstab
	}

/return



|***
 * note: Combat skills, Disciplines and Alternate abilities.
 * use: /call misc_combat_ROG DEBUG
 ***|
sub misc_combat_ROG(bool _debug)
	DEBUG \atmisc_combat_ROG\ax()

	CHECKEXIT
	GETINPUT
	/if (${is_target_dead[FALSE, TARGET]}) /return
	/call set_combat FALSE
	/if (${is_defensive_active[FALSE, FALSE, SAFEENV]} != TRUE) /call cast_defensive_cycle FALSE
	/if (${is_ready[FALSE, "${maChr.Find[stAssault].Value}"]}) /call voodoo FALSE "${maChr.Find[stAssault].Value}" TARGET FALSE
	
	/if (${is_ready[FALSE, "Shadow's Flanking"]}) /call voodoo FALSE "Shadow's Flanking" 0 FALSE
	/if (${is_ready[FALSE, "${maChr.Find[stVision].Value}"]} && !${Me.Song[${maChr.Find[stVision].Value}].ID}) /call voodoo FALSE "${maChr.Find[stVision].Value}" 0 FALSE
	/if (${is_ready[FALSE, "${maChr.Find[stTwistedShank].Value}"]}) /call voodoo FALSE "${maChr.Find[stTwistedShank].Value}" TARGET FALSE
	/if (${is_ready[FALSE, "${maChr.Find[stAbsorbingAgent].Value}"]}) /call voodoo FALSE "${maChr.Find[stAbsorbingAgent].Value}" TARGET FALSE

	/if (${is_target_dead[FALSE, TARGET]}) /return

	/if (${is_ready[FALSE, "${maChr.Find[stShadowHunter].Value}"]}) /call voodoo FALSE "${maChr.Find[stShadowHunter].Value}" TARGET FALSE
	/if (${is_ready[FALSE, "${maChr.Find[stDisassociative].Value}"]}) /call voodoo FALSE "${maChr.Find[stDisassociative].Value}" TARGET FALSE
	/if (${is_ready[FALSE, "${maChr.Find[stBleed].Value}"]}) /call voodoo FALSE "${maChr.Find[stBleed].Value}" TARGET FALSE

	/if (${is_target_dead[FALSE, TARGET]}) /return

	/if (${is_ready[FALSE, "${maChr.Find[stPlay].Value}"]}) {
		/if (${lsZoneNamed.Contains[${Spawn[TARGET].DisplayName}]} && ${is_ready[FALSE, "${maChr.Find[stFrenzy].Value}"]}) /goto :skipDiscPlay
		/if (${is_ready[FALSE, "${maChr.Find[stPlay].Value}"]} && !${Bool[${Me.ActiveDisc}]}) /call voodoo FALSE "${maChr.Find[stPlay].Value}" 0 FALSE
		:skipDiscPlay
	}
	
	/if (!${lsZoneNamed.Contains[${Target.DisplayName}]} && !${is_ready[FALSE, "${maChr.Find[stFrenzy].Value}"]}) {
		/if (${is_ready[FALSE, "${maChr.Find[stPlay].Value}"]} && !${Bool[${Me.ActiveDisc}]}) /call voodoo FALSE "${maChr.Find[stPlay].Value}" 0 FALSE
	}		
	/if (${is_target_dead[FALSE, TARGET]}) /return
	
	/if (${Target.ID} && ${is_ready[FALSE, "${maChr.Find[stPhantom].Value}"]}) /call voodoo FALSE "${maChr.Find[stPhantom].Value}" TARGET FALSE
	/if (${is_ready[FALSE, "${maChr.Find[stHiddenBlade].Value}"]}) /call voodoo FALSE "${maChr.Find[stHiddenBlade].Value}" 0 FALSE
	/if (${is_ready[FALSE, "${maChr.Find[stHack].Value}"]}) /call voodoo FALSE "${maChr.Find[stHack].Value}" TARGET FALSE
	/if (${is_ready[FALSE, "${maChr.Find[stBladePoison].Value}"]}) /call voodoo FALSE "${maChr.Find[stBladePoison].Value}" 0 FALSE
	/if (${is_ready[FALSE, "${maChr.Find[stAggro].Value}"]}) /call voodoo FALSE "${maChr.Find[stAggro].Value}" TARGET FALSE
	
	/if (${maChr.Find[swDiscBlinding].Value} && ${Target.Level} <= ${Me.Level}) {
		/if (${is_ready[FALSE, "${maChr.Find[stBlinding].Value}"]}) /call voodoo FALSE "${maChr.Find[stBlinding].Value}" TARGET FALSE
	}
	
/return



|***
 * note: healing
 * use: auto
 ***|
sub check_heal_ROG(bool _debug, string _type)
	DEBUG check_heal_ROG(${_debug}, ${_type})

	CHECKTIE
	ISMEDEAD
	CHECKEXIT
	CHECKREZ
	CHECKFADE

	SETHUD "heal ${_type}"

	/if (!${check_hurt[${_debug}, ${_type}]}) /return FALSE

	/if (${is_ready[FALSE, alt, "Tumble", ${hurtID}]}) {
		/call voodoo FALSE "Tumble" alt 0 FALSE
	}
	
	/call cast_list FALSE lsHealBaseLine ${Me.ID}	
	
	SETHUD FALSE

/return



|***
 * note: sets the rogue to invis
 * use: auto
 ***|
sub set_sos(bool _debug)
	DEBUG \atset_sos\ax()

	/if (!${maEnv.Find[swSoS].Value}) /return
	/if (${Me.Combat} || ${Me.Stunned} || ${Me.State.Equal[FEIGN]}) /return
	/if (${Me.Casting.ID} || ${Window[TradeWnd].Open} || ${Window[MerchantWnd].Open} || ${Window[BigBankWnd].Open} || ${Me.Mount.ID}) /return

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
	
/return



|***
 *	DES: keep poisons running
 *	USE: /call click_poison
 *  NOTE: 
 ***|
sub click_poison(bool _debug)
	DEBUG click_poison(${_debug})

	/call timer_update timer_click_poison ${tClick_Poison}
	
	/if (${Me.Moving} || ${Select[${Me.State},dead,feign,stun]} || ISMEDEAD || ${swNeedRez}) /return
	CHECKTIE
	/declare _count int local 0
	
	
	| keep a stock on hand
	DEBUG Poison counts${dep}\a-w${FindItemCount[${clickPoisonItem}]} < ${clickPoisonCount}\ax ${sep} clickPoisonDispenser:\a-w${Me.ItemReady[${clickPoisonDispenser}]}\ax
	/if (${FindItemCount[${clickPoisonItem}]} < ${clickPoisonCount} && ${Me.ItemReady[${clickPoisonDispenser}]}) {
		CHECKCOMBATCAST
		/if (${Me.Invis}) /makemevisible
		/if (${is_ready[FALSE, "${clickPoisonDispenser}", ${Me.ID}]}) {
			/call voodoo FALSE "${clickPoisonDispenser}" item 0 FALSE
		}
		/call sort_cursor TRUE FORCE
	}
	
	| check the buffs
	/for _count 1 to ${clickPoisonBuff.Count[|]}
		DEBUG Poison Check${dep}\aw${clickPoisonBuff.Arg[${_count},|]}\ax${dep}\a-w${Me.Buff[${clickPoisonBuff.Arg[${_count},|]}].ID}\ax
		/if (${Me.Buff[${clickPoisonBuff.Arg[${_count},|]}].ID}) /return
	/next _count		

	| CHECKCOMBATCAST
	/if (${Me.Invis}) /makemevisible
	/if (${is_ready[FALSE, "${clickPoisonItem}"]}) {
		/call voodoo FALSE "${clickPoisonItem}" 0 FALSE	
	}
	
/return



|***
 * note: burn routines
 * use: seriously. if you are not buning constantly, you are doing something wrong. wtf you thinking.
 ***| 
sub burn_ROG(bool _debug)
	DEBUG burn_ROG(${_debug})

	CHECKEXIT
	GETINPUT
	
	/if (${is_target_dead[FALSE, TARGET]}) /return
	/call set_combat FALSE

	/if (${Me.ActiveDisc.Name.Equal[${DiscPlay}]} && ${Me.CombatAbilityReady[${Spell[${DiscFrenzy}].RankName}]}) {
		:loopmakestop
		/delay 5
		/if (${Me.ActiveDisc.Name.Equal[${DiscPlay}]} && ${Me.CombatAbilityReady[${Spell[${DiscFrenzy}].RankName}]}) /stopdisc
		/delay 5
		/if (${Me.ActiveDisc.Name.Equal[${DiscPlay}]} && ${Me.CombatAbilityReady[${Spell[${DiscFrenzy}].RankName}]}) /goto :loopmakestop
	}

	| envenomed blades / dirty fighting stack issue
	/if (${Target.ID} && ${Me.AltAbilityReady[Dirty Fighting]} && !${Me.Buff[Envenomed Blades].ID}) {
		/if (${is_ready[FALSE, alt, "Dirty Fighting"]}) /call voodoo FALSE "Dirty Fighting" alt TARGET FALSE
	} else /if (${Me.AltAbilityReady[Envenomed Blades]} && !${Me.Buff[Dirty Fighting].ID}) {
		/if (${is_ready[FALSE, alt, "Envenomed Blades", ${Me.ID}]}) /call voodoo FALSE "Envenomed Blades" alt 0 FALSE
	}

	/if (${is_ready[FALSE, "${maChr.Find[stDicho].Value}", ${Me.ID}]}) /call voodoo FALSE "${DiscDicho}" disc 0 FALSE

	/if (!${AEMelee}) {
		/if (${is_ready[FALSE, "Focused Rake's Rampage", ${Me.ID}]}) /call voodoo FALSE "Focused Rake's Rampage" alt 0 FALSE
	} else /if (${AEMelee}) {
		/if (${is_ready[FALSE, "Rake's Rampage", ${Me.ID}]}) /call voodoo FALSE "Rake's Rampage" alt 0 FALSE
	}

	/if (${is_ready[FALSE, "${maChr.Find[stPinPoint].Value}"]}) /call voodoo FALSE "${DiscPinPoint}" disc TARGET FALSE
	/if (${is_ready[FALSE, "${maChr.Find[stAggro].Value}"]}) /call voodoo FALSE "${DiscAggro}" disc TARGET FALSE
	/if (${is_ready[FALSE, "Rogue's Fury", ${Me.ID}]}) /call voodoo FALSE "Rogue's Fury" alt 0 FALSE
	/if (${is_ready[FALSE, "${maChr.Find[stMark].Value}"]} && ${Me.PctEndurance} > 15) /call voodoo FALSE "${DiscMark}" disc TARGET FALSE
	/call cast_AA_spire
	/if (${is_ready[FALSE, "Intensity of the Resolute", ${Me.ID}]} && ${Raid.Members}) /call voodoo FALSE "Intensity of the Resolute" alt 0 FALSE
  /if (!${Me.ActiveDisc.ID}) {
  	/if (${is_ready[FALSE, "${maChr.Find[stFrenzy].Value}", ${Me.ID}]}) /call voodoo FALSE "${DiscFrenzy}" disc 0 FALSE
  	/delay 2
  }
  /if (${is_ready[FALSE, disc, "${maChr.Find[stTwistedChance].Value}", ${Me.ID}]} && !${Me.ActiveDisc.ID}) /call voodoo FALSE "${DiscTwistedChance}" disc 0 FALSE
  /if (${is_ready[FALSE, disc, "${maChr.Find[stEradicator].Value}", ${Me.ID}]} && !${Me.ActiveDisc.ID}) /call voodoo FALSE "${DiscEradicator}" disc 0 FALSE
  /if (${is_ready[FALSE, disc, "${maChr.Find[stRazor].Value}", ${Me.ID}]} && !${Me.ActiveDisc.ID}) /call voodoo FALSE "${DiscRazor}" disc 0 FALSE

  /if (${is_ready[FALSE, disc, "${maChr.Find[stAspBleeder].Value}", ${Me.ID}]} && !${Me.ActiveDisc.ID}) /call voodoo FALSE "${DiscAspBleeder}" disc 0 FALSE

	/call click_burn_circle
	/call click_item_burn

/return



|***
 * note: rogue specific controls
 * use: /chr
 ***|
sub set_control_ROG(string _type, string _verbage, string _silent, bool _debug)
	DEBUG \atset_control_ROG\ax(\a-w${_type}, "${_verbage}", ${_silent}\ax)
	
	/declare _out string local
	/declare _li listiterator local

	| output control map
	/if (${_type.Equal[see]}) {
		/call Bind_command_see maChr ${_verbage}
		/return
		
	| set ae on/off / count
	} else /if (${_type.Equal[ae]}) {
		/if (!${set_control_num_range[FALSE, stCountAE, "${_verbage}", 0, 99]}) /return

	| defensive mob agro count
	} else /if (${_type.Equal[def]}) {
		/if (!${set_control_num_range[FALSE, stCountDefensive, "${_verbage}", 0, 99]}) /return

	| set AA fade use %	
	} else /if (${_type.Equal[fade]}) {
		/if (!${set_control_num_range[FALSE, stPctFade, "${_verbage}", 0, 99]}) /return

	| set spire to be used
	} else /if (${_type.Equal[spire]}) {
		/call set_control_chr_spire FALSE ${_verbage} ${_silent}
		/return

	| auto SOS... or not...
	} else /if (${_type.Equal[sos]}) {
		/varset dump ${set_switch_env[${_debug}, swSoS, TRUE, ${_verbage}]}
		
	| use blinding disc
	} else /if (${_type.Equal[blind]}) {
		/varset dump ${set_switch_env[${_debug}, swBlinding, TRUE, ${_verbage}]}
		
	}

	/if (${_silent.Equal[SILENT]}) /return 
	/varset _out
	/vardata _li lsChr.First.Clone
	/while (!${_li.IsEnd}) {
		/varset _out ${_out} ${If[${_li.Value.Equal[${lsChr.First}]},,${dot}]} ${info}${_li.Value}\ax
		/if (${_li.Value.Equal[ae]}) /varset _out ${_out}:${If[${maChr.Find[stCountAE].Value},${num}${maChr.Find[stCountAE].Value}\ax,${r0}]}
		/if (${_li.Value.Equal[def]}) /varset _out ${_out}:${If[${maChr.Find[stCountDefensive].Value},${num}${maChr.Find[stCountDefensive].Value}\ax,${r0}]}
		/if (${_li.Value.Equal[fade]}) /varset _out ${_out}:${If[${maChr.Find[stPctFade].Value},${num}${maChr.Find[stPctFade].Value}\ax,${r0}]}
		/if (${_li.Value.Equal[spire]}) /varset _out ${_out}:${submenu}

		/if (${_li.Value.Equal[sos]}) /varset _out ${_out}:${If[${maEnv.Find[swSoS].Value},${on},${off}]}
		/if (${_li.Value.Equal[blind]}) /varset _out ${_out}:${If[${maChr.Find[swBlinding].Value},${on},${off}]}


		/varset dump ${_li.Advance}
	}	
	OUT /chr ${_out}

/return
