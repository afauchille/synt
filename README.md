#**PROJECT FAST MANTA**

Fast Manta is a **raytracer** software developed by four **EPITA** students in 2nd year.

**RAYTRACER?**

Fast Manta allows to synthesize pictures with a **3D rendering**. You just have to create a **.txt file** where you discribe your scene, put it in the program and run it. The raytracer will create and save on your computer the scene you discribed in your input file.

**INPUT FILE?**

Like it is exposed before, you have to "explain" what you want to synthesize in a **.txt file**. However, you can't just say *"I want some flowers here, and a house there, etc."*, you must write it in a **mathematical** way.

**HOW DOES IT WORK?**

Fast Manta allows to create lots of differents **geometrics shapes** like sphere, plan, cube, etc. All of them have to be discribed in a **precised way** otherwise the program **won't work**. Moreover, caracteristics of shapes have to be written in a **precised order**. Finally, you must write in the input file some others informations which are **essentials** for the program.

#**HOW TO WRITE AN INPUT FILE**

You have to **respect the order** of those instructions.

**OPTIONS**:

You have to tell how many cores you want to use, if you the image to be
anti-aliased and animated:

*OPTIONS (line break)  
CORES ANTI-ALIASING FPS SECONDS* 

**Example:**

*OPTIONS  
8 1 0 0*  

This will give you an image using 8 cores, not anti-aliased and not animated.

**DEVICE**

First of all, you have to put in your .txt file the **dimensions** of your picture. You have to do it in this way:

*DEVICE (line break)  
WIDTH HEIGHT (line break)* 

**Example :**

*DEVICE  
500 500*  

This one will give you in ouput an image with dimensions 500x500.

**CAMERA**

Now, you must discribe your camera. (XC, YC, ZC) is the **position** of your camera and (XDIR, YDIR, ZDIR) represents its **orientation**.

*CAMERA (line break)  
XC YC ZC XDIR YDIR ZDIR (line break)*  

**Example:**

*CAMERA  
0 3 -15 0 0 10*  

This camera is placed in (0, 3, -15) and looks in direction (0, 0, 10).

**Then**, you have to discribe all of the **shapes** you want to add in the scene.

**PLANE**

Create a plane with normal vector (XDIR, YDIR, ZDIR). You have to precise a point

**SPHERE**:

*SPHERE (line break)  
RADIUS X Y Z R G B*  

**PYRAMID**:

*PYRAMID (line break)  
X Y Z BASE_LENGTH HEIGHT R G B*  

**CUBE**:

*CUBE (line break)  
X Y Z LENGTH R G B*  

**TORUS**:

*TORUS (line break)  
X Y Z SMALL_RADIUS BIG_RADIUS R G B*  

**DIFFUSE**:
Diffuse is the light in the scenes.

*DIFFUSE (line break)  
X Y Z R G B*  

For the textures, you have to tell under each shape what you want. You can have
a wood texture, marble textures, miror and shapes with curves.
It is divided in two parts:
Part 1: NONE, WOOD, MARBLE1, MARBLE2, REFLE
Part 2: NONE, BM1, BM2, BM3, BM4, BM5, BM6

Therefore, under each shape, it will looks like:
*PART1 PART2*