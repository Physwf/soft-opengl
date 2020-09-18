# soft-opengl
An soft implementation of opengles 2.0. 

The goal of this project is to help myself to understand the hardware graphic pipeline. It is base on a series of [blog articles](https://fgiesen.wordpress.com/2013/02/08/triangle-rasterization-in-practice/) .

## v0.1 release notes

Currently only part of the features of the opengles 2.0 have been implemented:  
1.input assemable  
2.vertex shader  
3.primivite assemable, include culling and clipping(only in xy plane)  
4.rasteriazaion(points,line list, triangle list)  
5.pixel shader, only 2d texture sample supported, and event that was not fully testfied.  
~~6.output merge(not implemented yet!)~~

## demo 
Cat:
![image](https://github.com/Physwf/resources/blob/master/screenshot/lowpolycat.png)

Car:
![image](https://github.com/Physwf/resources/blob/master/screenshot/Lamborghini.png)

## samples usage
1.git clone https://github.com/Physwf/soft-opengl.git
2.cd soft-opengl
3.makdir build
4.cmake ..
5.git clone https://github.com/Physwf/resources.git
6.copy "Lamborginhi Aventador OBJ" "lowpolycat" resources to soft-opengl working directory
7.compile "samples" and run... (comment code in winmain to select model to render)
