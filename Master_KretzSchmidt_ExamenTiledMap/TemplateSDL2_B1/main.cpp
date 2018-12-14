#include <stdio.h>
#include <stdlib.h>
#include <SDL.h>
#include <SDL_image.h>
#include <time.h>
#include <List>
#include <string>
#include "tinyxml2.h"
#include <cerrno>
#include <cstdlib>
#include <cstring>
#include <ctime>

#define SCREEN_WIDTH 1920
#define SCREEN_HEIGHT 1080


#define LONGUEUR_TILE 24  // hauteur et largeur des tiles.
#define HAUTEUR_TILE 16 

#define NOMBRE_BLOCS_LONGUEUR 13  // NOMBRE DE BLOCS MAXIMUM DANS L'EDITEUR EN LONGUEUR ET EN LARGEUR
#define NOMBRE_BLOCS_LARGEUR 15

#define NOMBRE_TILES 8  //NOMBRE DE TILES PRESENT DANS LE FICHIER DE TILEDMAP ET LE NOMBRE DE LIGNES
#define NOMBRE_LIGNES 1


#if defined( _MSC_VER )
#if !defined( _CRT_SECURE_NO_WARNINGS )
#define _CRT_SECURE_NO_WARNINGS		// This test file is not intended to be secure.
#endif
#endif

#if defined( _MSC_VER ) || defined (WIN32)
#include <crtdbg.h>
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
_CrtMemState startMemState;
_CrtMemState endMemState;
#else
#include <sys/stat.h>
#include <sys/types.h>
#endif

using namespace tinyxml2;
using namespace std;
int gPass = 0;
int gFail = 0;


typedef struct game {


	SDL_Window *g_pWindow;
	SDL_Renderer *g_pRenderer;
	SDL_Texture *g_texture;
	SDL_Surface *g_surface;


}game;

typedef struct gameState {

	int g_bRunning;
	int screen;
	int left;
	int right;


}gameState;


typedef struct coordonnees {

	int x;
	int y;



}coordonnees;

int init(char *title, int xpos, int ypos, int height, int width, int flags, game *myGame);
void handleEvents(gameState *state, game *myGame, int *tilenumber, char table[NOMBRE_BLOCS_LONGUEUR][NOMBRE_BLOCS_LARGEUR]);
void cutTexture(game *myGame, gameState state, char table[NOMBRE_BLOCS_LONGUEUR][NOMBRE_BLOCS_LARGEUR]);
void delay(unsigned int frameLimit);
void destroyTexture(game *myGame);
void destroy(game *myGame);
void drawGrid(game *myGame);
void drawTile(int *tilenumber, game *myGame);
bool XMLTest(const char* testString, const char* expected, const char* found, bool echo, bool extraNL);
bool XMLTest(const char* testString, XMLError expected, XMLError found, bool echo, bool extraNL);
bool XMLTest(const char* testString, bool expected, bool found, bool echo, bool extraNL);
template< class T > bool XMLTest(const char* testString, T expected, T found, bool echo);
void create_map_file(char table[NOMBRE_BLOCS_LONGUEUR][NOMBRE_BLOCS_LARGEUR]);
void NullLineEndings(char* p);
//int create_map_file_V2(char tabmap[NOMBRE_BLOCS_LARGEUR*NOMBRE_BLOCS_HAUTEUR], int nummap)

int main(int argc, char *argv[])
{

	game myGame;
	gameState state;
	bool IsGrid = false;
	int tilenumber;
	
	
	char table[NOMBRE_BLOCS_LONGUEUR][NOMBRE_BLOCS_LARGEUR] = {
	{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
	{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
	{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
	{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
	{1,0,0,0,0,0,0,0,0,1,1,1,1,0,0},
	{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
	{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
	{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
	{0,3,4,0,0,0,0,0,0,0,2,2,2,0,0},
	{0,5,6,0,0,0,0,0,0,0,0,0,0,0,0},
	{0,5,6,0,0,0,0,0,0,0,0,0,0,0,0},
	{0,5,6,0,0,0,0,0,0,0,0,0,0,0,0},
	{7,7,7,7,7,7,7,7,7,7,7,7,7,7,7} };
	

	//Pour les 60 fps
	unsigned int frameLimit = SDL_GetTicks() + 16;


	if (init("TiledMap Examen", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN, &myGame)) {

		state.g_bRunning = 1;
		state.screen = 1;

	}
	else {

		return 1;//something's wrong

	}

	cutTexture(&myGame, state, table);
	drawGrid(&myGame);

	while (state.g_bRunning) {

		handleEvents(&state, &myGame, &tilenumber, table);
		//cutTexture(&myGame, state, table);
		//drawTile(&tilenumber,&myGame);
		//DESSIN DE LA GRILLE 
		//TODO
		//BUG LA GRILLE SE REFRESH EN PERMANENCE
		/*
		if ((!IsGrid) && (state.screen == 2)) {
			dessineGrid(&myGame);
			IsGrid = true;
		}
		*/


		// Gestion des 60 fps (1000ms/60 = 16.6 -> 16
		delay(frameLimit);
		frameLimit = SDL_GetTicks() + 16;


	}

	destroy(&myGame);



	// unload the dynamically loaded image libraries
	IMG_Quit();
	//quit sdl2
	SDL_Quit();



	
	return 0;
}


int init(char *title, int xpos, int ypos, int height, int width, int flags, game *myGame) {
	//BUT	 CREER LA FENETRE SDL ET L'INITIALISER
	//ENTREE LE NOM DE LA FENETRE, SA POSITION DE DEPART EN X ET EN Y, AINSI QUE SES DIMENSIONS, SES PARAMETRES (FLAGS)? AINSI QUE LA FENETRE
	//SORTIE LA FENETRE INITIALISE

	//initialize SDL

	if (SDL_Init(SDL_INIT_EVERYTHING) >= 0)
	{
		//if succeeded create our window
		myGame->g_pWindow = SDL_CreateWindow(title, xpos, ypos, height, width, flags);
		//if succeeded create window, create our render
		if (myGame->g_pWindow != NULL) {
			myGame->g_pRenderer = SDL_CreateRenderer(myGame->g_pWindow, -1, SDL_RENDERER_ACCELERATED);
			SDL_SetRenderDrawColor(myGame->g_pRenderer, 100, 100, 100, 255);
			SDL_RenderClear(myGame->g_pRenderer);

			int flags = IMG_INIT_JPG | IMG_INIT_PNG;
			int initted = IMG_Init(flags);
			if ((initted&flags) != flags) {
				printf("IMG_Init: Failed to init required jpg and png support!\n");
				printf("IMG_Init: %s\n", IMG_GetError());
				// handle error
			}

		}


	}
	else {


		return 0;
	}

	return 1;

}

void handleEvents(gameState *state, game *myGame, int *tilenumber, char table[NOMBRE_BLOCS_LONGUEUR][NOMBRE_BLOCS_LARGEUR]) {
	//BUT	 GESTION DES DIFFERENTS EVENEMENTS POUVANT APPARAITRE LORS DE NOTRE JEU
	//ENTREE L'ETAT DU PROGRAMME (SI IL EST ACTIF OU NON), LA FENETRE, LE NUMERO DE LA TILE POTENTIELLEMENT SELECTIONNE ET LE NIVEAU ACTUELLEMENT DESSINE A L'ECRAN
	//SORTIE SELON OU L'UTILISATEUR CLIC (TOUS LES EVENTS SONT GERES AVEC UN CLIC DE SOURIS SI POSSIBLE) ET DANS QUEL ETAT EST LA FENETRE (MAINMENU,1 OU PRINCIPAL 2)
	//CELA EFFECTUE LE CODE LIE A L'ENDROIT OU L'UTILISATEUR A CLIQUE CF PLUS BAS

	int mousex, mousey;  //COORDONEES DE LA SOURIS RELATIVE A LA FENETRE
	coordonnees Tile;    // COORDONEES DE LA TILE SELECTIONNEES PAR L'UTILISATEUR
	SDL_Event event;     // L'EVENEMENT A GERER
	int i = 0;           // ENTIER POUR GERER LES BOUCLES ITERATIVES 

	if (SDL_PollEvent(&event)) {
		switch (event.type) {
		case SDL_QUIT:
			state->g_bRunning = 0; break;
		case SDL_KEYDOWN:
			switch (event.key.keysym.sym)
			{
			case SDLK_LEFT:; break;
			case SDLK_RIGHT:; break;
			case SDLK_UP:; break;
			case SDLK_DOWN:; break;
			//ON QUITTE LE PROGRAMME
			case SDLK_ESCAPE:state->g_bRunning = 0; break;
				break;
			}
		case SDL_KEYUP:; break;
		case SDL_MOUSEBUTTONDOWN:SDL_GetMouseState(&mousex, &mousey);
			//ON CLIQUE SUR LE BOUTTON PLAY 
			if ( ( ( (mousex > SCREEN_WIDTH / 2 - 200) && (mousex < SCREEN_WIDTH / 2 + 200) ) && ( (mousey > 400) && (mousey < 500 ) ) ) && (state->screen == 1) )
			{
				//ON PASSE AU MENU SUIVANT ET ON ACTUALISE L'ETAT DE L'ECRAN
				SDL_RenderClear(myGame->g_pRenderer);
				state->screen = 2;
				cutTexture(myGame, *state, table);
			}
			//ON CLIQUE SUR LE BOUTTON EXIT
			else if ((((mousex > SCREEN_WIDTH / 2 - 200) && (mousex < SCREEN_WIDTH / 2 + 200)) && ((mousey > SCREEN_HEIGHT - 500) && (mousey < SCREEN_HEIGHT - 400))) && (state->screen == 1)) {
				
				//ON QUITTE LE PROGRAMME
				state->g_bRunning = 0;
			}else
			//L'UTILISATEUR SELECTIONNE UNE TILE DU TILESET
			if ((((mousex > SCREEN_WIDTH - 100) && (mousex < SCREEN_WIDTH -100 + LONGUEUR_TILE)) && ((mousey > 40) && (mousey < NOMBRE_TILES * HAUTEUR_TILE + 40 + NOMBRE_TILES * 2))) && (state->screen == 2))
			{
				//ON REGARDE SUR QUEL TILE IL A CLIQUE
				while (i < (NOMBRE_TILES * NOMBRE_LIGNES)) {
					if ((mousey > 40) && (mousey < (40+HAUTEUR_TILE) +  i * HAUTEUR_TILE + i*2)) {
						//ON AFFECETE LE NUMERO DE LA TILE A CETTE VALEUR
						*tilenumber = i;
						break;
					};
					i++;
					
				}
			}
			//L'UTILISATEUR REMPLACE UNE TILE DU NIVEAU PAR UNE AUTRE
			else if ((mousex > ((SCREEN_WIDTH / 2) - (LONGUEUR_TILE*NOMBRE_BLOCS_LARGEUR / 2))) && (mousex < ((SCREEN_WIDTH / 2) - (LONGUEUR_TILE*NOMBRE_BLOCS_LARGEUR / 2)) + (LONGUEUR_TILE * NOMBRE_BLOCS_LARGEUR)) && 
				    ((mousey > ((SCREEN_HEIGHT / 2) - (HAUTEUR_TILE*NOMBRE_BLOCS_LONGUEUR / 2))) && (mousey < ((SCREEN_HEIGHT / 2) - (HAUTEUR_TILE*NOMBRE_BLOCS_LONGUEUR / 2)) + (NOMBRE_BLOCS_LONGUEUR * HAUTEUR_TILE ))) && 
					(state->screen == 2))
			{
				//ON REGARDE OU L'UTILISATEUR A CLIQUE ET ON REMPLACE LA TILE SELECTIONNE PAR LA NOUVELLE TILE
				Tile.x = (mousex - ((SCREEN_WIDTH / 2) - (LONGUEUR_TILE*NOMBRE_BLOCS_LARGEUR / 2))) / LONGUEUR_TILE;
				Tile.y = (mousey - ((SCREEN_HEIGHT / 2) - (HAUTEUR_TILE*NOMBRE_BLOCS_LONGUEUR / 2))) / HAUTEUR_TILE;
				table[Tile.y][Tile.x] = *tilenumber;
				cutTexture(myGame, *state, table);

			}//L'UTILISATEUR CLIQUE SUR LE BOUTON IMPORTER
			else if (((mousex > 50) && (mousex < 100)) && ((mousey >50) && (mousey < 100)) && (state->screen == 2))
			{
				
			}//L'UTILISATEUR CLIQUE SUR LE BOUTTON SAVE
			else if (((mousex > 100) && (mousex < 150)) && ((mousey > 50) && (mousey < 100)) && (state->screen == 2))
			{
				create_map_file(table);

			}
			; break;

		default:break;

		}
	}


}



void cutTexture(game *myGame, gameState state, char table[NOMBRE_BLOCS_LONGUEUR][NOMBRE_BLOCS_LARGEUR]) {

	//BUT    DESSINER LES DIFFERENTS ELEMENTS DU PROGRAMME 
	//ENTREE LA FENETRE, SON ETAT ACTUEL, ET LE NIVEAU ACTUEL
	//SORTIE LES ELEMENTS DESSSINEES EN FONCTIONS DE L'ETAT DU PROGRAMME
	SDL_Rect rectangleDest;
	SDL_Rect rectangleSource;

	int i, j;

	if (state.screen == 1)
	{

		//CREATION DU BOUTTON PLAY

		//CHARGEMENT DE L'IMAGE
		myGame->g_surface = IMG_Load("./assets/button.png");//Chargement de l'image png

		//SI L'IMAGE N'EST PAS CHARGE
		if (!myGame->g_surface) {
			fprintf(stdout, "IMG_Load: %s\n", IMG_GetError());
			// handle error
		}
		
		//SI L'IMAGE EST CHARGE
		if (myGame->g_surface)
		{

			myGame->g_texture = SDL_CreateTextureFromSurface(myGame->g_pRenderer, myGame->g_surface); // Préparation du sprite
			SDL_FreeSurface(myGame->g_surface); // Libération de la ressource occupée par le sprite

			if (myGame->g_texture)
			{		
				rectangleSource.x = 0;
				rectangleSource.y = 0;
				rectangleSource.w = 400;
				rectangleSource.h = 100;

				rectangleDest.x = SCREEN_WIDTH/2-200;  //debut x
				rectangleDest.y = 400;  //debut y
				rectangleDest.w = rectangleSource.w; //Largeur
				rectangleDest.h = rectangleSource.h; //Hauteur
				
				SDL_RenderCopy(myGame->g_pRenderer, myGame->g_texture, &rectangleSource, &rectangleDest);
				
				SDL_RenderPresent(myGame->g_pRenderer); // Affichage

			}
			else {
				fprintf(stdout, "Échec de création de la texture (%s)\n", SDL_GetError());
			}

		}else {
			fprintf(stdout, "Échec de chargement du sprite (%s)\n", SDL_GetError());


		}

		//CREATION DU BOUTTON EXIT

		//CHARGEMENT DE L'IMAGE
		myGame->g_surface = IMG_Load("./assets/exit.png");//Chargement de l'image png

		//SI L'IMAGE N'EST PAS CHARGE
		if (!myGame->g_surface) {
			fprintf(stdout, "IMG_Load: %s\n", IMG_GetError());
			// handle error
		}

		//SI L'IMAGE EST CHARGE
		if (myGame->g_surface)
		{

			myGame->g_texture = SDL_CreateTextureFromSurface(myGame->g_pRenderer, myGame->g_surface); // Préparation du sprite
			SDL_FreeSurface(myGame->g_surface); // Libération de la ressource occupée par le sprite

			if (myGame->g_texture)
			{
				rectangleSource.x = 0;
				rectangleSource.y = 0;
				rectangleSource.w = 400;
				rectangleSource.h = 100;

				rectangleDest.x = SCREEN_WIDTH / 2 - 200;  //debut x
				rectangleDest.y = SCREEN_HEIGHT - 500;  //debut y
				rectangleDest.w = rectangleSource.w; //Largeur
				rectangleDest.h = rectangleSource.h; //Hauteur

				SDL_RenderCopy(myGame->g_pRenderer, myGame->g_texture, &rectangleSource, &rectangleDest);

				SDL_RenderPresent(myGame->g_pRenderer); // Affichage

			}
			else {
				fprintf(stdout, "Échec de création de la texture (%s)\n", SDL_GetError());
			}

		}
		else {
			fprintf(stdout, "Échec de chargement du sprite (%s)\n", SDL_GetError());


		}

	}
	else if (state.screen == 2)
	{
		//GESTION DE LA COULEUR DE FOND
		SDL_SetRenderDrawColor(myGame->g_pRenderer, 100, 100, 100, 255);
		SDL_RenderClear(myGame->g_pRenderer);
		SDL_RenderPresent(myGame->g_pRenderer);

		//drawGrid(myGame);

		//DESSIN DU NIVEAU
		myGame->g_surface = SDL_LoadBMP("tileset1.bmp");

		if (myGame->g_surface) {


			myGame->g_texture = SDL_CreateTextureFromSurface(myGame->g_pRenderer, myGame->g_surface); // Préparation du sprite
			SDL_FreeSurface(myGame->g_surface); // Libération de la ressource occupée par le sprite

			for (i = 0; i < NOMBRE_BLOCS_LARGEUR; i++)
			{
				for (j = 0; j < NOMBRE_BLOCS_LONGUEUR; j++)
				{

					if (myGame->g_texture) {

						/*
						rectangleSource.x = 0;
						rectangleSource.y = 0;
						rectangleSource.w = NOMBRE_BLOCS_LARGEUR * LARGEUR_TILE;
						rectangleSource.h = HAUTEUR_TILE;

						rectangleDest.x = 0;  //debut x
						rectangleDest.y = 0;  //debut y
						rectangleDest.w = rectangleSource.w; //Largeur
						rectangleDest.h = rectangleSource.h; //Hauteur
						*/

						rectangleSource.x = (table[j][i])*LONGUEUR_TILE;
						rectangleSource.y = 0;
						rectangleSource.w = LONGUEUR_TILE;
						rectangleSource.h = HAUTEUR_TILE;

						rectangleDest.x = ((SCREEN_WIDTH/2) - (LONGUEUR_TILE*NOMBRE_BLOCS_LARGEUR/2)) + i * LONGUEUR_TILE;
						rectangleDest.y = ((SCREEN_HEIGHT / 2) - (HAUTEUR_TILE*NOMBRE_BLOCS_LONGUEUR / 2)) + j * HAUTEUR_TILE;
						rectangleDest.w = rectangleSource.w; //Largeur
						rectangleDest.h = rectangleSource.h; //Hauteur




						/*
							//1s = 1000ms
							rectangleSource.x = 128 * (int)((SDL_GetTicks() / 100) % 6);//1 image tous les 100ms
							rectangleSource.y = 0;: Violation d'accès lors de la lecture de l'emplacement
							rectangleSource.w = 128;//1 image = w:128 et h:82
							rectangleSource.h = 82;



							//Définition du rectangle dest pour dessiner le png
							rectangleDest.x = 0;//debut x
							rectangleDest.y = 0;//debut y
							rectangleDest.w = rectangleSource.w; //Largeur
							rectangleDest.h = rectangleSource.h; //Hauteur
						*/


						//SDL_RenderCopy(myGame->g_pRenderer, myGame->g_texture, &rectangleSource, &rectangleDest);

						/*
						if(state.right)
						SDL_RenderCopy(myGame->g_pRenderer,myGame->g_texture,&rectangleSource,&rectangleDest); // Copie du sprite grâce au SDL_Renderer
						if(state.left)
						SDL_RenderCopyEx(myGame->g_pRenderer,myGame->g_texture,&rectangleSource,&rectangleDest,0,0,SDL_FLIP_HORIZONTAL);
						*/

						SDL_RenderCopy(myGame->g_pRenderer, myGame->g_texture, &rectangleSource, &rectangleDest);

						SDL_RenderPresent(myGame->g_pRenderer); // Affichage

						//SDL_RenderClear(myGame->g_pRenderer);

					}

					else {
						fprintf(stdout, "Échec de création de la texture (%s)\n", SDL_GetError());
					}
				}
			}

		

			//CREATION DU TILESET DE L'UTILISATEUR A L'ECRAN
			for (i = 0; i < NOMBRE_LIGNES; i++)
			{
				for (j = 0; j < NOMBRE_TILES; j++)
				{
					rectangleSource.x = LONGUEUR_TILE * j;
					rectangleSource.y = 0;
					rectangleSource.w = LONGUEUR_TILE;
					rectangleSource.h = HAUTEUR_TILE;

					rectangleDest.x = SCREEN_WIDTH - 100;
					rectangleDest.y = j * HAUTEUR_TILE + 40 + j*2;
					rectangleDest.w = rectangleSource.w; //Largeur
					rectangleDest.h = rectangleSource.h; //Hauteur

					SDL_RenderCopy(myGame->g_pRenderer, myGame->g_texture, &rectangleSource, &rectangleDest);

					SDL_RenderPresent(myGame->g_pRenderer); // Affichage
				}
			}

			//CREATION DU BOUTTON IMPORTER

			//CHARGEMENT DE L'IMAGE
			myGame->g_surface = IMG_Load("./assets/Import.png");//Chargement de l'image png

			//SI L'IMAGE N'EST PAS CHARGE
			if (!myGame->g_surface) {
				fprintf(stdout, "IMG_Load: %s\n", IMG_GetError());
				// handle error
			}

			//SI L'IMAGE EST CHARGE
			if (myGame->g_surface)
			{

				myGame->g_texture = SDL_CreateTextureFromSurface(myGame->g_pRenderer, myGame->g_surface); // Préparation du sprite
				SDL_FreeSurface(myGame->g_surface); // Libération de la ressource occupée par le sprite

				if (myGame->g_texture)
				{
					rectangleSource.x = 0;
					rectangleSource.y = 0;
					rectangleSource.w = 50;
					rectangleSource.h = 50;

					rectangleDest.x = 50;  //debut x
					rectangleDest.y = 50;  //debut y
					rectangleDest.w = rectangleSource.w; //Largeur
					rectangleDest.h = rectangleSource.h; //Hauteur

					SDL_RenderCopy(myGame->g_pRenderer, myGame->g_texture, &rectangleSource, &rectangleDest);

					SDL_RenderPresent(myGame->g_pRenderer); // Affichage

				}
				else {
					fprintf(stdout, "Échec de création de la texture (%s)\n", SDL_GetError());
				}

			}
			else {
				fprintf(stdout, "Échec de chargement du sprite (%s)\n", SDL_GetError());


			}

			//CREATION DU BOUTTON SAVE

			//CHARGEMENT DE L'IMAGE
			myGame->g_surface = IMG_Load("./assets/Save.png");//Chargement de l'image png

			//SI L'IMAGE N'EST PAS CHARGE
			if (!myGame->g_surface) {
				fprintf(stdout, "IMG_Load: %s\n", IMG_GetError());
				// handle error
			}

			//SI L'IMAGE EST CHARGE
			if (myGame->g_surface)
			{

				myGame->g_texture = SDL_CreateTextureFromSurface(myGame->g_pRenderer, myGame->g_surface); // Préparation du sprite
				SDL_FreeSurface(myGame->g_surface); // Libération de la ressource occupée par le sprite

				if (myGame->g_texture)
				{
					rectangleSource.x = 0;
					rectangleSource.y = 0;
					rectangleSource.w = 50;
					rectangleSource.h = 50;

					rectangleDest.x = 100;  //debut x
					rectangleDest.y = 50;  //debut y
					rectangleDest.w = rectangleSource.w; //Largeur
					rectangleDest.h = rectangleSource.h; //Hauteur

					SDL_RenderCopy(myGame->g_pRenderer, myGame->g_texture, &rectangleSource, &rectangleDest);

					SDL_RenderPresent(myGame->g_pRenderer); // Affichage

				}
				else {
					fprintf(stdout, "Échec de création de la texture (%s)\n", SDL_GetError());
				}

			}
			else {
				fprintf(stdout, "Échec de chargement du sprite (%s)\n", SDL_GetError());


			}


			//dessineGrid(myGame);
		}
		else {
			fprintf(stdout, "Échec de chargement du sprite (%s)\n", SDL_GetError());


		}

		//SDL_RenderClear(myGame->g_pRenderer);

		destroyTexture(myGame);
	}

}

void delay(unsigned int frameLimit) {
	// Gestion des 60 fps (images/seconde)
	unsigned int ticks = SDL_GetTicks();

	if (frameLimit < ticks)
	{
		return;
	}

	if (frameLimit > ticks + 16)
	{
		SDL_Delay(16);
	}

	else
	{
		SDL_Delay(frameLimit - ticks);
	}
}


void destroy(game *myGame) {

	//Destroy render
	if (myGame->g_pRenderer != NULL)
		SDL_DestroyRenderer(myGame->g_pRenderer);


	//Destroy window
	if (myGame->g_pWindow != NULL)
		SDL_DestroyWindow(myGame->g_pWindow);

}

void destroyTexture(game *myGame) {

	//Destroy texture
	if (myGame->g_texture != NULL)
		SDL_DestroyTexture(myGame->g_texture);


}


void drawGrid(game *myGame) {

	int i, j;

	//DESSIN DE LA GRILLE DE L'EDITEUR
	for (i = 0; i < NOMBRE_BLOCS_LARGEUR; i++)
	{
		for (j = 0; j < NOMBRE_BLOCS_LONGUEUR; j++)
		{

			SDL_SetRenderDrawColor(myGame->g_pRenderer, 255, 255, 255, 255);

			SDL_RenderDrawLine(myGame->g_pRenderer, ((SCREEN_WIDTH/2)-(LONGUEUR_TILE*NOMBRE_BLOCS_LARGEUR)/2) + LONGUEUR_TILE*i,
			((SCREEN_HEIGHT / 2) - (HAUTEUR_TILE*NOMBRE_BLOCS_LONGUEUR / 2)),
			(SCREEN_WIDTH / 2) - ((LONGUEUR_TILE * NOMBRE_BLOCS_LARGEUR)/2) + LONGUEUR_TILE * i, 
			((SCREEN_HEIGHT / 2) - (HAUTEUR_TILE*NOMBRE_BLOCS_LONGUEUR / 2)) + HAUTEUR_TILE*NOMBRE_BLOCS_LONGUEUR);


			SDL_RenderDrawLine(myGame->g_pRenderer, ((SCREEN_WIDTH / 2) - (LONGUEUR_TILE*NOMBRE_BLOCS_LARGEUR) / 2),
			((SCREEN_HEIGHT / 2) - (HAUTEUR_TILE*NOMBRE_BLOCS_LONGUEUR / 2)) + HAUTEUR_TILE*j,
			((SCREEN_WIDTH / 2) - (LONGUEUR_TILE*NOMBRE_BLOCS_LARGEUR) / 2) + 15 * 24,
			((SCREEN_HEIGHT / 2) - (HAUTEUR_TILE*NOMBRE_BLOCS_LONGUEUR / 2)) + HAUTEUR_TILE*j);

		}
	}
}

void drawTile(int *tilenumber, game *myGame) {

	SDL_Rect rectangleDest;
	SDL_Rect rectangleSource;
	int mousex, mousey;

	myGame->g_surface = SDL_LoadBMP("tileset1.bmp");
		//SI L'IMAGE N'EST PAS CHARGE
	if (!myGame->g_surface) {
		fprintf(stdout, "IMG_Load: %s\n", IMG_GetError());
		// handle error
	}

	//SI L'IMAGE EST CHARGE
	if (myGame->g_surface)
	{

		myGame->g_texture = SDL_CreateTextureFromSurface(myGame->g_pRenderer, myGame->g_surface); // Préparation du sprite
		SDL_FreeSurface(myGame->g_surface); // Libération de la ressource occupée par le sprite

		if (myGame->g_texture)
		{
			SDL_GetMouseState(&mousex, &mousey);

			rectangleSource.x = *tilenumber*LONGUEUR_TILE;
			rectangleSource.y = 0;
			rectangleSource.w = LONGUEUR_TILE;
			rectangleSource.h = HAUTEUR_TILE;

			rectangleDest.x = mousex;
			rectangleDest.y = mousey;
			rectangleDest.w = rectangleSource.w; //Largeur
			rectangleDest.h = rectangleSource.h; //Hauteur

			SDL_RenderCopy(myGame->g_pRenderer, myGame->g_texture, &rectangleSource, &rectangleDest);

			SDL_RenderPresent(myGame->g_pRenderer); // Affichage


		}

	}
}

bool XMLTest(const char* testString, const char* expected, const char* found, bool echo = true, bool extraNL = false)
{
	bool pass;
	if (!expected && !found)
		pass = true;
	else if (!expected || !found)
		pass = false;
	else
		pass = !strcmp(expected, found);
	if (pass)
		printf("[pass]");
	else
		printf("[fail]");

	if (!echo) {
		printf(" %s\n", testString);
	}
	else {
		if (extraNL) {
			printf(" %s\n", testString);
			printf("%s\n", expected);
			printf("%s\n", found);
		}
		else {
			printf(" %s [%s][%s]\n", testString, expected, found);
		}
	}

	if (pass)
		++gPass;
	else
		++gFail;
	return pass;
}

bool XMLTest(const char* testString, XMLError expected, XMLError found, bool echo = true, bool extraNL = false)
{
	return XMLTest(testString, XMLDocument::ErrorIDToName(expected), XMLDocument::ErrorIDToName(found), echo, extraNL);
}

bool XMLTest(const char* testString, bool expected, bool found, bool echo = true, bool extraNL = false)
{
	return XMLTest(testString, expected ? "true" : "false", found ? "true" : "false", echo, extraNL);
}

template< class T > bool XMLTest(const char* testString, T expected, T found, bool echo = true)
{
	bool pass = (expected == found);
	if (pass)
		printf("[pass]");
	else
		printf("[fail]");

	if (!echo)
		printf(" %s\n", testString);
	else {
		char expectedAsString[64];
		XMLUtil::ToStr(expected, expectedAsString, sizeof(expectedAsString));

		char foundAsString[64];
		XMLUtil::ToStr(found, foundAsString, sizeof(foundAsString));

		printf(" %s [%s][%s]\n", testString, expectedAsString, foundAsString);
	}

	if (pass)
		++gPass;
	else
		++gFail;
	return pass;
}

void NullLineEndings(char* p)
{
	while (p && *p) {
		if (*p == '\n' || *p == '\r') {
			*p = 0;
			return;
		}
		++p;
	}
}

void create_map_file(char table[NOMBRE_BLOCS_LONGUEUR][NOMBRE_BLOCS_LARGEUR])
{

	static char tabmap[NOMBRE_BLOCS_LARGEUR*NOMBRE_BLOCS_LONGUEUR];
	int i, j,k;

	k = 0; 
	for (i = 0; i < NOMBRE_BLOCS_LARGEUR; i++)
	{
		for (j = 0; j < NOMBRE_BLOCS_LONGUEUR; j++)
		{
			
			snprintf(&tabmap[k], sizeof(tabmap), "%d", table[i][j]);
			k++;
		}
	}
	

	XMLDocument doc = new XMLDocument;
	
	XMLDeclaration * decl = doc.NewDeclaration("id=\"Le numero ou le nom de la map\"");

	doc.LinkEndChild(decl);

	XMLElement * element = doc.NewElement("Tableau");

	XMLText  *text = doc.NewText(tabmap);

	element->LinkEndChild(text);

	doc.LinkEndChild(element);

	doc.SaveFile("MapTest.xml");

	
}

//FONCTION BUGUER  
int create_map_file_V2(char tabmap[NOMBRE_BLOCS_LARGEUR*NOMBRE_BLOCS_LONGUEUR], int nummap)
//BUT : créer un nouveau fichier XML contenant une map réalisée par l'utilisateur
//ENTREE : les données de la map et un compteur
//SORTIE : un fichier XML contenant les informations nécessaires
{

	//déclaration des variables nécessaires à la création d'un nom de document
	char *nomdoc = NULL;
	int longtab = 0;
	string numenstr = "";
	numenstr = to_string(nummap);
	int longnum;
	longnum = (int)numenstr.length();
	string Partie1 = "CustomMap";
	int longP1 = (int)Partie1.length();
	string Partie2 = ".xml";
	int longP2 = (int)Partie2.length();
	numenstr[longnum] = '\0';
	Partie2[longP2] = '\0';
	//on définit la taille qu'aura le nom du document
	longtab = 10 + longnum + 4;
	nomdoc = new char[longtab];

	//préparation du nouveau document XML
	XMLDocument doc = new XMLDocument;
	char *contenudecla = NULL;

	//déclaration respectant plus ou moins la norme
	XMLDeclaration * decl = doc.NewDeclaration("xml version=\"1.0\" encoding=\"UTF - 8\"");
	doc.LinkEndChild(decl);

	//déclaration d'un nouvel élément
	XMLElement * element = doc.NewElement("Tableau");
	//et du contenu de la map qui y sera associé
	XMLText  *text = doc.NewText(tabmap);
	element->LinkEndChild(text);
	doc.LinkEndChild(element);

	//on ajoute les différentes parties du nom du document au tableau
	for (int i = 0; i < longP1; i++)
	{
		nomdoc[i] = Partie1[i];
	}
	for (int i = longP1; i < longnum; i++)
	{
		nomdoc[i] = numenstr[i - longP1];
	}
	for (int i = (longP1 + longnum); i < longtab; i++)
	{
		nomdoc[i] = Partie2[i - (longP1 + longnum)];
	}

	//sauvegarde du document
	doc.SaveFile(nomdoc);
	//on prend en compte qu'une nouvelle map a été créée
	nummap++;

	//on libère la mémoire prise par les tableaux dynamiques
	delete[] nomdoc;
	nomdoc = NULL;

	//possible de se passer de ça si on utilise un pointeur
	return nummap;

}