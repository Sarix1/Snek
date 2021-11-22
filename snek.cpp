#include <SDL.h>
#include <SDL_image.h>
#include <stdio.h>
#include <string>
#include <vector>
#include <cmath>

#define BASE_WIDTH 320.0
#define BASE_HEIGHT 240.0

// Function definition list
void SetResolution(int);
bool Init();
SDL_Texture* LoadTexture(std::string);
bool StoreTexture(std::string);
SDL_Surface* GenerateGrass();
void Close();

// Global variables

// Time
unsigned int lastTime = 0, currentTime, loseTime;
Uint16 gameSpeed = 20, gameSpeedTemp = gameSpeed * 10;
const Uint16 stepNormal = 10;
Uint16 stepLogic = 2000 / gameSpeed;
Uint16 step = stepNormal;

// Math
const double CIRCLE = M_PI / 180.0;

// Graphics
int screenWidth;
int screenHeight;
SDL_Window* gWindow = NULL;
SDL_Renderer* gRenderer = NULL;
std::vector<SDL_Texture*> gTextures;

// Control
bool userKey, userLeft, userRight, userUp, userDown, userEnter, userSpace, userEsc;
bool userPressedKey, userPressedLeft, userPressedRight, userPressedUp, userPressedDown, userPressedEnter, userPressedSpace, userPressedEsc;

// Game states
bool stateTitleScreen, stateMenu, stateOptions, stateResolution, stateFullScreen, stateFullScreenTemp, stateSoftFilter, stateSoftFilterTemp, stateInGame, stateGameStart, stateGameRunning, stateGameOver;

// Menu and option selections
int menuSelect = 0, optionsSelect = 0, resolutionSelect = 1, resolutionSelectTemp = resolutionSelect;

// Game Over stuff
double positionGameOver = -32, velocityGameOver = 0;

// SDL_Rects
SDL_Rect Viewport;
SDL_Rect RectTitle;
SDL_Rect RectMenu;
SDL_Rect RectOptions;
SDL_Rect RectResolution;
SDL_Rect RectResolutionSource;
SDL_Rect RectArrow;
SDL_Rect RectMiniArrow;
SDL_Rect RectHammer;
SDL_Rect RectTickbox;
SDL_Rect RectMeter;
SDL_Rect RectPointer;
SDL_Rect RectSnakeSource;
SDL_Rect RectSnakeDest;
SDL_Rect RectApple;
SDL_Rect RectGameOver;

// Enums

// Menu enum
enum MenuSelect
{
	START,
	SCORES,
	OPTIONS,
	QUIT,
	MENU_SIZE
};

// Options enum
enum OptionsSelect
{
	RESOLUTION,
	FULL_SCREEN,
	SOFT_FILTER,
	GAME_SPEED,
	APPLY_CHANGES,
	OPTIONS_SIZE
};

// Resolution enum
enum ResolutionSelect
{
	RES_320X240,
	RES_640X480,
	RES_960X720,
	RES_1280X960,
	RES_NATIVE,
	RES_SIZE
};

// Snake direction enum
enum SnakeDirection
{
	UP,
	DOWN,
	LEFT,
	RIGHT
};

// Function implementations

// Set SDL_Rects and resolution
void SetResolution(int res)
{
	switch (res)
	{
	case RES_320X240:
		screenWidth = 320;
		screenHeight = 240;
		break;

	case RES_640X480:
		screenWidth = 640;
		screenHeight = 480;
		break;

	case RES_960X720:
		screenWidth = 960;
		screenHeight = 720;
		break;

	case RES_1280X960:
		screenWidth = 1280;
		screenHeight = 960;
		break;

	case RES_NATIVE:
		// Get desktop resolution
		SDL_DisplayMode DM;
		SDL_GetDesktopDisplayMode(0, &DM);
		screenWidth = DM.w;
		screenHeight = DM.h;
		break;
	}

	// Viewport
	Viewport.x = 0;
	Viewport.y = 0;
	Viewport.w = screenWidth;
	Viewport.h = screenHeight;
	SDL_RenderSetViewport(gRenderer, &Viewport);

	// Title
	RectTitle.x = 0;
	RectTitle.y = 0;
	RectTitle.w = int(256.0 / BASE_WIDTH * screenWidth);
	RectTitle.h = int(64.0 / BASE_HEIGHT * screenHeight);

	// Menu
	RectMenu.x = 0;
	RectMenu.y = 0;
	RectMenu.w = int(128.0 / BASE_WIDTH * screenWidth);
	RectMenu.h = int(128.0 / BASE_HEIGHT * screenHeight);

	// Options
	RectOptions.x = 0;
	RectOptions.y = 0;
	RectOptions.w = int(256.0 / BASE_WIDTH * screenWidth);
	RectOptions.h = int(192.0 / BASE_HEIGHT * screenHeight);

	// Resolution
	RectResolution.x = 0;
	RectResolution.y = 0;
	RectResolution.w = int(128.0 / BASE_WIDTH * screenWidth);
	RectResolution.h = int(32.0 / BASE_HEIGHT * screenHeight);

	// Resolution Source
	RectResolutionSource.x = 0;
	RectResolutionSource.y = 0;
	RectResolutionSource.w = 128;
	RectResolutionSource.h = 32;

	// Arrow
	RectArrow.x = 0;
	RectArrow.y = 0;
	RectArrow.w = int(32.0 / BASE_WIDTH * screenWidth);
	RectArrow.h = int(32.0 / BASE_HEIGHT * screenHeight);

	// MiniArrow
	RectMiniArrow.x = 0;
	RectMiniArrow.y = 0;
	RectMiniArrow.w = int(16.0 / BASE_WIDTH * screenWidth);
	RectMiniArrow.h = int(16.0 / BASE_HEIGHT * screenHeight);

	// Hammer
	RectHammer.x = 0;
	RectHammer.y = 0;
	RectHammer.w = int(32.0 / BASE_WIDTH * screenWidth);
	RectHammer.h = int(32.0 / BASE_HEIGHT * screenHeight);

	// Tickbox
	RectTickbox.x = 0;
	RectTickbox.y = 0;
	RectTickbox.w = int(32.0 / BASE_WIDTH * screenWidth);
	RectTickbox.h = int(32.0 / BASE_HEIGHT * screenHeight);

	// Meter
	RectMeter.x = 0;
	RectMeter.y = 0;
	RectMeter.w = int(64.0 / BASE_WIDTH * screenWidth);
	RectMeter.h = int(16.0 / BASE_HEIGHT * screenHeight);

	// Meter Scale Pointer
	RectPointer.x = 0;
	RectPointer.y = 0;
	RectPointer.w = int(screenWidth / BASE_WIDTH);
	RectPointer.h = int(screenHeight / BASE_HEIGHT * 10);

	// Snake Source Tile
	RectSnakeSource.x = 0;
	RectSnakeSource.y = 0;
	RectSnakeSource.w = 16;
	RectSnakeSource.h = 16;

	// Snake Destination Tile
	RectSnakeDest.x = 0;
	RectSnakeDest.y = 0;
	RectSnakeDest.w = int(16.0 / BASE_WIDTH * screenWidth);
	RectSnakeDest.h = int(16.0 / BASE_HEIGHT * screenHeight);

	// Apple
	RectApple.x = 0;
	RectApple.y = 0;
	RectApple.w = int(16.0 / BASE_WIDTH * screenWidth);
	RectApple.h = int(16.0 / BASE_HEIGHT * screenHeight);

	// Game Over
	RectGameOver.x = int(screenWidth / BASE_WIDTH * (BASE_WIDTH / 2 - 64));
	RectGameOver.y = 0;
	RectGameOver.w = int(128.0 / BASE_WIDTH * screenWidth);
	RectGameOver.h = int(32.0 / BASE_HEIGHT * screenHeight);
}

// Initialization function
bool Init()
{
	// Success flag
	bool success = true;

	// SDL video failure
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		printf("Failed to initialize SDL. SDL_Error: %s\n", SDL_GetError());
		success = false;
	}

	// SDL video success
	else
	{
		// Set texture filtering
		if (!SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "0"))
		{
			printf("Warning: Linear texture filtering not enabled!");
		}

		// Create window
		gWindow = SDL_CreateWindow("Snek",
			SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
			screenWidth, screenHeight, SDL_WINDOW_SHOWN);

		// Window creation failure
		if (gWindow == NULL)
		{
			printf("Failed to create window. SDL_Error: %s\n", SDL_GetError());
			success = false;
		}

		// Window creation success
		else
		{
			// Create renderer for gWindow
			gRenderer = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_ACCELERATED);

			// Renderer failure
			if (gRenderer == NULL)
			{
				printf("Failed to create renderer. SDL Error: %s\n", SDL_GetError());
				success = false;
			}

			// Renderer success
			else
			{
				// Set renderer colour
				SDL_SetRenderDrawColor(gRenderer, 0, 0, 0, 255);

				// PNG loading
				int imgFlags = IMG_INIT_PNG;

				// Failure
				if (!(IMG_Init(imgFlags) & imgFlags))
				{
					printf("Failed to initialize SDL_image. SDL_image Error: %s\n", IMG_GetError());
					success = false;
				}
			}
		}
	}

	return success;
}

// Load texture from file
SDL_Texture* LoadTexture(std::string path)
{
	// Final texture
	SDL_Texture* newTexture = NULL;

	// Load image to surface at specified path
	SDL_Surface* loadedSurface = IMG_Load(path.c_str());

	// Color key surface
	SDL_SetColorKey(loadedSurface, SDL_TRUE, SDL_MapRGB(loadedSurface->format, 0, 0, 255));

	// Failure
	if (loadedSurface == NULL)
	{
		printf("Failed to load image %s. SDL_image Error: %s\n", path.c_str(), IMG_GetError());

		return NULL;
	}

	// Success
	else
	{
		// Create texture from surface
		newTexture = SDL_CreateTextureFromSurface(gRenderer, loadedSurface);

		// Failure
		if (newTexture == NULL)
		{
			printf("Failed to create texture from %s. SDL Error: %s\n", path.c_str(), SDL_GetError());
		}

		// Free surface
		SDL_FreeSurface(loadedSurface);
	}

	return newTexture;
}

// Load and store texture into a vector container
bool StoreTexture(std::string path)
{
	// Load texture
	SDL_Texture* gTexture = NULL;
	gTexture = LoadTexture(path);

	// Failure
	if (gTexture == NULL)
	{
		printf("Failed to load texture image.\n");
		return false;
	}

	// Add texture to storage
	gTextures.push_back(gTexture);

	return true;
}

// Generate random field of grass
SDL_Surface* GenerateGrass()
{
	// Load source texture
	SDL_Surface* gGrass = IMG_Load("grass.png");

	// Destination surface
	SDL_Surface* gGrassField = SDL_CreateRGBSurface(NULL, int(BASE_WIDTH), int(BASE_HEIGHT), 24, 0, 0, 0, 255);

	// Failure
	if (gGrass == NULL)
	{
		printf("Failed to load texture image.\n");
		return NULL;
	}

	// Success
	else
	{
		// Set random seed
		srand(currentTime);
		
		// Source of grass patch
		SDL_Rect RectGrassSource;
		RectGrassSource.x = 0;
		RectGrassSource.y = 0;
		RectGrassSource.w = 32;
		RectGrassSource.h = 32;

		// Destination of grass patch
		SDL_Rect RectGrassDest;
		RectGrassDest.x = 0;
		RectGrassDest.y = 0;
		RectGrassDest.w = 32;
		RectGrassDest.h = 32;

		// Place 32x32 patches of grass on destination surface
		for (int y = 0; y < 8; ++y)
		{
			RectGrassDest.y = y * 32;

			for (int x = 0; x < 10; ++x)
			{
				RectGrassSource.x = rand() % 8 * 32;
				RectGrassDest.x = x * 32;
				SDL_BlitSurface(gGrass, &RectGrassSource, gGrassField, &RectGrassDest);
			}
		}

		return gGrassField;
	}
}

// Close function
void Close()
{
	// Destroy textures
	for (SDL_Texture* i : gTextures)
	{
		SDL_DestroyTexture(i);
		i = NULL;
	}

	// Clear texture vector
	gTextures.clear();

	// Destroy window
	SDL_DestroyWindow(gWindow);
	gWindow = NULL;

	// Quit SDL
	SDL_Quit();
}

// Main
int main(int argc, char* args[])
{
	// Set resolution
	SetResolution(resolutionSelect);

	// Initialize SDL and create window

	// Failure
	if (!Init())
	{
		printf("Failed to initialize.\n");
	}

	// Success
	else
	{

		// Main loop flag
		bool quit = false;

		// Start in title
		stateTitleScreen = true;
		stateMenu = false;
		stateInGame = false;

		// Event handler singleton
		SDL_Event Event;

		// Load textures
		StoreTexture("splash.png");
		SDL_Texture* gSplash = gTextures.back();
		SDL_Texture* gBackground = gSplash;

		StoreTexture("title.png");
		SDL_Texture* gTitle = gTextures.back();

		StoreTexture("menu.png");
		SDL_Texture* gMenu = gTextures.back();

		StoreTexture("options.png");
		SDL_Texture* gOptions = gTextures.back();

		StoreTexture("resolution.png");
		SDL_Texture* gResolution = gTextures.back();

		StoreTexture("arrow.png");
		SDL_Texture* gArrow = gTextures.back();

		StoreTexture("arrow_up.png");
		SDL_Texture* gArrowUp = gTextures.back();

		StoreTexture("arrow_dn.png");
		SDL_Texture* gArrowDown = gTextures.back();

		StoreTexture("hammer.png");
		SDL_Texture* gHammer = gTextures.back();

		StoreTexture("tickbox.png");
		SDL_Texture* gTickbox = gTextures.back();

		StoreTexture("tick.png");
		SDL_Texture* gTick = gTextures.back();

		StoreTexture("meter.png");
		SDL_Texture* gMeter = gTextures.back();

		StoreTexture("snake.png");
		SDL_Texture* gSnake = gTextures.back();

		StoreTexture("applered.png");
		SDL_Texture* gAppleRed = gTextures.back();

		StoreTexture("applegreen.png");
		SDL_Texture* gAppleGreen = gTextures.back();

		StoreTexture("gameover.png");
		SDL_Texture* gGameOver = gTextures.back();

		SDL_Texture* gGrassTexture = NULL;

		// Main loop
		while (!quit)
		{
			// Update after x ticks
			currentTime = SDL_GetTicks();
			if (currentTime > lastTime + step)
			{
				// Process event queue
				while (SDL_PollEvent(&Event) != 0)
				{
					// Quit
					if (Event.type == SDL_QUIT) quit = true;

					// Key unpress
					else if (Event.type == SDL_KEYUP)
					{
						userKey = false;

						// Key actions
						switch (Event.key.keysym.sym)
						{
						case SDLK_UP:
							userUp = false;
							break;

						case SDLK_DOWN:
							userDown = false;
							break;

						case SDLK_LEFT:
							userLeft = false;
							break;

						case SDLK_RIGHT:
							userRight = false;
							break;

						case SDLK_RETURN:
							userEnter = false;
							break;

						case SDLK_SPACE:
							userSpace = false;
							break;

						case SDLK_ESCAPE:
							userEsc = false;
							break;

						default:
							break;
						}
					}

					// Key press
					else if (Event.type == SDL_KEYDOWN)
					{
						userKey = true;
						userPressedKey = true;

						// Key actions
						switch (Event.key.keysym.sym)
						{
						case SDLK_UP:
							userUp = true;
							userPressedUp = true;
							break;

						case SDLK_DOWN:
							userDown = true;
							userPressedDown = true;
							break;

						case SDLK_LEFT:
							userLeft = true;
							userPressedLeft = true;
							break;

						case SDLK_RIGHT:
							userRight = true;
							userPressedRight = true;
							break;

						case SDLK_RETURN:
							userEnter = true;
							userPressedEnter = true;
							break;

						case SDLK_SPACE:
							userSpace = true;
							userPressedSpace = true;
							break;

						case SDLK_ESCAPE:
							userEsc = true;
							userPressedEsc = true;
							break;

						default:
							break;
						}
					}
				}

				// Title state
				if (stateTitleScreen == true)
				{
					// Set speed
					step = stepNormal;

					// Enter menu?
					if (!userEsc && userKey && !stateMenu)
					{
						stateMenu = true;
						userEnter = false;
						userKey = false;
					}

					// Bouncy title logo
					const double TITLE_R = 10.0;
					static Uint16 titleSpin = 0;

					RectTitle.x = (screenWidth * 0.1) + int(sin(titleSpin * CIRCLE) * (TITLE_R / BASE_WIDTH) * screenWidth / 2);
					RectTitle.y = (screenHeight * 0.1) + int(cos(titleSpin * CIRCLE) * (TITLE_R / BASE_HEIGHT) * screenHeight);
					titleSpin += 2; if (titleSpin >= 360) titleSpin = 0;
					
					// Update graphics

					// Clear screen
					SDL_RenderClear(gRenderer);

					// Render background
					SDL_RenderCopy(gRenderer, gBackground, NULL, NULL);

					// Render title
					SDL_RenderCopy(gRenderer, gTitle, NULL, &RectTitle);
				}

				// Menu state
				if (stateMenu)
				{
					// Escape from menu?
					if (userEsc)
					{
						stateMenu = false;
						menuSelect = 0;
					}

					// If staying in menu
					else
					{
						if (userKey)
						{

							// Press down in menu
							if (userDown && menuSelect < MENU_SIZE - 1)
							{
								menuSelect += 1;
								userDown = false;
							}

							// Press up in menu
							else if (userUp && menuSelect > 0)
							{
								menuSelect -= 1;
								userUp = false;
							}

							// Press enter in menu
							else if (userEnter)
							{
								switch (menuSelect)
								{
								case START:
									stateTitleScreen = false;
									stateMenu = false;
									stateInGame = true;
									stateGameStart = true;
									step = stepLogic;
									break;

								case SCORES:
									break;

								case OPTIONS:
									stateTitleScreen = false;
									stateMenu = false;
									stateOptions = true;
									break;

								case QUIT:
									quit = true;
									break;

								default:
									break;
								}

								userEnter = false;
								userKey = false;
							}
						}

						// Render menu graphics

						// Spinny menu
						const double MENU_R = 5.0;
						static Uint16 menuSpin = 0;

						RectMenu.x = (screenWidth * 0.3) - int(sin(menuSpin * CIRCLE) * (MENU_R / BASE_WIDTH) * screenWidth);
						RectMenu.y = (screenHeight * 0.4) + int(cos(menuSpin * CIRCLE) * (MENU_R / BASE_HEIGHT) * screenHeight);
						menuSpin += 3; if (menuSpin >= 360) menuSpin = 0;
						
						// Render menu
						SDL_RenderCopy(gRenderer, gMenu, NULL, &RectMenu);

						// Set arrow Y position
						RectArrow.y = (screenHeight * 0.45) + (menuSelect * screenHeight / 9.5);

						// Bouncy arrow X position
						const double ARROW_R = 5.0; 
						static Uint16 arrowSpin = 0;

						RectArrow.x = (screenWidth * 0.25) + int(sin(arrowSpin * CIRCLE) * (ARROW_R / BASE_WIDTH) * screenWidth);
						arrowSpin += 10; if (arrowSpin >= 360) arrowSpin = 0;
						
						// Render arrow
						SDL_RenderCopy(gRenderer, gArrow, NULL, &RectArrow);
					}
				}

				// Options state
				if (stateOptions)
				{
					// Escape from options?
					if (userEsc && !stateResolution)
					{
						EscapeOptions:

						stateTitleScreen = true;
						stateMenu = true;
						stateOptions = false;
						userEsc = false;

						resolutionSelectTemp = resolutionSelect;
						stateFullScreenTemp = stateFullScreen;
						stateSoftFilterTemp = stateSoftFilter;
						gameSpeedTemp = gameSpeed * 10;
						optionsSelect = 0;
					}

					// If staying in options
					else
					{
						if (userKey && !stateResolution)
						{
							// Press down in options
							if (userDown && optionsSelect < OPTIONS_SIZE - 1)
							{
								optionsSelect += 1;
								userDown = false;
							}

							// Press up in options
							else if (userUp && optionsSelect > 0)
							{
								optionsSelect -= 1;
								userUp = false;
							}

							// Press enter in options
							else if (userEnter)
							{
								switch (optionsSelect)
								{
								case RESOLUTION:
									stateResolution = true;
									break;

								case FULL_SCREEN:
									stateFullScreenTemp = !stateFullScreenTemp;
									break;

								case SOFT_FILTER:
									stateSoftFilterTemp = !stateSoftFilterTemp;
									break;

								case APPLY_CHANGES:
									// Set resolution
									resolutionSelect = resolutionSelectTemp;
									SetResolution(resolutionSelect);
									SDL_SetWindowSize(gWindow, screenWidth, screenHeight);

									// Set fullscreen
									stateFullScreen = stateFullScreenTemp;
									if (stateFullScreen)
									{
										SDL_SetWindowFullscreen(gWindow, SDL_WINDOW_FULLSCREEN_DESKTOP);
										SetResolution(RES_NATIVE);
									}

									// Set filtering
									stateSoftFilter = stateSoftFilterTemp;
									if (stateSoftFilter) SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1");
									else SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "0");

									// Set speed
									gameSpeed = gameSpeedTemp / 10;
									stepLogic = 2000 / gameSpeed;

									userEnter = false;
									userKey = false;

									goto EscapeOptions;

									break;

								default:
									break;
								}

								userEnter = false;
								userKey = false;
							}

							if (userKey && optionsSelect == GAME_SPEED)
							{
								if (userLeft && gameSpeedTemp > 10) --gameSpeedTemp;
								if (userRight && gameSpeedTemp < 500) ++gameSpeedTemp;
							}

							if (userRight && optionsSelect == RESOLUTION)
								stateResolution = true;
						}

						// If in resolution selection and not pressed escape
						else if (userKey && stateResolution)
						{
							// Press down in resolution
							if (userDown && resolutionSelectTemp < RES_SIZE - 1)
							{
								resolutionSelectTemp += 1;
								userDown = false;
							}

							// Press up in resolution
							else if (userUp && resolutionSelectTemp > 0)
							{
								resolutionSelectTemp -= 1;
								userUp = false;
							}

							// Press enter in resolution
							else if (userEnter || userEsc || userLeft)
							{
								stateResolution = false;
								userEsc = false;
								userKey = false;
							}
						}

						// Render options graphics

						// Clear screen
						SDL_RenderClear(gRenderer);

						// Render background
						SDL_RenderCopy(gRenderer, gBackground, NULL, NULL);

						// Spinny options
						const double OPTIONS_R = 5.0;
						static Uint16 optionsSpin = 0;

						RectOptions.x = (screenWidth * 0.1) - int(sin(optionsSpin * CIRCLE) * (OPTIONS_R / BASE_WIDTH) * screenWidth);
						RectOptions.y = (screenHeight * 0.1) + int(cos(optionsSpin * CIRCLE) * (OPTIONS_R / BASE_HEIGHT) * screenHeight);
						optionsSpin += 3; if (optionsSpin >= 360) optionsSpin = 0;

						// Render options
						SDL_RenderCopy(gRenderer, gOptions, NULL, &RectOptions);

						// Set resolution option position
						RectResolution.x = RectOptions.x + (0.42 * screenWidth);
						RectResolution.y = RectOptions.y + (0.09 * screenHeight);

						// Set resolution option source to chosen option
						RectResolutionSource.y = 32 * resolutionSelectTemp;

						// Render resolution option
						SDL_RenderCopy(gRenderer, gResolution, &RectResolutionSource, &RectResolution);

						// Set tickbox 1 position and render
						RectTickbox.x = RectOptions.x + (0.45 * screenWidth);
						RectTickbox.y = RectOptions.y + (0.22 * screenHeight);
						SDL_RenderCopy(gRenderer, gTickbox, NULL, &RectTickbox);
						if (stateFullScreenTemp) SDL_RenderCopy(gRenderer, gTick, NULL, &RectTickbox);

						// Set tickbox 2 position and render
						RectTickbox.x = RectOptions.x + (0.46 * screenWidth);
						RectTickbox.y = RectOptions.y + (0.34 * screenHeight);
						SDL_RenderCopy(gRenderer, gTickbox, NULL, &RectTickbox);
						if (stateSoftFilterTemp) SDL_RenderCopy(gRenderer, gTick, NULL, &RectTickbox);

						// Render meter
						RectMeter.x = RectOptions.x + (0.5 * screenWidth);
						RectMeter.y = RectOptions.y + (0.5 * screenHeight);
						SDL_RenderCopy(gRenderer, gMeter, NULL, &RectMeter);

						// Render meter scale pointer
						RectPointer.x = RectOptions.x + (0.5075 * screenWidth) + (screenWidth / BASE_WIDTH * gameSpeedTemp / 10);
						RectPointer.y = RectOptions.y + (0.51 * screenHeight);
						SDL_SetRenderDrawColor(gRenderer, 255, 0, 0, 255);
						SDL_RenderFillRect(gRenderer, &RectPointer);

						// When selecting options
						if (!stateResolution)
						{
							// Set hammer Y position
							RectHammer.y = (screenHeight * 0.18) + (optionsSelect * screenHeight / 8.0);

							// Bouncy hammer X position
							const double HAMMER_R = 5.0;
							static Uint16 hammerSpin = 0;

							RectHammer.x = (screenWidth * 0.08) + int(sin(hammerSpin * CIRCLE) * (HAMMER_R / BASE_WIDTH) * screenWidth);
							hammerSpin += 10; if (hammerSpin >= 360) hammerSpin = 0;

							// Render hammer
							SDL_RenderCopy(gRenderer, gHammer, NULL, &RectHammer);
						}

						// When selecting resolutions
						else
						{
							// Bouncy mini arrow Y positions
							const double MINIARROW_R = 4.0;
							static Uint16 miniArrowSpin = 0;

							// Set mini arrow positions and render
							RectMiniArrow.x = RectOptions.x + (0.54 * screenWidth);

							if (resolutionSelectTemp > 0)
							{
								RectMiniArrow.y = RectOptions.y + (0.05 * screenHeight) + int(sin(miniArrowSpin * CIRCLE) * (MINIARROW_R / BASE_WIDTH) * screenHeight);
								SDL_RenderCopy(gRenderer, gArrowUp, NULL, &RectMiniArrow);
							}

							if (resolutionSelectTemp < RES_SIZE - 1)
							{
								RectMiniArrow.y = RectOptions.y + (0.21 * screenHeight) - int(sin(miniArrowSpin * CIRCLE) * (MINIARROW_R / BASE_WIDTH) * screenHeight);
								SDL_RenderCopy(gRenderer, gArrowDown, NULL, &RectMiniArrow);
							}

							miniArrowSpin += 20; if (miniArrowSpin >= 360) miniArrowSpin = 0;							
						}
					}
				}

				// Game state
				if (stateInGame)
				{
					// Game logic data
					static Uint8 Snake[15][20][4];
					const Uint8 snakeLengthStart = 1, snakeStartPosX = 10, snakeStartPosY = 7;
					static Uint8 snakeLength, snakeDirection, snakeDirectionLast;
					static char snakePosX, snakePosY, applePosX, applePosY;
					bool appleEaten = false;

					// Game start condition
					if (stateGameStart)
					{
						// Reset inputs
						userPressedKey = false;
						userPressedUp = false;
						userPressedDown = false;
						userPressedLeft = false;
						userPressedRight = false;
						userPressedEnter = false;
						userPressedSpace = false;
						userPressedEsc = false;

						// Game is obviously not over
						stateGameOver = false;

						// Generate grass
						gGrassTexture = SDL_CreateTextureFromSurface(gRenderer, GenerateGrass());

						// Clear snake table
						for (int y = 0; y < 15; ++y)
						{
							for (int x = 0; x < 20; ++x)
							{
								Snake[y][x][0] = 0;
								Snake[y][x][1] = 0;
								Snake[y][x][2] = 0;
								Snake[y][x][3] = 0;
							}
						}

						// Initialise variables
						snakeLength = snakeLengthStart;
						snakeDirection = RIGHT;
						snakeDirectionLast = RIGHT;
						snakePosX = snakeStartPosX;
						snakePosY = snakeStartPosY;
						appleEaten = false;

						// Place apple at start
						do {
							applePosX = rand() % 20;
							applePosY = rand() % 15;
						} while (applePosX == snakeStartPosX && applePosY == snakeStartPosY);

						// Finished
						stateGameStart = false;
					}

					// Escape?
					if (userEsc || userPressedEsc)
					{
						stateTitleScreen = true;
						stateInGame = false;
						stateGameRunning = false;
						userPressedEsc = false;
					}

					// Update game logic
					else if (stateGameRunning)
					{
						// Set 'timer' for snake part (element 0)
						if (Snake[snakePosY][snakePosX][0] == 0)
							Snake[snakePosY][snakePosX][0] = snakeLength;

						// Jump over snake condition goes here
						// else if (snake is jumping) --> set element 3 to appropriate tile // Not implemented yet

						// If square was not empty, snake crawled into itself
						else
						{
							stateGameOver = true;
							stateGameRunning = false;
								
							positionGameOver = -32;
							velocityGameOver = 0;

							step = stepNormal;
							loseTime = currentTime;
						}

						// Set correct snake sprite (element 1)
						if (snakeDirection == snakeDirectionLast)
						{
							if (snakeDirection < 2) Snake[snakePosY][snakePosX][1] = 0;
							else Snake[snakePosY][snakePosX][1] = 1;
						}
						else if (snakeDirectionLast == UP) Snake[snakePosY][snakePosX][1] = 2 + snakeDirection;
						else if (snakeDirectionLast == DOWN) Snake[snakePosY][snakePosX][1] = 4 + snakeDirection;
						else if (snakeDirectionLast == LEFT) Snake[snakePosY][snakePosX][1] = 7 - (snakeDirection * 2);
						else Snake[snakePosY][snakePosX][1] = 6 - (snakeDirection * 2);

						// Set direction (element 2)
						Snake[snakePosY][snakePosX][2] = snakeDirection;

						// Remember direction faced at start
						snakeDirectionLast = snakeDirection;

						// Control and movement
						switch (snakeDirection)
						{
						case UP:
							snakePosY--;
							if (userLeft || userPressedLeft) snakeDirection = LEFT;
							else if (userRight || userPressedRight) snakeDirection = RIGHT;
							break;

						case DOWN:
							snakePosY++;
							if (userLeft || userPressedLeft) snakeDirection = LEFT;
							else if (userRight || userPressedRight) snakeDirection = RIGHT;
							break;

						case LEFT:
							snakePosX--;
							if (userUp || userPressedUp) snakeDirection = UP;
							else if (userDown || userPressedDown) snakeDirection = DOWN;
							break;

						case RIGHT:
							snakePosX++;
							if (userUp || userPressedUp) snakeDirection = UP;
							else if (userDown || userPressedDown) snakeDirection = DOWN;
							break;
						
						default:
							break;
						}

						// Warp around

						// X wrap
						if (snakePosX < 0) snakePosX = 19;
						else if (snakePosX >= 20) snakePosX = 0;

						// Y wrap
						if (snakePosY < 0) snakePosY = 14;
						else if (snakePosY >= 15) snakePosY = 0;

						// Collect apple
						if (applePosX == snakePosX && applePosY == snakePosY)
						{
							appleEaten = true;
							++snakeLength;

							// Find new place for apple
							do {
								applePosX = rand() % 20;
								applePosY = rand() % 15;
							} while (!Snake[applePosY][applePosX][0] == 0);
						}

						// Reset inputs
						userPressedKey = false;
						userPressedUp = false;
						userPressedDown = false;
						userPressedLeft = false;
						userPressedRight = false;
						userPressedEnter = false;
						userPressedSpace = false;
						userPressedEsc = false;
					}

					// If game is not running, run when key pressed
					else if ((userKey || userPressedKey) && !stateGameOver)
					{
						stateGameRunning = true;
						userPressedKey = false;
					}

					// Render game graphics

					// Render grass
					SDL_RenderCopy(gRenderer, gGrassTexture, NULL, NULL);

					// Render snake body
					for (int y = 0; y < 15; ++y)
					{
						for (int x = 0; x < 20; ++x)
						{
							if (Snake[y][x][0] > 0)
							{
								// Set correct tile position (x from element 1)
								if (Snake[y][x][0] > 1)
								{
									RectSnakeSource.x = 16 * Snake[y][x][1];
									RectSnakeSource.y = 16;
								}

								// Get direction for tail (element 2) and set correct tile
								else
								{
									RectSnakeSource.x = 64 + (16 * Snake[y][x][2]);
									RectSnakeSource.y = 0;
								}

								// Decrement 'timer' for snake part (element 0) unless apple eaten
								if (!appleEaten && !stateGameOver)
								{
									--Snake[y][x][0];
								}

								// Scale destination tile for screen resolution
								RectSnakeDest.x = int(x * (16.0 / BASE_WIDTH) * screenWidth);
								RectSnakeDest.y = int(y * (16.0 / BASE_HEIGHT) * screenHeight);

								// Actual rendering
								SDL_RenderCopy(gRenderer, gSnake, &RectSnakeSource, &RectSnakeDest);
							}
						}
					}

					// Render snake head
					RectSnakeSource.x = snakeDirectionLast * 16;
					RectSnakeSource.y = 0;
					RectSnakeDest.x = int(snakePosX * (16.0 / BASE_WIDTH) * screenWidth);
					RectSnakeDest.y = int(snakePosY * (16.0 / BASE_HEIGHT) * screenHeight);
					SDL_RenderCopy(gRenderer, gSnake, &RectSnakeSource, &RectSnakeDest);

					// Render apple
					RectApple.x = int(applePosX * (16.0 / BASE_WIDTH) * screenWidth);
					RectApple.y = int(applePosY * (16.0 / BASE_HEIGHT) * screenHeight);
					SDL_RenderCopy(gRenderer, gAppleRed, NULL, &RectApple);

					// Game Over condition and render Game Over text
					if (stateGameOver)
					{
						++velocityGameOver;
						positionGameOver += velocityGameOver;

						if (positionGameOver / 10 > (BASE_HEIGHT - 32) / 2)
						{
							positionGameOver = (BASE_HEIGHT - 32) * 5;
							velocityGameOver *= -0.5;
						}

						RectGameOver.y = (positionGameOver / 10) * (screenHeight / BASE_HEIGHT);
						SDL_RenderCopy(gRenderer, gGameOver, NULL, &RectGameOver);
						
						// Go back to title screen after 4 seconds
						if (loseTime + 4000 < currentTime)
						{
							stateTitleScreen = true;
							stateInGame = false;
							stateGameRunning = false;
							userPressedEsc = false;
						}
					}
				}

				// Update screen
				SDL_RenderPresent(gRenderer);

				// Update time
				lastTime = currentTime;
			}
		}

		// Special case for grass
		SDL_DestroyTexture(gGrassTexture);
		gGrassTexture = NULL;
	}

	// Deallocate memory and quit
	Close();

	// End main()
	return 0;
}