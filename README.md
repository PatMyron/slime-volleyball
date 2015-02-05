<h1>slime-volleyball</h1>
================

2 player volleyball game written in C using x11 graphics  made with <a href="https://github.com/amcmaho4">Anna McMahon</a>


<img src="https://raw.githubusercontent.com/PatMyron/slime-volleyball/master/photos/3.JPG" alt="Slime Volleyball" width="100%" <style="height:auto">

<h1>How this program works for the user:<h1>

<p>The game starts with a menu where the two players are asked to input
their names. The keys for each player are beautifully drawn on the
screen as well to instruct the players how to play. Then, the game
starts with the ball bouncing on one player. They move left, right,
and jump to hit the ball and play volleyball. They play until one
player scores 3 points. It displays the winner and asks you to press
‘c’ to get to the menu. In the menu, they can choose to play again
(p), see stats (s), change names (n), or quit (q). They can go through
this menu after every game played. The program runs until they quit
through the menu or press escape during a game being played.</p>

<h1>How this program works internally:</h1>

<p>To simulate the game “slime volleyball” we used the gfx_library
(slightly modified). We used the gfx_library to draw animations
according to key presses. We wanted our program to be able to read
more than one key being pressed at a time, so we made a new function
gfx_keyPress so that it would, using pointers, modify a bool array,
which corresponded to whether or not any of six keys were pressed. We
also slightly modified a few other functions in the gfx_library to
allow for this multiple- key input, including gfx_open() and
gfx_event_waiting() but renamed gfx_event_waiting2(). The graphic
continues to display update according to user input until the escape
key is pressed. The infinite loop is terminated with a return 0
command after the escape key is pressed.</p>

<p>In terms or variables, are most important ones are the ball and the
two player structs. We chose to use structs because there was a
variety of data that pertained to both the ball and the player. We
often pass these as parameters to functions. Whether we passed the
struct by reference or by value depended on whether or not the
function was intended to change the data in the stuct or not.
</p>


<p>The graphics are displayed by many different functions which can
essentially be broken down into three groups:
</p>


<h3>1. Functions that control the user interface between plays</h3>

<p>           Among these functions are getNames2, displayData,
actOnOptions, and optionsMenu, and drawPlayerKeysAtStartScreen.  First
getNames2 is called outside of the while(1) loop to get the player
names of the first two players. The loop reads in user key presses and
saves the two player names into the player1 & player 2 structs. Within
the while loop,  the first time ‘enter’ is pressed the key presses
start being read into the second username, the second time it is
pressed the game begins. It also checks if delete is pressed. If
delete is pressed the previous location in the name array is set to
blank and the place holder for the letter of the word is decremented.
The user input is then displayed the user can see what they are
typing, this all makes the user input more intuitive.
drawPlayerKeysAtStartScreen. Is called in this command, this displays
a graphic of the key commands that each player will use in the game.
</p>
<p>           Then there are two functions the control the menu that the
player sees between games/ player changes/ etc. and these are
actonOptions and optionsMenu. OptionsMenu is called from with
actOnOptions. OptionsMenu dispays the options to the user and then
waits for keyboard input with gfx_event_waiting() and gfx_wait().
Depending on the user input, a switch statement controls the int
returned by the function. The int returned by options menu is then
used in actOnOptions accordingly. In this function resetGame,
newPlayers, or diplayData is called. Displaydata displays the player
name and number of wins of every player since the window was opened.
It does this by traversing a linked list and displaying with gfx_text.
It then waits for player input to return to the main menu.
</p>






<h3>2. Functions that make and move that ball</h3>

<p>               These functions include moveBall, drawBall ,
calculateAngleBetweenCenters, and checkForCollisions,
checkForCollisions is called with in moveBall. And it compares the
ball location to the player, wall and net locations. Only the player
on the same side of the screen as the ball is passed into this
function for simplicity. The ball velocity is changed according to any
collisions that occur.  This function also decrements a players life
if the ball touches the ground on their side.  If the ball hits a
player, calculateBallsDirection is called to determine the new
velocity of the ball (x and y velocity) using the angle of deflection
with which the ball will bounce off the player. After collisions are
checked the vy is updated to give the ball a falling motion, like
gravity. Finally, adding vx/vy changes the ball’s x and y locations.
drawBall is called after makeBall in main to reflect all of these
changes
</p>


<h3>3. Functions that draw the player</h3>

<p>               The functions that control the player movement are
drawPlayer, movePlayer, bringToGround, and validatePlayerMove. The
player locations are updated upon keypress. A key press sends results
in a call to the function movePlayer which updates the player location
based on the keypress, by user. Before the player is actually drawn
though, the boundary locations are checked in the function
validatePlayerMove. This function re-updates the player location if
the key press would send the player off the board or into the net.
There is one player movement that is not triggered by a key press and
that is the motion of jumping. This motion is continual and takes
place out of the context of a key press, so a function
bringPlayerToGround controls this. The fuction updates the vy if the
player is off the ground, iterating through the jumping motion while
the player is not on the ground.  Finally, the player, a green/ blue
half circle is drawn using drawPlayer.
</p>


<h3>4. Functions that control data saving</h3>

<p>               Some functions that save and update the player
information are makeNode and playerExists. Both functions are called
in main when gameOver returns true. playerExists traverses the linked
list of players to see if the current player already has a node. It
does this by comparing the name string of the structs. If a node does
already exist, a pointer to that struct is returned. If a player does
not already exist, the function returns Null. If Null is returned,
then makeNode is called and the player information is saved into a new
node. If a pointer to a person struct is returned, the wins of the
player at that node are updated. In this way, player information is
saved between each game, and when the users are switched. This does
not save the data after the game quits. Had time permitted, we would
have read the data into a text file and reloaded it every time the
game was played.</p>

<h1>How we checked for errors:</h1>

<p>We made sure it could handle people trying to enter really long names
properly. Unfortunately, we did not have time to get uppercase letters
in names because this requires multiple keys and using something
similar to our new gfx function. We also made sure that every menu
option worked in every possible order. (Stats could be checked and
then another option could be selected). We also made sure every
possible movement direction worked for both characters even when other
keys were pressed down beforehand. We made sure the ball bounced off
the character correctly at various different angles. We made sure the
game registered the ball hitting the ground at various different
points and started the play up correctly with the ball dropping
straight down on the losing player regardless of what direction the
ball was headed when it hit the ground. We made sure holding down left
and right keys made them move continuously. We made sure the stats
worked correctly and only listed each name once with the correct
number of wins. We also made sure the escape key quit the game during
the game. If we had more time, we would have fully correctly
implemented the ‘p’ key pausing the game.</p>
