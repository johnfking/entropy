--[[
entropoy.mac
common.lua

lua shared stuffs
--]]

mq = require('mq')
echo = mq.cmd.echo
DisplayName = mq.TLO.Me.DisplayName()
ShortName = mq.TLO.Me.Class.ShortName()
discord = '${If[${Bool[${Plugin[MQ2Discord]}]},\at-\ax,]}'





out = function (option, verbage)

  --[ help response for no tags for the control --]
  if option == 'notag' then
    echo('${lsep}no tags for this control.')

  --[ help response for no map for this control --]
  elseif option == 'nomap' then
    echo('${lsep}no map for this control.')


    
  end
end


