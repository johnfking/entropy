





mq = require('mq')
out = mq.cmd.echo
DisplayName = mq.TLO.Me.DisplayName()
ShortName = mq.TLO.Me.Class.ShortName()









return


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







--[[
testsub = function (arg)
  for i = 1, #arg
  do
    echo(arg[i])  
  end
end  






testtable = {}
testtable[1] = "test1"  
testtable[2] = "i have no idea what i'm doing"  
 
for i = 1, 2
do
  mq.cmd.echo(testtable[i])  
end
--]]
  
  
--[[
array = {"Lua", "Tutorial"}
for i = 0, 2
do
   echo(array[i])
end
--]]


--[[
a = 1
while (a < 11)
do
 echo('a = '..a)
 a = a + 1
end
--]]



--[[
local testsub = function ()
    echo('1')
end
--]]










