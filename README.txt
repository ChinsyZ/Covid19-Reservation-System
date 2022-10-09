1.My Full name:				Chenxin Zhao
2.My School: USC
===================================================================================================
3.What I have done in the assignment:

In this project, I implemented a computing system with five ending systems to realize a reservation system based on their locations and the availability of hospitals. To complete the project, I learned socket programming and Dijkstra along with C++ programming. UDP and TCP are both used for communications between them.

Besides, I tested all my codes both on my local macOS and ubuntu provided on the VM.

====================================================================================================
4.What my code files are and what each one of them does:

I have five code files in total:

a) scheduler.cpp:
First of all, scheduler.cpp is responsible for receiving total capacity and occupation from hospitals using UDP and judging whether hospitals have availability for a new appointment. After that, scheduler will receive location of client using TCP and send it to the hospitals which have availabilities. Then, scheduler will receive and compare scores from hospitals to take the maximum value. Lastly, the scheduler will send the result to the client and the assigned hospital for updating his availability. The result of assignment may be None for the following situations: 1) the location of client is the same as any one of the hospital; 2) the location of client is not found; 3) all hospitals have full occupations.
	
b) hospital<A/B/C>.cpp:
Hospitals will read map.txt first, build their adjacent maps and then send their own total capacity and occupation to the scheduler using UDP. After that，they will receive location of the client from scheduler and find it in the map. If found, they will calculate ultimate score using availability and the shortest distance using Dijkstra. Lastly, they will keep waiting for receiving the assignment from scheduler to update their availability.

c) client.cpp:
Client will send his location to scheduler using TCP, and wait for the assignment. If his location is not found in the map. He will show "Location not found" on screen.

Note: I corrected the score to four decimals and distance to two decimals, which means the fifth and third decimals may affect the results separately. For example, the distance between node 1 and node 2 in sample map_simple.txt is 74.9992 and 75.00 is displayed on screen in my project. Also, the scores may seem to be the same but actually they are not.
 
====================================================================================================
5.The format of all the messages exchanged:

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~client -- scheduler~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
a) client --> scheduler:
<location>: client sends his location to scheduler.

b) scheduler --> client:
<"Location not found">: If location of the client is not found in the map, scheduler will send string "Location not found" to the client.
<Hospital(A/B/C/None)>:If location of the client is found, scheduler will send assignment to the client. If location of the client is the same as any one of the hospital, the result will be None.

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~scheduler -- hospital~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
a) scheduler --> hospital:
<location of the client>: After receiving location from client, scheduler will send his location to those who have availability.
<"we pick you">: After comparing scores and making a decision, scheduler will send this message to the hospital which client is assigned to.

b) hospital --> scheduler:
<total capacity " " occupation>: Hospitals send their own total capacity and occupation to scheduler with a blank(" ") to separate them.
<score " " distance>: Hospitals send their own scores and shortest distance to scheduler with a blank (" ") between them.
<"Location not found">:If location of the client is not found in the map, send this string to scheduler.

====================================================================================================
6.Any idiosyncrasy of my project:

I did not consider the situation that location of the client is the same as one of the hospitals while there is a hospital have none availability.

====================================================================================================
7.Reused Code:

I use Beej's example codes to realize my socket programming.