--
-- entropoy.mac
-- build.lua
-- 
-- character building
--



require 'common'
local openGUI = true
local shouldDrawHUD = true


healPoint = {}
local function updateHealPoints()
  for _, classCode in pairs(classTable) do
    if healPoint[classCode] and healPoint[classCode].updated then
      mq.cmd.luaedit('stHealPoint'..classCode, healPoint[classCode].value)
      healPoint[classCode].updated = false
    else
      if not healPoint[classCode] then healPoint[classCode] = {} end
      healPoint[classCode]['value'] = tonumber(mq.TLO.Macro.Variable('maHeal').Find('stHealPoint'..classCode).Value())
      healPoint[classCode]['updated'] = false
    end
  end
end


chrTable = {}
chrTable['CLR'] = chr_CLR




local classOutput = function (classID)
  if classID == 2 then -- cleric
    indent(1,1)
    ImGui.Columns(3, 'class', false)
      edit_text_perm('issuance', 'maChr', 'stCountIssuance')
      edit_text_perm('vp', 'maChr', 'stPctVP')
    ImGui.NextColumn()
      edit_text_perm('beacon', 'maChr', 'stCountBeaconofLife')
    ImGui.NextColumn()
    ImGui.Columns()     
    indent(1,2)
    ImGui.NewLine() 
    
    if ImGui.TreeNode('yaulp') then    
      ImGui.NewLine()   
      ImGui.Columns(2, 'yaulp', false)
        edit_switch_perm('yaulp', 'maChr', 'swYaulp')
      ImGui.NextColumn()
        edit_text_perm('yaulp', 'maChr', 'stYaulp')
      ImGui.Columns()    
      ImGui.NewLine()   
      ImGui.TreePop()      
    end    
    if ImGui.TreeNode('arbitration') then    
      ImGui.NewLine()   
      ImGui.Columns(2, 'arb', false)
        edit_switch_perm('aa', 'maChr', 'swArbitrateAA')
        edit_switch_perm('epic', 'maChr', 'swArbitrateEpic')
      ImGui.NextColumn()
        edit_text_perm('count', 'maChr', 'stCountArbitrate')
        edit_text_perm('pause', 'maChr', 'stArbitratePause')
      ImGui.Columns()    
      ImGui.NewLine()   
      ImGui.TreePop()      
    end   

    
     
  end

end


      


local function imguicallback()
  hudInfo()
  
  openGUI, shouldDrawHUD = ImGui.Begin('build###EntropyEditor', openGUI)
  
  if shouldDrawHUD and ent.build == '--' then
    ImGui.Text('Entropy is not running')
    ImGui.End()


  elseif shouldDrawHUD then  
    ImGui.NewLine() 
    cmd_button('start hud', 70, 22, 'entropy hud start')
    ImGui.NewLine()  
    
  -- AGRO
    if ImGui.CollapsingHeader('/agro') then
      ImGui.NewLine()
      indent(1,1)

      ImGui.Columns(2, 'agrothingies', false)
        edit_switch_perm('agro', 'maAgro', 'swAgro')
        edit_text_perm('hold', 'maAgro', 'stPctAgroHold')
      ImGui.NextColumn()
        edit_text_perm('taunt', 'maAgro', 'stPctTaunt')
      ImGui.Columns()  
      
      indent(1,2)
      ImGui.NewLine()
      
        if ImGui.TreeNode('list') then 
          ImGui.Indent(16)
          
          -- line
          local line, selected = ImGui.InputTextWithHint('line##lsLineAgro', mq.TLO.Macro.Variable('maAgro').Find('lsLineAgro').Value(), '', ImGuiInputTextFlags.EnterReturnsTrue)
          if selected then
             mq.cmd.luaedit('lsLineAgro', line, 'overwrite')
          end 
          ImGui.Indent(-16)
          ImGui.NewLine()
          for i=1,20 do 
            ImGui.TextColored(0.39, 0.58, 0.92, 1, i)
            ImGui.SameLine()
            if mq.TLO.Macro.Variable('maAgro').Find('swAgro'..i).Value() == 'TRUE' then
              local _switch, pressed = ImGui.Checkbox('##swAgro'..i, mq.TLO.Macro.Variable('maAgro').Find('swAgro'..i).Value() == 'TRUE')
              if pressed then
                mq.cmd.luaedit('swAgro'..i, _switch and 'TRUE' or 'FALSE')
              end
              
              indent(i,1)

              -- name
              local _name, selected = ImGui.InputTextWithHint('name##stAgro'..i, mq.TLO.Macro.Variable('maAgro').Find('stAgro'..i).Value(), '', ImGuiInputTextFlags.EnterReturnsTrue)
              if selected then
                mq.cmd.luaedit('stAgro'..i, _name)
              end
              -- tag
              local _tag, selected = ImGui.InputTextWithHint('tag##lsAgro'..i, mq.TLO.Macro.Variable('maAgro').Find('lsAgro'..i..'Tag').Value(), '', ImGuiInputTextFlags.EnterReturnsTrue)
              if selected then
                mq.cmd.luaedit('lsAgro'..i..'Tag', _tag, 'overwrite')
              end
              
              indent(i,2)
             
            else 
              if mq.TLO.Macro.Variable('maAgro').Find('stAgro'..i).Value() == 'FALSE' then
                tmpName = '##swAgro'..i
              else
                tmpName = mq.TLO.Macro.Variable('maAgro').Find('stAgro'..i).Value()..'##swAgro'..i
              end    

              local _switch, pressed = ImGui.Checkbox(tmpName, mq.TLO.Macro.Variable('maAgro').Find('swAgro'..i).Value() == 'TRUE')
              if pressed then
                mq.cmd.luaedit('swAgro'..i, _switch and 'TRUE' or 'FALSE')
              end
            end 
          end
          ImGui.TreePop()
        end

      ImGui.NewLine()
    end

  -- BUFF
    if ImGui.CollapsingHeader('/buff') then
      ImGui.NewLine()
      indent(1,1)
      ImGui.Columns(3, 'noname', false)
        edit_switch_perm('aura', 'maBuff', 'swBuffAura')
        edit_switch_perm('self', 'maBuff', 'swBuffSelf')
        edit_switch_perm('crew', 'maBuff', 'swBuffCrew')
        edit_switch_perm('temp', 'maBuff', 'swBuffTemp')
      ImGui.NextColumn()
        edit_switch_perm('unity', 'maBuff', 'swBuffUnity')
        edit_switch_perm('minion', 'maMinion', 'swPetBuff')
        edit_switch_perm('raid', 'maBuff', 'swBuffRaid')
        edit_switch_perm('group', 'maBuff', 'swBuffGroup')
      ImGui.NextColumn()
        edit_switch_perm('now', 'maBuff', 'swBuffNow')
        edit_switch_perm('ammo', 'maBuff', 'swBuffAmmo')
        edit_switch_perm('beg', 'maBuff', 'swBuffBeg')
        edit_switch_perm('nolobby', 'maBuff', 'swBuffNoLobby')
      ImGui.Columns()  
      indent(1,2)  
      ImGui.NewLine()
      
            
 
      if ImGui.TreeNode('shrink') then
        ImGui.NewLine()
        ImGui.Columns(3, 'shrink', false)
          edit_switch_perm('shrink', 'maBuff', 'swBuffShrink')
        ImGui.NextColumn()     
          edit_text_perm('height', 'maBuff', 'stShrinkHeight')
        ImGui.NextColumn()
        ImGui.Columns() 
        edit_text_perm('item', 'maBuff', 'stShrinkItem')
        ImGui.NewLine()
        ImGui.TreePop()      
      end

      if ImGui.TreeNode('list') then
        ImGui.NewLine()
        for i=1,50 do 
          ImGui.TextColored(0.39, 0.58, 0.92, 1, i)
          ImGui.SameLine()
          if mq.TLO.Macro.Variable('maBuff').Find('swBuff'..i).Value() == 'TRUE' then
            local _switch, pressed = ImGui.Checkbox('##swBuff'..i, mq.TLO.Macro.Variable('maBuff').Find('swBuff'..i).Value() == 'TRUE')
            if pressed then
              mq.cmd.luaedit('swBuff'..i, _switch and 'TRUE' or 'FALSE')
            end
            
            indent(i,1)

            -- spell name
            local line, selected = ImGui.InputTextWithHint('name##stBuff'..i, mq.TLO.Macro.Variable('maBuff').Find('stBuff'..i).Value(), '', ImGuiInputTextFlags.EnterReturnsTrue)
            if selected then
              mq.cmd.luaedit('stBuff'..i, line)
            end
            -- alias
            local line, selected = ImGui.InputTextWithHint('alias##stBuff'..i..'Alias', mq.TLO.Macro.Variable('maBuff').Find('stBuff'..i..'Alias').Value(), '', ImGuiInputTextFlags.EnterReturnsTrue)
            if selected then
              mq.cmd.luaedit('stBuff'..i..'Alias', line)
            end
            -- tag
            local line, selectedbufftag = ImGui.InputTextWithHint('tag##lsBuff'..i..'Tag', mq.TLO.Macro.Variable('maBuff').Find('lsBuff'..i..'Tag').Value(), '', ImGuiInputTextFlags.EnterReturnsTrue)
            if selectedbufftag then
              -- mq.cmd.luaedit('lsBuff'..i..'Tag', lsBuff..i..Tag, 'overwrite')
              mq.cmd.luaedit('lsBuff'..i..'Tag', line, 'overwrite')
            end
            
            indent(i,2)
            
          else 
            if mq.TLO.Macro.Variable('maBuff').Find('stBuff'..i).Value() == 'FALSE' then
              tmpName = '##swBuff'..i
            else
              tmpName = mq.TLO.Macro.Variable('maBuff').Find('stBuff'..i).Value()..'##swBuff'..i
            end            
            
            local _switch, pressed = ImGui.Checkbox(tmpName, mq.TLO.Macro.Variable('maBuff').Find('swBuff'..i).Value() == 'TRUE')
            if pressed then
              mq.cmd.luaedit('swBuff'..i, _switch and 'TRUE' or 'FALSE')
            end
          end
        end
        ImGui.TreePop()      
      end

      ImGui.NewLine()
    end

  -- BURN
    if ImGui.CollapsingHeader('/burn') then
      ImGui.NewLine()
      indent(1,1)

        edit_switch_perm('force', 'maBurn', 'swBurnForce')
        edit_switch_perm('auto', 'maBurn', 'swBurnAuto')
        edit_switch_perm('raid', 'maBurn', 'swBurnRaid')
        edit_text_perm('engage', 'maBurn', 'stPctBurnEngage')
        edit_text_perm('count', 'maBurn', 'stBurnCount')
        -- edit_text_perm('line', 'maBurn', 'lsLineBurn')


      indent(1,2)
      ImGui.NewLine()
    end

  -- CC
    if ImGui.CollapsingHeader('/cc') then
      ImGui.NewLine() 
      
      indent(1,1)
        ImGui.Columns(2, 'noname', false)
          edit_switch_perm('dot', 'maCC', 'swCombatDoT')
          edit_switch_perm('heal', 'maCC', 'swCombatHeal')
          edit_switch_perm('melee', 'maCC', 'swCombatMelee')
          edit_switch_perm('range', 'maCC', 'swCombatRange')
          edit_switch_perm('nuke', 'maCC', 'swCombatNuke')
          edit_switch_perm('crowd control', 'maCC', 'swCombatControl')
        ImGui.NextColumn()
          edit_text_perm('pct engage', 'maCC', 'stPctEngage')
          edit_text_perm('z radius', 'maCC', 'stZRadius')
        ImGui.Columns()

      ImGui.NewLine() 

      if ImGui.TreeNode('assist') then
        ImGui.NewLine()
        ImGui.Columns(2, 'assistcol', false)
          edit_switch_perm('force manual', 'maCC', 'swManualAssist')
          edit_switch_perm('stop on ds', 'maCC', 'swStoponDS')
        
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



        ImGui.NewLine()
        ImGui.Columns()

        ImGui.NewLine()
        edit_text_perm('assist line', 'maCC', 'lsAss')
        ImGui.NewLine()
        ImGui.TreePop()      
      end



      if ImGui.TreeNode('glyph') then
        ImGui.NewLine()
        ImGui.Columns(2, 'glyphtree', false)
          edit_switch_perm('group', 'maCC', 'swGlyphGroup')
          edit_switch_perm('raid', 'maCC', 'swGlyphRaid')
        ImGui.NextColumn()     
          edit_text_perm('glyph', 'maCC', 'stGlyph')
        ImGui.Columns() 
        ImGui.NewLine()
        ImGui.TreePop()      
      end


      
      ImGui.NewLine() 
      
        edit_text_perm('no cast with', 'maCC', 'lsNoCastWith')
        edit_text_perm('no melee with', 'maCC', 'lsNoMeleeWith')
      
      ImGui.NewLine() 
      
      indent(1,2)











    end

  -- CHR
    if ImGui.CollapsingHeader('/chr') then
      ImGui.NewLine() 
      
      indent(1,1)
      ImGui.Columns(4, 'spire', false)
        edit_text_perm('ae', 'maChr', 'stCountAE')
      ImGui.NextColumn()
        edit_text_perm('fade', 'maChr', 'stPctFade')      
      ImGui.NextColumn()
      ImGui.NextColumn()
      ImGui.Columns()      
      ImGui.NewLine() 
      indent(1,2)
      
      classOutput(mq.TLO.Me.Class.ID())
      
      local classoutput = chrTable[mq.TLO.Me.Class.ShortName()]
      if classoutput ~= nil then 
        edit_text_perm('issuance', 'maChr', 'stCountIssuance')
        classoutput() 
        ImGui.NewLine() 
      end

      if ImGui.TreeNode('intensity of the resolute') then
        ImGui.NewLine()   
        ImGui.Columns(4, 'spire', false)
          edit_switch_perm('group', 'maChr', 'swIntensityGroup')
        ImGui.NextColumn()
          edit_switch_perm('raid', 'maChr', 'swIntensityRaid')
        ImGui.NextColumn()
        ImGui.Columns()
        edit_text_perm('name', 'maChr', 'stAAIntensity')      
        ImGui.TreePop()      
        ImGui.NewLine()   
      end  

      if ImGui.TreeNode('spire') then
        ImGui.NewLine()   
        ImGui.Columns(4, 'spire', false)
          edit_switch_perm('group', 'maChr', 'swSpireGroup')
        ImGui.NextColumn()
          edit_switch_perm('raid', 'maChr', 'swSpireRaid')
        ImGui.NextColumn()
        ImGui.NextColumn()
        ImGui.Columns()
        ImGui.NewLine()   
        ImGui.TreePop()      
      end  

      if ImGui.TreeNode('invis') then
        ImGui.NewLine()   
        ImGui.Columns(4, 'colinvis', false)
          edit_text_perm('delay', 'maChr', 'stInvisDelay')
        ImGui.NextColumn()
        ImGui.NextColumn()
        ImGui.NextColumn()
        ImGui.Columns()
        edit_text_perm('self invis', 'maChr', 'stInvisSingle')
        edit_text_perm('group invis', 'maChr', 'stInvisGroup')
        edit_text_perm('self IVU', 'maChr', 'stInvisSingleIVU')
        edit_text_perm('group IVU', 'maChr', 'stInvisGroupIVU')
        ImGui.NewLine()   
        ImGui.TreePop()      
      end  
      
      if ImGui.TreeNode('class orders') then
        ImGui.NewLine()   
        edit_text_perm('base', 'maChr', 'lsOrderClassBase')
        edit_text_perm('pre', 'maChr', 'lsOrderClassPre')
        edit_text_perm('combat', 'maChr', 'lsOrderClass')
        edit_text_perm('post', 'maChr', 'lsOrderClassPost')
        ImGui.TreePop()      
      end  


      
      ImGui.NewLine()       
      indent(1,1)
      edit_text_perm('cast order', 'maChr', 'lsOrderCastType')
      indent(1,2)
          
      ImGui.NewLine()       
    end        

  -- CLICKITEM
    if ImGui.CollapsingHeader('/clickitem') then

      ImGui.NewLine()
      indent(1, 1)      
      -- line
      local line, selected = ImGui.InputTextWithHint('line##lsLineDefense', mq.TLO.Macro.Variable('maDefense').Find('lsLineDefense').Value(), '', ImGuiInputTextFlags.EnterReturnsTrue)
      if selected then
         mq.cmd.luaedit('lsLineDefense', line, 'overwrite')
      end 
      indent(1, 2)
      
      ImGui.NewLine()

      for i=1,20 do 
        ImGui.TextColored(0.39, 0.58, 0.92, 1, i)
        ImGui.SameLine()
        if mq.TLO.Macro.Variable('maItem').Find('swItem'..i).Value() == 'TRUE' then
          local _switch, pressed = ImGui.Checkbox('##swItem'..i, mq.TLO.Macro.Variable('maItem').Find('swItem'..i).Value() == 'TRUE')
          if pressed then
            mq.cmd.luaedit('swItem'..i, _switch and 'TRUE' or 'FALSE')
          end
          indent(i, 1)
          -- name
          local _name, selected = ImGui.InputTextWithHint('Name##stItem'..i, mq.TLO.Macro.Variable('maItem').Find('stItem'..i).Value(), '', ImGuiInputTextFlags.EnterReturnsTrue)
          if selected then
            mq.cmd.luaedit('stDefense'..i, _name)
          end
          -- tag
          local _tag, selected = ImGui.InputTextWithHint('Tag##lsItem'..i, mq.TLO.Macro.Variable('maItem').Find('lsItem'..i..'Tag').Value(), '', ImGuiInputTextFlags.EnterReturnsTrue)
          if selected then
            mq.cmd.luaedit('lsItem'..i..'Tag', _tag, 'overwrite')
          end
          indent(i, 0)
         
        else 
          if mq.TLO.Macro.Variable('maItem').Find('stItem'..i).Value() == 'FALSE' then
            tmpName = '##swItem'..i
          else
            tmpName = mq.TLO.Macro.Variable('maItem').Find('stItem'..i).Value()..'##swItem'..i
          end    

          local _switch, pressed = ImGui.Checkbox(tmpName, mq.TLO.Macro.Variable('maItem').Find('swItem'..i).Value() == 'TRUE')
          if pressed then
            mq.cmd.luaedit('swItem'..i, _switch and 'TRUE' or 'FALSE')
          end
        end 
      end
      ImGui.NewLine()
    end

  -- CURE
    if ImGui.CollapsingHeader('/cure') then
      ImGui.NewLine() 
      indent(1,1)

      -- no cure
      local line, selected = ImGui.InputTextWithHint('no cure##lsNoCure', mq.TLO.Macro.Variable('maCure').Find('lsNoCure').Value(), '', ImGuiInputTextFlags.EnterReturnsTrue)
      if selected then
        mq.cmd.luaedit('lsNoCure', line, 'overwrite')
      end         
      ImGui.NewLine() 
      
      -- cure crew
      local _switch, pressed = ImGui.Checkbox('crew##swCureCrew', mq.TLO.Macro.Variable('maCure').Find('swCureCrew').Value() == 'TRUE')
      if pressed then
        mq.cmd.luaedit('swCureCrew', _switch and 'TRUE' or 'FALSE')
      end
      -- cure group
      local _switch, pressed = ImGui.Checkbox('group##swCureGroup', mq.TLO.Macro.Variable('maCure').Find('swCureGroup').Value() == 'TRUE')
      if pressed then
        mq.cmd.luaedit('swCureGroup', _switch and 'TRUE' or 'FALSE')
      end
      -- cure self
      local _switch, pressed = ImGui.Checkbox('self##swCureSelf', mq.TLO.Macro.Variable('maCure').Find('swCureSelf').Value() == 'TRUE')
      if pressed then
        mq.cmd.luaedit('swCureSelf', _switch and 'TRUE' or 'FALSE')
      end
      

      ImGui.NewLine() 
      indent(1,2)

      if ImGui.TreeNode('list') then      
        ImGui.NewLine() 
        -- line
        local line, selected = ImGui.InputTextWithHint('line##lsLineCure', mq.TLO.Macro.Variable('maCure').Find('lsLineCure').Value(), '', ImGuiInputTextFlags.EnterReturnsTrue)
        if selected then
          mq.cmd.luaedit('lsLineCure', line, 'overwrite')
        end 
        ImGui.NewLine() 
        
        for i=1,12 do 
          ImGui.TextColored(0.39, 0.58, 0.92, 1, i)
          ImGui.SameLine()
          if mq.TLO.Macro.Variable('maCure').Find('swCure'..i).Value() == 'TRUE' then
            local _switch, pressed = ImGui.Checkbox('##swCure'..i, mq.TLO.Macro.Variable('maCure').Find('swCure'..i).Value() == 'TRUE')
            if pressed then
              mq.cmd.luaedit('swCure'..i, _switch and 'TRUE' or 'FALSE')
            end

            indent(i,1)

            -- name
            local cure, selected = ImGui.InputTextWithHint('name##stCure'..i, mq.TLO.Macro.Variable('maCure').Find('stCure'..i).Value(), '', ImGuiInputTextFlags.EnterReturnsTrue)
            if selected then
              mq.cmd.luaedit('stCure'..i, cure)
            end
            -- tag
            local _tag, selected = ImGui.InputTextWithHint('tag##lsCure'..i, mq.TLO.Macro.Variable('maCure').Find('lsCure'..i..'Tag').Value(), '', ImGuiInputTextFlags.EnterReturnsTrue)
            if selected then
              mq.cmd.luaedit('lsCure'..i..'Tag', _tag, 'overwrite')
            end
       
            indent(i,2)
           
          else 
            
            if mq.TLO.Macro.Variable('maCure').Find('stCure'..i).Value() == 'FALSE' then
              tmpName = '##swCure'..i
            else
              tmpName = mq.TLO.Macro.Variable('maCure').Find('stCure'..i).Value()..'##swCure'..i
            end 
              
            local _switch, pressed = ImGui.Checkbox(tmpName, mq.TLO.Macro.Variable('maCure').Find('swCure'..i).Value() == 'TRUE')
            if pressed then
              mq.cmd.luaedit('swCure'..i, _switch and 'TRUE' or 'FALSE')
            end
          end
          
        end
        ImGui.NewLine()
        ImGui.TreePop()
      end
       ImGui.NewLine() 
   end

  -- DOT
    if ImGui.CollapsingHeader('/dot') then
      ImGui.NewLine()
      ImGui.Indent(16)

      ImGui.Columns(2, 'nocurename', false)

        -- stDotRefresh
        local line, selected = ImGui.InputTextWithHint('refresh##stDotRefresh', mq.TLO.Macro.Variable('maDoT').Find('stDotRefresh').Value(), '', ImGuiInputTextFlags.EnterReturnsTrue)
        if selected then
           mq.cmd.luaedit('stDotRefresh', line)
        end 
        -- stPctStopDoT
        local line, selected = ImGui.InputTextWithHint('pct stop##stPctStopDoT', mq.TLO.Macro.Variable('maDoT').Find('stPctStopDoT').Value(), '', ImGuiInputTextFlags.EnterReturnsTrue)
        if selected then
           mq.cmd.luaedit('stPctStopDoT', line)
        end 

      ImGui.NextColumn()
      ImGui.Columns()
      ImGui.Indent(-16)

      ImGui.NewLine()
      if ImGui.TreeNode('list') then
        ImGui.NewLine()

        -- line
        local line, selected = ImGui.InputTextWithHint('line##lsLineDoT', mq.TLO.Macro.Variable('maDoT').Find('lsLineDoT').Value(), '', ImGuiInputTextFlags.EnterReturnsTrue)
        if selected then
           mq.cmd.luaedit('lsLineDoT', line, 'overwrite')
        end 

        ImGui.NewLine()

        for i=1,20 do 
          ImGui.TextColored(0.39, 0.58, 0.92, 1, i)
          ImGui.SameLine()
          if mq.TLO.Macro.Variable('maDoT').Find('swDoT'..i).Value() == 'TRUE' then
            local _switch, pressed = ImGui.Checkbox('##swDoT'..i, mq.TLO.Macro.Variable('maDoT').Find('swDoT'..i).Value() == 'TRUE')
            if pressed then
              mq.cmd.luaedit('swDoT'..i, _switch and 'TRUE' or 'FALSE')
            end

            indent(i,1)

            -- name
            local _name, selected = ImGui.InputTextWithHint('name##stDoT', mq.TLO.Macro.Variable('maDoT').Find('stDoT'..i).Value(), '', ImGuiInputTextFlags.EnterReturnsTrue)
            if selected then
              mq.cmd.luaedit('stDoT'..i, _name)
            end
            -- tag
            local _tag, selected = ImGui.InputTextWithHint('tag##lsDoT', mq.TLO.Macro.Variable('maDoT').Find('lsDoT'..i..'Tag').Value(), '', ImGuiInputTextFlags.EnterReturnsTrue)
            if selected then
              mq.cmd.luaedit('lsDoT'..i..'Tag', _tag, 'overwrite')
            end
       
            indent(i,2)
           
          else 
            if mq.TLO.Macro.Variable('maDoT').Find('stDoT'..i).Value() == 'FALSE' then
              tmpName = '##swDoT'..i
            else
              tmpName = mq.TLO.Macro.Variable('maDoT').Find('stDoT'..i).Value()..'##swDoT'..i
            end      
                        
            local _switch, pressed = ImGui.Checkbox(tmpName, mq.TLO.Macro.Variable('maDoT').Find('swDoT'..i).Value() == 'TRUE')
            if pressed then
              mq.cmd.luaedit('swDoT'..i, _switch and 'TRUE' or 'FALSE')
            end
          end 
        end
      end
      ImGui.NewLine()
    end

  -- DEBUFF
    if ImGui.CollapsingHeader('/debuff') then
      ImGui.NewLine()
      indent(1,1)
      edit_text_perm('order', 'madeBuff', 'lsdeBuffOrder')
      ImGui.NewLine() 

      ImGui.Columns(4, 'debuffathing', false)
        edit_switch_perm('cripple', 'madeBuff', 'swCripple')
        edit_switch_perm('eradicate', 'madeBuff', 'swEradicate')
        edit_switch_perm('malo', 'madeBuff', 'swMalo')
      ImGui.NextColumn()
        edit_switch_perm('slow', 'madeBuff', 'swSlow')
        edit_switch_perm('snare', 'madeBuff', 'swSnare')
        edit_switch_perm('tash', 'madeBuff', 'swTash')
      ImGui.NextColumn()
        edit_switch_perm('mez', 'madeBuff', 'swMez')
        edit_text_perm('mez resist', 'madeBuff', 'stCountMezResist')
      ImGui.NextColumn()
      ImGui.Columns()
      ImGui.NewLine()      
      indent(1,2)

      if ImGui.TreeNode('list') then
        ImGui.NewLine() 

        -- line
        local line, selected = ImGui.InputTextWithHint('line##lsLinedeBuff', mq.TLO.Macro.Variable('madeBuff').Find('lsLinedeBuff').Value(), '', ImGuiInputTextFlags.EnterReturnsTrue)
        if selected then
          mq.cmd.luaedit('lsLinedeBuff', line, 'overwrite')
        end 
        ImGui.NewLine() 

        for i=1,12 do 
          ImGui.TextColored(0.39, 0.58, 0.92, 1, i)
          ImGui.SameLine()
          if mq.TLO.Macro.Variable('madeBuff').Find('swdeBuff'..i).Value() == 'TRUE' then
            local _switch, pressed = ImGui.Checkbox('##swdeBuff'..i, mq.TLO.Macro.Variable('madeBuff').Find('swdeBuff'..i).Value() == 'TRUE')
            if pressed then
              mq.cmd.luaedit('swdeBuff'..i, _switch and 'TRUE' or 'FALSE')
            end

            indent(i,1)

            -- name
            local _name, selected = ImGui.InputTextWithHint('name##stdeBuff'..i, mq.TLO.Macro.Variable('madeBuff').Find('stdeBuff'..i).Value(), '', ImGuiInputTextFlags.EnterReturnsTrue)
            if selected then
              mq.cmd.luaedit('stdeBuff'..i, _name)
            end
            -- tag
            local _tag, selected = ImGui.InputTextWithHint('tag##lsdeBuff'..i, mq.TLO.Macro.Variable('madeBuff').Find('lsdeBuff'..i..'Tag').Value(), '', ImGuiInputTextFlags.EnterReturnsTrue)
            if selected then
              mq.cmd.luaedit('lsdeBuff'..i..'Tag', _tag, 'overwrite')
            end
       
            indent(i,2)
           
          else 
            if mq.TLO.Macro.Variable('madeBuff').Find('stdeBuff'..i).Value() == 'FALSE' then
              tmpName = '##swdeBuff'..i
            else
              tmpName = mq.TLO.Macro.Variable('madeBuff').Find('stdeBuff'..i).Value()..'##swdeBuff'..i
            end  
            
            local _switch, pressed = ImGui.Checkbox('##swdeBuff'..i, mq.TLO.Macro.Variable('madeBuff').Find('swdeBuff'..i).Value() == 'TRUE')
            if pressed then
              mq.cmd.luaedit('swdeBuff'..i, _switch and 'TRUE' or 'FALSE')
            end
          end 
        end
        ImGui.NewLine()
        ImGui.TreePop()
      end
      ImGui.NewLine() 
    end

  -- DEFENSE
    if ImGui.CollapsingHeader('/defense') then
      ImGui.NewLine()
      indent(1,1)
      ImGui.Columns(3, 'defense', false)

      -- count
      local line, selected = ImGui.InputTextWithHint('count##stCountDef', mq.TLO.Macro.Variable('maDefense').Find('stCountDef').Value(), '', ImGuiInputTextFlags.EnterReturnsTrue)
      if selected then
         mq.cmd.luaedit('stCountDef', line, 'overwrite')
      end 
      
      -- pct
      local line, selected = ImGui.InputTextWithHint('pct##stPctDef', mq.TLO.Macro.Variable('maDefense').Find('stPctDef').Value(), '', ImGuiInputTextFlags.EnterReturnsTrue)
      if selected then
         mq.cmd.luaedit('stPctDef', line, 'overwrite')
      end         
      indent(1,2)
      ImGui.NextColumn()
      ImGui.Columns()


      ImGui.NewLine()
      if ImGui.TreeNode('list') then
        ImGui.NewLine()
        
        -- line
        local line, selected = ImGui.InputTextWithHint('line##lsLineDefense', mq.TLO.Macro.Variable('maDefense').Find('lsLineDefense').Value(), '', ImGuiInputTextFlags.EnterReturnsTrue)
        if selected then
           mq.cmd.luaedit('lsLineDefense', line, 'overwrite')
        end 
        
        ImGui.NewLine()

        for i=1,20 do 
          ImGui.TextColored(0.39, 0.58, 0.92, 1, i)
          ImGui.SameLine()
          if mq.TLO.Macro.Variable('maDefense').Find('swDefense'..i).Value() == 'TRUE' then
            local _switch, pressed = ImGui.Checkbox('##swDefense'..i, mq.TLO.Macro.Variable('maDefense').Find('swDefense'..i).Value() == 'TRUE')
            if pressed then
              mq.cmd.luaedit('swDefense'..i, _switch and 'TRUE' or 'FALSE')
            end
            
            indent(i,1)

            -- name
            local _name, selected = ImGui.InputTextWithHint('name##stDefense'..i, mq.TLO.Macro.Variable('maDefense').Find('stDefense'..i).Value(), '', ImGuiInputTextFlags.EnterReturnsTrue)
            if selected then
              mq.cmd.luaedit('stDefense'..i, _name)
            end
            -- tag
            local _tag, selected = ImGui.InputTextWithHint('tag##lsDefense'..i, mq.TLO.Macro.Variable('maDefense').Find('lsDefense'..i..'Tag').Value(), '', ImGuiInputTextFlags.EnterReturnsTrue)
            if selected then
              mq.cmd.luaedit('lsDefense'..i..'Tag', _tag, 'overwrite')
            end
            indent(i,2)
           
          else 
            if mq.TLO.Macro.Variable('maDefense').Find('stDefense'..i).Value() == 'FALSE' then
              tmpName = '##swDefense'..i
            else
              tmpName = mq.TLO.Macro.Variable('maDefense').Find('stDefense'..i).Value()..'##swDefense'..i
            end    

            local _switch, pressed = ImGui.Checkbox(tmpName, mq.TLO.Macro.Variable('maDefense').Find('swDefense'..i).Value() == 'TRUE')
            if pressed then
              mq.cmd.luaedit('swDefense'..i, _switch and 'TRUE' or 'FALSE')
            end
          end 
        end
        ImGui.NewLine()
        ImGui.TreePop()
      end
      ImGui.NewLine()
    end

  -- ENTROPY
    if ImGui.CollapsingHeader('/entropy') then    
      ImGui.NewLine()
      indent(1,1)
      
      ImGui.Columns(2, 'entbase', false)
        edit_switch_perm('autolist', 'maEntropy', 'swAutoList')
        edit_switch_perm('buff cooldown', 'maEntropy', 'swBuffCooldownWait')
        edit_switch_perm('con color', 'maEntropy', 'swUseConColor')
        edit_switch_perm('TLP', 'maEntropy', 'swTLP')
        edit_switch_perm('set remember', 'maEntropy', 'swBuildSetRemember')
        edit_switch_perm('relay tells', 'maEntropy', 'swRelayTell')
        edit_switch_perm('maitnenace', 'maEntropy', 'swMaintenance')
        edit_switch_perm('inv in manual', 'maEntropy', 'swAutoInvManual')
        edit_switch_perm('events', 'maEntropy', 'swEventsInc')
        edit_switch_perm('hook', 'maEntropy', 'swHookSub')
        -- edit_switch_perm('HUD', 'maEntropy', 'swHUDAuto')
      ImGui.NextColumn()
        edit_text_perm('cast loop', 'maEntropy', 'stCastLoop')
        local drop = mq.TLO.Macro.Variable('maEntropy').Find('stEngine').Value()
        if ImGui.BeginCombo('engine', drop) then
          for _, v in ipairs({ '1', '2', '3' }) do
            local selected = v == drop
            if ImGui.Selectable(v, selected) and not selected then
              mq.cmd.luaedit('stEngine', v)
            end
          end
          ImGui.EndCombo()
        end
        edit_text_perm('fizzles', 'maEntropy', 'stCountFizzleRetry')
        edit_text_perm('main loop', 'maEntropy', 'stMainLoopDelay')
      ImGui.Columns() 
      indent(1,2)         

      ImGui.NewLine()         

      -- /entropy random
      if ImGui.TreeNode('random') then
        ImGui.NewLine()         
        ImGui.Columns(2, 'random', false)
          edit_text_perm('engage', 'maEntropy', 'stRNDEngage')
          edit_text_perm('engage swarm', 'maEntropy', 'stRNDEngageSwarm')
          edit_text_perm('rez take', 'maEntropy', 'stRNDRezTake')
        ImGui.NextColumn()
          edit_text_perm('engage pet', 'maEntropy', 'stRNDEngagePet')
          edit_text_perm('engage burn', 'maEntropy', 'stRNDEngageBurn')
          edit_text_perm('repeat', 'maEntropy', 'stRNDRepeat')
        ImGui.Columns() 
        ImGui.NewLine()   
        ImGui.TreePop()      
      end

      -- /entropy dannet
      if ImGui.TreeNode('dannet') then
        ImGui.NewLine()         
        ImGui.Columns(2, 'dannet', false)
          edit_switch_perm('local echo', 'maEntropy', 'swLocalEcho')
          edit_switch_perm('command echo', 'maEntropy', 'swCommandEcho')
          edit_switch_perm('front delimiter', 'maEntropy', 'swFrontDelim')
          edit_switch_perm('full names', 'maEntropy', 'swFullNames')
          edit_switch_perm('evasive refresh', 'maEntropy', 'swEvasiveRefresh')
        ImGui.NextColumn()
          edit_text_perm('evasive', 'maEntropy', 'stNetworkEvasive')
          edit_text_perm('expired', 'maEntropy', 'stNetworkExpired')
          edit_text_perm('query', 'maEntropy', 'stDanNetQueryDelay')
          edit_text_perm('timeout', 'maEntropy', 'stNetworkTimeout')
          edit_text_perm('group', 'maEntropy', 'stEntropyGroup_all')
        ImGui.Columns() 
        ImGui.NewLine()
        ImGui.TreePop()      
      end
      ImGui.NewLine()
    end

  -- ENV
    if ImGui.CollapsingHeader('/env') then    
      ImGui.NewLine()

        
      -- begmage:off . .  . . exp:-> . fireworks:off . incharge . 
      --  .  . rad:270 .  . saferadius:30 . . 
        
      if ImGui.TreeNode('food and drink') then        
        ImGui.NewLine()   
        ImGui.Columns(2, 'fd', false)
          edit_text_perm('food', 'maEnv', 'stFood')
          edit_text_perm('drink', 'maEnv', 'stDrink')
        ImGui.NextColumn()
          edit_text_perm('maintain##food', 'maHard', 'stFoodMaintain')
          edit_text_perm('maintain##drink', 'maHard', 'stDrinkMaintain')
        ImGui.Columns()
        ImGui.TreePop()      
        ImGui.NewLine()   
      end  

      -- swSafeListGuild      

      -- stMobLvlMin
      -- stMobLvlMax


 

          edit_switch_perm('auto inventory##swAutoInv', 'maEnv', 'swAutoInv')
          edit_text_perm('list##lsAutoInventory', 'maEnv', 'lsAutoInventory')

      
    if ImGui.TreeNode('loot') then
      ImGui.NewLine()
          local drop = mq.TLO.Macro.Variable('maEnv').Find('stLootMode').Value()
          if ImGui.BeginCombo('loot', drop) then
            for _, v in ipairs({ 'off', 'manual', 'advanced' }) do
              local selected = v == drop
              if ImGui.Selectable(v, selected) and not selected then
                mq.cmd.luaedit('stLootMode', v)
              end
            end
            ImGui.EndCombo()
          end
      ImGui.TreePop()      
      ImGui.NewLine()   
    end  



      -- swAutoInv
      -- lsAutoInventory

      -- stBuffGem     
      -- swHoldBuffGem   

          edit_switch_perm('tribute##swCheckTribute', 'maEnv', 'swCheckTribute')

          edit_switch_perm('group invite##swTakeGroupInvite', 'maEnv', 'swTakeGroupInvite')
          edit_switch_perm('raid invite##swTakeRaidInvite', 'maEnv', 'swTakeRaidInvite')

      ImGui.NewLine()
    end

  -- HEAL
    if ImGui.CollapsingHeader('/heal') then

      ImGui.NewLine()  
      indent(1,1) 

        ImGui.Columns(3, 'heal', false)
          edit_switch_perm('self', 'maHeal', 'swHealSelf')
          edit_switch_perm('dannet', 'maHeal', 'swHealDanNet')
          edit_switch_perm('group', 'maHeal', 'swHealGroup')
          -- xtarget   
          local _switch, xtpressed = ImGui.Checkbox("##swHealXTarget", mq.TLO.Macro.Variable('maHeal').Find('swHealXTarget').Value() == 'TRUE')
          if xtpressed then
            mq.cmd.luaedit('swHealXTarget', _switch and 'TRUE' or 'FALSE')
          end
          ImGui.SameLine()
          local healxt = ImGui.Button('xt', 60, 22)
          if healxt then
            mq.cmd.heal('xt build')
          end

          edit_switch_perm('pet', 'maHeal', 'swHealPet')
          edit_text_perm('rampage', 'maHeal', 'stRampageTank')
       
        ImGui.NextColumn()
          edit_switch_perm('weight', 'maHeal', 'swHealWeighted')
          edit_switch_perm('break', 'maHeal', 'swBreakHealPCT')
          edit_text_perm('adjust', 'maHeal', 'stHealAdjust')
        ImGui.NextColumn()
        ImGui.Columns()  
        edit_text_perm('order', 'maHeal', 'lsOrderHeal')
        indent(1,2)
        ImGui.NewLine()
        
      if ImGui.TreeNode('list') then 
        ImGui.NewLine()
        indent(1,1)
        -- line
        local line, selected = ImGui.InputTextWithHint('line##healline', mq.TLO.Macro.Variable('maHeal').Find('lsLineHeal').Value(), '', ImGuiInputTextFlags.EnterReturnsTrue)
        if selected then
          mq.cmd.luaedit('lsLineHeal', line, 'overwrite')
        end     
        indent(1,2)

        ImGui.NewLine()
        for i=1,50 do 
          ImGui.TextColored(0.39, 0.58, 0.92, 1, i)
          ImGui.SameLine()
          if mq.TLO.Macro.Variable('maHeal').Find('swHeal'..i).Value() == 'TRUE' then
            local _switch, pressed = ImGui.Checkbox('##swHeal'..i, mq.TLO.Macro.Variable('maHeal').Find('swHeal'..i).Value() == 'TRUE')
            if pressed then
              mq.cmd.luaedit('swHeal'..i, _switch and 'TRUE' or 'FALSE')
            end

            indent(i,1)

            -- name
            local line, selected = ImGui.InputTextWithHint('name##stHeal'..i, mq.TLO.Macro.Variable('maHeal').Find('stHeal'..i).Value(), '', ImGuiInputTextFlags.EnterReturnsTrue)
            if selectedbuff then
              mq.cmd.luaedit('stHeal'..i, line)
            end
            -- tag
            local line, selected = ImGui.InputTextWithHint('tag##lsHeal'..i..'Tag', mq.TLO.Macro.Variable('maHeal').Find('lsHeal'..i..'Tag').Value(), '', ImGuiInputTextFlags.EnterReturnsTrue)
            if selected then
              mq.cmd.luaedit('lsHeal'..i..'Tag', line, 'overwrite')
            end
       
            indent(i,2)
           
          else 
            if mq.TLO.Macro.Variable('maHeal').Find('stHeal'..i).Value() == 'FALSE' then
              tmpName = '##swHeal'..i
            else
              tmpName = mq.TLO.Macro.Variable('maHeal').Find('stHeal'..i).Value()..'##swHeal'..i
            end
            
            local _switch, pressed = ImGui.Checkbox(tmpName, mq.TLO.Macro.Variable('maHeal').Find('swHeal'..i).Value() == 'TRUE')
            if pressed then
              mq.cmd.luaedit('swHeal'..i, _switch and 'TRUE' or 'FALSE')
            end
          end
        end
        ImGui.NewLine()
        ImGui.TreePop()      
      end
      

      
      if ImGui.TreeNode('point') then     
        ImGui.NewLine() 
        indent(1,1)
        for classCode, healPoint in pairs(healPoint) do
          healPoint.value, used = ImGui.DragInt(classCode..'##healpointslide', healPoint.value, 1, 0, 99)
          if used then healPoint.updated = true end
        end
        ImGui.NewLine()
        indent(1,2)
      end
      ImGui.NewLine()
      ImGui.TreePop()      
    end

  -- HOME
    if ImGui.CollapsingHeader('/home') then
      ImGui.NewLine()
      indent(1,1)
      
      ImGui.Columns(2, 'homebase', false)
          edit_switch_perm('on auto', 'maHome', 'swonAuto')
          edit_switch_perm('check in combat', 'maHome', 'swinCombat')
          edit_switch_perm('clear on gather', 'maHome', 'swMoveGatherClear')
          edit_switch_perm('clear on death', 'maHome', 'swMoveDeathClear')
          edit_switch_perm('face fast', 'maHome', 'swFaceFast')
        ImGui.NextColumn()
          edit_text_perm('variance', 'maHome', 'stHomeVariance')
          --edit_text_perm('mount', 'maHome', 'stMount')
          edit_text_perm('view arc', 'maHome', 'stFaceArc')

        ImGui.Columns()  
      ImGui.NewLine()
        local pdrop = mq.TLO.Macro.Variable('maHome').Find('stCombatPosition').Value()
        if ImGui.BeginCombo('position', pdrop) then
          for _, v in ipairs({ 'free', 'left', 'right', 'rear', 'face', 'random' }) do
            local selected = v == pdrop
            if ImGui.Selectable(v, selected) and not selected then
              mq.cmd.luaedit('stCombatPosition', v)
            end
          end
          ImGui.EndCombo()
        end
        edit_text_perm('random list', 'maHome', 'lsPosition')
      ImGui.NewLine()
      indent(1,2)

    end


  -- MINION
    if class_pet[mq.TLO.Me.Class.ShortName()] then
      if ImGui.CollapsingHeader('/minion') then
        ImGui.NewLine()
        indent(1,1)        

        ImGui.Columns(3, 'minioncol', false)
          edit_switch_perm('use pet', 'maMinion', 'swPet')
          edit_switch_perm('buff pet', 'maMinion', 'swPetBuff')
          edit_switch_perm('shrink', 'maMinion', 'swPetShrink')
          edit_switch_perm('use swarm', 'maMinion', 'swSwarm')
          edit_switch_perm('no swarm on ds', 'maMinion', 'swNoSwarmonDS')
          edit_switch_perm('on cc force', 'maMinion', 'swonForce')
          edit_switch_perm('use pet gear', 'maMinion', 'swUsePetGear')

        ImGui.NextColumn()
          edit_text_perm('% to send pet', 'maMinion', 'stPctPetEngage')
          edit_text_perm('illusion', 'maMinion', 'stPetIllusion')
          edit_text_perm('shrink item', 'maMinion', 'stPetShrink')
          edit_text_perm('% to send swarm', 'maMinion', 'stPctSwarmEngage')
          edit_text_perm('weapons', 'maMinion', 'stPetWeapon')
          if mq.TLO.Me.Class.Shortname() == 'MAG' then
            edit_text_perm('beg alias', 'maMinion', 'stBegAlias')
          end
        
        ImGui.Columns()
        ImGui.NewLine()        
        indent(1,2)        
      end
    end


  -- MELEE
    if ImGui.CollapsingHeader('/melee') then
      ImGui.NewLine()
      indent(1,1)

        -- line
        local line, selected = ImGui.InputTextWithHint('line##lsLineMelee', mq.TLO.Macro.Variable('maMelee').Find('lsLineMelee').Value(), '', ImGuiInputTextFlags.EnterReturnsTrue)
        if selected then
           mq.cmd.luaedit('lsLineMelee', line, 'overwrite')
        end 
      indent(1,2)
      ImGui.NewLine()
      
      for i=1,12 do 
        ImGui.TextColored(0.39, 0.58, 0.92, 1, i)
        ImGui.SameLine()
        if mq.TLO.Macro.Variable('maMelee').Find('swMelee'..i).Value() == 'TRUE' then
          local _switch, pressed = ImGui.Checkbox('##swMelee'..i, mq.TLO.Macro.Variable('maMelee').Find('swMelee'..i).Value() == 'TRUE')
          if pressed then
            mq.cmd.luaedit('swMelee'..i, _switch and 'TRUE' or 'FALSE')
          end

          indent(i,1)

          -- name
          local _name, selected = ImGui.InputTextWithHint('name##stMelee'..i, mq.TLO.Macro.Variable('maMelee').Find('stMelee'..i).Value(), '', ImGuiInputTextFlags.EnterReturnsTrue)
          if selected then
            mq.cmd.luaedit('stMelee'..i, _name)
          end
          -- tag
          local _tag, selected = ImGui.InputTextWithHint('tag##lsMelee'..i, mq.TLO.Macro.Variable('maMelee').Find('lsMelee'..i..'Tag').Value(), '', ImGuiInputTextFlags.EnterReturnsTrue)
          if selected then
            mq.cmd.luaedit('lsMelee'..i..'Tag', _tag, 'overwrite')
          end
     
          indent(i,2)
         
        else 
          local _switch, pressed = ImGui.Checkbox('##swMelee'..i, mq.TLO.Macro.Variable('maMelee').Find('swMelee'..i).Value() == 'TRUE')
          if pressed then
            mq.cmd.luaedit('swMelee'..i, _switch and 'TRUE' or 'FALSE')
          end
        end 
      end

      ImGui.NewLine()
    end      

  -- NUKE
    if ImGui.CollapsingHeader('/nuke') then
      ImGui.NewLine()
      indent(1,1)

      ImGui.Columns(2, 'nocurename', false)
        edit_switch_perm('loop', 'maNuke', 'swNukeLoop')
        edit_switch_perm('sit', 'maNuke', 'swNukeWhackAMole')
      ImGui.NextColumn()

        -- stNukeDelay
        local line, selected = ImGui.InputTextWithHint('delay', mq.TLO.Macro.Variable('maNuke').Find('stNukeDelay').Value(), '', ImGuiInputTextFlags.EnterReturnsTrue)
        if selected then
           mq.cmd.luaedit('stNukeDelay', line)
        end 
        -- stPctStopNuke
        local line, selected = ImGui.InputTextWithHint('pct stop', mq.TLO.Macro.Variable('maNuke').Find('stPctStopNuke').Value(), '', ImGuiInputTextFlags.EnterReturnsTrue)
        if selected then
           mq.cmd.luaedit('stPctStopNuke', line)
        end 

      ImGui.Columns()
      indent(1,2)

      ImGui.NewLine()
      if ImGui.TreeNode('list') then
        ImGui.NewLine()

        -- line
        indent(1,1)
        local line, selected = ImGui.InputTextWithHint('line##nukeline', mq.TLO.Macro.Variable('maNuke').Find('lsLineNuke').Value(), '', ImGuiInputTextFlags.EnterReturnsTrue)
        if selected then
           mq.cmd.luaedit('lsLineNuke', line, 'overwrite')
        end 
        indent(1,2)

        ImGui.NewLine()

        for i=1,20 do 
          ImGui.TextColored(0.39, 0.58, 0.92, 1, i)
          ImGui.SameLine()
          if mq.TLO.Macro.Variable('maNuke').Find('swNuke'..i).Value() == 'TRUE' then
            local _switch, pressed = ImGui.Checkbox('##swNuke'..i, mq.TLO.Macro.Variable('maNuke').Find('swNuke'..i).Value() == 'TRUE')
            if pressed then
              mq.cmd.luaedit('swNuke'..i, _switch and 'TRUE' or 'FALSE')
            end

            indent(i,1)

            -- name
            local _name, selected = ImGui.InputTextWithHint('name##stNuke'..i, mq.TLO.Macro.Variable('maNuke').Find('stNuke'..i).Value(), '', ImGuiInputTextFlags.EnterReturnsTrue)
            if selected then
              mq.cmd.luaedit('stNuke'..i, _name)
            end
            -- tag
            local _tag, selected = ImGui.InputTextWithHint('tag##lsNuke'..i, mq.TLO.Macro.Variable('maNuke').Find('lsNuke'..i..'Tag').Value(), '', ImGuiInputTextFlags.EnterReturnsTrue)
            if selected then
              mq.cmd.luaedit('lsNuke'..i..'Tag', _tag, 'overwrite')
            end
       
            indent(i,2)
           
          else 
            if mq.TLO.Macro.Variable('maNuke').Find('stNuke'..i).Value() == 'FALSE' then
              tmpName = '##swNuke'..i
            else
              tmpName = mq.TLO.Macro.Variable('maNuke').Find('stNuke'..i).Value()..'##swNuke'..i
            end            
            
            local _switch, pressed = ImGui.Checkbox(tmpName, mq.TLO.Macro.Variable('maNuke').Find('swNuke'..i).Value() == 'TRUE')
            if pressed then
              mq.cmd.luaedit('swNuke'..i, _switch and 'TRUE' or 'FALSE')
            end
          end 
        end
        ImGui.NewLine()
        ImGui.TreePop()      
      end

      ImGui.NewLine()
    end

  -- OVERRIDE
    if ImGui.CollapsingHeader('/override') then
      ImGui.NewLine()
      indent(1,1)
      
      ImGui.Columns(3, 'overridecol', false)
        edit_switch_perm('LoS', 'maOver', 'swOverLOS')
        edit_switch_perm('pct engage', 'maOver', 'swOverPctEngage')
        edit_switch_perm('loot', 'maOver', 'swOverLoot')
        edit_switch_perm('safe names', 'maOver', 'swOverSafeNames')
      ImGui.NextColumn()
        edit_switch_perm('train spell', 'maOver', 'swOverTrainSpell')
        edit_switch_perm('env auto', 'maOver', 'swOverEnvAuto')
        edit_switch_perm('DS check', 'maOver', 'swOverDSCheck')
        edit_switch_perm('NAV check', 'maOver', 'swOverNavCheck')
      ImGui.NextColumn()
        edit_switch_perm('group ass', 'maOver', 'swOverGroupAss')
        edit_switch_perm('splash hurt', 'maOver', 'swOverSplashHurt')
        edit_switch_perm('target clear', 'maOver', 'swTargetClear')
      ImGui.Columns()
      ImGui.NewLine()
      indent(1,2)
    end 

  -- PULL
    if ImGui.CollapsingHeader('/pull') then
      ImGui.NewLine()
      indent(1,1)

      ImGui.Columns(2, 'col_pull', false)
        edit_text_perm('rad', 'maPull', 'stPullRadius')
        edit_text_perm('zrad', 'maPull', 'stPullZRadius')
        edit_text_perm('nav stop', 'maPull', 'stPullNavStopDistance')
        edit_text_perm('nav var', 'maPull', 'stPullNavVariance')


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



      ImGui.NextColumn()
        edit_text_perm('range bando', 'maPull', 'stRangeBandolier')
        edit_text_perm('outrun', 'maPull', 'stPullOutrunRange')
        edit_text_perm('chain', 'maPull', 'stCountChainPull')
        edit_text_perm('pet watch', 'maPull', 'stPullPetWatch')
      ImGui.Columns()
      
      ImGui.Columns(2, 'col_pull2', false)      
        edit_switch_perm('active', 'maPull', 'swPull')
        edit_switch_perm('navlos', 'maPull', 'swPullNavLoS')
        edit_switch_perm('pathlogic', 'maPull', 'swNavPathLogic')
      ImGui.NextColumn()
        edit_switch_perm('healcheck', 'maPull', 'swPullHealCheck')
        edit_switch_perm('sethome', 'maPull', 'swPullSetHome')
      ImGui.Columns()


      indent(1,2)
      ImGui.NewLine()
      ImGui.Columns(2, 'noname', false)

      if ImGui.TreeNode('hard stop') then 
        ImGui.NewLine()
        indent(1,1)
          edit_text_perm('pct', 'maPull', 'stPctHardStop')
          edit_text_perm('duration', 'maPull', 'stHardStopDuration')
          edit_switch_perm('rez', 'maPull', 'swHardStopRez')
        ImGui.NextColumn()
        ImGui.Columns()
        ImGui.TreePop()
        ImGui.NewLine()
        indent(1,2)
      end

      if ImGui.TreeNode('pull other list') then 
        ImGui.NewLine()
        indent(1,1)
        
        edit_text_perm('line', 'maPull', 'lsLinePull')
        indent(1,2)
        
        ImGui.NewLine()
        for i=1,12 do 
          ImGui.TextColored(0.39, 0.58, 0.92, 1, i)
          ImGui.SameLine()
          if mq.TLO.Macro.Variable('maPull').Find('swPull'..i).Value() == 'TRUE' then
            local _switch, pressed = ImGui.Checkbox('##swPull'..i, mq.TLO.Macro.Variable('maPull').Find('swPull'..i).Value() == 'TRUE')
            if pressed then
              mq.cmd.luaedit('swPull'..i, _switch and 'TRUE' or 'FALSE')
            end
            indent(i,1)
            -- name
            local _name, selected = ImGui.InputTextWithHint('name##stPull'..i, mq.TLO.Macro.Variable('maPull').Find('stPull'..i).Value(), '', ImGuiInputTextFlags.EnterReturnsTrue)
            if selected then
              mq.cmd.luaedit('stPull'..i, _name)
            end
            -- tag
            local _tag, selected = ImGui.InputTextWithHint('tag##lsPull'..i, mq.TLO.Macro.Variable('maPull').Find('lsPull'..i..'Tag').Value(), '', ImGuiInputTextFlags.EnterReturnsTrue)
            if selected then
              mq.cmd.luaedit('lsPull'..i..'Tag', _tag, 'overwrite')
            end
            indent(i,2)
           
          else 
            if mq.TLO.Macro.Variable('maPull').Find('stPull'..i).Value() == 'FALSE' then
              tmpName = '##swPull'..i
            else
              tmpName = mq.TLO.Macro.Variable('maPull').Find('stPull'..i).Value()..'##swPull'..i
            end    

            local _switch, pressed = ImGui.Checkbox(tmpName, mq.TLO.Macro.Variable('maPull').Find('swPull'..i).Value() == 'TRUE')
            if pressed then
              mq.cmd.luaedit('swPull'..i, _switch and 'TRUE' or 'FALSE')
            end
          end 
        end

        ImGui.TreePop()
      end
      ImGui.NewLine()
    end

  -- REST
    if ImGui.CollapsingHeader('/rest') then

      ImGui.NewLine()
      indent(1,1)
      
      ImGui.Columns(2, 'homebase', false)
          edit_switch_perm('to full', 'maRest', 'swRestFull')
          edit_switch_perm('use rods', 'maRest', 'swRestModRod')
          edit_switch_perm('check buffs', 'maRest', 'swRestBuff')
          edit_switch_perm('in combat', 'maRest', 'swRestCombat')
          edit_switch_perm('check healing', 'maRest', 'swRestCheckHealth')
          edit_switch_perm('in raids', 'maRest', 'swRestInRaid')

        ImGui.NextColumn()
          edit_text_perm('pct rest', 'maRest', 'stPctRest')
          edit_text_perm('radius check', 'maRest', 'stRestRadius')

        ImGui.Columns()  

      ImGui.NewLine()
        edit_text_perm('spawn search', 'maRest', 'stRestSpawnSearch')

      ImGui.NewLine()
      indent(1,2)
    end

  -- REZ
    if ImGui.CollapsingHeader('/rez') then
      ImGui.NewLine()
      indent(1,1)

      ImGui.Columns(2, 'rezstuff', false)
        edit_text_perm('radius', 'maRez', 'stMaxRezRange')
        local combo = mq.TLO.Macro.Variable('maRez').Find('stPctMinRez').Value()
        if ImGui.BeginCombo('pct', combo) then
          for _, v in ipairs({ '10', '20', '35', '50', '60', '75', '90', '93', '96' }) do
            local selected = v == combo
            if ImGui.Selectable(v, selected) and not selected then
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
      ImGui.NewLine()   

      edit_text_perm('IC', 'maRez', 'lsRezIC')
      edit_text_perm('OOC', 'maRez', 'lsRezOOC')

      
      indent(1,2)
      ImGui.NewLine()   
      if ImGui.TreeNode('spawn search') then  

        -- rez line
        ImGui.NewLine()   
        indent(1,1)
        edit_text_perm('line', 'maRez', 'lsLineRez')
        indent(1,2)

        ImGui.NewLine()

        for i=1,8 do 
          ImGui.TextColored(0.39, 0.58, 0.92, 1, i)
          ImGui.SameLine()
          if mq.TLO.Macro.Variable('maRez').Find('swRez'..i).Value() == 'TRUE' then
            local _switch, pressed = ImGui.Checkbox('##swRez'..i, mq.TLO.Macro.Variable('maRez').Find('swRez'..i).Value() == 'TRUE')
            if pressed then
              mq.cmd.luaedit('swRez'..i, _switch and 'TRUE' or 'FALSE')
            end
            indent(i,1)
            -- name
            local _name, selected = ImGui.InputTextWithHint('spawn##stRez'..i, mq.TLO.Macro.Variable('maRez').Find('stRez'..i).Value(), '', ImGuiInputTextFlags.EnterReturnsTrue)
            if selected then
              mq.cmd.luaedit('stRez'..i, _name)
            end
            indent(i,2)
          else 
            if mq.TLO.Macro.Variable('maRez').Find('stRez'..i).Value() == 'FALSE' then
              tmpName = '##swRez'..i
            else
              tmpName = mq.TLO.Macro.Variable('maRez').Find('stRez'..i).Value()..'##swRez'..i
            end
            local _switch, pressed = ImGui.Checkbox(tmpName, mq.TLO.Macro.Variable('maRez').Find('swRez'..i).Value() == 'TRUE')
            if pressed then
              mq.cmd.luaedit('swRez'..i, _switch and 'TRUE' or 'FALSE')
            end
          end 
        end
        ImGui.TreePop()      
      end

      ImGui.NewLine()
    end

  -- SONG
    if mq.TLO.Me.Class.ShortName() == "BRD" then
      if ImGui.CollapsingHeader('/song') then
      end
    end
    
  -- maHard
    if ImGui.CollapsingHeader('maHard') then 
      ImGui.NewLine()
      indent(1,1)
      

      edit_text_perm('stBardSwapRemain', 'maHard', 'stBardSwapRemain')
      edit_text_perm('stDrinkMaintain', 'maHard', 'stDrinkMaintain')
      edit_text_perm('stFoodMaintain', 'maHard', 'stFoodMaintain')
      edit_text_perm('stMaxBowDist', 'maHard', 'stMaxBowDist')
      edit_text_perm('stMaxCureRad', 'maHard', 'stMaxCureRad')
      edit_text_perm('stMaxMeleeAdj', 'maHard', 'stMaxMeleeAdj')
      edit_text_perm('stMaxRadPet', 'maHard', 'stMaxRadPet')
      edit_text_perm('stMinBowDist', 'maHard', 'stMinBowDist')
      edit_text_perm('stMinMovetoBow', 'maHard', 'stMinMovetoBow')
      edit_text_perm('stModeDragTargetCheck', 'maHard', 'stModeDragTargetCheck')
      edit_text_perm('stPullMobTagTry', 'maHard', 'stPullMobTagTry')
      edit_text_perm('stRadGrab', 'maHard', 'stRadGrab')
      edit_text_perm('stRezFail', 'maHard', 'stRezFail')
      edit_text_perm('stRezFailToken', 'maHard', 'stRezFailToken')
      edit_text_perm('stRezIgnoreFucktards', 'maHard', 'stRezIgnoreFucktards')
      edit_text_perm('stRezSuccess', 'maHard', 'stRezSuccess')
      edit_text_perm('stRezSummonRange', 'maHard', 'stRezSummonRange')
      edit_text_perm('stSpellBookSize', 'maHard', 'stSpellBookSize')
      edit_text_perm('stMaxSPARemoveCount', 'maHard', 'stMaxSPARemoveCount')
      edit_text_perm('stDelayLua', 'maHard', 'stDelayLua')
      ImGui.NewLine()
      indent(1,2)

    end

  -- maTimer
    if ImGui.CollapsingHeader('maTimer') then 
      ImGui.NewLine()
      indent(1,1)
      

      ImGui.Columns(2, 'timers', false)

        edit_text_perm('Try_Door', 'maTimer', 'tiTry_Door')
        edit_text_perm('Check_Ammo', 'maTimer', 'tiCheck_Ammo')
        edit_text_perm('Nuke_Delay', 'maTimer', 'tiNuke_Delay')
        edit_text_perm('Check_Cursor', 'maTimer', 'tiCheck_Cursor')
        edit_text_perm('Check_Zone_ID', 'maTimer', 'tiCheck_Zone_ID')
        edit_text_perm('Check_Observers', 'maTimer', 'tiCheck_Observers')
        edit_text_perm('Check_Poison', 'maTimer', 'tiCheck_Poison')
        edit_text_perm('Check_deBuffs', 'maTimer', 'tiCheck_deBuffs')
        edit_text_perm('Check_Behind', 'maTimer', 'tiCheck_Behind')
        edit_text_perm('Check_Tribute', 'maTimer', 'tiCheck_Tribute')
        edit_text_perm('Check_Loot', 'maTimer', 'tiCheck_Loot')
        edit_text_perm('Check_Mode', 'maTimer', 'tiCheck_Mode')
        edit_text_perm('Check_LoS_Timeout', 'maTimer', 'tiCheck_LoS_Timeout')
        edit_text_perm('Check_Housekeeping', 'maTimer', 'tiCheck_Housekeeping')
        edit_text_perm('Check_Home', 'maTimer', 'tiCheck_Home')
        edit_text_perm('Check_Target', 'maTimer', 'tiCheck_Target')
        edit_text_perm('Check_Alliance', 'maTimer', 'tiCheck_Alliance')
        edit_text_perm('Check_Mercenary', 'maTimer', 'tiCheck_Mercenary')
        edit_text_perm('Check_Pull_Hardstop', 'maTimer', 'tiCheck_Pull_Hardstop')
        edit_text_perm('Check_Pull_Hardstop_Delay', 'maTimer', 'tiCheck_Pull_Hardstop_Delay')
        edit_text_perm('Check_Inventory_ManaClick', 'maTimer', 'tiCheck_Inventory_ManaClick')
        edit_text_perm('Check_Build_Spawn_Map_Data', 'maTimer', 'tiCheck_Build_Spawn_Map_Data')
        edit_text_perm('Mode_Open', 'maTimer', 'tiMode_Open')


      ImGui.NextColumn()
      ImGui.Columns()




     ImGui.NewLine()
      indent(1,2)

    end

    
  end
  ImGui.End()

end

mq.imgui.init('editorwindow', imguicallback)

while openGUI do 
  updateHealPoints()
  mq.delay(100) 
end







 



