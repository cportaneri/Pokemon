/***** Pokemon in C *****/ 

This is a C school project made by Cedric Portaneri, Daro Heng and Yannick Havil from December 2013 to January 2014. 
At this time we were 3rd year computer science students from the engineering school Polytech Paris Sud.

The pdf file is a report of our work. You can check there the different features of our code. 
Warning: The method of how to launch the game in the pdf is out of date. In this file I will explain how to do it.

All sprites and sounds belong to Nintendo.

--- OS Compatibility ---
Windows only

--- LIBRARY USED ---
SDL 1.2.15 for the 2D graphics
FMOD for Sound

--- HOW TO LAUNCH THE GAME ---
We recommand you tu use CodeBlocks on Windows 

1) Install SDL library with these tutorials : http://wiki.codeblocks.org/index.php/Using_SDL_with_Code::Blocks (EN)
					       http://loka.developpez.com/tutoriel/sdl/installation/codeblocks/ (FR)

2) Same for FMOD : https://openclassrooms.com/courses/apprenez-a-programmer-en-c/jouer-du-son-avec-fmod (FR)

3) Lauch the game with CodeBlocks

--- IMPROVEMENT ---
Back in 2013, it was our very first big coding project and we just
start learning C. With only a month to do it, the quality of the code is quite bad.
Now that I am a 5th year student I see many things wrong with this code.
  - A lot of repetition
  - Use of direct integer values --> static code
  - More comments
  - Which leads to a really bad scalability, hard to extend some map or image

Thus I will improve the quality of this code in the futur.
