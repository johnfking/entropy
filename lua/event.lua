--
-- entropy.mac
-- 
-- Custom Events
-- 
--



require 'common'
local openGUI = true
local shouldDrawHUD = true




local function imguicallback()
  hudInfo()
  
  openGUI, shouldDrawHUD = ImGui.Begin('custom events '..ent['build']..'###EntropyEvent', openGUI)
  
  if shouldDrawHUD and ent['event'] == '--' then
    ImGui.Text('Entropy is not running')
    ImGui.End()


  elseif shouldDrawHUD then  
    ImGui.NewLine() 

 
    if ImGui.TreeNode('Close the Gates :: Bloodfalls') then
      ImGui.NewLine()
      edit_text_perm('Hide YX', 'maEvent', 'stBloodfallsHideLocYX')
      edit_text_perm('Bugs', 'maEvent', 'stBloodfallsBuggs')
      edit_text_perm('Chased', 'maEvent', 'stBloodfallsChased')
      edit_text_perm('Drowning', 'maEvent', 'stBloodfallsDrowning')
      edit_text_perm('Falling', 'maEvent', 'stBloodfallsFalling')
      edit_text_perm('Monster', 'maEvent', 'stBloodfallsMonster')
      edit_text_perm('Curse', 'maEvent', 'stBloodfallsCurse')
      ImGui.NewLine()
      indent(1,2)
    end  

    if ImGui.TreeNode('Gorgana :: Maidens Eye') then
      edit_text_perm('Hide Locs YX', 'maEvent', 'lsMaidenHideLocYX')
      ImGui.NewLine()
      indent(1,2)
    end  

    if ImGui.TreeNode('Mad Emperor :: Kanor Anni Event') then
      edit_text_perm('Hide Loc YX', 'maEvent', 'stKarnorHideLocYX')
      ImGui.NewLine()
      indent(1,2)
    end  

    ImGui.NewLine()


  end
  ImGui.End()

end

mq.imgui.init('editorwindow', imguicallback)

while openGUI do 
  mq.delay(1000) 
end







 



