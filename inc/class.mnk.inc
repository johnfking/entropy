|***
 * entropy
 * class.mnk.inc
 ***|



|***
 * note: primary initilization of all non shared data
 * use: not for you
 ***|
sub class_init(bool _debug)
  DEBUG \atclass_init\ax()

  /if (${maDebug.Find[init].Value}) {
    /declare _boottimestart int local ${MacroQuest.Running}
  }

  /call set_data ${_debug} maData stSpire                             map outer "Spire of the Sensei"
  /call set_data ${_debug} maData stSynergy                           map outer "Calanin's Synergy"
  /call set_data ${_debug} maData stFade                              map outer "Imitate Death"
  /call set_data ${_debug} maData stCompleteHeal                      map outer "Complete Heal"

  /call set_data_map ${_debug} maChr stRest                           PREGEN|1                      "${auxna} name of stamina rest disc"
  /call set_data_map ${_debug} maChr stHiatus                         PREGEN|1                      "${auxna} name of hiatus stamina recovery disc"
  /call set_data_map ${_debug} maChr swHiatus                         FALSE                         "${auxsw} use hiatus rest disc"
  /call set_data_map ${_debug} maChr swTrainMelee                     FALSE                         "${auxsw} alternate skills for training"

  /call set_data_map ${_debug} maChr stPctFDAggro                     0                             "${auxnum} % agro for fd"
  /call set_data_map ${_debug} maChr stPctFDHP                        0                             "${auxnum} % health fd at"

  /call set_data_map ${_debug} maChr stDichotomic                     PREGEN|1                      "${auxna} Dichotomic disc family"
  /call set_data_map ${_debug} maChr stBalance                        PREGEN|1                      "${auxna} Tiger's Balance disc family"
  /call set_data_map ${_debug} maChr stMonkey                         PREGEN|1                      "${auxna} Drunken Monkey Style disc family"
  /call set_data_map ${_debug} maChr stHeel                           PREGEN|1                      "${auxna} Heel of Kanji disc family"
  /call set_data_map ${_debug} maChr stDiscipline                     PREGEN|1                      "${auxna} Thunderkick Discipline disc family"
  /call set_data_map ${_debug} maChr stCrane                          PREGEN|1                      "${auxna} Crane Stance disc family"
  /call set_data_map ${_debug} maChr stPunch                          PREGEN|1                      "${auxna} Punch Through disc family"
  /call set_data_map ${_debug} maChr stSynergy                        PREGEN|1                      "${auxna} Shadewalker's Synergy disc family"
  /call set_data_map ${_debug} maChr stElbow                          PREGEN|1                      "${auxna} Elbow Strike disc family"
  /call set_data_map ${_debug} maChr stCurse                          PREGEN|1                      "${auxna} Curse of the Thirteen Fingers disc family"
  /call set_data_map ${_debug} maChr stPrecision                      PREGEN|1                      "${auxna} Set to Icewalker's Precision Strike"
  /call set_data_map ${_debug} maChr stPrecisionTwo                   PREGEN|1                      "${auxna} Set to Firewalker's Precision Strike"
  /call set_data_map ${_debug} maChr stPrecisionThree                 PREGEN|1                      "${auxna} Set to Doomwalker's Precision Strike"
  /call set_data_map ${_debug} maChr stShuriken                       PREGEN|1                      "${auxna} Vigorous Shuriken disc family"
  /call set_data_map ${_debug} maChr stStorm                          PREGEN|1                      "${auxna} Eye of the Storm disc family"
  /call set_data_map ${_debug} maChr stDeath                          PREGEN|1                      "${auxna} Delay Death disc family"
  /call set_data_map ${_debug} maChr stWu                             PREGEN|1                      "${auxna} Fists of Wu disc family"
  /call set_data_map ${_debug} maChr stEarthwalk		      		        PREGEN|1			    		        "${auxna} Earthwalk Discipline family"
  /call set_data_map ${_debug} maChr stSpeed			      		          PREGEN|1			    		        "${auxna} Speed Focus Disc"
  /call set_data_map ${_debug} maChr stInnerflame		      		        PREGEN|1			    		        "${auxna} Hundred Fists Discipline family"

  | Fists of Steel
  /invoke ${set_data_timer[${_debug}, Check_Proc_Fist, 5s]}
  /call set_data_map ${_debug} maChr swFistofSteel                    FALSE                         "${auxsw} use Fist of Steel AA"
  /call set_data_map ${_debug} maChr stFistofSteelBandolier           FALSE                         "${auxna} name of bandolier to trigger Fist of steel aa"

  | class order lists
  /call set_data_list ${_debug} maChr lsOrderClassBase                EMPTY                                    "${auxli} class base loop start"
  /call set_data_list ${_debug} maChr lsOrderClassPre                 heal                                      "${auxli} class pre combat"
  /call set_data_list ${_debug} maChr lsOrderClass                    melee|burn|misc|debuff|dot|nuke|item      "${auxli} class active combat order"
  /call set_data_list ${_debug} maChr lsOrderClassPost                manarecover                               "${auxli} class post combat"

  | AAs
  /call set_data_map ${_debug} maChr stAAUnity                        FALSE                         "${auxna} class aa unity buff"
  /call set_data_map ${_debug} maChr stAASilentStrikes                PREGEN|1                      "${auxna} Silent Strikes AA"
  /call set_data_map ${_debug} maChr stAAZanFisWhistle                PREGEN|1                      "${auxna} Zan Fi's Whistle AA"
  /call set_data_map ${_debug} maChr stAATwoFingerWaspTouch           PREGEN|1                      "${auxna} Two-Finger Wasp Touch AA"
  /call set_data_map ${_debug} maChr stAAInfusionofThunder            PREGEN|1                      "${auxna} Infusion of Thunder AA"
  /call set_data_map ${_debug} maChr stAAFocusedDestructiveForce      PREGEN|1                      "${auxna} Focused Destructive Force AA"
  /call set_data_map ${_debug} maChr stAADestructiveForce			        PREGEN|1						          "${auxna} Destructive Force AA"
  /call set_data_map ${_debug} maChr stAAFivePointPalm                PREGEN|1                      "${auxna} Five Point Palm AA"
  /call set_data_map ${_debug} maChr stAASwiftTailsChant              PREGEN|1                      "${auxna} Swift Tail's Chant AA"
  /call set_data_map ${_debug} maChr stAAFeignDeath                   PREGEN|1                      "${auxna} Feign Death AA"
  /call set_data_map ${_debug} maChr stAADestructiveForce			        PREGEN|1						          "${auxna} Destructive Force AA"
  /call set_data_map ${_debug} maChr stAATonPoStance		              PREGEN|1			                "${auxna} Ton Po's Stance AA"

  /if (${maDebug.Find[init].Value}) {
    OUT \agset_${Me.Class.ShortName}\ax${sep}\a-w${Math.Calc[((${MacroQuest.Running}-${_boottimestart}) / 10) / 60]}s\ax
    /mqp
  }

/return TRUE




|***
 * note: kick 'em in the nuts
 * use:
 ***|
sub class_main(bool _debug)
  DEBUG \atclass_main\ax()

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
 * note: al the extra combat shit
 * use:
 ***|
sub misc_combat(bool _debug, spawn _spawn)
  DEBUG \atmisc_combat\ax(\a-w${_spawn.ID}\ax)

  SPAWNDEAD
  CHECKEXIT
  GETINPUT

  /if (SWARM || PET) {
    /call send_pet ${maDebug.Find[sic].Value} ${_spawn.ID}
  }
  /if (AUTO && MELEE) {
    /invoke ${spawn_face[${_spawn.ID}]}
  }

  /if (ENGINE2) {
    /call cast_cycle_miscdps ${maDebug.Find[agro].Value} ${_spawn.ID}
    /return TRUE
  }
  
  /call check_fd ${_debug}

  | bandolier swap for AA Fist of Steel
  /if (${maChr.Find[swFistofSteel].Value} && !${timer_Check_Proc_Fist}) {
    /call check_fists_of_steel ${maDebug.Find[fist].Value}
  }

  /if (!AGRO) {
    /if (${cast_data[${_debug}, "${maChr.Find[stAASilentStrikes].Value}"]}) {
      /call cast ${_debug} 0 FALSE
    }
  }

  /if (${cast_data[${_debug}, "${maChr.Find[stWu].Value}"]} && !${Me.Song[${maChr.Find[stWu].Value}].ID}) {
    /call cast ${_debug} 0 FALSE
  }

  /if (${cast_data[${_debug}, "${maChr.Find[stSynergy].Value}"]}) {
    /call cast ${_debug} ${_spawn.ID} FALSE
  }
  
  /if (${cast_data[${_debug}, "${maChr.Find[stPunch].Value}"]}) {
    /call cast ${_debug} ${_spawn.ID} FALSE
  }
														  
  /if (${cast_data[${_debug}, "${maChr.Find[stElbow].Value}"]}) {
    /call cast ${_debug} ${_spawn.ID} FALSE
  }

  /if (${cast_data[${_debug}, "${maChr.Find[stCurse].Value}"]}) {
    /call cast ${_debug} ${_spawn.ID} FALSE
  }

  /if (${cast_data[${_debug}, "${maChr.Find[stPrecision].Value}"]}) {
    /call cast ${_debug} ${_spawn.ID} FALSE
  }
  
  /if (${cast_data[${_debug}, "${maChr.Find[stPrecisionTwo].Value}"]}) {
    /call cast ${_debug} ${_spawn.ID} FALSE
  }
  
  /if (${cast_data[${_debug}, "${maChr.Find[stPrecisionThree].Value}"]}) {
    /call cast ${_debug} ${_spawn.ID} FALSE
  }

  /if (${cast_data[${_debug}, "${maChr.Find[stShuriken].Value}"]} && ${Spawn[${_spawn.ID}].Distance} > 31) {
    /call cast ${_debug} ${_spawn.ID} FALSE
  }
  
  /if (${cast_data[${_debug}, "${maChr.Find[stMonkey].Value}"]}) {
    /call cast ${_debug} 0 FALSE
  }

  /if (!BURN || !${cast_data[${_debug}, "${maChr.Find[stHeel].Value}"]}) {
    /if (!BURN || !${cast_data[${_debug}, "${maChr.Find[stSpeed].Value}"]}) {
      /if (!BURN || !${cast_data[${_debug}, "${maChr.Find[stInnerflame].Value}"]}) {
        /if (${cast_data[${_debug}, "${maChr.Find[stStorm].Value}"]}) {
          /call cast ${_debug} 0 FALSE
        }
      }
    }
  }
  
  /if (!BURN || !${cast_data[${_debug}, "${maChr.Find[stHeel].Value}"]}) {
    /if (!BURN || !${cast_data[${_debug}, "${maChr.Find[stSpeed].Value}"]}) {
      /if (!BURN || !${cast_data[${_debug}, "${maChr.Find[stInnerflame].Value}"]}) {
        /if (${cast_data[${_debug}, "${maChr.Find[stEarthwalk].Value}"]}) {
          /call cast ${_debug} 0 FALSE
        }
      }
    }
  }
  
  /if (${cast_data[${_debug}, "${maChr.Find[stAAZanFisWhistle].Value}"]}) {
    /call cast ${_debug} 0 FALSE
  }

  /if (${cast_data[${_debug}, "${maChr.Find[stAATwoFingerWaspTouch].Value}"]}) {
    /call cast ${_debug} ${_spawn.ID} FALSE
  }
  
  /if (${Me.ActiveDisc.ID} && !${Me.ActiveDisc.Name.Find[Heel]}) { 
    /if (${Me.ActiveDisc.ID} && !${Me.ActiveDisc.Name.Find[Ironfist]}) { 
      /if (${cast_data[${_debug}, "${maChr.Find[stAAVehementRage].Value}"]}) {
        /call cast ${_debug} ${_spawn.ID} FALSE
      }
    }
  }
  
  /call check_fd ${_debug}

/return TRUE



|***
 * note: healing
 * use: auto
 ***|
sub check_heal(bool _debug, string _type)
  DEBUG \atcheck_heal\ax(\a-w${_type}\ax)

  CHECKEXIT

  | mend training
  /if (${maChr.Find[swTrainMelee].Value}) {
    /if (${Me.Skill[Mend]} < ${Me.SkillCap[Mend]} && ${Me.AbilityReady[Mend]}) {
      /doability Mend
    }
  }

  /call spawn_hurt ${maDebug.Find[hurt].Value} ${_type}

  | if no one is hurt. back to the start
  /if (!${hurtCount}) {
    SETHUD
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

    /if (${Select[${_type},self]} && ${hurtCount}) {
      /if (${Me.AbilityReady[Mend]}) {
        /doability Mend
        /return
      }
    }
  }

  /if (!${spawn_hurt[${maDebug.Find[hurt].Value}, ${_type}]}) {
    /return FALSE
  }

  /call cast_cycle_heal ${_debug} ${hurtData.ID} single

  SETHUD

/return TRUE



|***
 * note: Monk burn routines
 * use: you made a monk... hahaha
 ***|
sub burn(bool _debug, spawn _spawn)
  DEBUG \atburn\ax(\a-w${_spawn.ID}\ax)

  ISMEDEAD
  /if (${target[isdead, ${_spawn.ID}]}) {
    /return FALSE
  }

  CHECKEXIT
  GETINPUT

  /if (SWARM || PET) {
    /call send_pet ${maDebug.Find[sic].Value} ${_spawn.ID}
  }
  /if (AUTO && MELEE) {
    /invoke ${spawn_face[${_spawn.ID}]}
  }

  /if (ENGINE2) {
    /call cast_cycle_burn ${maDebug.Find[burn].Value} ${_spawn.ID}
    /return TRUE
  }

  | Check if we're using a disc, if not use Heel, if Heel is ready and we're using a different disc stop it and use Heel
	| Also make sure we are maxing out our burns
	| Primary burn: Heel + IoT + Dicho + Spire + Eagle's Poise + Ton Po + BP
	
  /if (${Me.ActiveDisc.ID} && !${Me.ActiveDisc.Name.Find[Heel]}) { 
    /if (${cast_data[${_debug}, "${maChr.Find[stHeel].Value}"]}) {
      /stopdisc
    }
  } 
  
  /if (!${Me.ActiveDisc.ID}) {
    /if (${cast_data[${_debug}, "${maChr.Find[stHeel].Value}"]}) {
      /stopdisc
      /call cast ${_debug} ${_spawn.ID} FALSE

      | About to draw aggro, get silent strikes up and running
      /if (${cast_data[${_debug}, "${maChr.Find[stAASilentStrikes].Value}"]}) {
        /call cast ${_debug} 0 FALSE
      }

      /if (${cast_data[${_debug}, "${maChr.Find[stAAInfusionofThunder].Value}"]}) {
        /call cast ${_debug} 0 FALSE
      }

      /call cast_AA_spire ${maDebug.Find[spire].Value}

      /if (${cast_data[${_debug}, "${maChr.Find[stBalance].Value}"]}) {
        /call cast ${_debug} 0 FALSE
      }
      
      | Five Point Palm once fully stacked 
      /if (${cast_data[${_debug}, "${maChr.Find[stAAFivePointPalm].Value}"]}) {
        /call cast ${_debug} ${_spawn.ID} FALSE
      }

      | Always use Dicho and Ton Po's together
      /if (${cast_data[${_debug}, "${maChr.Find[stDichotomic].Value}"]}) {
        /if (${cast_data[${_debug}, "${maChr.Find[stAATonPoStance].Value}"]}) {
          /call cast_data ${_debug} "${maChr.Find[stDichotomic].Value}"
          /call cast ${_debug} 0 FALSE
          /call cast_data ${_debug} "${maChr.Find[stAATonPoStance].Value}"
          /call cast ${_debug} 0 FALSE
        }
      }

      /if (${cast_data[${_debug}, "${maChr.Find[stCrane].Value}"]}) {
        /call cast ${_debug} ${_spawn.ID} FALSE
      }

      /if (${cast_data[${_debug}, "${maChr.Find[stAASwiftTailsChant].Value}"]}) {
        /call cast ${_debug} 0 FALSE
      }

      /call cast_glyph ${maDebug.Find[glyph].Value}

      | Intensity of the Resolute AA 4 hour reuse
      /call cast_AA_intensity ${maDebug.Find[intensity].Value}
    }
  }
  
  | If Heel has been used, use Speed Focus
  | Secondary Burn: Speed + FDF + IoT + Composite + Spire
  /if (!${Me.ActiveDisc.Name.Find[Heel]} && !${cast_data[${_debug}, "${maChr.Find[stHeel].Value}"]}) {
    /if (${cast_data[${_debug}, "${maChr.Find[stSpeed].Value}"]}) {
      /stopdisc
      /call cast ${_debug} 0 FALSE

      | About to draw aggro, get silent strikes up and running
      /if (${cast_data[${_debug}, "${maChr.Find[stAASilentStrikes].Value}"]}) {
        /call cast ${_debug} 0 FALSE
      }

      /if (!AE#) {
        /if (${cast_data[${_debug}, "${maChr.Find[stAAFocusedDestructiveForce].Value}"]}) {
          /call cast ${_debug} 0 FALSE
        }
      }

      /if (AE#) {
        /if (${cast_data[${_debug}, "${maChr.Find[stAADestructiveForce].Value}"]}) {
          /call cast ${_debug} 0 FALSE
        }
      }

      /if (${cast_data[${_debug}, "${maChr.Find[stAAInfusionofThunder].Value}"]}) {
        /call cast ${_debug} 0 FALSE
      }

      /call cast_AA_spire ${maDebug.Find[spire].Value}

      | Five Point Palm once fully stacked
      /if (${cast_data[${_debug}, "${maChr.Find[stAAFivePointPalm].Value}"]}) {
        /call cast ${_debug} ${_spawn.ID} FALSE
      }

      /if (${cast_data[${_debug}, "${maChr.Find[stAASwiftTailsChant].Value}"]}) {
        /call cast ${_debug} 0 FALSE
      }
    }
  }
  
  | If Heel and Speed Focus have been used, use Terror Palm
  /if (!${Me.ActiveDisc.Name.Find[Heel]} && !${cast_data[${_debug}, "${maChr.Find[stHeel].Value}"]}) {
    /if (!${Me.ActiveDisc.Name.Find[Speed]} && !${cast_data[${_debug}, "${maChr.Find[stSpeed].Value}"]}) {
      /if (${cast_data[${_debug}, "${maChr.Find[stInnerflame].Value}"]}) {
        /stopdisc
        /call cast ${_debug} 0 FALSE
        /delay 1
        /if (${cast_data[${_debug}, "${maChr.Find[stAAFivePointPalm].Value}"]}) {
          /call cast ${_debug} ${_spawn.ID} FALSE
        }

        | Always use Dicho and Ton Po's together
        /if (${cast_data[${_debug}, "${maChr.Find[stDichotomic].Value}"]}) {
          /if (${cast_data[${_debug}, "${maChr.Find[stAATonPoStance].Value}"]}) {
            /call cast ${_debug} "${maChr.Find[stDichotomic].Value}" 0 FALSE
            /call cast ${_debug} "${maChr.Find[stAATonPoStance].Value}" 0 FALSE
          }
        }

        /if (${cast_data[${_debug}, "${maChr.Find[stCrane].Value}"]}) {
          /call cast ${_debug} ${_spawn.ID} FALSE
        }
      }
    }
  }
  
  /if (${Me.ActiveDisc.ID} && !${Me.ActiveDisc.Name.Find[Heel]}) { 
    /if (${Me.ActiveDisc.ID} && !${Me.ActiveDisc.Name.Find[Speed]}) { 
      /if (${Me.ActiveDisc.ID} && !${Me.ActiveDisc.Name.Find[Terror]}) {
        /if (${cast_data[${_debug}, "${maChr.Find[stDiscipline].Value}"]}) {
          /stopdisc
          /call cast ${_debug} 0 FALSE
          /if (${cast_data[${_debug}, "${maChr.Find[stCrane].Value}"]}) {
            /call cast ${_debug} ${_spawn.ID} FALSE
          }
        }	
      }
    }
  }

  /call check_fd ${_debug}

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
      /if (${cast_data[${_debug}, "${maChr.Find[stFade].Value}"]}) {
        /call cast ${_debug} 0 FALSE
      }
      /return
    }
    /stand
    /if (${Me.AbilityReady[Mend]}) /doability Mend
    /if (${cast_data[${_debug}, "${maChr.Find[stDeath].Value}"]}) {
      /call cast ${_debug} 0 FALSE
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
      /if (${Me.AbilityReady[${maChr.Find[stAAFeignDeath].Value}]}) {
        /attack off
        /doability "${maChr.Find[stAAFeignDeath].Value}"
        /delay 5
      }
      /doevents MNKfdcheck
      /if (!AUTO) {
        /call set_stop ${maDebug.Find[stop].Value} SILENT
        /call clear_combat FALSE
        /return
      }
    }
  } else /if (${maChr.Find[stPctFDAggro].Value} && !AGRO && AUTO && ${Me.PctAggro} >= ${maChr.Find[stPctFDAggro].Value}) {
    /if (!${Me.State.Equal[FEIGN]} && !${swIsMeDead}) {
      /if (${Me.AbilityReady[${maChr.Find[stAAFeignDeath].Value}]}) {
        /attack off
        /doability "${maChr.Find[stAAFeignDeath].Value}"
        /delay 5
      }

      /doevents MNKfdcheck

      /if (!AUTO) {
        /call set_stop ${maDebug.Find[stop].Value} SILENT
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
 * note: bandolier swap to proc AA Fist of Steel
 * use: /call check_fists_of_steel
 ***|
sub check_fists_of_steel(bool _debug)
  DEBUG \atcheck_fists_of_steel\ax()

  | make sure the settings we need are there
  /if (!${maChr.Find[swFistofSteel].Value}) {
    /invoke ${set_data_timer[${_debug}, Check_Proc_Fist, 30s]}
    /return FALSE
  }

  /invoke ${set_data_timer[${_debug}, Check_Proc_Sash, RESTART]}

  | set stuff back the way it was
  /if (${Me.Song[Fists of Fury].ID} && ${Me.Bandolier[${maChr.Find[stFistofSteelBandolier].Value}].Active}) {
    | DEBUG ${sep}buff found${sep}\a-w${_buff}\ax

    | primary
    /if (${Bool[${maData.Find[stMainhand].Value}]}) {
      /call Bind_command_swap "${maData.Find[stMainhand].Value}" 13 ${_debug}
    }

    | secondary
    /if (${Bool[${maData.Find[stOffhand].Value}]}) {
      /call Bind_command_swap "${maData.Find[stOffhand].Value}" 14 ${_debug}
    }

    | ranged
    /if (${Bool[${maData.Find[stRanged].Value}]}) {
      /call Bind_command_swap "${maData.Find[stRanged].Value}" 11 ${_debug}
    }

    | ammo
    /if (${Bool[${maData.Find[stAmmo].Value}]}) {
      /call Bind_command_swap "${maData.Find[stAmmo].Value}" 22 ${_debug}
    }
    /return TRUE
  }

  /if (!!${Me.Song[Fists of Fury].ID} && !${Me.Bandolier[${maChr.Find[stFistofSteelBandolier].Value}].Active}) {
    /invoke ${maData.Add[stMainhand,${Me.Inventory[mainhand].Name}]}
    /invoke ${maData.Add[stOffhand,${Me.Inventory[offhand].Name}]}
    /invoke ${maData.Add[stRanged,${Me.Inventory[ranged].Name}]}
    /invoke ${maData.Add[stAmmo,${Me.Inventory[ammo].Name}]}
    /delay 1
    /invoke ${Me.Bandolier[${maChr.Find[stFistofSteelBandolier].Value}].Activate}
  }

/return TRUE



|***
 * note: class control
 * use: /chr
 ***|
sub set_control(string _type, string _verbage, string _verbage2, bool _debug)
  DEBUG \atset_control\ax(\a-w${_type}, "${_verbage}", ${_verbage2}\ax)

  | train skills
  /if (${_type.Equal[train]}) {
    /invoke ${set_switch_env[${_debug}, swTrainMelee, TRUE, ${_verbage}]}

  | set AA fade use %
  } else /if (${_type.Equal[fdhp]}) {
    /if (!${set_control_num_range[${_debug}, stPctFDHP, "${_verbage}", 0, 99]}) /return FALSE

  | fist of steel
  } else /if (${_type.Equal[fist]}) {
    /if (${_verbage.Equal[use]}) {
      /invoke ${set_switch_env[${_debug}, swFistofSteel, TRUE, ${_verbage2}]}
    } else /if (${_verbage.Equal[bandolier]}) {
      /invoke ${do_raw_edit[${_debug}, SILENT, stFistofSteelBandolier, "${_verbage2}"]}
    }

    /invoke ${maControl.Clear}
    /invoke ${maControl.Add[use,swFistofSteel]}
    /invoke ${maControl.Add[bandolier,stFistofSteelBandolier|br]}
    /invoke ${set_control_output[${_debug}, maChr, "chr fist", maControl]}
    /return TRUE

  | set AA fade use %
  } else /if (${_type.Equal[fdaggro]}) {
    /if (!${set_control_num_range[${_debug}, stPctFDAggro, "${_verbage}", 0, 99]}) /return FALSE

  }

  /call set_control_shared ${_type} "${_verbage}" "${_verbage2}" ${_debug}
  /if (${Macro.Return.Equal[SKIP]}) /return FALSE

  /if (${_verbage2.Equal[SILENT]}) /return
  /invoke ${set_control_output[${_debug}, maChr, chr, maChrControl]}

/return TRUE

