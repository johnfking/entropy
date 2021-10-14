--
-- entropoy.mac
-- common.lua
-- 
-- lua shared stuffs
--

mq = require('mq')
echo = mq.cmd.echo
discord = '${If[${Bool[${Plugin[MQ2Discord]}]},\at-\ax,]}'
ent = {}
target = {}
classTable = { "WAR", "PAL", "SHD", "BST", "ROG", "MNK", "RNG", "BER", "CLR", "SHM", "DRU", "WIZ", "NEC", "ENC", "MAG", "BRD" }


rez = {}
rez['radius'] = tonumber(mq.TLO.Macro.Variable('maRez').Find('stMaxRezRange').Value())




out = function (option, verbage)
  -- help response for no tags for the control
  if option == 'notag' then
    echo('${lsep}no tags for this control.')

  -- help response for no map for this control
  elseif option == 'nomap' then
    echo('${lsep}no map for this control.')
  end
end




hudInfo = function ()
  ent['inv'] = ''
  ent['ivu'] = ''

  -- invis 
  if mq.TLO.Me.SPA(12)() ~= 0 or mq.TLO.Me.SPA(314)() ~= 0 then
    ent['inv'] = 'IV'
  end
  
  -- ivu
  if mq.TLO.Me.SPA(28)() ~= 0 or mq.TLO.Me.SPA(315)() ~= 0 then
    ent['ivu'] = 'IVU'
  end
  
  -- base information
  ent['build'] = mq.TLO.Macro.Variable('maEnv').Find('build').Value() or '--'
  ent['mode'] = mq.TLO.Macro.Variable('maData').Find('mode').Value() or ''
  ent['set'] = mq.TLO.Macro.Variable('maEnv').Find('stBuildSetCurrent').Value() or '--'
  ent['body'] = mq.TLO.Target.Body.Name() or ''
  ent['auto'] = mq.TLO.Macro.Variable('maEnv').Find('swAuto').Value() or '0'
  ent['hudoutput'] = mq.TLO.Macro.Variable('HUDOutput')() or '--'

 
  -- target
  target['shortname'] = mq.TLO.Target.Class.ShortName() or '--'
  target['level'] = mq.TLO.Target.Level() or '--'
  target['los'] = mq.TLO.Target.LineOfSight() or '--'
  target['id'] = mq.TLO.Target.ID() or '--'
  target['displayname'] = mq.TLO.Target.DisplayName() or '--'
  target['guild'] = mq.TLO.Target.Guild() or '--'
  target['distance'] = mq.TLO.Target.Distance() or 0
  target['maxrangeto'] = mq.TLO.Target.MaxRangeTo() or 0 
  target['height'] = round(mq.TLO.Target.Height(), 2)
  target['name'] = mq.TLO.Target.Name() or '--'
  target['tashed'] = isTashed()
  target['slowed'] = isSlowed()
  target['maloed'] = isMaloed()
  target['crippled'] = isCrippled()
  target['snared'] = isSnared()
  -- target['ds'] = hasDS()
  

end



indent = function (count, pos)
  if pos == 1  then
    if count < 10 then
      ImGui.Indent(16)
    else
      ImGui.Indent(24)
    end
  else
    if count < 10 then
      ImGui.Indent(-16)
    else
      ImGui.Indent(-24)
    end
  end
end
 
  
edit_switch_temp = function (name, map, var)
  local switch, checked = ImGui.Checkbox(name..'##'..var, mq.TLO.Macro.Variable(map).Find(var).Value() == 'TRUE')
  if checked then
    mq.cmd.invoke('${'..map..'.Add['..var..','..(switch and 'TRUE' or 'FALSE')..']}')
  end
end


edit_switch_perm = function (name, map, var)
  local switch, checked = ImGui.Checkbox(name..'##'..var, mq.TLO.Macro.Variable(map).Find(var).Value() == 'TRUE')
  if checked then
    mq.cmd.luaedit(var, switch and 'TRUE' or 'FALSE')
  end
end


edit_text_perm = function (name, map, var)
  local line, selected = ImGui.InputTextWithHint(name..'##'..var, mq.TLO.Macro.Variable(map).Find(var).Value(), '', ImGuiInputTextFlags.EnterReturnsTrue)
  if selected then
    mq.cmd.luaedit(var, line, 'overwrite')
  end  
end


edit_tree = function (count, var, alias)
  
  if ImGui.TreeNode('list') then
    ImGui.NewLine()
    for i=1,50 do 
      ImGui.TextColored(0.39, 0.58, 0.92, 1, i)
      ImGui.SameLine()
      if mq.TLO.Macro.Variable('ma'..var).Find('sw'..var..i).Value() == 'TRUE' then
        local switch, pressed = ImGui.Checkbox('##sw'..var..i, mq.TLO.Macro.Variable('ma'..var).Find('sw'..var..i).Value() == 'TRUE')
        if pressed then
          mq.cmd.luaedit('sw'..var..i, switch and 'TRUE' or 'FALSE')
        end
        
        indent(i,1)

        local line, selected = ImGui.InputTextWithHint('name##st'..var..i, mq.TLO.Macro.Variable('ma'..var).Find('st'..var..i).Value(), '', ImGuiInputTextFlags.EnterReturnsTrue)
        if selected then
          mq.cmd.luaedit('st'..var..i, line)
        end
        
        if alias == 1 then
          local line, selected = ImGui.InputTextWithHint('alias##st'..var..i..'Alias', mq.TLO.Macro.Variable('ma'..var).Find('st'..var..i..'Alias').Value(), '', ImGuiInputTextFlags.EnterReturnsTrue)
          if selected then
            mq.cmd.luaedit('st'..var..i..'Alias', line)
          end
        end

        local line, selected = ImGui.InputTextWithHint('tag##ls'..var..i..'Tag', mq.TLO.Macro.Variable('ma'..var).Find('ls'..var..i..'Tag').Value(), '', ImGuiInputTextFlags.EnterReturnsTrue)
        if selected then
          mq.cmd.luaedit('ls'..var..i..'Tag', 'ls'..var..i..Tag, 'overwrite')
        end
        
        indent(i,2)
        
      else 
        if mq.TLO.Macro.Variable('ma'..var).Find('st'..var..i).Value() == 'FALSE' then
          tmpName = '##sw'..var..i
        else
          tmpName = mq.TLO.Macro.Variable('ma'..var).Find('st'..var..i).Value()..'##sw'..var..i
        end            
        
        local switch, pressed = ImGui.Checkbox(tmpName, mq.TLO.Macro.Variable('ma'..var).Find('swBuff'..i).Value() == 'TRUE')
        if pressed then
          mq.cmd.luaedit('sw'..var..i, switch and 'TRUE' or 'FALSE')
        end
      end
    end
    ImGui.TreePop()      
  end

end


function round(num, numDecimalPlaces)
  if num == nil then
    return 0
  end
  local mult = 10^(numDecimalPlaces or 0)
  return math.floor(num * mult + 0.5) / mult
end


function isSlowed()
  if mq.TLO.Target.Slowed.ID() == nil then
    return '--'
  end
  return 100 - mq.TLO.Target.Slowed.Base(2)()..'%% ('..mq.TLO.Target.BuffDuration(mq.TLO.Target.Slowed()).TotalSeconds()..'s)'
end


function isTashed()
  if mq.TLO.Target.Tashed.ID() == nil then
    return '--'
  end
  return mq.TLO.Target.Tashed.Base(2)()..'ac ('..mq.TLO.Target.BuffDuration(mq.TLO.Target.Tashed()).TotalSeconds()..'s)'
end


function isMaloed()
  if mq.TLO.Target.Maloed.ID() == nil then
    return '--'
  end
  return '+'..mq.TLO.Target.Maloed.Base(1)()..'%% ('..mq.TLO.Target.BuffDuration(mq.TLO.Target.Maloed()).TotalSeconds()..'s)'
end


function isCrippled()
  if mq.TLO.Target.FindBuff('subcat Disempowering').ID() == nil then
    return '--'
  end
  return mq.TLO.Target.FindBuff('subcat Disempowering').Base(4)()..' ('..mq.TLO.Target.BuffDuration(mq.TLO.Target.FindBuff('subcat Disempowering')()).TotalSeconds()..'s)'
end


function isSnared()
  if mq.TLO.Target.Snared.ID() == nil then
    return '--'
  end
  return mq.TLO.Target.Snared.Base(2)()..'%% ('..mq.TLO.Target.BuffDuration(mq.TLO.Target.Snared()).TotalSeconds()..'s)'
end


function hasDS()
  if mq.TLO.Target.DSed.ID() == nil then
    return '--'
  end
  return mq.TLO.Target.DSed.Base(2)()..' ('..mq.TLO.Target.BuffDuration(mq.TLO.Target.DSed()).TotalSeconds()..'s)'
end


function hasLoS()
  if mq.TLO.Target.LineOfSight() == nil then
    return '--'
  end
  return mq.TLO.Target.LineOfSight()
end


