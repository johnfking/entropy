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
  local name = '##'..spawnID
  for x = 1, 3, 1
  do
    if mq.TLO.Me.RaidMarkNPC(x).ID() == spawnID then name = 'm'..x
    elseif mq.TLO.Me.RaidAssistTarget(x).ID() == spawnID then name = 'a'..x
    elseif mq.TLO.Me.GroupMarkNPC(x).ID() == spawnID then name = 'g'..x
    end
  end

  if mq.TLO.Me.GroupAssistTarget.ID() == spawnID then
    name = 'g'
  end
  return name
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
  openGUI, shouldDrawHUD = ImGui.Begin(ico['radar'] .. ' radar###EntropyRadar', openGUI)
 
  if shouldDrawHUD and (ent.build == '--' or mq.TLO.EverQuest.GameState() ~= 'INGAME') then

  elseif shouldDrawHUD then  

        settableflags()
        ImGui.Text(ico['target'])
        if mq.TLO.Target.ID() > 0  and mq.TLO.Target.ID() ~= nil then
          ImGui.SameLine()
          ImGui.TextColored(0.39, 0.58, 0.92, 1, mq.TLO.Target.Name())
        end

        --ImGui.NewLine()
        if ImGui.BeginTable('spawns', 2, tableFlags) then
          local sorttype = mq.TLO.Macro.Variable('maHud').Find('stRadarSortType').Value()
          local sortcount = tonumber(mq.TLO.Macro.Variable('maHud').Find('stRadarCount').Value())
          local totalcount = tonumber(mq.TLO.SpawnCount(""..sorttype.." "..targetable().."")())
          if sortcount > totalcount then sortcount = totalcount end
          
          -- area sweep for mob counts
          for x = 1, sortcount, 1
          do
                  
            ImGui.TableNextColumn()
            nearspawn = mq.TLO.NearestSpawn(x .. ", " ..sorttype.." "..targetable().."")
            
            if nearspawn.ID() ~= nil then
              if ImGui.TreeNode(nodename(nearspawn.ID())) then
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
      
            end

          end
          ImGui.EndTable()
        end

  end
  ImGui.End()

end

mq.imgui.init('editorwindow', imguicallback)

while openGUI do 
  mq.delay(1000) 
end







 



