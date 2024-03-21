#pragma once

#include <string>
#include <vector>
#include <iostream>
#include <chrono>

#define TIMER_START \
    auto start_time = std::chrono::high_resolution_clock::now();

#define TIMER_END \
    auto end_time = std::chrono::high_resolution_clock::now(); \
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time); \
    std::cout << "Time taken: " << duration.count() << " milliseconds" << std::endl;

#define NUM_PIXEL_NEIGHB 8
#define NUM_DESIRED_CH 0

#define DEBUG_MODE 1
#define REALLY_BIG_NUM 1E99

/* Struct holds pixel values and energy values for displaying images. But it also holds doubles for calculation (not nice but works faster then runtime casting) */
struct Pixel
{	
	Pixel() = default; 
	Pixel(uint8_t  red, uint8_t  green, uint8_t  blue, uint8_t  alpha) : R(red), G(green), B(blue), A(alpha), 
		computeR(red), computeG(green), computeB(blue) {}

	uint8_t  R;
	uint8_t  G;
	uint8_t  B;
	uint8_t  A;

	uint8_t Energy = 0;
	uint8_t CumulativeEnergy = 0;

	double  computeR;
	double  computeG;
	double  computeB;

	// Call this on all pixels after removing first pixel column 
	void Reset()
	{
		computeR = R; 
		computeG = G;
		computeB = B;
	}

	Pixel operator +(const Pixel& other) const
	{
		return Pixel(this->computeR + other.computeR, this->computeG + other.computeG, this->computeB + other.computeB, this->A);
	}

	Pixel operator -(const Pixel& other) const
	{
		return Pixel(this->computeR - other.computeR, this->computeG - other.computeG, this->computeB - other.computeB, this->A);
	}

	Pixel operator *(const int& other) const
	{
		return Pixel(this->computeR * other, this->computeG * other, this->computeB * other, this->A);
	}
	
	Pixel operator * (const Pixel& other) const
	{
		return Pixel(this->computeR * other.computeR, this->computeG * other.computeG, this->computeB * other.computeB, this->A);
	}
};


class Image
{
public:
	Image(std::string filename, int numOfPixels);	
	~Image(); 

	/* C++ load image from file wrapper for C stlib */
	void LoadImage(std::string filename, int& outWidth, int& outHeight, int& outChannelNum, int& outArraySize);

	/* C++ write image to file wrapper for C stlib */
	void WriteImage(std::string filename);

	/* Displays basic image information */
	void DisplayInformation(); 

	/* Calculate and save gradient image - debug only */
	void ShowGradientImage(std::string filename);

	/* Calculate and save cumulative energy image - debug only */
	void ShowCumulativeEnergyImage(std::string filename); 

	void RemoveSeam();

private:
	void CalculateGradient(); 
	void CalculateCumulativeEnergy(); 
	
	Pixel CalculateGradientX(std::vector<Pixel> pixelNeighbors);
	Pixel CalculateGradientY(std::vector<Pixel> pixelNeighbors);	

	double CalculateEnergy(Pixel& gradX, Pixel& gradY);

	std::vector<Pixel> GetPixelNeighbors(const int index);

	// Call after removing pixel column 
	void ResetPixelValues(); 

	// Helper function for showing debug images 
	void WriteImageDebug(std::string filename, double* forArray);
	
	std::vector<int> FindMinPath();
	void RemovePixel(int atIndex, Pixel* newArray, int startingIndex, int endingIndex);

private:
	int imgWidth = 0;
	int imgHeight = 0;
	int channelsNum = 0;

	int arraySize = 0; 
	const int numOfPixels; 

	// Pointers to dynamic arrays 
	Pixel* pixelArray = nullptr;
	double* energyArray = nullptr; 
	double* cumulativeEnergyArray = nullptr;

	// Neighbor arrays - read only 
	static const int dx[NUM_PIXEL_NEIGHB];
	static const int dy[NUM_PIXEL_NEIGHB];
};

