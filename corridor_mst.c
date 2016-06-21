/* 
---------------------------------------------------------------------------------------------------------
Solution to the 3rd Project of Fundamental Algorithms I0600
City College of New York, CUNY 

AUTHOR : ANKAN KUMAR GIRI

Email : agiri000@citymail.cuny.edu / ankangiri@gmail.com
------------------------------------------------------------------------------------------------------------
*/
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xos.h>
#include <X11/Xatom.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <stdint.h>

Display *display_ptr;
Screen *screen_ptr;
int screen_num;
char *display_name = NULL;
unsigned int display_width, display_height;

Window win;
int border_width;
unsigned int win_width, win_height;
int win_x, win_y;

XWMHints *wm_hints;
XClassHint *class_hints;
XSizeHints *size_hints;
XTextProperty win_name, icon_name;
char *win_name_string = "Example Window";
char *icon_name_string = "Icon for Example Window";

XEvent report;

GC gc, gc_red, gc_green, gc_grey, gc_blue;
unsigned long valuemask = 0;
XGCValues gc_values, gc_red_values, gc_green_values, gc_grey_values, gc_blue_values;
Colormap color_map;
XColor tmp_color1, tmp_color2;

struct Line
{
  int x1;
  int y1;
  int x2;
  int y2;   
} v_line[100],h_line[100],ver[100],hor[100],mst[200],ver_disp[100],hor_disp[100];
struct Pixel
{
  int name;
  int x;
  int y;
} pixel[1000];
int pixel_count = 0;
int v_line_count = 0;
int h_line_count = 0;
int ver_disp_count = 0;
int hor_disp_count = 0;
int mst_count = 0;
int cost[1000][1000];
int parent[1000];
int h_mst = 0;
int v_mst = 0;

void sort_h_line()
{
  int i,j,temp;
  for(i=0; i<h_line_count; i++)
  {
    for(j=0; j<h_line_count-1; j++)
    {
      if(h_line[j].y1>h_line[j+1].y1)
      {
        temp = h_line[j+1].y1;
        h_line[j+1].y1 = h_line[j+1].y2 = h_line[j].y1;
        h_line[j].y1 = h_line[j].y2 = temp;
        temp = h_line[j+1].x1;
        h_line[j+1].x1 = h_line[j].x1;
        h_line[j].x1 = temp;
        temp = h_line[j+1].x2;
        h_line[j+1].x2 = h_line[j].x2;
        h_line[j].x2 = temp;
      }
    }
  }
}

void sort_v_line()
{
  int i,j,temp;
  for(i=0; i<v_line_count; i++)
  {
    for(j=0; j<v_line_count-1; j++)
    {
      if(v_line[j].x1>v_line[j+1].x1)
      {
        temp = v_line[j+1].x1;
        v_line[j+1].x1 = v_line[j+1].x2 = v_line[j].x1;
        v_line[j].x1 = v_line[j].x2 = temp;
        temp = v_line[j+1].y1;
        v_line[j+1].y1 = v_line[j].y1;
        v_line[j].y1 = temp;
        temp = v_line[j+1].y2;
        v_line[j+1].y2 = v_line[j].y2;
        v_line[j].y2 = temp;
      }
    }
  }
}

int eculidian_dist(int x1, int y1, int x2, int y2)
{
  int a,b;
  a = pow(x1-x2,2);
  b = pow(y1-y2,2);
  return sqrt(a+b);
}

int find(int i)
{
  while(parent[i])
  i=parent[i];
  return i;
}

int uni(int i,int j)
{
  if(i!=j)
  {
    parent[j]=i;
    return 1;
  }
  return 0;
}

int get_name(int x, int y)
{
  int i;
  for(i=0;i<pixel_count;i++)
  {
    if(pixel[i].x == x && pixel[i].y == y)
    {
      return pixel[i].name;
    }
  }
  return -1;
}

int get_x(int name)
{
  int i;
  for(i=0;i<pixel_count;i++)
  {
    if(pixel[i].name == name)
    {
      return pixel[i].x;
    }
  }
  return -999;
}

int get_y(int name)
{
  int i;
  for(i=0;i<pixel_count;i++)
  {
    if(pixel[i].name == name)
    {
      return pixel[i].y;
    }
  }
  return -999;
}

int get_x_min(int x1,int x2)
{
  return (x1 > x2 ? x2 : x1) ;        
}

int get_y_min(int y1,int y2)
{
  return (y1 > y2 ? y2 : y1) ;
}

int get_x_max(int x1,int x2)
{
  return (x1 < x2 ? x2 : x1) ;
}

int get_y_max(int y1,int y2)
{
  return (y1 < y2 ? y2 : y1) ;
}

int64_t orientation(int a_x, int a_y, int b_x, int b_y, int c_x, int c_y)
{
  int64_t ori = (a_x * b_y) + (b_x * c_y) + (c_x * a_y) - (a_y * b_x) - (b_y * c_x) - (c_y * a_x);
  return ori;
}

int intersects1(int p_x, int p_y, int q_x, int q_y, int r_x, int r_y, int s_x, int s_y)
{
  int64_t ori_1,ori_2,ori_3,ori_4,ori_5,ori_6;
  ori_1 = orientation(p_x,p_y,q_x,q_y,r_x,r_y);
  ori_2 = orientation(p_x,p_y,q_x,q_y,s_x,s_y);
  ori_3 = orientation(r_x,r_y,s_x,s_y,p_x,p_y);
  ori_4 = orientation(r_x,r_y,s_x,s_y,q_x,q_y);
  ori_5 = ori_1*ori_2;
  ori_6 = ori_3*ori_4;
  if( ori_5 < 0 && ori_6 < 0)
  {
    return 1; 
  }
  return 0;
}

int intersects2(int p_x, int p_y, int q_x, int q_y, int r_x, int r_y, int s_x, int s_y)
{
  int intersect = 0;
  if(p_x == r_x && p_y == r_y)
  {
    intersect = 1; 
  }
  if(p_x == s_x && p_y == s_y)
  {
    intersect = 1; 
  }
  if(q_x == r_x && q_y == r_y)
  {
    intersect = 1; 
  }
  if(q_x == s_x && q_y == s_y)
  {
    intersect = 1; 
  }
  if(p_x == r_x && ((p_y >= r_y && p_y <= s_y) || (p_y >= s_y && p_y <= r_y)))
  {
    intersect = 1; 
  }
  if(q_x == r_x && ((q_y >= r_y && q_y <= s_y) || (q_y >= s_y && q_y <= r_y)))
  {
    intersect = 1; 
  }
  if(p_y == r_y && ((r_x >= p_x && r_x <= q_x) || (r_x >= q_x && r_x <= p_x)))
  {
    intersect = 1; 
  }
  if(p_y == s_y && ((s_x >= p_x && s_x <= q_x) || (s_x >= q_x && s_x <= p_x)))
  {
    intersect = 1; 
  }
  if(p_x == r_x && ((r_y >= p_y && r_y <= q_y) || (r_y >= q_y && r_y <= p_y)))
  {
    intersect = 1; 
  }
  if(p_x == s_x && ((s_y >= p_y && s_y <= q_y) || (s_y >= q_y && s_y <= p_y)))
  {
    intersect = 1; 
  }
  if(p_y == r_y && ((p_x >= r_x && p_x <= s_x) || (p_x >= s_x && p_x <= r_x)))
  {
    intersect = 1; 
  }
  if(q_y == r_y && ((q_x >= r_x && q_x <= s_x) || (q_x >= s_x && q_x <= r_x)))
  {
    intersect = 1; 
  }
  return intersect;
}

void sort(int a[], int n)
{
  int i,j, temp;
  for(i=0; i<n; i++)
  {
    for(j=0; j<n-1; j++)
    {
      if(a[j]>a[j+1])
      {
        temp = a[j+1];
        a[j+1] = a[j];
        a[j] = temp;
      }
    }
  }
}

void vertical_lines()
{
  int i,j,k;
  for(i=0;i<h_line_count-1;i++)
  {
    for(j=i+1;j<h_line_count;j++)
    {
      for(k=0;k<v_line_count;k++)
      {
        if((intersects1(h_line[i].x1,h_line[i].y1,h_line[i].x2,h_line[i].y2,v_line[k].x1,v_line[k].y1,v_line[k].x2,v_line[k].y2) || 
          intersects2(h_line[i].x1,h_line[i].y1,h_line[i].x2,h_line[i].y2,v_line[k].x1,v_line[k].y1,v_line[k].x2,v_line[k].y2)) &&  
          (intersects1(h_line[j].x1,h_line[j].y1,h_line[j].x2,h_line[j].y2,v_line[k].x1,v_line[k].y1,v_line[k].x2,v_line[k].y2) || 
          intersects2(h_line[j].x1,h_line[j].y1,h_line[j].x2,h_line[j].y2,v_line[k].x1,v_line[k].y1,v_line[k].x2,v_line[k].y2)))
        {
          if(ver[k].y1 == ver[k].y2)
            {
                ver[k].x1 = v_line[k].x1;
                ver[k].y1 = h_line[i].y1;
                ver[k].x2 = v_line[k].x1;
                ver[k].y2 = h_line[j].y1;
            }
            else
            {
              if(h_line[i].y1<ver[k].y1)
              {
                ver[k].y1 = h_line[i].y1;
              }
              if(h_line[j].y1>ver[k].y2)
              {
                ver[k].y2 = h_line[j].y1;
              }
            }
          break;
        }
      }
    }
  }
  for(i=0;i<v_line_count;i++)
  {
    if(ver[i].y1 != ver[i].y2)
    {
      ver_disp[ver_disp_count].x1 = ver[i].x1;
      ver_disp[ver_disp_count].y1 = ver[i].y1;
      ver_disp[ver_disp_count].x2 = ver[i].x2;
      ver_disp[ver_disp_count].y2 = ver[i].y2;
      ver_disp_count++;
      v_mst = v_mst + eculidian_dist(ver[i].x1,ver[i].y1,ver[i].x2,ver[i].y2);
    }
  }
}

void horizontal_lines()
{
  int i,j,k;
  for(i=0;i<v_line_count-1;i++)
  {
    for(j=i+1;j<v_line_count;j++)
    {
      for(k=0;k<h_line_count;k++)
      {
        if((intersects1(v_line[i].x1,v_line[i].y1,v_line[i].x2,v_line[i].y2,h_line[k].x1,h_line[k].y1,h_line[k].x2,h_line[k].y2) || 
          intersects2(v_line[i].x1,v_line[i].y1,v_line[i].x2,v_line[i].y2,h_line[k].x1,h_line[k].y1,h_line[k].x2,h_line[k].y2)) &&  
          (intersects1(v_line[j].x1,v_line[j].y1,v_line[j].x2,v_line[j].y2,h_line[k].x1,h_line[k].y1,h_line[k].x2,h_line[k].y2) || 
          intersects2(v_line[j].x1,v_line[j].y1,v_line[j].x2,v_line[j].y2,h_line[k].x1,h_line[k].y1,h_line[k].x2,h_line[k].y2)))
        {
          if(hor[k].x1 == hor[k].x2)
            {
                hor[k].x1 = v_line[i].x1;
                hor[k].y1 = h_line[k].y1;
                hor[k].x2 = v_line[j].x1;
                hor[k].y2 = h_line[k].y1;
            }
            else
            {
              if(v_line[i].x1<hor[k].x1)
              {
                hor[k].x1 = v_line[i].x1;
              }
              if(v_line[j].x1>hor[k].x2)
              {
                hor[k].x2 = v_line[j].x1;
              }
            }
          break;
        }
      }
    }
  }
  for(i=0;i<h_line_count;i++)
  {
    if(hor[i].x1 != hor[i].x2)
    {
      hor_disp[hor_disp_count].x1 = hor[i].x1;
      hor_disp[hor_disp_count].y1 = hor[i].y1;
      hor_disp[hor_disp_count].x2 = hor[i].x2;
      hor_disp[hor_disp_count].y2 = hor[i].y2;
      hor_disp_count++;
      h_mst = h_mst + eculidian_dist(hor[i].x1,hor[i].y1,hor[i].x2,hor[i].y2);
    }
  }
}

void kruskal()
{
  int i,j,min,a,u,b,v,x1,y1,x2,y2;
  int ne = 1;
  int mincost = 0;
  double ratio;
  while(ne < pixel_count - 1)
  {
    for(i=0,min=999;i<pixel_count;i++)
    {
      for(j=0;j<pixel_count;j++)
      {
        if(cost[i][j]<min)
        {
          min=cost[i][j];
          a=u=i;
          b=v=j;
        }
      }
    }
    u=find(u);
    v=find(v);
    if(uni(u,v))
    {
      ne++;
      x1=get_x(a);
      y1=get_y(a);
      x2=get_x(b);
      y2=get_y(b);
      mst[mst_count].x1 = x1;
      mst[mst_count].y1 = y1;
      mst[mst_count].x2 = x2;
      mst[mst_count].y2 = y2;
      mst_count++;
      mincost +=min;
    }
    cost[a][b]=cost[b][a]=999;
  }
  printf("\n\tMinimum cost MST = %d\n",mincost);
  vertical_lines();
  horizontal_lines();
  printf("\nVertical MST = %d.",v_mst);
  printf("\nHorizontal MST = %d.",h_mst);
  printf("\nTotal MST (Vertical MST + Horizontal MST)= %d.",v_mst+h_mst);
  ratio = (double) mincost/(v_mst+h_mst);
  printf("\nRatio of mincost MST to Total MST = %lf", ratio);
  ratio = (double) (v_mst+h_mst)/mincost;
  printf("\nRatio of Total MST to mincost MST = %lf", ratio);
  printf("\nClick to exit\n=========================\n");
}

int main(int argc, char **argv)
{
  FILE *fp;
  int x_1,y_1,x_2,y_2;
  int temp_x, temp_y, temp_z;
  char c1;
  int i,j;
  int v_x,h_y;
  int arr[100];
  int arr_count = 0;
  int nm1,nm2;

/* opening display: basic connection to X Server */
  if( (display_ptr = XOpenDisplay(display_name)) == NULL )
    { printf("Could not open display. \n"); exit(-1);}
  printf("Connected to X server  %s\n", XDisplayName(display_name) );
  screen_num = DefaultScreen( display_ptr );
  screen_ptr = DefaultScreenOfDisplay( display_ptr );
  color_map  = XDefaultColormap( display_ptr, screen_num );
  display_width  = DisplayWidth( display_ptr, screen_num );
  display_height = DisplayHeight( display_ptr, screen_num );

  printf("Width %d, Height %d, Screen Number %d\n", 
           display_width, display_height, screen_num);
  /* creating the window */
  border_width = 10;
  win_x = 0; win_y = 0;
  win_width = display_width;
  win_height = display_height; /*rectangular window*/
  
  win= XCreateSimpleWindow( display_ptr, RootWindow( display_ptr, screen_num),
                            win_x, win_y, win_width, win_height, border_width,
                            BlackPixel(display_ptr, screen_num),
                            WhitePixel(display_ptr, screen_num) );
  /* now try to put it on screen, this needs cooperation of window manager */
  size_hints = XAllocSizeHints();
  wm_hints = XAllocWMHints();
  class_hints = XAllocClassHint();
  if( size_hints == NULL || wm_hints == NULL || class_hints == NULL )
    { printf("Error allocating memory for hints. \n"); exit(-1);}

  size_hints -> flags = PPosition | PSize | PMinSize  ;
  size_hints -> min_width = 60;
  size_hints -> min_height = 60;

  XStringListToTextProperty( &win_name_string,1,&win_name);
  XStringListToTextProperty( &icon_name_string,1,&icon_name);
  
  wm_hints -> flags = StateHint | InputHint ;
  wm_hints -> initial_state = NormalState;
  wm_hints -> input = False;

  class_hints -> res_name = "x_use_example";
  class_hints -> res_class = "examples";

  XSetWMProperties( display_ptr, win, &win_name, &icon_name, argv, argc,
                    size_hints, wm_hints, class_hints );

  /* what events do we want to receive */
  XSelectInput( display_ptr, win, 
            ExposureMask | StructureNotifyMask | ButtonPressMask );
  
  /* finally: put window on screen */
  XMapWindow( display_ptr, win );

  XFlush(display_ptr);

  /* create graphics context, so that we may draw in this window */
  gc = XCreateGC( display_ptr, win, valuemask, &gc_values);
  XSetForeground( display_ptr, gc, BlackPixel( display_ptr, screen_num ) );
  XSetLineAttributes( display_ptr, gc, 8, LineSolid, CapRound, JoinRound);
  /* and three other graphics contexts, to draw in yellow and red and grey*/

  gc_red = XCreateGC( display_ptr, win, valuemask, &gc_red_values);  
  XSetLineAttributes( display_ptr, gc_red, 10, LineSolid, CapRound, JoinRound);
  if( XAllocNamedColor( display_ptr, color_map, "red", 
      &tmp_color1, &tmp_color2 ) == 0 )
    {printf("failed to get color red\n"); exit(-1);} 
  else
    XSetForeground( display_ptr, gc_red, tmp_color1.pixel );
  
  gc_green = XCreateGC( display_ptr, win, valuemask, &gc_green_values);
  XSetLineAttributes(display_ptr, gc_green, 2, LineSolid,CapRound, JoinRound);
  if( XAllocNamedColor( display_ptr, color_map, "green", 
      &tmp_color1, &tmp_color2 ) == 0 )
    {printf("failed to get color green\n"); exit(-1);} 
  else
    XSetForeground( display_ptr, gc_green, tmp_color1.pixel );
  
  gc_grey = XCreateGC( display_ptr, win, valuemask, &gc_grey_values);
  XSetLineAttributes(display_ptr, gc_grey, 8, LineSolid,CapRound, JoinRound);
  if( XAllocNamedColor( display_ptr, color_map, "grey", 
      &tmp_color1, &tmp_color2 ) == 0 )
    {printf("failed to get color grey\n"); exit(-1);} 
  else
    XSetForeground( display_ptr, gc_grey, tmp_color1.pixel );

  gc_blue = XCreateGC( display_ptr, win, valuemask, &gc_blue_values);
  XSetLineAttributes(display_ptr, gc_blue, 2, LineSolid,CapRound, JoinRound);
  if( XAllocNamedColor( display_ptr, color_map, "blue", 
      &tmp_color1, &tmp_color2 ) == 0 )
    {printf("failed to get blue yellow\n"); exit(-1);} 
  else
    XSetForeground( display_ptr, gc_blue, tmp_color1.pixel );
  
  if ( argc != 2 ) 
  {
      printf( "Usage: %s filename.extension \n", argv[0] );
      exit(-1);
  }
  fp = fopen(argv[1], "r");
  if (fp == NULL) 
  {
    fprintf(stderr, "Can't open file %s!\n", argv[1]);
    printf( "Usage: %s filename.extension or Check whether file is present or not\n", argv[0] );
    exit(-1);
  } 
  v_line_count = h_line_count = 0;
  while (fscanf(fp, "%c %d,%d,%d", &c1,&temp_x,&temp_y,&temp_z) != EOF) 
  {
    if(c1 == 'v')
    {
      x_1 = x_2 = temp_x;
      y_1 = temp_y;
      y_2 = temp_z;
      if(get_x_min(x_1,x_2) >= 0 && get_y_min(y_1,y_2) >= 0 && get_x_max(x_1,x_2) <= display_width - 300 && get_y_max(y_1,y_2) <= display_height - 200)
      {
        v_line[v_line_count].x1 = x_1;
        v_line[v_line_count].x2 = x_2;
        v_line[v_line_count].y1 = y_1;
        v_line[v_line_count].y2 = y_2;
        v_line_count++;
      }  
    }
    else if(c1 == 'h')
    {
      y_1 = y_2 = temp_x;
      x_1 = temp_y;
      x_2 = temp_z;
      if(get_x_min(x_1,x_2) >= 0 && get_y_min(y_1,y_2) >= 0 && get_x_max(x_1,x_2) <= display_width - 300 && get_y_max(y_1,y_2) <= display_height - 200)
      {
        h_line[h_line_count].x1 = x_1;
        h_line[h_line_count].x2 = x_2;
        h_line[h_line_count].y1 = y_1;
        h_line[h_line_count].y2 = y_2;
        h_line_count++;
      }
    }    
  }
  sort_h_line();
  sort_v_line();
  for(i=0;i<v_line_count;i++)
  {
    ver[i].y1=ver[i].y2=0;
  }
  for(i=0;i<h_line_count;i++)
  {
    hor[i].x1=hor[i].x2=0;
  }
  for(i=0;i<1000;i++)
  {
    for(j=0;j<1000;j++)
    {
      cost[i][j]=999;
    }
  }
  for(i=0;i<v_line_count;i++)
  {
    for(j=0;j<h_line_count;j++)
    {
      if(intersects1(v_line[i].x1,v_line[i].y1,v_line[i].x2,v_line[i].y2,h_line[j].x1,h_line[j].y1,h_line[j].x2,h_line[j].y2))
      {
        v_x = v_line[i].x1;
        h_y = h_line[j].y1;
        pixel[pixel_count].x = v_x;
        pixel[pixel_count].y = h_y;
        pixel[pixel_count].name = pixel_count;
        pixel_count++;
      }
      if((v_line[i].x1 == h_line[j].x1 && h_line[j].y1 >= v_line[i].y1 && h_line[j].y1 <= v_line[i].y2) || (v_line[i].x1 == h_line[j].x2 && h_line[j].y1 >= v_line[i].y1 && h_line[j].y1 <= v_line[i].y2))
      {
       v_x = v_line[i].x1;
        h_y = h_line[j].y1;
        pixel[pixel_count].x = v_x;
        pixel[pixel_count].y = h_y;
        pixel[pixel_count].name = pixel_count;
        pixel_count++;
      }
      if((v_line[i].y1 == h_line[j].y1 && v_line[i].x1 >= h_line[j].x1 && v_line[i].x1 <= h_line[j].x2) || (v_line[i].y2 == h_line[j].y1 && v_line[i].x1 >= h_line[j].x1 && v_line[i].x1 <= h_line[j].x2))
      {
        v_x = v_line[i].x1;
        h_y = h_line[j].y1;
        pixel[pixel_count].x = v_x;
        pixel[pixel_count].y = h_y;
        pixel[pixel_count].name = pixel_count;
        pixel_count++;
      }
    }
  }
  for(i=0;i<v_line_count;i++)
  {
    arr_count = 0;
    for(j=0;j<pixel_count;j++)
    {
      if(v_line[i].x1 == pixel[j].x && pixel[j].y >= v_line[i].y1 && pixel[j].y <= v_line[i].y2)
      {
        arr[arr_count] = pixel[j].y;
        arr_count++;
      }
    }
    if(arr_count>1)
    {
        sort(arr,arr_count);
        for(j=0;j<arr_count-1;j++)
        {
          //XDrawLine(display_ptr, win, gc_blue, v_line[i].x1, arr[j], v_line[i].x1, arr[j+1]);
          nm1 = get_name(v_line[i].x1, arr[j]);
          nm2 = get_name(v_line[i].x1, arr[j+1]);
          cost[nm1][nm2]=cost[nm2][nm1]=eculidian_dist(v_line[i].x1, arr[j],v_line[i].x1, arr[j+1]);
        }
    }
  }
  for(i=0;i<h_line_count;i++)
  {
    arr_count = 0;
    for(j=0;j<pixel_count;j++)
    {
      if(h_line[i].y1 == pixel[j].y && pixel[j].x >= h_line[i].x1 && pixel[j].x <= h_line[i].x2)
      {
        arr[arr_count] = pixel[j].x;
        arr_count++;
      }
    }
    if(arr_count>1)
    {
        sort(arr,arr_count);
        for(j=0;j<arr_count-1;j++)
        {
          //XDrawLine(display_ptr, win, gc_blue, arr[j], h_line[i].y1, arr[j+1], h_line[i].y1);
          nm1 = get_name(arr[j], h_line[i].y1);
          nm2 = get_name(arr[j+1], h_line[i].y1);
          cost[nm1][nm2]=cost[nm2][nm1]=eculidian_dist(arr[j], h_line[i].y1,arr[j+1], h_line[i].y1);
        }
    }
  }
  kruskal();

  while(1)
  { 
    XNextEvent( display_ptr, &report );
    switch( report.type )
    {
    case Expose:
    {
      for(i=0;i<v_line_count;i++)
      {
        XDrawLine(display_ptr, win, gc_grey, v_line[i].x1, v_line[i].y1, v_line[i].x2, v_line[i].y2);
      }
      for(i=0;i<h_line_count;i++)
      {
        XDrawLine(display_ptr, win, gc_grey, h_line[i].x1, h_line[i].y1, h_line[i].x2, h_line[i].y2);
      }
      for(i=0;i<mst_count;i++)
      {
        XDrawLine(display_ptr, win, gc_red, mst[i].x1, mst[i].y1, mst[i].x2, mst[i].y2);
      }
      for(i=0;i<hor_disp_count;i++)
      {
        XDrawLine(display_ptr, win, gc_green, hor_disp[i].x1, hor_disp[i].y1, hor_disp[i].x2, hor_disp[i].y2);
      }   
      for(i=0;i<ver_disp_count;i++)
      {
        XDrawLine(display_ptr, win, gc_blue, ver_disp[i].x1, ver_disp[i].y1, ver_disp[i].x2, ver_disp[i].y2);
      }     
      break;
    }
    case ConfigureNotify:
            /* This event happens when the user changes the size of the window*/
            win_width = report.xconfigure.width;
            win_height = report.xconfigure.height;
            break;
    case ButtonPress:
    {  
      XFlush(display_ptr);
      XCloseDisplay(display_ptr);
      exit(0);
    }
      break;
    default:
      break;    
    }
  }
  exit(0);
}