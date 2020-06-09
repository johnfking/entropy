|***
 * entropy
 * class.shd.inc
 ***|



|***
 * note: primary initilization of all cleric non shared data
 * use: not for you
 ***|
sub set_SHD(bool _debug)
	DEBUG \atset_SHD\ax()

	/if (${maDebug.Find[init].Value}) {
		/declare _boottimestart int local ${MacroQuest.Running}
	}

	/call set_data ${_debug} maData stSpire															map outer "Spire of the Reavers"
	/call set_data ${_debug} maData stSynergy														map outer "Knight's Synergy"
	/call set_data ${_debug} maData stEpic1															map outer "Innoruuk's Curse"
	/call set_data ${_debug} maData stEpic15														map outer "Innoruuk's Voice"
	/call set_data ${_debug} maData stEpic2															map outer "Innoruuk's Dark Blessing"
	/call set_data ${_debug} maData stFade															map outer "Death Peace"

 	/call set_data_map ${_debug} maChr stRest 													PREGEN|1											"${auxna} name of stamina rest disc"
 	/call set_data_map ${_debug} maChr stHiatus													PREGEN|1											"${auxna} name of hiatus stamina recovery disc"
 	/call set_data_map ${_debug} maChr swHiatus													FALSE													"${auxsw} use hiatus rest disc"

 	/call set_data_map ${_debug} maChr stAlliance 											PREGEN|1											"${auxna} alliance spell .. damn useless thing"

	/invoke ${set_data_timer[${_debug}, Check_Harmonious, 1m]}
	/call set_data_map ${_debug} maChr swHarmonious 										FALSE													"${auxsw} use harmonious"
 	/call set_data_map ${_debug} maChr stHarmonious 										PREGEN|1											"${auxna} name of the harmonions spell"

	| temp combat buffs (self only)
 	/call set_data_map ${_debug} maChr stStance 												PREGEN|1											"${auxna} name of the self stance buff"
 	/call set_data_map ${_debug} maChr stSkin 													PREGEN|1											"${auxna} name of the steel self buff"

	/call set_data_map ${_debug} maChr swAttraction											FALSE													"${auxsw} use the attraction aa to help with agro/pulling"

	/invoke ${set_data_timer[${_debug}, Check_VoT, 21m]}
	/call set_data_map ${_debug} maChr swVoT				 										FALSE													"${auxsw} use voice of thule aa"
 	/call set_data_map ${_debug} maChr stTempRune 											PREGEN|1											"${auxna} fast reuse disc rune"
 	/call set_data_map ${_debug} maChr stChallenge 											PREGEN|1											"${auxna} challenge spell for agro"
	/call set_data_map ${_debug} maChr stCrimsonBlade			 							PREGEN|1											"${auxna} blade melee dps"
	/call set_data_map ${_debug} maChr stAcrimony			 									PREGEN|1											"${auxna} hate attention"
	/call set_data_map ${_debug} maChr stRancor				 									PREGEN|1											"${auxna} blade disc tap"
	/call set_data_list ${_debug} maChr lsHate 													EMPTY													"${auxli} single target hate skills to use"
	/call set_data_list ${_debug} maChr lsAEHate 												EMPTY													"${auxli} ae hate skills to use"
	/call set_data_map ${_debug} maChr stAELifeTap											PREGEN|1											"${auxna} se lifetap nuke"
	/call set_data_map ${_debug} maChr stLeechcurse											PREGEN|1											"${auxna} leechcurse disc"
	/call set_data_map ${_debug} maChr stGougingBlade										PREGEN|1											"${auxna} gouging blade family disc"

	| temp buff
  /call set_data_list ${_debug} maChr lsTempBuffToon									EMPTY													"${auxli} list of toons to use your temp buff on"
	/call set_data_map ${_debug} maChr stTempBuff		 										FALSE													"${auxna} temp buff spell"

	| AAs
	/call set_data_map ${_debug} maChr stAAUnity												PREGEN|1											"${auxna} class aa unity buff"
	/call set_data_map ${_debug} maChr stAAVoiceofThule									PREGEN|1											"${auxna} Voice of Thule AA"
	/call set_data_map ${_debug} maChr stAALeechTouch                   PREGEN|1											"${auxna} Leech Touch AA"
	/call set_data_map ${_debug} maChr stAAViciousBiteofChaos           PREGEN|1											"${auxna} Vicious Bite of Chaos AA"
	/call set_data_map ${_debug} maChr stAAChatteringBones              PREGEN|1											"${auxna} Chattering Bones AA"
	/call set_data_map ${_debug} maChr stAAProjectionofDoom             PREGEN|1											"${auxna} Projection of Doom AA"
	/call set_data_map ${_debug} maChr stAAThoughtLeech                 PREGEN|1											"${auxna} Thought Leech AA"
	/call set_data_map ${_debug} maChr stAAAgelessEnmity                PREGEN|1											"${auxna} Ageless Enmity AA"
	/call set_data_map ${_debug} maChr stAADeflectionDiscipline         PREGEN|1											"${auxna} Deflection Discipline AA"
	/call set_data_map ${_debug} maChr stAATVylsResolve                 PREGEN|1											"${auxna} T`Vyl's Resolve AA"
	/call set_data_map ${_debug} maChr stAAVeilofDarkness               PREGEN|1											"${auxna} Veil of Darkness AA"
	/call set_data_map ${_debug} maChr stAAHarmTouch                    PREGEN|1											"${auxna} Harm Touch AA"
	/call set_data_map ${_debug} maChr stAAScourgeSkin                  PREGEN|1											"${auxna} Scourge Skin AA"
	/call set_data_map ${_debug} maChr stAAVisageofDeath                PREGEN|1											"${auxna} Visage of Death AA"
	/call set_data_map ${_debug} maChr stAAVisageofDecay                PREGEN|1											"${auxna} Visage of Decay AA"
	/call set_data_map ${_debug} maChr stAAGiftoftheQuickSpear          PREGEN|1											"${auxna} Gift of the Quick Spear AA"

	/if (${maDebug.Find[init].Value}) {
		OUT \agset_${Me.Class.ShortName}\ax${sep}\a-w${Math.Calc[((${MacroQuest.Running}-${_boottimestart}) / 10) / 60]}s\ax
		/mqp
	}

/return TRUE



|***
 * note: Main Shadowknight Routine
 * use: self absorbed asses.. think they can tank but in reality wet TP is tougher.
 ***|
sub main_SHD(bool _debug)
	DEBUG \atmain_SHD\ax()

	/if (AUTO) {
		/if (!${check_engaged[FALSE]} && PET && !${timer_Check_Minion_Cycle}) /call check_minion_cycle FALSE
	}

	/if (${maSash.Find[stPctShieldFlash].Value} || ${maSash.Find[stCountShieldFlash].Value}) {
		/call cast_shield_flash ${_debug} aa
	}

	/while (${check_combat_status[${maDebug.Find[status].Value}]}) {
		/if (!${timer_Check_Cure} && ${maChr.Find[swHealCure].Value}) /call check_cure ${maDebug.Find[cure].Value}
		/if (!${timer_Check_Buff_Temp} && ${maBuff.Find[swBuffTemp].Value}) {
			/call check_buff_cycle_temp_list ${maDebug.Find[temp].Value} stTempBuff lsTempBuffToon
		}
		/if (${check_class_loop[${maDebug.Find[status].Value}]}) /return

		/if (VALID) {
			/call set_combat_control ${maDebug.Find[decision].Value} TARGET
			/if (MELEE) /call do_melee ${maDebug.Find[melee].Value} TARGET
			/if (${maSash.Find[stPctShieldFlash].Value} || ${maSash.Find[stCountShieldFlash].Value}) {
				/call cast_shield_flash ${_debug} aa
			}
			/if (AGRO) /call get_agro_${Me.Class.ShortName} ${maDebug.Find[agro].Value} TARGET
			/call check_item_cycle ${maDebug.Find[item].Value} TARGET epic
			/if (BURN) /call burn_${Me.Class.ShortName} ${maDebug.Find[burn].Value} TARGET
			/call misc_combat_${Me.Class.ShortName} ${maDebug.Find[misc].Value} TARGET
			/if (DOT) /call cast_cycle_dot ${maDebug.Find[dot].Value} TARGET
			/if (NUKE) /call cast_cycle_nuke ${maDebug.Find[nuke].Value} TARGET
			/if (!${timer_Check_deBuffs}) {
				/call check_deBuffs ${maDebug.Find[debuff].Value} TARGET
			}
			/call check_item_cycle ${maDebug.Find[item].Value} TARGET
			| /if (${swAlliance}) /call cast_alliance_${Me.Class.ShortName} TARGET
			/if (${maChr.Find[swHarmonious].Value} && !${timer_Check_Harmonious}) {
				/call check_harmonious ${maDebug.Find[harmonious].Value}
			}

			/if (!${timer_Check_Buff_Temp} && ${maBuff.Find[swBuffTemp].Value}) {
				/call check_buff_cycle_temp_self FALSE stSkin
				/call check_buff_cycle_temp_self FALSE stStance
			}

		}

		/if (AUTO) {
			/if (${maChr.Find[stPctManaRecovery].Value}) /call do_mana_recovery ${maDebug.Find[manarecover].Value}
			CHECKFADE
			/if (${lsClassMelee.Contains[${Me.Class.ShortName}]}) /call cast_stamina_recovery ${maDebug.Find[staminarecover].Value}
		}
	}

/return TRUE



|***
 * note: healing
 * use: /call check_heal_self_${Me.Class.ShortName}
 ***|
sub check_heal_SHD(bool _debug, string _type)
	DEBUG \atcheck_heal_${Me.Class.ShortName}\ax(\a-w${_type}\ax)
	FLOW check_heal_SHD ${_type}

	CHECKTIE
	ISMEDEAD
	CHECKEXIT
	CHECKREZ
	CHECKFADE
	GETINPUT check_heal_SHD NA|NA

	SETHUD "heal ${_type}"

	| tap 'dat ass..
	/if (${maHeal.Find[swHealSelf].Value}) {

		/if (${is_ready[${_debug}, "${maChr.Find[stRancor].Value}"]} && ${Me.PctHPs} < ${maHeal.Find[stHealPointSHD].Value}*.PCTHO) {
			/call voodoo ${_debug} "${maChr.Find[stRancor].Value}" TARGET FALSE
		}

		/if (${is_ready[${_debug}, "${maChr.Find[stAALeechTouch].Value}"]} && ${Me.PctHPs} < ${maHeal.Find[stHealPointSHD].Value}*.PCTHO) {
			/call voodoo ${_debug} "${maChr.Find[stAALeechTouch].Value}" TARGET FALSE
		}

		/if (${SpawnCount[npc OVERLOS radius SAFEENV*2 targetable]} >= ${maChr.Find[stCountDefensive].Value}) {
			/if (${is_ready[${_debug}, "${maChr.Find[stLeechcurs].Value}"]} && ${Me.PctHPs} < ${maHeal.Find[stHealPointSHD].Value}*.PCTHO) {
				/call voodoo ${_debug} "${maChr.Find[stLeechcurse].Value}" 0 FALSE
			}
		}
		/if (${is_ready[${_debug}, "${maChr.Find[stAAViciousBiteofChaos].Value}"]}) {
			/call voodoo ${_debug} "${maChr.Find[stAAViciousBiteofChaos].Value}" TARGET FALSE
		}
	}

	/call get_hurt FALSE ${_type}

	| if no one is hurt. back to the start
	/if (!${hurtCount}) {
		SETHUD EMPTY
		/return	FALSE
	}

	/if (!${get_hurt[${_debug}, ${_type}]}) /return FALSE

	/call cast_cycle_heal ${_debug} ${hurtID} healsingle

	SETHUD EMPTY

/return TRUE



|***
 * note: kill all the things!
 * use: this shits auto, learn to love it.
 ***|
sub misc_combat_SHD(bool _debug, int _tmpID)
	DEBUG \atmisc_combat_SHD\ax(\a-w${_tmpID}\ax)
	FLOW misc_combat_SHD

	ISMEDEAD
	/if (${target[isdead, ${_tmpID}]}) {
		/return FALSE
	}

	CHECKEXIT
	GETINPUT misc_combat_SHD _tmpID|${_tmpID}
	CHECKTIE

	CHECKDEFENSE
	/if (SWARM || PET) {
		/call send_pet ${maDebug.Find[sic].Value} ${_tmpID}
	}
	/if (AUTO && MELEE) {
		/call check_facing ${maDebug.Find[face].Value} ${_tmpID} FORCE
	}

	/if (ENGINE2) {
		/call cast_cycle_miscdps ${maDebug.Find[agro].Value} ${_tmpID}
		/return TRUE
	}

	/if (${maSash.Find[stPctShieldFlash].Value} || ${maSash.Find[stCountShieldFlash].Value}) {
		/call cast_shield_flash ${_debug} aa
	}

	| AA Voice of Thule AA
	/if (AGRO && !${timer_Check_VoT} && ${maChr.Find[swVoT].Value}) {
		/if (!${Me.Buff[${maChr.Find[stAAVoiceofThule].Value}].ID}) {
			/if (${is_ready[${_debug}, "${maChr.Find[stAAVoiceofThule].Value}"]}) {
				/call voodoo ${_debug} "${maChr.Find[stAAVoiceofThule].Value}" ${Me.ID} FALSE
			}
			/invoke ${set_data_timer[FALSE, Check_VoT, RESTART]}
		}
	} else /if ((!AGRO && ${Me.Buff[${maChr.Find[stAAVoiceofThule].Value}].ID}) || !${maChr.Find[swVoT].Value}) {
			/invoke ${Me.Buff[${maChr.Find[stAAVoiceofThule].Value}].Remove}
	}

	| get me sum...
	/if (AGRO) {
		/call get_agro_${Me.Class.ShortName} ${_debug} ${_tmpID}
	}

	| quickspear proc
	/if (${is_ready[${_debug}, "${maChr.Find[stAAGiftoftheQuickSpear].Value}"]}) {
		/call voodoo ${_debug} "${maChr.Find[stAAGiftoftheQuickSpear].Value}" 0 FALSE
	}

	| swarm pets
	/if (SWARM) {
		/if (${is_ready[${_debug}, "${maChr.Find[stAAChatteringBones].Value}"]}) {
			/call voodoo ${_debug} "${maChr.Find[stAAChatteringBones].Value}" ${_tmpID} FALSE
		}
	}

	/if (${target[isdead, ${_tmpID}]}) {
		/return FALSE
	}

	/if (${is_ready[${_debug}, "${maChr.Find[stTempRune].Value}"]}) {
		/call voodoo ${_debug} "${maChr.Find[stTempRune].Value}" 0 FALSE
	}
	/if (${is_ready[${_debug}, "${maChr.Find[stAAThoughtLeech].Value}"]} &&  ${Me.PctMana} <= 60*.PCTHO) {
		/call voodoo ${_debug} "${maChr.Find[stAAThoughtLeech].Value}" ${_tmpID} FALSE
	}
	/if (${is_ready[${_debug}, "${maChr.Find[stGougingBlade].Value}"]}) {
		/call voodoo ${_debug} "${maChr.Find[stGougingBlade].Value}" ${_tmpID} FALSE
	}

/return TRUE



|***
 * note: we are agro gods.
 * use: /call get_agro_${Me.Class.ShortName}
 ***|
sub get_agro_SHD(bool _debug, int _tmpID)
	DEBUG \atget_agro_SHD\ax()
	FLOW get_agro_SHD

	ISMEDEAD
	CHECKEXIT
	GETINPUT get_agro_SHD _tmpID|${_tmpID}

	/if (${target[isdead, ${_tmpID}]}) {
		/return FALSE
	}

	/if (PET) {
		/call send_pet ${maDebug.Find[sic].Value} ${_tmpID}
	}

	/if (ENGINE2) {
		/call cast_cycle_agro ${maDebug.Find[agro].Value} ${_tmpID}
		/return TRUE
	}

	/call cast_challenge ${_debug} ${_tmpID}

	/if (${maSash.Find[stPctShieldFlash].Value} || ${maSash.Find[stCountShieldFlash].Value}) {
		/call cast_shield_flash ${_debug} aa
	}

	| AA pet hate
	/if (${Me.TargetOfTarget.ID} != ${Me.ID}) {
		/if (${is_ready[${_debug}, "${maChr.Find[stAAProjectionofDoom].Value}"]}) {
			/call voodoo ${_debug} "${maChr.Find[stAAProjectionofDoom].Value}" ${_tmpID} FALSE
		}
	}

	| single hate
	/declare _liha listiterator local
	/vardata _liha lsHate.First.Clone
	/while (!${_liha.IsEnd}) {
		/if (${is_ready[${_debug}, "${_liha.Value}"]}) {
			/call voodoo ${_debug} "${_liha.Value}" ${_tmpID} FALSE
			/break
		}
		/invoke ${_liha.Advance}
	}

	| AE Hate
	/if (AE# && ${SpawnCount[npc radius 50 PS4]} >= AE#) {
			/declare _liaeha listiterator local
			/vardata _liaeha lsAEHate.First.Clone
			/while (!${_liaeha.IsEnd}) {
				/if (${is_ready[${_debug}, "${_liaeha.Value}"]}) {
					/call voodoo ${_debug} "${_liaeha.Value}" 0 FALSE
					/break
				}
				/invoke ${_liaeha.Advance}
			}

		| AE lifetap
		/if (!${SpawnCount[pc enchanter radius SAFEENV*2]}) {
			/if (${is_ready[${_debug}, "${maChr.Find[stAELifeTap].Value}"]}) {
				/call voodoo ${_debug} "${maChr.Find[stAELifeTap].Value}" 0 FALSE
			}
		}
	}

	/if (${target[isdead, ${_tmpID}]}) {
		/return FALSE
	}

	| fake hate
	/if (${Me.PctAggro} < 90 || ${Me.TargetOfTarget.ID} != ${Me.ID}) {
		/if (${is_ready[${_debug}, "${maChr.Find[stAAAgelessEnmity].Value}"]}) {
			/call voodoo ${_debug} "${maChr.Find[stAAAgelessEnmity].Value}" ${_tmpID} FALSE
		}
	}
	| named fake hate
	/if (${lsZoneNamed.Contains[${Target.DisplayName}]}) {
		| i have no idea WTF happened to mindless hatred
		| /if (${Me.PctAggro} < 90 || ${Me.TargetOfTarget.Name.NotEqual[${Me.DisplayName}]}) {
		| 	/if (${is_ready[${_debug}, "Mindless Hatred"]} && ${Target.Level} <= ${Me.Level}) {
		| 		/call voodoo ${_debug} "Mindless Hatred" ${_tmpID} FALSE
		| 	}
		| }
		/if (${Me.PctAggro} < 90 || ${Me.TargetOfTarget.ID} != ${Me.ID}) {
			/if (${is_ready[${_debug}, "${maChr.Find[stAcrimony].Value}"]}) {
				/call voodoo ${_debug} "${maChr.Find[stAcrimony].Value}" ${_tmpID} FALSE
			}
		}
	}

/return TRUE



|***
 * note: don't touch me there...
 * use:
 ***|
sub burn_SHD(bool _debug, int _tmpID)
	DEBUG \atburn_SHD\ax()
	FLOW burn_SHD

	ISMEDEAD
	/if (${target[isdead, ${_tmpID}]}) {
		/return FALSE
	}

	CHECKEXIT
	GETINPUT burn_SHD _tmpID|${_tmpID}
	CHECKTIE

	CHECKDEFENSE
	/if (SWARM || PET) {
		/call send_pet ${maDebug.Find[sic].Value} ${_tmpID}
	}
	/if (AUTO && MELEE) {
		/call check_facing ${maDebug.Find[face].Value} ${_tmpID} FORCE
	}

	| deflection
	/if (!${Me.ActiveDisc.ID} && ${Me.PctHPs} < 50*.PCTHO) {
		/if (${is_ready[${_debug}, "${maChr.Find[stAADeflectionDiscipline].Value}"]}) {
			/call voodoo ${_debug} "${maChr.Find[stAADeflectionDiscipline].Value}" 0 FALSE
			/return TRUE
		}
	}

	/if (ENGINE2) {
		/call cast_cycle_burn ${maDebug.Find[burn].Value} ${_tmpID}
		/return TRUE
	}

	/if (${maSash.Find[stPctShieldFlash].Value} || ${maSash.Find[stCountShieldFlash].Value}) {
		/call cast_shield_flash ${_debug} aa
	}

	/if (${is_ready[${_debug}, "${maChr.Find[stAATVylsResolve].Value}"]}) {
		/call voodoo ${_debug} "${maChr.Find[stAATVylsResolve].Value}" ${_tmpID} FALSE
	}
	/if (${is_ready[${_debug}, "${maChr.Find[stCrimsonBlade].Value}"]}) {
		/call voodoo ${_debug} "${maChr.Find[stCrimsonBlade].Value}" 0 FALSE
	}

	/call cast_AA_spire ${maDebug.Find[spire].Value}

	/if (${is_ready[${_debug}, "${maChr.Find[stAAVeilofDarkness].Value}"]}) {
		/call voodoo ${_debug} "${maChr.Find[stAAVeilofDarkness].Value}" ${_tmpID} FALSE
	}
	/if (${is_ready[${_debug}, "${maChr.Find[stAAHarmTouch].Value}"]}) {
		/call voodoo ${_debug} "${maChr.Find[stAAHarmTouch].Value}" ${_tmpID} FALSE
	}

	/if (${target[isdead, ${_tmpID}]}) {
		/return FALSE
	}

	/if (AGRO) {
		/if (${is_ready[${_debug}, "${maChr.Find[stAAScourgeSkin].Value}"]}) {
			/call voodoo ${_debug} "${maChr.Find[stAAScourgeSkin].Value}" 0 FALSE
		}
	}

	| AA Visage of Decay/Death stacking issues
	/if (!${Me.Buff[${maChr.Find[stAAVisageofDecay].Value}].ID}) {
		/if (${is_ready[${_debug}, "${maChr.Find[stAAVisageofDeath].Value}"]}) {
			/call voodoo ${_debug} "${maChr.Find[stAAVisageofDeath].Value}" 0 FALSE
		}
	} else /if (!${Me.AltAbilityReady[${maChr.Find[stAAVisageofDeath].Value}]} && !${Me.Buff[${maChr.Find[stAAVisageofDeath].Value}].ID}) {
		/if (${is_ready[${_debug}, "${maChr.Find[stAAVisageofDecay].Value}"]})	{
			/call voodoo ${_debug} "${maChr.Find[stAAVisageofDecay].Value}" 0 FALSE
		}
	}

	/call cast_glyph ${maDebug.Find[glyph].Value}

/return TRUE



|***
 * note: class controls
 * use:
 ***|
sub set_control_SHD(string _type, string _verbage, string _silent, bool _debug)
	DEBUG \atset_control_SHD\ax(\a-w${_type}, "${_verbage}", ${_silent}\ax)

  /declare _out string local

	| output control map
	/if (${_type.Equal[see]}) {
		/call Bind_command_see maChr
		/return TRUE

	| use mana click items
	} else /if (${_type.Equal[manarecover]}) {
		/if (!${set_control_num_range[${_debug}, stPctManaRecovery, "${_verbage}", 0, 99]}) /return

	| set spire to be used
	} else /if (${_type.Equal[spire]}) {
		/call set_control_chr_spire ${_debug} ${_verbage} ${_silent}
		/return TRUE

	| set AA fade use %
	} else /if (${_type.Equal[fade]}) {
		/if (!${set_control_num_range[${_debug}, stPctFade, "${_verbage}", 0, 99]}) /return

	| voice of Thule
	} else /if (${_type.Equal[vot]}) {
		/invoke ${set_switch_env[${_debug}, swVoT, TRUE, ${_verbage}]}

	| defensive mob agro count
	} else /if (${_type.Equal[def]}) {
		/if (!${set_control_num_range[${_debug}, stCountDefensive, "${_verbage}", 0, 99]}) /return

	| set ae on/off / count
	} else /if (${_type.Equal[ae]}) {
		/if (!${set_control_num_range[${_debug}, stCountAE, "${_verbage}", 0, 99]}) /return

	| harmonious
	} else /if (${_type.Equal[harmonious]}) {
		/invoke ${set_switch_env[${_debug}, swHarmonious, TRUE, ${_verbage}]}

	| attraction
	} else /if (${_type.Equal[attraction]}) {
		/invoke ${set_switch_env[${_debug}, swAttraction, TRUE, ${_verbage}]}

	| use hiatus disc
	} else /if (${_type.Equal[hiatus]}) {
		/invoke ${set_switch_env[${_debug}, swHiatus, TRUE, ${_verbage}]}

	}

	/if (${_silent.Equal[SILENT]}) /return
	/invoke ${set_control_output[${_debug}, maChr, chr, maChrControl]}

/return TRUE

