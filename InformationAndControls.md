Hello,

I wanted this seperate from the README.md because this is just going to be an informal list of high concepts in case you wanted to run the game.


### About the Game
JRPGAutoBattler is based on games such as Hearthstone - Battlegrounds, DoTA Underlords, and Teamfight Tactics. It pulls the most inspiration from Hearthstone - Battlegrounds.

The theme is Final Fantasy, and pulled all art (which is all temp art) from various places on the internet for sprites and I edited most sprite sheets to fit my needs.

### Important
The game is very rough around the edges and is a working and living prototype.
Multiplayer is not working at this time, that is being worked on.

### Running the Game
If you want to play the game, you need to first run the .exe found in the Run folder. Then, once you get the main screen, press space so you see the main menu. Then, press tilde (~) key to open a command console. Type 'create_server', one word and don't put the tick marks. This creates the server, which is needed even for playing alone.~

Go back to the folder and run a new .exe.
Press the space to get to the main menu.
Go to "Multiplayer" (even if you want to play solo, Single Player button does not work at this time).
Click the text box and enter a username.
Press "Connect", you should see that the Server lets you know you are connected.
Press "Ready" to start. You should see a countdown.


### Playing the Game
Once the game starts, you are going to be dropped into a game with no instructions.

## Purchase Phase
The first screen you see is called the Purchase Phase. There is a 30 second timer counting down.
The cards in the center of the screen are the "Marketplace Cards" and can be bought for 1 energy each (energy is displayed in bottom left).
Left click to buy a card.
It should go to the bottom of the screen, this is your "Hand".
Left click a card in your hand, this will sell the card back to the Marketplace. You get 1 energy back.
Right click a card in your hand. It will highlight. On the right side, 8 slots will highlight.
Left click a slot. The card in hand will be turned into a unit.
Units are used in the next phase.
Once you have spent all your energy, wait for the timer to get to 0.

## Battle Phase
You will be auto matched with 1 of 7 other AI players who will have units they purchased from their own Marketplace.
The battle will happen on its own, there is nothing for you to do here except watch the programmer art of units attack, things getting status effects and buffs.
I made all the art, well, stole some from sprite sheets to make my own sheets. I made some of the art.
Luckily I am not applying for an art position.

## Winning/Losing
Whoever is the last player with units alive during the battle phase will win that round. They will deal 1 dmg for each unit still alive to the enemy player.
Then you go back to the Purchase Phase, all units get full health, and you get one more energy than you had previously.

This repeats until the Human Player kills all AI players, or the Human Player has their health get to 0.


### What are the Cards
There are 7 different types of cards/units

PAL -> Paladin - In Battle Phase, will cast a Bubble Shield on friendly unit. This shield will last until THEIR turn starts, where it will remove STATUS EFFECTS and then remove itself.

WAR -> Warrior - In Battle Phase, will attack and may afflict the enemy unit with Bleed, a STATUS EFFECT. This will cause damage on THEIR turn. This damage grows the longer combat goes on.

WHT -> White Mage - Heals a friendly unit, prioritizes the most damaged friendly unit. Has a chance to cast Shimmer on the friendly unit. This will heal some additional amount on the friendly unit's turn.

BLK -> Black Mage - Deals damage, has a small chance to afflict the enemy unit with Burn, a STATUS EFFECT. This will cause damage on THEIR turn.

MOG -> Mog Knight - Attacks and has a chance to BUFF itself with Enrage. Each subsequent turn, the attack damage of this unit grows.

DRG -> Dragoon - Does damage when it attacks. Nothing special.

ARC -> Archer - Attacks and has a chance to BUFF itself with Enrage. Each subsequent turn, the attack damage of this unit grows. (Note: this unit never got reworked, it was my first unit type where I tested things. I left him, he is OP)


