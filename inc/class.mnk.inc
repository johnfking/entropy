|***
 * entropy
 * class.mnk.inc
 ***|



|***
 * note: primary initilization of all monk non shared data
 * use: not for you
 ***|
sub set_MNK(bool _debug)
	DEBUG \atset_MNK\ax()

	/if (${maDebug.Find[init].Value}) {
		/declare _boottimestart int local ${MacroQuest.Running}
	}

	/call set_data ${_debug} maData stSpire															map outer "Spire of the Sensei"
	/call set_data ${_debug} maData stSynergy														map outer "Calanin's Synergy"
	/call set_data ${_debug} maData stEpic1															map outer "Celestial Fists"
	/call set_data ${_debug} maData stEpic15														map outer "Fistwraps of Celestial Discipline"
	/call set_data ${_debug} maData stEpic2															map outer "Transcended Fistwraps of Immortality"
	/call set_data ${_debug} maData stFade															map outer "Imitate Death"
	/call set_data ${_debug} maData stCompleteHeal											map outer "Complete Heal"
	/call set_data_map ${_debug} maChr stAAUnity												FALSE													"${auxna} class aa unity buff"

  /call set_data_map ${_debug} maChr stRest 													PREGEN|1											"${auxna} name of stamina rest disc"
 	/call set_data_map ${_debug} maChr swHiatus													FALSE													"${auxsw} use hiatus rest disc"
	/call set_data_map ${_debug} maChr swTrainMelee											FALSE													"${auxsw} alternate skills for training"

	/call set_data_map ${_debug} maChr stPctFDAggro											0															"${auxnum} % agro for fd"
	/call set_data_map ${_debug} maChr stPctFDHP												0															"${auxnum} % health fd at"

	/call set_data_map ${_debug} maChr stDichotomic											PREGEN|1											"${auxna} Dichotomic disc family"
	/call set_data_map ${_debug} maChr stBalance												PREGEN|1											"${auxna} Tiger's Balance disc family"
	/call set_data_map ${_debug} maChr stMonkey													PREGEN|1											"${auxna} Drunken Monkey Style disc family"
	/call set_data_map ${_debug} maChr stHeel														PREGEN|1											"${auxna} Heel of Kanji disc family"
	/call set_data_map ${_debug} maChr stDiscipline											PREGEN|1											"${auxna} Thunderkick Discipline disc family"
	/call set_data_map ${_debug} maChr stCrane													PREGEN|1											"${auxna} Crane Stance disc family"
	/call set_data_map ${_debug} maChr stPunch													PREGEN|1											"${auxna} Punch Through disc family"
	/call set_data_map ${_debug} maChr stSynergy												PREGEN|1											"${auxna} Shadewalker's Synergy disc family"
	/call set_data_map ${_debug} maChr stElbow													PREGEN|1											"${auxna} Elbow Strike disc family"
	/call set_data_map ${_debug} maChr stCurse													PREGEN|1											"${auxna} Curse of the Thirteen Fingers disc family"
	/call set_data_map ${_debug} maChr stPrecision											PREGEN|1											"${auxna} Doomwalker's Precision Strike disc family"
	/call set_data_map ${_debug} maChr stShuriken												PREGEN|1											"${auxna} Vigorous Shuriken disc family"
	/call set_data_map ${_debug} maChr stStorm													PREGEN|1											"${auxna} Eye of the Storm disc family"
	/call set_data_map ${_debug} maChr stDeath													PREGEN|1											"${auxna} Delay Death disc family"
	/call set_data_map ${_debug} maChr stWu															PREGEN|1											"${auxna} Fists of Wu disc family"

	/if (${maDebug.Find[init].Value}) {
		OUT \agset_${Me.Class.ShortName}\ax${sep}\a-w${Math.Calc[((${MacroQuest.Running}-${_boottimestart}) / 10) / 60]}s\ax
		/mqp
	}

/return TRUE



|***
 * note: kick 'em in the nuts
 * use:
 ***|
sub main_MNK(bool _debug)
	DEBUG \atmain_MNK\ax()

	GETINPUT
	ISMEDEAD
	CHECKTIE
	CHECKREZ

	/while (${check_combat_status[FALSE]}) {
		GETINPUT
		/if (${maHeal.Find[swHealSelf].Value} && !${Group.Members}) /call check_heal_${Me.Class.ShortName} ${maDebug.Find[self].Value} Self
		/if (!${timer_Check_Cure} && ${maChr.Find[swHealCure].Value}) /call check_cure ${maDebug.Find[cure].Value}
		/if (${is_defensive_active[${maDebug.Find[def].Value}, FALSE, SAFEENV]}) /call cast_defensive_cycle ${maDebug.Find[def].Value}
		/if (${check_class_loop[FALSE]}) /return

		/if (VALID) {
			/call set_combat FALSE
			/if (MELEE && !${timer_Check_Behind}) /call stick_to_target FALSE TARGET
			CHECKDEFENSE
			/call check_item_cycle ${maDebug.Find[item].Value} TARGET epic
			/call check_fd ${_debug}
			/call misc_combat_${Me.Class.ShortName} ${maDebug.Find[misc].Value} TARGET
			/if (BURN) /call burn_${Me.Class.ShortName} ${maDebug.Find[burn].Value} TARGET
			/if (MELEE) /call do_melee FALSE TARGET
			/if (!${timer_Check_deBuffs}) {
				/call check_deBuffs ${maDebug.Find[debuff].Value} TARGET
			}
			/call check_item_cycle ${maDebug.Find[item].Value} TARGET

		}

		/if (AUTO) {
			CHECKFADE
			/if (${lsClassMelee.Contains[${Me.Class.ShortName}]}) /call cast_stamina_recovery ${maDebug.Find[staminarecover].Value}
		}
	}

/return TRUE



|***
 * note: al the extra combat shit
 * use:
 ***|
sub misc_combat_MNK(bool _debug, int _tmpID)
	DEBUG \atmisc_combat_MNK\ax(\a-w${_tmpID}\ax)

	ISMEDEAD
	/if (${target[isdead, ${_tmpID}]}) {
		/return FALSE
	}


	CHECKEXIT
	GETINPUT
	CHECKTIE

	CHECKDEFENSE
	/if (SWARM || PET) /call send_pet ${maDebug.Find[sic].Value} ${_tmpID}
	/if (AUTO && MELEE) /call check_facing ${maDebug.Find[face].Value} ${_tmpID} FORCE

	/if (!AGRO) {
		/if (${is_ready[${_debug}, "Silent Strikes"]}) {
			/call voodoo ${_debug} "Silent Strikes" 0 FALSE
		}
	}

	/if (${is_ready[${_debug}, "${maChr.Find[stWu].Value}"]} && !${Me.Song[${maChr.Find[stWu].Value}].ID}) {
		/call voodoo ${_debug} "${maChr.Find[stWu].Value}" 0 FALSE
	}

	/if (${is_ready[${_debug}, "Zan Fi's Whistle"]}) {
		/call voodoo ${_debug} "Zan Fi's Whistle" 0 FALSE
	}

	/if (${is_ready[${_debug}, "${maChr.Find[stMonkey].Value}"]}) {
		/call voodoo ${_debug} "${maChr.Find[stMonkey].Value}" ${_tmpID} FALSE
	}

	/if (${maCC.Find[swIntimidation].Value} && ${Me.AbilityReady["Intimidation"]}) {
		/doability Intimidation
	}

	/if (${is_ready[${_debug}, "Two-Finger Wasp Touch"]}) {
		/call voodoo ${_debug} "Two-Finger Wasp Touch" ${_tmpID} FALSE
	}

	/if (${is_ready[${_debug}, "${maChr.Find[stPunch].Value}"]}) {
		/call voodoo ${_debug} "${maChr.Find[stPunch].Value}" ${_tmpID} FALSE
	}

	/if (${is_ready[${_debug}, "${maChr.Find[stSynergy].Value}"]}) {
		/call voodoo ${_debug} "${maChr.Find[stSynergy].Value}" ${_tmpID} FALSE
	}

	/if (${is_ready[${_debug}, "${maChr.Find[stElbow].Value}"]}) {
		/call voodoo ${_debug} "${maChr.Find[stElbow].Value}" ${_tmpID} FALSE
	}

	/if (${is_ready[${_debug}, "${maChr.Find[stCurse].Value}"]}) {
		/call voodoo ${_debug} "${maChr.Find[stCurse].Value}" ${_tmpID} FALSE
	}

	/if (${is_ready[${_debug}, "${maChr.Find[stPrecision].Value}"]}) {
		/call voodoo ${_debug} "${maChr.Find[stPrecision].Value}" ${_tmpID} FALSE
	}


	/if (${is_ready[${_debug}, "${maChr.Find[stShuriken].Value}"]} && ${Spawn[${_tmpID}].Distance} > 31) {
		/call voodoo ${_debug} "${maChr.Find[stShuriken].Value}" ${_tmpID} FALSE
	}

	/if (${is_ready[${_debug}, "${maChr.Find[stStorm].Value}"]}) {
		/call voodoo ${_debug} "${maChr.Find[stStorm].Value}" 0 FALSE
	}

	/call check_fd ${_debug}

/return TRUE



|***
 * note: healing
 * use: auto
 ***|
sub check_heal_MNK(bool _debug, string _type)
	DEBUG \atcheck_heal_MNK\ax(\a-w${_type}\ax)

	CHECKTIE
	ISMEDEAD
	CHECKEXIT
	CHECKREZ
	CHECKFADE

	SETHUD "heal ${_type}"

	| mend training
	/if (${maChr.Find[swTrainMelee].Value}) {
		/if (${Me.Skill[Mend]} < ${Me.SkillCap[Mend]} && ${Me.AbilityReady[Mend]}) {
			/doability Mend
		}
	}

	/if (!${get_hurt[${maDebug.Find[hurt].Value}, ${_type}]}) /return FALSE
	/if (${is_ready[${_debug}, "${maChr.Find[stDeath].Value}"]}) {
		/call voodoo ${_debug} "${maChr.Find[stDeath].Value}" 0 FALSE
	}

	/if (${Me.AbilityReady[Mend]}) {
		/doability Mend
		/return
	}

	/call cast_cycle_heal ${_debug} ${hurtID}

	SETHUD FALSE

/return TRUE



|***
 * note: Monk burn routines
 * use: you made a monk... hahaha
 ***|
sub burn_MNK(bool _debug, int _tmpID)
	DEBUG \atburn_MNK\ax(\a-w${_tmpID}\ax)

	ISMEDEAD
	/if (${target[isdead, ${_tmpID}]}) {
		/return FALSE
	}

	CHECKEXIT
	GETINPUT
	CHECKTIE

	/if (!${is_defensive_active[${maDebug.Find[def].Value}, BURN, SAFEENV]}) {
		/call cast_defensive_cycle ${maDebug.Find[def].Value}
	}
	/if (SWARM || PET) /call send_pet ${maDebug.Find[sic].Value} ${_tmpID}
	/if (AUTO && MELEE) /call check_facing ${maDebug.Find[face].Value} ${_tmpID} FORCE

	/if (${is_ready[${_debug}, "${maChr.Find[stDichotomic].Value}"]}) {
		/call voodoo ${_debug} "${maChr.Find[stDichotomic].Value}" 0 FALSE
	}

	/if (${is_ready[${_debug}, "${maChr.Find[stAlliance].Value}"]}) {
		/call voodoo ${_debug} "${maChr.Find[stAlliance].Value}" ${_tmpID} FALSE
	}

	/call cast_AA_spire ${maDebug.Find[spire].Value}

	/if (${is_ready[${_debug}, "${maChr.Find[stBalance].Value}"]}) {
		/call voodoo ${_debug} "${maChr.Find[stBalance].Value}" 0 FALSE
	}

	/if (${is_ready[${_debug}, "Infusion of Thunder"]}) {
		/call voodoo ${_debug} "Infusion of Thunder" 0 FALSE
	}

	/if (${is_ready[${_debug}, "Focused Destructive Force"]}) {
		/call voodoo ${_debug} "Focused Destructive Force" ${_tmpID} FALSE
	}

	/if (${is_ready[${_debug}, "${maChr.Find[stMonkey].Value}"]}) {
		/call voodoo ${_debug} "${maChr.Find[stMonkey].Value}" 0 FALSE
	}

	/if (${is_ready[${_debug}, "Silent Strikes"]}) {
		/call voodoo ${_debug} "Silent Strikes" 0 FALSE
	}

	/call check_fd ${_debug}

	| Check if we're using a disc, if not use Heel, if another, cancel it and use Heel
	/if (!${Me.ActiveDisc.ID}) {
		/if (${is_ready[${_debug}, "${maChr.Find[stHeel].Value}"]}) {
		/if (!${maCC.Find[swFlyingkick].Value} || !${maCC.Find[swEaglestrike].Value}) {
			/invoke ${maCC.Add[swKick,FALSE]}
			/invoke ${maCC.Add[swFlyingkick,TRUE]}
			/invoke ${maCC.Add[swRoundkick,FALSE]}
			/invoke ${maCC.Add[swDragonpunch,FALSE]}
			/invoke ${maCC.Add[swEaglestrike,TRUE]}
			/invoke ${maCC.Add[swTigerclaw,FALSE]}
		}
		/call voodoo ${_debug} "${maChr.Find[stHeel].Value}" TARGET FALSE
		}
	} else /if (${Me.ActiveDisc.ID} && !${Me.ActiveDisc.Name.Find[Heel]}) {
		/if (${is_ready[${_debug}, "${maChr.Find[stHeel].Value}"]}) {
			/stopdisc
			/if (!${maCC.Find[swFlyingkick].Value} || !${maCC.Find[swEaglestrike].Value}) {
				/invoke ${maCC.Add[swKick,FALSE]}
				/invoke ${maCC.Add[swFlyingkick,TRUE]}
				/invoke ${maCC.Add[swRoundkick,FALSE]}
				/invoke ${maCC.Add[swDragonpunch,FALSE]}
				/invoke ${maCC.Add[swEaglestrike,TRUE]}
				/invoke ${maCC.Add[swTigerclaw,FALSE]}
			}
			/call voodoo ${_debug} "${maChr.Find[stHeel].Value}" ${_tmpID} FALSE
		}
	}
	/if (${is_ready[${_debug}, "${maChr.Find[stDiscipline].Value}"]}) {
		/call voodoo ${_debug} "${maChr.Find[stDiscipline].Value}" FALSE
	}

	/if (${is_ready[${_debug}, "${maChr.Find[stCrane].Value}"]}) {
		/call voodoo ${_debug} "${maChr.Find[stCrane].Value}" ${_tmpID} FALSE
	}

	/if (${is_ready[${_debug}, "Five Point Palm"]}) {
		/call voodoo ${_debug} "Five Point Palm" ${_tmpID} FALSE
	}

	/if (${is_ready[${_debug}, "Swift Tail's Chant"]}) {
		/call voodoo ${_debug} "Swift Tail's Chant" 0 FALSE
	}

  /call cast_glyph ${maDebug.Find[glyph].Value}

/return TRUE



|***
 * note: swap melee skills to train them
 * use: /call melee_train_MNK DEBUG
 ***|
sub melee_train_MNK(bool _debug)
	DEBUG \atmelee_train_MNK\ax()

| This code catches the call in the skillup sub and returns out using most efficient skills if not training melee
	/if (!${maChr.Find[swTrainMelee].Value}) {
		/invoke ${maCC.Add[swKick,FALSE]}
		/invoke ${maCC.Add[swFlyingkick,TRUE]}
		/invoke ${maCC.Add[swRoundkick,FALSE]}
		/invoke ${maCC.Add[swDragonpunch,FALSE]}
		/invoke ${maCC.Add[swEaglestrike,TRUE]}
		/invoke ${maCC.Add[swTigerclaw,FALSE]}
		/return TRUE
	}

	| kick skills
	/if (${Me.Skill[Flying Kick]} <= ${Me.Skill[Kick]} && ${Me.Skill[Flying Kick]} <= ${Me.Skill[Round Kick]}) {
		/invoke ${maCC.Add[swKick,FALSE]}
		/invoke ${maCC.Add[swFlyingkick,TRUE]}
		/invoke ${maCC.Add[swRoundkick,FALSE]}
	} else /if (${Me.Skill[Kick]} <= ${Me.Skill[Flying Kick]} && ${Me.Skill[Kick]} <= ${Me.Skill[Round Kick]}) {
		/invoke ${maCC.Add[swKick,TRUE]}
		/invoke ${maCC.Add[swFlyingkick,FALSE]}
		/invoke ${maCC.Add[swRoundkick,FALSE]}
	} else /if (${Me.Skill[Round Kick]} <= ${Me.Skill[Kick]} && ${Me.Skill[Round Kick]} <= ${Me.Skill[Flying Kick]}) {
		/invoke ${maCC.Add[swKick,FALSE]}
		/invoke ${maCC.Add[swFlyingkick,FALSE]}
		/invoke ${maCC.Add[swRoundkick,TRUE]}
	}

	| punch skills
	/if (${Me.Skill[Eagle Strike]} <= ${Me.Skill[Dragon Punch]} && ${Me.Skill[Eagle Strike]} <= ${Me.Skill[Tiger Claw]}) {
		/invoke ${maCC.Add[swDragonpunch,FALSE]}
		/invoke ${maCC.Add[swEaglestrike,TRUE]}
		/invoke ${maCC.Add[swTigerclaw,FALSE]}
	} else /if (${Me.Skill[Tiger Claw]} <= ${Me.Skill[Dragon Punch]} && ${Me.Skill[Tiger Claw]} <= ${Me.Skill[Eagle Strike]}) {
		/invoke ${maCC.Add[swDragonpunch,FALSE]}
		/invoke ${maCC.Add[swEaglestrike,FALSE]}
		/invoke ${maCC.Add[swTigerclaw,TRUE]}
	} else /if (${Me.Skill[Dragon Punch]} <= ${Me.Skill[Eagle Strike]} && ${Me.Skill[Dragon Punch]} <= ${Me.Skill[Tiger Claw]}) {
		/invoke ${maCC.Add[swDragonpunch,TRUE]}
		/invoke ${maCC.Add[swEaglestrike,FALSE]}
		/invoke ${maCC.Add[swTigerclaw,FALSE]}
	}

/return TRUE



#event MNKfdcheck "You have fallen to the ground"
sub Event_MNKfdcheck(string line, string _verbage)

	/if (!${validate_class[FALSE, |MNK]}) /return FALSE

	/if (${Me.State.Equal[FEIGN]} && !${swIsMeDead}) {
		/if (${Me.AltAbilityReady["${maChr.Find[stFade].Value}"]}) {
			/if (${is_ready[${_debug}, "${maChr.Find[stFade].Value}"]}) {
				/call voodoo ${_debug} "${maChr.Find[stFade].Value}" 0 FALSE
			}
			/return
		}
		/stand
		/if (${Me.AbilityReady[Mend]}) /doability Mend
		/if (${is_ready[${_debug}, "${maChr.Find[stDeath].Value}"]}) {
			/call voodoo ${_debug} "${maChr.Find[stDeath].Value}" 0 FALSE
		}
	}

/return FALSE



|***
 * note: To FD when hitpoints get low, cancel combat if driving, or stand up if healthy
 * use:
 ***|
sub check_fd(bool _debug)
	DEBUG \atcheck_fd\ax()

	ISMEDEAD

	/if (${Me.Combat} && ${Me.PctHPs} <= ${maChr.Find[stPctFDHP].Value}) {
		/if (!${Me.State.Equal[FEIGN]} && !${swIsMeDead}) {
			/if (${Me.AbilityReady[Feign Death]}) {
				/attack off
				/doability "Feign Death"
				/delay 5
			}
			/doevents MNKfdcheck
			/if (!AUTO) {
				/call set_stop FALSE SILENT
				/call clear_combat FALSE
				/return
			}
		}
	} else /if (${maChr.Find[stPctFDAggro].Value} && !AGRO && AUTO && ${Me.PctAggro} >= ${maChr.Find[stPctFDAggro].Value}) {
		/if (!${Me.State.Equal[FEIGN]} && !${swIsMeDead}) {
			/if (${Me.AbilityReady[Feign Death]}) {
				/attack off
				/doability "Feign Death"
				/delay 5
			}

			/doevents MNKfdcheck

			/if (!AUTO) {
				/call set_stop FALSE SILENT
				/call clear_combat FALSE
				/return
			}
		}
	}

	/if (${Me.State.Equal[FEIGN]}) {
		/if (${Me.PctHPs} > ${maChr.Find[stPctFDHP].Value} && AUTO && ${Me.PctAggro} <= ${maChr.Find[stPctFDAggro].Value}) /stand
	}

/return TRUE



|***
 * note: monk specific controls
 * use: /chr
 ***|
sub set_control_MNK(string _type, string _verbage, string _silent, bool _debug)
	DEBUG \atset_control_MNK\ax(\a-w${_type}, "${_verbage}", ${_silent}\ax)

	/declare _out string local

	| output control map
	/if (${_type.Equal[see]}) {
		/call Bind_command_see maChr ${_verbage}
		/return

	| set ae on/off / count
	} else /if (${_type.Equal[ae]}) {
		/if (!${set_control_num_range[${_debug}, stCountAE, "${_verbage}", 0, 99]}) /return

	| defensive mob agro count
	} else /if (${_type.Equal[def]}) {
		/if (!${set_control_num_range[${_debug}, stCountDefensive, "${_verbage}", 0, 99]}) /return

	| set AA fade use %
	} else /if (${_type.Equal[fade]}) {
		/if (!${set_control_num_range[${_debug}, stPctFade, "${_verbage}", 0, 99]}) /return

	| set spire to be used
	} else /if (${_type.Equal[spire]}) {
		/call set_control_chr_spire ${_debug} ${_verbage} ${_silent}
		/return

	| use hiatus disc
	} else /if (${_type.Equal[hiatus]}) {
		/invoke ${set_switch_env[${_debug}, swHiatus, TRUE, ${_verbage}]}

	| train skills
	} else /if (${_type.Equal[train]}) {
		/invoke ${set_switch_env[${_debug}, swTrainMelee, TRUE, ${_verbage}]}

	| set AA fade use %
	} else /if (${_type.Equal[fdhp]}) {
		/if (!${set_control_num_range[${_debug}, stPctFDHP, "${_verbage}", 0, 99]}) /return

	| set AA fade use %
	} else /if (${_type.Equal[fdaggro]}) {
		/if (!${set_control_num_range[${_debug}, stPctFDAggro, "${_verbage}", 0, 99]}) /return

	}

	/if (${_silent.Equal[SILENT]}) /return
	/invoke ${set_control_output[${_debug}, maChr, chr, maChrControl]}

/return



