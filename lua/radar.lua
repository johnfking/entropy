--
-- entropy.mac
-- 
-- 
-- "we ain't found shit!"
--



require 'common'
local openGUI = true
local shouldDrawHUD = true


local function nodename(spawnID)
  -- local name = '##'..spawnID
  local name = ''
  for x = 1, 3, 1
  do
    if mq.TLO.Me.RaidMarkNPC(x).ID() == spawnID then 
      name = 'M'..x
    elseif mq.TLO.Me.RaidAssistTarget(x).ID() == spawnID then 
      name = 'A'..x
    elseif mq.TLO.Me.GroupMarkNPC(x).ID() == spawnID then 
      name = 'GM'..x
    end
  end

  if mq.TLO.Me.GroupAssistTarget.ID() == spawnID then
     name = 'G'
  end

  if name ~= nil then
    ImGui.SameLine()
    color("ORANGE", name)
  end
  
end

local function targetable()
  if mq.TLO.Macro.Variable('maHud').Find('swRadarSortTargetable').Value() == 'TRUE' then
    return 'targetable'
  else
    return ''
  end
end


local function imguicallback()
  hudInfo()
  openGUI, shouldDrawHUD = ImGui.Begin(ico.radar .. ' radar###EntropyRadar', openGUI)
 
  if shouldDrawHUD and (ent.build == '--' or mq.TLO.EverQuest.GameState() ~= 'INGAME') then

  elseif shouldDrawHUD then  

    settableflags()
    ImGui.Text(ico['target'])
    if mq.TLO.Target.ID() > 0  and mq.TLO.Target.ID() ~= nil then
      ImGui.SameLine()
      ImGui.TextColored(0.39, 0.58, 0.92, 1, mq.TLO.Target.Name())
    end
    ImGui.Separator()


    local petbutton = mq.TLO.Macro.Variable('maHud').Find('swRadarshowPetbutton').Value()
    local markbutton = mq.TLO.Macro.Variable('maHud').Find('swRadarshowMarkbutton').Value()
    local view = mq.TLO.Macro.Variable('maHud').Find('stRadarViewType').Value()



    local columns = 2
    if markbutton == 'TRUE' or petbutton == 'TRUE' then 
      columns = 3 
    end




       


    if view == 'spawn' then
        
      ImGui.Columns(2, 'noname', false)
        local radarcount = tonumber(mq.TLO.Macro.Variable('maHud').Find('stRadarCount').Value())
        radarcount, used = ImGui.DragInt("count##radarcount", radarcount, 1, 0, 50);
        if used then
          mq.cmd.luaedit('stRadarCount', radarcount)  
        end
      ImGui.NextColumn()
      ImGui.Columns()
      

      if ImGui.BeginTable(view, columns, tableFlags) then
        local sorttype = mq.TLO.Macro.Variable('maHud').Find('stRadarSortType').Value()
        local sortcount = tonumber(mq.TLO.Macro.Variable('maHud').Find('stRadarCount').Value())
        local totalcount = tonumber(mq.TLO.SpawnCount(""..sorttype.." "..targetable().."")())
        if sortcount > totalcount then sortcount = totalcount end
        -- area sweep for mob counts
        for x = 1, sortcount, 1 do
          ImGui.TableNextColumn()
          nearspawn = mq.TLO.NearestSpawn(x .. ", " ..sorttype.." "..targetable().."")
          if nearspawn.ID() ~= nil then
            
            if ImGui.TreeNode('##'..x) then
              ImGui.Text('ID: %d', nearspawn.ID())
              ImGui.SameLine()
              ImGui.Text('::')
              ImGui.SameLine()
              ImGui.Text(nearspawn.Body())                 
              ImGui.Text('Y %d, X %d, Z %d', round(nearspawn.Y(), 0), round(nearspawn.X(), 0), round(nearspawn.Z(), 0))
             if nearspawn.Master() ~= nil then
                ImGui.Text('Master: %s', nearspawn.Master())
                ImGui.TextColored('Owner: %s', nearspawn.Owner())
              end
              ImGui.TreePop()
              ImGui.NewLine()
            end
            ImGui.SameLine()
            ImGui.TextColored(0.39, 0.58, 0.92, 1, ""..nearspawn.Name().."")
            if ImGui.IsItemHovered() and ImGui.IsMouseReleased(ImGuiMouseButton.Left) then
              mq.cmd.target('id ' .. nearspawn.ID())
            end
            ImGui.TableNextColumn()
            
            if nearspawn.Distance() <= nearspawn.MaxRangeTo() then
              ImGui.TextColored(0, 1, 0, 1, round(nearspawn.Distance(), 0))
            else
              ImGui.TextColored(1, 0, 0, 1, round(nearspawn.Distance(), 0))
            end
 
            if markbutton == 'TRUE' or petbutton == 'TRUE' then
              ImGui.TableNextColumn()
              
              if petbutton == 'TRUE' then
                ImGui.Text(ico.pet)
                if ImGui.IsItemHovered() and ImGui.IsMouseReleased(ImGuiMouseButton.Left) then
                  mq.cmd.target('id ' .. nearspawn.ID())
                  mq.cmd.minion('force')
                end
              end
              
              if markbutton == 'TRUE' then
                if petbutton == 'TRUE' then ImGui.SameLine() end
               
                for m = 1, 3, 1 do
                  if m >= 2 then
                    ImGui.SameLine()
                  end
                  ImGui.Text(ico['num' .. m])
                  if ImGui.IsItemHovered() and ImGui.IsMouseReleased(ImGuiMouseButton.Left) then
                    mq.cmd.target('id ' .. nearspawn.ID())
                    if mq.TLO.Raid.Members() >= 1 then
                      mq.cmd.rmark(m)
                    else
                      mq.cmd.gmark(m)
                    end
                  end                     
                end
              end
            end              
          end
        end

      ImGui.EndTable()        
      end





    elseif view == 'xtarget' then

      if ImGui.BeginTable(view, columns, tableFlags) then       
        for x = 1, mq.TLO.Me.XTargetSlots(), 1 do
          xtspawn = mq.TLO.Spawn(mq.TLO.Me.XTarget(x).ID())
          ImGui.TableNextColumn()
            if xtspawn.ID() == 0 then
              ImGui.Text(x)
            else
              ImGui.Text(x)
              nodename(xtspawn.ID())
              ImGui.SameLine()
              color(xtspawn.ConColor(), xtspawn.Name())
              if ImGui.IsItemHovered() and ImGui.IsMouseReleased(ImGuiMouseButton.Left) then
                mq.cmd.xtarget(x)
              end                
              
              if mq.TLO.Target.ID() == xtspawn.ID() then
                ImGui.SameLine()
                ImGui.Text(ico.arrowleft)
              end
            end
          ImGui.TableNextColumn()
            if xtspawn.ID() ~= 0 then
              ImGui.Text(xtspawn.PctHPs())
            end

            if markbutton == 'TRUE' or petbutton == 'TRUE' then
              ImGui.TableNextColumn()
              
              if petbutton == 'TRUE' then
                ImGui.Text(ico.pet)
                if ImGui.IsItemHovered() and ImGui.IsMouseReleased(ImGuiMouseButton.Left) then
                  mq.cmd.target('id ' .. nearspawn.ID())
                  mq.cmd.minion('force')
                end
              end
              
              if markbutton == 'TRUE' then
                if petbutton == 'TRUE' then ImGui.SameLine() end
               
                for m = 1, 3, 1 do
                  if m >= 2 then
                    ImGui.SameLine()
                  end
                  ImGui.Text(ico['num' .. m])
                  if ImGui.IsItemHovered() and ImGui.IsMouseReleased(ImGuiMouseButton.Left) then
                    mq.cmd.target('id ' .. nearspawn.ID())
                    if mq.TLO.Raid.Members() >= 1 then
                      mq.cmd.rmark(m)
                    else
                      mq.cmd.gmark(m)
                    end
                  end                     
                end
              end
            end 
















        end
      end
      ImGui.EndTable()        
    end
   
  end
  ImGui.End()

end

mq.imgui.init('editorwindow', imguicallback)

while openGUI do 
  mq.delay(500) 
end







 



