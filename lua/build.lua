--
-- entropoy.mac
-- build.lua
-- 
-- character building
--



require "common"
local openGUI = true
local shouldDrawHUD = true


healPoint = {}
local function updateHealPoints()
  for _, classCode in pairs(classTable) do
    healPoint[classCode] = tonumber(mq.TLO.Macro.Variable('maHeal').Find('stHealPoint'..classCode).Value())
  end
end

updateHealPoints()


local function imguicallback()
  hudInfo()
  
  openGUI, shouldDrawHUD = ImGui.Begin('Entropy Editor '..ent['build'], openGUI)
  
  if shouldDrawHUD and ent['build'] == '--' then
    ImGui.Text('Entropy is not running')
    ImGui.End()


  elseif shouldDrawHUD then  
    if ImGui.BeginTabBar('##editortabs') then
      

-- ENTROPY TAB
      if ImGui.BeginTabItem('Entropy') then
        ImGui.NewLine()

        ImGui.Columns(2, 'entbase', false)
          -- swAutoList
          local _switch, pressed = ImGui.Checkbox("AutoList", mq.TLO.Macro.Variable('maEntropy').Find('swAutoList').Value() == 'TRUE')
          if pressed then
            mq.cmd.luaedit('swAutoList', _switch and 'TRUE' or 'FALSE')
          end
          -- swBuffCooldownWait
          local _switch, pressed = ImGui.Checkbox("Buff Cooldown", mq.TLO.Macro.Variable('maEntropy').Find('swBuffCooldownWait').Value() == 'TRUE')
          if pressed then
            mq.cmd.luaedit('swBuffCooldownWait', _switch and 'TRUE' or 'FALSE')
          end
          -- swUseConColor
          local _switch, pressed = ImGui.Checkbox("Con Color", mq.TLO.Macro.Variable('maEntropy').Find('swUseConColor').Value() == 'TRUE')
          if pressed then
            mq.cmd.luaedit('swUseConColor', _switch and 'TRUE' or 'FALSE')
          end
          -- swTLP
          local _switch, pressed = ImGui.Checkbox("TLP", mq.TLO.Macro.Variable('maEntropy').Find('swTLP').Value() == 'TRUE')
          if pressed then
            mq.cmd.luaedit('swTLP', _switch and 'TRUE' or 'FALSE')
          end
          -- swBuildSetRemember
          local _switch, pressed = ImGui.Checkbox("Set Remember", mq.TLO.Macro.Variable('maEntropy').Find('swBuildSetRemember').Value() == 'TRUE')
          if pressed then
            mq.cmd.luaedit('swBuildSetRemember', _switch and 'TRUE' or 'FALSE')
          end
          -- swRelayTell
          local _switch, pressed = ImGui.Checkbox("Relay Tells", mq.TLO.Macro.Variable('maEntropy').Find('swRelayTell').Value() == 'TRUE')
          if pressed then
            mq.cmd.luaedit('swRelayTell', _switch and 'TRUE' or 'FALSE')
          end
          -- swMaintenance
          local _switch, pressed = ImGui.Checkbox("Maitnenace", mq.TLO.Macro.Variable('maEntropy').Find('swMaintenance').Value() == 'TRUE')
          if pressed then
            mq.cmd.luaedit('swMaintenance', _switch and 'TRUE' or 'FALSE')
          end
          -- swAutoInvManual
          local _switch, pressed = ImGui.Checkbox("Inv in Manual", mq.TLO.Macro.Variable('maEntropy').Find('swAutoInvManual').Value() == 'TRUE')
          if pressed then
            mq.cmd.luaedit('swAutoInvManual', _switch and 'TRUE' or 'FALSE')
          end
          -- swEventsInc
          local _switch, pressed = ImGui.Checkbox("Events", mq.TLO.Macro.Variable('maEntropy').Find('swEventsInc').Value() == 'TRUE')
          if pressed then
            mq.cmd.luaedit('swEventsInc', _switch and 'TRUE' or 'FALSE')
          end
          -- swHookSub
          local _switch, pressed = ImGui.Checkbox("Hooks", mq.TLO.Macro.Variable('maEntropy').Find('swHookSub').Value() == 'TRUE')
          if pressed then
            mq.cmd.luaedit('swHookSub', _switch and 'TRUE' or 'FALSE')
          end
          -- swHUDAuto
          local _switch, pressed = ImGui.Checkbox("HUD", mq.TLO.Macro.Variable('maEntropy').Find('swHUDAuto').Value() == 'TRUE')
          if pressed then
            mq.cmd.luaedit('swHUDAuto', _switch and 'TRUE' or 'FALSE')
          end

        ImGui.NextColumn()
          
          -- stCastLoop
          local line, selected = ImGui.InputTextWithHint('Cast Loop', mq.TLO.Macro.Variable('maEntropy').Find('stCastLoop').Value(), '', ImGuiInputTextFlags.EnterReturnsTrue)
          if selected then
            mq.cmd.luaedit('stCastLoop', line)
          end  
          -- stEngine
          local line, selected = ImGui.InputTextWithHint('Engine', mq.TLO.Macro.Variable('maEntropy').Find('stEngine').Value(), '', ImGuiInputTextFlags.EnterReturnsTrue)
          if selected then
            mq.cmd.luaedit('stEngine', line)
          end  
          -- stCountFizzleRetry
          local line, selected = ImGui.InputTextWithHint('Fizzles', mq.TLO.Macro.Variable('maEntropy').Find('stCountFizzleRetry').Value(), '', ImGuiInputTextFlags.EnterReturnsTrue)
          if selected then
            mq.cmd.luaedit('stCountFizzleRetry', line)
          end  
          -- stMainLoopDelay
          local line, selected = ImGui.InputTextWithHint('Main Loop', mq.TLO.Macro.Variable('maEntropy').Find('stMainLoopDelay').Value(), '', ImGuiInputTextFlags.EnterReturnsTrue)
          if selected then
            mq.cmd.luaedit('stMainLoopDelay', line)
          end  

        ImGui.Columns()          

        ImGui.Columns(3, 'ent1', false)
        ImGui.NextColumn()
        ImGui.NextColumn()
        ImGui.Columns()

        ImGui.NewLine()         

        ImGui.Separator()
        ImGui.TextColored(0.39, 0.58, 0.92, 1, 'Random')
        ImGui.Columns(2, 'random', false)
          -- stRNDEngage
          local line, selected = ImGui.InputTextWithHint('Engage', mq.TLO.Macro.Variable('maEntropy').Find('stRNDEngage').Value(), '', ImGuiInputTextFlags.EnterReturnsTrue)
          if selected then
            mq.cmd.luaedit('stRNDEngage', line)
          end   
          -- stRNDEngageSwarm
         local line, selected = ImGui.InputTextWithHint('Engage Swarm', mq.TLO.Macro.Variable('maEntropy').Find('stRNDEngageSwarm').Value(), '', ImGuiInputTextFlags.EnterReturnsTrue)
          if selected then
            mq.cmd.luaedit('stRNDEngageSwarm', line)
          end   
          -- stRNDRezTake
          local line, selected = ImGui.InputTextWithHint('Rez Take', mq.TLO.Macro.Variable('maEntropy').Find('stRNDRezTake').Value(), '', ImGuiInputTextFlags.EnterReturnsTrue)
          if selected then
            mq.cmd.luaedit('stRNDRezTake', line)
          end   
          ImGui.NextColumn()
          -- stRNDEngagePet
          local line, selected = ImGui.InputTextWithHint('Engage Pet', mq.TLO.Macro.Variable('maEntropy').Find('stRNDEngagePet').Value(), '', ImGuiInputTextFlags.EnterReturnsTrue)
          if selected then
            mq.cmd.luaedit('stRNDEngagePet', line)
          end  
          -- stRNDEngageBurn      
          local line, selected = ImGui.InputTextWithHint('Engage Burn', mq.TLO.Macro.Variable('maEntropy').Find('stRNDEngageBurn').Value(), '', ImGuiInputTextFlags.EnterReturnsTrue)
          if selected then
            mq.cmd.luaedit('stRNDEngageBurn', line)
          end   
          -- stRNDRepeat   
          local line, selected = ImGui.InputTextWithHint('Repeat', mq.TLO.Macro.Variable('maEntropy').Find('stRNDRepeat').Value(), '', ImGuiInputTextFlags.EnterReturnsTrue)
          if selected then
            mq.cmd.luaedit('stRNDRepeat', line)
          end   
        ImGui.Columns() 

        ImGui.NewLine()         

        ImGui.Separator()
        ImGui.TextColored(0.39, 0.58, 0.92, 1, 'DanNet')
        ImGui.Columns(2, 'dannet', false)
          -- swLocalEcho
          local _switch, pressed = ImGui.Checkbox("Local Echo", mq.TLO.Macro.Variable('maEntropy').Find('swLocalEcho').Value() == 'TRUE')
          if pressed then
            mq.cmd.luaedit('swLocalEcho', _switch and 'TRUE' or 'FALSE')
          end
          -- swCommandEcho
          local _switch, pressed = ImGui.Checkbox("Command Echo", mq.TLO.Macro.Variable('maEntropy').Find('swCommandEcho').Value() == 'TRUE')
          if pressed then
            mq.cmd.luaedit('swCommandEcho', _switch and 'TRUE' or 'FALSE')
          end
          -- swFrontDelim
          local _switch, pressed = ImGui.Checkbox("Front Delimiter", mq.TLO.Macro.Variable('maEntropy').Find('swFrontDelim').Value() == 'TRUE')
          if pressed then
            mq.cmd.luaedit('swFrontDelim', _switch and 'TRUE' or 'FALSE')
          end
          -- swFullNames
          local _switch, pressed = ImGui.Checkbox("Full Names", mq.TLO.Macro.Variable('maEntropy').Find('swFullNames').Value() == 'TRUE')
          if pressed then
            mq.cmd.luaedit('swFullNames', _switch and 'TRUE' or 'FALSE')
          end
          -- swEvasiveRefresh
          local _switch, pressed = ImGui.Checkbox("Evasive Refresh", mq.TLO.Macro.Variable('maEntropy').Find('swEvasiveRefresh').Value() == 'TRUE')
          if pressed then
            mq.cmd.luaedit('swEvasiveRefresh', _switch and 'TRUE' or 'FALSE')
          end
          ImGui.NextColumn()
          -- stNetworkEvasive
          local line, selected = ImGui.InputTextWithHint('Evasive', mq.TLO.Macro.Variable('maEntropy').Find('stNetworkEvasive').Value(), '', ImGuiInputTextFlags.EnterReturnsTrue)
          if selected then
            mq.cmd.luaedit('stNetworkEvasive', line)
          end           
          -- stNetworkExpired
          local line, selected = ImGui.InputTextWithHint('Expired', mq.TLO.Macro.Variable('maEntropy').Find('stNetworkExpired').Value(), '', ImGuiInputTextFlags.EnterReturnsTrue)
          if selected then
            mq.cmd.luaedit('stNetworkExpired', line)
          end   
          -- stDanNetQueryDelay
          local line, selected = ImGui.InputTextWithHint('Query', mq.TLO.Macro.Variable('maEntropy').Find('stDanNetQueryDelay').Value(), '', ImGuiInputTextFlags.EnterReturnsTrue)
          if selected then
            mq.cmd.luaedit('stDanNetQueryDelay', line)
          end   
          -- stNetworkTimeout
          local line, selected = ImGui.InputTextWithHint('Timeout', mq.TLO.Macro.Variable('maEntropy').Find('stNetworkTimeout').Value(), '', ImGuiInputTextFlags.EnterReturnsTrue)
          if selected then
            mq.cmd.luaedit('stNetworkTimeout', line)
          end 
          -- stEntropyGroup_all
          local line, selected = ImGui.InputTextWithHint('Group', mq.TLO.Macro.Variable('maEntropy').Find('stEntropyGroup_all').Value(), '', ImGuiInputTextFlags.EnterReturnsTrue)
          if selected then
            mq.cmd.luaedit('stEntropyGroup_all', line)
          end 
        ImGui.Columns() 

        ImGui.EndTabItem()
      end

-- ENV TAB
      if ImGui.BeginTabItem('Env') then
        ImGui.NewLine()
        
        
        -- auto:(off) . autoinv:off . begmage:off . .  . . exp:-> . fireworks:off . incharge . loot:-> . 
        --  .  . rad:270 .  . saferadius:30 . takegi:on . takeri:off . 
        
        
-- stFood
-- stDrink


-- swSafeListGuild      

-- stMobLvlMin
-- stMobLvlMax

-- swADVLoot

-- swAutoInv
-- lsAutoInventory

-- stBuffGem     
-- swHoldBuffGem   

-- swCheckTribute

-- swTakeGroupInvite
-- swTakeRaidInvite


        
        
        
        
        
        
        
        
        
        ImGui.EndTabItem()
      end

-- BUFF TAB
      if ImGui.BeginTabItem('Buff') then
      ImGui.NewLine()
      
      ImGui.Separator()
      ImGui.TextColored(0.39, 0.58, 0.92, 1, 'shrink')      
      ImGui.Columns(3, 'noname', false)
        -- shrink
        local _switch, pressed = ImGui.Checkbox("Shrink", mq.TLO.Macro.Variable('maBuff').Find('swBuffShrink').Value() == 'TRUE')
        if pressed then
          mq.cmd.luaedit('swBuffShrink', _switch and 'TRUE' or 'FALSE')
        end 
      ImGui.NextColumn()     
        -- shrink height
        local line, selected = ImGui.InputTextWithHint('Height', mq.TLO.Macro.Variable('maBuff').Find('stShrinkHeight').Value(), '', ImGuiInputTextFlags.EnterReturnsTrue)
        if selected then
          mq.cmd.luaedit('stShrinkHeight', line)
        end
      ImGui.NextColumn()      
        -- shrink Item
        local line, selected = ImGui.InputTextWithHint('Item', mq.TLO.Macro.Variable('maBuff').Find('stShrinkItem').Value(), '', ImGuiInputTextFlags.EnterReturnsTrue)
        if selected then
          mq.cmd.luaedit('stShrinkItem', line)
        end
      ImGui.Columns()      
      
      ImGui.NewLine()
      ImGui.Separator()      
        
      ImGui.Columns(4, 'noname', false)
        -- auras
        local _switch, pressed = ImGui.Checkbox("Aura", mq.TLO.Macro.Variable('maBuff').Find('swBuffAura').Value() == 'TRUE')
        if pressed then
          mq.cmd.luaedit('swBuffAura', _switch and 'TRUE' or 'FALSE')
        end
        -- self
        local _switch, pressed = ImGui.Checkbox("Self", mq.TLO.Macro.Variable('maBuff').Find('swBuffSelf').Value() == 'TRUE')
        if pressed then
          mq.cmd.luaedit('swBuffSelf', _switch and 'TRUE' or 'FALSE')
        end
        -- crew
        local _switch, pressed = ImGui.Checkbox("Crew", mq.TLO.Macro.Variable('maBuff').Find('swBuffCrew').Value() == 'TRUE')
        if pressed then
          mq.cmd.luaedit('swBuffCrew', _switch and 'TRUE' or 'FALSE')
        end
        -- temp
        local _switch, pressed = ImGui.Checkbox("Temp", mq.TLO.Macro.Variable('maBuff').Find('swBuffTemp').Value() == 'TRUE')
        if pressed then
          mq.cmd.luaedit('swBuffTemp', _switch and 'TRUE' or 'FALSE')
        end
        
        ImGui.NextColumn()
                
        -- unity
        local _switch, pressed = ImGui.Checkbox("Unity", mq.TLO.Macro.Variable('maBuff').Find('swBuffUnity').Value() == 'TRUE')
        if pressed then
          mq.cmd.luaedit('swBuffUnity', _switch and 'TRUE' or 'FALSE')
        end
        -- minion
        local _switch, pressed = ImGui.Checkbox("Minion", mq.TLO.Macro.Variable('maMinion').Find('swPetBuff').Value() == 'TRUE')
        if pressed then
          mq.cmd.luaedit('swPetBuff', _switch and 'TRUE' or 'FALSE')
        end
        -- raid
        local _switch, pressed = ImGui.Checkbox("Raid", mq.TLO.Macro.Variable('maBuff').Find('swBuffRaid').Value() == 'TRUE')
        if pressed then
          mq.cmd.luaedit('swBuffRaid', _switch and 'TRUE' or 'FALSE')
        end   
        -- group
        local _switch, pressed = ImGui.Checkbox("Group", mq.TLO.Macro.Variable('maBuff').Find('swBuffGroup').Value() == 'TRUE')
        if pressed then
          mq.cmd.luaedit('swBuffGroup', _switch and 'TRUE' or 'FALSE')
        end   

        ImGui.NextColumn()

        -- now
        local _switch, pressed = ImGui.Checkbox("Now", mq.TLO.Macro.Variable('maBuff').Find('swBuffNow').Value() == 'TRUE')
        if pressed then
          mq.cmd.luaedit('swBuffNow', _switch and 'TRUE' or 'FALSE')
        end        
        -- ammo
        local _switch, pressed = ImGui.Checkbox("Ammo", mq.TLO.Macro.Variable('maBuff').Find('swBuffAmmo').Value() == 'TRUE')
        if pressed then
          mq.cmd.luaedit('swBuffAmmo', _switch and 'TRUE' or 'FALSE')
        end
        -- temp
        local _switch, pressed = ImGui.Checkbox("Beg", mq.TLO.Macro.Variable('maBuff').Find('swBuffBeg').Value() == 'TRUE')
        if pressed then
          mq.cmd.luaedit('swBuffBeg', _switch and 'TRUE' or 'FALSE')
        end

        ImGui.NextColumn()

        -- master
        local _switch, pressed = ImGui.Checkbox("Master", mq.TLO.Macro.Variable('maBuff').Find('swBuffMaster').Value() == 'TRUE')
        if pressed then
          mq.cmd.invoke('${maBuff.Add[swBuffMaster,'..(_switch and 'TRUE' or 'FALSE')..']}')
        end        
                     
        ImGui.Columns()    
        ImGui.NewLine()

        for i=1,50 do 
          ImGui.TextColored(0.39, 0.58, 0.92, 1, i)
          ImGui.SameLine()
          if mq.TLO.Macro.Variable('maBuff').Find('swBuff'..i).Value() == 'TRUE' then
            local _switch, pressed = ImGui.Checkbox("##doswbuff"..i, mq.TLO.Macro.Variable('maBuff').Find('swBuff'..i).Value() == 'TRUE')
            if pressed then
              mq.cmd.luaedit('swBuff'..i, _switch and 'TRUE' or 'FALSE')
            end
            
            ImGui.Indent(16)

            -- spell name
            local line, selected = ImGui.InputTextWithHint('Name', mq.TLO.Macro.Variable('maBuff').Find('stBuff'..i).Value(), '', ImGuiInputTextFlags.EnterReturnsTrue)
            if selected then
              mq.cmd.luaedit('stBuff'..i, line)
            end
            -- alias
            local line, selected = ImGui.InputTextWithHint('Alias', mq.TLO.Macro.Variable('maBuff').Find('stBuff'..i..'Alias').Value(), '', ImGuiInputTextFlags.EnterReturnsTrue)
            if selected then
              mq.cmd.luaedit('stBuff'..i..'Alias', line)
            end
            -- tag
            local line, selectedbufftag = ImGui.InputTextWithHint('Tag', mq.TLO.Macro.Variable('maBuff').Find('lsBuff'..i..'Tag').Value(), '', ImGuiInputTextFlags.EnterReturnsTrue)
            if selectedbufftag then
              mq.cmd.luaedit('lsBuff'..i..'Tag', lsBuff..i..Tag, "overwrite")
            end
            
            ImGui.Indent(-16)
            
          else 
            local _switch, pressed = ImGui.Checkbox("##doswbuff"..i, mq.TLO.Macro.Variable('maBuff').Find('swBuff'..i).Value() == 'TRUE')
            if pressed then
              mq.cmd.luaedit('swBuff'..i, _switch and 'TRUE' or 'FALSE')
            end
          end
        end
        ImGui.EndTabItem()
      end


-- REZ TAB
      if ImGui.BeginTabItem('Rez') then
        ImGui.NewLine()
        ImGui.Columns(2, 'Rez', false)

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
        local _switch, pressed = ImGui.Checkbox("IC", mq.TLO.Macro.Variable('maRez').Find('swRezIC').Value() == 'TRUE')
        if pressed then
          mq.cmd.luaedit('swRezIC', _switch and 'TRUE' or 'FALSE')
        end
        -- out of combat rez
        local _switch, pressed = ImGui.Checkbox("OOC", mq.TLO.Macro.Variable('maRez').Find('swRezOOC').Value() == 'TRUE')
        if pressed then
          mq.cmd.luaedit('swRezOOC', _switch and 'TRUE' or 'FALSE')
        end

        ImGui.NextColumn()

        -- dannet rezing
        local _switch, pressed = ImGui.Checkbox("dannet", mq.TLO.Macro.Variable('maRez').Find('swRezDanNet').Value() == 'TRUE')
        if pressed then
          mq.cmd.luaedit('swRezDanNet', _switch and 'TRUE' or 'FALSE')
        end
        -- rez everyone in range
        local _switch, pressed = ImGui.Checkbox("everyone", mq.TLO.Macro.Variable('maRez').Find('swRezEveryone').Value() == 'TRUE')
        if pressed then
          mq.cmd.luaedit('swRezEveryone', _switch and 'TRUE' or 'FALSE')
        end        
        -- use token
        local _switch, pressed = ImGui.Checkbox("use token", mq.TLO.Macro.Variable('maRez').Find('swRezToken').Value() == 'TRUE')
        if pressed then
          mq.cmd.luaedit('swRezToken', _switch and 'TRUE' or 'FALSE')
        end  
        -- take rezes
        local _switch, pressed = ImGui.Checkbox("take", mq.TLO.Macro.Variable('maRez').Find('swRezTake').Value() == 'TRUE')
        if pressed then
          mq.cmd.luaedit('swRezTake', _switch and 'TRUE' or 'FALSE')
        end  
        -- take call to corpse
        local _switch, pressed = ImGui.Checkbox("take call", mq.TLO.Macro.Variable('maRez').Find('swRezTakeCall').Value() == 'TRUE')
        if pressed then
          mq.cmd.luaedit('swRezTakeCall', _switch and 'TRUE' or 'FALSE')
        end  

        ImGui.Columns()   
        ImGui.NewLine()   
        
      -- lsRezIC
      local line, selected = ImGui.InputTextWithHint('IC Rez', mq.TLO.Macro.Variable('maRez').Find('lsRezIC').Value(), '', ImGuiInputTextFlags.EnterReturnsTrue)
      if selected then
         mq.cmd.luaedit('lsRezIC', line, "overwrite")
      end 
      -- lsRezOOC
      local line, selected = ImGui.InputTextWithHint('OOC Rez', mq.TLO.Macro.Variable('maRez').Find('lsRezOOC').Value(), '', ImGuiInputTextFlags.EnterReturnsTrue)
      if selected then
         mq.cmd.luaedit('lsRezOOC', line, "overwrite")
      end 
        
        


        ImGui.NewLine()   
        ImGui.Separator()
        ImGui.TextColored(0.39, 0.58, 0.92, 1, 'Spawn Search')   

        -- line
        local line, selected = ImGui.InputTextWithHint('Line', mq.TLO.Macro.Variable('maRez').Find('lsLineRez').Value(), '', ImGuiInputTextFlags.EnterReturnsTrue)
        if selected then
           mq.cmd.luaedit('lsLineRez', line, "overwrite")
        end 

        ImGui.NewLine()

        for i=1,8 do 
          ImGui.TextColored(0.39, 0.58, 0.92, 1, i)
          ImGui.SameLine()
          if mq.TLO.Macro.Variable('maRez').Find('swRez'..i).Value() == 'TRUE' then
            local _switch, pressed = ImGui.Checkbox("##sw"..i, mq.TLO.Macro.Variable('maRez').Find('swRez'..i).Value() == 'TRUE')
            if pressed then
              mq.cmd.luaedit('swRez'..i, _switch and 'TRUE' or 'FALSE')
            end

            ImGui.Indent(16)

            -- name
            local _name, selected = ImGui.InputTextWithHint('Spawn', mq.TLO.Macro.Variable('maRez').Find('stRez'..i).Value(), '', ImGuiInputTextFlags.EnterReturnsTrue)
            if selected then
              mq.cmd.luaedit('stRez'..i, _name)
            end
       
            ImGui.Indent(-16)
           
          else 
            local _switch, pressed = ImGui.Checkbox("##sw"..i, mq.TLO.Macro.Variable('maRez').Find('swRez'..i).Value() == 'TRUE')
            if pressed then
              mq.cmd.luaedit('swRez'..i, _switch and 'TRUE' or 'FALSE')
            end
          end 
        end

       
        ImGui.EndTabItem()
      end
 
 -- HEAL TAB
      if ImGui.BeginTabItem('Heal') then
 
        ImGui.NewLine()   

        
        -- line
        local line, selected = ImGui.InputTextWithHint('Line##textentry', mq.TLO.Macro.Variable('maHeal').Find('lsLineHeal').Value(), '', ImGuiInputTextFlags.EnterReturnsTrue)
        if selected then
          mq.cmd.luaedit('lsLineHeal', line, "overwrite")
        end     
        ImGui.NewLine()
           
        ImGui.Columns(2, 'nocurename', false)
        
        for i=1,50 do 
          
          ImGui.TextColored(0.39, 0.58, 0.92, 1, i)
          ImGui.SameLine()
          if mq.TLO.Macro.Variable('maHeal').Find('swHeal'..i).Value() == 'TRUE' then
            local _switch, pressed = ImGui.Checkbox("##doswheal"..i, mq.TLO.Macro.Variable('maHeal').Find('swHeal'..i).Value() == 'TRUE')
            if pressed then
              mq.cmd.luaedit('swHeal'..i, _switch and 'TRUE' or 'FALSE')
            end

            ImGui.Indent(16)

            -- name
            local line, selected = ImGui.InputTextWithHint('Name', mq.TLO.Macro.Variable('maHeal').Find('stHeal'..i).Value(), '', ImGuiInputTextFlags.EnterReturnsTrue)
            if selectedbuff then
              mq.cmd.luaedit('stHeal'..i, line)
            end
            -- tag
            local line, selected = ImGui.InputTextWithHint('Tag', mq.TLO.Macro.Variable('maHeal').Find('lsHeal'..i..'Tag').Value(), '', ImGuiInputTextFlags.EnterReturnsTrue)
            if selected then
              mq.cmd.luaedit('lsHeal'..i..'Tag', line, "overwrite")
            end
       
            ImGui.Indent(-16)
           
          else 
            local _switch, pressed = ImGui.Checkbox("##doswheal"..i, mq.TLO.Macro.Variable('maHeal').Find('swHeal'..i).Value() == 'TRUE')
            if pressed then
              mq.cmd.luaedit('swHeal'..i, _switch and 'TRUE' or 'FALSE')
            end
          end
         
          
        end
        
        ImGui.NextColumn()
        
        
        -- now the loop inside the imgui callback
        for classCode, healPoint in pairs(healPoint) do
          healPoint, used = ImGui.DragInt(classCode..'##healpointslide', healPoint, 1, 0, 99) -- I think this will work, if not I'll show you a way that definitely will work
          if used then mq.cmd.luaedit('stHealPoint'..classCode, healPoint) end
        end

        ImGui.Columns()
      
       
        ImGui.EndTabItem()
      end
      
-- NUKE TAB
      if ImGui.BeginTabItem('Nuke') then
        ImGui.NewLine()

        ImGui.Columns(2, 'nocurename', false)

          -- swNukeLoop
          local _switch, pressed = ImGui.Checkbox("Nuke Loop", mq.TLO.Macro.Variable('maNuke').Find('swNukeLoop').Value() == 'TRUE')
          if pressed then
            mq.cmd.luaedit('swNukeLoop', _switch and 'TRUE' or 'FALSE')
          end
          -- swNukeWhackAMole
          local _switch, pressed = ImGui.Checkbox("Sit", mq.TLO.Macro.Variable('maNuke').Find('swNukeWhackAMole').Value() == 'TRUE')
          if pressed then
            mq.cmd.luaedit('swNukeWhackAMole', _switch and 'TRUE' or 'FALSE')
          end

        ImGui.NextColumn()

          -- stNukeDelay
          local line, selected = ImGui.InputTextWithHint('Delay', mq.TLO.Macro.Variable('maNuke').Find('stNukeDelay').Value(), '', ImGuiInputTextFlags.EnterReturnsTrue)
          if selected then
             mq.cmd.luaedit('stNukeDelay', line)
          end 
          -- stPctStopNuke
          local line, selected = ImGui.InputTextWithHint('Pct Stop', mq.TLO.Macro.Variable('maNuke').Find('stPctStopNuke').Value(), '', ImGuiInputTextFlags.EnterReturnsTrue)
          if selected then
             mq.cmd.luaedit('stPctStopNuke', line)
          end 

        ImGui.Columns()

        ImGui.NewLine()

        -- line
        local line, selected = ImGui.InputTextWithHint('Line', mq.TLO.Macro.Variable('maNuke').Find('lsLineNuke').Value(), '', ImGuiInputTextFlags.EnterReturnsTrue)
        if selected then
           mq.cmd.luaedit('lsLineNuke', line, "overwrite")
        end 

        ImGui.NewLine()

        for i=1,20 do 
          ImGui.TextColored(0.39, 0.58, 0.92, 1, i)
          ImGui.SameLine()
          if mq.TLO.Macro.Variable('maNuke').Find('swNuke'..i).Value() == 'TRUE' then
            local _switch, pressed = ImGui.Checkbox("##sw"..i, mq.TLO.Macro.Variable('maNuke').Find('swNuke'..i).Value() == 'TRUE')
            if pressed then
              mq.cmd.luaedit('swNuke'..i, _switch and 'TRUE' or 'FALSE')
            end

            ImGui.Indent(16)

            -- name
            local _name, selected = ImGui.InputTextWithHint('Name', mq.TLO.Macro.Variable('maNuke').Find('stNuke'..i).Value(), '', ImGuiInputTextFlags.EnterReturnsTrue)
            if selected then
              mq.cmd.luaedit('stNuke'..i, _name)
            end
            -- tag
            local _tag, selected = ImGui.InputTextWithHint('Tag', mq.TLO.Macro.Variable('maNuke').Find('lsNuke'..i..'Tag').Value(), '', ImGuiInputTextFlags.EnterReturnsTrue)
            if selected then
              mq.cmd.luaedit('lsNuke'..i..'Tag', _tag, "overwrite")
            end
       
            ImGui.Indent(-16)
           
          else 
            local _switch, pressed = ImGui.Checkbox("##sw"..i, mq.TLO.Macro.Variable('maNuke').Find('swNuke'..i).Value() == 'TRUE')
            if pressed then
              mq.cmd.luaedit('swNuke'..i, _switch and 'TRUE' or 'FALSE')
            end
          end 
        end

        ImGui.EndTabItem()
      end

-- DOT TAB
      if ImGui.BeginTabItem('DoT') then
        ImGui.NewLine()

        ImGui.Columns(2, 'nocurename', false)

          -- stDotRefresh
          local line, selected = ImGui.InputTextWithHint('Refresh', mq.TLO.Macro.Variable('maDoT').Find('stDotRefresh').Value(), '', ImGuiInputTextFlags.EnterReturnsTrue)
          if selected then
             mq.cmd.luaedit('stDotRefresh', line)
          end 
          -- stPctStopDoT
          local line, selected = ImGui.InputTextWithHint('Pct Stop', mq.TLO.Macro.Variable('maDoT').Find('stPctStopDoT').Value(), '', ImGuiInputTextFlags.EnterReturnsTrue)
          if selected then
             mq.cmd.luaedit('stPctStopDoT', line)
          end 

        ImGui.NextColumn()
        ImGui.Columns()

        ImGui.NewLine()

        -- line
        local line, selected = ImGui.InputTextWithHint('Line', mq.TLO.Macro.Variable('maDoT').Find('lsLineDoT').Value(), '', ImGuiInputTextFlags.EnterReturnsTrue)
        if selected then
           mq.cmd.luaedit('lsLineDoT', line, "overwrite")
        end 

        ImGui.NewLine()

        for i=1,20 do 
          ImGui.TextColored(0.39, 0.58, 0.92, 1, i)
          ImGui.SameLine()
          if mq.TLO.Macro.Variable('maDoT').Find('swDoT'..i).Value() == 'TRUE' then
            local _switch, pressed = ImGui.Checkbox("##sw"..i, mq.TLO.Macro.Variable('maDoT').Find('swDoT'..i).Value() == 'TRUE')
            if pressed then
              mq.cmd.luaedit('swDoT'..i, _switch and 'TRUE' or 'FALSE')
            end

            ImGui.Indent(16)

            -- name
            local _name, selected = ImGui.InputTextWithHint('Name', mq.TLO.Macro.Variable('maDoT').Find('stDoT'..i).Value(), '', ImGuiInputTextFlags.EnterReturnsTrue)
            if selected then
              mq.cmd.luaedit('stDoT'..i, _name)
            end
            -- tag
            local _tag, selected = ImGui.InputTextWithHint('Tag', mq.TLO.Macro.Variable('maDoT').Find('lsDoT'..i..'Tag').Value(), '', ImGuiInputTextFlags.EnterReturnsTrue)
            if selected then
              mq.cmd.luaedit('lsDoT'..i..'Tag', _tag, "overwrite")
            end
       
            ImGui.Indent(-16)
           
          else 
            local _switch, pressed = ImGui.Checkbox("##sw"..i, mq.TLO.Macro.Variable('maDoT').Find('swDoT'..i).Value() == 'TRUE')
            if pressed then
              mq.cmd.luaedit('swDoT'..i, _switch and 'TRUE' or 'FALSE')
            end
          end 
        end

        ImGui.EndTabItem()
      end

-- DEFENSE TAB
      if ImGui.BeginTabItem('Defense') then
        ImGui.NewLine()

        -- count
        local line, selected = ImGui.InputTextWithHint('Count', mq.TLO.Macro.Variable('maDefense').Find('stCountDef').Value(), '', ImGuiInputTextFlags.EnterReturnsTrue)
        if selected then
           mq.cmd.luaedit('stCountDef', line, "overwrite")
        end 
        
        -- pct
        local line, selected = ImGui.InputTextWithHint('Pct', mq.TLO.Macro.Variable('maDefense').Find('stPctDef').Value(), '', ImGuiInputTextFlags.EnterReturnsTrue)
        if selected then
           mq.cmd.luaedit('stPctDef', line, "overwrite")
        end         



        ImGui.NewLine()
        -- line
        local line, selected = ImGui.InputTextWithHint('Line', mq.TLO.Macro.Variable('maDefense').Find('lsLineDefense').Value(), '', ImGuiInputTextFlags.EnterReturnsTrue)
        if selected then
           mq.cmd.luaedit('lsLineDefense', line, "overwrite")
        end 
        
        ImGui.NewLine()

        for i=1,20 do 
          ImGui.TextColored(0.39, 0.58, 0.92, 1, i)
          ImGui.SameLine()
          if mq.TLO.Macro.Variable('maDefense').Find('swDefense'..i).Value() == 'TRUE' then
            local _switch, pressed = ImGui.Checkbox("##sw"..i, mq.TLO.Macro.Variable('maDefense').Find('swDefense'..i).Value() == 'TRUE')
            if pressed then
              mq.cmd.luaedit('swDefense'..i, _switch and 'TRUE' or 'FALSE')
            end

            ImGui.Indent(16)

            -- name
            local _name, selected = ImGui.InputTextWithHint('Name', mq.TLO.Macro.Variable('maDefense').Find('stDefense'..i).Value(), '', ImGuiInputTextFlags.EnterReturnsTrue)
            if selected then
              mq.cmd.luaedit('stDefense'..i, _name)
            end
            -- tag
            local _tag, selected = ImGui.InputTextWithHint('Tag', mq.TLO.Macro.Variable('maDefense').Find('lsDefense'..i..'Tag').Value(), '', ImGuiInputTextFlags.EnterReturnsTrue)
            if selected then
              mq.cmd.luaedit('lsDefense'..i..'Tag', _tag, "overwrite")
            end
       
            ImGui.Indent(-16)
           
          else 
            local _switch, pressed = ImGui.Checkbox("##sw"..i, mq.TLO.Macro.Variable('maDefense').Find('swDefense'..i).Value() == 'TRUE')
            if pressed then
              mq.cmd.luaedit('swDefense'..i, _switch and 'TRUE' or 'FALSE')
            end
          end 
        end

        ImGui.EndTabItem()
      end

-- DEBUFF TAB
      if ImGui.BeginTabItem('Debuff') then
        ImGui.NewLine()
 
        -- line
        local line, selected = ImGui.InputTextWithHint('Line', mq.TLO.Macro.Variable('madeBuff').Find('lsLinedeBuff').Value(), '', ImGuiInputTextFlags.EnterReturnsTrue)
        if selected then
          mq.cmd.luaedit('lsLinedeBuff', line, "overwrite")
        end 
        -- lsdeBuffOrder
        local line, selected = ImGui.InputTextWithHint('Debuff Order', mq.TLO.Macro.Variable('madeBuff').Find('lsdeBuffOrder').Value(), '', ImGuiInputTextFlags.EnterReturnsTrue)
        if selected then
          mq.cmd.luaedit('lsdeBuffOrder', line, "overwrite")
        end         
        
        ImGui.NewLine() 

        ImGui.Columns(2, 'nocurename', false)
          -- cripple:swCripple
          local _switch, pressed = ImGui.Checkbox("Cripple", mq.TLO.Macro.Variable('madeBuff').Find('swCripple').Value() == 'TRUE')
          if pressed then
            mq.cmd.luaedit('swCripple', _switch and 'TRUE' or 'FALSE')
          end
          -- eradicate:wEradicate
          local doswerad, pressed = ImGui.Checkbox("Eradicate", mq.TLO.Macro.Variable('madeBuff').Find('swEradicate').Value() == 'TRUE')
          if pressed then
            mq.cmd.luaedit('swEradicate', doswerad and 'TRUE' or 'FALSE')
          end
          -- malo:swMalo
          local _switch, pressed = ImGui.Checkbox("Malo", mq.TLO.Macro.Variable('madeBuff').Find('swMalo').Value() == 'TRUE')
          if pressed then
            mq.cmd.luaedit('swMalo', _switch and 'TRUE' or 'FALSE')
          end
                    
          ImGui.NextColumn()

          -- slow:swSlow
          local _switch, pressed = ImGui.Checkbox("Slow", mq.TLO.Macro.Variable('madeBuff').Find('swSlow').Value() == 'TRUE')
          if pressed then
            mq.cmd.luaedit('swSlow', _switch and 'TRUE' or 'FALSE')
          end
          -- snare:swSnare
          local _switch, pressed = ImGui.Checkbox("Snare", mq.TLO.Macro.Variable('madeBuff').Find('swSnare').Value() == 'TRUE')
          if pressed then
            mq.cmd.luaedit('swSnare', _switch and 'TRUE' or 'FALSE')
          end          
          -- tash:swTash
          local _switch, pressed = ImGui.Checkbox("Tash", mq.TLO.Macro.Variable('madeBuff').Find('swTash').Value() == 'TRUE')
          if pressed then
            mq.cmd.luaedit('swTash', _switch and 'TRUE' or 'FALSE')
          end
                  
          ImGui.NextColumn()
                  
          -- mez
          -- mez active:swMez
          local _switch, pressed = ImGui.Checkbox("Mez", mq.TLO.Macro.Variable('madeBuff').Find('swMez').Value() == 'TRUE')
          if pressed then
            mq.cmd.luaedit('swMez', _switch and 'TRUE' or 'FALSE')
          end
          -- mez count:stCountMezResist
          local line, selected = ImGui.InputTextWithHint('Mez Resist##text', mq.TLO.Macro.Variable('madeBuff').Find('stCountMezResist').Value(), '', ImGuiInputTextFlags.EnterReturnsTrue)
          if selected then
            mq.cmd.luaedit('stCountMezResist', line, "overwrite")
          end 
          
        ImGui.Columns()

        ImGui.NewLine() 

        for i=1,12 do 
          ImGui.TextColored(0.39, 0.58, 0.92, 1, i)
          ImGui.SameLine()
          if mq.TLO.Macro.Variable('madeBuff').Find('swdeBuff'..i).Value() == 'TRUE' then
            local _switch, pressed = ImGui.Checkbox("##sw"..i, mq.TLO.Macro.Variable('madeBuff').Find('swdeBuff'..i).Value() == 'TRUE')
            if pressed then
              mq.cmd.luaedit('swdeBuff'..i, _switch and 'TRUE' or 'FALSE')
            end

            ImGui.Indent(16)

            -- name
            local _name, selected = ImGui.InputTextWithHint('Name', mq.TLO.Macro.Variable('madeBuff').Find('stdeBuff'..i).Value(), '', ImGuiInputTextFlags.EnterReturnsTrue)
            if selected then
              mq.cmd.luaedit('stdeBuff'..i, _name)
            end
            -- tag
            local _tag, selected = ImGui.InputTextWithHint('Tag', mq.TLO.Macro.Variable('madeBuff').Find('lsdeBuff'..i..'Tag').Value(), '', ImGuiInputTextFlags.EnterReturnsTrue)
            if selected then
              mq.cmd.luaedit('lsdeBuff'..i..'Tag', _tag, "overwrite")
            end
       
            ImGui.Indent(-16)
           
          else 
            local _switch, pressed = ImGui.Checkbox("##sw"..i, mq.TLO.Macro.Variable('madeBuff').Find('swdeBuff'..i).Value() == 'TRUE')
            if pressed then
              mq.cmd.luaedit('swdeBuff'..i, _switch and 'TRUE' or 'FALSE')
            end
          end 
        end


        ImGui.EndTabItem()
      end
      
-- Cure TAB
      if ImGui.BeginTabItem('Cure') then
        ImGui.NewLine() 

        -- line
        local line, selected = ImGui.InputTextWithHint('Line##text', mq.TLO.Macro.Variable('maCure').Find('lsLineCure').Value(), '', ImGuiInputTextFlags.EnterReturnsTrue)
        if selected then
          mq.cmd.luaedit('lsLineCure', line, "overwrite")
        end 
        -- no cure
        local line, selected = ImGui.InputTextWithHint('No Cure##text', mq.TLO.Macro.Variable('maCure').Find('lsNoCure').Value(), '', ImGuiInputTextFlags.EnterReturnsTrue)
        if selected then
          mq.cmd.luaedit('lsNoCure', line, "overwrite")
        end         
        ImGui.NewLine() 
        
        -- cure crew
        local _switch, pressed = ImGui.Checkbox("Crew", mq.TLO.Macro.Variable('maCure').Find('swCureCrew').Value() == 'TRUE')
        if pressed then
          mq.cmd.luaedit('swCureCrew', _switch and 'TRUE' or 'FALSE')
        end
        -- cure group
        local _switch, pressed = ImGui.Checkbox("Group", mq.TLO.Macro.Variable('maCure').Find('swCureGroup').Value() == 'TRUE')
        if pressed then
          mq.cmd.luaedit('swCureGroup', _switch and 'TRUE' or 'FALSE')
        end
        -- cure self
        local _switch, pressed = ImGui.Checkbox("Self", mq.TLO.Macro.Variable('maCure').Find('swCureSelf').Value() == 'TRUE')
        if pressed then
          mq.cmd.luaedit('swCureSelf', _switch and 'TRUE' or 'FALSE')
        end
        

        ImGui.NewLine() 
        

        for i=1,12 do 
          ImGui.TextColored(0.39, 0.58, 0.92, 1, i)
          ImGui.SameLine()
          if mq.TLO.Macro.Variable('maCure').Find('swCure'..i).Value() == 'TRUE' then
            local _switch, pressed = ImGui.Checkbox("##dosw"..i, mq.TLO.Macro.Variable('maCure').Find('swCure'..i).Value() == 'TRUE')
            if pressed then
              mq.cmd.luaedit('swCure'..i, _switch and 'TRUE' or 'FALSE')
            end

            ImGui.Indent(16)

            -- name
            local cure, selected = ImGui.InputTextWithHint('Name', mq.TLO.Macro.Variable('maCure').Find('stCure'..i).Value(), '', ImGuiInputTextFlags.EnterReturnsTrue)
            if selected then
              mq.cmd.luaedit('stCure'..i, cure)
            end
            -- tag
            local _tag, selected = ImGui.InputTextWithHint('Tag', mq.TLO.Macro.Variable('maCure').Find('lsCure'..i..'Tag').Value(), '', ImGuiInputTextFlags.EnterReturnsTrue)
            if selected then
              mq.cmd.luaedit('lsCure'..i..'Tag', _tag, "overwrite")
            end
       
            ImGui.Indent(-16)
           
          else 
            local _switch, pressed = ImGui.Checkbox("##doswheal"..i, mq.TLO.Macro.Variable('maCure').Find('swCure'..i).Value() == 'TRUE')
            if pressed then
              mq.cmd.luaedit('swCure'..i, _switch and 'TRUE' or 'FALSE')
            end
          end
          
        end
        ImGui.EndTabItem()
      end
      
      
-- MELEE TAB
      if ImGui.BeginTabItem('Melee') then
        ImGui.NewLine()

        for i=1,12 do 
          ImGui.TextColored(0.39, 0.58, 0.92, 1, i)
          ImGui.SameLine()
          if mq.TLO.Macro.Variable('maMelee').Find('swMelee'..i).Value() == 'TRUE' then
            local _switch, pressed = ImGui.Checkbox("##sw"..i, mq.TLO.Macro.Variable('maMelee').Find('swMelee'..i).Value() == 'TRUE')
            if pressed then
              mq.cmd.luaedit('swMelee'..i, _switch and 'TRUE' or 'FALSE')
            end

            ImGui.Indent(16)

            -- name
            local _name, selected = ImGui.InputTextWithHint('Name', mq.TLO.Macro.Variable('maMelee').Find('stMelee'..i).Value(), '', ImGuiInputTextFlags.EnterReturnsTrue)
            if selected then
              mq.cmd.luaedit('stMelee'..i, _name)
            end
            -- tag
            local _tag, selected = ImGui.InputTextWithHint('Tag', mq.TLO.Macro.Variable('maMelee').Find('lsMelee'..i..'Tag').Value(), '', ImGuiInputTextFlags.EnterReturnsTrue)
            if selected then
              mq.cmd.luaedit('lsMelee'..i..'Tag', _tag, "overwrite")
            end
       
            ImGui.Indent(-16)
           
          else 
            local _switch, pressed = ImGui.Checkbox("##sw"..i, mq.TLO.Macro.Variable('maMelee').Find('swMelee'..i).Value() == 'TRUE')
            if pressed then
              mq.cmd.luaedit('swMelee'..i, _switch and 'TRUE' or 'FALSE')
            end
          end 
        end

        ImGui.EndTabItem()
      end      

-- OVERRIDE TAB
      if ImGui.BeginTabItem('Override') then
        ImGui.NewLine()
        
        -- swOverLOS          
        local _switch, pressed = ImGui.Checkbox("LoS", mq.TLO.Macro.Variable('maOver').Find('swOverLOS').Value() == 'TRUE')
        if pressed then
          mq.cmd.luaedit('swOverLOS', _switch and 'TRUE' or 'FALSE')
        end
        -- swOverPctEngage    
        local _switch, pressed = ImGui.Checkbox("Pct Engage", mq.TLO.Macro.Variable('maOver').Find('swOverPctEngage').Value() == 'TRUE')
        if pressed then
          mq.cmd.luaedit('swOverPctEngage', _switch and 'TRUE' or 'FALSE')
        end
        -- swOverLoot         
        local _switch, pressed = ImGui.Checkbox("Loot", mq.TLO.Macro.Variable('maOver').Find('swOverLoot').Value() == 'TRUE')
        if pressed then
          mq.cmd.luaedit('swOverLoot', _switch and 'TRUE' or 'FALSE')
        end
        -- swOverSafeNames    
        local _switch, pressed = ImGui.Checkbox("Safe Names", mq.TLO.Macro.Variable('maOver').Find('swOverSafeNames').Value() == 'TRUE')
        if pressed then
          mq.cmd.luaedit('swOverSafeNames', _switch and 'TRUE' or 'FALSE')
        end
        -- swOverTrainSpell   
        local _switch, pressed = ImGui.Checkbox("Train Spell", mq.TLO.Macro.Variable('maOver').Find('swOverTrainSpell').Value() == 'TRUE')
        if pressed then
          mq.cmd.luaedit('swOverTrainSpell', _switch and 'TRUE' or 'FALSE')
        end
        -- swOverEnvAuto      
        local _switch, pressed = ImGui.Checkbox("Env Auto", mq.TLO.Macro.Variable('maOver').Find('swOverEnvAuto').Value() == 'TRUE')
        if pressed then
          mq.cmd.luaedit('swOverEnvAuto', _switch and 'TRUE' or 'FALSE')
        end
        -- swOverDSCheck      
        local _switch, pressed = ImGui.Checkbox("DS Check", mq.TLO.Macro.Variable('maOver').Find('swOverDSCheck').Value() == 'TRUE')
        if pressed then
          mq.cmd.luaedit('swOverDSCheck', _switch and 'TRUE' or 'FALSE')
        end
        -- swOverNavCheck     
        local _switch, pressed = ImGui.Checkbox("NAV Check", mq.TLO.Macro.Variable('maOver').Find('swOverNavCheck').Value() == 'TRUE')
        if pressed then
          mq.cmd.luaedit('swOverNavCheck', _switch and 'TRUE' or 'FALSE')
        end
        -- swOverGroupAss     
        local _switch, pressed = ImGui.Checkbox("Group Ass", mq.TLO.Macro.Variable('maOver').Find('swOverGroupAss').Value() == 'TRUE')
        if pressed then
          mq.cmd.luaedit('swOverGroupAss', _switch and 'TRUE' or 'FALSE')
        end
        -- swOverSplashHurt   
        local _switch, pressed = ImGui.Checkbox("Splash Hurt", mq.TLO.Macro.Variable('maOver').Find('swOverSplashHurt').Value() == 'TRUE')
        if pressed then
          mq.cmd.luaedit('swOverSplashHurt', _switch and 'TRUE' or 'FALSE')
        end
        -- swTargetClear      
        local _switch, pressed = ImGui.Checkbox("Target Clear", mq.TLO.Macro.Variable('maOver').Find('swTargetClear').Value() == 'TRUE')
        if pressed then
          mq.cmd.luaedit('swTargetClear', _switch and 'TRUE' or 'FALSE')
        end
        
        ImGui.EndTabItem()
      end 


      ImGui.EndTabBar()
      
      
      
    end
    ImGui.End()
  end
end

mq.imgui.init('editorwindow', imguicallback)

while openGUI do 
  mq.delay(1000) 
  updateHealPoints()
end







 



