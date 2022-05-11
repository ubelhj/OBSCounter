# OBSCounter

Counts all major stat notifications for an in-game overlay or OBS source. Demolitions, Goals, MVPS, even High Fives. Any stat you want all available for streaming or recording. This works only if you're playing, not spectating, and only tracks your individual stats.  
[gif demonstration](https://gfycat.com/reasonableunhappygar)

Outputs *575!* files in your Bakkesmod data folder. It's found at "%appdata%\bakkesmod\bakkesmod\data\OBSCounter". There are 11 types of files, all plain numbers in .txt form. 
1. Current session  
2. Current game  
3. Average this session  
4. Career total    
5. Career average  
6. Career total in casual  
7. Career total private  
8. Career total ranked  
9. Other miscellaneous compound stats like session shooting percentage and K/D ratio
10. Team stats this session
11. Opponent team stats this session


[Here's a guide to OBS setup for the plugin by toothboto#3030](https://pastebin.com/w3kzUAh3)

## Fully customizable in game overlay    
![Example of the overlay](https://cdn.discordapp.com/attachments/777175369782263828/973741777813143602/unknown.png)  
All stats are accessible from f2 -> plugins -> Stat Counter     
See from 1-10 stats at once, with any color text and background. You can also rename the stats, for example it can display "Game Demolitions" as "Demos". 

### Custom fonts
The plugin now supports custom fonts. By default it uses the Ubuntu Regular font.  
If you want a custom font for the overlay, first close Rocket League.  
Open the fonts folder at `%appdata%\bakkesmod\bakkesmod\data\fonts\`  
Replace `OBSCounter.ttf` with any .ttf font of your choice, renamed to `OBSCounter.ttf`

**Known Issue**  
Ragequitting casual games messes up the games tracking. You must wait until the end of games for the plugin to count them. It was really hard already to get the plugin to support casual games and joining matches in progress, so this issue is unlikely to be fixable. If you want to reset your most recent game stats, just press the add a game button in the f2 -> plugins -> OBSCounter menu.

If you have a great idea and need an implementation DM or add me as friend on discord JerryTheBee#1117! I do plugin commissions!