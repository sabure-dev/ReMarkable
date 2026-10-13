#include "../BMP-info/BMPClass.h"


//example of code to change one pixel!!!
/*
you just can include this code to all paths in LAB
*/
int main(){
    BMPImage img;
    bool a = img.read("example.bmp");
    img.change_pixel(100,100,20,20,20);
    img.save("alloc.bmp");
}