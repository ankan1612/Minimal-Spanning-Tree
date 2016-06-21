# Minimal-Spanning-Tree
###XLIB Project
The aim of this project is to approximate the shortest tree that joins all segments of a given arrangement of 
horizontal and vertical segments. 

The arrangement will be given as an input ﬁle (as command line argument), with lines of the format:
```sh
v x, y1, y2
```
for a vertical line going from (x,y1) to (x,y2), and 
```sh
h y, x1, x2 
```
for a horizontal line going from (x1,y) to (x2,y).

Your tree uses only edges along these segments.

Computing this tree is known to be NP-complete, so you are asked to create your own approximation algorithm. For comparison, you have to compute the following lower bound: you deﬁne a graph Gh with the horizontal segments as vertices, and join two of these vertices by an edge if they are intersected by the same vertical segment; the length of this edge is their distance along the segment. Similar you deﬁne a graph Gv on the vertical segments. Then the length of the minimum spanning tree of Gh is a lower bound for the length of vertical connections any solution must have, and the length of the minimum spanning tree of Gv is a lower bound for the length of horizontal connections any solution must have; their sum is a lower bound for the total length of any solution. You compute both minimum spanning trees with any MST algorithm. Then you show on the screen (using xlib) the set of segments, your solution in red, and (with smaller width, so they don’t interfere with the lines of your solution) sets of segment pieces corresponding to the horizontal MST in green and the vertical MST in blue. Also you print the length of your solution, and the lower bound, and their ration on the screen.

***Choose the line widths and colors in such a way that all types of edges are visible.***

###Compilation

```sh
gcc corridor_mst.c -lX11 -lm -L/usr/X11R6/lib -o corridor_mst.out
```
Run as command line argument using
```sh
./corridor_mst.out inputfilename.extension
```
inputfilename.extension is a file containing vertical and horizontal lines with v denoting vertical
lines and h denoting horizontal lines in the following format :
>h y, x1, x2

>v x, y1, y2

>and so on................

Each point of a line should be within the minimum and maximum range to be displayed full as 
a line.
* Minimum x value : 0
* Minimum y value : 0
* Maximum x value : Your Max Screen Widthness - 300
* Maximum y value : Your Max Screen Height - 200

***User can left mouse click or right mouse click to exit.***


