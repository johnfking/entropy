|***
 * entropy
 * class.enc.inc
 ***|



|***
 * note: primary initilization of all enchanter non shared data
 * use: not for you
 ***|
sub class_init(bool _debug)
	DEBUG \atclass_init\ax()

	/if (${maDebug.Find[init].Value}) {
		/declare _boottimestart int local ${MacroQuest.Running}
	}

	/call set_data ${_debug} maData stSpire															map outer "Spire of Enchantment"
	/call set_data ${_debug} maData stSynergy														map outer "Begulier's Synergy"
	/call set_data ${_debug} maData stFade															map outer "Self Stasis"

	| map for mob control
	/declare maMobControl map outer

	/call set_data_map ${_debug} maChr stPctGather											0															"${auxnum} % of your mana to usin gather mana AA and crystal if enabled"

	/call set_data_map ${_debug} maChr stAlliance												PREGEN|1											"${auxna} alliance spell"

	/call set_data_map ${_debug} maChr swAABoS													FALSE													"${auxsw} use aa Beam of Slumber in your mez routines"

	| single target charm
	/call set_data_map ${_debug} maChr stCharm													PREGEN|1											"${auxna} name of your charm spell"
	/call set_data_map ${_debug} maChr swCharm													FALSE													"${auxsw} enable using charm"

  | nights endless terror
  /call set_data_map ${_debug} maChr stTerror													PREGEN|1											"${auxna} name of terror spell"
  /call set_data_list ${_debug} maChr lsTerrorToon										EMPTY													"${auxli} lsit of toons you want to keep terror buff on"

	/call set_data_map ${_debug} maChr stPctGather				 							0															"${auxnum} % of manna you want to use gather at"

	| aa crystals
  /invoke ${set_data_timer[${_debug}, Check_Crystals, 10s]}
	/call set_data_map ${_debug} maChr swAACrystals											FALSE													"${auxsw} use aa crystals:\a-wAzure Mind Crystal, Sanguine Mind Crystal/ax"

	| PBAE stun
	/call set_data_map ${_debug} maChr stAEStun													PREGEN|1											"${auxna} name of your point blank ae stun"

	| temp buff
  /call set_data_list ${_debug} maChr lsTempBuffToon									EMPTY													"${auxli} list of toons to use your temp buff on"
	/call set_data_map ${_debug} maChr stTempBuff		 										FALSE													"${auxna} temp buff spell"

	| AAs
	/call set_data_map ${_debug} maChr stAABeamofSlumber								PREGEN|1											"${auxna} Beam of Slumber AA"
	/call set_data_map ${_debug} maChr stAAPhantasmalOpponent           PREGEN|1											"${auxna} Phantasmal Opponent AA"
	/call set_data_map ${_debug} maChr stAAChromaticHaze                PREGEN|1											"${auxna} Chromatic Haze AA"
	/call set_data_map ${_debug} maChr stAAAzureMindCrystal             PREGEN|1											"${auxna} Azure Mind Crystal AA"
	/call set_data_map ${_debug} maChr stAASanguineMindCrystal          PREGEN|1											"${auxna} Sanguine Mind Crystal AA"
	/call set_data_map ${_debug} maChr stAAGatherMana                   PREGEN|1											"${auxna} Gather Mana AA"
	/call set_data_map ${_debug} maChr stAAIllusionsofGrandeur          PREGEN|1											"${auxna} Illusions of Grandeur AA"
	/call set_data_map ${_debug} maChr stAAFocusofArcanum               PREGEN|1											"${auxna} Focus of Arcanum AA"
	/call set_data_map ${_debug} maChr stAADimensionalShield            PREGEN|1											"${auxna} Dimensional Shield AA"
	/call set_data_map ${_debug} maChr stAASilentCasting                PREGEN|1											"${auxna} Silent Casting AA"
	/call set_data_map ${_debug} maChr stAAImprovedTwincast             PREGEN|1											"${auxna} Improved Twincast AA"
	/call set_data_map ${_debug} maChr stAAMentalContortion             PREGEN|1											"${auxna} Mental Contortion AA"
	/call set_data_map ${_debug} maChr stAADoppelganger									PREGEN|1											"${auxna} Doppelganger AA"
	/call set_data_map ${_debug} maChr stAACalculatedInsanity						PREGEN|1											"${auxna} Calculated Insanity AA"
	/call set_data_map ${_debug} maChr stAAUnity												PREGEN|1											"${auxna} class aa unity buff"

	/if (${maDebug.Find[init].Value}) {
		OUT \agset_${Me.Class.ShortName}\ax${sep}\a-w${Math.Calc[((${MacroQuest.Running}-${_boottimestart}) / 10) / 60]}s\ax
		/mqp
	}

/return TRUE




|***
 * note: main enchanter routine
 * use:
 ***|
sub class_main(bool _debug)
	DEBUG \atclass_main\ax()


	/if (AUTO && !ENGINE3) {
		/if (${maChr.Find[swAACrystals].Value} && !${timer_Check_Crystals}) {
			/call check_crystals ${maDebug.Find[crystal].Value}
		}
		/if (${maChr.Find[stPctGather].Value}) {
			/call check_gather ${maDebug.Find[gather].Value}
		}
		/if (!${check_engaged[FALSE]} && PET && !${timer_Check_Minion_Cycle}) {
			/call check_minion_cycle ${maDebug.Find[petbuild].Value}
		}
	} else /if (AUTO && ENGINE3) {
		/if (${SubDefined[e3_pre_${Me.Class.ShortName}]}) {
			/call e3_pre_${Me.Class.ShortName} ${maDebug.Find[e3].Value}
		}
	}


	/while (${check_combat_status[${maDebug.Find[status].Value}]}) {
		GETINPUT
		CHECKREZ
		/if (!${timer_Check_Cure} && ${maChr.Find[swHealCure].Value}) /call check_cure ${maDebug.Find[cure].Value}
		/call control_routine ${maDebug.Find[mez].Value}
		/if (!${timer_Check_Buff_Temp} && ${maBuff.Find[swBuffTemp].Value}) {
			/call check_buff_cycle_temp_list ${maDebug.Find[temp].Value} stTempBuff lsTempBuffToon
		}
		/if (!${check_class_loop[${maDebug.Find[status].Value}]}) /return FALSE

		/if (VALID && !ENGINE3) {
			/if (PET || SWARM || ${maMinion.Find[swPetCharm].Value}) /call send_pet ${maDebug.Find[sic].Value} TARGET
			/call set_combat_control ${maDebug.Find[decision].Value} TARGET
			/if (MELEE) /call do_melee ${maDebug.Find[melee].Value} TARGET
			CHECKDEFENSE
			/if (!${timer_Check_deBuffs}) {
				/call check_deBuffs ${maDebug.Find[debuff].Value} TARGET
			}
			/call control_routine ${maDebug.Find[mez].Value}
			| /if (${maMinion.Find[swPetCharm].Value}) /call find_mob_to_charm ${maDebug.Find[charm].Value}
			/call check_heal_routines
			/if (BURN) /call burn ${maDebug.Find[burn].Value} TARGET
			/call misc_combat ${maDebug.Find[misc].Value} TARGET
			/call control_routine ${maDebug.Find[mez].Value}
			/if (DOT) /call cast_cycle_dot ${maDebug.Find[dot].Value} TARGET
			/call control_routine ${maDebug.Find[mez].Value}
			/if (NUKE) /call cast_cycle_nuke ${maDebug.Find[nuke].Value} TARGET
			/call control_routine ${maDebug.Find[mez].Value}
			/if (!${timer_Check_Buff_Temp} && ${maBuff.Find[swBuffTemp].Value}) {
				/call check_buff_cycle_temp_list ${maDebug.Find[temp].Value} stTerror lsTerrorToon
			}
			/call control_routine ${maDebug.Find[mez].Value}
			| /if (${swAlliance}) /call trigger_alliance TARGET

			/call check_item_cycle ${maDebug.Find[item].Value} TARGET
		} else /if (VALID && ENGINE3) {
			/if (${SubDefined[e3_combat_${Me.Class.ShortName}]}) {
				/call e3_combat_${Me.Class.ShortName} ${maDebug.Find[e3].Value}
			}

		}

	}

	/if (AUTO && !ENGINE3) {
		/if (${maChr.Find[swAACrystals].Value} && !${timer_Check_Crystals}) {
			/call check_crystals ${maDebug.Find[crystal].Value}
		}
		/if (${maChr.Find[stPctManaRecovery].Value}) {
			/call do_mana_recovery ${maDebug.Find[manarecover].Value}
		}
		/if (${maChr.Find[stPctGather].Value}) {
			/call check_gather_${Me.Class.ShortName} ${maDebug.Find[gather].Value}
		}
		| /if (${switchCharmPet} && ${swPet} && !${Me.Pet.ID}) {
		| 	/call find_mob_to_charm
		| }
	} else /if (AUTO && ENGINE3) {
		/if (${SubDefined[e3_post_${Me.Class.ShortName}]}) {
			/call e3_post_${Me.Class.ShortName} ${maDebug.Find[e3].Value}
		}
	}

/return TRUE




|***
 * note: basic DPS discs
 * use:
 ***|
sub misc_combat(bool _debug, int _tmpID)
	DEBUG \atmisc_combat\ax(\a-w${_tmpID}\ax)
	FLOW misc_combat ${_tmpID} (1)

	ISMEDEAD
	/invoke ${target[assist, ${_tmpID}]}

	CHECKEXIT misc_combat
	FLOW misc_combat_ENC ${_tmpID} (2)
	GETINPUT
	CHECKTIE
	CHECKDEFENSE
	/if (!PULL) {
		/call get_assist_target ${maDebug.Find[target].Value}
		/if (TARGET != ${_tmpID}) /varset _tmpID TARGET
	}

	/if (SWARM || PET || ${maMinion.Find[swPetCharm].Value}) {
		/call send_pet ${maDebug.Find[sic].Value} ${_tmpID}
	}
	/if (AUTO && MELEE) {
		/call check_facing ${maDebug.Find[face].Value} ${_tmpID} FORCE
	}

	/if (ENGINE2) {
		/call cast_cycle_miscdps ${maDebug.Find[agro].Value} ${_tmpID}
		/return TRUE
	}

	| use AA beam of slumber?
	/if (${maChr.Find[swAABoS].Value} && ${madeBuff.Find[swMez].Value}) {
		/if (${Spawn[${_tmpID}].NearestSpawn[3,npc radius 30].ID} && ${Spawn[${_tmpID}].Distance3D} <= 200 && ${Spawn[TARGET].Distance3D} >= ENV*.5) {
			/if (${Me.AltAbilityReady[${maChr.Find[stAABeamofSlumber].Value}]}) {
				/if (${Me.Casting.ID}) /stopcast
				/face ID ${_tmpID} ${If[${maHome.Find[swFaceFast].Value},fast,]}
				/if (${is_ready[${_debug}, "${maChr.Find[stAABeamofSlumber].Value}"]}) {
					/call voodoo ${_debug} "${maChr.Find[stAABeamofSlumber].Value}" 0 FALSE
				}
			}
		}
	}

	/if (${target[isdead, ${_tmpID}]}) {
		/return FALSE
	}

	| swarm pet
	/if (SWARM && AGRO) {
		/if (${Target.PctHPs} <= ${maMinion.Find[stPctSwarmEngage].Value}) {
			/if (${is_ready[${_debug}, "${maChr.Find[stAAPhantasmalOpponent].Value}"]}) {
				/call voodoo ${_debug} "${maChr.Find[stAAPhantasmalOpponent].Value}" 0 FALSE
			}
		}
	}

	| group DPS bonus
	/if (${is_ready[${_debug}, "${maChr.Find[stAAChromaticHaze].Value}"]}) {
		/call voodoo ${_debug} "${maChr.Find[stAAChromaticHaze].Value}" 0 FALSE
	}

/return TRUE



|***
 * note: Keep the chanter stocked with self AA crystals
 * use: /call check_crystals DEBUG
 ***|
sub check_crystals(bool _debug)
	DEBUG \atcheck_crystals\ax()
	FLOW check_crystals

	/invoke ${set_data_timer[FALSE, Check_Crystals, RESTART]}

	ISMEDEAD
	CHECKEXIT check_crystals
	GETINPUT
	CHECKTIE

	| mana crystal
	/if (!${FindItemCount[${Spell[${maChr.Find[stAAAzureMindCrystal].Value}].Base[1]}]}) {
		/if (${is_ready[${_debug}, "${maChr.Find[stAAAzureMindCrystal].Value}"]}) {
			/call voodoo ${_debug} "${maChr.Find[stAAAzureMindCrystal].Value}" 0 FALSE
		}
		/autoinventory
		CHECKCURSOR FALSE clear
	}

	| hp crystal
	/if (!${FindItemCount[${Spell[${maChr.Find[stAASanguineMindCrystal].Value}].Base[1]}]}) {
		/if (${is_ready[${_debug}, "${maChr.Find[stAASanguineMindCrystal].Value}"]}) {
			/call voodoo ${_debug} "${maChr.Find[stAASanguineMindCrystal].Value}" 0 FALSE
		}
		/autoinventory
		CHECKCURSOR FALSE clear
	}

/return TRUE



|***
 * note: needs manna!!!
 * use: /call check_gather DEBUG
 ***|
sub check_gather(bool _debug)
	DEBUG \atcheck_gather\ax()
	FLOW check_gather (1)

	/if (${Me.PctMana} >= ${maChr.Find[stPctGather].Value}*.PCTHO) /return FALSE

	ISMEDEAD
	CHECKEXIT check_gather_ENC
	FLOW check_gather (2)
	GETINPUT
	CHECKTIE

	| AA manna crystal
	/if (${maChr.Find[swAACrystals].Value}) {
		/if (${is_ready[${_debug}, "${FindItem[${Spell[Azure Mind Crystal].Base[1]}].Name}"]}) {
			/call voodoo ${_debug} "${FindItem[${Spell[Azure Mind Crystal].Base[1]}].Name}" 0 FALSE
		}
	}

	/if (${Me.PctMana} >= ${maChr.Find[stPctGather].Value}*.PCTHO) /return FALSE

	| aa Gather Mana
	/if (${is_ready[${_debug}, "${maChr.Find[stAAGatherMana].Value}"]}) {
		/call voodoo ${_debug} "${maChr.Find[stAAGatherMana].Value}" 0 FALSE
	}

/return TRUE



|***
 * note: chanter self healing? like this is really gonna work. wtf
 * use: /call check_heal_${Me.Class.ShortName} ${maDebug.Find[self].Value} Self
 ***|
sub check_heal(bool _debug, string _type)
	DEBUG \atcheck_heal\ax(\a-w${_type}\ax)
	FLOW check_heal ${_type} (1)

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
	}

  /if (!${get_hurt[${maDebug.Find[hurt].Value}, ${_type}]}) {
 		/return FALSE
 	}


	/if (${Select[${_type},self]}) {
		/if (${Me.PctHPs} > ${maHeal.Find[stHealPoint${Me.Class.ShortName}].Value}*.PCTHO) {

			| AA Sanguine mind crystal summoned item 5 min
			/if (${maChr.Find[swAACrystals].Value}) {
				/if (${is_ready[${_debug}, "${maChr.Find[stAASanguineMindCrystal].Value}"]}) {
					/call voodoo ${_debug} "${maChr.Find[stAASanguineMindCrystal].Value}" 0 FALSE
				}
			}

			| rune and anti hate 20 min
			/if (${is_ready[${_debug}, "${maChr.Find[stAADimensionalShield].Value}"]}) {
				/call voodoo ${_debug} "${maChr.Find[stAADimensionalShield].Value}" 0 FALSE
				/return
			}

			| 3 decoys 10 min
			| /if (${is_ready[${_debug}, "${maChr.Find[stAADoppelganger].Value}"]}) {
			| 	/call voodoo ${_debug} "${maChr.Find[stAADoppelganger].Value}" ${Me.ID} FALSE
			| }

		}
	}

  /if (!${get_hurt[${maDebug.Find[hurt].Value}, ${_type}]}) {
 		/return FALSE
 	}

	GETINPUT ${Me.Class.ShortName}_heal_end type|${_type}

	/call cast_cycle_heal ${_debug} ${hurtID} single

	SETHUD EMPTY

/return TRUE



|***
 * note: toast should not be burnt
 * use: establish all comp/trigger skills
 ***|
sub burn(bool _debug, int _tmpID)
	DEBUG \atburn\ax(\a-w${_tmpID}\ax)
	FLOW burn (1)

	ISMEDEAD
	/if (${target[isdead, ${_tmpID}]}) {
		/return FALSE
	}

	CHECKEXIT
	FLOW burn (2)
	GETINPUT
	CHECKTIE
	CHECKDEFENSE
	/if (!PULL) {
		/call get_assist_target ${maDebug.Find[target].Value}
		/if (TARGET != ${_tmpID}) /varset _tmpID TARGET
	}

	/if (SWARM || PET || ${maMinion.Find[swPetCharm].Value}) {
		/call send_pet ${maDebug.Find[sic].Value} ${_tmpID}
	}
	/if (AUTO && MELEE) {
		/call check_facing ${maDebug.Find[face].Value} ${_tmpID} FORCE
	}

	/if (ENGINE2) {
		/call cast_cycle_burn ${maDebug.Find[burn].Value} ${_tmpID}
		/return TRUE
	}

	/if (${is_ready[${_debug}, "${maChr.Find[stAAIllusionsofGrandeur].Value}"]}) {
		/call voodoo ${_debug} "${maChr.Find[stAAIllusionsofGrandeur].Value}" 0 FALSE
	}

	/if (${is_ready[${_debug}, "${maChr.Find[stAAMentalContortion].Value}"]}) {
		/call voodoo ${_debug} "${maChr.Find[stAAMentalContortion].Value}" ${_tmpID} FALSE
	}

	/if (${target[isdead, ${_tmpID}]}) /return FALSE

	/call cast_AA_spire ${maDebug.Find[spire].Value}

	/if (${is_ready[${_debug}, "${maChr.Find[stAAImprovedTwincast].Value}"]}) {
		/call voodoo ${_debug} "${maChr.Find[stAAImprovedTwincast].Value}" 0 FALSE
	}

	| # DD spell damage bonus
	/if (${is_ready[${_debug}, "${maChr.Find[stAACalculatedInsanity].Value}"]}) {
		/call voodoo ${_debug} "${maChr.Find[stAACalculatedInsanity].Value}" 0 FALSE
	}

	/if (${target[isdead, ${_tmpID}]}) /return FALSE

	/if (${is_ready[${_debug}, "${maChr.Find[stAASilentCasting].Value}"]}) {
		/call voodoo ${_debug} "${maChr.Find[stAASilentCasting].Value}" 0 FALSE
	}

	/if (${is_ready[${_debug}, "${maChr.Find[stAADimensionalShield].Value}"]}) {
		/call voodoo ${_debug} "${maChr.Find[stAADimensionalShield].Value}" 0 FALSE
	}

	| resist mod
	/if (${is_ready[${_debug}, "${maChr.Find[stAAFocusofArcanum].Value}"]}) {
		/call voodoo ${_debug} "${maChr.Find[stAAFocusofArcanum].Value}" 0 FALSE
	}

	| Group DD Bonus
	/if (${is_ready[${_debug}, "${maChr.Find[stAAChromaticHaze].Value}"]}) {
		/call voodoo ${_debug} "${maChr.Find[stAAChromaticHaze].Value}" 0 FALSE
	}

	/if (${target[isdead, ${_tmpID}]}) /return FALSE

	/call cast_glyph ${maDebug.Find[glyph].Value}
  | Intensity of the Resolute AA 4 hour reuse
  /call cast_AA_intensity ${maDebug.Find[intensity].Value}

/return






|***
 * note: do people still really do this shit?
 * use: /call find_mob_to_charm()
 ***|
sub find_mob_to_charm(bool _debug)
	DEBUG \a-pfind_mob_to_charm\ax()

	/if (${Me.Pet.ID}) {
		/return TRUE
	}


	/declare _count int local 0
	/declare _spawns int local 0
	/declare _tmpID int local 0
	/declare _maxlvl int local ${Spell[${maChr.Find[stCharm].Value}].Max[1]}

	/delay 1

	/if (${SpawnCount[npc los Range 1 ${_maxlvl} radius ENV targetable]}) {
		/varset _spawns ${SpawnCount[npc los Range 1 ${_maxlvl} radius ENV targetable]}
		/for _count 1 to ${_spawns}
			/varset _tmpID ${NearestSpawn[${_count}, npc los Range 1 ${_maxlvl} radius ENV targetable].ID}
			/if (${_tmpID} == TARGET) /continue
			/if (${lsZoneCharm.Contains[${Spawn[id ${_tmpID}].DisplayName}]} && !${Spawn[${_tmpID}].Master.Type.Equal[Pet]} && !${Spawn[${_tmpID}].Master.Type.Equal[Corpse]}) {

				/squelch /target ID ${_tmpID}
				/if (!${Target.Tashed.Duration}) {
					/call voodoo ${_debug} {madeBuff.Find[stTash].Value} ${_tmpID} FALSE

					/while (${Me.SpellInCooldown}) {
						CHECKEXIT
						ISMEDEAD
						/if (!${Me.SpellInCooldown}) /break
						/delay 5
					}

				| /if (${is_ready[${_debug}, spell, "${spellpbaeMez}", ${Me.ID}]}) /call voodoo ${_debug} "${spellpbaeMez}" 0 check_for_adds

				/if (${Target.Level} <= 46) {
					/if (${is_ready[${_debug}, "Dire Charm"]}) {
						/call voodoo ${_debug} "Dire Charm" ${_tmpID} FALSE
					}
				} else {
					/if (${is_ready[${_debug}, "${maChr.Find[stCharm].Value}"]}) {
						/call voodoo ${_debug} "${maChr.Find[stCharm].Value}" ${_tmpID} FALSE
					}
				}

				/break
			}
		/next _count
	}
/return



|***
 * note: class control
 * use: /chr
 ***|
sub set_control(string _type, string _verbage, string _verbage2, bool _debug)
	DEBUG \atset_control\ax(\a-w${_type}, "${_verbage}", ${_verbage2}\ax)

	| set gather use %
	/if (${_type.Equal[gather]}) {
		/if (!${set_control_num_range[${_debug}, stPctGather, "${_verbage}", 0, 99]}) /return FALSE

	| use AA crystals
	} else /if (${_type.Equal[crystal]}) {
		/invoke ${set_switch_env[${_debug}, swAACrystals, TRUE, ${_verbage}]}

	| use AA Beam of Slumber
	} else /if (${_type.Equal[bos]}) {
		/invoke ${set_switch_env[${_debug}, swAABoS, TRUE, ${_verbage}]}

	}

	/call set_control_shared ${_type} "${_verbage}" "${_verbage2}" ${_debug}
	/if (${Macro.Return.Equal[SKIP]}) /return FALSE

	/if (${_verbage2.Equal[SILENT]}) /return
	/invoke ${set_control_output[${_debug}, maChr, chr, maChrControl]}

/return TRUE



