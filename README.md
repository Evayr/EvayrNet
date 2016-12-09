### EvayrNet ###
Online multiplayer networking library for games made by Ivar "Evayr" Slotboom

This project is still a work in progress!

### GOAL ###
The goal of this networking library is to provide an easy to use and fast-paced networking utility for games that need it.
The aim is to make this networking library cross-platform over at least Windows and Linux.

Examples of game genres that could benefit from this library:
- First Person Shooters (FPS)
- Racing games
- Role Playing Games (RPGs)

This library won't be as useful for:
- Chess games
- Poker games

### FEATURES ###
The library does everything in UDP for transmission speed and has a reliability layer written on top of it in case you need it.
It also supports sequencing of packets, which allows you to process received messages in the same order as you sent it.

Gameplay hooks are built in for the player to receive data like the connection result, new incoming connection and more.

An example server and client project has also been set up so you can check out the implementation of it.

As a summary, this library has the following features:
- Full UDP connectivity for fast transmission speed
- Reliability for whenever you need it
- Sequencing for received messages so that it's processed in the same order as you received it
- Gameplay hooks for gameplay programmers (for data like ping, OnConnection, etc)
- Theoretically up to 65533 players at the same time
- Automated network message generation using Python

### GAMES ###
Games that have used this library:
< To be added, will be making a test game testing the library >

### CONTACT ###
Either PM me on GitHub or mail me at mail{at}evayr[dot]com

### LICENSE ###
See LICENSE.txt
