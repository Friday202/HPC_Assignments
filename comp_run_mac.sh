
clang -v -Xclang -fopenmp -std=c++17 -L/opt/homebrew/opt/libomp/lib -I/opt/homebrew/opt/libomp/include -lomp Image.cpp Main.cpp -lc++ -o run.out
./run.out /Images/720x480.png 128