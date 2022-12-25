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

outs = {}
outs['nopath'] = ' \a-w[\ax\arno valid path\ax\a-w]\ax'
outs['move'] = '\a-tMove:\ax '

function out (option, verbage)
  -- help response for no tags for the control
  if option == 'notag' then
    echo('${lsep}no tags for this control.')

  -- help response for no map for this control
  elseif option == 'nomap' then
    echo('${lsep}no map for this control.')
  end
end




function hudInfo ()

  ent['hudam'] = ''
  ent['hudtoonname'] = ''
  ent['hudivu'] = ''
  ent['hudinv'] = ''
  ent['hudbuild'] = ''
  ent['hudadj'] =  mq.TLO.Macro.Variable('maHeal').Find('stHealAdjust').Value() or ' 0'
  ent['hudent'] = ''
  ent['dash'] = ''
  ent['build'] = mq.TLO.Macro.Variable('maEnv').Find('build').Value() or '--'
  ent['channel'] = mq.TLO.Macro.Variable('maEntropy').Find('stEntropyGroup_all').Value()


  -- buid hud variables
  if ent['build'] ~= '--' then 

    -- title?
    if mq.TLO.Macro.Variable('maHud').Find('swTitleEnt').Value() == 'TRUE' then
      ent['hudent'] = 'Entropy'
      ent['dash'] = ' - '
    end

    -- build
    if mq.TLO.Macro.Variable('maHud').Find('swTitleBuild').Value() == 'TRUE' then
      ent['hudbuild'] = ent['dash']..ent['build']
      ent['dash'] = ' - '
    end

    -- toon name?
    if mq.TLO.Macro.Variable('maHud').Find('swTitleName').Value() == 'TRUE' then
      ent['hudtoonname'] = ent['dash']..mq.TLO.Me.DisplayName()
      ent['dash'] = ' - '
    end  

    -- auto on or off?
    if mq.TLO.Macro.Variable('maHud').Find('swTitleAM').Value() == 'TRUE' then
      if mq.TLO.Macro.Variable('maEnv').Find('swAuto').Value() == 'TRUE' then
        ent['hudam'] = ent['dash']..'auto'
        ent['dash'] = ' - '
      else
        ent['hudam'] = ent['dash']..'manual'
        ent['dash'] = ' - '
      end
    end

    -- invis 
    if mq.TLO.Macro.Variable('maHud').Find('swTitleIV').Value() == 'TRUE' then
      if mq.TLO.Me.SPA(12)() ~= 0 or mq.TLO.Me.SPA(314)() ~= 0 then
        ent['hudinv'] = ent['dash']..'IV'
        ent['dash'] = ' - '
        ent['inv'] = 'IV'
      else
        ent['inv'] = ''
      end
    end
    
    -- ivu
    if mq.TLO.Macro.Variable('maHud').Find('swTitleIVU').Value() == 'TRUE' then
      if mq.TLO.Me.SPA(28)() ~= 0 or mq.TLO.Me.SPA(315)() ~= 0 then
        ent['hudivu'] = ent['dash']..'IVU'
        ent['dash'] = ' - '
        ent['ivu'] = 'IVU'
      else
        ent['ivu'] = ''
      end
    end
    
    -- adj
    if mq.TLO.Macro.Variable('maHud').Find('swTitleHealAdj').Value() == 'TRUE' then
      if ent['hudadj'] ~= '0' then
        ent['hudadj'] = ent['dash']..ent['hudadj']
        ent['dash'] = ' - '
      else
        ent['hudadj'] = ''
      end
    else
      if mq.TLO.Macro.Variable('maHud').Find('swTitleHealAdj').Value() ~= 'TRUE' then
        ent['hudadj'] = ''
      end
    end
    
    
    -- build hud string
    ent['hudtitle'] = ent['hudent']..ent['hudbuild']..ent['hudtoonname']..ent['hudam']..ent['hudinv']..ent['hudivu']..ent['hudadj']

  else
    ent['hudtitle'] = 'Off'
  end



  
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
  -- target['validloc'] = mq.TLO.EverQuest.ValidLoc(string.format("%f %f %f", mq.TLO.Target.X(), mq.TLO.Target.Y(), mq.TLO.Target.Z())) or '--'
  target['validloc'] = mq.TLO.EverQuest.ValidLoc(mq.TLO.Target.ID()) or '--'
  target['tashed'] = isTashed()
  target['slowed'] = isSlowed()
  target['maloed'] = isMaloed()
  target['crippled'] = isCrippled()
  target['snared'] = isSnared()
  -- target['ds'] = hasDS()
  

end



function indent (count, pos)
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
 
 -- temporary switch 
function edit_switch_temp (name, map, var)
  local switch, checked = ImGui.Checkbox(name..'##'..var, mq.TLO.Macro.Variable(map).Find(var).Value() == 'TRUE')
  if checked then
    mq.cmd.invoke('${'..map..'.Add['..var..','..(switch and 'TRUE' or 'FALSE')..']}')
  end
end

-- simple switchbox
function edit_switch_perm (name, map, var)
  local switch, checked = ImGui.Checkbox(name..'##'..var, mq.TLO.Macro.Variable(map).Find(var).Value() == 'TRUE')
  if checked then
    mq.cmd.luaedit(var, switch and 'TRUE' or 'FALSE')
  end
end

-- blank checkbox with a command button as the name
function edit_switch_perm_cmd (name, map, var, cmd)
  local switch, checked = ImGui.Checkbox('##'..var, mq.TLO.Macro.Variable(map).Find(var).Value() == 'TRUE')
  if checked then
    mq.cmd.luaedit(var, switch and 'TRUE' or 'FALSE')
  end
  ImGui.SameLine()
  local _button = ImGui.Button(name, 60, 22)
  if _button then
    mq.cmd('/'..cmd)
  end  
  
end

-- edit text variable
function edit_text_perm (name, map, var)
  if mq.TLO.Macro.Variable(map).Find(var).Value() == 'FALSE' then 
    map = '--'
  end
  
  
  
  local line, selected = ImGui.InputTextWithHint(name..'##', tostring(mq.TLO.Macro.Variable(map).Find(var).Value()), '', ImGuiInputTextFlags.EnterReturnsTrue)
  if selected then
    mq.cmd.luaedit(var, '"'..line..'"', 'overwrite')
  end  
end


function cmd_button (name, y, x, cmd)
  local cb = ImGui.Button(name, y, x)
  if cb then
    mq.cmd('/'..cmd)
  end
end

function cmd_button_all (name, y, x, cmd)
 local off = ImGui.Button(name, y, x)
  if off then
    mq.cmd.dgae('/' .. cmd)
  end
end





function edit_tree (count, var, alias)
  
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


function isCrippled ()
  if mq.TLO.Target.FindBuff('subcat Disempowering').ID() == nil then
    return '--'
  end
  return mq.TLO.Target.FindBuff('subcat Disempowering').Base(4)()..' ('..mq.TLO.Target.BuffDuration(mq.TLO.Target.FindBuff('subcat Disempowering')()).TotalSeconds()..'s)'
end


function isSnared ()
  if mq.TLO.Target.Snared.ID() == nil then
    return '--'
  end
  return mq.TLO.Target.Snared.Base(2)()..'%% ('..mq.TLO.Target.BuffDuration(mq.TLO.Target.Snared()).TotalSeconds()..'s)'
end


function hasDS ()
  if mq.TLO.Target.DSed.ID() == nil then
    return '--'
  end
  return mq.TLO.Target.DSed.Base(2)()..' ('..mq.TLO.Target.BuffDuration(mq.TLO.Target.DSed()).TotalSeconds()..'s)'
end


function hasLoS ()
  if mq.TLO.Target.LineOfSight() == nil then
    return '--'
  end
  return mq.TLO.Target.LineOfSight()
end



info_about = function ()
  ImGui.TextColored(0.39, 0.58, 0.92, 1, 'Written By: ')
  ImGui.SameLine()
  ImGui.TextColored(1, 1, 1, 1, 'Exspes')        
  ImGui.TextColored(0.39, 0.58, 0.92, 1, 'Discord: ')
  ImGui.SameLine()
  ImGui.TextColored(1, 1, 1, 1, 'exspes#1620')
  ImGui.TextColored(0.39, 0.58, 0.92, 1, 'Site: ')
  ImGui.SameLine()
  ImGui.TextColored(1, 1, 1, 1, 'https://entropy.exspes.com/')
  ImGui.TextColored(0.39, 0.58, 0.92, 1, 'Donate: ')
  ImGui.SameLine()
  ImGui.TextColored(1, 1, 1, 1, 'paypal.me/exspes')
end


function sleep (a) 
  local sec = tonumber(os.clock() + a); 
  while (os.clock() < sec) do 
  end 
end


-- check range
rangeCheck = function (targetRange)
  
  return targetRange <= tonumber(mq.TLO.Macro.Variable('maEnv').Find('stEnvRadius').Value())
  -- return memberRange <= math.max(unpack(range))
end
