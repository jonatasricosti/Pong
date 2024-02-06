#include <SDL.h>

// use essa fun��o pra carregar uma imagem.bmp e deixar o fundo transparente
SDL_Surface *fundo_transparente(const char *filename, Uint8 red, Uint8 green, Uint8 blue)
{
    SDL_Surface *load = NULL;
    SDL_Surface *otimizado = NULL;

    load = SDL_LoadBMP(filename);

    if(load != NULL)
    {
        otimizado = SDL_DisplayFormat(load);
        SDL_FreeSurface(load);

        if(otimizado != NULL)
        {
            SDL_SetColorKey(otimizado, SDL_SRCCOLORKEY, SDL_MapRGB(otimizado->format, red, green, blue));
        }
    }

    return otimizado;
}


SDL_Event evento;
SDL_Surface *tela = NULL;
bool executando = true;

const int screen_width = 640;
const int screen_height = 480;
const int screen_bpp = 32;


const int speed = 10;

SDL_Surface *player1Image = NULL;
SDL_Surface *player2Image = NULL;
SDL_Surface *ballImage = NULL;

void LoadFiles()
{
    player1Image = SDL_LoadBMP("gfx/player1.bmp");
    player2Image = SDL_LoadBMP("gfx/player2.bmp");
    ballImage = fundo_transparente("gfx/ball.bmp", 0,255,255);
}

void CloseFiles()
{
    SDL_FreeSurface(player1Image);
    SDL_FreeSurface(player2Image);
    SDL_FreeSurface(ballImage);
}

// use essa fun��o pra desenhar uma imagem na tela
void DrawImage(int x, int y, SDL_Surface *image)
{
    SDL_Rect mover;
    mover.x = x;
    mover.y = y;

    SDL_BlitSurface(image, NULL, tela, &mover);
}

// para o framerate
Uint32 start = 0;
const int fps = 30;
const int framerate =  1000/fps;

int Player1Pontos = 0;
int Player2Pontos = 0;


// use essa fun��o pra detectar colis�o entre dois ret�ngulos
int AABB(int x1, int y1, int width1, int height1, int x2, int y2, int width2, int height2)
{
    if(x1 < x2 + width2 &&
	x2 < x1+width1 &&
	y1 < y2+height2 &&
	y2 < y1+height1)
	{
		return 1;
	}

	return 0;
}

// essa estrutura representa um ret�ngulo
class rectangle
{
	public:
	int x;
	int y;
	int width;
	int height;
};

rectangle player1;
rectangle player2;

// essa estrutura representa uma bola
class _BALL
{
	public:
	int x;
	int y;
	int vx;
	int vy;
	int width;
	int height;
};


_BALL ball;

// inicia as propriedades dos ret�ngulos
void ResetGame()
{
	player1.width = 20;
	player1.height = 100;
	player1.x = player1.width;
	player1.y = (screen_height-player1.height)/2;

	player2.width = 20;
	player2.height = 100;
	player2.x = screen_width - 2*player2.width;
	player2.y = (screen_height-player2.height)/2;

	ball.width  = 20;
	ball.height = 20;
	ball.x = (screen_width-ball.width)/2;
	ball.y = (screen_height-ball.height)/2;

	ball.vx = speed;
	ball.vy = speed;
}


// use essa fun��o pra mover o player1
void MovePlayer1(int speed)
{
	Uint8 *tecla = SDL_GetKeyState(NULL);

	if(tecla[SDLK_DOWN])
	{
		player1.y = player1.y+speed;
	}

	if(tecla[SDLK_UP])
	{
		player1.y = player1.y-speed;
	}

	// colis�o lado de cima
	if(player1.y < 0)
	{
		player1.y = 0;
	}

	// colis�o lado de baixo
	if(player1.y > screen_height-player1.height)
	{
		player1.y = screen_height-player1.height;
	}
}

// use essa fun��o pra mover o player2 por intelig�ncia1 artificial
void MovePlayer2(int speed)
{
	int py = player2.y + player2.height/2;

	if(ball.vx > 0 && ball.x > 100)
	{
		if(py > ball.y)
		{
			// move pra cima
			player2.y = player2.y - speed;
		}


		if(py < ball.y)
		{
			// move pra baixo
			player2.y = player2.y + speed;
		}
	}

	// colis�o lado de cima
	if(player2.y < 0)
	{
		player2.y = 0;
	}

	// colis�o lado de baixo
	if(player2.y > screen_height-player2.height)
	{
		player2.y = screen_height-player2.height;
	}
}

// move a bola
void MoveBall()
{
	ball.x = ball.x + ball.vx;
	ball.y = ball.y + ball.vy;

	// tocou lado esquerdo da tela
	if(ball.x < 0)
	{
		// coloca a bola no centro da tela
		ball.x = (screen_width-ball.width)/2;
		ball.y = (screen_height-ball.height)/2;
		Player2Pontos = Player2Pontos+1; // aumenta 1 ponto
	}

	// tocou lado direito da tela
	if(ball.x > screen_width-ball.width)
	{
		// coloca a bola no centro da tela
		ball.x = (screen_width-ball.width)/2;
		ball.y = (screen_height-ball.height)/2;
		Player1Pontos = Player1Pontos+1; // aumenta 1 ponto
	}

	// tocou em cima da tela
	if(ball.y < 0)
	{
		ball.vy = -ball.vy;
	}

	// tocou em baixo da tela
	if(ball.y > screen_height-ball.height)
	{
		ball.vy = -ball.vy;
	}

	// colis�o com o player1
	if(AABB(player1.x,player1.y,player1.width,player1.height,ball.x,ball.y,ball.width,ball.height))
	{
		ball.x = ball.x+speed;
		ball.vx = -ball.vx;

	}

	// colis�o com o player2
	if(AABB(player2.x,player2.y,player2.width,player2.height,ball.x,ball.y,ball.width,ball.height))
	{
		ball.x = ball.x-speed;
		ball.vx = -ball.vx;
	}
}


void DrawScore()
{
	char m1[10];
	char m2[10];

	sprintf(m1,"%i",Player1Pontos);
	sprintf(m2,"%i",Player2Pontos);

	//DrawRect(96,46,3*8,8,RGB15bits(31,31,31));
	//DrawRect(136,46,3*8,8,RGB15bits(31,31,31));

	//DrawTextMode3(96,46,m1,0xff);
//	DrawTextMode3(136,46,m2,0xff);

	if(Player1Pontos >= 100)
	{
		Player1Pontos = 0;
		Player2Pontos = 0;
	}

	if(Player2Pontos >= 100)
	{
		Player1Pontos = 0;
		Player2Pontos = 0;
	}

}

int main(int argc, char*args[])
{
SDL_Init(SDL_INIT_EVERYTHING);
tela = SDL_SetVideoMode(screen_width,screen_height,screen_bpp,SDL_HWSURFACE | SDL_DOUBLEBUF);

LoadFiles();
ResetGame();

// game loop
while(executando)
{
    start = SDL_GetTicks();
    while(SDL_PollEvent(&evento))
    {
        // clicou no x da janela
        if(evento.type == SDL_QUIT)
        {
            executando = false; // fecha o programa
        }
    }

    SDL_FillRect(tela, 0, 0);

    MovePlayer1(10);
    MovePlayer2(10);
    MoveBall();

    DrawImage(player1.x, player1.y, player1Image);
    DrawImage(player2.x, player2.y, player2Image);
    DrawImage(ball.x, ball.y, ballImage);


    SDL_Flip(tela);
    if(framerate > (SDL_GetTicks()-start))
    {
        SDL_Delay(framerate - (SDL_GetTicks()-start));
    }
}


CloseFiles();
SDL_Quit();
return 0;
}