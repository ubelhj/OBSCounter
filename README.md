# DemolitionCounter
Counts demolitions in rocket league and outputs to files
[https://bakkesplugins.com/plugins/view/123](https://bakkesplugins.com/plugins/view/123)

Takes all major stat notifications and creates .txt files to read and use in OBS. Demolitions, Goals, MVPS, even High Fives. Any stat you want all available for streaming or recording. This works only if you're playing, not spectating, and only tracks your individual stats.
[gif demonstration](https://gfycat.com/reasonableunhappygar)

Outputs in files in a folder /OBSCounter/ next to your rocket league.exe. You have to make this folder in advance before the plugin runs or it will be unable to write the files. 

enable the plugin in BakkesMod console by pressing f6 and typing counter_enabled 1

There are 2 types of files, both plain numbers in .txt form, stats and gameStats. Stats use a running total across the current session, and gameStats only count the latest game. 
There are also setters for the stats files, by pressing f6 to enter the BakkesMod console and typing counter_set_statName. At the moment there are no setters for gameStats.

[Here's a guide to OBS setup for the plugin by toothboto#3030](https://pastebin.com/w3kzUAh3)

There may be an issue with games counting, and therefore gameStats files, so if anyone knows a good way to check if an online and/or private game has started let me know.

If you have any permissions issues, try creating the /OBSCounter/ folder first

The .dll file is still named DemolitionCounter but will be updated later to fit the new and more broad scope.
