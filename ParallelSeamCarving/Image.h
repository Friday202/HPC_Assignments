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

	double  computeR;
	double  computeG;
	double  computeB;

	// Call this on all pixels after removing pixel column 
	void Reset()
	{
		computeR = R; 
		computeG = G;
		computeB = B;
	}
};

struct Vector3
{
	Vector3() = default; 
	Vector3(double X, double Y, double Z) : X(X), Y(Y), Z(Z) {}
	double X, Y, Z; 

	Vector3 operator * (const Vector3& other) const
	{
		return Vector3(this->X * other.X, this->Y * other.Y, this->Z * other.Z);
	}

	Vector3 operator +(const Vector3& other) const
	{
		return Vector3(this->X + other.X, this->Y + other.Y, this->Z + other.Z);
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

	/* Main function to remove semas */
	void RemoveSeam();

private:
	void CalculateEnergy(); 
	void CalculateCumulativeEnergy(); 
	
	Vector3 CalculateGradientX(std::vector<Pixel> pixelNeighbors);
	Vector3 CalculateGradientY(std::vector<Pixel> pixelNeighbors);

	double GetEnergy(Vector3& gradX, Vector3& gradY);
	std::vector<int> FindMinPath();

	/* Returns vector (array) of 8 neighbouring pixels */
	std::vector<Pixel> GetPixelNeighbors(const int index);

	/* Call after removing pixel column */
	void ResetPixelValues(); 
	  
	/* Helper function for showing debug images */
	void WriteImageDebug(std::string filename, double* forArray);

	/* Shifts all pixes after index in pixel array*/
	void ShiftPixelArray(int index, int size);

private:
	int imgWidth = 0;
	int imgHeight = 0;
	int channelsNum = 0;

	int arraySize = 0; 

	// User-specified number of pixel columns to remove from input image
	const int numOfPixels; 

	// Pointers to dynamic arrays 
	Pixel* pixelArray = nullptr;
	double* energyArray = nullptr; 
	double* cumulativeEnergyArray = nullptr;

	// Neighbor arrays - read only 
	static const int dx[NUM_PIXEL_NEIGHB];
	static const int dy[NUM_PIXEL_NEIGHB];
};

