|***
 * entropy
 * class.rng.inc
 ***|




|***
 * note: primary initilization of all ranger non shared data
 * use: not for you
 ***|
sub class_init(bool _debug)
	DEBUG \atclass_init\ax()

	/if (${maDebug.Find[init].Value}) {
		/declare _boottimestart int local ${MacroQuest.Running}
	}

	/call set_data FALSE maData stSpire																	map outer "Spire of the Pathfinders"
	/call set_data FALSE maData stSynergy																map outer "Outrider's Synergy"
	/call set_data FALSE maData stFade																	map outer "Cover Tracks"
	/call set_data_map FALSE maChr stAAUnity														"Wildstalker's Unity (Azia)"	"${auxna} class aa unity buff"

	/call set_data_map FALSE maChr stAlliance														PREGEN|1											"${auxna} alliance spell"

	| spam summer nuke
	/invoke ${set_data_timer[FALSE, Summer_Nuke, 1s]}
	/call set_data_map FALSE maChr stSummer		 													PREGEN|1											"${auxna} summer line nuke spell"
	/call set_data_map FALSE maChr stCountSummer												0															"${auxnum} number of mobs in environment to start using summer nuke line"

	| rest disc
 	/call set_data_map FALSE maChr stRest 															PREGEN|1											"${auxna} name of stamina rest disc"
 	/call set_data_map FALSE maChr stHiatus															PREGEN|1											"${auxna} name of hiatus stamina recovery disc"
 	/call set_data_map FALSE maChr swHiatus															FALSE													"${auxsw} use hiatus rest disc"

	| heals
	/call set_data_map FALSE maChr stDiscHeal		 												PREGEN|1											"${auxna} group heal disc"

	| bow disc
	/call set_data_map FALSE maChr stDiscRange			 										PREGEN|1											"${auxna} ranged DPS disc"

	| melee disc
	/call set_data_map FALSE maChr stDiscMelee			 										PREGEN|1											"${auxna} melee DPS disc"

	| agro/anti agro kicks
	/call set_data_map FALSE maChr stDiscJolt			 											PREGEN|1											"${auxna} hate reduction disc"
	/call set_data_map FALSE maChr stDiscEnrage		 											PREGEN|1											"${auxna} hate increase disc"

	| ae melee disc
	/call set_data_map FALSE maChr stDiscMeleeAE		 										PREGEN|1											"${auxna} ae melee disc"

	| focused melee disc
	/call set_data_map FALSE maChr stDiscMeleeSingle			 							PREGEN|1											"${auxna} single melee disc"

	| temp buff
  /call set_data_list ${_debug} maChr lsTempBuffToon									EMPTY													"${auxli} list of toons to use your temp buff on"
	/call set_data_map ${_debug} maChr stTempBuff		 										FALSE													"${auxna} temp buff spell"

	| AAs
	/call set_data_map ${_debug} maChr stAAImbuedFerocity								PREGEN|1											"${auxna} Imbued Ferocity aa"
	/call set_data_map ${_debug} maChr stAAEmpoweredBlades							PREGEN|1											"${auxna} Empowered Blades aa"
	/call set_data_map ${_debug} maChr stAAOutridersAccuracy						PREGEN|1											"${auxna} Outrider's Accuracy aa"
	/call set_data_map ${_debug} maChr stAAOutridersAttack							PREGEN|1											"${auxna} Outrider's Attack aa"
	/call set_data_map ${_debug} maChr stAAGroupGuardianoftheForest			PREGEN|1											"${auxna} Group Guardian of the Forest aa"
	/call set_data_map ${_debug} maChr stAAGuardianoftheForest					PREGEN|1											"${auxna} Guardian of the Forest aa"
	/call set_data_map ${_debug} maChr stAAPackHunt											PREGEN|1											"${auxna} Pack Hunt aa"
	/call set_data_map ${_debug} maChr stAAElementalArrow								PREGEN|1											"${auxna} Elemental Arrow aa"
	/call set_data_map ${_debug} maChr stAAAuspiceoftheHunter						PREGEN|1											"${auxna} Auspice of the Hunter aa"
	/call set_data_map ${_debug} maChr stAAScarletCheetahsFang					PREGEN|1											"${auxna} Scarlet Cheetah's Fang aa"

	/if (${maDebug.Find[init].Value}) {
		OUT \agset_${Me.Class.ShortName}\ax${sep}\a-w${Math.Calc[((${MacroQuest.Running}-${_boottimestart}) / 10) / 60]}s\ax
		/mqp
	}

/return TRUE



|***
 * note: Main Ranger Routine
 * use:
 ***|
sub class_main(bool _debug)
	DEBUG \atclass_main\ax()

	GETINPUT
	ISMEDEAD
	CHECKTIE
	CHECKREZ

	/if (AUTO && !ENGINE3) {
	} else /if (AUTO && ENGINE3) {
		/if (${SubDefined[e3_pre_${Me.Class.ShortName}]}) {
			/call e3_pre_${Me.Class.ShortName} ${maDebug.Find[e3].Value}
		}
	}

	/while (${check_combat_status[${maDebug.Find[status].Value}]}) {
		/call check_heal_routines
		/if (!${timer_Check_Buff_Temp} && ${maBuff.Find[swBuffTemp].Value}) {
			/call check_buff_cycle_temp_list ${maDebug.Find[temp].Value} stTempBuff lsTempBuffToon
		}
		/if (!${check_class_loop[${maDebug.Find[status].Value}]}) /return FALSE FALSE

		/if (VALID && !ENGINE3) {
			/call set_combat_control ${maDebug.Find[decision].Value} TARGET
			/if (MELEE) /call do_melee ${maDebug.Find[melee].Value} TARGET
			/call check_heal_routines
			/call check_item_cycle ${maDebug.Find[item].Value} TARGET epic
			/if (BURN) /call burn ${maDebug.Find[burn].Value} TARGET
			/call misc_combat ${maDebug.Find[misc].Value} TARGET
			/if (DOT) /call cast_cycle_dot ${maDebug.Find[dot].Value} TARGET
			/if (NUKE) /call cast_cycle_nuke ${maDebug.Find[nuke].Value} TARGET
			/if (${maChr.Find[stCountSummer].Value} && !${timer_Summer_Nuke}) /call cast_nuke_summer ${maDebug.Find[summer].Value} TARGET
			/if (!${timer_Check_deBuffs}) {
				/call check_deBuffs ${maDebug.Find[debuff].Value} TARGET
			}
			/call check_item_cycle ${maDebug.Find[item].Value} TARGET
			| /if (${swAlliance}) /call trigger_alliance TARGET
		} else /if (VALID && ENGINE3) {
			/if (${SubDefined[e3_combat_${Me.Class.ShortName}]}) {
				/call e3_combat_${Me.Class.ShortName} ${maDebug.Find[e3].Value}
			}

		}

		/if (AUTO && !ENGINE3) {
			CHECKFADE
			/if (${maChr.Find[stPctManaRecovery].Value}) /call do_mana_recovery ${maDebug.Find[manarecover].Value}
			/if (${lsClassMelee.Contains[${Me.Class.ShortName}]}) /call cast_stamina_recovery ${maDebug.Find[staminarecover].Value}
		} else /if (AUTO && !ENGINE3) {
			/if (${SubDefined[e3_post_${Me.Class.ShortName}]}) {
				/call e3_post_${Me.Class.ShortName} ${maDebug.Find[e3].Value}
			}
		}

	}

/return TRUE



|***
 * note: Combat skills, Disciplines and Alternate abilities.
 * use: /call misc_combat DEBUG TARGETID
 ***|
sub misc_combat(bool _debug, int _tmpID)
	DEBUG \atmisc_combat\ax()
	| FLOW misc_combat

	ISMEDEAD
	CHECKEXIT
	GETINPUT
	CHECKTIE
	CHECKDEFENSE
	/if (!PULL) {
		/call get_assist_target ${maDebug.Find[target].Value}
		/if (TARGET != ${_tmpID}) /varset _tmpID TARGET
	}

	/if (${target[isdead, ${_tmpID}]}) {
		/return FALSE
	}

	/call check_heal_routines

	/if (!PULL) {
		/call get_assist_target ${maDebug.Find[target].Value}
	}

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

	| agro control
	/if (AGRO) {

		/invoke ${do_taunt[${_debug}, ${_tmpID}]}

		/if (${Me.PctAggro} < ${maAgro.Find[stPctAgroHold].Value}) {
			/if (${is_ready[${_debug}, "${maChr.Find[stDiscEnrage].Value}"]}) {
				/call voodoo ${_debug} "${maChr.Find[stDiscEnrage].Value}" ${_tmpID} FALSE
			}
		}
	}

	/if (!AGRO) {
		/if (${is_ready[${_debug}, "${maChr.Find[stDiscJolt].Value}"]}) {
			/call voodoo ${_debug} "${maChr.Find[stDiscJolt].Value}" ${_tmpID} FALSE
		}
	}

	| elemental arrows rotation
	/if (!${Target.Buff[Elemental Arrow].ID}) {
		/if (${is_ready[${_debug}, "Elemental Arrow"]}) {
			/call voodoo ${_debug} "Elemental Arrow" ${_tmpID} FALSE
		}
	}

	/if (!AE#) {
		/if (${is_ready[${_debug}, "${maChr.Find[stDiscMeleeSingle].Value}"]}) {
			/call voodoo ${_debug} "${maChr.Find[stDiscMeleeSingle].Value}" ${_tmpID} FALSE
		}
	} else /if (${SpawnCount[npc OVERLOS radius 60 PS4 targetable]} >= ${maChr.Find[stCountAE].Value} && ${maChr.Find[stCountAE].Value}) {
		/if (${is_ready[${_debug}, "${maChr.Find[stDiscMeleeAE].Value}"]}) {
			/call voodoo ${_debug} "${maChr.Find[stDiscMeleeAE].Value}" ${_tmpID} FALSE
		}
	}

/return TRUE



|***
 * note: MGB ranger Auspice if both AAs are ready
 * use:
 ***|
sub cast_AA_auspice(bool _debug)
	DEBUG \atcast_AA_auspice\ax()
	| FLOW cast_AA_auspice

	ISMEDEAD
	CHECKEXIT
	GETINPUT
	/if (${Me.AltAbilityReady[Mass Group Buff]} && ${Me.AltAbilityReady[${maChr.Find[stAAAuspiceoftheHunter].Value}]} && !${Bool[${Me.Song[${maChr.Find[stAAAuspiceoftheHunter].Value}]}]}) {
		/if (${is_ready[${_debug}, "Mass Group Buff"]}) {
			/call voodoo ${_debug} "Mass Group Buff" 0 FALSE
		}
		/if (${is_ready[${_debug}, "${maChr.Find[stAAAuspiceoftheHunter].Value}"]}) {
			/call voodoo ${_debug} "${maChr.Find[stAAAuspiceoftheHunter].Value}" 0 FALSE
		}
	} else /if (${Me.Song[${maChr.Find[stAAAuspiceoftheHunter].Value}].ID}) {
		OUT ${bad}QUIT\ax trying to cast ${sp}${maChr.Find[stAAAuspiceoftheHunter].Value}\ax while its running.
	}
/return TRUE



|***
 * note: healing
 * use: /call check_heal
 ***|
sub check_heal(bool _debug, string _type)
	DEBUG \atcheck_heal\ax(\a-w${_type}\ax)
	| FLOW check_heal ${_type}

	CHECKTIE
	ISMEDEAD
	CHECKEXIT
	CHECKREZ
	CHECKFADE

	/call get_hurt ${maDebug.Find[hurt].Value} ${_type}

	| if no one is hurt. back to the start
	/if (!${hurtCount}) {
		SETHUD EMPTY
		/return TRUE
	}

	/if (!SAFEZONE) {

		| group healing
		/if (${Select[${_type},group]} && ${hurtCount}) {

			| group heal
			/if (${maHeal.Find[stCountHealGroup].Value} && ${hurtCount} >= ${maHeal.Find[stCountHealGroup].Value}) {
				/call cast_cycle_heal ${_debug} 0 group
 			}
		}

		| ToT heals
		/if (${Select[${_type},tot]} && ${hurtCount}) {
			/if (${hurtCount})  {
				/call cast_cycle_heal ${_debug} ${hurtID} tot
				/if (!${get_hurt[${maDebug.Find[hurt].Value}, ${_type}]}) /return FALSE
			}
		}


		/if (!${get_hurt[${_debug}, ${_type}]}) /return FALSE

		| Reflexive Healing Disc
		/if (${hurtCount} && ${Select[${_type},self,group,tot]}) {
			/if (${is_ready[${_debug}, "${maChr.Find[stDiscHeal].Value}"]} && !${Me.Song[Bladespur Reflection].ID}) {
				/call voodoo ${_debug} "${maChr.Find[stDiscHeal].Value}" TARGET FALSE
			}
		}
	}


	/if (${Select[${_type},tot]}) {
 	 /if (!${get_hurt[${maDebug.Find[hurt].Value}, ${_type}]}) {
 			/return FALSE
 		}

		/call cast_cycle_heal ${_debug} ${Target.ID} tot
	}

  /if (!${get_hurt[${maDebug.Find[hurt].Value}, ${_type}]}) {
 		/return FALSE
 	}

	GETINPUT ${Me.Class.ShortName}_heal_end type|${_type}

	/call cast_cycle_heal ${_debug} ${hurtID} single

	SETHUD EMPTY

/return TRUE



|***
 * note: Control for Spam Agro Nuke
 * use: /call cast_nuke_summer DEBUG [TARGETID]
 ***|
sub cast_nuke_summer(bool _debug, int _tmpID)
	DEBUG \atcast_nuke_summer\ax(\a-w${_tmpID}\ax)
	| FLOW cast_nuke_summer

	/if (${target[isdead, ${_tmpID}]}) {
		/return FALSE
	}

	/if (!${maChr.Find[stCountSummer].Value}) {
		/return FALSE
	}

	/if (${SpawnCount[npc radius ENV PS4]} < ${maChr.Find[stCountSummer].Value}) {
		/return FALSE
	}

	ISMEDEAD
	CHECKEXIT
	/invoke ${set_data_timer[${_debug}, Summer_Nuke, RESTART]}

	/if (${is_ready[${_debug}, "${maChr.Find[stSummer].Value}"]}) {
		/call voodoo ${_debug} "${maChr.Find[stSummer].Value}" ${_tmpID} FALSE
	}

/return TRUE



|***
 * note: really should be starting fires
 * use:
 ***|
sub burn(bool _debug, int _tmpID)
	DEBUG \atburn\ax(\a-w${_tmpID}\ax)
	| FLOW burn

	ISMEDEAD
	/if (${target[isdead, ${_tmpID}]}) {
		/return FALSE
	}

	CHECKEXIT
	GETINPUT
	CHECKTIE
	CHECKDEFENSE
	/if (!PULL) {
		/call get_assist_target ${maDebug.Find[target].Value}
		/if (TARGET != ${_tmpID}) /varset _tmpID TARGET
	}

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


	/call cast_AA_spire ${maDebug.Find[spire].Value}

	| cheeta
	/if (${is_ready[${_debug}, "${maChr.Find[stAAScarletCheetahsFang].Value}"]}) {
		/call voodoo ${_debug} "${maChr.Find[stAAScarletCheetahsFang].Value}" 0 FALSE
	}

	| auspice
	/if (${is_ready[${_debug}, "${maChr.Find[stAAAuspiceoftheHunter].Value}"]}) {
		/call voodoo ${_debug} "${maChr.Find[stAAAuspiceoftheHunter].Value}" 0 FALSE
	}

	| MELEE
	/if (MELEE && !RANGE || MELEE && ${Spawn[${_tmpID}].Distance} <= ${Spawn[${_tmpID}].MaxRangeTo}*.${maHard.Find[stMaxMeleeAdj].Value}) {

		| weapon proc
		/if (${is_ready[${_debug}, "${maChr.Find[stAAImbuedFerocity].Value}"]}) {
			/call voodoo ${_debug} "${maChr.Find[stAAImbuedFerocity].Value}" 0 FALSE
		}

		/if (!${Me.ActiveDisc.ID} && !${Me.Buff[${maCC.Find[stGlyph].Value}].ID}) {
			/if (${is_ready[${_debug}, "${maChr.Find[stAAEmpoweredBlades].Value}"]}) {
				/call voodoo ${_debug} "${maChr.Find[stAAEmpoweredBlades].Value}" 0 FALSE
			}

		} else /if (!${Me.ActiveDisc.ID} && !${Me.Song[${maChr.Find[stAAEmpoweredBlades].Value}].ID} && !${Me.Buff[${maCC.Find[stGlyph].Value}].ID}) {
			/if (${is_ready[${_debug}, "${maChr.Find[stDiscMele].Value}"]}) {
				/call voodoo ${_debug} "${maChr.Find[stDiscMele].Value}" 0 FALSE
			}
		}

		/if (!${Me.ActiveDisc.ID} && !${is_ready[${_debug}, "${maChr.Find[stDiscMele].Value}"]} && !${Me.AltAbilityReady[${maChr.Find[stAAEmpoweredBlades].Value}]} && !${Me.Song[${maChr.Find[stAAEmpoweredBlades].Value}].ID}) {
			/call cast_glyph ${maDebug.Find[glyph].Value}
		  | Intensity of the Resolute AA 4 hour reuse
		  /call cast_AA_intensity ${maDebug.Find[intensity].Value}

		}
	}


	| RANGE
	/if (RANGE && ${Target.Distance} >= ${maHard.Find[stMinBowDist].Value}) {
		/if (!${Me.ActiveDisc.ID} && !${Me.Buff[${maCC.Find[stGlyph].Value}].ID}) {
			/if (${is_ready[${_debug}, "${maChr.Find[stDiscRange].Value}"]}) {
				/call voodoo ${_debug} "${maChr.Find[stDiscRange].Value}" 0 FALSE
			}
		}

		/if (!${Me.ActiveDisc.ID} && !${is_ready[${_debug}, "${maChr.Find[stDiscRange].Value}"]}) {
			/call cast_glyph ${maDebug.Find[glyph].Value}
		  | Intensity of the Resolute AA 4 hour reuse
		  /call cast_AA_intensity ${maDebug.Find[intensity].Value}
		}
	}


	| attack / dmg AA
	/if (${is_ready[${_debug}, "${maChr.Find[stAAOutridersAccuracy].Value}"]}) {
		/call voodoo ${_debug} "${maChr.Find[stAAOutridersAccuracy].Value}" 0 FALSE
	}
	/if (${is_ready[${_debug}, "${maChr.Find[stAAOutridersAttack].Value}"]}) {
		/call voodoo ${_debug} "${maChr.Find[stAAOutridersAttack].Value}" 0 FALSE
	}

	| guardian buff (group first)
	/if (!${Me.Song[${maChr.Find[stAAGuardianoftheForest].Value}].ID} && ${Me.AltAbilityReady[${maChr.Find[stAAGroupGuardianoftheForest].Value}]}) {
		/if (${is_ready[${_debug}, "${maChr.Find[stAAGroupGuardianoftheForest].Value}"]}) {
			/call voodoo ${_debug} "${maChr.Find[stAAGroupGuardianoftheForest].Value}" 0 FALSE
		}

	} else /if (!${Me.Song[${maChr.Find[stAAGroupGuardianoftheForest].Value}].ID} && ${Me.AltAbilityReady[${maChr.Find[stAAGuardianoftheForest].Value}]}) {
		/if (${is_ready[${_debug}, "${maChr.Find[stAAGuardianoftheForest].Value}"]}) {
			/call voodoo ${_debug} "${maChr.Find[stAAGuardianoftheForest].Value}" 0 FALSE
		}
	}

	| swarm pets
	/if (SWARM) {
		/if (${is_ready[${_debug}, "${maChr.Find[stAAPackHunt].Value}"]}) {
			/call voodoo ${_debug} "${maChr.Find[stAAPackHunt].Value}" ${_tmpID} FALSE
		}
	}

	| ae regen
	/if (${is_ready[${_debug}, "${maChr.Find[stDiscHeal].Value}"]} && !${Me.Song[Bladespur Reflection].ID}) {
		/call voodoo ${_debug} "${maChr.Find[stDiscHeal].Value}" ${_tmpID} FALSE
	}

 /return TRUE



|***
 * note: class control
 * use: /chr
 ***|
sub set_control(string _type, string _verbage, string _verbage2, bool _debug)
	DEBUG \atset_control\ax(\a-w${_type}, "${_verbage}", ${_verbage2}\ax)

	| summer switch
	/if (${_type.Equal[summer]}) {
		/if (!${set_control_num_range[${_debug}, stCountSummer, "${_verbage}", 0, 100]}) /return
	}

	/call set_control_shared ${_type} "${_verbage}" "${_verbage2}" ${_debug}
	/if (${Macro.Return.Equal[SKIP]}) /return FALSE

	/if (${_verbage2.Equal[SILENT]}) /return
	/invoke ${set_control_output[${_debug}, maChr, chr, maChrControl]}

/return TRUE


