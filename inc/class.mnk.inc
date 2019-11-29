|***
 * entropy
 * class.mnk.inc
 ***|
 


|***
 * note: primary initilization of all cleric non shared data
 * use: not for you
 ***|
sub set_MNK(bool _debug)
	DEBUG \atset_MNK\ax()

	/call set_data FALSE maData stSpire																	map outer "Spire of the Sensei"
	/call set_data FALSE maData stSynergy																map outer "Calanin's Synergy"
	/call set_data FALSE maData stEpic1																	map outer "Celestial Fists"
	/call set_data FALSE maData stEpic15																map outer "Fistwraps of Celestial Discipline"
	/call set_data FALSE maData stEpic2																	map outer "Transcended Fistwraps of Immortality"
	/call set_data FALSE maData stFade																	map outer "Imitate Death"
	/call set_data FALSE maData stCompleteHeal													map outer "Complete Heal"
	/call set_data_map FALSE maChr stAAUnity														FALSE													"${auxna} class aa unity buff"


  /call set_data_map FALSE maChr stRest 															PREGEN|1											"${auxna} name of stamina rest disc"
 	/call set_data_map FALSE maChr swHiatus															FALSE													"${auxsw} use hiatus rest disc"




	
	/call sql_control FALSE ${chrID} switchMNKTrainMelee								bool			outer FALSE

	/call sql_control FALSE ${chrID} setFDAgroPct												int				outer FALSE
	/call sql_control FALSE ${chrID} setAgroPct													int				outer 95
	/call sql_control FALSE ${chrID} setFDPct														int				outer 25
 	
	/call set_data_map FALSE maChr stDichotomic													PREGEN|1											"${auxna} Dichotomic disc family"
	
	
	/call sql_control FALSE ${chrID} stBalance												PREGEN|1
	/call sql_control FALSE ${chrID} maChr.Find[stMonkey].Value													PREGEN|1
	/call sql_control FALSE ${chrID} maChr.Find[stHeel].Value														PREGEN|1
	/call sql_control FALSE ${chrID} maChr.Find[stDiscipline].Value											PREGEN|1
	/call sql_control FALSE ${chrID} maChr.Find[stCrane].Value													PREGEN|1
	/call sql_control FALSE ${chrID} maChr.Find[stPunch].Value													PREGEN|1
	/call sql_control FALSE ${chrID} maChr.Find[stSynergy].Value												PREGEN|1
	/call sql_control FALSE ${chrID} maChr.Find[stElbow].Value														PREGEN|1
	/call sql_control FALSE ${chrID} maChr.Find[stCurse].Value													PREGEN|1
	/call sql_control FALSE ${chrID} maChr.Find[stPrecision].Value												PREGEN|1
	/call sql_control FALSE ${chrID} maChr.Find[stShuriken].Value												PREGEN|1
	/call sql_control FALSE ${chrID} maChr.Find[stStorm].Value													PREGEN|1
	/call sql_control FALSE ${chrID} maChr.Find[stDeath].Value											PREGEN|1
	/call sql_control FALSE ${chrID} maChr.Find[stWu].Value															PREGEN|1
	











stAlliance "Doomwalker's Alliance"
stDichotomic "Dissident Form|Dichotomic Form"
stBalance "Tiger's Poise|Dragon's Balance|Eagle's Balance|92|Tiger's Balance"
maChr.Find[stMonkey].Value "Drunken Monkey Style"
maChr.Find[stHeel].Value "Heel of Zagali|Heel of Kojai|Hheel of Kai|70|Heel of Kanji"
maChr.Find[stDiscipline].Value "Ironfist Discipline|Scaledfist Discipline|Ashenhand Discipline|Silentfist Discipline|Thunderkick Discipline"
maChr.Find[stCrane].Value "Crane Stance"
maChr.Find[stPunch].Value "Firestorm of Fists|Torrent of Fists|Strike Through|Jab Through|Punch Through"
maChr.Find[stSynergy].Value "Firewalker's Synergy|Doomwalker's Synergy|Shadewalker's Synergy"
maChr.Find[stElbow].Value "Hoshkar's Fang|Zalikor's Fang|Dragon Fang|Leopard Claw|Elbow Strike"
maChr.Find[stCurse].Value "Curse of Fourteen Fists|Curse of the Thirteen Fingers"
maChr.Find[stPrecision].Value "Firewalker's Precision Strike|Doomwalker's Precision Strike"
maChr.Find[stShuriken].Value "Vigorous Shuriken"
maChr.Find[stStorm].Value "Eye of the Storm"
stRest "Breather|Seventh Wind|Reprieve|Rest|Sixth Wind|Fifth Wind|Respite|Fourth Wind|Third Wind|Second Wind"
maChr.Find[stDeath].Value "Reject Death|Refuse Death|Forestall Death|Decry Death|Deny Death|Defer Death|Delay Death"
maChr.Find[stWu].Value "Fists of Wu"





/return




 

 
 | spire of the sensei


sub core_MNK(bool _debug)
	DEBUG core_MNK()

	/if (${coreAuto}) {
		/call check_fd
	}

	/while (${prep_combat[]}) {
		CHECKINPUTS
		/if (${swHealSelf}) /call patch_heal_${Me.Class.ShortName}
		/if (!${timer_check_cast_cure} && ${swHealCure}) /call cast_cure FALSE
		/if (${exit_class_loop[]}) /return

		/if (${isValidTarget}) {
			/call set_combat ${combat.Upper}
			/if (${validate_mitigation[FALSE, burn, ${setCombatRadius}]}) /call mitigation_cycle FALSE
			/if (${swBurnControl}) /call burn_${Me.Class.ShortName}
			/if (${swSwarm} || ${swPet}) /call send_pet FALSE ${Target2Kill}
			/if (${swCombatMelee}) /call use_skill_melee ${Target2Kill}
			/call disc_${Me.Class.ShortName}
			/if (${swAlliance}) /call trigger_alliance ${Target2Kill}
			/call check_deBuffs FALSE ${Target2Kill}
			/call click_BP
			/call click_item
			/if (${swSwarm}) /call click_item_swarm FALSE ${Target2Kill}
			/call click_DPS_${Me.Class.ShortName}
		}

		/if (${coreAuto}) {	
			/if (${validate_class[FALSE, ${lstClassMelee}]}) /call cast_stamina_recovery
		}
	}
/return	
	
	
	
	
|***
 *	DES: 
 *	USE: 
 *  NOTE: 
 ***|
sub disc_MNK(bool _debug)
	DEBUG disc_MNK()
	CHECKEXIT
	CHECKINPUTS
	/if (${is_target_dead[${Target2Kill}]}) /return
	/call check_deBuffs FALSE ${Target2Kill}
	/call get_agro_${Me.Class.ShortName}

	/if (${validate_mitigation[FALSE, burn, ${setCombatRadius}]}) /call mitigation_cycle FALSE

	/if (${Me.PctAggro} >= ${setAgroPct}) {
		/if (${validate_cast[FALSE, alt, "Silent Strikes", ${Me.ID}]}) /call core_cast2 "Silent Strikes" alt 0 FALSE
	}
	/if (${validate_cast[FALSE, disc, "${maChr.Find[stWu].Value}", ${Me.ID}]} && !${Me.Song[${maChr.Find[stWu].Value}].ID}) /call core_cast2 "${maChr.Find[stWu].Value}" disc 0 FALSE
	| /if (${validate_cast[FALSE, disc, "${DiscDoubleAttack}", ${Me.ID}]}) /call core_cast2 "${DiscDoubleAttack}" disc 0 FALSE
	/if (${validate_cast[FALSE, alt, "Zan Fi's Whistle", ${Me.ID}]}) /call core_cast2 "Zan Fi's Whistle" alt 0 FALSE
	/if (${validate_cast[FALSE, disc, "${maChr.Find[stMonkey].Value}", ${Target2Kill}]}) /call core_cast2 "${maChr.Find[stMonkey].Value}" disc ${Target2Kill} FALSE	
	/if (${Bool[${intimidation}]} && ${Me.AbilityReady["Intimidation"]}) /doability Intimidation
	/if (${validate_cast[FALSE, alt, "Two-Finger Wasp Touch", ${Target2Kill}]}) /call core_cast2 "Two-Finger Wasp Touch" alt ${Target2Kill} FALSE   
	/if (${validate_cast[FALSE, disc, "${maChr.Find[stPunch].Value}", ${Target2Kill}]}) /call core_cast2 "${maChr.Find[stPunch].Value}" disc ${Target2Kill} FALSE
	/if (${validate_cast[FALSE, disc, "${maChr.Find[stSynergy].Value}", ${Target2Kill}]}) /call core_cast2 "${maChr.Find[stSynergy].Value}" disc ${Target2Kill} FALSE
	/if (${validate_cast[FALSE, disc, "${maChr.Find[stElbow].Value}", ${Target2Kill}]}) /call core_cast2 "${maChr.Find[stElbow].Value}" disc ${Target2Kill} FALSE
	/if (${validate_cast[FALSE, disc, "${maChr.Find[stCurse].Value}", ${Target2Kill}]}) /call core_cast2 "${maChr.Find[stCurse].Value}" disc ${Target2Kill} FALSE
	/if (${validate_cast[FALSE, disc, "${maChr.Find[stPrecision].Value}", ${Target2Kill}]}) /call core_cast2 "${maChr.Find[stPrecision].Value}" disc ${Target2Kill} FALSE
	/if (${validate_cast[FALSE, disc, "${maChr.Find[stShuriken].Value}", ${Target2Kill}]}) /call core_cast2 "${maChr.Find[stShuriken].Value}" disc ${Target2Kill} FALSE
	/if (${validate_cast[FALSE, disc, "${maChr.Find[stStorm].Value}", ${Me.ID}]}) /call core_cast2 "${maChr.Find[stStorm].Value}" disc 0 FALSE

/return



|***
 *	DES: emergency healing
 *	USE: /call patch_heal_${Me.Class.ShortName}
 *  NOTE: 
 ***|
sub patch_heal_MNK(bool _debug)
	DEBUG patch_heal_MNK()
	CHECKEXIT
	AMIDEAD
	
	| mend training
	/if (${switchMNKTrainMelee}) {
		/if (${Me.Skill[Mend]} < ${Me.SkillCap[Mend]} && ${Me.AbilityReady[Mend]}) {
			/doability Mend
			/return
		}
	}

	/if (${Me.PctHPs} > ${${Me.Class.ShortName}Point}*.${tmpHealOverride}) /return

	/if (${Me.PctHPs} <= ${${Me.Class.ShortName}Point}*.${tmpHealOverride} && ${Me.AbilityReady[Mend]}) {
		/doability Mend
		/return
	}

	/if (${Me.PctHPs} <= ${${Me.Class.ShortName}Point}*.${tmpHealOverride}) {
		/if (${validate_cast[FALSE, disc, "${maChr.Find[stDeath].Value}", ${Me.ID}]}) /call core_cast2 "${maChr.Find[stDeath].Value}" disc 0 FALSE
	}
	/call click_item_heal
/return



|***
 *	DES: DPS Clickies
 *	USE: /call click_DPS_${Me.Class.ShortName}
 *  NOTE: 
 ***|
sub click_DPS_MNK(bool _debug)
	DEBUG click_DPS_MNK()
	CHECKEXIT
	CHECKINPUTS
	/call click_horn_of_unity 	
/return



|***
 *	DES: Agro Routines
 *	USE: /call get_agro_${Me.Class.ShortName}
 *  NOTE: manual/Named/raid. all sorts...
 ***|
sub get_agro_MNK(bool _debug)
	DEBUG get_agro_MNK()
	CHECKEXIT
/return



 |***
 *	DES: Monk burn routines
 *	USE: establish all comp/trigger skills
 *	NOTE: 
 ***| 
sub burn_MNK(bool _debug)
	DEBUG burn_MNK()
	CHECKEXIT
	CHECKINPUTS
	
	/if (${validate_mitigation[FALSE, FALSE, ${setCombatRadius}]}) /call mitigation_cycle FALSE
	/if (${validate_cast[FALSE, disc, "${stDichotomic}", ${Me.ID}]}) /call core_cast2 "${stDichotomic}" disc 0 FALSE
	/if (${validate_cast[FALSE, disc, "${MNKAlliance}", ${Target2Kill}]}) /call core_cast2 "${MNKAlliance}" disc ${Target2Kill} FALSE 
	/call cast_AA_spire
	/if (${validate_cast[FALSE, disc, "${maChr.Find[stBalance].Value}", ${Me.ID}]}) /call core_cast2 "${maChr.Find[stBalance].Value}" disc 0 FALSE
	/if (${validate_cast[FALSE, alt, "Infusion of Thunder", ${Me.ID}]}) /call core_cast2 "Infusion of Thunder" alt 0 FALSE
	/if (${validate_cast[FALSE, alt, "Focused Destructive Force", ${Target2Kill}]}) /call core_cast2 "Focused Destructive Force" alt ${Target2Kill} FALSE
	/if (${validate_cast[FALSE, disc, "${maChr.Find[stMonkey].Value}", ${Me.ID}]}) /call core_cast2 "${maChr.Find[stMonkey].Value}" disc 0 FALSE
	/if (${validate_cast[FALSE, alt, "Silent Strikes", ${Me.ID}]}) /call core_cast2 "Silent Strikes" alt 0 FALSE
	
| 	Check if we're using a disc, if not use Heel, if another, cancel it and use Heel

	/if (!${Bool[${Me.ActiveDisc.Name}]}) {
		/if (${validate_cast[FALSE, disc, "${maChr.Find[stHeel].Value}", ${Target2Kill}]}) {
		/if (!${flyingkick} || !${eaglestrike}) {
			/varset kick FALSE
			/varset flyingkick TRUE
			/varset roundkick FALSE
			/varset dragonpunch FALSE
			/varset eaglestrike TRUE
			/varset tigerclaw FALSE
		}
		/call core_cast2 "${maChr.Find[stHeel].Value}" disc ${Target2Kill} FALSE
		}
	} else /if (${Bool[${Me.ActiveDisc.Name}]} && !${Bool[${Me.ActiveDisc.Name.Find[Heel]}]}) {
		/if (${validate_cast[FALSE, disc, "${maChr.Find[stHeel].Value}", ${Target2Kill}]}) {
			/stopdisc
			/if (!${flyingkick} || !${eaglestrike}) {
				/varset kick FALSE
				/varset flyingkick TRUE
				/varset roundkick FALSE
				/varset dragonpunch FALSE
				/varset eaglestrike TRUE
				/varset tigerclaw FALSE
			}
			/call core_cast2 "${maChr.Find[stHeel].Value}" disc ${Target2Kill} FALSE
		}
	}
	/if (${validate_cast[FALSE, disc, "${maChr.Find[stDiscipline].Value}", ${Me.ID}]}) /call core_cast2 "${maChr.Find[stDiscipline].Value}" disc 0 FALSE
	/if (${validate_cast[FALSE, disc, "${maChr.Find[stCrane].Value}", ${Target2Kill}]}) /call core_cast2 "${maChr.Find[stCrane].Value}" disc ${Target2Kill} FALSE
	/if (${validate_cast[FALSE, alt, "Five Point Palm", ${Target2Kill}]}) /call core_cast2 "Five Point Palm" alt ${Target2Kill} FALSE
	/if (${validate_cast[FALSE, alt, "Swift Tail's Chant", ${Me.ID}]}) /call core_cast2 "Swift Tail's Chant" alt 0 FALSE    

  /call use_glyph
  /call click_burn_circle
  /call click_item_burn
/return



|***
 *	DES: swap melee skills to train them
 *	USE: /mnk trainmelee | /call melee_train_MNK
 *  NOTE:
 ***|
sub melee_train_MNK(bool _debug)
	DEBUG melee_train_MNK()
	
| This code catches the call in the skillup sub and returns out using most efficient skills if not training melee
	/if (!${switchMNKTrainMelee}) {
		/varset kick FALSE
		/varset flyingkick TRUE
		/varset roundkick FALSE
		/varset dragonpunch FALSE
		/varset eaglestrike TRUE
		/varset tigerclaw FALSE
		/return
	}

	| kick skills
	/if (${Me.Skill[Flying Kick]} <= ${Me.Skill[Kick]} && ${Me.Skill[Flying Kick]} <= ${Me.Skill[Round Kick]}) {
		/varset kick FALSE
		/varset flyingkick TRUE
		/varset roundkick FALSE
	} else /if (${Me.Skill[Kick]} <= ${Me.Skill[Flying Kick]} && ${Me.Skill[Kick]} <= ${Me.Skill[Round Kick]}) {
		/varset kick TRUE
		/varset flyingkick FALSE
		/varset roundkick FALSE
	} else /if (${Me.Skill[Round Kick]} <= ${Me.Skill[Kick]} && ${Me.Skill[Round Kick]} <= ${Me.Skill[Flying Kick]}) {
		/varset kick FALSE
		/varset flyingkick FALSE
		/varset roundkick TRUE
	}

	| punch skills
	/if (${Me.Skill[Eagle Strike]} <= ${Me.Skill[Dragon Punch]} && ${Me.Skill[Eagle Strike]} <= ${Me.Skill[Tiger Claw]}) {
		/varset dragonpunch FALSE
		/varset eaglestrike TRUE
		/varset tigerclaw FALSE
	} else /if (${Me.Skill[Tiger Claw]} <= ${Me.Skill[Dragon Punch]} && ${Me.Skill[Tiger Claw]} <= ${Me.Skill[Eagle Strike]}) {
		/varset dragonpunch FALSE
		/varset eaglestrike FALSE
		/varset tigerclaw TRUE
	} else /if (${Me.Skill[Dragon Punch]} <= ${Me.Skill[Eagle Strike]} && ${Me.Skill[Dragon Punch]} <= ${Me.Skill[Tiger Claw]}) {
		/varset dragonpunch TRUE
		/varset eaglestrike FALSE
		/varset tigerclaw FALSE
	}
/return



#event MNKfdcheck "You have fallen to the ground"
sub Event_MNKfdcheck(string line, string _verbage)
	/if (!${validate_class[FALSE, |MNK]}) /return
	/if (${Me.State.Equal[FEIGN]} && !${swAmIDead}) {
		/if (${Me.AltAbilityReady["${MNKAAFade}"]}) {
			/if (${validate_cast[FALSE, alt, "${MNKAAFade}", ${Me.ID}]}) /call core_cast2 "${MNKAAFade}" alt 0 FALSE
			/return
		}
		/stand
		/if (${Me.AbilityReady["Mend"]}) /doability Mend
		/if (${validate_cast[FALSE, disc, "${maChr.Find[stDeath].Value}", ${Me.ID}]}) /call core_cast2 "${maChr.Find[stDeath].Value}" disc 0 FALSE
	}
/return



|***
 *  DES: To FD when hitpoints get low, cancel combat if driving, or stand up if healthy
 *  USE: 
 *  NOTE: 
 ***|   
sub check_fd(bool _debug)
	DEBUG check_fd()
	AMIDEAD
	/if (${Me.Combat} && ${Me.PctHPs} <= ${setFDPct}) {
		/if (!${Me.State.Equal[FEIGN]} && !${swAmIDead}) {
			/if (${Me.AbilityReady[Feign Death]}) {
				/attack off
				/doability "Feign Death"
				/delay 5
			}
			/doevents MNKfdcheck
			/if (!${coreAuto}) {
				/call set_stop
				/call clear_combat
				/return
			}
		}
	}

	/if (${Me.State.Equal[FEIGN]}) {
		/if (${Me.PctHPs} > ${setFDPct} && ${coreAuto} && ${Me.PctAggro} <= ${setAgroPct}) /stand
	}
/return 



|***
 *	DES: Class controls
 *	USE: /mnk []
 *  NOTE:
 ***|
#bind setMNK /mnk
sub Bind_setMNK(string _type, string _verbage, bool _debug)
	/if (!${validate_class[TRUE, |MNK]}) /return

	/declare _sep bool local
	/declare _listout string local
	/declare _count int local 0
	/declare _bind string local ${Me.Class.ShortName.Lower}
  /declare _lstClass string	local |ae|agpct|defcount|fdpct|healpoint|spiregroup|spireraid|trainmelee

	| set FD HP PCT
	/if (${_type.Equal[fdpct]}) {
		/if (${Range.Between[1,99:${Int[${_verbage}]}]}) {
			/call sql_update FALSE ${chrID} setFDAgroPct ${_verbage}
		} else /if (!${Range.Between[1,99:${Int[${_verbage}]}]}) {
			VOUT /${_bind} fdpct(${cnum}${setFDAgroPct}\ax) allowable range ${sep} ${cinfo}1 - 99\ax
			/return
		}	
		
	| set FD agro PCT
	} else /if (${_type.Equal[agpct]}) {
		/if (${Range.Between[1,99:${Int[${_verbage}]}]}) {
			/call sql_update FALSE ${chrID} setAgroPct ${_verbage}
		} else /if (!${Range.Between[1,99:${Int[${_verbage}]}]}) {
			VOUT /${_bind} agpct(${cnum}${setAgroPct}\ax) allowable range ${sep} ${cinfo}1 - 99\ax
			/return
		}

	| spire control
	} else /if (${_type.Equal[spiregroup]} || ${_type.Equal[spireraid]}) {
		/call set_character_spire FALSE ${_type} ${_verbage}

	| healpoint
	} else /if (${_type.Equal[healpoint]}) {
		/call set_character_healpoint FALSE ${_verbage}
		/return

	| enable/disable training melee punches and kicks
	} else /if (${_type.Equal[trainmelee]}) {
		/call set_switch ${chrID} switchMNKTrainMelee ${_verbage}

	| defensive mob agro count
	} else /if (${_type.Equal[defcount]}) {
		/call set_character_defcount FALSE ${_verbage}
		
	| set ae on/off / count
	} else /if (${_type.Equal[ae]}) {
		/call set_character_ae FALSE ${_verbage}
		
	}
	
	/if (${swMQClear}) /mqclear
	/for _count 1 to ${_lstClass.Count[|]}
		/varset _listout ${_listout} ${If[${_sep},${sep},]} ${cinfo}${_lstClass.Arg[${_count},|]}\ax
		/if (${_lstClass.Arg[${_count},|].Equal[ae]}) /varset _listout ${_listout}:${If[${AEMelee},${cnum}${AEMelee}\ax,${ooff}]}
		/if (${_lstClass.Arg[${_count},|].Equal[agpct]}) /varset _listout ${_listout}:${cnum}${setAgroPct}\ax
		/if (${_lstClass.Arg[${_count},|].Equal[defcount]}) /varset _listout ${_listout}:${If[${defCount},${cnum}${defCount}\ax,${ooff}]}
		/if (${_lstClass.Arg[${_count},|].Equal[fdpct]}) /varset _listout ${_listout}:${cnum}${setFDAgroPct}\ax
		/if (${_lstClass.Arg[${_count},|].Equal[healpoint]}) /varset _listout ${_listout}:${cnum}${${Me.Class.ShortName}Point}\ax
		/if (${_lstClass.Arg[${_count},|].Equal[spiregroup]}) /varset _listout ${_listout}:${If[${spireGroup},${cnum}${spireGroup}\ax,${ooff}]}
		/if (${_lstClass.Arg[${_count},|].Equal[spireraid]}) /varset _listout ${_listout}:${If[${spireRaid},${cnum}${spireRaid}\ax,${ooff}]}
		/if (${_lstClass.Arg[${_count},|].Equal[trainmelee]}) /varset _listout ${_listout}:${If[${switchMNKTrainMelee},${oon},${ooff}]}
		/varset _sep TRUE
	/next _count
	VOUT /${_bind} [${_listout}]

/return

