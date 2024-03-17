#include "Image.h"

#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION

#include "stb_image.h"
#include "stb_image_write.h"
#include <omp.h>


Image::Image(std::string filename, int numOfPixels) : numOfPixels(numOfPixels)
{
	imgArray = Image::LoadImage(filename, imgWidth, imgHeight, channelsNum, arraySize);
	assert(imgArray);
}

Image::~Image()
{
	delete[] imgArray; 
}

/*static*/ Pixel* Image::LoadImage(std::string filename, int& outWidth, int& outHeight, int& outChannelNum, int& outArraySize)
{
	uint8_t* loadedImage = stbi_load(filename.c_str(), &outWidth, &outHeight, &outChannelNum, 0);
	outArraySize = outWidth * outHeight;

	// Allocate dynamic array
	Pixel* imageArray = new Pixel[outArraySize];
	
	for (int i = 0; i < outArraySize; ++i)
	{
		int pixelIndex = i * outChannelNum;
		Pixel pixel(loadedImage[pixelIndex], loadedImage[pixelIndex + 1], loadedImage[pixelIndex + 2], outChannelNum == 4 ? loadedImage[pixelIndex + 3] : 255);
		imageArray[i] = pixel;
	}

	return imageArray;
}

void Image::WriteImage(std::string filename)
{
	std::vector<uint8_t> outArray(arraySize * channelsNum);

	for (int i = 0, j = 0; i < arraySize; ++i, j += channelsNum)
	{
		outArray[j] = imgArray[i].R;
		outArray[j + 1] = imgArray[i].G;
		outArray[j + 2] = imgArray[i].B;

		if (channelsNum == 4)
		{
			outArray[j + 3] = imgArray[i].A;
		}
	}

	stbi_write_png(filename.c_str(), imgWidth, imgHeight, channelsNum, outArray.data(), 0);
}

void Image::DisplayInformation()
{
	std::cout << "Image stats\n\theight: " << imgHeight << "\n\twidth: " << imgWidth << "\n\tnumber of channels: " << channelsNum << std::endl; 
}

void Image::ShowGradientImage(std::string filename)
{
	TIMER_START; 
	CalculateGradient(); 
	TIMER_END; 

	std::vector<uint8_t> outArray(arraySize);

	for (int i = 0; i < arraySize; ++i)
	{
		outArray[i] = imgArray[i].Energy;		
	}
	
	stbi_write_png(filename.c_str(), imgWidth, imgHeight, 1, outArray.data(), 0);
}

void Image::CalculateGradient()
{
	#pragma omp parallel for
	for (int i = 0; i < arraySize; ++i)
	{
		// each thread will take x pixel and:
		// get its 8 neighbours
		// calculate gradient x and y

		// Each thread takes a pixel

		// Gets its 8 neighbors
		std::vector<Pixel> pixelNeihgbors = GetPixelNeighbors(imgArray[i], i); 
		
		// Calculates gradient 
		Pixel gradX = CalculateGradientX(pixelNeihgbors); 
		Pixel gradY = CalculateGradientY(pixelNeihgbors); 

		double energy = CalculateEnergy(gradX, gradY); 

		imgArray[i].Energy = static_cast<uint8_t>( energy < 0 ? 0 : energy);

	}	
}

Pixel Image::CalculateGradientX(std::vector<Pixel> pixelNeighbors)
{
	assert(pixelNeighbors.size() == 8); 
	return pixelNeighbors[7] - pixelNeighbors[0] - pixelNeighbors[1] * 2 - pixelNeighbors[2]+ pixelNeighbors[5] + pixelNeighbors[6] * 2;
}

Pixel Image::CalculateGradientY(std::vector<Pixel> pixelNeighbors)
{
	assert(pixelNeighbors.size() == 8);
	return pixelNeighbors[0] + pixelNeighbors[3] * 2 + pixelNeighbors[5] - pixelNeighbors[2] - pixelNeighbors[4] * 2 - pixelNeighbors[7];
}

double Image::CalculateEnergy(Pixel& gradX, Pixel& gradY)
{
	Pixel SquaredSummed = gradX * gradX + gradY * gradY;
	return (std::sqrt(SquaredSummed.computeR) + std::sqrt(SquaredSummed.computeG) + std::sqrt(SquaredSummed.computeB)) / 3 ;	
}

std::vector<Pixel> Image::GetPixelNeighbors(Pixel& ForPixel, int index)
{
	std::vector<Pixel> neighbors;

	int x = index % imgWidth;
	int y = index / imgWidth; 

	for (int i = 0; i < 8; ++i)
	{
		int nx = x + dx[i];
		int ny = y + dy[i]; 
		int neighborIndex = ny * imgWidth + nx;

		if (nx >= 0 && nx < imgWidth && ny >= 0 && ny < imgHeight) 
		{
			neighbors.push_back(imgArray[neighborIndex]);
		}
		else
		{
			// else push back yourself 
			neighbors.push_back(imgArray[index]);
		}
	}
	return neighbors;
}

