# Village Tower Defense

Welcome to **Village Tower Defense**, a strategic tower defense game where players must defend their village from waves of enemies using various towers and strategic upgrades, and with the help of the dragon.

## Introduction

**Village Tower Defense** is an immersive tower defense game that challenges players to protect their village from different enemy types. The game is built using C++ and SDL, and features dynamic gameplay, strategic tower placement, and real-time resource management.
<p align="center">
  <img src="https://github.com/user-attachments/assets/82a3002b-77fe-44c0-8022-dfaeb7c2032d" alt="Game screenshot" width="720" />
</p>

## Features

- **Diverse Enemies:** Face a variety of enemies, including slimes, skeletons, goblins, and more, each with unique abilities and challenges.
- **Tower Defense Mechanics:** Strategically place towers to defend your village. Upgrade and manage resources effectively to improve your defenses.
- **RPG Component:** Control the dragon to assit your tower defense. Use normal attack and special skill to defeat the monsters.
- **Resource Management:** Collect coins from defeated enemies to upgrade towers and purchase new defenses.
- **Dynamic Waves:** Experience progressively challenging enemy waves, requiring adaptable strategies and quick thinking.
- **Game Pausing:** Pause and resume gameplay at any time with a simple key press, allowing for thoughtful planning and breaks.
- **Win and Lose Conditions:** Engage in intense battles and experience victory or defeat, with appropriate in-game feedback and progression.

## Gameplay

1. **Objective:** Defend your village from incoming enemy waves by strategically placing and upgrading towers.
2. **Enemy Waves:** Each wave introduces a new set of enemies. Manage resources and adjust your strategy accordingly.
3. **Towers:** Build and upgrade towers to increase their effectiveness against different enemy types.
4. **Resource Management:** Build and upgrade tower consumes coin. Make sure to manage it well.
5. **Pause Feature:** Press `P` to pause and resume the game at any point.

## Installation

Follow these steps to set up the game on your local machine:

1. **Clone the Repository:**

   git clone https://github.com/yourusername/village-tower-defense.git
   cd village-tower-defense

2. **Denpendencies**

   Third party libraries used (SDL, TTF, and others) are included in the folder "thirdParty"

3. **Build the Game:**

  Use a C++ compiler to build the game. An example command using g++:
  g++ -o VillageTD *.cpp -lSDL2 -lSDL2_image -lSDL2_mixer -lSDL2_ttf

4. **Run the Game**

   ./VillageTD


## Controls

- **Mouse Click:** Place and upgrade towers on the map.
- **WASD** Control the location of the dragon.
- **J** Normal Attack of Dragon.
- **K** Special Attack of Dragon.
- **P:** Pause and resume the game at any point.
- **Y:** Restart the game after losing.
- **N:** Exit the game after losing.

## Dependencies

The game relies on the following libraries:

- **SDL2:** For handling window management, rendering, and events.
- **SDL2_image:** For loading images in various formats.
- **SDL2_mixer:** For playing sound effects and music.
- **SDL2_ttf:** For rendering TrueType fonts.

## Credit

Part of Game resource and tutorial from @VoidMatrix 
https://space.bilibili.com/25864506


