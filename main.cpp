#include <cmath>
#include <cstdlib>
#include <iostream>
using namespace std;

void roomInfo(int roomX, int roomY);

void initializeGame();

void showMap();

void defineRoom(int roomX, int roomY, char roomType);

void generateFloor();

void makeCliff(int roomX, int roomY);

void move(char direction);

void getInput();

void clearPlayerMap();

void placeHouse();

void houseGoDirection(int x, int y, char direction);

bool checkTile(int x, int y);

void dispHouseDir();

void test();

void talkToGranny();

void print(string msg);

void setRoomDescription(string start, string verbs, string descriptor,
                        int type);

const int worldSize = 14;
char worldMap[worldSize][worldSize];
string description[worldSize][worldSize];
string simpleDesc[worldSize][worldSize];
char playerMap[worldSize][worldSize];
int playerY = worldSize / 2;
int playerX = worldSize / 2;
int houseCoords[2];
bool gameOver = false;
int currentFloor = 1;

// int playerY = 0;
// int playerX = 0;

// perlin noise code (I didn't write any of this)

typedef struct {
  float x, y;
} vector2;

vector2 randomGradient(int ix, int iy) {
  // No precomputed gradients mean this works for any number of grid coordinates
  const unsigned w = 8 * sizeof(unsigned);
  const unsigned s = w / 2;
  unsigned a = ix, b = iy;
  a *= 3284157443;

  b ^= a << s | a >> w - s;
  b *= 1911520717;

  a ^= b << s | b >> w - s;
  a *= 2048419325;
  float random = a * (3.14159265 / ~(~0u >> 1)); // in [0, 2*Pi]

  // Create the vector from the angle
  vector2 v;
  v.x = sin(random);
  v.y = cos(random);

  return v;
}

// Computes the dot product of the distance and gradient vectors.
float dotGridGradient(int ix, int iy, float x, float y) {
  // Get gradient from integer coordinates
  vector2 gradient = randomGradient(ix, iy);

  // Compute the distance vector
  float dx = x - (float)ix;
  float dy = y - (float)iy;

  // Compute the dot-product
  return (dx * gradient.x + dy * gradient.y);
}

float interpolate(float a0, float a1, float w) {
  return (a1 - a0) * (3.0 - w * 2.0) * w * w + a0;
}

// Sample Perlin noise at coordinates x, y
float perlin(float x, float y) {

  // Determine grid cell corner coordinates
  int x0 = (int)x;
  int y0 = (int)y;
  int x1 = x0 + 1;
  int y1 = y0 + 1;

  // Compute Interpolation weights
  float sx = x - (float)x0;
  float sy = y - (float)y0;

  // Compute and interpolate top two corners
  float n0 = dotGridGradient(x0, y0, x, y);
  float n1 = dotGridGradient(x1, y0, x, y);
  float ix0 = interpolate(n0, n1, sx);

  // Compute and interpolate bottom two corners
  n0 = dotGridGradient(x0, y1, x, y);
  n1 = dotGridGradient(x1, y1, x, y);
  float ix1 = interpolate(n0, n1, sx);

  // Final step: interpolate between the two previously interpolated values, now
  // in y
  float value = interpolate(ix0, ix1, sy);

  return value;
}

void generateNoiseWorld(int resolution) {
  for (int x = 0; x < worldSize; x++) {
    for (int y = 0; y < worldSize; y++) {
      int index = (y * worldSize + x) * 4;

      float val = 0;

      float freq = 8;
      float amp = 8;

      for (int i = 0; i < 15; i++) {
        srand(time(NULL)); // 1696432080);
        val += perlin((x + (rand() % 5000)) * freq / 50,
                      (y + (rand() % 5000)) * freq / 50) *
               amp;

        freq *= 2;
        amp /= 2;
      }

      // Contrast
      val *= 1.2;

      // Clipping
      if (val > 1.0f)
        val = 1.0f;
      else if (val < -1.0f)
        val = -1.0f;

      // Convert 1 to -1 into 255 to 0
      int color = (int)(((val + 1.0f) * 0.5f) * resolution);

      worldMap[y][x] = color;
    }
  }
}

// ok everything from here on, I've written.
int main() {
  char direction;

  cout << "-Welcome, please enjoy your stay.-" << endl;
  cout << "--                              --" << endl;
  cout << "             by Tess" << endl << endl;
  print("*This game has not been play tested a huge amount.");
  print(
      "*Please make sure you input the correct command when sending commands");
  print("as to avoid a potential softlock, or crash.");
  print("\nEnjoy!\n");
  print("\n\n\nYou spawn into a world that seems just as full of nature as "
        "your life was before you moved to the city. You can't seem to "
        "remember how you got here, but You can see a house in the distance "
        "and feel like it's the only interesting thing for miles.");
  print("\n remember that you can always type \"Help\" if you feel like you're "
        "confused with the game input.");
  initializeGame();
  while (!gameOver) {
    roomInfo(playerX, playerY);
    getInput();
  }
}

void initializeGame() {
  generateFloor();
  roomInfo(playerX, playerY);
  getInput();
}

void roomInfo(int roomX, int roomY) {

  bool N = false;
  bool E = false;
  bool S = false;
  bool W = false;

  if (worldMap[roomY][roomX - 1] != '#') {
    W = true;
  }
  if (worldMap[roomY + 1][roomX] != '#') {
    S = true;
  }
  if (worldMap[roomY][roomX + 1] != '#') {
    E = true;
  }
  if (worldMap[roomY - 1][roomX] != '#') {
    N = true;
  }

  int type = rand() % 2;

  if (type == 0) {
    cout << endl << description[roomY][roomX];
    if (N == true) {
      cout << " North of you, lies " + simpleDesc[roomY - 1][roomX];
    }
    if (E == true) {
      cout << " East is " + simpleDesc[roomY][roomX + 1];
    }
    if (S == true) {
      cout << " To the south, there is " + simpleDesc[roomY + 1][roomX];
    }
    if (N == true || E == true || S == true) {
      cout << " And west of you, there is " + simpleDesc[roomY][roomX - 1];
    } else if (W == true) {
      cout << " West of you, there is " + simpleDesc[roomY][roomX - 1];
    }
    cout << " ";
    dispHouseDir();
  } else if (type == 1) {
    cout << endl << description[roomY][roomX];
    dispHouseDir();
    if (N == true) {
      cout << " North of you, lies " + simpleDesc[roomY - 1][roomX];
    }
    if (E == true) {
      cout << " East is " + simpleDesc[roomY][roomX + 1];
    }
    if (S == true) {
      cout << " To the south, there is " + simpleDesc[roomY + 1][roomX];
    }
    if (N == true || E == true || S == true) {
      cout << " And west of you, there is " + simpleDesc[roomY][roomX - 1];
    } else if (W == true) {
      cout << " West of you, there is " + simpleDesc[roomY][roomX - 1];
    }
  }
}

void dispHouseDir() {
  string houseDirection1 = "";
  string houseDirection2 = "";
  string houseDistance = "";

  if (abs(playerY - houseCoords[0]) == 5 ||
      abs(playerX - houseCoords[1]) == 5) {
    houseDistance = "is in the distant ";
  } else if (abs(playerY - houseCoords[0]) >= 3 ||
             abs(playerX - houseCoords[1]) >= 3) {
    houseDistance = "is a bit ";
  } else {
    houseDistance = "is ";
  }

  if (houseCoords[0] - playerX < 0) {
    houseDirection2 = "west.";
  } else if (houseCoords[0] - playerX > 0) {
    houseDirection2 = "east.";
  }
  if (houseCoords[1] - playerY < 0) {
    houseDirection1 = "north";
  } else if (houseCoords[1] - playerY > 0) {
    houseDirection1 = "south";
  }

  cout << "The house " << houseDistance << houseDirection1 << houseDirection2;
}

void getInput() {
  print("\n\nInput: ");
  bool setCommand = true;
  while (setCommand) {
    string command;
    cin >> command;

    if ((command == "n" || command == "north" || command == "N" ||
         command == "North") &&
        worldMap[playerY - 1][playerX] != '#') {
      move('n');
      setCommand = false;
    } else if ((command == "e" || command == "east" || command == "E" ||
                command == "East") &&
               worldMap[playerY][playerX + 1] != '#') {
      move('e');
      setCommand = false;
    } else if ((command == "s" || command == "south" || command == "S" ||
                command == "South") &&
               worldMap[playerY + 1][playerX] != '#') {
      move('s');
      setCommand = false;
    } else if ((command == "w" || command == "west" || command == "W" ||
                command == "West") &&
               worldMap[playerY][playerX - 1] != '#') {
      move('w');
      setCommand = false;
    } else if (command == "n" || command == "north" || command == "N" ||
               command == "North" || command == "e" || command == "east" ||
               command == "E" || command == "East" || command == "s" ||
               command == "south" || command == "S" || command == "South" ||
               command == "w" || command == "west" || command == "W" ||
               command == "West") {
      cout << "You can't go there." << endl;
      // } else if (command == "map") {
      //   // showMap();
      //   print("I'm sorry, there isn't a map available.");
    } else if (command == "help" || command == "Help") {
      print("Possible directions: North, East, South, and West.");
      print("Inputs such as \"N\", \"North\", \"n\", or \"north\" should work "
            "fine.");
      print("To progress the story, you may find looking for the house to be "
            "the best course of action.");
      print("And although sometimes, it may seem like the house is "
            "unreachable, it is programmed in such a way that it is impossible "
            "for the house to be out of your reach.");
      print("\n\nAlso, throughout dialogue be careful with input because those "
            "sections are the most jank.");
    } else {
      cout << "I don't understand \"" << command << "\"" << endl;
    }
  }
}

void makeCliff(int roomX, int roomY) {
  worldMap[roomY][roomX] = '#';
  description[roomY][roomX] = "This is a cliff, idk how you got here...";
}

void generateFloor() {
  int usedRooms[worldSize][worldSize];

  for (int y; y < worldSize; y++) {
    int x = rand() % worldSize;
    defineRoom(x, y, 4);
    usedRooms[y][x] = 1;
  }

  generateNoiseWorld(100);
  // sets up used rooms array so that the generator knows what tiles are taken
  // up already
  for (int x = 0; x < worldSize; x++) {
    for (int y = 0; y < worldSize; y++) {
      if (worldMap[y][x] == '#') {
        usedRooms[y][x] = 1;
      } else {
        usedRooms[y][x] = 0;
      };
    }
  }

  // Generates rooms based on numbers placed by noise generation
  for (int x = 0; x < worldSize; x++) {
    for (int y = 0; y < worldSize; y++) {
      if (worldMap[y][x] <= 5) {
        defineRoom(x, y, '_');
      } else if (worldMap[y][x] <= 50) {
        defineRoom(x, y, 'w');
      } else if (worldMap[y][x] <= 75) {
        defineRoom(x, y, '*');
      } else if (worldMap[y][x] <= 99) {
        defineRoom(x, y, 'T');
      } else if (worldMap[y][x] <= 100) {
        defineRoom(x, y, '#');
      }
    }
  }

  // spawns the player in a random spot if all the tiles around the player are
  // also free.
  bool placedPlayer = true;
  while (placedPlayer) {
    int x = rand() % worldSize;
    int y = rand() % worldSize;
    if (worldMap[y][x] == '_' && worldMap[y - 1][x] == '_' &&
        worldMap[y + 1][x] == '_' && worldMap[y][x - 1] == '_' &&
        worldMap[y][x + 1] == '_') {
      defineRoom(x, y, 'O');
      placedPlayer = false;
    }
  }

  // generates tall grass randomly over the short grass
  for (int i = 0; i < 100; i++) {
    int x = rand() % worldSize;
    int y = rand() % worldSize;
    if (worldMap[y][x] == '_') {
      defineRoom(x, y, 'w');
    }
  }
  // generates rocks randomly over the grass tiles
  for (int i = 0; i < 25; i++) {
    int x = rand() % worldSize;
    int y = rand() % worldSize;
    if (worldMap[y][x] == '_' || worldMap[y][x] == 'w') {
      defineRoom(x, y, '*');
    }
  }
  // generates trees randomly over the cliffs
  for (int i = 0; i < 50; i++) {
    int x = rand() % worldSize;
    int y = rand() % worldSize;
    if (worldMap[y][x] == '#') {
      defineRoom(x, y, 'T');
    }
  }

  // creates walls at the edge of the map
  for (int x = 0; x < worldSize; x++) {
    defineRoom(x, 0, '#');
  }
  for (int x = 0; x < worldSize; x++) {
    defineRoom(x, worldSize - 1, '#');
  }
  for (int y = 0; y < worldSize; y++) {
    defineRoom(0, y, '#');
  }
  for (int y = 0; y < worldSize; y++) {
    defineRoom(worldSize - 1, y, '#');
  }

  placeHouse();
}

bool checkTile(int x, int y) {
  if (worldMap[y][x] != '#') {
    return true;
  } else {
    return false;
  }
}

void placeHouse() {
  int reachableTiles[worldSize][worldSize];
  int listLength = 4;

  for (int x = 0; x < worldSize; x++) {
    for (int y = 0; y < worldSize; y++) {
      reachableTiles[y][x] = 0;
    }
  }

  // starting list information
  int startingList[4][2];
  if (checkTile(playerX - 1, playerY)) {
    startingList[0][1] = playerX - 1;
    startingList[0][0] = playerY;
    reachableTiles[playerX - 1][playerY] = 1;
  }
  if (checkTile(playerX + 1, playerY)) {
    startingList[1][0] = playerX + 1;
    startingList[1][1] = playerY;
    reachableTiles[playerX + 1][playerY] = 1;
  }
  if (checkTile(playerX, playerY - 1)) {
    startingList[2][0] = playerX;
    startingList[2][1] = playerY - 1;
    reachableTiles[playerX][playerY - 1] = 1;
  }
  if (checkTile(playerX, playerY + 1)) {
    startingList[3][0] = playerX;
    startingList[3][1] = playerY + 1;
    reachableTiles[playerX][playerY + 1] = 1;
  }

  for (int i = 0;
       i < ((worldSize * 2) - 3); // this set of loops fills out the reachable
                                  // tiles with a kind of flood filling method.
       i++) { // subtract the starting list info, subtract one bcs of corners on
              // the map.

    for (int y = 0; y < worldSize; y++) {
      for (int x = 0; x < worldSize; x++) {
        int dist = 1;
        if (reachableTiles[x][y] == 1) {
          if (checkTile(x - 1, y)) {
            reachableTiles[x - 1][y] = dist;
          }
          if (checkTile(x + 1, y)) {
            reachableTiles[x + 1][y] = dist;
          }
          if (checkTile(x, y - 1)) {
            reachableTiles[x][y - 1] = dist;
          }
          if (checkTile(x, y + 1)) {
            reachableTiles[x][y + 1] = dist;
          }
        }
      }
    }
  }

  // sets distance for all reachable tiles
  for (int x = 0; x < worldSize; x++) {
    for (int y = 0; y < worldSize; y++) {
      int dist = 1;
      if (abs(x - playerX) >= 3 || abs(y - playerY) >= 3) {
        dist = 2;
      }
      if (abs(x - playerX) >= 6 || abs(y - playerY) >= 6) {
        dist = 3;
      }
      if (reachableTiles[x][y] == 1) {
        reachableTiles[x][y] = dist;
      }
    }
  }

  // prints all reachable tiles to the screen
  // for (int x = 0; x < worldSize; x++) {
  //   for (int y = 0; y < worldSize; y++) {
  //     cout << reachableTiles[y][x] << " ";
  //   }
  //   cout << endl;
  // }

  // sets variables that dictate the possible distances from the player.
  bool placedHouse = false;
  bool distThree = false;
  bool distTwo = false;
  for (int i = 0; i < 3; i++) {
    for (int x = 0; x < worldSize; x++) {
      for (int y = 0; y < worldSize; y++) {
        if (distThree == false) {
          if (reachableTiles[x][y] == 3) {
            distThree = true;
          }
        } else if (distTwo == false) {
          if (reachableTiles[x][y] == 2) {
            distTwo = true;
          }
        }
      }
    }
  }

  distThree = false;

  // depending on the possible distances, the house is placed at one of the
  // distances.
  if (distThree) {
    bool placingHouse = true;
    while (placingHouse) {
      int randX = rand() % worldSize;
      int randY = rand() % worldSize;
      if (reachableTiles[randX][randY] == 3) {
        defineRoom(randX, randY, 'H');
        placingHouse = false;
      }
    }
  } else if (distTwo) {
    bool placingHouse = true;
    while (placingHouse) {
      int randX = rand() % worldSize;
      int randY = rand() % worldSize;
      if (reachableTiles[randX][randY] == 2) {
        defineRoom(randX, randY, 'H');
        placingHouse = false;
      }
    }
  } else {
    bool placingHouse = true;
    while (placingHouse) {
      int randX = rand() % worldSize;
      int randY = rand() % worldSize;
      if (reachableTiles[randX][randY] == 1) {
        defineRoom(randX, randY, 'H');
        placingHouse = false;
      }
    }
  }
}

void defineRoom(int roomX, int roomY, char roomType) {
  string startFeel[4] = {"A sense of overwhelming joy forms a ",
                         "The deep sorrow creates a ",
                         "Anxiety and tension constitute a ",
                         "A feeling of calm and tranquility makes up a "};
  string verbsFeel[10] = {"warm ",     "intense ",  "unsettling ", "gentle ",
                          "powerful ", "churning ", "comforting ", "restless ",
                          "serene ",   "stirring "};
  string descriptorFeel[4] = {"emotion.", "mood.", "state of being.",
                              "feeling."};

  string feelingDescription = startFeel[rand() % 4] + verbsFeel[rand() % 10] +
                              descriptorFeel[rand() % 4];

  if (roomType == '_') {
    worldMap[roomY][roomX] = '_';
    string start[4] = {"The earth forms a ", "The terrain forms a ",
                       "The surface of the ground makes up a ",
                       "The landscape makes up a  "};
    string verbs[10] = {"soggy ",       "mushy ",  "sparse ", "rocky ",
                        "uneven ",      "firm ",   "clayey ", "sandy ",
                        "hard-packed ", "cracked "};
    string descriptor[4] = {"lawn.", "field.", "pasture.", "meadow."};
    int verb1 = rand() % 10;
    int verb2 = rand() % 10;
    while (verb2 == verb1) {
      verb2 = rand() % 10;
    }
    int descriptor1 = rand() % 4;

    description[roomY][roomX] = start[rand() % 4] + verbs[verb1] + "and " +
                                verbs[verb2] + descriptor[descriptor1] + " " +
                                feelingDescription;

    simpleDesc[roomY][roomX] = "a " + verbs[verb1] + descriptor[descriptor1];

  } else if (roomType == 'w') {
    worldMap[roomY][roomX] = 'w';
    string start[4] = {"The towering plants create a ",
                       "The tall vegetation forms a ",
                       "The long grass constitutes a ",
                       "The field of tall greenery makes up a "};
    string verbs[10] = {"lush ",     "dense ",    "luxuriant ", "towering ",
                        "verdant ",  "thick ",    "overgrown ", "swaying ",
                        "majestic ", "bountiful "};
    string descriptor[4] = {"jungle around you.", "wilderness.", "thicket.",
                            "tall grassland."};
    int verb1 = rand() % 10;
    int verb2 = rand() % 10;
    while (verb2 == verb1) {
      verb2 = rand() % 10;
    }
    int descriptor1 = rand() % 4;

    description[roomY][roomX] = start[rand() % 4] + verbs[verb1] + "and " +
                                verbs[verb2] + descriptor[descriptor1] + " " +
                                feelingDescription;

    simpleDesc[roomY][roomX] = "a " + verbs[verb1] + descriptor[descriptor1];

  } else if (roomType == 'T') {
    worldMap[roomY][roomX] = 'T';
    string start[4] = {"A majestic tree stands as a ",
                       "The tall oak creates a ", "A single tree rises as a ",
                       "A sturdy trunk makes up a "};
    string verbs[10] = {"peaceful ", "towering ", "stately ", "ancient ",
                        "mighty ",   "leafy ",    "green ",   "solitary ",
                        "noble ",    "tranquil "};
    string descriptor[4] = {"tree.", "natural landmark.", "wooden sentinel.",
                            "towering oak."};

    int verb1 = rand() % 10;
    int verb2 = rand() % 10;
    while (verb2 == verb1) {
      verb2 = rand() % 10;
    }
    int descriptor1 = rand() % 4;

    description[roomY][roomX] = start[rand() % 4] + verbs[verb1] + "and " +
                                verbs[verb2] + descriptor[descriptor1] + " " +
                                feelingDescription;

    simpleDesc[roomY][roomX] = "a " + verbs[verb1] + descriptor[descriptor1];

  } else if (roomType == '#') {
    worldMap[roomY][roomX] = '#';

    string verbs[2] = {"tall ", "towering "};

    simpleDesc[roomY][roomX] = verbs[rand() % 2] + "cliffs.";

  } else if (roomType == '*') {
    worldMap[roomY][roomX] = '*';
    string start[4] = {
        "A massive pile of boulders forms a ", "The rugged stones create a ",
        "A jumble of rocks constitutes a ", "A mound of boulders makes up a "};
    string verbs[10] = {"sturdy ",  "towering ",      "rocky ",   "jagged ",
                        "rough ",   "formidable ",    "massive ", "hefty ",
                        "jutting ", "boulder-strewn "};
    string descriptor[4] = {"rock formation.", "rocky outcrop.",
                            "boulder heap.", "pile of stones."};

    int verb1 = rand() % 10;
    int verb2 = rand() % 10;
    while (verb2 == verb1) {
      verb2 = rand() % 10;
    }
    int descriptor1 = rand() % 4;

    description[roomY][roomX] = start[rand() % 4] + verbs[verb1] + "and " +
                                verbs[verb2] + descriptor[descriptor1] + " " +
                                feelingDescription;

    simpleDesc[roomY][roomX] = "a " + verbs[verb1] + descriptor[descriptor1];

  } else if (roomType == 'H') {
    worldMap[roomY][roomX] = 'H';
    houseCoords[0] = roomX;
    houseCoords[1] = roomY;
    simpleDesc[roomY][roomX] = "the house.";
  } else if (roomType == 'O') {
    worldMap[roomY][roomX] = 'O';
    playerMap[roomY][roomX] = 'i';
    description[roomY][roomX] =
        "You seem to recognise the slab below you. It looks as if you might "
        "have stood in this very spot many times before and you're not sure "
        "why.";
    simpleDesc[roomY][roomX] = "The slab you spawned on.";
    playerX = roomX;
    playerY = roomY;
  }
}
// need defineRoom to define a basic definition for the other rooms to call,
// and then an indepth definition for the room to call(?)

void showMap() {
  for (int x = 0; x < worldSize; x++) {
    for (int y = 0; y < worldSize; y++) {
      if (playerMap[x][y] == 'i') {
        cout << playerMap[x][y] << "  ";
      } else {
        cout << worldMap[x][y] << "  ";
      }
    }
    cout << endl;
  }
}

void clearPlayerMap() {
  for (int x = 0; x < worldSize; x++) {
    for (int y = 0; y < worldSize; y++) {
      playerMap[y][x] = '_';
    }
  }
}

void move(char direction) {
  playerMap[playerY][playerX] = '_';
  if (direction == 'n') {
    playerY -= 1;
  }
  if (direction == 'e') {
    playerX += 1;
  }
  if (direction == 's') {
    playerY += 1;
  }
  if (direction == 'w') {
    playerX -= 1;
  }

  if (worldMap[playerY][playerX] == 'H') {
    generateFloor();
    system("clear");
    clearPlayerMap();
    cout << "\n\nnext floor!" << endl;
    talkToGranny();
    currentFloor += 1;
  } else {
    playerMap[playerY][playerX] = 'i';
    system("clear");
  }
}

void talkToGranny() {
  char answer;
  if (currentFloor == 1) {
    system("clear");
    print("An old lady sits in a creaking old chair. \nIt makes you feel as if "
          "she's been waiting. For you.");
    print("She doesn't seem like she was waiting long.");
    print("\n\"Well, you've made it to the house, haven't you?\"");
    print("\"You've gone a good bit, now haven't you?\"");
    print("\"But something is... off. It's a little bit hard to describe, "
          "yeah?\"");
    print("\"It's almost like the world around you is being formed as you step "
          "around.\"");
    print(
        "\"Oh, you poor thing, you just don't really know what's happening, do "
        "you?\"\n(y)es, I know what's happening! / (n)o, I'm not too sure...");
    cin >> answer;
    system("clear");
    if (answer == 'y' || answer == 'Y') {
      print("\"Oh, do tell then! how do you know???\"");
      string reason;
      cin >> reason;
      print("\"hmmm...\"");
      print("\"Well, regardless, I'll see you next time either way, so I guess "
            "that's neat.\"");
    } else if (answer == 'n' || answer == 'N') {
      print("\"That's to be expected of you I suppose...\"");
      print("\"I suppose I'll see you in a bit then now won't I?\"");
      print("\"Who knows how long it will be though...\"");
    }

    print("you walk down the stairs you see leading down and start to realize "
          "that you're not underground.");
    print("there is trees, rocks, wait a minute... It's almost like everything "
          "you already experienced has been shifted around!");
    print("Almost instinctively, you look down.");
  } else if (currentFloor == 2) {
    system("clear");
    print("She's there again.");
    print("She's in the exact same spot, doing the exact same thing as last "
          "time.");
    print("... I think. Wait, what WAS she doing last time I saw her?");
    print("\n\"Oh hello dearie! I see you've found your way back to the house "
          "again. Glad you could make it!\"");
    print("\"How was the trip? find any bears?\"\n(y/n)");
    char response;
    cin >> response;
    system("clear");
    if (response == 'y' || response == 'Y') {
      print("\"Haha, you're funny, dearie.\"");
      print("\"There haven't been bears here for a while.\"");
      print("\"right now, it looks like it's just you, me, and the forest "
            "surrounding us.\"");
      print("\"Oh, and the house, of course.\"");
      print("She smiles slightly but you don't really feel like you want to "
            "believe what you are seeing right now.");
    } else if (response == 'n' || response == 'N') {
      print("\"Yeah, that's to be expected.\"");
      print("\"There hasn't been bears here for years.\"");
      print("You find that to be a little bit odd that in all this forest, "
            "there's no bears.");
      print("But you imediately realise that it's not that simple. It can't be "
            "that simple.");
      print("An anxious feeling takes over your entire body as you realize "
            "that it's just like before.");
    }
    print("Right in front of you, right there.");
    print("There, right there, lies yet another set of stairs.");
    print("The sound of wind blowing over the cliffs that seem to surround you "
          "in every direction isn't coming from the open door anymore.");
    print("It's coming from down the stairs.");
    print("Again, almost instinctively, you look down.");
  } else if (currentFloor == 3) {
    system("clear");
    print("It's just like last time");
    print("You can't help but think to yourself, isn't this getting a little "
          "annoying yet?");
    print("She's just there. Again. what happened to her? How did she get to "
          "this place?");
    print("Nothings changed since last time. It's ALL the exact same.");
    print("She finally faces you.");
    print("All she has to say is \"Dearie... Do you remember what it was like "
          "before?\"");
    print("\n(Y)es, I think I do?/(N)o... I don't...");
    char response2;
    cin >> response2;
    system("clear");
    if (response2 == 'y' || response2 == 'Y') {
      print("\"Oh, that's good to know.\"");
      print("\"Best be careful, you might become me if you're not careful.\"");
    } else if (response2 == 'n' || response2 == 'N') {
      print("\"Oh... well, I think that's fine. You'll see what this world has "
            "to offer soon enough.\"");
      print("\"I have sure as hell gotten my fair share of everything here.\"");
      print("\"It's all the same. Always has been.\"");
      print("\"It's pretty but that's about as deep as it goes.\"");
    }
    print("You're not sure why, but you feel like you have a deeper connection "
          "with her. The granny. She seems closer to you than you thought "
          "last... \"Floor\".");
    print("Just as you expected, at the bottom of the stairs, there lies it.");
    print("What is this, like the fourth Floor that you're walking into now?");
  } else if (currentFloor == 4) {
    system("clear");
    print("\"How long do you think you've been in here now?\"");
    print("The granny looks tired and a bit sad.");
    print(
        "\"I've... I lost track a long time ago.\"\n\"Every time that I see "
        "you, I feel like I can't seem to tell any difference in the world.\"");
    print("\"Is there any differences between you and me? What's the whole "
          "point of what's going on in here??\"");
    print("\"I've been here for centuries, and I've never seen someone like "
          "you in all this time.\"");
    print("\"Actually, I think it's been millenia.\"");
    print("\"No... wait...\"");
    print("she looks down and doesn't seem like she really knows how to "
          "feel.\nEverything seems just as it was.\n\n\n    How long has it "
          "REALLY been?\n\n");
    print("What have you really been doing?\nJust exploring?\nExploring "
          "what?\nThis is a world where nothing changes. It's all very "
          "generic.\nThe plants... The trees... All of it. What is it?\n Is it "
          "any different from descriptions in a book?");
    print("\"I don't remember how long it's been.\"");
    print("\"The only thing I do remember really... is when I used to look "
          "like you.\"");
    print("\"So pretty...\"");
    print("\n\n\nIt's time to go. There isn't anything more to do.");
    print("will you leave down the stairs? (y/n)");
    char response3;
    cin >> response3;
    system("clear");
    if (response3 == 'n' || response3 == 'N') {
      print("\"What are you still doing here? do you need something?? who told "
            "you to come here???\"\n\"You don't remember??\"");
      print("\"You HAVE to remember.\"");
      print("\"I can't look at you anymore. You remind me too much about what "
            "I've lost.\"");
    }
    print("\n\n");
    print("you leave down the stairs, just as you have, the last 4 or 5 "
          "times.\nSomething feels off about the granny... you're not entirely "
          "sure what but it seems like she's not going to be ok.");
  } else if (currentFloor >= 5) {
    system("clear");
    if (currentFloor == 5) {
      print("She's gone.");
    } else {
      print("She's still gone.");
    }
    print("\n\n\n\n\n\n");
    print("Will you sit in her chair? (y/n)");
    char response4;
    cin >> response4;
    system("clear");
    if (response4 == 'y' || response4 == 'Y') {
      print("You sit down.");
      print("Suddenly it all hits you.");
      print("You've been here for centuries; Maybe even melenia.");
      print("You hear a knock at the door");
      print("A young woman walks in.");
      print("\n\"Well, you've made it to the house, haven't you?\", you say to "
            "her.");
      gameOver = true;
    } else if (response4 == 'n' || response4 == 'N') {
      print("That's ok. Take as long as you like. \nYou could take a look "
            "around the house if you want but There's not much point to that.");
      print("Will you look around? (y/n)");
      char lookAround;
      cin >> lookAround;
      system("clear");
      if (lookAround == 'y' || lookAround == 'Y') {
        print(
            "You find some painting supplies. Would you like to paint? (y/n)");
        char paint;
        cin >> paint;
        system("clear");
        if (paint == 'y' || paint == 'Y') {
          print(
              "It's been hours. You take a look back to see your work. It "
              "seems a bit... impressionistic.\nIt reminds you of granny.\n\n");
          print("-------------");
          print("- |   - |   -");
          print("- |   - | | -");
          print("-------------");
          print("- | | - |   -");
          print("- | | - | ==-");
          print("-------------");
          print("\n\nWhat should you call it?");
          string name;
          cin >> name;
          if (name == "loss" || name == "Loss") {
            print("Granny is gone. It's good to know you knew how to properly "
                  "appreciate her.");
          }
          cout << name << "; circa 2023." << endl;

          print("would you like to sit in the chair, or go down the stairs? "
                "\n(y)es, I'll sit/(n)o, I'm going down the stairs again.");
          char sit;
          cin >> sit;
          system("clear");
          if (sit == 'y' || sit == 'Y') {
            print("You sit down.");
            print("Suddenly it all hits you.");
            print("You've been here for centuries; Maybe even melenia.");
            print("You hear a knock at the door");
            print("A young woman walks in.");
            print("\n\"Well, you've made it to the house, haven't you?\", you "
                  "say to her.");
            gameOver = true;
          }
        }
        print("you head down the stairs. \nYou're not sure how long you've "
              "been going down these same stairs at this point. \nToo many "
              "times to remember at least.");
      }
    }
  }
}

// awooga 69 go brr

void print(string msg) { cout << msg << endl; }