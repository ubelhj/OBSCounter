# OBSCounter
Counts demolitions in rocket league and outputs to files
[https://bakkesplugins.com/plugins/view/123](https://bakkesplugins.com/plugins/view/126)

Takes all major stat notifications and creates .txt files to read and use in OBS. Demolitions, Goals, MVPS, even High Fives. Any stat you want all available for streaming or recording. This works only if you're playing, not spectating, and only tracks your individual stats.
[gif demonstration](https://gfycat.com/reasonableunhappygar)

Outputs in files in a folder /OBSCounter/ next to your rocket league.exe. You have to make this folder in advance before the plugin runs or it will be unable to write the files. 

enable the plugin in BakkesMod console by pressing f6 and typing counter_enabled 1

There are 3 types of files, all plain numbers in .txt form, stats, gameStats, and averageStats. Stats use a running total across the current session, gameStats only count the latest game, and averageStats divide the running total by number of games played. 
There are also setters for the stats files, by pressing f6 to enter the BakkesMod console and typing counter_set_statName. At the moment there are no setters for gameStats or averageStats.

[Here's a guide to OBS setup for the plugin by toothboto#3030](https://pastebin.com/w3kzUAh3)

If you have any permissions issues, try creating the /OBSCounter/ folder first

**CURRENTLY KNOWN ISSUE **
First Touches, Centers, and Clears fail to count. Only stats that can appear on the right side ticker are counted properly. 

The .dll and .set files have been renamed from DemolitionCounter to OBSCounter. Please make sure that all old copies are deleted to prevent the code from running twice.
