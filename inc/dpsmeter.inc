| DPSMeter.inc file by Maskoi V1.02 06/21/2019
| Core/Entropy adjustments Exspes 
| Place at the beginning of your macro where you define your variables
| /if (${SubDefined[DPSMeter_Init]}) /call DPSMeter_Init
| You will need a doevents in your main loop
| /doevents
| Window Hitmodes All reporting must be set to Normal

#Event YourDamage       "#2# is #1# by YOUR #4# for #3# points of non-melee damage.#*#"
#Event YourDamage       "#2# has taken #3# damage from your #5#.#*#"
#Event YourDamage       "You #1# #2# for #3# points of damage.#*#"
#Event YourDamage       "You #1# #2# for #3# points of #4# damage by #5#.#*#"
#Event YourDamage       "You try to #*#, but miss"
#Event YourPetDamage    "|${Me.Pet.CleanName}| #1# #2# for #3# points of damage.#*#"
#Event YourPetDamage    "|${Me.Pet.CleanName}| #1# #2# for #3# points of #4# damage by #5#.#*#"
#Event YourPetDamage    "|${Me}|`s pet #1# #2# for #3# points of damage.#*#"
#Event YourPetDamage    "|${Me}|`s pet #1# #2# for #3# points of #4# damage by #5#.#*#"
#Event SlainBy          "You have slain #1#!"
#Event SlainBy          "#1# has been slain by #*#"
#DEFINE DEBUGDPSMETER    "/if (${DebugDPSMeter}) /echo \arCOMBAT \agL:${Macro.CurLine} T:${Macro.RunTime} \aySub: \aw"

| -------------------------------------------------------------------------------------
| SUB: DPSMeter Init 
| -------------------------------------------------------------------------------------  
Sub DPSMeter_Init()

	| user settings ------

		| write to ini: 0:OFF, 1:ON
		/declare DPSWriteOn            	int         outer			0

	| NOT user setting ------ IE don't touch
	/declare DebugDPSMeter          int         outer       0
	/declare DPSSpam                int         outer				0
	/declare DPSCounter             int         outer       0
	/declare DPSLastTime            int         outer       0
	/declare DPSMeter               int         outer       1
	/declare DPSPetTotal            int         outer       0
	/declare DPSTimeStart           int         outer       0
	/declare DPSMeterTargetID       int         outer       0
	/declare DPSTargetName          string      outer       0
	/declare DPSTotal               int         outer       0
	
	
	| set meter type	
	/if (${Macro.Name.Find[kiss]}) {
		/declare metertype string outer kiss
	} else /if (${Macro.Name.Find[core]}) {
		/declare metertype string outer core
	} else /if (${Macro.Name.Find[entropy]}) {
		/declare metertype string outer entropy
	} else {
		/declare metertype string outer base
	}
	
	
	

/return

| -------------------------------------------------------------------------------------
| SUB: Visual outputs
| -------------------------------------------------------------------------------------  

Sub DPSMeter_Output(string DPSTargetName, DPSLastTime, DPSMyDam, DPSCalc, DPSTCalc, DPSMPTCalc, DPSMeAndPetDam, DPSMyPetDam, DPSPTCalc)

    | KissAssist.mac Output
    /if (${metertype.Equal[kiss]}) {
      /echo SLAIN >> ${DPSTargetName} << 
      /echo \aw DPS Meter - ${DPSTargetName} - \ay Your Total Damage: \at ${CommaMeBro[${DPSTCalc}]} - \ay Fight Time: \aw ${CommaMeBro[${DPSLastTime}]}s - \ay DPS: \ag ${CommaMeBro[${DPSMyDam}]}    
      /if (${Bool[${DPSMPTCalc}]}) {
          /echo \aw DPS Meter - \ay Pet Total Damage: \at ${CommaMeBro[${DPSPTCalc}]} \ay - Pet DPS: \ag ${CommaMeBro[${DPSMyPetDam}]}
          /echo \aw DPS Meter - \ay You + Pet Total Damage: \at ${CommaMeBro[${DPSMPTCalc}]} \ay - DPS: \ag ${CommaMeBro[${DPSMeAndPetDam}]}
      }
      /return

    | Core.mac output
    } else /if (${metertype.Equal[core]}) {
      /if (!${swDPSMeter}) /return
      /if (${Bool[${DPSMPTCalc}]}) OUT DPS Pet:\aw${CommaMeBro[${DPSMyPetDam}]}\ax
      OUT DPS:\aw${CommaMeBro[${DPSMyDam}]} \ax Time:\a-w${CommaMeBro[${DPSLastTime}]}s\ax Total:\a-w${CommaMeBro[${If[${Bool[${DPSMPTCalc}]},${DPSMPTCalc},${DPSTCalc}]}]}\ax ${sep} [${ctar}${DPSTargetName}\ax]
      /return


		| entropy.mac output
		} else /if (${metertype.Equal[entropy]}) {
			/if (!${maEntropy.Find[swDPSMeter].Value}) /return FALSE
      OUT DPS${sep}\aw${CommaMeBro[${DPSMyDam}]} \ax Time${sep}\a-w${CommaMeBro[${DPSLastTime}]}s\ax Total${sep}\a-w${CommaMeBro[${If[${Bool[${DPSMPTCalc}]},${DPSMPTCalc},${DPSTCalc}]}]}\ax ${dot} ${tar}${DPSTargetName}\ax
      /if (${Bool[${DPSMPTCalc}]}) OUT DPS Pet:\aw${CommaMeBro[${DPSMyPetDam}]}\ax
      /return

    | generic output
    } else /if (${metertype.Equal[base]}) {
	    /echo DPS Meter - ${DPSTargetName} - Your Total Damage: ${CommaMeBro[${DPSTCalc}]} - Fight Time: ${CommaMeBro[${DPSLastTime}]}s - DPS: ${CommaMeBro[${DPSMyDam}]}    
	    /if (${Bool[${DPSMPTCalc}]}) {
        /echo DPS Meter - Pet Total Damage: ${CommaMeBro[${DPSPTCalc}]} - Pet DPS: ${CommaMeBro[${DPSMyPetDam}]}
        /echo DPS Meter - You + Pet Total Damage: ${CommaMeBro[${DPSMPTCalc}]} - DPS: ${CommaMeBro[${DPSMeAndPetDam}]}
	    }
	    /return    
	    
	  }
	        
/return

| -------------------------------------------------------------------------------------
| SUB: Your Damage
| ------------------------------------------------------------------------------------- 
Sub Event_YourDamage(string Line, string YHType, string YHMob, int YHDamage, string YHSpellDamageType, string YHSpellName)
	DEBUGDPSMETER Event_YourDamage Enter 
	| Prevents endless MQ2 event loop and crash
	/if (${Line.Find[MQ2]}) /return
    | Flush events if dpsmeter off
    /if (!${DPSMeter}) {
		/doevents flush YourDamage 
		/return
	}
    /declare DPSType string local Melee
	/declare DPSTime int local
	/if ((${DPSMeterTargetID}==0 || !${Spawn[id ${DPSMeterTargetID}].ID}) && ${Target.Type.Equal[npc]}) {
        /varset DPSMeterTargetID ${Target.ID}
		/varset DPSTargetName ${Target.DisplayName}
    }
    | Assign missing undefined vars from dps messages not in melee to prevent #warning 
	/if (!${Defined[YHType]}) /declare YHType string local 
	/if (!${Defined[YHSpellName]}) /declare YHSpellName string local 
	/if (!${Defined[YHSpellDamageType]}) /declare YHSpellDamageType string local 
    | Record start time of fight and assign mob id
	/if (${DPSTimeStart}==0) {
		/varset DPSTimeStart ${Time.SecondsSinceMidnight}
	}
    | Calculate fight time length	
	/varset DPSTime ${Math.Calc[${Time.SecondsSinceMidnight}-${DPSTimeStart}]}
    | Calculate running damage total
	/varcalc DPSTotal ${DPSTotal}+${YHDamage}
	| Return to avoid divide by 0 errors in calculations
	/if (${DPSTime}<=0 || !${DPSMeterTargetID}) /return
    | echo to test if file is processing events
	/if (${DPSSpam} && ${Select[${YHMob},${Me},${Me.Pet.CleanName}]}==0) /echo Mob/ID: ${YHMob}/${DPSMeterTargetID} -  Type: ${DPSType}/${YHDamage} - Total Damage: ${DPSTotal} - Time: ${DPSTime}s - DPS: ${Math.Calc[${DPSTotal}/${DPSTime}]}
	/doevents YourDamage
	/doevents YourPetDamage
	DEBUGDPSMETER Event_YourDamage Leave 
/return

| -------------------------------------------------------------------------------------
| SUB: Your Pet Hit Event
| -------------------------------------------------------------------------------------      
Sub Event_YourPetDamage(string Line, string YHPType, string YHPMob, int YHPDamage,string YHPSpellDamageType,string YHPSpellName)
	/if (${metertype.Equal[entropy]}) {
		/if (!${maEntropy.Find[swDPSMeter].Value}) /return FALSE
	}
	|  "|${Me}|`s pet #1# #2# for #3# points of damage.#4#"
	/if (${Line.Find[MQ2]}) /return
    | Flush events if dpsmeter off
    /if (!${DPSMeter}) {
		/doevents flush YourPetDamage 
		/return
	}
    DEBUGDPSMETER Event_YourPetDamage Enter 
    /declare DPSPType string local
	/declare DPSTime int local 
    | Assign missing undefined vars from dps messages not in melee to prevent #warning 
	/if (!${Defined[YHPType]}) /declare YHPType string local 
	/if (!${Defined[YHPSpellName]}) /declare YHPSpellName string local 
	/if (!${Defined[YHPSpellDamageType]}) /declare YHPSpellDamageType string local 
	/if ((${DPSMeterTargetID}==0 || !${Spawn[id ${DPSMeterTargetID}].ID}) && ${Target.Type.Equal[npc]}) {
        /varset DPSMeterTargetID ${Target.ID}
		/varset DPSTargetName ${Target.DisplayName}
    }
    | Record start time of fight and assign mob id
	/if (${DPSTimeStart}==0) {
		/varset DPSTimeStart ${Time.SecondsSinceMidnight}

	}
    | Calculate fight time length	
	/varset DPSTime ${Math.Calc[${Time.SecondsSinceMidnight}-${DPSTimeStart}]}
    | Calculate running damage total
	/varcalc DPSPetTotal ${DPSPetTotal}+${YHPDamage}
	| Return to avoid divide by 0 errors in calculations
	/if (${DPSTime}<=0 || !${DPSMeterTargetID}) {
		/return
	}
	| echo to test if file is processing events
	/if (${DPSSpam} && ${Select[${YHPMob},${Me},${Me.Pet.CleanName}]}==0) /echo Mob/ID: ${YHPMob}/${DPSMeterTargetID} -  Type: ${YHPType}/${YHPDamage} - Total Damage: ${DPSTotal} - Time: ${DPSTime}s - DPS: ${Math.Calc[${DPSTotal}/${DPSTime}]}
	/doevents YourDamage
	/doevents YourPetDamage
	DEBUGDPSMETER Event_YourPetDamage Leave 
/return

| -------------------------------------------------------------------------------------
| SUB: Slain By Event
| -------------------------------------------------------------------------------------        
Sub Event_SlainBy(string Message, string SlainMob)
	DEBUGDPSMETER /echo ${SlainMob} ${Spawn[id ${SlainMob}].Type.Equal[corpse]} ${Spawn[id ${DPSMeterTargetID}].Type.Equal[corpse]} 
 	/if (${metertype.Equal[entropy]}) {
		/if (!${maEntropy.Find[swDPSMeter].Value}) /return FALSE
	}
  /varset DPSLastTime ${Math.Calc[${Time.SecondsSinceMidnight}-${DPSTimeStart}]}
	/if (${DPSMeter} && (${Spawn[id ${DPSMeterTargetID}].Type.Equal[corpse]} || !${Spawn[${DPSMeterTargetID}].ID})) {
		/call DPSMeter_Report
		/call DPSMeter_Reset 
	}
/return

| -------------------------------------------------------------------------------------
| SUB: DPSMeter Report
| -------------------------------------------------------------------------------------     
Sub DPSMeter_Report()

	/declare DPSCalc string local
	/declare DPSTCalc string local
	/declare DPSMyDam string local
	/declare DPSPCalc string local
	/declare DPSPTCalc string local
	/declare DPSMyPetDam string local
	/declare DPSMeAndPetDam string local
	/declare DPSMPCalc string local
	/declare DPSMPTCalc string local

	DEBUGDPSMETER CombatReset: Enter 

	/if (${DPSMeterTargetID} && ${DPSMeter}) {
		/if (!${DPSLastTime}) {
			/echo DPS Meter: Was a timing error skipping DPS Meter
		} else {

			| My damage calculation
			/varset DPSCalc ${Math.Calc[${DPSTotal}/${DPSLastTime}]}
            /varset DPSMyDam  ${Math.Calc[${DPSTotal}/${DPSLastTime}]}
            /varset DPSTCalc ${DPSTotal}

			| My pet damage calculation
			/if (${DPSPetTotal}) {
				/varset DPSPCalc ${Math.Calc[${DPSPetTotal}/${DPSLastTime}]}
				/varset DPSMyPetDam ${Math.Calc[${DPSPetTotal}/${DPSLastTime}]}
				/varset DPSPTCalc ${DPSPetTotal}
				| Me + pet damage calculation
				/varset DPSMPCalc ${Math.Calc[(${DPSTotal}+${DPSPetTotal})/${DPSLastTime}]}
				/varset DPSMeAndPetDam ${Math.Calc[(${DPSTotal}+${DPSPetTotal})/${DPSLastTime}]}
				/varset DPSMPTCalc ${Math.Calc[${DPSTotal}+${DPSPetTotal}]}

			}

			/call DPSMeter_Output "${DPSTargetName}" ${DPSLastTime} ${DPSMyDam} ${DPSCalc} ${DPSTCalc} ${DPSMPTCalc} ${DPSMeAndPetDam} ${DPSMyPetDam} ${DPSPTCalc}

			/if (${DPSWriteOn}) {
				/ini "DPSMeter_${Me}" "${DPSTargetName}-${DPSMeterTargetID} - ${Time.Month}/${Time.Day}/${Time.Year}-${Time.Time12}" "DPS-${Me.Class}-${Me.Level}" "Total Damage: ${DPSTCalc} - Fight Time: ${DPSLastTime}s - DPS: ${DPSMyDam}"
				/if (${DPSPetTotal}) {
					/ini "DPSMeter_${Me}" "${DPSTargetName}-${DPSMeterTargetID} - ${Time.Month}/${Time.Day}/${Time.Year}-${Time.Time12}" "DPS-MyPet" "My Pet Total Damage: ${DPSPTCalc} - Pet DPS: ${DPSMyPetDam}"
					/ini "DPSMeter_${Me}" "${DPSTargetName}-${DPSMeterTargetID} - ${Time.Month}/${Time.Day}/${Time.Year}-${Time.Time12}" "DPS-Combined" "Me + Pet Total Damage: ${DPSMPTCalc} - Combined DPS: ${DPSMeAndPetDam}"
				}

			}
		}
	}
/return    

| -------------------------------------------------------------------------------------
| SUB: Comma Me Bro - CommaNation Domination
| ------------------------------------ -------------------------------------------------           
Sub CommaMeBro(string cmbnumber)
	/doevents YourDamage
	/doevents YourPetDamage
    | Strip out any decimal points 
    /varset cmbnumber ${cmbnumber.Arg[1,.]}
    /if (${cmbnumber.Length}<=3 ) {
        /return ${cmbnumber}
    } else /if (${cmbnumber.Length}>3 && ${cmbnumber.Length}<=6) {
        /return  ${cmbnumber.Left[${Math.Calc[${cmbnumber.Length}-3]}]},${cmbnumber.Right[3]}
    } /else /if (${cmbnumber.Length}>6  && ${cmbnumber.Length}<=9) {
        /return ${cmbnumber.Left[${Math.Calc[${cmbnumber.Length}-6]}]},${cmbnumber.Mid[${Math.Calc[${cmbnumber.Length}-5]},3]},${cmbnumber.Right[3]}
    } else /if (${cmbnumber.Length}>9) {
        /return ${cmbnumber.Left[${Math.Calc[${cmbnumber.Length}-9]}]},${cmbnumber.Mid[${Math.Calc[${cmbnumber.Length}-8]},3]},${cmbnumber.Mid[${Math.Calc[${cmbnumber.Length}-5]},3]},${cmbnumber.Right[3]}
    }

/return ERROR

| -------------------------------------------------------------------------------------
| SUB: DPSMeter Reset
| -------------------------------------------------------------------------------------     
Sub DPSMeter_Reset()
	/varset DPSLastTime 0
	/varset DPSTotal 0
	/varset DPSMeterTargetID 0
	/varset DPSPetTotal 0
	/varset DPSCounter 0
	/varset DPSTargetName
    /varset DPSTimeStart 0
	/doevents flush YourDamage 
	/doevents flush YourPetDamage 
	/doevents flush SlainBy 
/return

      