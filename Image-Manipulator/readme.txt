Lucas O'Dowd-Jones
Week 9 Lists Extention

For my extention this week I developed a simple command line based image processing program.
I have implemented the following effects:
	flipX		Flip around central x axis
	flipY		Flip around central y axis
	brighten x 	Brighten by x% (x is an integer in the range 0 - 100)
	darken x 	Darken by x% (x is an integer in the range 0 - 100)
	greyscale	Converts the image into greyscale representation
	invert 		Invert colours
	blur x 		Performs a mean blur over the image with strength x (x is an integer in the range 0 - 100)
	edges 		Performs sobel edge detection a greyscale version of the image

Effects can be "chained" together (i.e. exectuted sequentially) for more complex effects in a single execution of the program

The simplest widely used image file type is bitmap so I have chosen to develop my program to manipulate only bitmaps to avoid issues with compression common to other popular image filetypes as this exceeds the scope of thte project.

All input is validated, including ensuring the file is suitable for proccesing and user input is syntatically correct

SYNTAX:
$./image [INPUT FILE NAME] [NEW FILE NAME] [EFFECT 1] [EFFECT 2] ...
Proccesses a single image ([INPUT FILE NAME]) and writes the proccessed image file to [NEW FILE NAME]

Example program call
$./image example.bmp newimage.bmp blur 3 greyscale darken 40

$./image
Runs automated testing of effect operations

I have included 2 example bitmaps to test the program with. 


