--
-- entropoy.mac
-- hud.lua
-- 
-- hud and things
--


require 'common'


local openGUI = true
local shouldDrawHUD = true

local function imguicallback()
  hudInfo()
  openGUI, shouldDrawHUD = ImGui.Begin(ent.hudtitle .. '###EntropyHUD', openGUI)
  
  if shouldDrawHUD and (ent.build == '--' or mq.TLO.EverQuest.GameState() ~= 'INGAME') then
    info_about()

  elseif shouldDrawHUD then  
    if ImGui.BeginTabBar('##mytabs') then

-- home tab
      if ImGui.BeginTabItem('Home') then
        settableflags()
       	if ImGui.BeginTable('hud', 6, tableFlags) then

          -- row 1
      		ImGui.TableNextRow()	
        		ImGui.TableNextColumn()
              ImGui.TextColored(0.39, 0.58, 0.92, 1, 'mode')
      			ImGui.TableNextColumn()
      			  ImGui.TextColored(1, 1, 1, 1, ent.mode)
      			ImGui.TableNextColumn()
      			  ImGui.TextColored(0.39, 0.58, 0.92, 1, (mq.TLO.Macro.Variable('maTie').Find('stTieMode').Value() or ''))
      			ImGui.TableNextColumn()
      			  ImGui.TextColored(1, 1, 1, 1, (mq.TLO.Macro.Variable('maTie').Find('stTieToon').Value() or ''))
      			ImGui.TableNextColumn()
              ImGui.TextColored(0.39, 0.58, 0.92, 1, 'env')
      			ImGui.TableNextColumn()
      			  ImGui.TextColored(1, 1, 1, 1, (mq.TLO.Macro.Variable('maEnv').Find('stEnvRadius').Value() or ''))

          -- row 2
      		ImGui.TableNextRow()	
        		ImGui.TableNextColumn()
              ImGui.TextColored(0.39, 0.58, 0.92, 1, 'yxz')
    	  		ImGui.TableNextColumn()
              if mq.TLO.Me.Y() then
                ImGui.TextColored(1, 1, 1, 1, string.format('%.1f, %.1f, %.1f', mq.TLO.Me.Y(), mq.TLO.Me.X(), mq.TLO.Me.Z()))
             end
    	  		ImGui.TableNextColumn()
    	  		ImGui.TableNextColumn()
    	  		ImGui.TableNextColumn()
              if mq.TLO.Macro.Variable('maPull').Find('swPull').Value() == 'TRUE' then
                ImGui.TextColored(0, 1, 0, 1, 'pull')
              else 
                ImGui.TextColored(1, 0, 0, 1, 'pull')
              end
              if ImGui.IsItemHovered() and ImGui.IsMouseReleased(ImGuiMouseButton.Left) then
                mq.cmd.pull('active')
              end 
    	  		ImGui.TableNextColumn()
      			  ImGui.TextColored(1, 1, 1, 1, (mq.TLO.Macro.Variable('maPull').Find('stPullRadius').Value() or ''))
              ImGui.SameLine()
    		
      		-- row 3
      		ImGui.TableNextRow()	
        		ImGui.TableNextColumn()
              ImGui.TextColored(0.39, 0.58, 0.92, 1, 'set')
        		ImGui.TableNextColumn()
              ImGui.TextColored(1, 1, 1, 1, ent.set)
        		ImGui.TableNextColumn()
        		ImGui.TableNextColumn()
        		ImGui.TableNextColumn()
        		ImGui.TableNextColumn()

      		ImGui.Columns()
          ImGui.EndTable()
        end

        -- ImGui.Separator()
        -- engage %s, target body, distances
        ImGui.TextColored(0.39, 0.58, 0.92, 1, '[')
        ImGui.SameLine()
        ImGui.TextColored(1, 1, 1, 1, (mq.TLO.Macro.Variable('maCC').Find('stPctEngage').Value() or ''))
        ImGui.SameLine()
        ImGui.TextColored(0.39, 0.58, 0.92, 1, '][')
        ImGui.SameLine()
        ImGui.TextColored(1, 1, 1, 1, (mq.TLO.Macro.Variable('maMinion').Find('stPctPetEngage').Value() or ''))
        ImGui.SameLine()
        ImGui.TextColored(0.39, 0.58, 0.92, 1, '][')
        ImGui.SameLine()
        ImGui.TextColored(1, 1, 1, 1, (mq.TLO.Macro.Variable('maMinion').Find('stPctSwarmEngage').Value() or ''))
        ImGui.SameLine()
        ImGui.TextColored(0.39, 0.58, 0.92, 1, '][')
        ImGui.SameLine()
        ImGui.TextColored(1, 1, 1, 1, (mq.TLO.Target.Body.Name() or ''))
        ImGui.SameLine()
        ImGui.TextColored(0.39, 0.58, 0.92, 1,']')
        ImGui.SameLine()
        ImGui.Text(string.format('%.0f : %.0f', mq.TLO.Target.Distance() or 0 , mq.TLO.Target.MaxRangeTo() or 0))
        ImGui.SameLine()
        ImGui.TextColored(1, 1, 1, 1, (isSlowedBase() or ''))


        -- target information
        if mq.TLO.Target.ID() ~= 0 then
          local spawn = mq.TLO.Spawn(mq.TLO.Target.ID())
          local guildName = ' '
          if spawn.Guild() ~= nil then 
            guildName = '<' .. spawn.Guild()  .. '>'
          end
          if spawn.LineOfSight() then
            if mq.TLO.Macro.Variable('maHud').Find('swHUDDisplayName').Value() == 'TRUE' then
              ImGui.TextColored(0, 1, 0, 1, '[' .. spawn.Class.ShortName() .. ' ' .. spawn.Level() .. '] ' .. spawn.DisplayName() .. ' ' .. guildName )
            else
              ImGui.TextColored(0, 1, 0, 1, '[' .. spawn.Class.ShortName() .. ' ' .. spawn.Level() .. '] ' .. spawn.Name() .. ' ' .. guildName )
             end
          else
            if mq.TLO.Macro.Variable('maHud').Find('swHUDDisplayName').Value() == 'TRUE' then
              ImGui.TextColored(1, 0, 0, 1, '[' .. spawn.Class.ShortName() .. ' ' .. spawn.Level() .. '] ' .. spawn.DisplayName() .. ' ' .. guildName )
            else
              ImGui.TextColored(1, 0, 0, 1, '[' .. spawn.Class.ShortName() .. ' ' .. spawn.Level() .. '] ' .. spawn.Name() .. ' ' .. guildName )
            end
         end

        else
          ImGui.TextDisabled('')
        end
        
        ImGui.EndTabItem()
      end

-- buff tab 
      if mq.TLO.Macro.Variable('maHud').Find('swTabBuff').Value() == "TRUE" then
        if ImGui.BeginTabItem('Buff') then
        ImGui.Columns(4, 'noname', false)
          edit_switch_perm_cmd('aura', 'maBuff', 'swBuffAura', 'cb a')
          edit_switch_perm_cmd('self', 'maBuff', 'swBuffSelf', 'cb s')
          edit_switch_perm_cmd('crew', 'maBuff', 'swBuffCrew', 'cb c')
          edit_switch_perm_cmd('temp', 'maBuff', 'swBuffTemp', 'cb t')
        ImGui.NextColumn()
          edit_switch_perm_cmd('unity', 'maBuff', 'swBuffUnity', 'cb u')
          edit_switch_perm_cmd('minion', 'maMinion', 'swPetBuff', 'cb m')
          edit_switch_perm_cmd('raid', 'maBuff', 'swBuffRaid', 'cb r')
          edit_switch_perm_cmd('group', 'maBuff', 'swBuffGroup', 'cb g')
          ImGui.NextColumn()
            edit_switch_perm('shrink', 'maBuff', 'swBuffShrink')
            edit_switch_perm('nolobby', 'maBuff', 'swBuffNoLobby')
          ImGui.NextColumn()
            edit_switch_temp('master', 'maBuff', 'swBuffMaster')
            edit_switch_perm('now', 'maBuff', 'swBuffNow')
            edit_switch_perm('ammo', 'maBuff', 'swBuffAmmo')
            edit_switch_perm('beg', 'maBuff', 'swBuffBeg')
        ImGui.Columns()    
        ImGui.EndTabItem()
        end
      end

-- heal tab
      if mq.TLO.Macro.Variable('maHud').Find('swTabHeal').Value() == "TRUE" then
        if ImGui.BeginTabItem('Heal') then
          ImGui.Columns(2, 'noname', false)
            edit_switch_perm('self', 'maHeal', 'swHealSelf')
            ImGui.SameLine()
            edit_switch_perm('pet', 'maHeal', 'swHealPet')
            edit_switch_perm('dannet', 'maHeal', 'swHealDanNet')
            ImGui.SameLine()
            edit_switch_perm('group', 'maHeal', 'swHealGroup')
            edit_switch_perm_cmd('xt', 'maHeal', 'swHealXTarget', 'heal xt build')
            if mq.TLO.Me.Class.ShortName() == 'CLR' or mq.TLO.Me.Class.ShortName() == 'PAL' then
              edit_switch_perm('splashmeonly', 'maHeal', 'swSplashMeOnly')     
            end  

          ImGui.NextColumn()
            edit_switch_perm('weight', 'maHeal', 'swHealWeighted')
            ImGui.SameLine()
            edit_switch_perm('break', 'maHeal', 'swBreakHealPCT')
            if class_wis[mq.TLO.Me.Class.ShortName()] then
              edit_switch_perm('twinheal', 'maHeal', 'swTwinHeal')     
            end
     
            edit_text_perm('adj', 'maHeal', 'stHealAdjust')
            edit_text_perm('ramp', 'maHeal', 'stRampageTank')
          ImGui.Columns()  
          ImGui.EndTabItem()
        end
      end

-- combat tab
      if mq.TLO.Macro.Variable('maHud').Find('swTabCombat').Value() == "TRUE" then
        if ImGui.BeginTabItem('Combat') then
          ImGui.Columns(3, 'firstc', false)
            edit_switch_perm('dot', 'maCC', 'swCombatDoT')
            edit_switch_perm('heal', 'maCC', 'swCombatHeal')
          ImGui.NextColumn()
            edit_switch_perm('melee', 'maCC', 'swCombatMelee')
            edit_switch_perm('range', 'maCC', 'swCombatRange')
          ImGui.NextColumn()
            edit_switch_perm('nuke', 'maCC', 'swCombatNuke')
            if mq.TLO.Me.Class.ShortName() == 'ENC' or mq.TLO.Me.Class.ShortName() == 'BRD' or mq.TLO.Me.Class.ShortName() == 'NEC' or mq.TLO.Me.Class.ShortName() == 'MAG' then
              edit_switch_perm('cc', 'maCC', 'swCombatControl')
            end   
          ImGui.Columns()
            
            
            
          ImGui.Columns(2, 'secondc', false)
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
            edit_switch_perm('manual', 'maCC', 'swManualAssist')
            
          ImGui.NextColumn()
            edit_text_perm('engage', 'maCC', 'stPctEngage')
            edit_text_perm('zrad', 'maCC', 'stZRadius')
          ImGui.Columns()
          ImGui.EndTabItem()
        end
      end

-- pull tab
      if mq.TLO.Macro.Variable('maHud').Find('swTabPull').Value() == "TRUE" then
        if ImGui.BeginTabItem('Pull') then
          ImGui.Columns(3, 'noname', false)

            edit_switch_perm('pull', 'maPull', 'swPull')
 
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
            edit_text_perm('var', 'maPull', 'stPullNavVariance')

          ImGui.NextColumn()
            edit_text_perm('rad', 'maPull', 'stPullRadius')
            edit_text_perm('zrad', 'maPull', 'stPullZRadius')
            edit_text_perm('stop', 'maPull', 'stPullNavStopDistance')
            edit_text_perm('chain', 'maPull', 'stCountChainPull')
          ImGui.NextColumn()
            edit_switch_perm('navlos', 'maPull', 'swPullNavLoS')
            edit_switch_perm('pathlogic', 'maPull', 'swNavPathLogic')
            edit_switch_perm('sethome', 'maPull', 'swPullSetHome')
          ImGui.Columns()
          ImGui.EndTabItem()
                  
        end
      end

-- rez tab
      if mq.TLO.Macro.Variable('maHud').Find('swTabRez').Value() == "TRUE" then
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

            edit_switch_perm('IC', 'maRez', 'swRezIC')
            edit_switch_perm('OOC', 'maRez', 'swRezOOC')
          ImGui.NextColumn()
            edit_switch_perm('dannet', 'maRez', 'swRezDanNet')
            edit_switch_perm('everyone', 'maRez', 'swRezEveryone')
            edit_switch_perm('use token', 'maRez', 'swRezToken')
            edit_switch_perm('take', 'maRez', 'swRezTake')
            ImGui.SameLine()
            edit_switch_perm('take call', 'maRez', 'swRezTakeCall')
          ImGui.Columns()      
          ImGui.EndTabItem()
        end      
      end

-- tie tab
      if mq.TLO.Macro.Variable('maHud').Find('swTabTie').Value() == "TRUE" then
        if ImGui.BeginTabItem('Tie') then
          ImGui.Columns(2, 'noname', false)
            edit_text_perm('toon', 'maTie', 'stTieToon')
            edit_text_perm('variance', 'maTie', 'stTieVariance')
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
            edit_switch_perm('incombat', 'maTie', 'swTieCombat')
            edit_switch_perm('break to', 'maTie', 'swTieBreak')
          ImGui.Columns()
          ImGui.EndTabItem()
        end
      end

-- rest tab
      if mq.TLO.Macro.Variable('maHud').Find('swTabRest').Value() == "TRUE" then
        if ImGui.BeginTabItem('Rest') then
          ImGui.Columns(2, 'noname', false)
            edit_text_perm('pct', 'maRest', 'stPctRest')
            edit_text_perm('radius', 'maRest', 'stRestRadius')
            edit_switch_perm('buff', 'maRest', 'swRestBuff')
            edit_switch_perm('full', 'maRest', 'swRestFull')
          ImGui.NextColumn()
            edit_switch_perm('combat', 'maRest', 'swRestCombat')
            edit_switch_perm('health', 'maRest', 'swRestCheckHealth')
            edit_switch_perm('inraid', 'maRest', 'swRestInRaid')
            edit_switch_perm('modrod', 'maRest', 'swRestModRod')
          ImGui.Columns()
          ImGui.EndTabItem()
        end    
      end

-- target tab
      if mq.TLO.Macro.Variable('maHud').Find('swTabTarget').Value() == "TRUE" then
        if ImGui.BeginTabItem('Target') then
   
          -- name
          ImGui.TextColored(0.39, 0.58, 0.92, 1, 'DisplayName:')
          ImGui.SameLine()
          ImGui.TextColored(1, 1, 1, 1, (mq.TLO.Target.DisplayName() or ''))        
          
          ImGui.TextColored(0.39, 0.58, 0.92, 1, 'Name:')
          ImGui.SameLine()
          ImGui.TextColored(1, 1, 1, 1, (mq.TLO.Target.Name() or ''))        
            
          ImGui.NewLine()

          ImGui.Columns(2, 'tarbase', false)

            -- ID
            ImGui.TextColored(0.39, 0.58, 0.92, 1, 'ID:')
            ImGui.SameLine()
            ImGui.TextColored(1, 1, 1, 1, (mq.TLO.Target.ID() or ''))        

            -- body
            ImGui.TextColored(0.39, 0.58, 0.92, 1, 'Body:')
            ImGui.SameLine()
            ImGui.TextColored(1, 1, 1, 1, (mq.TLO.Target.Body() or ''))        

            -- slowed
            ImGui.TextColored(0.39, 0.58, 0.92, 1, 'Slowed:')
            ImGui.SameLine()
            ImGui.TextColored(1, 1, 1, 1, isSlowed()) 

            -- tashed
            ImGui.TextColored(0.39, 0.58, 0.92, 1, 'Tashed:')
            ImGui.SameLine()
            ImGui.TextColored(1, 1, 1, 1, isTashed()) 

            -- maloed
            ImGui.TextColored(0.39, 0.58, 0.92, 1, 'Malo:')
            ImGui.SameLine()
            ImGui.TextColored(1, 1, 1, 1, isMaloed()) 


          ImGui.NextColumn()

            -- height
            ImGui.TextColored(0.39, 0.58, 0.92, 1, 'Height:')
            ImGui.SameLine()
            ImGui.TextColored(1, 1, 1, 1, string.format('%.2f', mq.TLO.Target.Height() or 0))

            -- crippled
            ImGui.TextColored(0.39, 0.58, 0.92, 1, 'Cripple:')
            ImGui.SameLine()
            ImGui.TextColored(1, 1, 1, 1, isCrippled()) 

            -- snared
            ImGui.TextColored(0.39, 0.58, 0.92, 1, 'Snared:')
            ImGui.SameLine()
            ImGui.TextColored(1, 1, 1, 1, isSnared()) 

          ImGui.Columns()
          ImGui.EndTabItem()
        end    
      end  

-- about tab
      if mq.TLO.Macro.Variable('maHud').Find('swTabAbout').Value() == "TRUE" then
        if ImGui.BeginTabItem('About') then
         
          ImGui.TextColored(1, 1, 1, 1, 'Entropy')
          ImGui.TextColored(0.39, 0.58, 0.92, 1, 'Build: ')
          ImGui.SameLine()
          ImGui.TextColored(1, 1, 1, 1, ent['build'])        
          ImGui.NewLine()
          info_about()

        end
      end      
      ImGui.EndTabBar()
    end  
    
    -- buttons at the bottom
    if mq.TLO.Macro.Variable('maHud').Find('swHUDbuttons').Value() == "TRUE" then
      ImGui.Separator()
      
      if btnoptn('allauto') then 
        cmd_button(ico.allauto, ico.x, ico.y, 'on', 'all macro auto') 
        ImGui.SameLine()
      end
      if btnoptn('allmanual') then 
        cmd_button(ico.allmanual, ico.x, ico.y, 'off', 'all macro manual')
        ImGui.SameLine()
      end      
      if btnoptn('auto') then 
        if mq.TLO.Macro.Variable('maEnv').Find('swAuto').Value() == 'TRUE' then
          cmd_button(ico.manual, ico.x, ico.y, 'env auto', 'macro manual')
        else 
          cmd_button(ico.auto, ico.x, ico.y, 'env auto', 'macro auto')
        end
        ImGui.SameLine()
      end
      if btnoptn('tie') then 
        cmd_button(ico.tie, ico.x, ico.y, 'dga /tie', '/tie on/off')
        ImGui.SameLine()
      end
      if btnoptn('tienav') then 
        cmd_button(ico.tienav, ico.x, ico.y, 'dga /tie nav', 'tie with nav')
        ImGui.SameLine()
      end
      if btnoptn('tiestick') then 
        cmd_button(ico.tiestick, ico.x, ico.y, 'dga /tie stick', 'tie with stick')
        ImGui.SameLine()
      end
      if btnoptn('incharge') then 
        cmd_button(ico.incharge, ico.x, ico.y, 'env incharge', '/incharge')
        ImGui.SameLine()
      end
      if btnoptn('invis') then 
        cmd_button(ico.invis, ico.x, ico.y, 'invis', 'stack invisibility')
        ImGui.SameLine()
      end
      if btnoptn('noinvis') then 
        cmd_button(ico.noinvis, ico.x, ico.y, 'dga /nomore invis', 'remove all invisibility')
        ImGui.SameLine()
      end
      if btnoptn('intpull') then 
        cmd_button(ico.intpull, ico.x, ico.y, 'pull one int', 'single intervention pull')
        ImGui.SameLine()
      end
      if btnoptn('gather') then 
        cmd_button(ico.gather, ico.x, ico.y, 'gather', '/gather')
        ImGui.SameLine()
      end
      if btnoptn('here') then 
        cmd_button(ico.here, ico.x, ico.y, 'here', '/here')
        ImGui.SameLine()
      end
      if btnoptn('radar') then 
        cmd_button(ico.radar, ico.x, ico.y, 'ui2 radar', 'ui: radar')
        ImGui.SameLine()
      end      
      if btnoptn('drive') then 
        cmd_button(ico.drive, ico.x, ico.y, 'ui2 drive', 'ui: drive')
        ImGui.SameLine()
      end
      if btnoptn('event') then 
        cmd_button(ico.event, ico.x, ico.y, 'ui2 event buttons', 'ui: event buttons')
        ImGui.SameLine()
      end
      if btnoptn('notes') then 
        cmd_button(ico.notes, ico.x, ico.y, 'ui2 notes', 'ui: notes')
        ImGui.SameLine()
      end    end

  end
  ImGui.End()
end

mq.imgui.init('hudwindow', imguicallback)

while openGUI do 
  mq.delay(1000)
end







 



