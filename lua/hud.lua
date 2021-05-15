--
-- entropoy.mac
-- hud.lua
-- 
-- hud and things
--


require "common"
local openGUI = true
local shouldDrawHUD = true


local function imguicallback()
  hudInfo()
  
  -- openGUI, shouldDrawHUD = ImGui.Begin('Entropy '..ent['build'], openGUI, ImGuiWindowFlags.NoScrollbar)
  openGUI, shouldDrawHUD = ImGui.Begin('Entropy '..ent['build'], openGUI)
  
  if shouldDrawHUD and ent['build'] == '--' then
    ImGui.Text('Entropy is not running')
    ImGui.End()


  elseif shouldDrawHUD then  
    if ImGui.BeginTabBar('##mytabs') then
      if ImGui.BeginTabItem('Home') then

        ImGui.Columns(3, 'noname', false)
          -- mode
          ImGui.TextColored(0.39, 0.58, 0.92, 1, 'mode')
          ImGui.SameLine()
          ImGui.TextColored(1, 1, 1, 1, ent['mode'])
          -- y, x, z location
          ImGui.TextColored(0.39, 0.58, 0.92, 1, 'yxz')
          ImGui.SameLine()
          ImGui.TextColored(1, 1, 1, 1, math.floor(mq.TLO.Me.Y())..', '..math.floor(mq.TLO.Me.X())..', '..math.floor(mq.TLO.Me.Z()))
          -- last set used
          ImGui.TextColored(0.39, 0.58, 0.92, 1, 'set')
          ImGui.SameLine()
          ImGui.TextColored(1, 1, 1, 1, ent['set'])

        ImGui.NextColumn()
          -- tie   
          ImGui.TextColored(0.39, 0.58, 0.92, 1, mq.TLO.Macro.Variable('maTie').Find('stTieMode').Value())
          ImGui.SameLine()
          ImGui.TextColored(1, 1, 1, 1, mq.TLO.Macro.Variable('maTie').Find('stTieToon').Value())


        ImGui.NextColumn()
          -- env
          ImGui.TextColored(0.39, 0.58, 0.92, 1, 'env')
          ImGui.SameLine()
          ImGui.TextColored(1, 1, 1, 1, mq.TLO.Macro.Variable('maEnv').Find('stEnvRadius').Value())
          -- pull
          local pullactive = ImGui.SmallButton('pull')
          if pullactive then
            mq.cmd.pull('active')
          end
          ImGui.SameLine()
          ImGui.TextColored(1, 1, 1, 1, mq.TLO.Macro.Variable('maPull').Find('stPullRadius').Value())
          ImGui.SameLine()
          if mq.TLO.Macro.Variable('maPull').Find('swPull').Value() == 'TRUE' then
            ImGui.TextColored(0, 1, 0, 1, mq.TLO.Macro.Variable('maPull').Find('stPullMode').Value())
          else 
            ImGui.TextColored(1, 0, 0, 1, mq.TLO.Macro.Variable('maPull').Find('stPullMode').Value())
          end
          ImGui.TextColored(0.39, 0.58, 0.92, 1, 'invis')
          ImGui.SameLine()
          ImGui.TextColored(0, 1, 1, 1, ent['ivu'])
          ImGui.SameLine()
          ImGui.TextColored(0, 1, 1, 1, ent['inv'])
        -- end of columns
        ImGui.Columns()

        -- engage %s, target body, distances
        ImGui.TextColored(0.39, 0.58, 0.92, 1, '[')
        ImGui.SameLine()
        ImGui.TextColored(1, 1, 1, 1, mq.TLO.Macro.Variable('maCC').Find('stPctEngage').Value())
        ImGui.SameLine()
        ImGui.TextColored(0.39, 0.58, 0.92, 1, '][')
        ImGui.SameLine()
        ImGui.TextColored(1, 1, 1, 1, mq.TLO.Macro.Variable('maMinion').Find('stPctPetEngage').Value())
        ImGui.SameLine()
        ImGui.TextColored(0.39, 0.58, 0.92, 1, '][')
        ImGui.SameLine()
        ImGui.TextColored(1, 1, 1, 1, mq.TLO.Macro.Variable('maMinion').Find('stPctSwarmEngage').Value())
        ImGui.SameLine()
        ImGui.TextColored(0.39, 0.58, 0.92, 1, '][')
        ImGui.SameLine()
        ImGui.TextColored(1, 1, 1, 1, ent['body'])
        ImGui.SameLine()
        ImGui.TextColored(0.39, 0.58, 0.92, 1,']')
        ImGui.SameLine()
        ImGui.TextColored(1, 1, 1, 1, math.floor(target['distance']))
        ImGui.SameLine()
        ImGui.TextColored(0.39, 0.58, 0.92, 1, '-')
        ImGui.SameLine()
        ImGui.TextColored(1, 1, 1, 1, math.floor(target['maxrangeto']))

        -- target information
        if mq.TLO.Target.ID() ~= nil then   
          ImGui.TextColored(1, 1, 1, 1, '['..target['shortname'].." "..target['level']..'] '..target['displayname']..' <'..target['guild']..'>')
        else
          ImGui.TextDisabled(' ')
        end
        
        -- ImGui.TextDisabled(mq.TLO.Macro.Variable('HUDOutput'))

-- if ImGui.BeginChild("logmessages", 500, 40) then
--   ImGui.Text(ent['hudoutput'])
--   ImGui.EndChild()
-- end



-- home tab
        ImGui.Columns(5, 'noname', false)

        -- column 1

          if mq.TLO.Macro.Variable('maEnv').Find('swAuto').Value() == 'TRUE' then
            local autoon = ImGui.Button('Auto: on', 60, 48)
            if autoon then
              mq.cmd.env('auto')
            end
          else 
            local autooff = ImGui.Button('Auto: off', 60, 48)
            if autooff then
              mq.cmd.env('auto')
            end            
          end

        -- column 2
          ImGui.NextColumn()
          local on = ImGui.Button('On', 60, 22)
          if on then
            mq.cmd.on()
          end
          
          local off = ImGui.Button('Off', 60, 22)
          if off then
            mq.cmd.off()
          end


        -- column 3
          ImGui.NextColumn()

          local incharge = ImGui.Button('Control', 60, 22)
          if incharge then
            mq.cmd.env('incharge')
          end

          local tie = ImGui.Button('Tie', 60, 22)
          if tie then
            mq.cmd.dga('/tie')
          end


        -- column 4
          ImGui.NextColumn()



        -- column 5
          ImGui.NextColumn()

          local gather = ImGui.Button('Gather', 60, 22)
          if gather then
            mq.cmd.gather()
          end

          local here = ImGui.Button('Here', 60, 22)
          if here then
            mq.cmd.here()
          end

        ImGui.Columns()
        ImGui.EndTabItem()
      end



-- buff tab 
      if ImGui.BeginTabItem('Buff') then

      ImGui.Columns(4, 'noname', false)
        -- auras

        local doaura, aurapressed = ImGui.Checkbox("##doaura", mq.TLO.Macro.Variable('maBuff').Find('swBuffAura').Value() == 'TRUE')
        if aurapressed then
          -- mq.cmd.invoke('${maBuff.Add[swBuffAura,'..(doaura and 'TRUE' or 'FALSE')..']}')
          mq.cmd.luaedit('swBuffAura', doaura and 'TRUE' or 'FALSE')
        end
        ImGui.SameLine()
        local buffaura = ImGui.Button('aura', 60, 22)
        if buffaura then
          mq.cmd.cb('a')
        end         
        -- mq.TLO.Macro.Variable('maBuff').Find('swBuffAura').Value()
          
        -- self
        local doself, selfpressed = ImGui.Checkbox("##doself", mq.TLO.Macro.Variable('maBuff').Find('swBuffSelf').Value() == 'TRUE')
        if selfpressed then
          mq.cmd.luaedit('swBuffSelf', doself and 'TRUE' or 'FALSE')
        end
        ImGui.SameLine()
        local buffself = ImGui.Button('self', 60, 22)
        if buffself then
          mq.cmd.cb('s')
        end
        
        -- crew
        local docrew, crewpressed = ImGui.Checkbox("##docrew", mq.TLO.Macro.Variable('maBuff').Find('swBuffCrew').Value() == 'TRUE')
        if crewpressed then
          mq.cmd.luaedit('swBuffCrew', docrew and 'TRUE' or 'FALSE')
        end
        ImGui.SameLine()
        local buffcrew = ImGui.Button('crew', 60, 22)
        if buffcrew then
          mq.cmd.cb('c')
        end
                
        -- temp
        local dotemp, temppressed = ImGui.Checkbox("##dotemp", mq.TLO.Macro.Variable('maBuff').Find('swBuffTemp').Value() == 'TRUE')
        if temppressed then
          mq.cmd.luaedit('swBuffTemp', dotemp and 'TRUE' or 'FALSE')
        end
        ImGui.SameLine()
        local bufftemp = ImGui.Button('temp', 60, 22)
        if bufftemp then
          mq.cmd.cb('t')
        end
        
        ImGui.NextColumn()
                
        -- unity
        local dounity, unitypressed = ImGui.Checkbox("##dounity", mq.TLO.Macro.Variable('maBuff').Find('swBuffUnity').Value() == 'TRUE')
        if unitypressed then
          mq.cmd.luaedit('swBuffUnity', dounity and 'TRUE' or 'FALSE')
        end
        ImGui.SameLine()
        local buffunity = ImGui.Button('unity', 60, 22)
        if buffunity then
          mq.cmd.cb('u')
        end
        
        -- minion
        local dominion, minionpressed = ImGui.Checkbox("##dominion", mq.TLO.Macro.Variable('maMinion').Find('swPetBuff').Value() == 'TRUE')
        if minionpressed then
          mq.cmd.luaedit('swPetBuff', dominion and 'TRUE' or 'FALSE')
        end
        ImGui.SameLine()
        local buffminion = ImGui.Button('minion', 60, 22)
        if buffminion then
          mq.cmd.cb('m')
        end
        
        -- raid
        local doraid, raidpressed = ImGui.Checkbox("##doraid", mq.TLO.Macro.Variable('maBuff').Find('swBuffRaid').Value() == 'TRUE')
        if raidpressed then
          mq.cmd.luaedit('swBuffRaid', doraid and 'TRUE' or 'FALSE')
        end   
        ImGui.SameLine()
        local buffraid = ImGui.Button('raid', 60, 22)
        if buffraid then
          mq.cmd.cb('r')
        end
        
        -- group
        local dogroup, grouppressed = ImGui.Checkbox("##dogroup", mq.TLO.Macro.Variable('maBuff').Find('swBuffGroup').Value() == 'TRUE')
        if grouppressed then
          mq.cmd.luaedit('swBuffGroup', dogroup and 'TRUE' or 'FALSE')
        end   
        ImGui.SameLine()
        local buffgroup = ImGui.Button('group', 60, 22)
        if buffgroup then
          mq.cmd.cb('g')
        end

        ImGui.NextColumn()

        -- shrink
        local doshrink, shrinkpressed = ImGui.Checkbox("shrink", mq.TLO.Macro.Variable('maBuff').Find('swBuffShrink').Value() == 'TRUE')
        if shrinkpressed then
          mq.cmd.luaedit('swBuffShrink', doshrink and 'TRUE' or 'FALSE')
        end

        ImGui.NextColumn()

        -- master
        local domaster, masterpressed = ImGui.Checkbox("master", mq.TLO.Macro.Variable('maBuff').Find('swBuffMaster').Value() == 'TRUE')
        if masterpressed then
          mq.cmd.invoke('${maBuff.Add[swBuffMaster,'..(domaster and 'TRUE' or 'FALSE')..']}')
        end        
               
        -- now
        local donow, nowpressed = ImGui.Checkbox("now", mq.TLO.Macro.Variable('maBuff').Find('swBuffNow').Value() == 'TRUE')
        if nowpressed then
          mq.cmd.luaedit('swBuffNow', donow and 'TRUE' or 'FALSE')
        end        

        -- ammo
        local doammo, ammopressed = ImGui.Checkbox("ammo", mq.TLO.Macro.Variable('maBuff').Find('swBuffAmmo').Value() == 'TRUE')
        if ammopressed then
          mq.cmd.luaedit('swBuffAmmo', doammo and 'TRUE' or 'FALSE')
        end

        -- temp
        local dobeg, begpressed = ImGui.Checkbox("beg", mq.TLO.Macro.Variable('maBuff').Find('swBuffBeg').Value() == 'TRUE')
        if begpressed then
          mq.cmd.luaedit('swBuffBeg', dobeg and 'TRUE' or 'FALSE')
        end
         
        ImGui.Columns()    
        ImGui.EndTabItem()
      end



-- heal tab
      if ImGui.BeginTabItem('Heal') then
        ImGui.Columns(3, 'noname', false)

        -- self
        local doself, selfpressed = ImGui.Checkbox("self", mq.TLO.Macro.Variable('maHeal').Find('swHealSelf').Value() == 'TRUE')
        if selfpressed then
          mq.cmd.luaedit('swHealSelf', doself and 'TRUE' or 'FALSE')
        end   

        -- dannet
        local dodannet, dannetpressed = ImGui.Checkbox("dannet", mq.TLO.Macro.Variable('maHeal').Find('swHealDanNet').Value() == 'TRUE')
        if dannetpressed then
          mq.cmd.luaedit('swHealDanNet', dodannet and 'TRUE' or 'FALSE')
        end  
        
        -- group
        local dogroup, grouppressed = ImGui.Checkbox("group", mq.TLO.Macro.Variable('maHeal').Find('swHealGroup').Value() == 'TRUE')
        if grouppressed then
          mq.cmd.luaedit('swHealGroup', dogroup and 'TRUE' or 'FALSE')
        end  
        
        -- xtarget        
        local doxt, xtpressed = ImGui.Checkbox("##xtarget", mq.TLO.Macro.Variable('maHeal').Find('swHealXTarget').Value() == 'TRUE')
        if xtpressed then
          mq.cmd.luaedit('swHealXTarget', doxt and 'TRUE' or 'FALSE')
        end
        ImGui.SameLine()
        local healxt = ImGui.Button('xt', 60, 22)
        if healxt then
          mq.cmd.heal('xt build')
        end

        -- pet        
        local dopet, petpressed = ImGui.Checkbox("pet", mq.TLO.Macro.Variable('maHeal').Find('swHealPet').Value() == 'TRUE')
        if petpressed then
          mq.cmd.luaedit('swHealPet', dopet and 'TRUE' or 'FALSE')
        end           

        -- rampage       
        local rampage, selectedramp = ImGui.InputTextWithHint('ramp##textentry', mq.TLO.Macro.Variable('maHeal').Find('stRampageTank').Value(), '', ImGuiInputTextFlags.EnterReturnsTrue)
        if selectedramp then
          mq.cmd.luaedit('stRampageTank', rampage)
        end
       
        ImGui.NextColumn()
        
        -- weight
        local doweight, weightpressed = ImGui.Checkbox("weight", mq.TLO.Macro.Variable('maHeal').Find('swHealWeighted').Value() == 'TRUE')
        if weightpressed then
          mq.cmd.luaedit('swHealWeighted', doweight and 'TRUE' or 'FALSE')
        end        
        
        -- break
        local dobreak, breakpressed = ImGui.Checkbox("break", mq.TLO.Macro.Variable('maHeal').Find('swBreakHealPCT').Value() == 'TRUE')
        if breakpressed then
          mq.cmd.luaedit('swBreakHealPCT', dobreak and 'TRUE' or 'FALSE')
        end         
        
        
        ImGui.NextColumn()
 
        -- xt healing and points
        -- war
        local warhp = tonumber(mq.TLO.Macro.Variable('maHeal').Find('stHealPointWAR').Value())
        warhp, used = ImGui.DragInt("WAR", warhp, 1, 0, 99);
        if used then
          mq.cmd.luaedit('stHealPointWAR', warhp)  
        end
        -- pal
        local palhp = tonumber(mq.TLO.Macro.Variable('maHeal').Find('stHealPointPAL').Value())
        palhp, used = ImGui.DragInt("PAL", palhp, 1, 0, 99)
        if used then
          mq.cmd.luaedit('stHealPointPAL', palhp)  
        end
        -- shd
        local shdhp = tonumber(mq.TLO.Macro.Variable('maHeal').Find('stHealPointSHD').Value())
        shdhp, used = ImGui.DragInt("SHD", shdhp, 1, 0, 99)
        if used then
          mq.cmd.luaedit('stHealPointSHD', shdhp)  
        end
        -- CLR
        local clrhp = tonumber(mq.TLO.Macro.Variable('maHeal').Find('stHealPointCLR').Value())
        clrhp, used = ImGui.DragInt("CLR", clrhp, 1, 0, 99)
        if used then
          mq.cmd.luaedit('stHealPointCLR', clrhp)  
        end
        -- SHM
        local shmhp = tonumber(mq.TLO.Macro.Variable('maHeal').Find('stHealPointSHM').Value())
        shmhp, used = ImGui.DragInt("SHM", shmhp, 1, 0, 99)
        if used then
          mq.cmd.luaedit('stHealPointSHM', shmhp)  
        end
        -- DRU
        local druhp = tonumber(mq.TLO.Macro.Variable('maHeal').Find('stHealPointDRU').Value())
        druhp, used = ImGui.DragInt("DRU", druhp, 1, 0, 99)
        if used then
          mq.cmd.luaedit('stHealPointDRU', druhp)  
        end
        -- MNK
        local mnkhp = tonumber(mq.TLO.Macro.Variable('maHeal').Find('stHealPointMNK').Value())
        mnkhp, used = ImGui.DragInt("MNK", mnkhp, 1, 0, 99)
        if used then
          mq.cmd.luaedit('stHealPointMNK', mnkhp)  
        end
        -- BER
        local BERhp = tonumber(mq.TLO.Macro.Variable('maHeal').Find('stHealPointBER').Value())
        BERhp, used = ImGui.DragInt("BER", BERhp, 1, 0, 99)
        if used then
          mq.cmd.luaedit('stHealPointBER', BERhp)  
        end
        -- BRD
        local BRDhp = tonumber(mq.TLO.Macro.Variable('maHeal').Find('stHealPointBRD').Value())
        BRDhp, used = ImGui.DragInt("BRD", BRDhp, 1, 0, 99)
        if used then
          mq.cmd.luaedit('stHealPointBRD', BRDhp)  
        end
        -- ROG
        local ROGhp = tonumber(mq.TLO.Macro.Variable('maHeal').Find('stHealPointROG').Value())
        ROGhp, used = ImGui.DragInt("ROG", ROGhp, 1, 0, 99)
        if used then
          mq.cmd.luaedit('stHealPointROG', ROGhp)  
        end
        -- BST
        local BSThp = tonumber(mq.TLO.Macro.Variable('maHeal').Find('stHealPointBST').Value())
        BSThp, used = ImGui.DragInt("BST", BSThp, 1, 0, 99)
        if used then
          mq.cmd.luaedit('stHealPointBST', BSThp)  
        end
        -- RNG
        local RNGhp = tonumber(mq.TLO.Macro.Variable('maHeal').Find('stHealPointRNG').Value())
        RNGhp, used = ImGui.DragInt("RNG", RNGhp, 1, 0, 99)
        if used then
          mq.cmd.luaedit('stHealPointRNG', RNGhp)  
        end
        -- WIZ
        local WIZhp = tonumber(mq.TLO.Macro.Variable('maHeal').Find('stHealPointWIZ').Value())
        WIZhp, used = ImGui.DragInt("WIZ", WIZhp, 1, 0, 99)
        if used then
          mq.cmd.luaedit('stHealPointRNG', WIZhp)  
        end
        -- MAG
        local MAGhp = tonumber(mq.TLO.Macro.Variable('maHeal').Find('stHealPointMAG').Value())
        MAGhp, used = ImGui.DragInt("MAG", MAGhp, 1, 0, 99)
        if used then
          mq.cmd.luaedit('stHealPointMAG', MAGhp)  
        end
        -- NEC
        local NEChp = tonumber(mq.TLO.Macro.Variable('maHeal').Find('stHealPointNEC').Value())
        NEChp, used = ImGui.DragInt("NEC", NEChp, 1, 0, 99)
        if used then
          mq.cmd.luaedit('stHealPointNEC', NEChp)  
        end
        -- ENC
        local ENChp = tonumber(mq.TLO.Macro.Variable('maHeal').Find('stHealPointENC').Value())
        ENChp, used = ImGui.DragInt("ENC", ENChp, 1, 0, 99)
        if used then
          mq.cmd.luaedit('stHealPointENC', ENChp)  
        end

        
        ImGui.Columns()  

        ImGui.EndTabItem()
      end



-- combat tab
      if ImGui.BeginTabItem('Combat') then
        ImGui.Columns(3, 'noname', false)

        -- modes
        -- dot
        local dotnow, dotpressed = ImGui.Checkbox("dot", mq.TLO.Macro.Variable('maCC').Find('swCombatDoT').Value() == 'TRUE')
        if dotpressed then
          mq.cmd.luaedit('swCombatDoT', dotnow and 'TRUE' or 'FALSE')
        end   
        -- heal
        local healnow, healpressed = ImGui.Checkbox("heal", mq.TLO.Macro.Variable('maCC').Find('swCombatHeal').Value() == 'TRUE')
        if healpressed then
          mq.cmd.luaedit('swCombatHeal', healnow and 'TRUE' or 'FALSE')
        end   
        -- melee
        local meleenow, meleepressed = ImGui.Checkbox("melee", mq.TLO.Macro.Variable('maCC').Find('swCombatMelee').Value() == 'TRUE')
        if meleepressed then
          mq.cmd.luaedit('swCombatMelee', meleenow and 'TRUE' or 'FALSE')
        end   
        -- range
        local rangenow, rangepressed = ImGui.Checkbox("range", mq.TLO.Macro.Variable('maCC').Find('swCombatRange').Value() == 'TRUE')
        if rangepressed then
          mq.cmd.luaedit('swCombatRange', rangenow and 'TRUE' or 'FALSE')
        end   
        -- nuke
        local nukenow, nukepressed = ImGui.Checkbox("nuke", mq.TLO.Macro.Variable('maCC').Find('swCombatNuke').Value() == 'TRUE')
        if nukepressed then
          mq.cmd.luaedit('swCombatNuke', nukenow and 'TRUE' or 'FALSE')
        end   
        -- cc
        local ccnow, ccpressed = ImGui.Checkbox("cc", mq.TLO.Macro.Variable('maCC').Find('swCombatControl').Value() == 'TRUE')
        if ccpressed then
          if mq.TLO.Me.ClassShortName() == 'ENC' or mq.TLO.Me.ClassShortName() == 'BRD' or mq.TLO.Me.ClassShortName() == 'NEC' or mq.TLO.Me.ClassShortName() == 'MAG' then
            mq.cmd.luaedit('swCombatControl', ccnow and 'TRUE' or 'FALSE')
          end
        end   

        ImGui.NextColumn()
        
        -- smart assist        
        local current_smartass = mq.TLO.Macro.Variable('maCC').Find('stAssistMode').Value()
        if ImGui.BeginCombo("smart", current_smartass) then
          for _, v in ipairs({ 'off', 'g', '1', '2', '3' }) do
            local selectedshade = v == current_smartass
            if ImGui.Selectable(v, selectedshade) and not selectedshade then
              mq.cmd.cc('ass smart', v)
            end
          end

          ImGui.EndCombo()
        end
        
        -- manual assist
        local manualnow, manualpressed = ImGui.Checkbox("manual", mq.TLO.Macro.Variable('maCC').Find('swManualAssist').Value() == 'TRUE')
        if manualpressed then
          mq.cmd.luaedit('swManualAssist', manualnow and 'TRUE' or 'FALSE')
        end 


        ImGui.NextColumn()
        
        -- engage
        local engage, selectedeng = ImGui.InputTextWithHint('engage##textentry', mq.TLO.Macro.Variable('maCC').Find('stPctEngage').Value(), '', ImGuiInputTextFlags.EnterReturnsTrue)
        if selectedeng then
          mq.cmd.luaedit('stPctEngage', engage)
        end
        
        -- zrad
        local zrad, selectedzrad = ImGui.InputTextWithHint('zrad##textentry', mq.TLO.Macro.Variable('maCC').Find('stZRadius').Value(), '', ImGuiInputTextFlags.EnterReturnsTrue)
        if selectedzrad then
          mq.cmd.luaedit('stZRadius', zrad)
        end        
        
       
       
        ImGui.Columns()
        ImGui.EndTabItem()
        
        
      end


-- Pull tab
      if ImGui.BeginTabItem('Pull') then
        ImGui.Columns(3, 'noname', false)

        -- pull active
        local pullactive, pullactivepressed = ImGui.Checkbox("pull", mq.TLO.Macro.Variable('maPull').Find('swPull').Value() == 'TRUE')
        if pullactivepressed then
          mq.cmd.luaedit('swPull', pullactive and 'TRUE' or 'FALSE')
        end  
        -- pull mode
        local pullmode = mq.TLO.Macro.Variable('maPull').Find('stPullMode').Value()
        if ImGui.BeginCombo("mode", pullmode) then
          for _, v in ipairs({ 'base', 'pet', 'int', 'nav', 'multi', 'watch' }) do
            local selectedpm = v == pullmode
            if ImGui.Selectable(v, selectedpm) and not selectedpm then
              mq.cmd.luaedit('stPullMode', v)
            end
          end

          ImGui.EndCombo()
        end

        -- pull with
        local pullwith = mq.TLO.Macro.Variable('maPull').Find('stPullWith').Value()
        if ImGui.BeginCombo("with", pullwith) then
          for _, v in ipairs({ 'melee', 'range', 'other' }) do
            local selectedpw = v == pullwith
            if ImGui.Selectable(v, selectedpw) and not selectedpw then
              mq.cmd.luaedit('stPullWith', v)
            end
          end

          ImGui.EndCombo()
        end

        -- pathlogic
        local pathlogic, pathlogicpressed = ImGui.Checkbox("pathlogic", mq.TLO.Macro.Variable('maPull').Find('swNavPathLogic').Value() == 'TRUE')
        if pathlogicpressed then
          mq.cmd.luaedit('swNavPathLogic', pathlogic and 'TRUE' or 'FALSE')
        end 

        -- sethome
        local sethome, sethomepressed = ImGui.Checkbox("sethome", mq.TLO.Macro.Variable('maPull').Find('swPullSetHome').Value() == 'TRUE')
        if sethomepressed then
          mq.cmd.luaedit('swPullSetHome', sethome and 'TRUE' or 'FALSE')
        end 


        ImGui.NextColumn()

        -- pull radius
        local pullrad, selectedpr = ImGui.InputTextWithHint('rad##textentry', mq.TLO.Macro.Variable('maPull').Find('stPullRadius').Value(), '', ImGuiInputTextFlags.EnterReturnsTrue)
        if selectedpr then
          mq.cmd.luaedit('stPullRadius', pullrad)
        end    
       -- pullzrad
        local pullzrad, selectedpzr = ImGui.InputTextWithHint('zrad##textentry', mq.TLO.Macro.Variable('maPull').Find('stPullZRadius').Value(), '', ImGuiInputTextFlags.EnterReturnsTrue)
        if selectedpzr then
          mq.cmd.luaedit('stPullZRadius', pullzrad)
        end           
        -- navstop
        local navstop, selectedns = ImGui.InputTextWithHint('stop##textentry', mq.TLO.Macro.Variable('maPull').Find('stPullNavStopDistance').Value(), '', ImGuiInputTextFlags.EnterReturnsTrue)
        if selectedns then
          mq.cmd.luaedit('stPullNavStopDistance', navstop)
        end           
       -- chainpull
        local chainpull, selectedcp = ImGui.InputTextWithHint('chain##textentry', mq.TLO.Macro.Variable('maPull').Find('stCountChainPull').Value(), '', ImGuiInputTextFlags.EnterReturnsTrue)
        if selectedcp then
          mq.cmd.luaedit('stCountChainPull', chainpull)
        end 

       -- navvariance
        local navvariance, selectednv = ImGui.InputTextWithHint('var##textentry', mq.TLO.Macro.Variable('maPull').Find('stPullNavVariance').Value(), '', ImGuiInputTextFlags.EnterReturnsTrue)
        if selectednv then
          mq.cmd.luaedit('stPullNavVariance', navvariance)
        end 




        ImGui.Columns()

        ImGui.EndTabItem()
                
      end

-- rez tab
      if ImGui.BeginTabItem('Rez') then


        ImGui.Columns(2, 'noname', false)

        local rezrad = tonumber(mq.TLO.Macro.Variable('maRez').Find('stMaxRezRange').Value())
        rezrad, used = ImGui.DragInt("rad", rezrad, 1, 0, 200);
        if used then
          mq.cmd.luaedit('stMaxRezRange', rezrad)  
        end

        local current_rezpct = mq.TLO.Macro.Variable('maRez').Find('stPctMinRez').Value()
        if ImGui.BeginCombo("pct", current_rezpct) then
          for _, v in ipairs({ '10', '20', '35', '50', '60', '75', '90', '93', '96' }) do
            local selectedrezpct = v == current_rezpct
            if ImGui.Selectable(v, selectedrezpct) and not selectedrezpct then
              mq.cmd.rez('pct', v)
            end
          end

          ImGui.EndCombo()
        end

        -- in combat rez
        local doic, icpressed = ImGui.Checkbox("IC", mq.TLO.Macro.Variable('maRez').Find('swRezIC').Value() == 'TRUE')
        if icpressed then
          mq.cmd.luaedit('swRezIC', doic and 'TRUE' or 'FALSE')
        end
        -- out of combat rez
        local doooc, oocpressed = ImGui.Checkbox("OOC", mq.TLO.Macro.Variable('maRez').Find('swRezOOC').Value() == 'TRUE')
        if oocpressed then
          mq.cmd.luaedit('swRezOOC', doooc and 'TRUE' or 'FALSE')
        end

        ImGui.NextColumn()

        -- dannet rezing
        local dodannet, dannetpressed = ImGui.Checkbox("dannet", mq.TLO.Macro.Variable('maRez').Find('swRezDanNet').Value() == 'TRUE')
        if dannetpressed then
          mq.cmd.luaedit('swRezDanNet', dodannet and 'TRUE' or 'FALSE')
        end
        -- rez everyone in range
        local doeveryone, everyonepressed = ImGui.Checkbox("everyone", mq.TLO.Macro.Variable('maRez').Find('swRezEveryone').Value() == 'TRUE')
        if everyonepressed then
          mq.cmd.luaedit('swRezEveryone', doeveryone and 'TRUE' or 'FALSE')
        end        
        -- use token
        local dotoken, tokenpressed = ImGui.Checkbox("use token", mq.TLO.Macro.Variable('maRez').Find('swRezToken').Value() == 'TRUE')
        if tokenpressed then
          mq.cmd.luaedit('swRezToken', dotokenl and 'TRUE' or 'FALSE')
        end  
        -- take rezes
        local dotake, takepressed = ImGui.Checkbox("take", mq.TLO.Macro.Variable('maRez').Find('swRezTake').Value() == 'TRUE')
        if takepressed then
          mq.cmd.luaedit('swRezTake', dotake and 'TRUE' or 'FALSE')
        end  
        -- take call to corpse
        local dotakecall, takecallpressed = ImGui.Checkbox("take call", mq.TLO.Macro.Variable('maRez').Find('swRezTakeCall').Value() == 'TRUE')
        if takecallpressed then
          mq.cmd.luaedit('swRezTakeCall', dotakecall and 'TRUE' or 'FALSE')
        end  

        
        ImGui.Columns()      
        ImGui.EndTabItem()
      end      


-- tie tab
      if ImGui.BeginTabItem('Tie') then

        ImGui.Columns(2, 'noname', false)
        -- tietoon       
        local tietoon, selectedtoon = ImGui.InputTextWithHint('toon##textentry', mq.TLO.Macro.Variable('maTie').Find('stTieToon').Value(), '', ImGuiInputTextFlags.EnterReturnsTrue)
         if selectedtoon then
         mq.cmd.luaedit('stTieToon', tietoon)
        end

        -- tie variance
        local tievar, selectedvar = ImGui.InputTextWithHint('variance', mq.TLO.Macro.Variable('maTie').Find('stTieVariance').Value(), '', ImGuiInputTextFlags.EnterReturnsTrue)
        if selectedvar then
          mq.cmd.luaedit('stTieVariance', tievar)
        end


        local tiemode = mq.TLO.Macro.Variable('maTie').Find('stTieMode').Value()
        if ImGui.BeginCombo("mode", tiemode) then
          for _, v in ipairs({ 'nav', 'stick' }) do
            local selectedtiemode = v == tiemode
            if ImGui.Selectable(v, selectedtiemode) and not selectedtiemode then
              mq.cmd.tc('mode', v)
            end
          end

          ImGui.EndCombo()
        end


        ImGui.NextColumn()

        -- tie in combat
        local incombat, incombatpressed = ImGui.Checkbox("incombat", mq.TLO.Macro.Variable('maTie').Find('swTieCombat').Value() == 'TRUE')
        if incombatpressed then
          mq.cmd.luaedit('swTieCombat', incombat and 'TRUE' or 'FALSE')
        end          

        -- breakworld to tie
        local breakworld, breakworldpressed = ImGui.Checkbox("break to", mq.TLO.Macro.Variable('maTie').Find('swTieBreak').Value() == 'TRUE')
        if breakworldpressed then
          mq.cmd.luaedit('swTieCombat', breakworld and 'TRUE' or 'FALSE')
        end  


        ImGui.Columns()

        ImGui.EndTabItem()
      end

      ImGui.EndTabBar()
    end
    ImGui.End()
  end
end

mq.imgui.init('mywindow', imguicallback)

while openGUI do 
  mq.delay(1) 
end







 



