#include "../header/global.h"

std::map<int, int> global::locationMap;
std::map<int, int> global::locationMap_2;
std::vector<int> global::inliersRecord;
std::vector<vector3d> global::firstImagePoints;
std::vector<vector3d> global::secondImagePoints;
Matrix3f global::fundamentalMatrix;
vector3d global::epipoleA;
vector3d global::epipoleB;

LMatrix3f global::LfundamentalMatrix;
Lvector3d global::LepipoleA;
Lvector3d global::LepipoleB;

glm::mat4 global::firstHomographyMatrix;
glm::mat4 global::secondHomographyMatrix;

void global::initializeShader()
{
 mShader[DERIVATIVE_SHADER] = new shader("../v_shader/derivativeShader.vs","../f_shader/derivativeShader.frag");

 mShader[BLUR_SHADER1] = new shader("../v_shader/blurShader.vs","../f_shader/blurShader.frag");
 mShader[BLUR_SHADER2]	= new shader("../v_shader/blurShader2.vs","../f_shader/blurShader2.frag");		

 mShader[HARRIS_CORNER_SHADER] = new shader("../v_shader/harrisShader.vs","../f_shader/harrisShader.frag");

 mShader[SUPPRESS_SHADER] = new shader("../v_shader/suppressionShader.vs","../f_shader/suppressionShader.frag");

 mShader[GRAYSCALE_SHADER] = new shader("../v_shader/grayShader.vs","../f_shader/grayShader.frag");

 mShader[ORIENT_ASSIGN_SHADER] = new shader("../v_shader/orientShader.vs","../f_shader/orientShader.frag");

 mShader[RING_BLUR_SHADER] = new shader("../v_shader/ringBlurShader.vs","../f_shader/ringBlurShader.frag");

 mShader[RECTIFY_SHADER] = new shader("../v_shader/rectifyShader.vs","../f_shader/rectifyShader.frag");

 mShader[KERNEL_SHADER] = new shader("../v_shader/kernelShader.vs","../f_shader/kernelShader.frag");

 mShader[SIMPLE_SHADER] = new shader("../v_shader/simpleShader.vs","../f_shader/simpleShader.frag");

 mShader[TRUE_COLOR_SHADER] = new shader("../v_shader/trueShader.vs","../f_shader/trueShader.frag");

 mShader[SCENE_SHADER] = new shader("../v_shader/sceneShader.vs","../f_shader/sceneShader.frag");

 mShader[BIG_BLUR_SHADER1] = new shader("../v_shader/bigBlurShader1.vs","../f_shader/bigBlurShader1.frag");

 mShader[BIG_BLUR_SHADER2] = new shader("../v_shader/bigBlurShader2.vs","../f_shader/bigBlurShader2.frag");

 mShader[DESCRIPT_SHADER] = new shader("../v_shader/featureDescript.vs","../f_shader/featureDescript.frag");

 for(int i = 0; i < SHADER_COUNT_TOTAL; i++) isShaderSet[i] = 1;
}

void global::initializeTexture()
{
 for(int i = 0; i <= TRUE_COLOR_TEX; i++)
  mTexture[i] = createTexture(SCREEN_WIDTH, SCREEN_HEIGHT);

  mTexture[SCENE_TEX] = createTexture(SCREEN_WIDTH, SCREEN_HEIGHT, true);
  mTexture[BIG_BLUR_TEX1] = createTexture(SCREEN_WIDTH, SCREEN_HEIGHT, true);
  mTexture[BIG_BLUR_TEX2] = createTexture(SCREEN_WIDTH, SCREEN_HEIGHT, true);

 for(int i = DESCRIPT_TEX1A; i < TEX_COUNT_TOTAL; i++)
  mTexture[i] = createTexture(SCREEN_WIDTH, SCREEN_HEIGHT, true);


}

void global::initializeFBO()
{
 for(int i = 0; i <= ORIENT_ASSIGN_FBO; i++)
	createFrameBuffer(&mFBO[i], mTexture[i]);
 createFrameBuffer(&mFBO[RING_BLUR_FBO1], &(mTexture[RING_BLUR_TEX1A]));
 createFrameBuffer(&mFBO[RING_BLUR_FBO2], &(mTexture[RING_BLUR_TEX2A]));
 
 int i, j;
 for(i = RECTIFY_FBO1, j = RECTIFY_TEX1; i <= BIG_BLUR_FBO2, j <= BIG_BLUR_TEX2; i++, j++)
 createFrameBuffer(&mFBO[i], mTexture[j]);

	glGenFramebuffers(1, &mFBO[DESCRIPT_FBO1]);
	glEnable(GL_TEXTURE_2D);
	glBindFramebuffer(GL_FRAMEBUFFER, mFBO[DESCRIPT_FBO1]);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, mTexture[DESCRIPT_TEX1A], 0);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, mTexture[DESCRIPT_TEX1B], 0);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, mTexture[DESCRIPT_TEX1C], 0);
	GLenum bufs[3] = {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2};
	glDrawBuffers(3, bufs);

	i = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if(i!=GL_FRAMEBUFFER_COMPLETE)
		std::cout << "Framebuffer is not OK, status=" << i << std::endl;
	glBindFramebuffer(GL_FRAMEBUFFER,0);

	glGenFramebuffers(1, &mFBO[DESCRIPT_FBO2]);
	glEnable(GL_TEXTURE_2D);
	glBindFramebuffer(GL_FRAMEBUFFER, mFBO[DESCRIPT_FBO2]);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, mTexture[DESCRIPT_TEX2A], 0);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, mTexture[DESCRIPT_TEX2B], 0);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, mTexture[DESCRIPT_TEX2C], 0);
	GLenum bufsa[3] = {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2};
	glDrawBuffers(3, bufsa);

	i = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if(i!=GL_FRAMEBUFFER_COMPLETE)
		std::cout << "Framebuffer is not OK, status=" << i << std::endl;
	glBindFramebuffer(GL_FRAMEBUFFER,0);
}

void global::initialize()
{
   isShaderSet.reset();
   locationMap.clear(); 
   locationMap_2.clear();
   initializeShader();
   initializeTexture();
   initializeFBO();
  outputTex1 = &mTexture[SCENE_TEX];
  outputTex2 = &mTexture[TRUE_COLOR_TEX];
//  outputTex1 = &mTexture[SCENE_TEX];
}

unsigned int global::createTexture(int w,int h,bool isDepth)
{
	unsigned int textureId;
	glEnable(GL_TEXTURE_2D);
	glGenTextures(1,&textureId);
	glBindTexture(GL_TEXTURE_2D,textureId);
	glTexImage2D(GL_TEXTURE_2D,0,(isDepth ? GL_RGBA32UI:GL_RGBA8),w,h,0,(isDepth ? GL_RGBA_INTEGER:GL_RGBA),(isDepth ? GL_UNSIGNED_INT : GL_FLOAT), NULL);
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

void global::createFrameBuffer(unsigned int *FBO, unsigned int textureID)
{
	glGenFramebuffers(1, FBO);
	glEnable(GL_TEXTURE_2D);
	glBindFramebuffer(GL_FRAMEBUFFER, *FBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER,GL_COLOR_ATTACHMENT0,GL_TEXTURE_2D,textureID,0);

	int i = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if(i!=GL_FRAMEBUFFER_COMPLETE)
		std::cout << "Framebuffer  "<<FBO<<" is not OK, status=" << i << std::endl;
	glBindFramebuffer(GL_FRAMEBUFFER,0);
}

void global::createFrameBuffer(unsigned int *FBO, unsigned int* textureID)
{
	glGenFramebuffers(1, FBO);
	glEnable(GL_TEXTURE_2D);
	glBindFramebuffer(GL_FRAMEBUFFER, *FBO);
	for(int i = 0; i < 6; i++)
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, *(textureID + i), 0);
	GLenum bufs[6] = {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3, GL_COLOR_ATTACHMENT4,
			GL_COLOR_ATTACHMENT5};
	glDrawBuffers(6, bufs);

	int i = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if(i!=GL_FRAMEBUFFER_COMPLETE)
		std::cout << "Framebuffer is not OK, status=" << i << std::endl;
	glBindFramebuffer(GL_FRAMEBUFFER,0);
}

global::~global()
	{
	 for(int j = 0; j < SHADER_COUNT_TOTAL; j++)
		{
		  if(isShaderSet[j] == 1)delete mShader[j];
		}
	}


void global::dataPoints()
{	
 assert(locationMap.size() == locationMap_2.size());
 assert(locationMap.size() != 0);
 vector3d firstImage, secondImage;
	 firstImage.z = 1.0;	 
	 secondImage.z = 1.0;
 firstImagePoints.clear();
 firstImagePoints.reserve(locationMap.size());
 secondImagePoints.clear();
 secondImagePoints.reserve(locationMap.size());

 for(int dataID = 0; dataID < locationMap.size(); dataID++)
 {
	 firstImage.y = locationMap[dataID]/SCREEN_WIDTH;
	 firstImage.x = (locationMap[dataID] - SCREEN_WIDTH * (int)firstImage.y);

	 secondImage.y = locationMap_2[dataID]/SCREEN_WIDTH;
	 secondImage.x = (locationMap_2[dataID] - SCREEN_WIDTH * (int)secondImage.y);

	firstImagePoints.push_back(firstImage);
	secondImagePoints.push_back(secondImage);
	firstImage.Display("f");
	secondImage.Display("s");
 }
}




