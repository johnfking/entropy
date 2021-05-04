--
-- entropoy.mac
-- buttons.lua
-- 
-- premade hotkeys
--




require "common"
local openGUI = true
-- local shouldDrawGUI = true


local function imguicallback()
  openGUI, shouldDrawGUI = ImGui.Begin('Entropy', openGUI, ImGuiWindowFlags.NoTitleBar)
  -- ImGui.SetWindowSize(100, 300)
  ImGui.Columns(10)

  -- column 1
  auto = ImGui.SmallButton("Auto")
  if auto then
    mq.cmd.env('auto')
  end




  -- column 2
  ImGui.NextColumn()
  
 


  -- column 3
  ImGui.NextColumn()

  pull = ImGui.SmallButton("Pull")
  if pull then
    mq.cmd.pull('active')
  end

  agro = ImGui.SmallButton("Agro")
  if agro then
    mq.cmd.agro('active')
  end


  -- column 4
  ImGui.NextColumn()


  -- column 5
  ImGui.NextColumn()


  -- column 6
  ImGui.NextColumn()


  -- column 7
  ImGui.NextColumn()


  -- column 8
  ImGui.NextColumn()


  -- column 9
  ImGui.NextColumn()

  -- column 10
  ImGui.NextColumn()


  gather = ImGui.Button('Gather', 55, 20)
  if gather then
    mq.cmd.gather()
  end
  
  here = ImGui.SmallButton("Here")
  if here then
    mq.cmd.here()
  end



    
    
  ImGui.End()

end


mq.imgui.init('hotkeys', imguicallback)

while true do 
  mq.delay(1) 
end
  
  



