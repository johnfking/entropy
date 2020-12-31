
























testsub = function (arg)
  for i = 1, #arg
  do
    echo(arg[i])  
  end
end  











--[[
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




