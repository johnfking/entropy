--
-- entropoy.mac
-- common.lua
-- 
-- lua shared stuffs
--

mq = require('mq')
ICON = require('mq/icons')

discord = '${If[${Bool[${Plugin[MQ2Discord]}]},\at-\ax,]}'
classTable = { "WAR", "PAL", "SHD", "BST", "ROG", "MNK", "RNG", "BER", "CLR", "SHM", "DRU", "WIZ", "NEC", "ENC", "MAG", "BRD" }


rez = {
  radius = tonumber(mq.TLO.Macro.Variable('maRez').Find('stMaxRezRange').Value() or 0)
}

outs = {
  num = '\a-t',
  submenu = '\a#1E90FF->\ax',
  r0 = '\ar0\ax',
  erroro = '\arERROR\ax',
  tar = '\a-w',
  pc = '\a-w',
  sp = '\a-r',
  bad = '\ar',
  good = '\ag',
  bug = '\at',
  info = '\aw',
  sep = '\a#AA00AA::\ax',
  dot = '\at.\ax',
  on = '\agon\ax',
  off = '\aroff\ax',
  warning = '\arWARNING\ax',
  fatal = '\arFATAL ERROR\ax \at..\ax \arENDING MACRO\ax',
  caution = '\arCAUTION\ax',
  trueo = '\arTRUE\ax',
  falseo = '\arFALSE\ax',
  destroy = '\arDESTROY\ax',
  leave = '\awLeave\ax',
  add = '\agAdd\ax',
  keep = '\agKeep\ax',
  notice = '\ayNotice\ax',
  combat = '\amCombat\ax',
  removing = '\arRemoving\ax',
  broadcast = 'Broadcast',
  breako = '${sep}\a#FF06BBbreak\ax${sep}',
  lsep = 'a#00FFFF :: \a#C0C0C0',
  enabled = '\agenabled\ax',
  disabled = '\ardisabled\ax'

}

ico = {
  none = '',
  y = 22,
  x = 22,
  invis = ICON.FA_EYE,
  invisvsundead = ICON.MD_REMOVE_RED_EYE,
  levi = ICON.MD_FILTER_DRAMA,
  movement = ICON.MD_DIRECTIONS_RUN,
  here = ICON.MD_LOCATION_ON,
  noinvis = ICON.FA_EYE_SLASH,
  nolevi = ICON.MD_CLOUD_OFF,
  auto = ICON.FA_TOGGLE_OFF,
  manual = ICON.FA_TOGGLE_ON,
  allauto = ICON.FA_PLAY,
  allmanual = ICON.FA_STOP,
  tie = ICON.MD_GESTURE,
  tienav = ICON.MD_NAVIGATION,
  tiestick = ICON.FA_STICKY_NOTE,
  incharge = ICON.MD_INSERT_LINK,
  intpull = ICON.MD_EXPOSURE_PLUS_1,
  gather = ICON.FA_USERS,
  autoinventory = ICON.MD_IMPORT_CONTACTS,
  hail = ICON.FA_WEIXIN,
  assist = ICON.FA_EXCHANGE,
  campfire = ICON.FA_FIRE,
  mqp = ICON.FA_PAUSE,
  home = ICON.MD_HOME,
  cure = ICON.FA_BUG,
  say = ICON.MD_CHAT,
  banner = ICON.FA_FLAG_O,
  hide = ICON.MD_DIRECTIONS_RUN,
  aura = ICON.MD_PANORAMA_FISH_EYE,
  drive = ICON.MD_DRIVE_ETA,
  radar = ICON.MD_SETTINGS_INPUT_ANTENNA,
  target = ICON.FA_BULLSEYE,
  event = ICON.MD_EVENT,
  notes = ICON.MD_NOTE,
  arrowleft = ICON.MD_ARROW_BACK,
  pet = ICON.MD_PETS,
  num1 = ICON.MD_FILTER_1,
  num2 = ICON.MD_FILTER_2,
  num3 = ICON.MD_FILTER_3
}



ent = {}


function hudInfo ()

  local swBreak = false
  ent.dash = ' - '
  ent.hudinv = ''
  ent.hudtoonname = ''
  ent.hudbuild = ''
  ent.hudam = ''
  ent.hudinv = ''
  ent.hudivu = ''
  ent.hudent = ''
  ent.hudadj = tonumber(mq.TLO.Macro.Variable('maHeal').Find('stHealAdjust').Value()) or 0
  ent.build = mq.TLO.Macro.Variable('maEnv').Find('build').Value() or '--'
  ent.channel = mq.TLO.Macro.Variable('maEntropy').Find('stEntropyGroup_all').Value()
  ent.mode = mq.TLO.Macro.Variable('maData').Find('mode').Value() or ''
  ent.set = mq.TLO.Macro.Variable('maEnv').Find('stBuildSetCurrent').Value() or '--'
  ent.auto = mq.TLO.Macro.Variable('maEnv').Find('swAuto').Value() or '0'

  -- buid hud variables
  if ent.build ~= '--' then 

    -- title?
    if mq.TLO.Macro.Variable('maHud').Find('swTitleEnt').Value() == 'TRUE' then
      ent.hudent = 'Entropy'
      swBreak = true
    end

    -- build
    if mq.TLO.Macro.Variable('maHud').Find('swTitleBuild').Value() == 'TRUE' then
      ent.hudbuild = (swBreak and ent.dash or '') .. ent.build
      swBreak = true
    end

    -- toon name?
    if mq.TLO.Macro.Variable('maHud').Find('swTitleName').Value() == 'TRUE' then
      ent.hudtoonname = (swBreak and ent.dash or '') .. mq.TLO.Me.DisplayName()
      swBreak = true
    end  

    -- auto on or off?
    if mq.TLO.Macro.Variable('maHud').Find('swTitleAM').Value() == 'TRUE' then
      if mq.TLO.Macro.Variable('maEnv').Find('swAuto').Value() == 'TRUE' then
        ent.hudam = (swBreak and ent.dash or '') .. 'auto'
        swBreak = true
      else
        ent.hudam = (swBreak and ent.dash or '') .. 'manual'
        swBreak = true
      end
    end

    -- invis 
    if mq.TLO.Macro.Variable('maHud').Find('swTitleIV').Value() == 'TRUE' then
      if mq.TLO.Me.SPA(12)() ~= 0 or mq.TLO.Me.SPA(314)() ~= 0 then
        ent.hudinv = (swBreak and ent.dash or '') .. ico.invis
        swBreak = true
      end
    end
    
    -- ivu
    if mq.TLO.Macro.Variable('maHud').Find('swTitleIVU').Value() == 'TRUE' then
      if mq.TLO.Me.SPA(28)() ~= 0 or mq.TLO.Me.SPA(315)() ~= 0 then
        ent.hudivu = (swBreak and ent.dash or '') .. ico.invisvsundead
        swBreak = true
      end
    end
    
    -- heal adjust
    if mq.TLO.Macro.Variable('maHud').Find('swTitleHealAdj').Value() == 'TRUE' then
      if ent.hudadj ~= 0 and swBreak then 
        ent.hudadj = ent.dash .. ent.hudadj
      else
        ent.hudadj = ''
      end
    else 
      ent.hudadj = ''
    end

    -- build hud string
    ent.hudtitle = ent.hudent .. ent.hudbuild .. ent.hudtoonname .. ent.hudam .. ent.hudinv .. ent.hudivu .. ent.hudadj

  else
    ent.hudtitle = '---'
  end

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


function cmd_button (name, y, x, cmd, tooltip)
  local cb = ImGui.Button(name, y, x)
  if cb then
    mq.cmd('/'..cmd)
  end
  if tooltip ~= nil then
    if ImGui.IsItemHovered() then
      ImGui.BeginTooltip()
      ImGui.Text(tooltip)
      ImGui.EndTooltip()
    end   
  end
end





function edit_tree (count, var, alias)
  
  if ImGui.TreeNode('list') then
    ImGui.NewLine()
    for i = 1, 50 do 
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
    return ''
  end
  return 100 - mq.TLO.Target.Slowed.Base(2)()..'%% ('..mq.TLO.Target.BuffDuration(mq.TLO.Target.Slowed()).TotalSeconds()..'s)'
end


function isTashed()
  if mq.TLO.Target.Tashed.ID() == nil then
    return ''
  end
  return mq.TLO.Target.Tashed.Base(2)()..'ac ('..mq.TLO.Target.BuffDuration(mq.TLO.Target.Tashed()).TotalSeconds()..'s)'
end


function isMaloed()
  if mq.TLO.Target.Maloed.ID() == nil then
    return ''
  end
  return '+'..mq.TLO.Target.Maloed.Base(1)()..'%% ('..mq.TLO.Target.BuffDuration(mq.TLO.Target.Maloed()).TotalSeconds()..'s)'
end


function isCrippled ()
  if mq.TLO.Target.FindBuff('subcat Disempowering').ID() == nil then
    return ''
  end
  return mq.TLO.Target.FindBuff('subcat Disempowering').Base(4)()..' ('..mq.TLO.Target.BuffDuration(mq.TLO.Target.FindBuff('subcat Disempowering')()).TotalSeconds()..'s)'
end


function isSnared ()
  if mq.TLO.Target.Snared.ID() == nil then
    return ''
  end
  return mq.TLO.Target.Snared.Base(2)()..'%% ('..mq.TLO.Target.BuffDuration(mq.TLO.Target.Snared()).TotalSeconds()..'s)'
end


function hasDS ()
  if mq.TLO.Target.DSed.ID() == nil then
    return ''
  end
  return mq.TLO.Target.DSed.Base(2)()..' ('..mq.TLO.Target.BuffDuration(mq.TLO.Target.DSed()).TotalSeconds()..'s)'
end


function hasLoS ()
  if mq.TLO.Target.LineOfSight() == nil then
    return ''
  end
  return mq.TLO.Target.LineOfSight()
end



function info_about ()
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



-- return color for text
function color (color, thing_to_color)
  
  if color == 'RED' then
    ImGui.TextColored(1, 0.0, 0.0, 1, thing_to_color)
  elseif color == 'YELLOW' then
    ImGui.TextColored(1, 1, 0.0, 1, thing_to_color)
  elseif color == 'WHITE' then
    ImGui.TextColored(1, 1, 1, 1, thing_to_color)
  elseif color == 'BLUE' then
    ImGui.TextColored(0.0, 0.0, 1, 1, thing_to_color)
  elseif color == 'LIGHT BLUE' then
    ImGui.TextColored(0.50, 0.50, 0.80, 1, thing_to_color)
  elseif color == 'GREEN' then
    ImGui.TextColored(0.0, 1, 0.0, 1, thing_to_color)
  elseif color == 'GREY' then
    ImGui.TextColored(0.50, 0.50, 0.50, 1, thing_to_color)
  elseif color == 'ORANGE' then
    ImGui.TextColored(0.99, 0.65, 0, 1, thing_to_color)
  end
  
end
 
 
 
function btnoptn (name)
  
  if mq.TLO.Macro.Variable('maHud').Find('swHUDBTN'..name..'').Value() == 'TRUE' then 
    return true
  else 
    return false 
  end
  
  
end



function control_movement (...)  
  local op = {...}   
   
  if op[1] == 'stop_all' then   
    if mq.TLO.Navigation.Active() == true then
      mq.cmd.nav('stop |log=off')
    end
    if mq.TLO.Stick.Active() == true then
      mq.cmd.squelch('/stick off')
    end
    if mq.TLO.Me.Moving() == true then
      mq.cmd.keypress('forward')
      mq.cmd.keypress('back')
    end
  end

end


function control_casting (...)
  local op = {...}
 
  if op[1] == 'stop_all'then 
    if mq.TLO.Me.Casting.ID() ~= nil then
      mq.cmd.stopcast()
    end
  end
  
end


function control_combat (...)
  local op = {...}

    if op[1] == 'stop_all' then 
      if mq.TLO.Me.AutoFire() == true then 
        mq.cmd.squelch('/autofire off')
      end
    
      if mq.TLO.Me.Combat() == true then
        mq.cmd.squelch('/attack off')
      end
    end

    if op[1] == 'stop_melee' then 
      if mq.TLO.Me.Combat() == true then
        mq.cmd.squelch('/attack off')
      end
    end
    
    if op[1] == 'stop_range' then 
      if mq.TLO.Me.AutoFire() == true then 
        mq.cmd.squelch('/autofire off')
      end
    end

end


function control_target (...)
  local op = {...}

  if op[1] == 'release' then
    mq.cmd.varset('stAssTarID 0')
    mq.cmd.varset('targetData 0')
    mq.cmd.varset('assistData 0')
    mq.cmd.squelch('/target clear')
    mq.cmd.invoke('${maCC.Add[swValidAssTarID,FALSE]}')
    mq.cmd.invoke('${maCC.Add[swSetCombat,FALSE]}')
    mq.cmd.invoke('${maCC.Add[swSetForced,FALSE]}')
    mq.cmd.invoke('${set_data_timer[FALSE, Check_Behind, 0]}')
    mq.cmd.invoke('${maData.Add[stFacingPositionCheck,FALSE]}')
    mq.cmd.invoke('${maData.Add[stPctEngageSet,FALSE]}')
    mq.cmd.invoke('${maData.Add[stPctEngagePetSet,FALSE]}')
    mq.cmd.invoke('${maData.Add[stPctEngageSwarmSet,FALSE]}')
    mq.cmd.invoke('${maData.Add[stPctEngageBurnSet,FALSE]}')
    
  end


end




function out (...)
  local op = {...}

  -- default 2 strings    
  if op[1] == 2 then
    print(string.format('\a-t%s\ax \a-w%s\ax', op[2], op[3]))

  -- default 2 strings with an error
  elseif op[1] == 2.1 then
   print(string.format('\a-t%s\ax \a-w%s\ax '.. outs['dot'] ..' \ar%s\ax', op[2], op[3], op[4])) 

  
  -- help response for no tags for the control
  elseif op[1] == 'notag' then
    print('no tags for this control.')

  -- help response for no map for this control
  elseif op[1] == 'nomap' then
    print('no map for this control.')

  end
end





function settableflags ()
  tableFlags = bit32.bor(ImGuiTableFlags.Resizable, ImGuiTableFlags.RowBg, ImGuiTableFlags.SizingFixedFit, ImGuiTableFlags.Borders, ImGuiTableFlags.Hideable) 
end


