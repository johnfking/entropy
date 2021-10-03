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
  
  if shouldDrawHUD and (ent['build'] == '--' or mq.TLO.EverQuest.GameState() ~= 'INGAME') then
    ImGui.Text('Entropy is not running')

  elseif shouldDrawHUD then  
    if ImGui.BeginTabBar('##mytabs') then


-- home tab
      if ImGui.BeginTabItem('Home') then

        ImGui.Columns(3, 'noname', false)
          -- mode
          ImGui.TextColored(0.39, 0.58, 0.92, 1, 'mode')
          ImGui.SameLine()
          ImGui.TextColored(1, 1, 1, 1, ent['mode'])
          -- y, x, z location
          ImGui.TextColored(0.39, 0.58, 0.92, 1, 'yxz')
          ImGui.SameLine()
          if mq.TLO.Me.Y() then
            ImGui.TextColored(1, 1, 1, 1, math.floor(mq.TLO.Me.Y())..', '..math.floor(mq.TLO.Me.X())..', '..math.floor(mq.TLO.Me.Z()))
          end
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

        local _switch, aurapressed = ImGui.Checkbox("##doaura", mq.TLO.Macro.Variable('maBuff').Find('swBuffAura').Value() == 'TRUE')
        if aurapressed then
          -- mq.cmd.invoke('${maBuff.Add[swBuffAura,'..(doaura and 'TRUE' or 'FALSE')..']}')
          mq.cmd.luaedit('swBuffAura', _switch and 'TRUE' or 'FALSE')
        end
        ImGui.SameLine()
        local buffaura = ImGui.Button('aura', 60, 22)
        if buffaura then
          mq.cmd.cb('a')
        end         
        -- mq.TLO.Macro.Variable('maBuff').Find('swBuffAura').Value()
          
        -- self
        local _switch, selfpressed = ImGui.Checkbox("##doself", mq.TLO.Macro.Variable('maBuff').Find('swBuffSelf').Value() == 'TRUE')
        if selfpressed then
          mq.cmd.luaedit('swBuffSelf', _switch and 'TRUE' or 'FALSE')
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
        local _switch, unitypressed = ImGui.Checkbox("##dounity", mq.TLO.Macro.Variable('maBuff').Find('swBuffUnity').Value() == 'TRUE')
        if unitypressed then
          mq.cmd.luaedit('swBuffUnity', _switch and 'TRUE' or 'FALSE')
        end
        ImGui.SameLine()
        local buffunity = ImGui.Button('unity', 60, 22)
        if buffunity then
          mq.cmd.cb('u')
        end
        
        -- minion
        local _switch, minionpressed = ImGui.Checkbox("##dominion", mq.TLO.Macro.Variable('maMinion').Find('swPetBuff').Value() == 'TRUE')
        if minionpressed then
          mq.cmd.luaedit('swPetBuff', _switch and 'TRUE' or 'FALSE')
        end
        ImGui.SameLine()
        local buffminion = ImGui.Button('minion', 60, 22)
        if buffminion then
          mq.cmd.cb('m')
        end
        
        -- raid
        local _switch, raidpressed = ImGui.Checkbox("##doraid", mq.TLO.Macro.Variable('maBuff').Find('swBuffRaid').Value() == 'TRUE')
        if raidpressed then
          mq.cmd.luaedit('swBuffRaid', _switch and 'TRUE' or 'FALSE')
        end   
        ImGui.SameLine()
        local buffraid = ImGui.Button('raid', 60, 22)
        if buffraid then
          mq.cmd.cb('r')
        end
        
        -- group
        local _switch, grouppressed = ImGui.Checkbox("##dogroup", mq.TLO.Macro.Variable('maBuff').Find('swBuffGroup').Value() == 'TRUE')
        if grouppressed then
          mq.cmd.luaedit('swBuffGroup', _switch and 'TRUE' or 'FALSE')
        end   
        ImGui.SameLine()
        local buffgroup = ImGui.Button('group', 60, 22)
        if buffgroup then
          mq.cmd.cb('g')
        end

        ImGui.NextColumn()

        -- shrink
        local _switch, shrinkpressed = ImGui.Checkbox("shrink", mq.TLO.Macro.Variable('maBuff').Find('swBuffShrink').Value() == 'TRUE')
        if shrinkpressed then
          mq.cmd.luaedit('swBuffShrink', _switch and 'TRUE' or 'FALSE')
        end

        ImGui.NextColumn()

        -- master
        local _switch, masterpressed = ImGui.Checkbox("master", mq.TLO.Macro.Variable('maBuff').Find('swBuffMaster').Value() == 'TRUE')
        if masterpressed then
          mq.cmd.invoke('${maBuff.Add[swBuffMaster,'..(_switch and 'TRUE' or 'FALSE')..']}')
        end        
               
        -- now
        local _switch, nowpressed = ImGui.Checkbox("now", mq.TLO.Macro.Variable('maBuff').Find('swBuffNow').Value() == 'TRUE')
        if nowpressed then
          mq.cmd.luaedit('swBuffNow', _switch and 'TRUE' or 'FALSE')
        end        

        -- ammo
        local _switch, ammopressed = ImGui.Checkbox("ammo", mq.TLO.Macro.Variable('maBuff').Find('swBuffAmmo').Value() == 'TRUE')
        if ammopressed then
          mq.cmd.luaedit('swBuffAmmo', _switch and 'TRUE' or 'FALSE')
        end

        -- temp
        local _switch, begpressed = ImGui.Checkbox("beg", mq.TLO.Macro.Variable('maBuff').Find('swBuffBeg').Value() == 'TRUE')
        if begpressed then
          mq.cmd.luaedit('swBuffBeg', _switch and 'TRUE' or 'FALSE')
        end
         
        ImGui.Columns()    
        ImGui.EndTabItem()
      end


-- heal tab
      if ImGui.BeginTabItem('Heal') then
        ImGui.Columns(3, 'noname', false)

        -- self
        local _switch, selfpressed = ImGui.Checkbox("self", mq.TLO.Macro.Variable('maHeal').Find('swHealSelf').Value() == 'TRUE')
        if selfpressed then
          mq.cmd.luaedit('swHealSelf', _switch and 'TRUE' or 'FALSE')
        end   

        -- dannet
        local _switch, dannetpressed = ImGui.Checkbox("dannet", mq.TLO.Macro.Variable('maHeal').Find('swHealDanNet').Value() == 'TRUE')
        if dannetpressed then
          mq.cmd.luaedit('swHealDanNet', _switch and 'TRUE' or 'FALSE')
        end  
        
        -- group
        local _switch, grouppressed = ImGui.Checkbox("group", mq.TLO.Macro.Variable('maHeal').Find('swHealGroup').Value() == 'TRUE')
        if grouppressed then
          mq.cmd.luaedit('swHealGroup', _switch and 'TRUE' or 'FALSE')
        end  
        
        -- xtarget        
        local _switch, xtpressed = ImGui.Checkbox("##xtarget", mq.TLO.Macro.Variable('maHeal').Find('swHealXTarget').Value() == 'TRUE')
        if xtpressed then
          mq.cmd.luaedit('swHealXTarget', _switch and 'TRUE' or 'FALSE')
        end
        ImGui.SameLine()
        local healxt = ImGui.Button('xt', 60, 22)
        if healxt then
          mq.cmd.heal('xt build')
        end

        -- pet        
        local _switch, petpressed = ImGui.Checkbox("pet", mq.TLO.Macro.Variable('maHeal').Find('swHealPet').Value() == 'TRUE')
        if petpressed then
          mq.cmd.luaedit('swHealPet', _switch and 'TRUE' or 'FALSE')
        end           

        ImGui.NextColumn()
        
        -- weight
        local _switch, weightpressed = ImGui.Checkbox("weight", mq.TLO.Macro.Variable('maHeal').Find('swHealWeighted').Value() == 'TRUE')
        if weightpressed then
          mq.cmd.luaedit('swHealWeighted', _switch and 'TRUE' or 'FALSE')
        end        
        
        -- break
        local _switch, breakpressed = ImGui.Checkbox("break", mq.TLO.Macro.Variable('maHeal').Find('swBreakHealPCT').Value() == 'TRUE')
        if breakpressed then
          mq.cmd.luaedit('swBreakHealPCT', _switch and 'TRUE' or 'FALSE')
        end         
       
        -- rampage       
        local rampage, selectedramp = ImGui.InputTextWithHint('ramp##textentry', mq.TLO.Macro.Variable('maHeal').Find('stRampageTank').Value(), '', ImGuiInputTextFlags.EnterReturnsTrue)
        if selectedramp then
          mq.cmd.luaedit('stRampageTank', rampage)
        end

        -- twinheal: healers.. duh..
        if mq.TLO.Me.Class.ShortName() == 'CLR' or mq.TLO.Me.Class.ShortName() == 'DRU' or mq.TLO.Me.Class.ShortName() == 'SHM' then
          local _switch, twinpressed = ImGui.Checkbox("twinheal", mq.TLO.Macro.Variable('maHeal').Find('swTwinHeal').Value() == 'TRUE')
          if twinpressed then
            mq.cmd.luaedit('swTwinHeal', _switch and 'TRUE' or 'FALSE')
          end      
        end           
        
        ImGui.NextColumn()
        ImGui.Columns()  
        ImGui.EndTabItem()
      end


-- combat tab
      if ImGui.BeginTabItem('Combat') then
        ImGui.Columns(3, 'noname', false)

        -- modes
        -- dot
        local _switch, dotpressed = ImGui.Checkbox("dot", mq.TLO.Macro.Variable('maCC').Find('swCombatDoT').Value() == 'TRUE')
        if dotpressed then
          mq.cmd.luaedit('swCombatDoT', _switch and 'TRUE' or 'FALSE')
        end   
        -- heal
        local _switch, healpressed = ImGui.Checkbox("heal", mq.TLO.Macro.Variable('maCC').Find('swCombatHeal').Value() == 'TRUE')
        if healpressed then
          mq.cmd.luaedit('swCombatHeal', _switch and 'TRUE' or 'FALSE')
        end   
        -- melee
        local _switch, meleepressed = ImGui.Checkbox("melee", mq.TLO.Macro.Variable('maCC').Find('swCombatMelee').Value() == 'TRUE')
        if meleepressed then
          mq.cmd.luaedit('swCombatMelee', _switch and 'TRUE' or 'FALSE')
        end   
        -- range
        local _switch, rangepressed = ImGui.Checkbox("range", mq.TLO.Macro.Variable('maCC').Find('swCombatRange').Value() == 'TRUE')
        if rangepressed then
          mq.cmd.luaedit('swCombatRange', _switch and 'TRUE' or 'FALSE')
        end   
        -- nuke
        local _switch, nukepressed = ImGui.Checkbox("nuke", mq.TLO.Macro.Variable('maCC').Find('swCombatNuke').Value() == 'TRUE')
        if nukepressed then
          mq.cmd.luaedit('swCombatNuke', _switch and 'TRUE' or 'FALSE')
        end   
        -- cc
        local _switch, ccpressed = ImGui.Checkbox("cc", mq.TLO.Macro.Variable('maCC').Find('swCombatControl').Value() == 'TRUE')
        if ccpressed then
          if mq.TLO.Me.Class.ShortName() == 'ENC' or mq.TLO.Me.Class.ShortName() == 'BRD' or mq.TLO.Me.Class.ShortName() == 'NEC' or mq.TLO.Me.Class.ShortName() == 'MAG' then
            mq.cmd.luaedit('swCombatControl', _switch and 'TRUE' or 'FALSE')
          end
        end   

        ImGui.NextColumn()
        
        -- smart assist        
        local current_smartass = mq.TLO.Macro.Variable('maCC').Find('stAssistMode').Value()
        if ImGui.BeginCombo("smart", current_smartass) then
          for _, t in ipairs({ 'off', 'g', '1', '2', '3', 'm1', 'm2', 'm3' }) do
            local selectedshade = t == current_smartass
            if ImGui.Selectable(t, selectedshade) and not selectedshade then
              mq.cmd.cc('ass smart', t)
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


-- pull tab
      if ImGui.BeginTabItem('Pull') then
        ImGui.Columns(3, 'noname', false)

        -- pull active
        local _switch, pullactivepressed = ImGui.Checkbox("pull", mq.TLO.Macro.Variable('maPull').Find('swPull').Value() == 'TRUE')
        if pullactivepressed then
          mq.cmd.luaedit('swPull', _switch and 'TRUE' or 'FALSE')
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
        local _switch, pathlogicpressed = ImGui.Checkbox("pathlogic", mq.TLO.Macro.Variable('maPull').Find('swNavPathLogic').Value() == 'TRUE')
        if pathlogicpressed then
          mq.cmd.luaedit('swNavPathLogic', _switch and 'TRUE' or 'FALSE')
        end 

        -- sethome
        local _switch, sethomepressed = ImGui.Checkbox("sethome", mq.TLO.Macro.Variable('maPull').Find('swPullSetHome').Value() == 'TRUE')
        if sethomepressed then
          mq.cmd.luaedit('swPullSetHome', _switch and 'TRUE' or 'FALSE')
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

        ImGui.NextColumn()

        -- navlos
        local _switch, navlospressed = ImGui.Checkbox("navlos", mq.TLO.Macro.Variable('maPull').Find('swPullNavLoS').Value() == 'TRUE')
        if navlospressed then
          mq.cmd.luaedit('swPullNavLoS', _switch and 'TRUE' or 'FALSE')
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
          for _, v in ipairs({ '10', '20', '35', '50', '60', '75', '85', '90', '93', '96' }) do
            local selectedrezpct = v == current_rezpct
            if ImGui.Selectable(v, selectedrezpct) and not selectedrezpct then
              mq.cmd.rez('pct', v)
            end
          end

          ImGui.EndCombo()
        end

        -- in combat rez
        local _switch, icpressed = ImGui.Checkbox("IC", mq.TLO.Macro.Variable('maRez').Find('swRezIC').Value() == 'TRUE')
        if icpressed then
          mq.cmd.luaedit('swRezIC', _switch and 'TRUE' or 'FALSE')
        end
        -- out of combat rez
        local _switch, oocpressed = ImGui.Checkbox("OOC", mq.TLO.Macro.Variable('maRez').Find('swRezOOC').Value() == 'TRUE')
        if oocpressed then
          mq.cmd.luaedit('swRezOOC', _switch and 'TRUE' or 'FALSE')
        end

        ImGui.NextColumn()

        -- dannet rezing
        local _switch, dannetpressed = ImGui.Checkbox("dannet", mq.TLO.Macro.Variable('maRez').Find('swRezDanNet').Value() == 'TRUE')
        if dannetpressed then
          mq.cmd.luaedit('swRezDanNet', _switch and 'TRUE' or 'FALSE')
        end
        -- rez everyone in range
        local _switch, everyonepressed = ImGui.Checkbox("everyone", mq.TLO.Macro.Variable('maRez').Find('swRezEveryone').Value() == 'TRUE')
        if everyonepressed then
          mq.cmd.luaedit('swRezEveryone', _switch and 'TRUE' or 'FALSE')
        end        
        -- use token
        local _switch, tokenpressed = ImGui.Checkbox("use token", mq.TLO.Macro.Variable('maRez').Find('swRezToken').Value() == 'TRUE')
        if tokenpressed then
          mq.cmd.luaedit('swRezToken', _switch and 'TRUE' or 'FALSE')
        end  
        -- take rezes
        local _switch, takepressed = ImGui.Checkbox("take", mq.TLO.Macro.Variable('maRez').Find('swRezTake').Value() == 'TRUE')
        if takepressed then
          mq.cmd.luaedit('swRezTake', _switch and 'TRUE' or 'FALSE')
        end  
        -- take call to corpse
        local _switch, takecallpressed = ImGui.Checkbox("take call", mq.TLO.Macro.Variable('maRez').Find('swRezTakeCall').Value() == 'TRUE')
        if takecallpressed then
          mq.cmd.luaedit('swRezTakeCall', _switch and 'TRUE' or 'FALSE')
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
        local _switch, incombatpressed = ImGui.Checkbox("incombat", mq.TLO.Macro.Variable('maTie').Find('swTieCombat').Value() == 'TRUE')
        if incombatpressed then
          mq.cmd.luaedit('swTieCombat', _switch and 'TRUE' or 'FALSE')
        end          

        -- breakworld to tie
        local _switch, breakworldpressed = ImGui.Checkbox("break to", mq.TLO.Macro.Variable('maTie').Find('swTieBreak').Value() == 'TRUE')
        if breakworldpressed then
          mq.cmd.luaedit('swTieCombat', _switch and 'TRUE' or 'FALSE')
        end  

        ImGui.Columns()
        ImGui.EndTabItem()
      end


-- rest tab
      if ImGui.BeginTabItem('Rest') then
 
        ImGui.Columns(2, 'noname', false)

          -- pct
          local restpct, selectedrestpct = ImGui.InputTextWithHint('pct##textentry', mq.TLO.Macro.Variable('maRest').Find('stPctRest').Value(), '', ImGuiInputTextFlags.EnterReturnsTrue)
          if selectedrestpct then
            mq.cmd.luaedit('stPctRest', restpct)
          end
          
          -- rad
          local restrad, selectedrestrad = ImGui.InputTextWithHint('rad##textentry', mq.TLO.Macro.Variable('maRest').Find('stRestRadius').Value(), '', ImGuiInputTextFlags.EnterReturnsTrue)
          if selectedrestrad then
            mq.cmd.luaedit('stRestRadius', restrad)
          end     

          -- buff
          local _switch, restbuffpressed = ImGui.Checkbox("buff", mq.TLO.Macro.Variable('maRest').Find('swRestBuff').Value() == 'TRUE')
          if restbuffpressed then
            mq.cmd.luaedit('swRestBuff', _switch and 'TRUE' or 'FALSE')
          end  

          -- full
          local _switch, restfullpressed = ImGui.Checkbox("full", mq.TLO.Macro.Variable('maRest').Find('swRestFull').Value() == 'TRUE')
          if restfullpressed then
            mq.cmd.luaedit('swRestFull', _switch and 'TRUE' or 'FALSE')
          end  


        ImGui.NextColumn()

          -- combat
          local _switch, restcombatpressed = ImGui.Checkbox("combat", mq.TLO.Macro.Variable('maRest').Find('swRestCombat').Value() == 'TRUE')
          if restcombatpressed then
            mq.cmd.luaedit('swRestCombat', _switch and 'TRUE' or 'FALSE')
          end  

          -- health
          local _switch, resthealthpressed = ImGui.Checkbox("health", mq.TLO.Macro.Variable('maRest').Find('swRestCheckHealth').Value() == 'TRUE')
          if resthealthpressed then
            mq.cmd.luaedit('swRestCheckHealth', _switch and 'TRUE' or 'FALSE')
          end  


          -- inraid
          local _switch, restraidpressed = ImGui.Checkbox("inraid", mq.TLO.Macro.Variable('maRest').Find('swRestInRaid').Value() == 'TRUE')
          if restraidpressed then
            mq.cmd.luaedit('swRestInRaid', _switch and 'TRUE' or 'FALSE')
          end  
          
          -- modrod
          local _switch, restrodpressed = ImGui.Checkbox("modrod", mq.TLO.Macro.Variable('maRest').Find('swRestModRod').Value() == 'TRUE')
          if restrodpressed then
            mq.cmd.luaedit('swRestModRod', _switch and 'TRUE' or 'FALSE')
          end  
          
        ImGui.Columns()
        ImGui.EndTabItem()
      end    


-- target tab
      if ImGui.BeginTabItem('Target') then
 
        -- name
        ImGui.TextColored(0.39, 0.58, 0.92, 1, 'Name:')
        ImGui.SameLine()
        ImGui.TextColored(1, 1, 1, 1, target['name'])        
        
        ImGui.Columns(2, 'noname', false)

          -- body
          ImGui.TextColored(0.39, 0.58, 0.92, 1, 'Body:')
          ImGui.SameLine()
          ImGui.TextColored(1, 1, 1, 1, ent['body'])        

          -- height
          ImGui.TextColored(0.39, 0.58, 0.92, 1, 'Height:')
          ImGui.SameLine()
          ImGui.TextColored(1, 1, 1, 1, target['height'])


        ImGui.NextColumn()
        
          -- slowed
          ImGui.TextColored(0.39, 0.58, 0.92, 1, 'Slowed:')
          ImGui.SameLine()
          ImGui.TextColored(1, 1, 1, 1, target['slowed']) 

          -- tashed
          ImGui.TextColored(0.39, 0.58, 0.92, 1, 'Tashed:')
          ImGui.SameLine()
          ImGui.TextColored(1, 1, 1, 1, target['tashed']) 

          -- maloed
          ImGui.TextColored(0.39, 0.58, 0.92, 1, 'Malo:')
          ImGui.SameLine()
          ImGui.TextColored(1, 1, 1, 1, target['maloed']) 

          -- crippled
          ImGui.TextColored(0.39, 0.58, 0.92, 1, 'Cripple:')
          ImGui.SameLine()
          ImGui.TextColored(1, 1, 1, 1, target['crippled']) 

          -- snared
          ImGui.TextColored(0.39, 0.58, 0.92, 1, 'Snared:')
          ImGui.SameLine()
          ImGui.TextColored(1, 1, 1, 1, target['snared']) 

          -- ds
          -- ImGui.TextColored(0.39, 0.58, 0.92, 1, 'DS:')
          -- ImGui.SameLine()
          -- ImGui.TextColored(1, 1, 1, 1, target['ds']) 

        ImGui.Columns()
        ImGui.EndTabItem()
      end      

      ImGui.EndTabBar()
    end
  end
  ImGui.End()
end

mq.imgui.init('hudwindow', imguicallback)

while openGUI do 
  mq.delay(1000)
  --if not imgui.exists('hudwindow') then
  --  imgui.init('hudwindow', callback)
  --end  
end







 



