--[[
entropoy.mac
get_help.lua

help output
--]]

mq = require('mq')

local arg = {...}

local lu = {}
lu['subject'] = arg[1]
lu['help'] = arg[2]



local sep = "\a#AA00AA::\ax"


local edebug = {}
edebug['u'] = {}

edebug['u'][1] = "usage"..sep
edebug['u'][2] = "\a-w/debug [subject] [detail]\ax"



mq.cmd.echo(lu['subject'][lu['help']][1])
-- mq.cmd.echo(lu['subject'][lu['help']][2])




