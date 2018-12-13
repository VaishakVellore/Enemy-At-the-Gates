#EnemyAtTheGates
							Team: Vaishak Ramesh Vellore
								Chiranjeevi Ramamurthy
								Ananya Dutta 

Game Inspiration:
The idea behind creating the game came from something which we played when we were kids and it was called Space Impact which is a mobile game series that was developed by Nokia. Space Impact is a shoot 'em up game and the player has the ability to freely move horizontally and vertically. We developed a newer version for the game with improved graphics.

Description:
The game starts with the Start Page where the user has been given the options to choose between the “viewer mode” and “game mode”. There is a menu which displays what keys to press to enter the modes.
1.	In the viewer mode the user can move around the gaming environment using mouse cursor and menu options and can see all that’s happening in the game without playing.
2.	In the game mode the user enters “Level 1” and starts playing and has to shoot down incoming enemy planes and the only way to win is to not allow the enemy planes to go beyond the player.
Since it’s an arcade style game, there is no end goal apart from setting up High Scores. For every incoming plane that gets shot down the score increases. The scores and levels are displayed on the right side of the game and if the player reaches a score equal to 100, he moves onto “Level 2” and can play the game indefinitely.
If the enemy plane happens to go beyond the player, then the game ends and a “Game Over” page is shown with his current score and the highest scores. 
Gaming Environment:
•	2 viewports one for displaying the game and the other for showing the score.
•	Mesh Data Structure for building the skybox & mountain terrain.
•	Text used for displaying the Start Page, Menu Options, Scoreboard and Game over.
•	F-16 plane which can be controlled by the player to move left and right in-order to shoot down enemy planes.
•	Enemy planes that are flying towards the F-16 which are not controlled by the player.
•	Light is present in the game and all the objects have normal’s to deflect the light.
•	Images were used as textures for the mountain terrain, skybox and riverbed.
•	Procedural Texture Generation where the sky texture was used for display in the waterfall, fire texture for laser and lava and marble texture for explosion particles.
•	Materials for island in the river and leaves for the tree.
•	Collision Detection to destroy enemy planes using the laser and with F-16 for game to end.
•	Bounding Boxes for F-16 and enemy planes
•	A skybox with sky image forming the boundaries for the game.
•	River drawn using curves.
•	Mountains using multi-scaled terrain.
•	Fog is present all around and can be turned on & off.
•	Planar Reflections and Shadows of F-16 built using stencil which can be seen on the river.
•	Laser, Waterfall, F-16 after jet, Explosion created using particle system.
•	Curves used for F-16 and Camera movement along left and right direction.
•	Island in the river using knots and control points.
•	Oscillating Flag with image.
•	A Fractal Tree along the river bed.
•	Game Start and End page.


System Requirements: 
•	OpengGL version 4 Libraries 
•	1 GB RAM
•	128 MB Video Card
•	If Linux/Mac have wine installed


