#include "ImageSeamCarver.h"

#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION

#include "stb_image.h"
#include "stb_image_write.h"
#include <algorithm>
#include <omp.h>

const int ImageSeamCarver::dx[NUM_PIXEL_NEIGHB] = { -1, 0, 1, -1, 1, -1, 0, 1 };
const int ImageSeamCarver::dy[NUM_PIXEL_NEIGHB] = { -1, -1, -1, 0, 0, 1, 1, 1 };

ImageSeamCarver::ImageSeamCarver(std::string filename, int numOfSeams) : numOfSeams(numOfSeams)
{
	LoadImage(filename, imgWidth, imgHeight, channelsNum);	

	lastElement = imgHeight * imgWidth;
	originalImgWidth = imgWidth;

	energyArray = new double[imgHeight * imgWidth];
	cumulativeEnergyArray = new double[imgHeight * imgWidth];
}

ImageSeamCarver::~ImageSeamCarver()
{
	// Relese memory 
	delete[] pixelArray;
	delete[] energyArray;
	delete[] cumulativeEnergyArray;
}

void ImageSeamCarver::RemoveSeam()
{
	std::cout << "Removing " << numOfSeams << " seams..." << std::endl;

	auto start_time = std::chrono::high_resolution_clock::now();	
	std::chrono::microseconds duration; 

	double averageTimes[3] = { 0 }; 

	for (int i = 0; i < numOfSeams; ++i)
	{
#if DEBUG_MODE
		std::cout << "Removing " << i + 1 << ". seam... " << std::endl;
#endif

		// 1. Step calculate energy 
		CalculateEnergy(); 

		duration = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - start_time);
		averageTimes[0] += duration.count();

#if DEBUG_MODE
		std::cout << "Time taken for Energy calculation: " << duration.count() << " microseconds" << std::endl;
#endif

		start_time = std::chrono::high_resolution_clock::now();

		// 2. Step calculate cumulative energy 
		CalculateCumulativeEnergy(); 
		
		duration = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - start_time);
		averageTimes[1] += duration.count();

#if DEBUG_MODE
		std::cout << "Time taken for Cumulative energy calculation: " << duration.count() << " microseconds" << std::endl;
#endif

		start_time = std::chrono::high_resolution_clock::now();

		// 3. Step remove 1 pixel column from image 
		RemovePixelColumn();

		duration = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - start_time);
		averageTimes[2] += duration.count();

#if DEBUG_MODE
		std::cout << "Time taken for Seam removal: " << duration.count() << " microseconds" << std::endl;
#endif
		start_time = std::chrono::high_resolution_clock::now();

		// 4. Step update values
		--imgWidth;
		--lastElement;			
	}

	for (int i = 0; i < 3; ++i) {
		averageTimes[i] /= numOfSeams;
	}

	std::cout << "Average time taken for Energy calculation: " << averageTimes[0] << " microseconds" << std::endl;
	std::cout << "Average time taken for Cumulative energy calculation: " << averageTimes[1] << " microseconds" << std::endl;
	std::cout << "Average time taken for Seam removal: " << averageTimes[2] << " microseconds" << std::endl;
}

void ImageSeamCarver::LoadImage(std::string filename, int& outWidth, int& outHeight, int& outChannelNum)
{
	uint8_t* loadedImage = stbi_load(filename.c_str(), &outWidth, &outHeight, &outChannelNum, NUM_DESIRED_CH);	

	// Allocate dynamic memory for array
	int arraySize = outWidth * outHeight; 
	pixelArray = new Pixel[arraySize];

	#pragma omp parallel for
	for (int i = 0; i < arraySize; ++i)
	{
		int pixelIndex = i * outChannelNum;
		Pixel pixel(loadedImage[pixelIndex], loadedImage[pixelIndex + 1], loadedImage[pixelIndex + 2], outChannelNum == 4 ? loadedImage[pixelIndex + 3] : 255);
		pixelArray[i] = pixel;
	}
}

void ImageSeamCarver::WriteImage(std::string filename)
{
	std::vector<uint8_t> outArray(imgHeight * imgWidth * channelsNum);
	int k = 0;
	for (int i = 0; i < imgHeight; ++i)
	{
		for (int j = 0; j < imgWidth; ++j)
		{
			int index = i * originalImgWidth + j;

			outArray[k] = pixelArray[index].R;
			outArray[k + 1] = pixelArray[index].G;
			outArray[k + 2] = pixelArray[index].B;

			if (channelsNum == 4)
			{
				outArray[k + 3] = pixelArray[index].A;
			}
			k += channelsNum;
		}
	}
	stbi_write_png(filename.c_str(), imgWidth, imgHeight, channelsNum, outArray.data(), 0);
}

void ImageSeamCarver::DisplayInformation()
{
	std::cout << "Image stats\n\theight: " << imgHeight << "\n\twidth: " << imgWidth << "\n\tnumber of channels: " << channelsNum << std::endl;
}

void ImageSeamCarver::CalculateEnergy()
{
	for (int i = 0; i < imgHeight; ++i)
	{
		#pragma omp parallel for
		for (int j = 0; j < imgWidth; ++j)
		{

#if USE_ENERGY_OPTIMIZATION
			// If pixel already has calculated energy skip
			if (!pixelArray[i * originalImgWidth + j].bNeedsUpdate) continue; 
#endif

			// Gets its 8 neighbors
			std::vector<Pixel*> pixelNeighbors = GetPixelNeighbors(j, i);

			// Calculates gradient 
			Vector3 gradX = CalculateGradientX(pixelNeighbors);
			Vector3 gradY = CalculateGradientY(pixelNeighbors);

			// Calcualtes pixel energy
			double energy = GetEnergy(gradX, gradY);

			// Stores it in energy array
			energyArray[i * originalImgWidth + j] = energy;

#if USE_ENERGY_OPTIMIZATION
			pixelArray[i * originalImgWidth + j].bNeedsUpdate = false; 
#endif
		}
	}
}

void ImageSeamCarver::CalculateCumulativeEnergy()
{
	// Copy last row values directly 		
	#pragma omp parallel for
	for (int i = lastElement - 1; i >= lastElement - imgWidth; --i)
	{
		cumulativeEnergyArray[i] = energyArray[i];
	}

	for (int i = imgHeight - 2; i >= 0; --i)
	{
		#pragma omp parallel for
		for (int j = imgWidth - 1; j >= 0; --j)
		{
			int index = i * originalImgWidth + j;

			int indexC = index + originalImgWidth;
			int indexL = indexC - 1;
			int indexR = indexC + 1;

			double minValue;

			int leftMostIndex = i * originalImgWidth;
			int rightMostIndex = leftMostIndex + imgWidth - 1;

			// Right pixel border 
			if (index == rightMostIndex)
			{
				minValue = std::min({ cumulativeEnergyArray[indexC], cumulativeEnergyArray[indexL], REALLY_BIG_NUM });
			}
			// Left pixel border 
			else if (index == leftMostIndex || index == 0)
			{
				minValue = std::min({ cumulativeEnergyArray[indexC], REALLY_BIG_NUM, cumulativeEnergyArray[indexR] });
			}
			// Center pixel 
			else
			{
				minValue = std::min({ cumulativeEnergyArray[indexC], cumulativeEnergyArray[indexL], cumulativeEnergyArray[indexR] });
			}
			// Sum with previous row min value
			cumulativeEnergyArray[index] = energyArray[index] + minValue;
		}
	}
}

Vector3 ImageSeamCarver::CalculateGradientX(std::vector<Pixel*> pixelNeighbors)
{
	Vector3 vec;
	vec.X = pixelNeighbors[7]->computeR - pixelNeighbors[0]->computeR - pixelNeighbors[1]->computeR * 2 - pixelNeighbors[2]->computeR + pixelNeighbors[5]->computeR + pixelNeighbors[6]->computeR * 2;
	vec.Y = pixelNeighbors[7]->computeG - pixelNeighbors[0]->computeG - pixelNeighbors[1]->computeG * 2 - pixelNeighbors[2]->computeG + pixelNeighbors[5]->computeG + pixelNeighbors[6]->computeG * 2;
	vec.Z = pixelNeighbors[7]->computeB - pixelNeighbors[0]->computeB - pixelNeighbors[1]->computeB * 2 - pixelNeighbors[2]->computeB + pixelNeighbors[5]->computeB + pixelNeighbors[6]->computeB * 2;

	return vec;
}

Vector3 ImageSeamCarver::CalculateGradientY(std::vector<Pixel*> pixelNeighbors)
{
	Vector3 vec;
	vec.X = pixelNeighbors[0]->computeR + pixelNeighbors[3]->computeR * 2 + pixelNeighbors[5]->computeR - pixelNeighbors[2]->computeR - pixelNeighbors[4]->computeR * 2 - pixelNeighbors[7]->computeR;
	vec.Y = pixelNeighbors[0]->computeG + pixelNeighbors[3]->computeG * 2 + pixelNeighbors[5]->computeG - pixelNeighbors[2]->computeG - pixelNeighbors[4]->computeG * 2 - pixelNeighbors[7]->computeG;
	vec.Z = pixelNeighbors[0]->computeB + pixelNeighbors[3]->computeB * 2 + pixelNeighbors[5]->computeB - pixelNeighbors[2]->computeB - pixelNeighbors[4]->computeB * 2 - pixelNeighbors[7]->computeB;
	return vec;
}

double ImageSeamCarver::GetEnergy(Vector3& gradX, Vector3& gradY)
{
	Vector3 SquaredSummed = gradX * gradX + gradY * gradY;
	return (std::sqrt(SquaredSummed.X) + std::sqrt(SquaredSummed.Y) + std::sqrt(SquaredSummed.Z)) / channelsNum;
}

std::vector<int> ImageSeamCarver::FindMinPath()
{
	std::vector<int> indexesToRemove(imgHeight);

	// Find value in top row with lowest cumulative energy 	
	double* minValue = std::min_element(cumulativeEnergyArray, cumulativeEnergyArray + imgWidth);
	size_t indexToRemove = minValue - cumulativeEnergyArray;
	indexesToRemove[0] = indexToRemove;

	for (int i = 1; i < imgHeight; ++i)
	{
		// Get index neighbors 
		int indexC = indexToRemove + originalImgWidth;
		int indexL = indexC - 1;
		int indexR = indexC + 1;

		int selectedIndex = -1;

		int leftMostIndex = i * originalImgWidth;
		int rightMostIndex = leftMostIndex + imgWidth - 1;

		// Right pixel border 
		if (indexToRemove == rightMostIndex)
		{
			selectedIndex = cumulativeEnergyArray[indexC] > cumulativeEnergyArray[indexL] ? indexL : indexC;
		}
		// Left pixel border 
		else if (indexToRemove == leftMostIndex)
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

std::vector<Pixel*> ImageSeamCarver::GetPixelNeighbors(const int indexX, const int indexY)
{
	std::vector<Pixel*> neighbors(NUM_PIXEL_NEIGHB);

	for (int i = 0; i < NUM_PIXEL_NEIGHB; ++i)
	{
		int nx = indexX + dx[i];
		int ny = indexY + dy[i];
		int neighborIndex = ny * originalImgWidth + nx;

		neighbors[i] = (nx >= 0 && nx < imgWidth && ny >= 0 && ny < imgHeight) ? &pixelArray[neighborIndex] : &pixelArray[indexY * originalImgWidth + indexX];
	}
	return neighbors;
}

void ImageSeamCarver::RemovePixelColumn()
{
	std::vector<int> indexesToRemove = FindMinPath();

	#pragma omp parallel for
	for (int i = 0; i < indexesToRemove.size(); ++i)
	{
		ShiftPixelArrayParallel(indexesToRemove[i], i * originalImgWidth + imgWidth - 1);
	}
#if USE_ENERGY_OPTIMIZATION

	// Update pixels that need update
	for (int i = 0; i < indexesToRemove.size(); ++i)
	{
		pixelArray[indexesToRemove[i]].bNeedsUpdate = true; 

		int x = indexesToRemove[i] % originalImgWidth;
		int y = indexesToRemove[i] / originalImgWidth;
		std::vector<Pixel*> Neighbors = GetPixelNeighbors(x,y); 
		for (Pixel* neighbor : Neighbors)
		{
			neighbor->bNeedsUpdate = true; 
		}
	}

#endif
}

void ImageSeamCarver::ShiftPixelArrayParallel(int startingIndex, int endingIndex)
{
	for (int i = startingIndex; i < endingIndex; ++i)
	{
		pixelArray[i] = pixelArray[i + 1];
#if USE_ENERGY_OPTIMIZATION
		pixelArray[i].bNeedsUpdate = true;
		energyArray[i] = energyArray[i + 1];
#endif
	}
}
