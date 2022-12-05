--
-- entropoy.mac
-- notes.lua
-- 
-- notes for EQ
--


require 'common'

local scriptPath = (debug.getinfo(1, "S").source:sub(2)):match("(.*[\\|/]).*$")
package.path = package.path .. ';' .. scriptPath .. '?.lua'
local resourcePath = mq.TLO.MacroQuest.Path('Resources')()
local luasharePath = resourcePath .. "\\luarocks\\share\\lua\\5.1\\"
local lualibPath = resourcePath .. "\\luarocks\\lib\\lua\\5.1\\"
package.path = package.path .. ';' .. luasharePath .. '?.lua'
package.cpath = package.cpath .. ';' .. lualibPath .. '?.dll'
local sql = require('lsqlite3')





local openGUI = true
local shouldDrawGUI = true

notecount = tonumber(mq.TLO.Macro.Variable('notescount')())

multitext = {}
for i = 1, notecount do  
  multitext[i] = mq.TLO.Macro.Variable('note'..i..'text')()
end

-- print(notecount)



local imguicallback = function()
  openGUI, shouldDrawGUI = ImGui.Begin('/notes', openGUI)
  if shouldDrawGUI then
    ImGui.NewLine()



  local addathing = ImGui.Button('+ Note', 60, 22)
  if addathing then
    mq.cmd('/notes add')
    -- print(notecount)
    notecount = notecount + 1
    -- print(notecount)
    for i = 1, notecount do
      multitext[i] = mq.TLO.Macro.Variable('note'..i..'text')()
     end    
  end

  ImGui.NewLine()

  if notecount > 0 then 
      
      for i = 1, notecount-1 do 
        if ImGui.TreeNode(mq.TLO.Macro.Variable('note'..i..'name')()) then 
          
          ImGui.NewLine()
          
          
          -- edit title
          local line, selected = ImGui.InputTextWithHint('##notes'..i..'name', mq.TLO.Macro.Variable('note'..i..'name')(), '', ImGuiInputTextFlags.EnterReturnsTrue)
          if selected then
            mq.cmd.notes(i, 'title', '"'..line..'"')
          end  
          ImGui.NewLine()
          
          
          -- update note body
          multitext[i], selected = ImGui.InputTextMultiline('##Label'..i, multitext[i], 800, 400)
          local noteupdate = ImGui.SmallButton('Update')
          if noteupdate then
            mq.cmd.notes(i, '"'..multitext[i]..'"')
          end
        
        
          ImGui.NewLine()
          
          
        ImGui.TreePop()
        end
      end
    end
  end
  ImGui.End()



end

mq.imgui.init('Notes', imguicallback)

while openGUI do 
  mq.delay(1000)
end


