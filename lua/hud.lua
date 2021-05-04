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
  openGUI, shouldDrawHUD = ImGui.Begin('Entropy '..ent['build'], openGUI, ImGuiWindowFlags.NoScrollbar)

  if shouldDrawHUD then
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
          ImGui.TextColored(0.39, 0.58, 0.92, 1, ent['tiemode'])
          ImGui.SameLine()
          ImGui.TextColored(1, 1, 1, 1, ent['tietoon'])


        ImGui.NextColumn()
          -- env
          ImGui.TextColored(0.39, 0.58, 0.92, 1, 'env')
          ImGui.SameLine()
          ImGui.TextColored(1, 1, 1, 1, ent['envrad'])
          -- pull
          pull = ImGui.SmallButton('pull')
          if pull then
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
        -- end of columns
        ImGui.Columns()

        -- engage %s, target body, distances
        ImGui.TextColored(0.39, 0.58, 0.92, 1, '[')
        ImGui.SameLine()
        ImGui.TextColored(1, 1, 1, 1, ent['engage'])
        ImGui.SameLine()
        ImGui.TextColored(0.39, 0.58, 0.92, 1, '][')
        ImGui.SameLine()
        ImGui.TextColored(1, 1, 1, 1, ent['petengage'])
        ImGui.SameLine()
        ImGui.TextColored(0.39, 0.58, 0.92, 1, '][')
        ImGui.SameLine()
        ImGui.TextColored(1, 1, 1, 1, ent['swarmengage'])
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
        ImGui.TextColored(1, 1, 1, 1, target['maxrangeto'])

        -- target information
        if mq.TLO.Target.ID() ~= nil then   
          ImGui.TextColored(1, 1, 1, 1, '['..target['shortname'].." "..target['level']..'] '..target['displayname']..' <'..target['guild']..'>')
        else
          ImGui.TextDisabled('  ')
        end



-- home tab
        ImGui.Columns(5, 'noname', false)

        -- column 1
          local auto = ImGui.Button('Auto', 60, 48)
          if auto then
            mq.cmd.env('auto')
          end

        -- column 2
          ImGui.NextColumn()
          local on = ImGui.Button('On', 60, 22)
          if auto then
            mq.cmd.on()
          end
          
          local off = ImGui.Button('Off', 60, 22)
          if auto then
            mq.cmd.off()
          end


        -- column 3
          ImGui.NextColumn()

          local incharge = ImGui.Button('Control', 60, 22)
          if agro then
            mq.cmd.agro('active')
          end

          local tie = ImGui.Button('Tie', 60, 22)
          if agro then
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
          mq.cmd.buff('aura', doaura and 'on' or 'off')
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
          mq.cmd.buff('self', doself and 'on' or 'off')
        end
        ImGui.SameLine()
        local buffself = ImGui.Button('self', 60, 22)
        if buffself then
          mq.cmd.cb('s')
        end
        
        -- crew
        local docrew, crewpressed = ImGui.Checkbox("##docrew", mq.TLO.Macro.Variable('maBuff').Find('swBuffCrew').Value() == 'TRUE')
        if crewpressed then
          mq.cmd.buff('crew', docrew and 'on' or 'off')
        end
        ImGui.SameLine()
        local buffcrew = ImGui.Button('crew', 60, 22)
        if buffcrew then
          mq.cmd.cb('c')
        end
                
        -- temp
        local dotemp, temppressed = ImGui.Checkbox("##dotemp", mq.TLO.Macro.Variable('maBuff').Find('swBuffTemp').Value() == 'TRUE')
        if temppressed then
          mq.cmd.buff('temp', dotemp and 'on' or 'off')
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
          mq.cmd.buff('unity', dounity and 'on' or 'off')
        end
        ImGui.SameLine()
        local buffunity = ImGui.Button('unity', 60, 22)
        if buffunity then
          mq.cmd.cb('u')
        end
        
        -- minion
        local dominion, minionpressed = ImGui.Checkbox("##dominion", mq.TLO.Macro.Variable('maMinion').Find('swPetBuff').Value() == 'TRUE')
        if minionpressed then
          mq.cmd.minion('buff', dominion and 'on' or 'off')
        end
        ImGui.SameLine()
        local buffminion = ImGui.Button('minion', 60, 22)
        if buffminion then
          mq.cmd.cb('m')
        end
        
        -- raid
        local doraid, raidpressed = ImGui.Checkbox("##doraid", mq.TLO.Macro.Variable('maBuff').Find('swBuffRaid').Value() == 'TRUE')
        if raidpressed then
          mq.cmd.buff('raid', doraid and 'on' or 'off')
        end   
        ImGui.SameLine()
        local buffraid = ImGui.Button('raid', 60, 22)
        if buffraid then
          mq.cmd.cb('r')
        end
        
        -- group
        local dogroup, grouppressed = ImGui.Checkbox("##dogroup", mq.TLO.Macro.Variable('maBuff').Find('swBuffGroup').Value() == 'TRUE')
        if grouppressed then
          mq.cmd.buff('group', dogroup and 'on' or 'off')
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
          mq.cmd.buff('shrink active', doshrink and 'on' or 'off')
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
          mq.cmd.buff('now', donow and 'on' or 'off')
        end        

        -- ammo
        local doammo, ammopressed = ImGui.Checkbox("ammo", mq.TLO.Macro.Variable('maBuff').Find('swBuffAmmo').Value() == 'TRUE')
        if ammopressed then
          mq.cmd.buff('ammo', doammo and 'on' or 'off')
        end

        -- temp
        local dobeg, begpressed = ImGui.Checkbox("beg", mq.TLO.Macro.Variable('maBuff').Find('swBuffBeg').Value() == 'TRUE')
        if begpressed then
          mq.cmd.buff('beg', dobeg and 'on' or 'off')
        end
         
        ImGui.Columns()    
        ImGui.EndTabItem()
      end



-- heal tab
      if ImGui.BeginTabItem('Heal') then
        ImGui.Columns(4, 'noname', false)

        -- self
        local doself, selfpressed = ImGui.Checkbox("self", mq.TLO.Macro.Variable('maHeal').Find('swHealSelf').Value() == 'TRUE')
        if selfpressed then
          mq.cmd.heal('self', doself and 'on' or 'off')
        end   

        -- dannet
        local dodannet, dannetpressed = ImGui.Checkbox("dannet", mq.TLO.Macro.Variable('maHeal').Find('swHealDanNet').Value() == 'TRUE')
        if dannetpressed then
          mq.cmd.heal('dannet', dodannet and 'on' or 'off')
        end  
        
        -- group
        local dogroup, grouppressed = ImGui.Checkbox("group", mq.TLO.Macro.Variable('maHeal').Find('swHealGroup').Value() == 'TRUE')
        if grouppressed then
          mq.cmd.heal('group', dogroup and 'on' or 'off')
        end  
        
        -- xtarget        
        local doxt, xtpressed = ImGui.Checkbox("##xtarget", mq.TLO.Macro.Variable('maHeal').Find('swHealXTarget').Value() == 'TRUE')
        if xtpressed then
          mq.cmd.heal('xt', doxt and 'on' or 'off')
        end
        ImGui.SameLine()
        local healxt = ImGui.Button('xt', 60, 22)
        if healxt then
          mq.cmd.heal('xt build')
        end

        -- pet        
        local dopet, petpressed = ImGui.Checkbox("pet", mq.TLO.Macro.Variable('maHeal').Find('swHealPet').Value() == 'TRUE')
        if petpressed then
          mq.cmd.heal('pet', dopet and 'on' or 'off')
        end           
       
        ImGui.NextColumn()
        
        -- weight
        local doweight, weightpressed = ImGui.Checkbox("weight", mq.TLO.Macro.Variable('maHeal').Find('swHealWeighted').Value() == 'TRUE')
        if weightpressed then
          mq.cmd.heal('weight', doweight and 'on' or 'off')
        end        
        
        -- break
        local dobreak, breakpressed = ImGui.Checkbox("break", mq.TLO.Macro.Variable('maHeal').Find('swBreakHealPCT').Value() == 'TRUE')
        if breakpressed then
          mq.cmd.heal('break', dobreak and 'on' or 'off')
        end         
        
        
        ImGui.NextColumn()
        
        ImGui.NextColumn()
        
        ImGui.Columns()   
        ImGui.EndTabItem()
      end



-- combat tab
      if ImGui.BeginTabItem('Combat') then
        ImGui.EndTabItem()
      end


-- rez tab
      if ImGui.BeginTabItem('Rez') then

        local rezrad = tonumber(mq.TLO.Macro.Variable('maRez').Find('stMaxRezRange').Value())
        rezrad, used = ImGui.DragInt("rez radius", rezrad, 1, 0, 200);
        if used then
          mq.cmd.edit('stMaxRezRange', rezrad, FALSE, FALSE, TRUE)  
        end

        
      
        ImGui.EndTabItem()
      end      


-- tie tab
      if ImGui.BeginTabItem('Tie') then
        ImGui.Columns(3, 'noname', false)

        ImGui.NextColumn()

        ImGui.NextColumn()

        ImGui.Columns()

        ImGui.EndTabItem()
      end

      ImGui.EndTabBar()
    end
    ImGui.End()
  end
end

mq.imgui.init('mywindow', imguicallback)

mq.bind('/entropygui', function()
    openGUI = not openGUI
end)

while true do mq.delay(1) end







 



