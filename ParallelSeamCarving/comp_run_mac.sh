
clang -v -Xclang -fopenmp -std=c++17 -L/opt/homebrew/opt/libomp/lib -I/opt/homebrew/opt/libomp/include -lomp Main.cpp ImageSeamCarver.cpp -lc++ -o run.out
time ./run.out /Images/720x480.png 128

clang -v -Xclang -fopenmp -std=c++17 -L/opt/homebrew/opt/libomp/lib -I/opt/homebrew/opt/libomp/include -lomp Image_2d.cpp -lc++ -o run2.out
time ./run2.out 