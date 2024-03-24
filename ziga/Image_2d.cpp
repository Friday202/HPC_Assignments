#include "Image.h"

#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION

#define TIMER_START \
    auto start_time = std::chrono::high_resolution_clock::now();

#define TIMER_SECTION(section) \
    std::cout << "Time taken for " << section << " :" << std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - start_time).count() << " milliseconds" << std::endl; \
	start_time = std::chrono::high_resolution_clock::now();	

#include "stb_image.h"
#include "stb_image_write.h"
#include <algorithm>
#include <omp.h>

Pixel** loadImage(const std::string& filename, int& imgWidth, int& imgHeight, int& channelsNum)
{
    uint8_t* loadedImage = stbi_load(filename.c_str(), &imgWidth, &imgHeight, &channelsNum, STBI_rgb_alpha);

    // Check if the image was loaded successfully
    if (loadedImage == nullptr) {
        std::cerr << "Error: Image not loaded" << std::endl;
        return nullptr; // Return nullptr to indicate failure
    }

    // Allocate memory for the pixel array
    Pixel** pixelArray = new Pixel*[imgHeight];
    for (int i = 0; i < imgHeight; ++i) {
        pixelArray[i] = new Pixel[imgWidth];
    }

    // Copy pixel data from loaded image to pixelArray
    #pragma omp parallel for
    for (int i = 0; i < imgHeight; ++i) {
        for (int j = 0; j < imgWidth; ++j) {
            int pixelIndex = (i * imgWidth + j) * 4;
            Pixel pixel(loadedImage[pixelIndex], loadedImage[pixelIndex + 1], loadedImage[pixelIndex + 2], channelsNum == 4 ? loadedImage[pixelIndex + 3] : 255);
            pixelArray[i][j] = pixel;
        }
    }

    stbi_image_free(loadedImage); // Free the loaded image memory

    return pixelArray; // Return the dynamically allocated pixelArray
}

void writeImage(const std::string& filename, Pixel** pixelArray , int imgWidth, int imgHeight, int channelsNum)
{
    // Allocate memory for the image data
    uint8_t* imageData = new uint8_t[imgWidth * imgHeight * channelsNum];

    // Copy pixel data from pixelArray to imageData
    #pragma omp parallel for
    for (int i = 0; i < imgHeight; ++i) {
        for (int j = 0; j < imgWidth; ++j) {
            int pixelIndex = (i * imgWidth + j) * channelsNum;
            imageData[pixelIndex] = pixelArray[i][j].R;
            imageData[pixelIndex + 1] = pixelArray[i][j].G;
            imageData[pixelIndex + 2] = pixelArray[i][j].B;
            imageData[pixelIndex + 3] = channelsNum == 4 ? pixelArray[i][j].A : 255;
        }
    }

    // Write the image to file
    stbi_write_png(filename.c_str(), imgWidth, imgHeight, channelsNum, imageData, imgWidth * channelsNum);

    delete[] imageData; // Free the dynamically allocated memory
}

void writeImageGray(const std::string& filename, int** pixelArray, int imgWidth, int imgHeight)
{
    // Allocate memory for the image data
    uint8_t* imageData = new uint8_t[imgWidth * imgHeight];

    // Copy pixel data from pixelArray to imageData
    #pragma omp parallel for
    for (int i = 0; i < imgHeight; ++i) {
        for (int j = 0; j < imgWidth; ++j) {
            imageData[i * imgWidth + j] = pixelArray[i][j];
        }
    }

    // Write the image to file
    stbi_write_png(filename.c_str(), imgWidth, imgHeight, 1, imageData, imgWidth);

    delete[] imageData; // Free the dynamically allocated memory
}

const int dx[NUM_PIXEL_NEIGHB] = { -1, 0, 1, -1, 1, -1, 0, 1 };
const int dy[NUM_PIXEL_NEIGHB] = { -1, -1, -1, 0, 0, 1, 1, 1 };
std::vector<Pixel> getPixelNeighbors(Pixel** pixelArray, int x, int y, int imgWidth, int imgHeight)
{
    std::vector<Pixel> neighbors(8);
    for (int i = 0; i < NUM_PIXEL_NEIGHB; ++i)
    {
        int nx = x + dx[i];
        int ny = y + dy[i];
        neighbors[i] = (nx >= 0 && nx < imgWidth && ny >= 0 && ny < imgHeight) ? pixelArray[ny][nx] : pixelArray[y][x];
    }
    return neighbors;
}

unsigned short** calculateEnergy(Pixel** pixelArray, int imgWidth, int imgHeight, int channelsNum) {
    unsigned short** energyArray = new unsigned short*[imgHeight];
    for (int i = 0; i < imgHeight; ++i) {
        energyArray[i] = new unsigned short[imgWidth]();
    }

    for (int y = 0; y < imgHeight; ++y) {
        #pragma omp parallel for num_threads(32)
        for (int x = 0; x < imgWidth; ++x) {
            std::vector<Pixel> pixelNeighbors = getPixelNeighbors(pixelArray, x, y, imgWidth, imgHeight);

            int GxR = pixelNeighbors[7].computeR - pixelNeighbors[0].computeR - pixelNeighbors[1].computeR * 2 - pixelNeighbors[2].computeR + pixelNeighbors[5].computeR + pixelNeighbors[6].computeR * 2;
            int GxG = pixelNeighbors[7].computeG - pixelNeighbors[0].computeG - pixelNeighbors[1].computeG * 2 - pixelNeighbors[2].computeG + pixelNeighbors[5].computeG + pixelNeighbors[6].computeG * 2;
            int GxB = pixelNeighbors[7].computeB - pixelNeighbors[0].computeB - pixelNeighbors[1].computeB * 2 - pixelNeighbors[2].computeB + pixelNeighbors[5].computeB + pixelNeighbors[6].computeB * 2;

            int GyR = pixelNeighbors[0].computeR + pixelNeighbors[3].computeR * 2 + pixelNeighbors[5].computeR - pixelNeighbors[2].computeR - pixelNeighbors[4].computeR * 2 - pixelNeighbors[7].computeR;
            int GyG = pixelNeighbors[0].computeG + pixelNeighbors[3].computeG * 2 + pixelNeighbors[5].computeG - pixelNeighbors[2].computeG - pixelNeighbors[4].computeG * 2 - pixelNeighbors[7].computeG;
            int GyB = pixelNeighbors[0].computeB + pixelNeighbors[3].computeB * 2 + pixelNeighbors[5].computeB - pixelNeighbors[2].computeB - pixelNeighbors[4].computeB * 2 - pixelNeighbors[7].computeB;

            energyArray[y][x] = (sqrt(GxR * GxR + GyR * GyR) + sqrt(GxG * GxG + GyG * GyG) + sqrt(GxB * GxB + GyB * GyB) / 3);
        }
    }
    

    return energyArray;
}


int** calculateCumulativeEnergy(unsigned short** pixelArray, int imgWidth, int imgHeight, int channelsNum) {
    int** energyArray = new int*[imgHeight];
    for (int i = 0; i < imgHeight; ++i) {
        energyArray[i] = new int[imgWidth]();
    }
    // Copy the last row of the energy array
    #pragma omp parallel for
    for (int x = 0; x < imgWidth; ++x) {
        energyArray[imgHeight - 1][x] = pixelArray[imgHeight - 1][x];
    }

    // Calculate the cumulative energy from bottom to top
    for (int y = imgHeight - 2; y >= 0; --y) {
        //fill the pixels between the first and last pixel of the row
        #pragma omp parallel for
        for (int x = 1; x < imgWidth - 1; ++x) {
            int minEnergy = energyArray[y + 1][x - 1];
            int temp = std::min(minEnergy, energyArray[y + 1][x]);
            minEnergy = std::min(temp, energyArray[y + 1][x + 1]);
            energyArray[y][x] = pixelArray[y][x] + minEnergy;
        }
        //fill the first and last pixel of the row
        energyArray[y][0] = pixelArray[y][0] + std::min(energyArray[y + 1][0], energyArray[y + 1][1]);
        energyArray[y][imgWidth - 1] = pixelArray[y][imgWidth - 1] + std::min(energyArray[y + 1][imgWidth - 1], energyArray[y + 1][imgWidth - 2]);
    }

    return energyArray;
}

unsigned short* findSeamBasic(int** cumulativeEnergyArray, int imgWidth, int imgHeight) {
    unsigned short* seam = new unsigned short[imgHeight];
    int minEnergy = cumulativeEnergyArray[0][0];
    int minEnergyIndex = 0;
    for (int x = 0; x < imgWidth; ++x) {
        if (cumulativeEnergyArray[0][x] < minEnergy) {
            minEnergy = cumulativeEnergyArray[0][x];
            minEnergyIndex = x;
        }
    }
    seam[0] = minEnergyIndex;

    for (int y = 1; y < imgHeight; ++y) {
        //choose neighboring pixel with minimum energy
        int x = seam[y - 1];
        int minEnergy = cumulativeEnergyArray[y][x - 1];
        if (cumulativeEnergyArray[y - 1][x] < minEnergy) {
            minEnergy = cumulativeEnergyArray[y - 1][x];
            x = x;
        }
        if (cumulativeEnergyArray[y - 1][x + 1] < minEnergy) {
            x = x + 1;
        }
        seam[y] = x;
    }

    return seam;
}

void removeSeam(Pixel** pixelArray, unsigned short* seam, int* imgWidth, int imgHeight) {
    // Remove the seam from the image
    #pragma omp parallel for
    for (int y = 0; y < imgHeight; ++y) {
        for (int x = seam[y]; x < *imgWidth - 1; ++x) {
            pixelArray[y][x] = pixelArray[y][x + 1];
        }
    }

    // Decrease the image width
    (*imgWidth) -= 1;
}



int main() {
    
    // imput arguments
    // std::string filename = "Images/4x5";
    // std::string filename = "Images/20x10";
    // std::string filename = "Images/720x480";
    std::string filename = "Images/7680x4320";
    int numOfPixels = 1;

    // Load the image
    int imgWidth;
    int imgHeight;
    int channelsNum;
    Pixel** pixelArray = loadImage(filename + ".png", imgWidth, imgHeight, channelsNum);
    TIMER_START;
    for (int i = 0; i < numOfPixels; ++i) {
        // calculate the energy of the image
        unsigned short** energyArray = calculateEnergy(pixelArray, imgWidth, imgHeight, channelsNum);
        TIMER_SECTION("Energy calculation");
        //if i == 0
        if (i == 0) {
        // Cast energy array to int
        int** intEnergyArray = new int*[imgHeight];
        for (int j = 0; j < imgHeight; ++j) {
            intEnergyArray[j] = new int[imgWidth]();
        }
        for (int y = 0; y < imgHeight; ++y) {
            for (int x = 0; x < imgWidth; ++x) {
                intEnergyArray[y][x] = static_cast<int>(energyArray[y][x]);
            }
        }
        writeImageGray(filename + "_Energy.png", intEnergyArray, imgWidth, imgHeight);
        }

    

        //calculate the cumulative energy of the image
        int** cumulativeEnergyArray = calculateCumulativeEnergy(energyArray, imgWidth, imgHeight, channelsNum);
        TIMER_SECTION("Cumulative energy calculation");
        // // print maximum energy
        // int maxEnergy = 0;
        // for (int x = 0; x < imgWidth; ++x) {
        // maxEnergy = std::max(maxEnergy, cumulativeEnergyArray[imgHeight - 1][x]);
        // }
        
        // //if i == 0
        // if (i == 0) {
        //     writeImageGray(filename + "_CumulativeEnergy.png", cumulativeEnergyArray, imgWidth, imgHeight);
        // }
        unsigned short* seam = findSeamBasic(cumulativeEnergyArray, imgWidth, imgHeight);

        // remove the seam from the image
        removeSeam(pixelArray, seam, &imgWidth, imgHeight);
        TIMER_SECTION("Seam removal");

        //release memory
        for (int i = 0; i < imgHeight; ++i) {
            delete[] energyArray[i];
            delete[] cumulativeEnergyArray[i];
        }
        delete[] energyArray;
        delete[] cumulativeEnergyArray;
        delete[] seam;
        
    }


    TIMER_SECTION("before write image");
    // write image to file
    writeImage(filename + "_out.png", pixelArray, imgWidth, imgHeight, channelsNum);
    TIMER_SECTION("after write image");


    return 0;
}
