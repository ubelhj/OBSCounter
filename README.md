# OBSCounter

Takes all major stat notifications and creates .txt files to read and use in OBS and an in game overlay. Demolitions, Goals, MVPS, even High Fives. Any stat you want all available for streaming or recording. This works only if you're playing, not spectating, and only tracks your individual stats.
[gif demonstration](https://gfycat.com/reasonableunhappygar)

Outputs *98!* files in a folder /OBSCounter/ next to your RocketLeague.exe. Usually "C:\Program Files (x86)\Steam\steamapps\common\rocketleague\Binaries\Win64\OBSCounter". There are 3 types of files, all plain numbers in .txt form, stats, gameStats, and averageStats. Stats use a running total across the current session, gameStats only count the latest game, and averageStats divide the running total by number of games played.  
There are also setters for the stats files, by pressing f6 to enter the BakkesMod console and typing counter_set_statName. At the moment there are no setters for gameStats or averageStats.

Download for yourself at [https://bakkesplugins.com/plugins/view/126](https://bakkesplugins.com/plugins/view/126)

[Here's a guide to OBS setup for the plugin by toothboto#3030](https://pastebin.com/w3kzUAh3)

# Updates
**Update 1** In game stat overlay! Go to the settings page for the plugin (f2 -> Plugins -> OBS Counter Plugin) to choose what stats you want to see, what color they should be, and where you want to see them on your screen. Supports total session stats, most recent game stats, and average per game stats. No software required!

**Update 2** Compound stats! I've added K/D ratio, shooting %, and missed extermination % (exterminations / (demolitions / 7)). These stats are only output to .txt files, and are not in the in-game overlay. If you want me to add another compound stat for a stream overlay, @ me on discord and I might be able to get it working. 

**Update 3** Complete stat tracking! I've successfully added clears, centers, bicycle hits, first touches, and points to the plugin. Points are still missing the 2 point ball hits, so they're not 100% accurate but I will look into that. As a bonus I've also added losses. They're all available both in game and in OBS. Win % is also newly available for OBS only. I don't plan on adding any mmr or win streak stuff, as there's a couple plugins that support that already for OBS.

**Update 4** Automatically creates folder for text files. I know this isn't much, but the plugin no longer needs any pre-install effort. 

**Known Issue**  
Ragequitting casual games messes up the games tracking. You must wait until the end of games for the plugin to count them. It was really hard already to get the plugin to support casual games and joining matches in progress, so this issue is unlikely to be fixable. If you want to reset your most recent game stats, just press the add a game button in the f2 -> plugins -> OBSCounter menu.
