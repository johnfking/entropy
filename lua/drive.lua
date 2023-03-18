--
-- entropoy.mac
-- drive.lua
-- 
-- driving toon
--


require 'common'
local openGUI = true
local shouldDrawHUD = true


local function imguicallback()
  hudInfo()
  openGUI, shouldDrawHUD = ImGui.Begin('drive###EntropyDrive', openGUI)
  
  local toon = 'tempname'
  
  if shouldDrawHUD and (ent.build == '--' or mq.TLO.EverQuest.GameState() ~= 'INGAME') then

  elseif shouldDrawHUD then  
   
		local ColumnID_Name = 0
		local ColumnID_HP = 1
		local ColumnID_Mana = 2
		local ColumnID_End = 3
		local ColumnID_Dist = 4
		local ColumnID_Lev = 5
		local ColumnID_Invis = 6
		local ColumnID_IVU = 7
		local ColumnID_Movement = 8
		local ColumnID_CMD = 9
		local ColumnID_Target = 10
		local ColumnID_Cast = 11

    local tableFlags = bit32.bor(ImGuiTableFlags.Resizable,
                                ImGuiTableFlags.RowBg,
                                ImGuiTableFlags.SizingFixedFit,
                                ImGuiTableFlags.Borders,
                                ImGuiTableFlags.Hideable)
					
  	if ImGui.BeginTable('Crew', 12, tableFlags) then

  		ImGui.TableSetupColumn('Name', ImGuiTableColumnFlags.DefaultSort, 0.0, ColumnID_Name)
  		ImGui.TableSetupColumn('HP', ImGuiTableColumnFlags.DefaultSort, 0.0, ColumnID_HP)
  		ImGui.TableSetupColumn('MP', ImGuiTableColumnFlags.DefaultSort, 0.0, ColumnID_Mana)
  		ImGui.TableSetupColumn('EP', ImGuiTableColumnFlags.DefaultSort, 0.0, ColumnID_End)
  		ImGui.TableSetupColumn('Dist', ImGuiTableColumnFlags.DefaultSort, 0.0, ColumnID_Dist)
  		ImGui.TableSetupColumn('Lev', ImGuiTableColumnFlags.DefaultSort, 0.0, ColumnID_Lev)
  		ImGui.TableSetupColumn('Inv', ImGuiTableColumnFlags.DefaultSort, 0.0, ColumnID_Invis)
  		ImGui.TableSetupColumn('Ivu', ImGuiTableColumnFlags.DefaultSort, 0.0, ColumnID_IVU)
  		ImGui.TableSetupColumn('Mov', ImGuiTableColumnFlags.DefaultSort, 0.0, ColumnID_Movement)
  		ImGui.TableSetupColumn('CMD', ImGuiTableColumnFlags.DefaultSort, 0.0, ColumnID_CMD)
  		ImGui.TableSetupColumn('Target', ImGuiTableColumnFlags.DefaultSort, 0.0, ColumnID_Target)
  		ImGui.TableSetupColumn('Action', ImGuiTableColumnFlags.DefaultSort, 0.0, ColumnID_Cast)
  		ImGui.TableHeadersRow()
  		ImGui.TableNextRow()	
  		ImGui.TableNextColumn()

    -- column: toon names & state: Green = on | purple = no macro | yella = off | red's ded baby


  		for i = 1, mq.TLO.DanNet.PeerCount(ent.channel)() 
  			do
  			
  			  -- tostring("zone_" .. mq.TLO.MacroQuest.Server() .. "_" .. mq.TLO.Zone.ShortName())
  			  local toon = mq.TLO.Spawn('pc ' .. mq.TLO.DanNet.Peers(ent.channel).Arg(i,'|')())
  			  if toon.ID() ~= 0 then

  					if toon.Dead() then
  						ImGui.TextColored(1, 0, 0, 1, toon.DisplayName()) 
  					elseif mq.TLO.DanNet(toon).Observe('maEnv.Find[swAuto].Value')() == 'TRUE' then
  						ImGui.TextColored(0, 1, 0, 1, toon.DisplayName())
  					elseif mq.TLO.DanNet(toon).Observe('maEnv.Find[swAuto].Value')() == 'FALSE' then
  						ImGui.TextColored(0.39, 0.58, 0.92, 1, toon.DisplayName())
  					else
  						ImGui.TextColored(0.90, 0.20, 0.92, 1, toon.DisplayName())
  					end  

    				ImGui.TableNextColumn()
    					if toon.PctHPs() >= 70 then
    						ImGui.TextColored(0, 1, 0, 1, toon.PctHPs())
    					elseif toon.PctHPs() < 70 and toon.PctHPs() >= 40 then
    						ImGui.TextColored(1, 1, 0, 1, toon.PctHPs())
    				  elseif toon.PctHPs() < 40 then
    						ImGui.TextColored(1, 0, 0, 1, toon.PctHPs())
    					end

  				  
  				  -- pct manna
  					ImGui.TableNextColumn()
              if toon.PctMana() >= 65 then
  							ImGui.TextColored(0, 1, 0, 1, toon.PctMana())
  						elseif toon.PctMana() < 65 and toon.PctMana() >= 30 then
  							ImGui.TextColored(1, 1, 0, 1, toon.PctMana())
  						elseif toon.PctMana() < 30 then
  							ImGui.TextColored(1, 0, 0, 1, toon.PctMana())
  					  end

          
            -- pct endurance
  					ImGui.TableNextColumn()
  						if toon.PctEndurance() >= 65 then
  							ImGui.TextColored(0, 1, 0, 1, toon.PctEndurance())
  						elseif toon.PctEndurance() < 65 and toon.PctEndurance() >= 30 then
  							ImGui.TextColored(1, 1, 0, 1, toon.PctEndurance())
  							elseif toon.PctEndurance() < 30 then
  							ImGui.TextColored(1, 0, 0, 1, toon.PctEndurance())
  						end

  					
  					-- distance		
  					ImGui.TableNextColumn()
  					
  					  if toon.ID() == mq.TLO.Me.ID() then
  					    ImGui.TextColored(0.39, 0.58, 0.92, 1, ico['none'])
  					  else 
                ImGui.TextColored(1, 1, 1, 1, string.format('%.1f', toon.Distance()))
                if ImGui.IsItemHovered() and ImGui.IsMouseReleased(ImGuiMouseButton.Left) then
                  mq.cmd.dexecute(toon.DisplayName() .. ' /nav id ' .. mq.TLO.Me.ID() .. ' |log=off')
                end
  					  end
  					
            -- levitate
  					ImGui.TableNextColumn()
  						if tonumber(mq.TLO.DanNet(toon).Observe('Me.SPA[57]')()) ~= 0 then
                ImGui.Text(ico.levi)
                if toon.ID() == mq.TLO.Me.ID() then
                  if ImGui.IsItemHovered() and ImGui.IsMouseReleased(ImGuiMouseButton.Left) then
                    mq.cmd.nomore('57') -- movement spa
                  end  
                else
                  if ImGui.IsItemHovered() and ImGui.IsMouseReleased(ImGuiMouseButton.Left) then
                    mq.cmd.dexecute(toon.DisplayName() .. ' /nomore 57') -- movement spa
                  end
                end
  						else
  						  ImGui.Text(ico.none)
  						end

  							  
  				  -- invis
  					ImGui.TableNextColumn()
  				  	if tonumber(mq.TLO.DanNet(toon).Observe('Me.SPA[314]')()) ~= 0 or tonumber(mq.TLO.DanNet(toon).Observe('Me.SPA[12]')()) ~= 0 then
  						  ImGui.Text(ico.invis)
                if toon.ID() == mq.TLO.Me.ID() then
                  if ImGui.IsItemHovered() and ImGui.IsMouseReleased(ImGuiMouseButton.Left) then
                    mq.cmd.nomore('normalinvis') -- normal invis

                  end  
                else
                  if ImGui.IsItemHovered() and ImGui.IsMouseReleased(ImGuiMouseButton.Left) then
                    mq.cmd.dexecute(toon.DisplayName() .. ' /nomore normalinvis') -- invis
                  end
                end
  						else
  						  ImGui.Text(ico.none)
  					  end


            -- IVU
  					ImGui.TableNextColumn()
  					 	if tonumber(mq.TLO.DanNet(toon).Observe('Me.SPA[315]')()) ~= 0 or tonumber(mq.TLO.DanNet(toon).Observe('Me.SPA[28]')()) ~= 0 then
  						  ImGui.Text(ico.invisvsundead)
                if toon.ID() == mq.TLO.Me.ID() then
                  if ImGui.IsItemHovered() and ImGui.IsMouseReleased(ImGuiMouseButton.Left) then
                    mq.cmd.nomore('undeadinvis') -- vs undead
                  end
                else
                  if ImGui.IsItemHovered() and ImGui.IsMouseReleased(ImGuiMouseButton.Left) then
                    mq.cmd.dexecute(toon.DisplayName() .. ' /nomore undeadinvis') -- vs undead
                  end
                end
  						else
  						  ImGui.Text(ico.none)
  						end


            -- movement
  					ImGui.TableNextColumn()
  						if tonumber(mq.TLO.DanNet(toon).Observe('Me.SPA[3]')()) ~= 0 then
  						  ImGui.Text(ico.movement)
                if toon.ID() == mq.TLO.Me.ID() then
                  if ImGui.IsItemHovered() and ImGui.IsMouseReleased(ImGuiMouseButton.Left) then
                    mq.cmd.nomore('3') -- movement spa
                  end  
                else
                  if ImGui.IsItemHovered() and ImGui.IsMouseReleased(ImGuiMouseButton.Left) then
                    mq.cmd.dexecute(toon.DisplayName() .. ' /nomore 3') -- movement spa
                  end  	  						  
                end
  						else
  						  ImGui.Text(ico.none)
  						end

  						
  				  -- CMD
  					ImGui.TableNextColumn()
    					if mq.TLO.DanNet(toon).Observe('maEnv.Find[swAuto].Value')() == 'TRUE' then
       					ImGui.Text(ico.manual)
                if ImGui.IsItemHovered() and ImGui.IsMouseReleased(ImGuiMouseButton.Left) then
                  if toon.ID() == mq.TLO.Me.ID() then
                    mq.cmd.env('auto off')
                  else
                    mq.cmd.dexecute(toon.DisplayName() .. ' /env auto off') -- turn toon auto
                  end
                end  	 
              elseif mq.TLO.DanNet(toon).Observe('maEnv.Find[swAuto].Value')() == 'FALSE' then
       					ImGui.Text(ico.auto)
                if ImGui.IsItemHovered() and ImGui.IsMouseReleased(ImGuiMouseButton.Left) then
                  if toon.ID() == mq.TLO.Me.ID() then
                    mq.cmd.env('auto on')
                  else
                    mq.cmd.dexecute(toon.DisplayName() .. ' /env auto on') -- turn toon auto
                  end
                end  	       					
              end


            -- target cons
  					ImGui.TableNextColumn()
  					  local spawn = mq.TLO.Spawn(mq.TLO.DanNet(toon).Observe('Target.ID')())
  						if spawn.ID() ~= 0 then
  							if spawn.ConColor() == 'RED' then
  								ImGui.TextColored(1, 0, 0, 1, spawn.DisplayName())
  							elseif spawn.ConColor() == 'YELLOW' then
  								ImGui.TextColored(1, 1, 0, 1, spawn.DisplayName())
  							elseif spawn.ConColor() == 'WHITE' then
  								ImGui.TextColored(1, 1, 1, 1, spawn.DisplayName())
  							elseif spawn.ConColor() == 'BLUE' then
  								ImGui.TextColored(0, 0, 1, 1, spawn.DisplayName())
  							elseif spawn.ConColor() == 'LIGHT BLUE' then
  								ImGui.TextColored(.6, .8, .90, 1, spawn.DisplayName())
  							elseif spawn.ConColor() == 'GREEN' then
  								ImGui.TextColored(0, 1, 0, 1, spawn.DisplayName())
  							elseif spawn.ConColor() == 'GREY' then
  								ImGui.TextColored(.4, .4, .4, 1, spawn.DisplayName())
  							end
  						else
  							ImGui.Text(ico.none)
  					 	end

            -- action
  					ImGui.TableNextColumn()
    					if mq.TLO.DanNet(toon).Observe('Me.Casting.ID')() ~= 'NULL' and mq.TLO.Spell(mq.TLO.DanNet(toon).Observe('Me.Casting.ID')()).Name() ~= nil then
    						ImGui.TextColored(1, 1, 1, 1, mq.TLO.Spell(mq.TLO.DanNet(toon).Observe('Me.Casting.ID')()).Name())
    					elseif mq.TLO.DanNet(toon).Observe('Me.ActiveDisc.Name')() ~= 'Null' and mq.TLO.Spell(mq.TLO.DanNet(toon).Observe('Me.ActiveDisc.Name')()).ID() ~= nil then
    					  ImGui.TextColored(1, 1, 1, 0.9, mq.TLO.DanNet(toon).Observe('Me.ActiveDisc.Name')())
    					else
    					  ImGui.Text(ico.none)
    					end
  				
  					ImGui.TableNextColumn()
  			    ImGui.Columns()
    	    end
  		end
    
      ImGui.EndTable()
  	end
  end
    	

  ImGui.End()
  
end



mq.imgui.init('drivewindow', imguicallback)

while openGUI do 
  mq.delay(250)
end