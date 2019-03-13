# C++ Image Processing and Computer Vision OpenCV Teaching Examples

OpenCV C++ Image Processing and Computer Vision examples used for teaching over the years (2010-2013+).

All tested with [OpenCV](http://www.opencv.org) 3.1 (see also [4.0 branch](https://github.com/tobybreckon/cpp-examples-ipcv/tree/opencv4)) and GCC (Linux).

---

### Background:

If I taught you between 2010 and 2013 at [Cranfield University](http://www.cranfield.ac.uk) or [ESTIA](http://www.estia.fr) - these are the C++ examples from class.

Additionally used to generate the video examples within the ebook version of:

[Dictionary of Computer Vision and Image Processing](http://dx.doi.org/10.1002/9781119286462) (R.B. Fisher, T.P. Breckon, K. Dawson-Howe, A. Fitzgibbon, C. Robertson, E. Trucco, C.K.I. Williams), Wiley, 2014.
[[Google Books](http://books.google.co.uk/books?id=TaEQAgAAQBAJ&lpg=PP1&dq=isbn%3A1118706811&pg=PP1v=onepage&q&f=false)] [[doi](http://dx.doi.org/10.1002/9781119286462)]

---

### How to Build and run:

```
git clone https://github.com/tobybreckon/cpp-examples-ipcv.git
cd cpp-examples-ipcv
cmake .
make
./<insert executable name of one of the examples>
```

Demo source code is provided "as is" to aid your learning and understanding of topics on the course.

Most run with a webcam connected or from a command line supplied video file of a format OpenCV supports on your system (otherwise edit the script to provide your own image source).

N.B. you may need to change the line near the top that specifies the camera device to use on some examples below - change "0" if you have one webcam, I have it set to "1" to skip my built-in laptop webcam and use the connected USB camera.

---

### Reference:

Many of these techniques are fully explained in corresponding section of:

_Fundamentals of Digital Image Processing: A Practical Approach with Examples in Matlab_,
Chris J. Solomon and Toby P. Breckon, Wiley-Blackwell, 2010
ISBN: 0470844736, DOI:10.1002/9780470689776, http://www.fundipbook.com

```
bibtex:

@Book{solomonbreckon10fundamentals,
  author 	= 	 {Solomon, C.J. and Breckon, T.P.},
  title 	= 	 {Fundamentals of Digital Image Processing:
                                A Practical Approach with Examples in Matlab},
  publisher 	= 	 {Wiley-Blackwell},
  year 		= 	 {2010},
  isbn 		= {0470844736},
  doi 		= {10.1002/9780470689776},
  url 		= {http://www.fundipbook.com}
}
```

---

If you find any bugs report them to me (or better still submit a pull request) - toby.breckon@durham.ac.uk

_"may the source be with you"_ - anon.
