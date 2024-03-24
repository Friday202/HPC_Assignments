
# clang -v -Xclang -fopenmp -std=c++17 -L/opt/homebrew/opt/libomp/lib -I/opt/homebrew/opt/libomp/include -lomp Image.cpp Main.cpp -lc++ -o run1.out
# ./run1.out /Images/720x480.png 200
clang -v -Xclang -fopenmp -std=c++17 -L/opt/homebrew/opt/libomp/lib -I/opt/homebrew/opt/libomp/include -lomp Image_2d.cpp -lc++ -o run2.out
./run2.out 