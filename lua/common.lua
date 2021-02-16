--[[
- entropoy.mac
- common.lua
- 
- lua shared _stuffs_
-
--]]




mq = require('mq')
echo = mq.cmd.echo
DisplayName = mq.TLO.Me.DisplayName()
ShortName = mq.TLO.Me.Class.ShortName()
discord = '${If[${Bool[${Plugin[MQ2Discord]}]},\at-\ax,]}'


