package.cpath = "/home/microwish/lua-datetime/lib/?.so;" .. package.cpath

local datetime = require("datetime");

local a = datetime.gettimeofday()

for k, v in pairs(a) do print(k, v) end

--datetime.gettimeofday = 1
