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
  openGUI, shouldDrawHUD = ImGui.Begin('Entropy'..ent['hudtitle']..'###EntropyHUD', openGUI)
  
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
        
        -- column 1
        ImGui.Columns(5, 'noname', false)
          if mq.TLO.Macro.Variable('maEnv').Find('swAuto').Value() == 'TRUE' then
            cmd_button('Auto: on', 60, 48, 'env auto')
          else 
            cmd_button('Auto: off', 60, 48, 'env auto')
          end
        -- column 2
        ImGui.NextColumn()
          cmd_button('On', 60, 22, 'on')
          cmd_button('Off', 60, 22, 'off')
        -- column 3
        ImGui.NextColumn()
          cmd_button('Control', 60, 22, 'env incharge')
          cmd_button('Tie', 60, 22, 'dga /tie')
        -- column 4
        ImGui.NextColumn()
        -- column 5
        ImGui.NextColumn()
          cmd_button('Gather', 60, 22, 'gather')
          cmd_button('Here', 60, 22, 'here')
        ImGui.Columns()
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
          edit_switch_perm_cmd('minion', 'maBuff', 'swPetBuff', 'cb m')
          edit_switch_perm_cmd('raid', 'maBuff', 'swBuffRaid', 'cb r')
          edit_switch_perm_cmd('group', 'maBuff', 'swBuffGroup', 'cb g')
          ImGui.NextColumn()
            edit_switch_perm('shrink', 'maBuff', 'swBuffShrink')
          ImGui.NextColumn()
            edit_switch_perm('master', 'maBuff', 'swBuffMaster')
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
            edit_switch_perm('dannet', 'maHeal', 'swHealDanNet')
            edit_switch_perm('group', 'maHeal', 'swHealGroup')
            edit_switch_perm_cmd('xt', 'maHeal', 'swHealXTarget', 'heal xt build')
            edit_switch_perm('pet', 'maHeal', 'swHealPet')
          ImGui.NextColumn()
            edit_switch_perm('weight', 'maHeal', 'swHealWeighted')
            edit_switch_perm('break', 'maHeal', 'swBreakHealPCT')
            edit_text_perm('rampage', 'maHeal', 'stRampageTank')
            if mq.TLO.Me.Class.ShortName() == 'CLR' or mq.TLO.Me.Class.ShortName() == 'DRU' or mq.TLO.Me.Class.ShortName() == 'SHM' then
              edit_switch_perm('twinheal', 'maHeal', 'swTwinHeal')     
            end           
          ImGui.Columns()  
          ImGui.EndTabItem()
        end
      end

-- combat tab
      if mq.TLO.Macro.Variable('maHud').Find('swTabCombat').Value() == "TRUE" then
        if ImGui.BeginTabItem('Combat') then
          ImGui.Columns(2, 'noname', false)
            edit_switch_perm('dot', 'maCC', 'swCombatDoT')
            edit_switch_perm('heal', 'maCC', 'swCombatHeal')
            edit_switch_perm('melee', 'maCC', 'swCombatMelee')
            edit_switch_perm('range', 'maCC', 'swCombatRange')
            edit_switch_perm('nuke', 'maCC', 'swCombatNuke')
            if mq.TLO.Me.Class.ShortName() == 'ENC' or mq.TLO.Me.Class.ShortName() == 'BRD' or mq.TLO.Me.Class.ShortName() == 'NEC' or mq.TLO.Me.Class.ShortName() == 'MAG' then
              edit_switch_perm('cc', 'maCC', 'swCombatControl')
            end   
          ImGui.NextColumn()
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
            ImGui.NewLine()
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

            edit_switch_perm('pathlogic', 'maPull', 'swNavPathLogic')
            edit_switch_perm('sethome', 'maPull', 'swPullSetHome')
          ImGui.NextColumn()
            edit_text_perm('rad', 'maPull', 'stPullRadius')
            edit_text_perm('zrad', 'maPull', 'stPullZRadius')
            edit_text_perm('stop', 'maPull', 'stPullNavStopDistance')
            edit_text_perm('chain', 'maPull', 'stCountChainPull')
            edit_text_perm('var', 'maPull', 'stPullNavVariance')
          ImGui.NextColumn()
            edit_switch_perm('navlos', 'maPull', 'swPullNavLoS')
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
          ImGui.TextColored(1, 1, 1, 1, target['displayname'])        
          
          ImGui.TextColored(0.39, 0.58, 0.92, 1, 'Name:')
          ImGui.SameLine()
          ImGui.TextColored(1, 1, 1, 1, target['name'])        


            
          ImGui.NewLine()

          ImGui.Columns(2, 'tarbase', false)


            -- body
            ImGui.TextColored(0.39, 0.58, 0.92, 1, 'Body:')
            ImGui.SameLine()
            ImGui.TextColored(1, 1, 1, 1, ent['body'])        

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


          ImGui.NextColumn()

            -- height
            ImGui.TextColored(0.39, 0.58, 0.92, 1, 'Height:')
            ImGui.SameLine()
            ImGui.TextColored(1, 1, 1, 1, target['height'])


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
      end  

-- about tab
      if mq.TLO.Macro.Variable('maHud').Find('swTabAbout').Value() == "TRUE" then
        if ImGui.BeginTabItem('About') then
        end
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







 



