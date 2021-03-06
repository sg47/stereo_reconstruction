#include <GL/glew.h>
#include <SDL2/SDL_opengl.h>
#include "../header/meshLoader.h"
#include "../header/shader.h"
#include "../header/matrices.h"
#include <fstream>
#include <iostream>
#include <bitset>
using namespace std;

float SCREEN_WIDTH = 200;
float SCREEN_HEIGHT = 200;
SDL_Window* gWindow;
SDL_Surface* gScreenSurface;
matrices pipeline;
glm::mat4 modelMatrix;
glm::mat4 viewMatrix;
glm::mat4 projectionMatrix;
meshLoader* scene;
shader* shades;
shader* texturingShades;
shader* texturingShades2;
shader* harrisShades;
shader* cornerShades;
unsigned int FBO;		//for detecting edges of gray_scale image using two program objects
unsigned int FBO2;		//for gaussian blur
unsigned int FBO3;		//for local non-maximum suppression
unsigned int textureMap;	//of FBO, shows sharp edges
unsigned int blurredImage;	//blurred image of textureMap
unsigned int cornerImage;
unsigned int img;
mesh* quad;

unsigned int loadTexture(const char* filename)
{
	unsigned int num;
	glGenTextures(1,&num);
	std::string file_name = "../models/textures/";
	file_name += filename;
	SDL_Surface* imgs=IMG_Load(file_name.c_str());
	if(imgs==NULL)
	{
		//std::cout << "img was not loaded" << std::endl;
		return -1;
	}
	//SDL_PixelFormat form={NULL,32,4,0,0,0,0,0,0,0,0,0xff000000,0x00ff0000,0x0000ff00,0x000000ff,0,255};
	SDL_Surface* img2=SDL_ConvertSurface( imgs, gScreenSurface->format,0 );
	if(img2==NULL)
	{
		//std::cout << "img2 was not loaded" << std::endl;
		return -1;		
	}
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D,num);		
	
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR); 
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR); 
	glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,img2->w,img2->h,0,GL_BGRA,GL_UNSIGNED_BYTE,img2->pixels);		
	//glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,img->w,img->h,0,GL_BGRA,GL_UNSIGNED_BYTE,img->pixels);		
	SDL_FreeSurface(imgs);
	SDL_FreeSurface(img2);
	return num;	
}

unsigned int createTexture(int w,int h,bool isDepth=false)
{
	unsigned int textureId;
	glEnable(GL_TEXTURE_2D);
	glGenTextures(1,&textureId);
	glBindTexture(GL_TEXTURE_2D,textureId);
	glTexImage2D(GL_TEXTURE_2D,0,(!isDepth ? GL_RGBA8 : GL_DEPTH_COMPONENT),w,h,0,isDepth ? GL_DEPTH_COMPONENT : GL_RGBA,GL_FLOAT,NULL);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP_TO_BORDER);
	
	int i;
	i=glGetError();
	if(i!=0)
	{
		std::cout << "Error happened while loading the texture: " << gluErrorString(i) << std::endl;
	}
	glBindTexture(GL_TEXTURE_2D,0);
	return textureId;
}


void init()
{
	pipeline.matrixMode(PROJECTION_MATRIX);
	pipeline.loadIdentity();
	//pipeline.perspective(50,SCREEN_WIDTH/SCREEN_HEIGHT, 1, 1000);
	pipeline.ortho(-1.0, 1.0, -1.0, 1.0, 1, 100);
	glEnable(GL_DEPTH_TEST);
	shades = new shader("../v_shader/simpleShader.vs","../f_shader/simpleShader.frag");
	texturingShades = new shader("../v_shader/textureShader.vs","../f_shader/textureShader.frag");
	texturingShades2 = new shader("../v_shader/textureShader2.vs","../f_shader/textureShader2.frag");
	harrisShades = new shader("../v_shader/harrisShader.vs","../f_shader/harrisShader.frag");
	cornerShades = new shader("../v_shader/cornerShader.vs","../f_shader/cornerShader.frag");

	scene = new meshLoader();
	textureMap = createTexture(SCREEN_WIDTH, SCREEN_HEIGHT);
	blurredImage = createTexture(SCREEN_WIDTH, SCREEN_HEIGHT);
	cornerImage = createTexture(SCREEN_WIDTH, SCREEN_HEIGHT);
	img = loadTexture("baby.jpg");

	glGenFramebuffers(1, &FBO);
	glEnable(GL_TEXTURE_2D);
	glBindFramebuffer(GL_FRAMEBUFFER,FBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER,GL_COLOR_ATTACHMENT0,GL_TEXTURE_2D,textureMap,0);

	int i=glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if(i!=GL_FRAMEBUFFER_COMPLETE)
	{
		std::cout << "Framebuffer is not OK, status=" << i << std::endl;
	}
	glBindFramebuffer(GL_FRAMEBUFFER,0);


	//next framebuffer
	glGenFramebuffers(1, &FBO2);
	glEnable(GL_TEXTURE_2D);
	glBindFramebuffer(GL_FRAMEBUFFER,FBO2);
	glFramebufferTexture2D(GL_FRAMEBUFFER,GL_COLOR_ATTACHMENT0,GL_TEXTURE_2D,blurredImage,0);

	i=glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if(i!=GL_FRAMEBUFFER_COMPLETE)
	{
		std::cout << "Framebuffer is not OK, status=" << i << std::endl;
	}
	glBindFramebuffer(GL_FRAMEBUFFER,0);

	//next framebuffer
	glGenFramebuffers(1, &FBO3);
	glEnable(GL_TEXTURE_2D);
	glBindFramebuffer(GL_FRAMEBUFFER,FBO3);
	glFramebufferTexture2D(GL_FRAMEBUFFER,GL_COLOR_ATTACHMENT0,GL_TEXTURE_2D,cornerImage,0);

	i=glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if(i!=GL_FRAMEBUFFER_COMPLETE)
	{
		std::cout << "Framebuffer is not OK, status=" << i << std::endl;
	}
	glBindFramebuffer(GL_FRAMEBUFFER,0);

	//create the quad here
	{
		std::vector<unsigned int> indices;
		std::vector<vertexData> vertices;
		vertexData tmp;
		//1.
		tmp.position.change(-1.0,1.0,-1.1);
		tmp.U=0;
		tmp.V=1;
		vertices.push_back(tmp);
		//2.
		tmp.position.change(-1.0,-1.0,-1.1);
		tmp.U=0;
		tmp.V=0;
		vertices.push_back(tmp);
		//3.
		tmp.position.change(1.0,-1.0,-1.1);
		tmp.U=1;
		tmp.V=0;
		vertices.push_back(tmp);
		//4.
		tmp.position.change(1.0,1.0,-1.1);
		tmp.U=1;
		tmp.V=1;
		vertices.push_back(tmp);
		
		indices.push_back(0);
		indices.push_back(1);
		indices.push_back(2);		
		
		indices.push_back(0);
		indices.push_back(2);
		indices.push_back(3);
		quad=new mesh(&vertices,&indices);
	}
}

	float pixies[200*200 * 3];
	std::bitset<120000>apple;
	unsigned char ball_buf[25 * 200*3];

void display()
{
		//pipeline.matrixMode(PROJECTION_MATRIX);
		//pipeline.loadIdentity();
		//pipeline.perspective(50,SCREEN_HEIGHT/SCREEN_HEIGHT, near, far);
		//pipeline.ortho (-100.0, 100.0, -100, 100.0, 1.0, -1.0);

	glEnable(GL_DEPTH_TEST);			//render original image "decalred in mesh.cpp" and obtain derv. image
	glClearColor(1, 0, 0.0, 1);
	pipeline.matrixMode(MODEL_MATRIX);
	//glBindFramebuffer(GL_FRAMEBUFFER,FBO);
		shades->useShader();
		glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
		pipeline.updateMatrices(shades->getProgramId());
		scene->draw(shades->getProgramId());
		shades->delShader();
	//glBindFramebuffer(GL_FRAMEBUFFER,0);


	glEnable(GL_DEPTH_TEST);			//render original image "decalred in mesh.cpp" and obtain derv. image
	glClearColor(1, 0, 0.0, 1);
	pipeline.matrixMode(MODEL_MATRIX);
	glBindFramebuffer(GL_FRAMEBUFFER,FBO);
		shades->useShader();
		glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
		pipeline.updateMatrices(shades->getProgramId());
		scene->draw(shades->getProgramId());
		shades->delShader();
	glBindFramebuffer(GL_FRAMEBUFFER,0);

	glBindFramebuffer(GL_FRAMEBUFFER, FBO);
	glReadBuffer(GL_COLOR_ATTACHMENT0);
	glReadPixels(0, 0, 200, 200, GL_RGB, GL_FLOAT, &pixies[0]);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

 std::ofstream outfile("../models/flagofnorway.bin", ios::binary);
	for(int k = 0; k < 200*200*3; k++)
	 {
		//std::cout<<"  "<<pixies[k]<<"  "<<pixies[k+1]<<"  "<<pixies[k+2]<<std::endl;
		//outfile <<(int)(pixies[k]*1.5)<<(int)(pixies[k+1]*1.5)<<(int)(pixies[k+2]*1.5);
		if(pixies[k] < 0.5) apple[k] = 0;
		else apple[k] = 1;
	 }

	int m;
	unsigned char check;
	for(int k = 0; k < 200*200*3; k+=8)
	{
		check = 0x00;
		m = k/8;
		check |= apple[k];		//1
		check <<= 1;			//10
		check |= apple[k+1];		//11
		check <<= 1;			//110
		check |= apple[k+2];		//111
		check <<= 1;			//1110
		check |= apple[k+3];		//1111
		check <<= 1;			//11110
		check |= apple[k+4];		//11111
		check <<= 1;
		check |= apple[k+5];		//111111
		check <<= 1;
		check |= apple[k+6];
		check <<= 1;
		check |= apple[k+7];

		ball_buf[m] = check;
		outfile <<check;	
	}




/*
	glBindFramebuffer(GL_FRAMEBUFFER,FBO2);
	glClearColor(1.0, 0.0, 0.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	texturingShades->useShader();			//take derv image and output it's gaussian blur
	texturingShades2->useShader();
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D,textureMap);
		glUniform1i(glGetUniformLocation(texturingShades->getProgramId(),"textures"),0);
		glUniform1i(glGetUniformLocation(texturingShades2->getProgramId(),"textures"),0);
		pipeline.updateMatrices(texturingShades->getProgramId());
		pipeline.updateMatrices(texturingShades2->getProgramId());
		quad->draw(texturingShades->getProgramId());
		quad->draw(texturingShades2->getProgramId());	
	texturingShades->delShader();
	texturingShades2->delShader();
	glBindFramebuffer(GL_FRAMEBUFFER,0);

	glBindFramebuffer(GL_FRAMEBUFFER,FBO3);
	glClearColor(0.0, 1.0, 0.0, 1.0);
	pipeline.loadIdentity();
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	harrisShades->useShader();
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, blurredImage);
		glUniform1i(glGetUniformLocation(harrisShades->getProgramId(),"blurred_textures"),0);
		pipeline.updateMatrices(harrisShades->getProgramId());
		quad->draw(harrisShades->getProgramId());
	harrisShades->delShader();
	glBindFramebuffer(GL_FRAMEBUFFER,0);

	glClearColor(0.0, 0.0, 1.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	cornerShades->useShader();
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, cornerImage);
		glUniform1i(glGetUniformLocation(cornerShades->getProgramId(),"jagged_textures"),0);
		pipeline.updateMatrices(cornerShades->getProgramId());
		quad->draw(cornerShades->getProgramId());
	cornerShades->delShader();
*/
}


int main()
{

	SDL_Init(SDL_INIT_EVERYTHING);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
	gWindow = SDL_CreateWindow("SDL_COLLIDE", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL);
	SDL_GLContext gContext = SDL_GL_CreateContext(gWindow);
	glewExperimental = GL_TRUE;
	glewInit();
	SDL_GL_SetSwapInterval( 1 );
	gScreenSurface = SDL_GetWindowSurface( gWindow );
	bool running=true;
	SDL_Event event;	
	init();

	while(running)
	{
		while(SDL_PollEvent(&event))
		{
			switch(event.type)
			{
				case SDL_QUIT:
				running = false;
				break;
	
				case SDL_KEYDOWN:
				switch(event.key.keysym.sym)
					{
						case SDLK_ESCAPE:
							running=false;
							break;
						
					}
	
			}
		}
		//std::cout<<std::endl<<"left "<<left<<" right  "<<right<<" bottom  "<<bottom<<"  top  "<<top<<std::endl;
		//std::cout<<" near "<<near<<" and far  "<<far<<std::endl;
		display();
		SDL_GL_SwapWindow(gWindow);
	}


	delete shades;
	delete texturingShades;
	delete texturingShades2;
	delete harrisShades;
	delete cornerShades;
	delete scene;
	delete quad;
	SDL_FreeSurface(gScreenSurface);
	SDL_GL_DeleteContext(gContext);
	SDL_DestroyWindow(gWindow);
	SDL_Quit();
	return 0;
}
