#include "../header/image_read.h"
#include <string>
#include <fstream>

imageRead::imageRead(global *instantGlobal)
{
SCREEN_WIDTH = instantGlobal->SCREEN_WIDTH;
SCREEN_HEIGHT = instantGlobal->SCREEN_HEIGHT;
numTex = 6;
ringFBO1 = &(instantGlobal->mFBO[RING_BLUR_FBO1]);
ringFBO2 = &(instantGlobal->mFBO[RING_BLUR_FBO2]);
supFBO1 = &(instantGlobal->mFBO[SUPPRESS_FBO1]);
supFBO2 = &(instantGlobal->mFBO[SUPPRESS_FBO2]);

totalVertexData.clear();
totalVertexData.reserve(SCREEN_WIDTH * SCREEN_HEIGHT);
totalIndexData.clear();
totalIndexData.reserve(SCREEN_WIDTH * SCREEN_HEIGHT);
}

void imageRead::initialize(global *world)
{
   sceneL = new meshLoader("cones/imL.png");
   sceneR = new meshLoader("cones/imR.png");
   initializeQuad();
   initializeComboQuad();
   imageDraw(world);
 pixelRead();
 pixelRead_2();
pointDescript(world);

 global::firstImagePoints.clear();
 global::firstImagePoints.reserve(global::locationMap.size());
 global::secondImagePoints.clear();
 global::secondImagePoints.reserve(global::locationMap.size());
 
}

void imageRead::initializeQuad()
{
		std::vector<unsigned int> indices;
		std::vector<vertexData> vertices;
		indices.clear();	vertices.clear();
		indices.reserve(6);	vertices.reserve(4);
		vertexData tmp;
		//1.
		tmp.position.change(-1.0,1.0,-1.00);
		tmp.U=0;
		tmp.V=1;
		vertices.push_back(tmp);
		//2.
		tmp.position.change(-1.0,-1.0,-1.00);
		tmp.U=0;
		tmp.V=0;
		vertices.push_back(tmp);
		//3.
		tmp.position.change(1.0,-1.0,-1.00);
		tmp.U=1;
		tmp.V=0;
		vertices.push_back(tmp);
		//4.
		tmp.position.change(1.0,1.0,-1.00);
		tmp.U=1;
		tmp.V=1;
		vertices.push_back(tmp);
		
		indices.push_back(0);
		indices.push_back(1);
		indices.push_back(2);		
		
		indices.push_back(0);
		indices.push_back(2);
		indices.push_back(3);
		quad = new mesh(&vertices,&indices);



		int w = 800;
		int h = 600;
		std::vector<unsigned int> bindices;
		std::vector<vertexData> bvertices;
		bindices.clear();	bvertices.clear();
		bindices.reserve(6);	bvertices.reserve(4);
		vertexData btmp;
		//1.
		btmp.position.change(0,h,-1.00);
		btmp.U=0;
		btmp.V=1;
		bvertices.push_back(btmp);
		//2.
		btmp.position.change(0,0,-1.00);
		btmp.U=0;
		btmp.V=0;
		bvertices.push_back(btmp);
		//3.
		btmp.position.change(w, 0,-1.00);
		btmp.U=1;
		btmp.V=0;
		bvertices.push_back(btmp);
		//4.
		btmp.position.change(w,h,-1.00);
		btmp.U=1;
		btmp.V=1;
		bvertices.push_back(btmp);
		
		bindices.push_back(0);
		bindices.push_back(1);
		bindices.push_back(2);		
		
		bindices.push_back(0);
		bindices.push_back(2);
		bindices.push_back(3);
		bigQuad = new mesh(&bvertices,&bindices);
}

void imageRead::initializeComboQuad()
{
		int w = 800;
		int h = 600;

		std::vector<unsigned int> indices;
		std::vector<vertexData> vertices;
		indices.clear();	vertices.clear();
		indices.reserve(6);	vertices.reserve(4);
		vertexData tmp;
		//1.
		tmp.position.change(0.0, h,-1.00);
		tmp.U=0;
		tmp.V=1;
		vertices.push_back(tmp);
		//2.
		tmp.position.change(0.0, 0.0,-1.00);
		tmp.U=0;
		tmp.V=0;
		vertices.push_back(tmp);
		//3.
		tmp.position.change(w/2, 0.0,-1.00);
		tmp.U=1;
		tmp.V=0;
		vertices.push_back(tmp);
		//4.
		tmp.position.change(w/2, h,-1.00);
		tmp.U=1;
		tmp.V=1;
		vertices.push_back(tmp);
		
		indices.push_back(0);
		indices.push_back(1);
		indices.push_back(2);		
		
		indices.push_back(0);
		indices.push_back(2);
		indices.push_back(3);
		comboQuad1 = new mesh(&vertices,&indices);



		std::vector<unsigned int> bindices;
		std::vector<vertexData> bvertices;
		bindices.clear();	bvertices.clear();
		bindices.reserve(6);	bvertices.reserve(4);
		vertexData btmp;
		//1.
		btmp.position.change(w/2,h,-1.00);
		btmp.U=0;
		btmp.V=1;
		bvertices.push_back(btmp);
		//2.
		btmp.position.change(w/2,0,-1.00);
		btmp.U=0;
		btmp.V=0;
		bvertices.push_back(btmp);
		//3.
		btmp.position.change(w, 0,-1.00);
		btmp.U=1;
		btmp.V=0;
		bvertices.push_back(btmp);
		//4.
		btmp.position.change(w,h,-1.00);
		btmp.U=1;
		btmp.V=1;
		bvertices.push_back(btmp);
		
		bindices.push_back(0);
		bindices.push_back(1);
		bindices.push_back(2);		
		
		bindices.push_back(0);
		bindices.push_back(2);
		bindices.push_back(3);
		comboQuad2 = new mesh(&bvertices,&bindices);

}


void imageRead::pixelRead()
{
	cornerCount.clear();
	cornerCount.reserve(800);

	glBindFramebuffer(GL_FRAMEBUFFER, *ringFBO1);
	for(int m = 1; m <= numTex; m++)
	{
	glReadBuffer(GL_COLOR_ATTACHMENT0 + (m-1));
	glReadPixels(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, GL_RGBA, GL_FLOAT, &pixies_1[m]);
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	
	glBindFramebuffer(GL_FRAMEBUFFER, *supFBO1);
	glReadBuffer(GL_COLOR_ATTACHMENT0);
	glReadPixels(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, GL_RGBA, GL_FLOAT, &pixies_1[0]);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	for(int i = 0; i < SCREEN_WIDTH * SCREEN_HEIGHT * 4; i+=4)
	{
		if(pixies_1[0][i] == 1)
		cornerCount.push_back(i);

	}
 std::cout<<" corner count "<<cornerCount.size()<<std::endl;
}

void imageRead::pixelRead_2()
{
	cornerCount_2.clear();
	cornerCount_2.reserve(800);

	glBindFramebuffer(GL_FRAMEBUFFER, *ringFBO2);
	for(int m = 1; m <= numTex; m++)
	{
	glReadBuffer(GL_COLOR_ATTACHMENT0 + (m-1));
	glReadPixels(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, GL_RGBA, GL_FLOAT, &pixies_2[m][0]);
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glBindFramebuffer(GL_FRAMEBUFFER, *supFBO2);
	glReadBuffer(GL_COLOR_ATTACHMENT0);
	glReadPixels(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, GL_RGBA, GL_FLOAT, &pixies_2[0][0]);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	for(int k = 0; k < SCREEN_WIDTH * SCREEN_HEIGHT * 4; k+=4)
	 {
		if(pixies_2[0][k] == 1.0)
			cornerCount_2.push_back(k);
	 }
 std::cout<<" corner count2 "<<cornerCount_2.size()<<std::endl;
}




void imageRead::pointDescript(global *world)
{
 int texWidth = 4;
 int kay;
 float mark;
 int i , j, k, l, m;

 int desHeight = cornerCount.size();
 int desWidth = (numTex * texWidth) * (numTex * texWidth - 1)/2;	//(n(n-1)/2)
 std::vector<unsigned int>binaryDes[desHeight];				//for all corners
std::cout<<" start ";
 for(i = 0; i < desHeight; i++)
 {
	kay = cornerCount[i];						//take location of any one corner
	binaryDes[i].clear();
	binaryDes[i].reserve(desWidth);					//make room for all possible unrepeated comparisions

	//now go through all intensity values encoded in pixies or compare intensities of receptive fields
	for(j = 1; j <= numTex; j++)
	{
		for(k = 0; k < texWidth; k++)
		{
			mark = pixies_1[j][kay + k];			//take one intensity to compare with rest
			m = k + 1;					//start from the next receptive field's intensity
			for(l = j; l <= numTex; l++)			//to be compared with the following
			{
				while(m < texWidth)
				{
					if(mark < pixies_1[l][kay + m]) binaryDes[i].push_back(0);	//actual comparision and assignment
					else binaryDes[i].push_back(1);
					m++;								//next
				}
				m = 0;
			}
		}
	}
 }


 int desHeight_2 = cornerCount_2.size();
 std::vector<unsigned int>binaryDes_2[desHeight_2];
 for(i = 0; i < desHeight_2; i++)
 {
	kay = cornerCount_2[i];
	binaryDes_2[i].clear();
	binaryDes_2[i].reserve(desWidth);
	for(j = 1; j <= numTex; j++)
	{
		for(k = 0; k < texWidth; k++)
		{
			mark = pixies_2[j][kay + k];
			m = k + 1;
			for(l = j; l <= numTex; l++)
			{
				while(m < texWidth)
				{
					if(mark < pixies_2[l][kay + m]) binaryDes_2[i].push_back(0);
					else binaryDes_2[i].push_back(1);
					m++;
				}
				m = 0;
			}
		}
	}
 }
std::cout<<" end ";
float sum = 0.0;
int key[desWidth];
float value[desWidth];
for(j = 0; j < desWidth; j++)
 {

	sum = 0.0;
	for(i = 0; i < desHeight; i++)
		sum += binaryDes[i][j];
	for(k = 0; k < desHeight_2; k++)
		sum += binaryDes_2[k][j];

 value[j] = fabs(sum/(desHeight+desHeight_2) - 0.5);

 key[j] = j;
 }

 float middleVal, middleKey;
 for(j = 0; j < desWidth; j++)
 {
	for(k = j + 1; k < desWidth; k++)
	{
		if(value[j] > value[k])
		{
			middleVal = value[j]; value[j] = value[k]; value[k] = middleVal;
			middleKey = key[j];   key[j] = key[k];     key[k] = middleKey;
		}
	}
 }



std::bitset<64>img1;
std::bitset<64>img2;
std::bitset<800>flag;
int overallMiss[desHeight_2];
int sect = desWidth / 64;
int p;
std::map<int, int>match;
match.clear();
for(i = 0; i < desHeight; i++)
match[i] = -1;

int maxVal = desWidth; 
for(int w = 0; w < desHeight_2; w++)
	overallMiss[w] = 0;

for(j = 0; j < desHeight; j++)
{
	flag.set();
	maxVal = desWidth;
 for(int m = 0; m < sect; m++)
 {
 	for(i = 0; i < 64; i++){ p = m*64 +i;
	img1[i] = binaryDes[j][key[p]];}

	for(k = 0; k < desHeight_2; k++)
	{
	  if(flag[k] == 1)
	  {
		for(l = 0; l < 64; l++){p = m*64 + l;
		img2[l] = binaryDes_2[k][key[p]];}
	
		img2 ^= img1;
		if(img2.count() > 3*(sect - m)) { flag[k] = 0; overallMiss[k] = 0;} 
		else
			overallMiss[k] += img2.count();
	  }
	} 
 }
	i = 0;
 	for(p = sect*64; p < desWidth; p++){
	img1[i] = binaryDes[j][key[p]]; i++;}
	for(p = i; p < 64; p++)
	img1[p] = 0;

	for(k = 0; k < desHeight_2; k++)
	{
	 if(flag[k] == 1)
	 {
		i = 0;
		for(p = sect*64; p < desWidth; p++){
		img2[i] = binaryDes_2[k][key[p]]; i++;}

		for(p = i; p < 64; p++)
		img2[p] = 0;

		img2 ^= img1;
		if(img2.count() > 8){ flag[k] = 0; overallMiss[k] = 0;}
		else 
		{
			overallMiss[k] += img2.count();
			if(overallMiss[k] < maxVal)
			{maxVal = overallMiss[k]; match[j] = k;}
			overallMiss[k] = 0;
		}
	 }
	} 
}
k = 0;
i = 0;
for(j = 0; j < desHeight; j++)
 {
	if(match[j] != -1)
	{

		 world->locationMap_2[k] = cornerCount_2[match[j]]/4;	//divided by 4 because buffer stored RGBA component of each pixels
		 world->locationMap[k] = cornerCount[j]/4;		//contiguously, so next pixel is 4 units away.
		 k++;
		//std::cout<<" "<<cornerCount[j]/4<<"  "<<cornerCount_2[match[j]]/4<<" and  ";
	}
	
 }
	std::cout<<std::endl<<" matches "<<i<<std::endl;
}



void imageRead::pointDescriptor(const int index)
{
 const int desWidth = 288;		//(n(n-1)/2)

 uint16_t mark[8] = {0};
 uint64_t dummyContain[8] = {0};
 uint64_t dummyContain2[8] = {0};
 uint16_t dummy;
 __m128i first;
 uint16_t hamValueMap[288] = {0};


//using index as height of pixel co-ordinate
//go through all the points along the width i.e. 800 pixels
//each pixel's data: pX[0][i], pX[0][i+1], pX[0][i+2], pX[0][i+3], pX[1][i], pX[1][i+1], pX[1][i+2], pX[1][i+3], pX[2][i], pX[2][i+1]
int i;
for(int widthID = 0; widthID < SCREEN_WIDTH; widthID++)
	{
	 i = (index * SCREEN_WIDTH + widthID) * 4;

//each pixel's data: pX[0][i], pX[0][i+1], pX[0][i+2], pX[0][i+3], pX[1][i], pX[1][i+1], pX[1][i+2], pX[1][i+3], pX[2][i], pX[2][i+1]
	 binChar[widthID][3] = pX[0][i];
	 binChar[widthID][2] = pX[0][i]>>8;
	 binChar[widthID][1] = pX[0][i]>>16;
	 binChar[widthID][0] = pX[0][i]>>24;

	 binChar[widthID][7] = pX[0][i + 1];
	 binChar[widthID][6] = pX[0][i + 1]>>8;
	 binChar[widthID][5] = pX[0][i + 1]>>16;
	 binChar[widthID][4] = pX[0][i + 1]>>24;

	 binChar[widthID][11] = pX[0][i + 2];
	 binChar[widthID][10] = pX[0][i + 2]>>8;
	 binChar[widthID][9] = pX[0][i + 2]>>16;
	 binChar[widthID][8] = pX[0][i + 2]>>24;

	 binChar[widthID][15] = pX[0][i + 3];
	 binChar[widthID][14] = pX[0][i + 3]>>8;
	 binChar[widthID][13] = pX[0][i + 3]>>16;
	 binChar[widthID][12] = pX[0][i + 3]>>24;

	 binChar[widthID][19] = pX[1][i];
	 binChar[widthID][18] = pX[1][i]>>8;
	 binChar[widthID][17] = pX[1][i]>>16;
	 binChar[widthID][16] = pX[1][i]>>24;

	 binChar[widthID][23] = pX[1][i + 1];
	 binChar[widthID][22] = pX[1][i + 1]>>8;
	 binChar[widthID][21] = pX[1][i + 1]>>16;
	 binChar[widthID][20] = pX[1][i + 1]>>24;

	 binChar[widthID][27] = pX[1][i + 2];
	 binChar[widthID][26] = pX[1][i + 2]>>8;
	 binChar[widthID][25] = pX[1][i + 2]>>16;
	 binChar[widthID][24] = pX[1][i + 2]>>24;

	 binChar[widthID][31] = pX[1][i + 3];
	 binChar[widthID][30] = pX[1][i + 3]>>8;
	 binChar[widthID][29] = pX[1][i + 3]>>16;
	 binChar[widthID][28] = pX[1][i + 3]>>24;

	 binChar[widthID][35] = pX[2][i];
	 binChar[widthID][34] = pX[2][i]>>8;
	 binChar[widthID][33] = pX[2][i]>>16;
	 binChar[widthID][32] = pX[2][i]>>24;

	 binCharNext[widthID][3] = pX[3][i];
	 binCharNext[widthID][2] = pX[3][i]>>8;
	 binCharNext[widthID][1] = pX[3][i]>>16;
	 binCharNext[widthID][0] = pX[3][i]>>24;

	 binCharNext[widthID][7] = pX[3][i + 1];
	 binCharNext[widthID][6] = pX[3][i + 1]>>8;
	 binCharNext[widthID][5] = pX[3][i + 1]>>16;
	 binCharNext[widthID][4] = pX[3][i + 1]>>24;

	 binCharNext[widthID][11] = pX[3][i + 2];
	 binCharNext[widthID][10] = pX[3][i + 2]>>8;
	 binCharNext[widthID][9] = pX[3][i + 2]>>16;
	 binCharNext[widthID][8] = pX[3][i + 2]>>24;

	 binCharNext[widthID][15] = pX[3][i + 3];
	 binCharNext[widthID][14] = pX[3][i + 3]>>8;
	 binCharNext[widthID][13] = pX[3][i + 3]>>16;
	 binCharNext[widthID][12] = pX[3][i + 3]>>24;

	 binCharNext[widthID][19] = pX[4][i];
	 binCharNext[widthID][18] = pX[4][i]>>8;
	 binCharNext[widthID][17] = pX[4][i]>>16;
	 binCharNext[widthID][16] = pX[4][i]>>24;

	 binCharNext[widthID][23] = pX[4][i + 1];
	 binCharNext[widthID][22] = pX[4][i + 1]>>8;
	 binCharNext[widthID][21] = pX[4][i + 1]>>16;
	 binCharNext[widthID][20] = pX[4][i + 1]>>24;

	 binCharNext[widthID][27] = pX[4][i + 2];
	 binCharNext[widthID][26] = pX[4][i + 2]>>8;
	 binCharNext[widthID][25] = pX[4][i + 2]>>16;
	 binCharNext[widthID][24] = pX[4][i + 2]>>24;

	 binCharNext[widthID][31] = pX[4][i + 3];
	 binCharNext[widthID][30] = pX[4][i + 3]>>8;
	 binCharNext[widthID][29] = pX[4][i + 3]>>16;
	 binCharNext[widthID][28] = pX[4][i + 3]>>24;

	 binCharNext[widthID][35] = pX[5][i];
	 binCharNext[widthID][34] = pX[5][i]>>8;
	 binCharNext[widthID][33] = pX[5][i]>>16;
	 binCharNext[widthID][32] = pX[5][i]>>24;

	}

/******************************transposition routine************************************/


	int valueIndex;
	//transposition routine starts
	for(int counter = 0; counter < 36; counter++)			//go through 36 characters, which is the descriptionTable of pixels
	{
	 for(int cornerID = 0; cornerID < 8; cornerID++)
	 {
		mark[cornerID] = 0;
		dummyContain[cornerID] = 0;
		dummyContain2[cornerID] = 0;
	 }

	 for(int widthID = 0; widthID < 768; widthID+=16)
	 {
	 //first Image
	 first = _mm_set_epi8(binChar[widthID][counter], binChar[widthID+1][counter], binChar[widthID+2][counter], binChar[widthID+3][counter],
				binChar[widthID+4][counter], binChar[widthID+5][counter], binChar[widthID+6][counter], binChar[widthID+7][counter],binChar[widthID+8][counter], binChar[widthID+9][counter], binChar[widthID+10][counter], binChar[widthID+11][counter],
 binChar[widthID+12][counter], binChar[widthID+13][counter], binChar[widthID+14][counter], binChar[widthID+15][counter]);
	 
	 	for(int i = 0; i < 8; i++)
	 	{
			valueIndex = counter * 800 + i * 100 + widthID/8; 
			dummy = _mm_movemask_epi8(first);
			binCharOut[valueIndex] = dummy>>8;
			binCharOut[valueIndex+1] = dummy;
			dummyContain[i] <<= 16;
			dummyContain[i] |= dummy;
			first = _mm_slli_epi64(first, 1);
	 	}

	  first = _mm_set_epi8(binCharNext[widthID][counter], binCharNext[widthID+1][counter], binCharNext[widthID+2][counter], binCharNext[widthID+3][counter],binCharNext[widthID+4][counter], binCharNext[widthID+5][counter], binCharNext[widthID+6][counter], binCharNext[widthID+7][counter],binCharNext[widthID+8][counter], binCharNext[widthID+9][counter], binCharNext[widthID+10][counter], binCharNext[widthID+11][counter],binCharNext[widthID+12][counter], binCharNext[widthID+13][counter], binCharNext[widthID+14][counter], binCharNext[widthID+15][counter]);
	 
	 	for(int i = 0; i < 8; i++)
	 	{
			valueIndex = counter * 800 + i * 100 + widthID/8; 
			dummy = _mm_movemask_epi8(first);
			binCharNextOut[valueIndex] = dummy>>8;
			binCharNextOut[valueIndex+1] = dummy;
			dummyContain2[i] <<= 16;
			dummyContain2[i] |= dummy;
			first = _mm_slli_epi64(first, 1);
	 	}

		if((widthID+16) % 64 == 0)
		{
			for(int i = 0; i < 8; i++)
			{
				mark[i] += _mm_popcnt_u64(dummyContain[i]);
				mark[i] += _mm_popcnt_u64(dummyContain2[i]);
				dummyContain[i] = 0;
				dummyContain2[i] = 0;
			}
		}
	 }//endof WidthID loop

//start of end segment
//first Image
	for(int widthID = 768; widthID < 800; widthID+=16)
	{
	 first = _mm_set_epi8(binChar[widthID][counter], binChar[widthID+1][counter], binChar[widthID+2][counter], binChar[widthID+3][counter],
				binChar[widthID+4][counter], binChar[widthID+5][counter], binChar[widthID+6][counter], binChar[widthID+7][counter],binChar[widthID+8][counter], binChar[widthID+9][counter], binChar[widthID+10][counter], binChar[widthID+11][counter],
 binChar[widthID+12][counter], binChar[widthID+13][counter], binChar[widthID+14][counter], binChar[widthID+15][counter]);
	 
	 	for(int i = 0; i < 8; i++)
	 	{
			valueIndex = counter * 800 + i * 100 + widthID/8; 
			dummy = _mm_movemask_epi8(first);
			binCharOut[valueIndex] = dummy>>8;
			binCharOut[valueIndex+1] = dummy;
			dummyContain[i] <<= 16;
			dummyContain[i] |= dummy;
			first = _mm_slli_epi64(first, 1);
	 	}
//second Image
	  first = _mm_set_epi8(binCharNext[widthID][counter], binCharNext[widthID+1][counter], binCharNext[widthID+2][counter], binCharNext[widthID+3][counter],binCharNext[widthID+4][counter], binCharNext[widthID+5][counter], binCharNext[widthID+6][counter], binCharNext[widthID+7][counter],binCharNext[widthID+8][counter], binCharNext[widthID+9][counter], binCharNext[widthID+10][counter], binCharNext[widthID+11][counter],binCharNext[widthID+12][counter], binCharNext[widthID+13][counter], binCharNext[widthID+14][counter], binCharNext[widthID+15][counter]);
	 
	 	for(int i = 0; i < 8; i++)
	 	{
			valueIndex = counter * 800 + i * 100 + widthID/8; 
			dummy = _mm_movemask_epi8(first);
			binCharNextOut[valueIndex] = dummy>>8;
			binCharNextOut[valueIndex+1] = dummy;
			dummyContain2[i] <<= 16;
			dummyContain2[i] |= dummy;
			first = _mm_slli_epi64(first, 1);
	 	}

		if((widthID + 16) % 800 == 0)
		{
			for(int i = 0; i < 8; i++)
			{
				mark[i] += _mm_popcnt_u64(dummyContain[i]);
				mark[i] += _mm_popcnt_u64(dummyContain2[i]);
				dummyContain[i] = 0;
				dummyContain2[i] = 0;
			}
		 }
	 }//end of end segment's width wala loop
		for(int markID = 0; markID < 8; markID++)
			hamValueMap[counter*8 + markID] = abs(mark[markID] - 800);
	}//end of corner loop

/****************************** end of transposition routine************************************/


/***************************Sorting Algorithm*************************************/

	uint16_t countMap[801] = {0};
	
	for(int j = 0; j < 288; j++) countMap[hamValueMap[j]]++;

	//cumulative count
	for(int j = 1; j < 801; j++) countMap[j] += countMap[j-1];

	for(int j = 287; j >= 0; j--) {	countMap[hamValueMap[j]]--;	relationMap[countMap[hamValueMap[j]]] = j;	}


/***************************Sorting Algorithm*************************************/


/******************************Second Transposition Routine************************************/
uint64_t firstImageBinary[800][5] = {0};
uint64_t secondImageBinary[800][5] = {0};
int indexValue[16];
	//transposition routine starts
	for(int widthID = 0; widthID < 800; widthID+=8)	
	{
	 for(int counter = 0; counter < 288; counter+=16)
	 {

		for(int id = 0; id < 16; id++)
		indexValue[id] = relationMap[counter + id] * 100 + widthID/8;

	 //first Image
	 first = _mm_set_epi8(binCharOut[indexValue[0]], binCharOut[indexValue[1]], binCharOut[indexValue[2]], binCharOut[indexValue[3]], binCharOut[indexValue[4]], binCharOut[indexValue[5]], binCharOut[indexValue[6]], binCharOut[indexValue[7]], binCharOut[indexValue[8]], binCharOut[indexValue[9]], binCharOut[indexValue[10]], binCharOut[indexValue[11]], binCharOut[indexValue[12]], binCharOut[indexValue[13]], binCharOut[indexValue[14]], binCharOut[indexValue[15]]);
	 
	 	for(int i = 0; i < 8; i++)
	 	{
			dummy = _mm_movemask_epi8(first);
			firstImageBinary[widthID + i][counter/64] <<= 16;
			firstImageBinary[widthID + i][counter/64] |= dummy;
			first = _mm_slli_epi64(first, 1);
	 	}

	 //secondImage
 	 	 first = _mm_set_epi8(binCharNextOut[indexValue[0]], binCharNextOut[indexValue[1]], binCharNextOut[indexValue[2]], binCharNextOut[indexValue[3]], binCharNextOut[indexValue[4]], binCharNextOut[indexValue[5]], binCharNextOut[indexValue[6]], binCharNextOut[indexValue[7]], binCharNextOut[indexValue[8]], binCharNextOut[indexValue[9]], binCharNextOut[indexValue[10]], binCharNextOut[indexValue[11]], binCharNextOut[indexValue[12]], binCharNextOut[indexValue[13]], binCharNextOut[indexValue[14]], binCharNextOut[indexValue[15]]);
	 
	 	for(int i = 0; i < 8; i++)
	 	{
			dummy = _mm_movemask_epi8(first);
			secondImageBinary[widthID + i][counter/64] <<= 16;
			secondImageBinary[widthID + i][counter/64] |= dummy;
			first = _mm_slli_epi64(first, 1);
	 	}

	 }//endof counter loop
	}//end of width loop

std::cout<<" UD "<<index;
/****************************** End of Second Transposition Routine************************************/



/**********************SECOND_COMPARISION_ROUTINE***************************/

std::bitset<800>flag;
uint16_t overallMiss[800] = {0};
uint64_t returnValue;
uint64_t returnPopCount;
int maxVal;
valueIndex = 0;

for(int widthFID = 0; widthFID < 800; widthFID++)
{
 flag.set();
 maxVal = 20;
 for(int sectionID = 0; sectionID < 5; sectionID++)
 {
	for(int widthSID = 0; widthSID < 800; widthSID++)
	{
	  if(flag[widthSID] == 1)
	  {
		returnValue = firstImageBinary[widthFID][sectionID] ^ secondImageBinary[widthSID][sectionID];
		returnPopCount = _mm_popcnt_u64(returnValue);

		if(returnPopCount <  3 * (5 - sectionID))  overallMiss[widthSID] += returnPopCount;
		else flag[widthSID] = 0;
	  }
	} 
 }

 for(int pID = 0; pID < 800; pID++)
 {
	if(flag[pID] == 1 && overallMiss[pID] < maxVal)
	{
		maxVal = overallMiss[pID];
		//std::cout<<" fid "<<widthFID<<" sID "<<pID<<" with miss "<<overallMiss[pID]<<std::endl;
		totalVertexData.push_back(vector3d(widthFID, index,  fabs(widthFID - pID)));
		valueIndex++;
	}
	overallMiss[pID] = 0;
 }
}
std::cout<<" ID "<<index<<" matches "<<valueIndex<<std::endl;

/*********************END_OF_SECOND_COMPARISION_ROUTINE********************/



/*************************END DISPLAY*************************
//int k = 0;
i = 0;
for(int j = 0; j < SCREEN_WIDTH; j++)
 {
	if(match[j] != -1)
	{		
			//sum = fabs(j - match[j]);
			//dispairVertexData[j].color = vector3d(sum, sum, sum);
			std::cout<<"   "<<j<<" and   "<<match[j]<<" next  ";
			i++;
	}
	if(j % 4 == 0) std::cout<<std::endl;
	//else if(match[j] == -1) dispairVertexData[j].color = vector3d(0.0, 0.0, -1.0);
	
 }
	std::cout<<std::endl<<" matches "<<i<<std::endl;
/******************END********************************/
}


void imageRead::drawFST(unsigned int fbo, shader *shades, unsigned int texID)
{
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	glClear(GL_COLOR_BUFFER_BIT);
	shades->useShader();			
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D,texID);
		glUniform1i(glGetUniformLocation(shades->getProgramId(),"textures"),0);
		pipeline.updateMatrices(shades->getProgramId());
		quad->draw(shades->getProgramId());
	shades->delShader();
	glBindFramebuffer(GL_FRAMEBUFFER,0);

}

void imageRead::drawFST(unsigned int fbo, shader *shades, unsigned int texID1, unsigned int texID2)
{
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);		
	glClear(GL_COLOR_BUFFER_BIT);
	shades->useShader();
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texID1);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, texID2);	
		glUniform1i(glGetUniformLocation(shades->getProgramId(),"super_textures"),0);
		glUniform1i(glGetUniformLocation(shades->getProgramId(),"gray_textures"),1);
		pipeline.updateMatrices(shades->getProgramId());
		quad->draw(shades->getProgramId());
	shades->delShader();
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void imageRead::drawFSM(unsigned int fbo, shader *shades, meshLoader *mesh)
{
	glBindFramebuffer(GL_FRAMEBUFFER,fbo);		
	shades->useShader();
		glClear(GL_COLOR_BUFFER_BIT);
		pipeline.updateMatrices(shades->getProgramId());
		mesh->draw(shades->getProgramId());
	shades->delShader();
	glBindFramebuffer(GL_FRAMEBUFFER,0);
}

void imageRead::followRectificationImageDraw(global *world)
{

 pipeline.matrixMode(VIEW_MATRIX);
 pipeline.loadIdentity();
 pipeline.matrixMode(PROJECTION_MATRIX);
 pipeline.loadIdentity();
 pipeline.ortho(0.0, SCREEN_WIDTH, 0.0, SCREEN_HEIGHT, 1.0, 100);
 
 pipeline.matrixMode(MODEL_MATRIX);
 pipeline.loadIdentity();
 glClearColor(1.0, 1.0, 0.0, 1.0);

 glBindFramebuffer(GL_FRAMEBUFFER, world->mFBO[RECTIFY_FBO1]);
 glClear(GL_COLOR_BUFFER_BIT);
 world->mShader[RECTIFY_SHADER]->useShader();			
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D,world->mTexture[GRAYSCALE_TEX1]);
	glUniform1i(glGetUniformLocation(world->mShader[RECTIFY_SHADER]->getProgramId(),"textures"),0);
	pipeline.updateMatrices(world->mShader[RECTIFY_SHADER]->getProgramId());
	glUniformMatrix4fv(glGetUniformLocation(world->mShader[RECTIFY_SHADER]->getProgramId(),"homographyMat"),1,GL_FALSE,&world->firstHomographyMatrix[0][0]);
	bigQuad->draw(world->mShader[RECTIFY_SHADER]->getProgramId());
 world->mShader[RECTIFY_SHADER]->delShader();
 glBindFramebuffer(GL_FRAMEBUFFER,0);


 glBindFramebuffer(GL_FRAMEBUFFER, world->mFBO[RECTIFY_FBO2]);
 glClear(GL_COLOR_BUFFER_BIT);
 world->mShader[RECTIFY_SHADER]->useShader();			
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D,world->mTexture[GRAYSCALE_TEX2]);
	glUniform1i(glGetUniformLocation(world->mShader[RECTIFY_SHADER]->getProgramId(),"textures"),0);
	pipeline.updateMatrices(world->mShader[RECTIFY_SHADER]->getProgramId());
	glUniformMatrix4fv(glGetUniformLocation(world->mShader[RECTIFY_SHADER]->getProgramId(),"homographyMat"),1,GL_FALSE,&world->secondHomographyMatrix[0][0]);
	bigQuad->draw(world->mShader[RECTIFY_SHADER]->getProgramId());
 world->mShader[RECTIFY_SHADER]->delShader();
 glBindFramebuffer(GL_FRAMEBUFFER,0);

 glBindFramebuffer(GL_FRAMEBUFFER, world->mFBO[SIMPLE_FBO]);
 glClear(GL_COLOR_BUFFER_BIT);
 world->mShader[SIMPLE_SHADER]->useShader();			
	pipeline.updateMatrices(world->mShader[SIMPLE_SHADER]->getProgramId());
	bigQuad->draw(world->mShader[SIMPLE_SHADER]->getProgramId());
 world->mShader[SIMPLE_SHADER]->delShader();
 glBindFramebuffer(GL_FRAMEBUFFER,0);

	pipeline.matrixMode(PROJECTION_MATRIX);
	pipeline.loadIdentity();
	pipeline.ortho(-1.0, 1.0, -1.0, 1.0, 1.0, 100);

	pipeline.matrixMode(MODEL_MATRIX);
	pipeline.loadIdentity();
	glClearColor(1.0, 1.0, 0.0, 1.0);

drawFST(world->mFBO[ORIENT_ASSIGN_FBO], world->mShader[ORIENT_ASSIGN_SHADER], world->mTexture[SIMPLE_TEX], world->mTexture[RECTIFY_TEX1]);
//drawFST(world->mFBO[DESCRIPT_FBO1], world->mShader[DESCRIPT_SHADER], world->mTexture[ORIENT_ASSIGN_TEX], world->mTexture[RECTIFY_TEX1]);
	glBindFramebuffer(GL_FRAMEBUFFER, world->mFBO[DESCRIPT_FBO1]);		
	glClear(GL_COLOR_BUFFER_BIT);
	world->mShader[DESCRIPT_SHADER]->useShader();
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, world->mTexture[ORIENT_ASSIGN_TEX]);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, world->mTexture[RECTIFY_TEX1]);	
		glUniform1i(glGetUniformLocation(world->mShader[DESCRIPT_SHADER]->getProgramId(),"super_textures"),0);
		glUniform1i(glGetUniformLocation(world->mShader[DESCRIPT_SHADER]->getProgramId(),"gray_textures"),1);
		glBindFragDataLocation(world->mShader[DESCRIPT_SHADER]->getProgramId(), 0, "out_a");
		glBindFragDataLocation(world->mShader[DESCRIPT_SHADER]->getProgramId(), 1, "out_b");
		glBindFragDataLocation(world->mShader[DESCRIPT_SHADER]->getProgramId(), 2, "out_c");
		pipeline.updateMatrices(world->mShader[DESCRIPT_SHADER]->getProgramId());
		quad->draw(world->mShader[DESCRIPT_SHADER]->getProgramId());
		glBindTexture(GL_TEXTURE_2D, 0);
	world->mShader[DESCRIPT_SHADER]->delShader();
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

drawFST(world->mFBO[ORIENT_ASSIGN_FBO], world->mShader[ORIENT_ASSIGN_SHADER], world->mTexture[SIMPLE_TEX], world->mTexture[RECTIFY_TEX2]);
//drawFST(world->mFBO[DESCRIPT_FBO2], world->mShader[DESCRIPT_SHADER], world->mTexture[ORIENT_ASSIGN_TEX], world->mTexture[RECTIFY_TEX2]);
	glBindFramebuffer(GL_FRAMEBUFFER, world->mFBO[DESCRIPT_FBO2]);		
	glClear(GL_COLOR_BUFFER_BIT);
	world->mShader[DESCRIPT_SHADER]->useShader();
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, world->mTexture[ORIENT_ASSIGN_TEX]);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, world->mTexture[RECTIFY_TEX2]);	
		glUniform1i(glGetUniformLocation(world->mShader[DESCRIPT_SHADER]->getProgramId(),"super_textures"),0);
		glUniform1i(glGetUniformLocation(world->mShader[DESCRIPT_SHADER]->getProgramId(),"gray_textures"),1);
		glBindFragDataLocation(world->mShader[DESCRIPT_SHADER]->getProgramId(), 0, "out_a");
		glBindFragDataLocation(world->mShader[DESCRIPT_SHADER]->getProgramId(), 1, "out_b");
		glBindFragDataLocation(world->mShader[DESCRIPT_SHADER]->getProgramId(), 2, "out_c");
		pipeline.updateMatrices(world->mShader[DESCRIPT_SHADER]->getProgramId());
		quad->draw(world->mShader[DESCRIPT_SHADER]->getProgramId());
		glBindTexture(GL_TEXTURE_2D, 0);
	world->mShader[DESCRIPT_SHADER]->delShader();
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

/*
for(int i = 0; i < 2; i++)
{
 glBindFramebuffer(GL_FRAMEBUFFER, world->mFBO[KERNEL_FBOV1 + i]);
 glClear(GL_COLOR_BUFFER_BIT);
 world->mShader[KERNEL_SHADER]->useShader();			
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D,world->mTexture[RECTIFY_TEX1 + i]);
	glUniform1i(glGetUniformLocation(world->mShader[KERNEL_SHADER]->getProgramId(),"gray_textures"),0);
	pipeline.updateMatrices(world->mShader[KERNEL_SHADER]->getProgramId());
	glVertexAttrib1f(glGetAttribLocation(world->mShader[KERNEL_SHADER]->getProgramId(), "ID"), 1.0f);
	bigQuad->draw(world->mShader[KERNEL_SHADER]->getProgramId());
 world->mShader[KERNEL_SHADER]->delShader();
 glBindFramebuffer(GL_FRAMEBUFFER,0);

 glBindFramebuffer(GL_FRAMEBUFFER, world->mFBO[KERNEL_FBOH1 + i]);
 glClear(GL_COLOR_BUFFER_BIT);
 world->mShader[KERNEL_SHADER]->useShader();			
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D,world->mTexture[KERNEL_TEXV1 + i]);
	glUniform1i(glGetUniformLocation(world->mShader[KERNEL_SHADER]->getProgramId(),"gray_textures"),0);
	pipeline.updateMatrices(world->mShader[KERNEL_SHADER]->getProgramId());
	glVertexAttrib1f(glGetAttribLocation(world->mShader[KERNEL_SHADER]->getProgramId(), "ID"), 0.0f);
	bigQuad->draw(world->mShader[KERNEL_SHADER]->getProgramId());
 world->mShader[KERNEL_SHADER]->delShader();
 glBindFramebuffer(GL_FRAMEBUFFER,0);
}
*/
}

void imageRead::followRectificationPixelRead(global *world)
{

	//first Image
	glBindFramebuffer(GL_FRAMEBUFFER, world->mFBO[DESCRIPT_FBO1]);
	for(int m = 0; m < 3; m++)
	 {
		glReadBuffer(GL_COLOR_ATTACHMENT0 + m);
		glReadPixels(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, GL_RGBA_INTEGER, GL_UNSIGNED_INT, &pX[m][0]);		//123
	 }
	glReadBuffer(GL_BACK);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	//second Image
	glBindFramebuffer(GL_FRAMEBUFFER, world->mFBO[DESCRIPT_FBO2]);
	for(int m = 0; m < 3; m++)
	 {
		glReadBuffer(GL_COLOR_ATTACHMENT0 + m);
		glReadPixels(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, GL_RGBA_INTEGER, GL_UNSIGNED_INT, &pX[m+3][0]);		//456
	 }
	glReadBuffer(GL_BACK);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
/*	
	std::cout<<std::endl<<" evalution "<<std::endl;
	for(int i = 0; i < 100; i++)
		std::cout<<" i "<<std::dec<<i<<"  "<<std::hex<<pX[0][i]<<"   and   "<<pX[3][i]<<std::endl;

	glBindFramebuffer(GL_FRAMEBUFFER, world->mFBO[DESCRIPT_FBO1]);
	glReadBuffer(GL_COLOR_ATTACHMENT0);
	int pixelui[4 * SCREEN_WIDTH * SCREEN_HEIGHT];
	glReadPixels(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, GL_RGBA_INTEGER, GL_UNSIGNED_INT, pixelui);
	glReadBuffer(GL_BACK);
	for(int i = 0; i < 100; i++)
		std::cout<<" i "<<std::dec<<i<<std::hex<<"  "<<pixelui[i]<<std::endl;
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
*/
}

void imageRead::followRectification(global *world)
{
 //draw image, each pixel has grayscale rgb values encoded with varying size of kernels
 followRectificationImageDraw(world);
 //read pixel values and store
 followRectificationPixelRead(world);

 std::cout<<" start "<<std::endl;
 for(int i = 0; i < 600;i++)
	 pointDescriptor(i);
 for(unsigned int k = 0; k < totalVertexData.size(); k++) totalIndexData.push_back(k);

 //for(int k = 0; k < totalVertexData.size()/10; k++) totalVertexData[k].Display();

 grayMesh = new lines(&totalVertexData, &totalIndexData);

pipeline.matrixMode(PROJECTION_MATRIX);
pipeline.pushMatrix();
pipeline.loadIdentity();
pipeline.ortho(0.0, SCREEN_WIDTH, 0.0, SCREEN_HEIGHT, -1000.0, 1000.0);

glClearColor(0.0, 0.0, 0.0, 1.0);

glBindFramebuffer(GL_FRAMEBUFFER, world->mFBO[SCENE_FBO]);		
		world->mShader[SCENE_SHADER]->useShader();
		glBindFragDataLocation(world->mShader[SCENE_SHADER]->getProgramId(), 0, "outVal");
		glClear(GL_COLOR_BUFFER_BIT);
		pipeline.updateMatrices(world->mShader[SCENE_SHADER]->getProgramId());
		grayMesh->linesDraw(world->mShader[SCENE_SHADER]->getProgramId(), true);
		world->mShader[SCENE_SHADER]->delShader();
glBindFramebuffer(GL_FRAMEBUFFER, 0);

glClear(GL_COLOR_BUFFER_BIT);
glBindFramebuffer(GL_FRAMEBUFFER, world->mFBO[BIG_BLUR_FBO1]);		
world->mShader[BIG_BLUR_SHADER1]->useShader();
		glBindFragDataLocation(world->mShader[BIG_BLUR_SHADER1]->getProgramId(), 0, "outVal");
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, world->mTexture[SCENE_TEX]);	
		glUniform1i(glGetUniformLocation(world->mShader[BIG_BLUR_SHADER1]->getProgramId(),"gray_textures"),0);
		pipeline.updateMatrices(world->mShader[BIG_BLUR_SHADER1]->getProgramId());
		bigQuad->draw(world->mShader[BIG_BLUR_SHADER1]->getProgramId());
world->mShader[BIG_BLUR_SHADER1]->delShader();
glBindFramebuffer(GL_FRAMEBUFFER, 0);

glClear(GL_COLOR_BUFFER_BIT);
glBindFramebuffer(GL_FRAMEBUFFER, world->mFBO[BIG_BLUR_FBO2]);		
world->mShader[BIG_BLUR_SHADER2]->useShader();
		glBindFragDataLocation(world->mShader[BIG_BLUR_SHADER2]->getProgramId(), 0, "outVal");
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, world->mTexture[BIG_BLUR_TEX1]);	
		glUniform1i(glGetUniformLocation(world->mShader[BIG_BLUR_SHADER2]->getProgramId(),"gray_textures"),0);
		pipeline.updateMatrices(world->mShader[BIG_BLUR_SHADER2]->getProgramId());
		bigQuad->draw(world->mShader[BIG_BLUR_SHADER2]->getProgramId());
world->mShader[BIG_BLUR_SHADER2]->delShader();
glBindFramebuffer(GL_FRAMEBUFFER, 0);

pipeline.popMatrix();



 std::cout<<" end "<<std::endl;
}

void imageRead::imageDraw(global *instantGlobal)
{
	pipeline.matrixMode(PROJECTION_MATRIX);
	pipeline.loadIdentity();
	pipeline.ortho(-1.0, 1.0, -1.0, 1.0, 1.0, 100);

	pipeline.matrixMode(MODEL_MATRIX);
	pipeline.loadIdentity();
	glClearColor(0.25, 0.25, 0.25, 1.0);

	drawFSM(instantGlobal->mFBO[TRUE_COLOR_FBO], instantGlobal->mShader[TRUE_COLOR_SHADER], sceneL);

	//create derivative image
	drawFSM(instantGlobal->mFBO[DERIVATIVE_FBO], instantGlobal->mShader[DERIVATIVE_SHADER], sceneL);
	//blur derivative image
	drawFST(instantGlobal->mFBO[BLUR_FBO1], instantGlobal->mShader[BLUR_SHADER1], instantGlobal->mTexture[DERIVATIVE_TEX]);
	//blur derivative image again
	drawFST(instantGlobal->mFBO[BLUR_FBO2], instantGlobal->mShader[BLUR_SHADER2], instantGlobal->mTexture[BLUR_TEX1]);
	//detect corners
	drawFST(instantGlobal->mFBO[HARRIS_CORNER_FBO], instantGlobal->mShader[HARRIS_CORNER_SHADER], instantGlobal->mTexture[BLUR_TEX2]);
	//suppress corners
	drawFST(instantGlobal->mFBO[SUPPRESS_FBO1], instantGlobal->mShader[SUPPRESS_SHADER], instantGlobal->mTexture[HARRIS_CORNER_TEX]);
	//grayscale image
	drawFSM(instantGlobal->mFBO[GRAYSCALE_FBO1], instantGlobal->mShader[GRAYSCALE_SHADER], sceneL);
	//use grayscale and suppress texture, to find degree orientation
	drawFST(instantGlobal->mFBO[ORIENT_ASSIGN_FBO], instantGlobal->mShader[ORIENT_ASSIGN_SHADER], instantGlobal->mTexture[SUPPRESS_TEX1], instantGlobal->mTexture[GRAYSCALE_TEX1]);
	//use orientation texture and grayscale to find blur and store all values in 6 textures
	drawFST(instantGlobal->mFBO[RING_BLUR_FBO1], instantGlobal->mShader[RING_BLUR_SHADER], instantGlobal->mTexture[ORIENT_ASSIGN_TEX], instantGlobal->mTexture[GRAYSCALE_TEX1]);

	drawFSM(instantGlobal->mFBO[DERIVATIVE_FBO], instantGlobal->mShader[DERIVATIVE_SHADER], sceneR);
	drawFST(instantGlobal->mFBO[BLUR_FBO1], instantGlobal->mShader[BLUR_SHADER1], instantGlobal->mTexture[DERIVATIVE_TEX]);
	drawFST(instantGlobal->mFBO[BLUR_FBO2], instantGlobal->mShader[BLUR_SHADER2], instantGlobal->mTexture[BLUR_TEX1]);
	drawFST(instantGlobal->mFBO[HARRIS_CORNER_FBO], instantGlobal->mShader[HARRIS_CORNER_SHADER], instantGlobal->mTexture[BLUR_TEX2]);
	drawFST(instantGlobal->mFBO[SUPPRESS_FBO2], instantGlobal->mShader[SUPPRESS_SHADER], instantGlobal->mTexture[HARRIS_CORNER_TEX]);
	drawFSM(instantGlobal->mFBO[GRAYSCALE_FBO2], instantGlobal->mShader[GRAYSCALE_SHADER], sceneR);
	drawFST(instantGlobal->mFBO[ORIENT_ASSIGN_FBO], instantGlobal->mShader[ORIENT_ASSIGN_SHADER], instantGlobal->mTexture[SUPPRESS_TEX2], instantGlobal->mTexture[GRAYSCALE_TEX2]);
	drawFST(instantGlobal->mFBO[RING_BLUR_FBO2], instantGlobal->mShader[RING_BLUR_SHADER], instantGlobal->mTexture[ORIENT_ASSIGN_TEX], instantGlobal->mTexture[GRAYSCALE_TEX2]);

}

imageRead::~imageRead()	
{
	 delete sceneL;
	 delete sceneR;
	delete quad;
	delete bigQuad;
	delete comboQuad1;
	delete comboQuad2;
	delete grayMesh;
}

void imageRead::fillMatchingPoints()
{
	//take global::inliersRecord, firstImagePoints, secondImagePoints
	//create vector
	int num_pts = global::inliersRecord.size();
	int dataID;
	g_points.clear();
	g_points.reserve(num_pts);
	g_index.clear();
	g_index.reserve(num_pts);
	vector3d first;
	float width = global::SCREEN_WIDTH/2;
	unsigned int counter = 0;
	for(int i = 0; i < num_pts; i++)
	{
		dataID = global::inliersRecord[i];
		first = global::firstImagePoints[dataID];
		g_points.push_back(vector3d(first.x * 0.5, first.y, -1.0));
		first = global::secondImagePoints[dataID];
		g_points.push_back(vector3d(first.x * 0.5 + width, first.y, -1.0));
		g_index.push_back(counter);
		counter++;
		g_index.push_back(counter);
		counter++;
	}
	
}



