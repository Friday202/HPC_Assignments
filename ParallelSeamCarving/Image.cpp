#include "Image.h"

#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION

#include "stb_image.h"
#include "stb_image_write.h"
#include <algorithm>
#include <omp.h>

const int Image::dx[NUM_PIXEL_NEIGHB] = { -1, 0, 1, -1, 1, -1, 0, 1 };
const int Image::dy[NUM_PIXEL_NEIGHB] = { -1, -1, -1, 0, 0, 1, 1, 1 };

Image::Image(std::string filename, int numOfPixels) : numOfPixels(numOfPixels)
{
	LoadImage(filename, imgWidth, imgHeight, channelsNum, arraySize);
	assert(pixelArray);

	energyArray = new double[arraySize]; 
	cumulativeEnergyArray = new double[arraySize]; 
}

Image::~Image()
{
	// Relese memory 
	delete[] pixelArray; 
	delete[] energyArray;
	delete[] cumulativeEnergyArray; 
}

void Image::LoadImage(std::string filename, int& outWidth, int& outHeight, int& outChannelNum, int& outArraySize)
{
	uint8_t* loadedImage = stbi_load(filename.c_str(), &outWidth, &outHeight, &outChannelNum, NUM_DESIRED_CH);
	outArraySize = outWidth * outHeight;

	// Allocate dynamic memory for array
	pixelArray = new Pixel[outArraySize];
	
	#pragma omp parallel for
	for (int i = 0; i < outArraySize; ++i)
	{
		int pixelIndex = i * outChannelNum;
		Pixel pixel(loadedImage[pixelIndex], loadedImage[pixelIndex + 1], loadedImage[pixelIndex + 2], outChannelNum == 4 ? loadedImage[pixelIndex + 3] : 255);
		pixelArray[i] = pixel;
	}	
}

void Image::WriteImage(std::string filename)
{
	std::vector<uint8_t> outArray(arraySize * channelsNum);

	for (int i = 0, j = 0; i < arraySize; ++i, j += channelsNum)
	{
		outArray[j] = pixelArray[i].R;
		outArray[j + 1] = pixelArray[i].G;
		outArray[j + 2] = pixelArray[i].B;

		if (channelsNum == 4)
		{
			outArray[j + 3] = pixelArray[i].A;
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

	WriteImageDebug(filename, energyArray); 
}

void Image::ShowCumulativeEnergyImage(std::string filename)
{
	TIMER_START;
	CalculateCumulativeEnergy();
	TIMER_END;

	WriteImageDebug(filename, cumulativeEnergyArray); 
}

void Image::CalculateGradient()
{
	// Each thread takes a pixel
	#pragma omp parallel for
	for (int i = 0; i < arraySize; ++i)
	{
		// Gets its 8 neighbors
		std::vector<Pixel> pixelNeighbors = GetPixelNeighbors(i);
		
		// Calculates gradient 
		Pixel gradX = CalculateGradientX(pixelNeighbors);
		Pixel gradY = CalculateGradientY(pixelNeighbors);

		// Calcualtes pixel energy
		double energy = CalculateEnergy(gradX, gradY); 

		// Stores it in energy array
		energyArray[i] = energy; 
	}	
}

void Image::CalculateCumulativeEnergy()
{
	// Copy first row values directly 
	for (int i = arraySize; i >= arraySize - imgWidth; --i)
	{
		cumulativeEnergyArray[i] = energyArray[i]; 
	}

	// The rest is caculated
	int startingIndex = arraySize - imgWidth; // ???  -1 ??? ;
	for (int i = startingIndex; i >= 0; --i)
	{
		int indexC = i + imgWidth;
		int indexL = indexC - 1;
		int indexR = indexC + 1;

		double minValue; 

		// Right pixel border 
		if ((i + 1) % imgWidth == 0 || i == startingIndex)
		{
			minValue = std::min({ cumulativeEnergyArray[indexC], cumulativeEnergyArray[indexL], REALLY_BIG_NUM });
		}
		// Left pixel border 
		else if ((i - 1) % imgWidth == 0 || i == 0)
		{
			minValue = std::min({ cumulativeEnergyArray[indexC], REALLY_BIG_NUM, cumulativeEnergyArray[indexR] });
		}
		// Center pixel 
		else
		{
			minValue = std::min({ cumulativeEnergyArray[indexC], cumulativeEnergyArray[indexL], cumulativeEnergyArray[indexR] });
		}		 

		// Sum with previous row min value
		cumulativeEnergyArray[i] = energyArray[i] + minValue;
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
	return (std::sqrt(SquaredSummed.computeR) + std::sqrt(SquaredSummed.computeG) + std::sqrt(SquaredSummed.computeB)) / channelsNum;
}

std::vector<Pixel> Image::GetPixelNeighbors(const int index)
{
	std::vector<Pixel> neighbors(NUM_PIXEL_NEIGHB);

	// Get position of index in 2D array 
	int x = index % imgWidth;
	int y = index / imgWidth; 

	for (int i = 0; i < NUM_PIXEL_NEIGHB; ++i)
	{
		int nx = x + dx[i];
		int ny = y + dy[i]; 
		int neighborIndex = ny * imgWidth + nx;

		neighbors[i] = (nx >= 0 && nx < imgWidth && ny >= 0 && ny < imgHeight) ? pixelArray[neighborIndex] : pixelArray[index]; 		
	}
	return neighbors;
}

void Image::ResetPixelValues()
{
	for (int i = 0; i < arraySize; ++i)
	{
		pixelArray[i].Reset(); 
	}
}

void Image::WriteImageDebug(std::string filename, double* forArray)
{
	assert(forArray);
	std::vector<uint8_t> outArray(arraySize);
	for (int i = 0; i < arraySize; ++i)
	{
		outArray[i] = static_cast<uint8_t>(forArray[i] < 0 ? 0 : forArray[i]) * 3;
	}
	stbi_write_png(filename.c_str(), imgWidth, imgHeight, 1, outArray.data(), 0);
}

void Image::RemoveSeam()
{
	for (int i = 0; i < numOfPixels; ++i)
	{
		CalculateGradient();
		CalculateCumulativeEnergy(); 

		std::vector<int> indexesToRemoeve = FindMinPath();

		// Problem 




		// Update array size and img width
		--imgWidth;
		arraySize = imgHeight * imgWidth; 
		ResetPixelValues(); 
	}
}

std::vector<int> Image::FindMinPath()
{
	std::vector<int> indexesToRemove(imgHeight);

	// Find value in top row with lowest cumulative energy 	
	double* minValue = std::min_element(energyArray, energyArray + imgWidth);
	size_t indexToRemove = minValue - energyArray;
	indexesToRemove[0] = indexToRemove;

	// Loop thru one column 
	for (int i = 1; i < imgHeight; ++i)
	{
		// Get index neighbors 
		int indexC = indexToRemove + imgWidth;
		int indexL = indexC - 1;
		int indexR = indexC + 1;

		int selectedIndex = -1;

		// Right pixel border 
		if ((i + 1) % imgWidth == 0)
		{
			selectedIndex = cumulativeEnergyArray[indexC] > cumulativeEnergyArray[indexL] ? indexL : indexC;
		}
		// Left pixel border 
		else if ((i - 1) % imgWidth == 0)
		{
			selectedIndex = cumulativeEnergyArray[indexC] > cumulativeEnergyArray[indexR] ? indexR : indexC;
		}
		// Center pixel 
		else
		{
			if (cumulativeEnergyArray[indexC] < cumulativeEnergyArray[indexR] && cumulativeEnergyArray[indexC] < cumulativeEnergyArray[indexL])
			{
				selectedIndex = indexC;
			}
			else
			{
				selectedIndex = cumulativeEnergyArray[indexR] > cumulativeEnergyArray[indexL] ? indexL : indexR;
			}
		}

		// Pixel with min value is selected 
		indexToRemove = selectedIndex;
		indexesToRemove[i] = indexToRemove;
	}
	return indexesToRemove; 
}

