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
		local ColumnID_Auto = 9
		local ColumnID_Event = 10
		local ColumnID_Target = 11
		local ColumnID_Cast = 12

		settableflags()			
  	if ImGui.BeginTable('Crew', 13, tableFlags) then

  		ImGui.TableSetupColumn('Name', ImGuiTableColumnFlags.DefaultSort, 0.0, ColumnID_Name)
  		ImGui.TableSetupColumn('HP', ImGuiTableColumnFlags.DefaultSort, 0.0, ColumnID_HP)
  		ImGui.TableSetupColumn('MP', ImGuiTableColumnFlags.DefaultSort, 0.0, ColumnID_Mana)
  		ImGui.TableSetupColumn('EP', ImGuiTableColumnFlags.DefaultSort, 0.0, ColumnID_End)
  		ImGui.TableSetupColumn('Dist', ImGuiTableColumnFlags.DefaultSort, 0.0, ColumnID_Dist)
  		ImGui.TableSetupColumn('Lev', ImGuiTableColumnFlags.DefaultSort, 0.0, ColumnID_Lev)
  		ImGui.TableSetupColumn('Inv', ImGuiTableColumnFlags.DefaultSort, 0.0, ColumnID_Invis)
  		ImGui.TableSetupColumn('Ivu', ImGuiTableColumnFlags.DefaultSort, 0.0, ColumnID_IVU)
  		ImGui.TableSetupColumn('Mov', ImGuiTableColumnFlags.DefaultSort, 0.0, ColumnID_Movement)
  		ImGui.TableSetupColumn('Auto', ImGuiTableColumnFlags.DefaultSort, 0.0, ColumnID_Auto)
  		ImGui.TableSetupColumn('Event', ImGuiTableColumnFlags.DefaultSort, 0.0, ColumnID_Event)
  		ImGui.TableSetupColumn('Target', ImGuiTableColumnFlags.DefaultSort, 0.0, ColumnID_Target)
  		ImGui.TableSetupColumn('Action', ImGuiTableColumnFlags.DefaultSort, 0.0, ColumnID_Cast)
  		ImGui.TableHeadersRow()
  		ImGui.TableNextRow()	
  		ImGui.TableNextColumn()

    -- column: toon names & state: Green = on | purple = no macro | yella = off | red's ded baby


  		for i = 1, mq.TLO.DanNet.PeerCount(ent.channel)() 
  			do
  			  
  			  local toonName = mq.TLO.DanNet.Peers(ent.channel).Arg(i,'|')()
  			  if toonName ~= nil then

    			  local toon = mq.TLO.Spawn('pc ' .. toonName)
     			  if toon() ~= nil then
              -- #1
              
    					if toon.Dead() then
    						ImGui.TextColored(1, 0, 0, 1, toon.DisplayName()) 
    					elseif mq.TLO.DanNet(toon).Observe('maEnv.Find[swAuto].Value')() == 'TRUE' then
    						ImGui.TextColored(0, 1, 0, 1, toon.DisplayName())
    					elseif mq.TLO.DanNet(toon).Observe('maEnv.Find[swAuto].Value')() == 'FALSE' then
    						ImGui.TextColored(0.39, 0.58, 0.92, 1, toon.DisplayName())
    					else
    						ImGui.TextColored(0.90, 0.20, 0.92, 1, toon.Name())
    					end  

      				-- #2
      				ImGui.TableNextColumn()
      					if toon.PctHPs() >= 70 then
      						ImGui.TextColored(0, 1, 0, 1, toon.PctHPs())
      					elseif toon.PctHPs() < 70 and toon.PctHPs() >= 40 then
      						ImGui.TextColored(1, 1, 0, 1, toon.PctHPs())
      				  elseif toon.PctHPs() < 40 then
      						ImGui.TextColored(1, 0, 0, 1, toon.PctHPs())
      					end

    				  
    				  -- #3 pct manna
    					ImGui.TableNextColumn()
                if toon.PctMana() >= 65 then
    							ImGui.TextColored(0, 1, 0, 1, toon.PctMana())
    						elseif toon.PctMana() < 65 and toon.PctMana() >= 30 then
    							ImGui.TextColored(1, 1, 0, 1, toon.PctMana())
    						elseif toon.PctMana() < 30 then
    							ImGui.TextColored(1, 0, 0, 1, toon.PctMana())
    					  end

            
              -- #4 pct endurance
    					ImGui.TableNextColumn()
    						if toon.PctEndurance() >= 65 then
    							ImGui.TextColored(0, 1, 0, 1, toon.PctEndurance())
    						elseif toon.PctEndurance() < 65 and toon.PctEndurance() >= 30 then
    							ImGui.TextColored(1, 1, 0, 1, toon.PctEndurance())
    							elseif toon.PctEndurance() < 30 then
    							ImGui.TextColored(1, 0, 0, 1, toon.PctEndurance())
    						end

    					
    					-- #5 distance		
    					ImGui.TableNextColumn()
    					
    					  if toon.ID() == mq.TLO.Me.ID() then
    					    ImGui.TextColored(0.39, 0.58, 0.92, 1, ico['none'])
    					  else 
                  ImGui.TextColored(1, 1, 1, 1, string.format('%.1f', toon.Distance()))
                  if ImGui.IsItemHovered() and ImGui.IsMouseReleased(ImGuiMouseButton.Left) then
                    mq.cmd.dexecute(toon.DisplayName() .. ' /nav id ' .. mq.TLO.Me.ID() .. ' |log=off')
                  end
    					  end
    					
              -- #6 levitate
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

    							  
    				  -- #7 invis
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


              -- #8 IVU
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


              -- #9 movement
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

    						
    				  -- #10 Auto
    					ImGui.TableNextColumn()
      					if mq.TLO.DanNet(toon).Observe('maEnv.Find[swAuto].Value')() == 'TRUE' then
         					ImGui.Text(ico.manual)
                  if ImGui.IsItemHovered() and ImGui.IsMouseReleased(ImGuiMouseButton.Left) then
                    if toon.ID() == mq.TLO.Me.ID() then
                      mq.cmd.env('auto off')
                    else
                      mq.cmd.dexecute(toon.DisplayName() .. ' /env auto off')
                    end
                  end  	 
                elseif mq.TLO.DanNet(toon).Observe('maEnv.Find[swAuto].Value')() == 'FALSE' then
         					ImGui.Text(ico.auto)
                  if ImGui.IsItemHovered() and ImGui.IsMouseReleased(ImGuiMouseButton.Left) then
                    if toon.ID() == mq.TLO.Me.ID() then
                      mq.cmd.env('auto on')
                    else
                      mq.cmd.dexecute(toon.DisplayName() .. ' /env auto on')
                    end
                  end  	       					
                end


    				  -- #11 Event
    					ImGui.TableNextColumn()
                
                -- banner event
       					ImGui.Text(ico.banner)
                if ImGui.IsItemHovered() and ImGui.IsMouseReleased(ImGuiMouseButton.Left) then
                  if toon.ID() == mq.TLO.Me.ID() then
                    mq.cmd.env('event banner')
                  else
                    mq.cmd.dexecute(toon.DisplayName() .. ' /event banner')
                  end
                end  	               
                
                -- campfire move
                ImGui.SameLine()
       					ImGui.Text(ico.campfire)
                if ImGui.IsItemHovered() and ImGui.IsMouseReleased(ImGuiMouseButton.Left) then
                  if toon.ID() == mq.TLO.Me.ID() then
                    mq.cmd.env('event campfire')
                  else
                    mq.cmd.dexecute(toon.DisplayName() .. ' /event campfire')
                  end
                end  	               
                
                -- event hide
                ImGui.SameLine()
       					ImGui.Text(ico.hide)
                if ImGui.IsItemHovered() and ImGui.IsMouseReleased(ImGuiMouseButton.Left) then
                  if toon.ID() == mq.TLO.Me.ID() then
                    mq.cmd.env('event hide')
                  else
                    mq.cmd.dexecute(toon.DisplayName() .. ' /event hide')
                  end
                end             
                
                -- event aura
                ImGui.SameLine()
       					ImGui.Text(ico.aura)
                if ImGui.IsItemHovered() and ImGui.IsMouseReleased(ImGuiMouseButton.Left) then
                  if toon.ID() == mq.TLO.Me.ID() then
                    mq.cmd.env('event aura')
                  else
                    mq.cmd.dexecute(toon.DisplayName() .. ' /event aura')
                  end
                end                
                
                -- event home
                ImGui.SameLine()
       					ImGui.Text(ico.home)
                if ImGui.IsItemHovered() and ImGui.IsMouseReleased(ImGuiMouseButton.Left) then
                  if toon.ID() == mq.TLO.Me.ID() then
                    mq.cmd.env('event home')
                  else
                    mq.cmd.dexecute(toon.DisplayName() .. ' /event home')
                  end
                end  
                
                -- move and say
                ImGui.SameLine()
       					ImGui.Text(ico.say)
                if ImGui.IsItemHovered() and ImGui.IsMouseReleased(ImGuiMouseButton.Left) then
                  if toon.ID() == mq.TLO.Me.ID() then
                    mq.cmd.env('event say')
                  else
                    mq.cmd.dexecute(toon.DisplayName() .. ' /event say')
                  end
                end  


              -- #12 target cons
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

              -- #13 action
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
  		end
      ImGui.EndTable()
  	end

    -- start buttons
    -- buttons at the bottom
    if mq.TLO.Macro.Variable('maHud').Find('swHUDDrivebuttons').Value() == "TRUE" then
      ImGui.Separator()


      ImGui.Columns(7, 'buttons', false)
        cmd_button(ico.allauto, ico.x, ico.y, 'on', 'all macro auto')
        ImGui.SameLine()
        cmd_button(ico.allmanual, ico.x, ico.y, 'off', 'all macro manual')
        ImGui.SameLine()
        cmd_button(ico.mqp, ico.x, ico.y, 'dga /mqp', 'all macro pause')

      -- 2
      ImGui.NextColumn()
        cmd_button(ico.tie, ico.x, ico.y, 'dga /tie', '/tie on/off')
        ImGui.SameLine()
        cmd_button(ico.tienav, ico.x, ico.y, 'dga /tie nav', 'tie with nav')
        ImGui.SameLine()
        cmd_button(ico.tiestick, ico.x, ico.y, 'dga /tie stick', 'tie with stick')

      --3
      ImGui.NextColumn()
       if mq.TLO.Macro.Variable('maEnv').Find('swAuto').Value() == 'TRUE' then
          cmd_button(ico.manual, ico.x, ico.y, 'env auto', 'macro manual')
        else 
          cmd_button(ico.auto, ico.x, ico.y, 'env auto', 'macro auto')
        end
        ImGui.SameLine()
        cmd_button(ico.incharge, ico.x, ico.y, 'env incharge', '/incharge')
        ImGui.SameLine()
        cmd_button(ico.campfire, ico.x, ico.y, 'home campfire incharge', 'drop campfire')

      -- 4
      ImGui.NextColumn()
        cmd_button(ico.invis, ico.x, ico.y, 'invis', 'stack invisibility')
        ImGui.SameLine()
        cmd_button(ico.noinvis, ico.x, ico.y, 'dga /nomore invis', 'remove all invisibility')
        ImGui.SameLine()
        cmd_button(ico.nolevi, ico.x, ico.y, 'dga /nomore levi', 'remove all levitation')

      -- 5
      ImGui.NextColumn()
        --local myname = mq.TLO.Me.DisplayName() or ''
        cmd_button(ico.assist, ico.x, ico.y, 'dge /assist ' .. (mq.TLO.Me.DisplayName() or ' '), 'all /assist')
        ImGui.SameLine()
        cmd_button(ico.hail, ico.x, ico.y, 'dga /hail', 'all /hail')
        ImGui.SameLine()
        cmd_button(ico.intpull, ico.x, ico.y, 'pull one int', 'single intervention pull')

      -- 6
      ImGui.NextColumn()
        cmd_button(ico.autoinventory, ico.x, ico.y, 'dga /autoinventory', 'everyone autoinventoy')
        ImGui.SameLine()
        cmd_button(ico.gather, ico.x, ico.y, 'gather', '/gather')
        ImGui.SameLine()
        cmd_button(ico.here, ico.x, ico.y, 'here', '/here')

      -- 7
      ImGui.NextColumn()
        cmd_button(ico.radar, ico.x, ico.y, 'ui2 radar', 'ui: radar')


      ImGui.Columns()
    end
 	
  end

  ImGui.End()
  
end



mq.imgui.init('drivewindow', imguicallback)

while openGUI do 
  mq.delay(250)
end