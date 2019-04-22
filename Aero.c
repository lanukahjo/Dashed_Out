#include <stdio.h>                
#include <GL/glut.h>
#include <math.h> 
#include <time.h>
#include <string.h>
//------------------------------------------------------------
//Global Variables
int control = 0;            //controls what is displayed on the screen in a broad sense / state variable
int control_buffer = 0;     //just stores the control value while the game is paused
float screen_height = 680;   
float screen_width = 1250;
float p_x = 0.0f;           // present coordinates of the plane
float p_y = -300.0f;
float v_x = 0.0f;           //components of velocity in x and y direction
float v_y = 1.0f;
float velocity = 1.5f;      //magnitude of plane's velocity
float theta = 1.5708;       //angle with the horizonatal 
float trail_x1[153];        //storing the two trails(previously traversed) cooardinates to be displayed 
float trail_y1[153];
float trail_x2[153];
float trail_y2[153];
float exit_x1 = 0.0f;       //stores the continuously changing exit area
float exit_y1 = 0.0f;
float exit_x2 = 0.0f;
float exit_y2 = 0.0f;
int change_exit_x = 2;      //rate at which the exit is changing
int level = 1;              //level in the game
int collision = 0;          //to record collision
float plane_xs[27];         //to  store world coordinates of the plane
float plane_ys[27];
int win_flag = 0;           //to denote that a level has been won
int cheat = 0;              //cheat code XXDD
int score = 0;              //to store the score
int pause_c = 0;            //pause value
//---------------------------------------------------
//Functions 
void displayText(float x,float y,char *string);    //render text to screen
void resetGame();                                  //resets the values of the global variables
void initialiseExit();                             //create exit area
void createObstacles();                            //randomly create obstacles
void updateObstacles();                           //move obstacles
void drawObstacles();                             //draw obstacles on screen
void drawPlane();                                 //draw the plane
void updateExitPoint();                           //update exit area
void drawBorder();                                //draw the border of arena
void updateTrail();                               //update the train coordinates in the array
void checkCollision();                            //checks for collisions
void checkVictory();                              //checks if a level has been won
void drawTrail();                                 // draws trail
void move_in_dir(void);                           //function to collect and call other functions in right order
void updateVelocity();                            //update velocity on key press
void updateTheta(int id);                         //update angle on key press
void play();
void pause();
//------------------------------------------------------------------------
//Structure to hold details of each obstacle
typedef struct Obstacle
{
        //set of coordinates of corner points of the obstacle
	int x[4]; 
        int y[4];
        int layout; //determining the direction of motion
        float vel;  //random velocity assigned to each obstacle
}obstacle;
obstacle *list[80];  //to hold the obstacle structures

int obstacle_count = 0; //number of obstacles
void resetGame()
{   
     // reseting all necessary global variables
     p_x = 0.0f;
     p_y = -300.0f;
     v_x = 0.0f;
     v_y = 1.0f;
     velocity = 1.5f;
     theta = 1.5708;
     change_exit_x = 2;
     win_flag = 0;
     cheat = 0;
     collision = 0;
     for(int i= 0; i < 153 ; i++)
     {
         trail_x1[i] = 2000;
         trail_y1[i] = 2000;
         trail_x2[i] = 2000;
         trail_y2[i] = 2000;
     }
}
void initialiseExit()
{
     //setting up exit
     exit_x1 = screen_width/2;
     exit_y1 = screen_height/2;
     exit_x2 = screen_width/2 - 76;
     exit_y2 = screen_height/2;
}
void createObstacles()
{
     obstacle_count = 10 + level*2;  //obstacle count is level dependent
     int obs_width = level + 5;
     if(obs_width > 9)
         obs_width = 9;
     int max_obs_vel = level + 5;
     if(max_obs_vel > 7)
         max_obs_vel = 7;
     srand(time(NULL));             //obstacle loctaion, size and velocity is random
     for(int i=0 ; i<obstacle_count ; i++)
     {
	list[i] = (obstacle *)malloc(sizeof(obstacle));
        list[i]->layout = rand()%6;
        if(list[i]->layout == 0 || list[i]->layout == 4)
        {
             list[i]->y[0] = rand()%500 - 170;
             list[i]->x[0] = -625;
             list[i]->y[1] = list[i]->y[0] - obs_width;
             list[i]->x[1] = -625; 
             list[i]->y[2] = list[i]->y[0] - obs_width;
             list[i]->x[2] = -625 + (rand()%150 + 50); 
             list[i]->y[3] = list[i]->y[0];
             list[i]->x[3] = list[i]->x[2]; 
             list[i]->vel = rand()%max_obs_vel + 1;
        }
	else if(list[i]->layout == 1 || list[i]->layout == 5)
	{
	     list[i]->y[0] = rand()%500 - 170;
             list[i]->x[0] = 625;
             list[i]->y[1] = list[i]->y[0] - obs_width;
             list[i]->x[1] = 625; 
             list[i]->y[2] = list[i]->y[0] - obs_width;
             list[i]->x[2] = 625 - (rand()%150 + 50); 
             list[i]->y[3] = list[i]->y[0];
             list[i]->x[3] = list[i]->x[2];
             list[i]->vel = -(rand()%max_obs_vel + 1);
	}
        else if(list[i]->layout == 2)
	{
	     list[i]->x[0] = (rand()%2 == 0?-1:1) * (rand()%525 + 100);
             list[i]->y[0] = 340;
             list[i]->x[1] = list[i]->x[0];
             list[i]->y[1] = 340 - (rand()%100 + 50); 
             list[i]->x[2] = list[i]->x[0] + obs_width;
             list[i]->y[2] = list[i]->y[1]; 
             list[i]->x[3] = list[i]->x[2];
             list[i]->y[3] = list[i]->y[0];
             list[i]->vel = -(rand()%max_obs_vel + 1);
	}
        else
	{
             list[i]->x[0] = (rand()%2 == 0?-1:1) * (rand()%525 + 100);
             list[i]->y[0] = -340;
             list[i]->x[1] = list[i]->x[0];
             list[i]->y[1] = -340 + (rand()%100 + 50); 
             list[i]->x[2] = list[i]->x[0] + obs_width;
             list[i]->y[2] = list[i]->y[1]; 
             list[i]->x[3] = list[i]->x[2];
             list[i]->y[3] = list[i]->y[0];
             list[i]->vel = (rand()%max_obs_vel + 1);
	}
     }
}
void updateObstacles()
{
     //obstacle positions are updated
     //if the obstacle touch the boundary then their direction is reversed
     //speed may change after reversal
     int max_obs_vel = 7;
     for(int i=0 ; i<obstacle_count ; i++)
     {
        if(list[i]->layout == 0 || list[i]->layout == 4)
        {
             list[i]->x[0] += list[i]->vel;
             list[i]->x[1] += list[i]->vel;
             list[i]->x[2] += list[i]->vel;
             list[i]->x[3] += list[i]->vel;
             if(list[i]->x[2] >= 625)
             {
                 list[i]->vel = (list[i]->vel>0?-1:1) * (rand()%max_obs_vel + 1);
             }
             if(list[i]->x[0] <= -625)
             {
                 list[i]->vel = (list[i]->vel>0?-1:1) * (rand()%max_obs_vel + 1);
             }
        }
	else if(list[i]->layout == 1 || list[i]->layout == 5)
	{
	     list[i]->x[0] += list[i]->vel;
             list[i]->x[1] += list[i]->vel;
             list[i]->x[2] += list[i]->vel;
             list[i]->x[3] += list[i]->vel;
             if(list[i]->x[2] <= -625)
             {
                 list[i]->vel = (list[i]->vel>0?-1:1) * (rand()%max_obs_vel + 1);
             }
             if(list[i]->x[0] >= 625)
             {
                 list[i]->vel = (list[i]->vel>0?-1:1) * (rand()%max_obs_vel + 1);
             }
	}
        else if(list[i]->layout == 2)
	{
	     list[i]->y[0] += list[i]->vel;
             list[i]->y[1] += list[i]->vel;
             list[i]->y[2] += list[i]->vel;
             list[i]->y[3] += list[i]->vel;
             if(list[i]->y[1] <= -340)
             {
                 list[i]->vel = (list[i]->vel>0?-1:1) * (rand()%max_obs_vel + 1);
             }
             if(list[i]->y[0] >= 340)
             {
                 list[i]->vel = (list[i]->vel>0?-1:1) * (rand()%max_obs_vel + 1);
             }
	}
        else
	{
             list[i]->y[0] += list[i]->vel;
             list[i]->y[1] += list[i]->vel;
             list[i]->y[2] += list[i]->vel;
             list[i]->y[3] += list[i]->vel;
             if(list[i]->y[1] >= 340)
             {
                 list[i]->vel = (list[i]->vel>0?-1:1) * (rand()%max_obs_vel + 1);
             }
             if(list[i]->y[0] <= -340)
             {
                 list[i]->vel = (list[i]->vel>0?-1:1) * (rand()%max_obs_vel + 1);
             }
	}
     }
}
void drawObstacles()
{
     glColor3f(0.564f,0.286f,0.286f);
     for(int i = 0; i<obstacle_count; i++)
     {
        glBegin(GL_POLYGON);
        for(int j=0;j<4;j++)
        {
            glVertex2i(list[i]->x[j],list[i]->y[j]);
        }
        glEnd();
     }
     glFlush(); 
     glColor3f(1.0f,0.0f,0.0f);
}
void drawPlane()
{
    glColor3f(0.909f,0.05f,0.05f);
    glBegin(GL_POLYGON);
    //FuseLage
    glVertex2f(21*cos(theta)-0*sin(theta)+p_x , 21*sin(theta)+0*cos(theta)+p_y);
    glVertex2f(19*cos(theta)-2*sin(theta)+p_x , 19*sin(theta)+2*cos(theta)+p_y);
    glVertex2f(18*cos(theta)-4*sin(theta)+p_x , 18*sin(theta)+4*cos(theta)+p_y);
    glVertex2f(17*cos(theta)-4.5*sin(theta)+p_x , 17*sin(theta)+4.5*cos(theta)+p_y);
    glVertex2f(-5*cos(theta)-4*sin(theta)+p_x , -5*sin(theta)+4*cos(theta)+p_y);
    glVertex2f(-5*cos(theta)+4*sin(theta)+p_x , -5*sin(theta)-4*cos(theta)+p_y);
    glVertex2f(17*cos(theta)+4.5*sin(theta)+p_x , 17*sin(theta)-4.5*cos(theta)+p_y);
    glVertex2f(18*cos(theta)+4*sin(theta)+p_x , 18*sin(theta)-4*cos(theta)+p_y);
    glVertex2f(19*cos(theta)+2*sin(theta)+p_x , 19*sin(theta)-2*cos(theta)+p_y);
    //------------------------------------------------------------------------------
    plane_xs[0] = 21*cos(theta)-0*sin(theta)+p_x; 
    plane_ys[0] = 21*sin(theta)+0*cos(theta)+p_y;
    plane_xs[1] = 19*cos(theta)-2*sin(theta)+p_x; 
    plane_ys[1] = 19*sin(theta)+2*cos(theta)+p_y;
    plane_xs[2] = 18*cos(theta)-4*sin(theta)+p_x; 
    plane_ys[2] = 18*sin(theta)+4*cos(theta)+p_y;
    plane_xs[3] = 17*cos(theta)-4.5*sin(theta)+p_x; 
    plane_ys[3] = 17*sin(theta)+4.5*cos(theta)+p_y;
    plane_xs[4] = -5*cos(theta)-4*sin(theta)+p_x; 
    plane_ys[4] = -5*sin(theta)+4*cos(theta)+p_y;
    plane_xs[5] = -5*cos(theta)+4*sin(theta)+p_x; 
    plane_ys[5] = -5*sin(theta)-4*cos(theta)+p_y;
    plane_xs[6] = 17*cos(theta)+4.5*sin(theta)+p_x; 
    plane_ys[6] = 17*sin(theta)-4.5*cos(theta)+p_y;
    plane_xs[7] = 18*cos(theta)+4*sin(theta)+p_x; 
    plane_ys[7] = 17*sin(theta)-4*cos(theta)+p_y;
    plane_xs[8] = 19*cos(theta)+2*sin(theta)+p_x; 
    plane_ys[8] = 19*sin(theta)-2*cos(theta)+p_y;
    glEnd();
    //Right Wing
    glBegin(GL_POLYGON);
    glVertex2f(13*cos(theta)-4.5*sin(theta)+p_x , 13*sin(theta)+4.5*cos(theta)+p_y);
    glVertex2f(10*cos(theta)-2*10.5*sin(theta)+p_x , 10*sin(theta)+2*10.5*cos(theta)+p_y);
    glVertex2f(6*cos(theta)-2*10*sin(theta)+p_x , 6*sin(theta)+2*10*cos(theta)+p_y);
    glVertex2f(8*cos(theta)-4.2*sin(theta)+p_x , 8*sin(theta)+4.2*cos(theta)+p_y);
    //--------------------------------------------------------------------------------
    plane_xs[9] = 13*cos(theta)-4.5*sin(theta)+p_x; 
    plane_ys[9] = 13*sin(theta)+4.5*cos(theta)+p_y;
    plane_xs[10] = 10*cos(theta)-2*10.5*sin(theta)+p_x; 
    plane_ys[10] = 10*sin(theta)+2*10.5*cos(theta)+p_y;
    plane_xs[11] = 6*cos(theta)-2*10*sin(theta)+p_x; 
    plane_ys[11] = 6*sin(theta)+2*10*cos(theta)+p_y;
    plane_xs[12] = 8*cos(theta)-4.2*sin(theta)+p_x; 
    plane_ys[12] = 8*sin(theta)+4.2*cos(theta)+p_y;
    glEnd();
    //Left Wing
    glBegin(GL_POLYGON);
    glVertex2f(13*cos(theta)+4.5*sin(theta)+p_x , 13*sin(theta)-4.5*cos(theta)+p_y);
    glVertex2f(10*cos(theta)+2*10.5*sin(theta)+p_x , 10*sin(theta)-2*10.5*cos(theta)+p_y);
    glVertex2f(6*cos(theta)+2*10*sin(theta)+p_x , 6*sin(theta)-2*10*cos(theta)+p_y);
    glVertex2f(8*cos(theta)+4.2*sin(theta)+p_x , 8*sin(theta)-4.2*cos(theta)+p_y);
    //-----------------------------------------------------------------------------------
    plane_xs[13] = 13*cos(theta)+4.5*sin(theta)+p_x; 
    plane_ys[13] = 13*sin(theta)-4.5*cos(theta)+p_y;
    plane_xs[14] = 10*cos(theta)+2*10.5*sin(theta)+p_x; 
    plane_ys[14] = 10*sin(theta)-2*10.5*cos(theta)+p_y;
    plane_xs[15] = 6*cos(theta)+2*10.5*sin(theta)+p_x; 
    plane_ys[15] = 6*sin(theta)-2*10.5*cos(theta)+p_y;
    plane_xs[16] = 8*cos(theta)+4.2*sin(theta)+p_x; 
    plane_ys[16] = 8*sin(theta)-4.2*cos(theta)+p_y;
    glEnd();
    //Tail
    glBegin(GL_POLYGON);
    glVertex2f(-5*cos(theta)-4*sin(theta)+p_x , -5*sin(theta)+4*cos(theta)+p_y);
    glVertex2f(-7*cos(theta)-9*sin(theta)+p_x , -7*sin(theta)+9*cos(theta)+p_y);
    glVertex2f(-11*cos(theta)-7*sin(theta)+p_x , -11*sin(theta)+7*cos(theta)+p_y);
    glVertex2f(-10*cos(theta)-0*sin(theta)+p_x , -10*sin(theta)+0*cos(theta)+p_y);
    glVertex2f(-5*cos(theta)-0*sin(theta)+p_x , -5*sin(theta)+0*cos(theta)+p_y);
    //----------------------------------------------------------------------------------
    plane_xs[17] = -5*cos(theta)-4*sin(theta)+p_x; 
    plane_ys[17] = -5*sin(theta)+4*cos(theta)+p_y;
    plane_xs[18] = -7*cos(theta)-9*sin(theta)+p_x; 
    plane_ys[18] = -7*sin(theta)+9*cos(theta)+p_y;
    plane_xs[19] = -11*cos(theta)-7*sin(theta)+p_x; 
    plane_ys[19] = -11*sin(theta)+7*cos(theta)+p_y;
    plane_xs[20] = -10*cos(theta)-0*sin(theta)+p_x; 
    plane_ys[20] = -10*sin(theta)+0*cos(theta)+p_y;
    plane_xs[21] = -5*cos(theta)-0*sin(theta)+p_x; 
    plane_ys[21] = -5*sin(theta)+0*cos(theta)+p_y;
    glEnd();
    glBegin(GL_POLYGON);
    glVertex2f(-5*cos(theta)+4*sin(theta)+p_x , -5*sin(theta)-4*cos(theta)+p_y);
    glVertex2f(-7*cos(theta)+9*sin(theta)+p_x , -7*sin(theta)-9*cos(theta)+p_y);
    glVertex2f(-11*cos(theta)+7*sin(theta)+p_x , -11*sin(theta)-7*cos(theta)+p_y);
    glVertex2f(-10*cos(theta)+0*sin(theta)+p_x , -10*sin(theta)-0*cos(theta)+p_y);
    glVertex2f(-5*cos(theta)-0*sin(theta)+p_x , -5*sin(theta)+0*cos(theta)+p_y);
    //------------------------------------------------------------------------------------
    plane_xs[22] = -5*cos(theta)+4*sin(theta)+p_x; 
    plane_ys[22] = -5*sin(theta)-4*cos(theta)+p_y;
    plane_xs[23] = -7*cos(theta)+9*sin(theta)+p_x; 
    plane_ys[23] = -7*sin(theta)-9*cos(theta)+p_y;
    plane_xs[24] = -11*cos(theta)+7*sin(theta)+p_x; 
    plane_ys[24] = -11*sin(theta)-7*cos(theta)+p_y;
    plane_xs[25] = -10*cos(theta)+0*sin(theta)+p_x; 
    plane_ys[25] = -10*sin(theta)-0*cos(theta)+p_y;
    plane_xs[26] = -5*cos(theta)-0*sin(theta)+p_x; 
    plane_ys[26] = -5*sin(theta)+0*cos(theta)+p_y;
    glEnd();
    glFlush();
    glColor3f(1.0f,0.0f,0.0f);
}
void updateExitPoint()
{
    if(exit_x2 == -screen_width/2)
	change_exit_x = -2;
    if(exit_x1 == screen_width/2)
	change_exit_x = 2;
    exit_x1 -= change_exit_x;
    exit_x2 -= change_exit_x;
}
void drawBorder()
{
    glColor3f(0.717f,0.388f,0.133f);
    //Top Border
    glBegin(GL_POLYGON);
    glVertex2i(-screen_width/2,screen_height/2);
    glVertex2i(exit_x2,screen_height/2);
    glVertex2i(exit_x2,screen_height/2 - 5);
    glVertex2i(-screen_width/2,screen_height/2 - 5);
    glEnd();
    glBegin(GL_POLYGON);
    glVertex2i(screen_width/2,screen_height/2);
    glVertex2i(exit_x1,screen_height/2);
    glVertex2i(exit_x1,screen_height/2 - 5);
    glVertex2i(screen_width/2,screen_height/2 - 5);
    glEnd();
    //Left Border
    glBegin(GL_POLYGON);
    glVertex2i(screen_width/2,screen_height/2);
    glVertex2i(screen_width/2,-screen_height/2);
    glVertex2i(screen_width/2 - 5,-screen_height/2);
    glVertex2i(screen_width/2 - 5,screen_height/2);
    glEnd();
    //Right Border
    glBegin(GL_POLYGON);
    glVertex2i(-screen_width/2,screen_height/2);
    glVertex2i(-screen_width/2,-screen_height/2);
    glVertex2i(-screen_width/2 + 5,-screen_height/2);
    glVertex2i(-screen_width/2 + 5,screen_height/2);
    glEnd();
    //Bottom Border
    glBegin(GL_POLYGON);
    glVertex2i(-screen_width/2,-screen_height/2);
    glVertex2i(screen_width/2,-screen_height/2);
    glVertex2i(screen_width/2,-screen_height/2 + 5);
    glVertex2i(-screen_width/2,-screen_height/2 + 5);
    glEnd();
    glFlush();
    glColor3f(1.0f,0.0f,0.0f);
}
void updateTrail()
{
    //153 previous positions are stored. Rest are popped out giving a disappearing effect
    for(int i=152;i>0;i--)
    {
        trail_x1[i] = trail_x1[i-1];
        trail_y1[i] = trail_y1[i-1];
        trail_x2[i] = trail_x2[i-1];
        trail_y2[i] = trail_y2[i-1];
    }
    trail_x1[0] = 10*cos(theta)-10*sin(theta) + p_x;
    trail_y1[0] = 10*sin(theta)+10*cos(theta) + p_y;
    trail_x2[0] = 10*cos(theta)+10*sin(theta) + p_x;
    trail_y2[0] = 10*sin(theta)-10*cos(theta) + p_y;
}
void checkCollision()
{
    //checking for rectangular collision. All the points on the plane-polygon are examined if they fall inside a rectangle or not
    int flag = 0;
    //This part checks the collision with borders
    for(int i = 0; i<27 ; i++)
    {
	if(plane_xs[i]<=-screen_width/2+5 && plane_xs[i]>=-screen_width/2)
        {
            flag = 1;
            break;
        }
        else if(plane_xs[i]>=screen_width/2-5 && plane_xs[i]<=screen_width/2)
        {
            flag = 1;
            break;
        }
        else if(plane_ys[i]<=-screen_height/2+5 && plane_ys[i]>=-screen_height/2)
        {
            flag = 1;
            break;
        }
        else if(plane_ys[i]>=screen_height/2-5 && plane_ys[i]<=screen_height/2)
        {
	    if(plane_xs[i]<=exit_x2 || plane_xs[i]>=exit_x1)
            {
                 flag = 1;
                 break;
            }
        }
    }
    //--------------------------------------------------------------------------
    //This part checks the collision with the moving obstacles
    for(int i=0;i < obstacle_count ; i++)
    {
	for(int j=0; j<27 ;j++)
        {
            if(list[i]->layout == 0 || list[i]->layout == 4)
            {
                if(plane_xs[j]>=list[i]->x[0] && plane_xs[j]<=list[i]->x[3] && plane_ys[j]>=list[i]->y[1] && plane_ys[j]<=list[i]->y[0])
                {    
                    flag = 1;
                    break;
                }
            }
	    else if(list[i]->layout == 1 || list[i]->layout == 5)
	    {
	         if(plane_xs[j]>=list[i]->x[2] && plane_xs[j]<=list[i]->x[1] && plane_ys[j]>=list[i]->y[1] && plane_ys[j]<=list[i]->y[0])
                {    
                    flag = 1;
                    break;
                }    
	    }
            else if(list[i]->layout == 2)
	    {
	        if(plane_xs[j]>=list[i]->x[0] && plane_xs[j]<=list[i]->x[3] && plane_ys[j]>=list[i]->y[1] && plane_ys[j]<=list[i]->y[0])
                {    
                    flag = 1;
                    break;
                } 
            }
            else
            {
                if(plane_xs[j]>=list[i]->x[1] && plane_xs[j]<=list[i]->x[2] && plane_ys[j]>=list[i]->y[0] && plane_ys[j]<=list[i]->y[1])
                {    
                    flag = 1;
                    break;
                }
	    }
        }
    }
    //----------------------------------------------------------------------------------------
    if(cheat == 1)
        flag == 0;
    if(flag == 1)
        collision = 1;
}
void checkVictory()
{
     //checking that all part of plane has crossed the screen without collison
     int flag = 1;
     for(int i=0; i<27 ;i++)
     {
         flag = flag && (plane_ys[i] > 341);
     }
     if(flag == 1)
         win_flag = 1;
}
void displayText(float x ,float y,char *stringToDisplay) 
{
        //renders the text to the desired coordinates
	int length;
	glRasterPos2f(x, y);
        length = strlen(stringToDisplay);
	for(int i=0 ;i<length ;i++)
        {
		glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24 ,stringToDisplay[i]);
	}
}
void drawTrail()
{
    //draws the trail leaving a smoke like pattern
    glPointSize(3.0);
    glBegin(GL_POINTS);
    glColor3f(0.784f,0.784f,0.784f);
    for(int i = 0; i<153 ; i++)
    {
        glVertex2f(trail_x1[i],trail_y1[i]);
        glVertex2f(trail_x2[i],trail_y2[i]);
    }
    glEnd();
    glFlush();
    glColor3f(1.0f,0.0f,0.0f);
}
void move_in_dir(void)
{
    //this function is continuously called and is majorly responsible for what is displayed on screen.
    if(control == 1)
    {
        //This is the first screen of the game
        glClear(GL_COLOR_BUFFER_BIT);
        drawBorder();
        updateExitPoint();
        srand(time(NULL));
        //---------------------------------------------------------------------------------------
        //This loop generates 30 planes of random colour at 30 random locations.
        int count = 0;
        while(count<30)
        {
            int loc_x = rand()%1100 - 550;
            int loc_y = rand()%600 - 300;
            float loc_theta = rand()%60 * 0.1;
            if((loc_x > -250 && loc_x<350) && (loc_y > -150 && loc_y < 150))
                  continue;
            glColor3f(0.1f*(rand()%11),0.1f*(rand()%11),0.1f*(rand()%11));
            glBegin(GL_POLYGON);
    	    //FuseLage
            glVertex2f(21*cos(loc_theta)-0*sin(loc_theta)+loc_x , 21*sin(loc_theta)+0*cos(loc_theta)+loc_y);
            glVertex2f(19*cos(loc_theta)-2*sin(loc_theta)+loc_x , 19*sin(loc_theta)+2*cos(loc_theta)+loc_y);
            glVertex2f(18*cos(loc_theta)-4*sin(loc_theta)+loc_x , 18*sin(loc_theta)+4*cos(loc_theta)+loc_y);
            glVertex2f(17*cos(loc_theta)-4.5*sin(loc_theta)+loc_x , 17*sin(loc_theta)+4.5*cos(loc_theta)+loc_y);
            glVertex2f(-5*cos(loc_theta)-4*sin(loc_theta)+loc_x , -5*sin(loc_theta)+4*cos(loc_theta)+loc_y);
            glVertex2f(-5*cos(loc_theta)+4*sin(loc_theta)+loc_x , -5*sin(loc_theta)-4*cos(loc_theta)+loc_y);
            glVertex2f(17*cos(loc_theta)+4.5*sin(loc_theta)+loc_x , 17*sin(loc_theta)-4.5*cos(loc_theta)+loc_y);
            glVertex2f(18*cos(loc_theta)+4*sin(loc_theta)+loc_x , 18*sin(loc_theta)-4*cos(loc_theta)+loc_y);
            glVertex2f(19*cos(loc_theta)+2*sin(loc_theta)+loc_x , 19*sin(loc_theta)-2*cos(loc_theta)+loc_y);
            glEnd();
            //Right Wing
            glBegin(GL_POLYGON);
            glVertex2f(13*cos(loc_theta)-4.5*sin(loc_theta)+loc_x , 13*sin(loc_theta)+4.5*cos(loc_theta)+loc_y);
            glVertex2f(10*cos(loc_theta)-2*10.5*sin(loc_theta)+loc_x , 10*sin(loc_theta)+2*10.5*cos(loc_theta)+loc_y);
            glVertex2f(6*cos(loc_theta)-2*10*sin(loc_theta)+loc_x , 6*sin(loc_theta)+2*10*cos(loc_theta)+loc_y);
            glVertex2f(8*cos(loc_theta)-4.2*sin(loc_theta)+loc_x , 8*sin(loc_theta)+4.2*cos(loc_theta)+loc_y);
            glEnd();
            //Left Wing
            glBegin(GL_POLYGON);
            glVertex2f(13*cos(loc_theta)+4.5*sin(loc_theta)+loc_x , 13*sin(loc_theta)-4.5*cos(loc_theta)+loc_y);
            glVertex2f(10*cos(loc_theta)+2*10.5*sin(loc_theta)+loc_x , 10*sin(loc_theta)-2*10.5*cos(loc_theta)+loc_y);
            glVertex2f(6*cos(loc_theta)+2*10*sin(loc_theta)+loc_x , 6*sin(loc_theta)-2*10*cos(loc_theta)+loc_y);
            glVertex2f(8*cos(loc_theta)+4.2*sin(loc_theta)+loc_x , 8*sin(loc_theta)-4.2*cos(loc_theta)+loc_y);
            glEnd();
            //Tail
            glBegin(GL_POLYGON);
            glVertex2f(-5*cos(loc_theta)-4*sin(loc_theta)+loc_x , -5*sin(loc_theta)+4*cos(loc_theta)+loc_y);
            glVertex2f(-7*cos(loc_theta)-9*sin(loc_theta)+loc_x , -7*sin(loc_theta)+9*cos(loc_theta)+loc_y);
            glVertex2f(-11*cos(loc_theta)-7*sin(loc_theta)+loc_x , -11*sin(loc_theta)+7*cos(loc_theta)+loc_y);
            glVertex2f(-10*cos(loc_theta)-0*sin(loc_theta)+loc_x , -10*sin(loc_theta)+0*cos(loc_theta)+loc_y);
            glVertex2f(-5*cos(loc_theta)-0*sin(loc_theta)+loc_x , -5*sin(loc_theta)+0*cos(loc_theta)+loc_y);
            glEnd();
            glBegin(GL_POLYGON);
            glVertex2f(-5*cos(loc_theta)+4*sin(loc_theta)+loc_x , -5*sin(loc_theta)-4*cos(loc_theta)+loc_y);
            glVertex2f(-7*cos(loc_theta)+9*sin(loc_theta)+loc_x , -7*sin(loc_theta)-9*cos(loc_theta)+loc_y);
            glVertex2f(-11*cos(loc_theta)+7*sin(loc_theta)+loc_x , -11*sin(loc_theta)-7*cos(loc_theta)+loc_y);
            glVertex2f(-10*cos(loc_theta)+0*sin(loc_theta)+loc_x , -10*sin(loc_theta)-0*cos(loc_theta)+loc_y);
            glVertex2f(-5*cos(loc_theta)-0*sin(loc_theta)+loc_x , -5*sin(loc_theta)+0*cos(loc_theta)+loc_y);
            glEnd();
            glFlush();
            count++;
            glColor3f(1.0f,0.0f,0.0f);
        }
        //---------------------------------------------------------------------------------------------
        //This part contains instructions and controls guide
        glColor3f(0.768f,0.858f,0.937f);
        glBegin(GL_POLYGON);
        glVertex2i(-250,150);
        glVertex2i(350,150);
        glVertex2i(350,-150);
        glVertex2i(-250,-150);
        glEnd();
        glColor3f(1.0f,0.0f,0.0f);
        displayText(-200.0,105.0,">Instructions To Play:-");
        displayText(-200.0,75.0,"1. Press A to turn left");
        displayText(-200.0,50.0,"2. Press D to turn right");
        displayText(-200.0,25.0,"3. Press W to increase speed");
        displayText(-200.0,0.0,"4. Press S to decrease speed");
        displayText(-200.0,-25.0,"5. Press P to pause game");
        displayText(-200.0,-50.0,"6. Press Q to resume game");
        displayText(-200.0,-75.0,"Avoid all the obstacles and Reach the exit at the top");
        displayText(-200.0,-110.0,"Press Y to start game");
        //-----------------------------------------------------------------------------------------------
        displayText(490.0,-310.0,"Kunal Ojha");
        glutSwapBuffers();
        glFlush();
    }
    if(collision == 0 && win_flag == 0 && control>1)
    {
        //This is the main playing arena.
        //Called only if collision and victory are not detected
    	glClear(GL_COLOR_BUFFER_BIT);
    	drawPlane();
    	updateTrail();
    	drawTrail();
    	drawBorder();
    	drawObstacles();
        checkCollision();
        checkVictory();
    	updateObstacles();
    	updateExitPoint();
        displayText(530.0,300.0,"Level:");
        char lev[3];
        if(level<=9)
        {
           lev[0] = '0' + level;
           lev[1] = '\0';
        }
        else
        {
           lev[0] = '0' + (level/10);
           lev[1] = '0' + (level%10);
           lev[2] = '\0';
        }
        displayText(590.0,300.0,lev);
    	glutSwapBuffers();
    	glFlush();
        p_x = p_x + v_x;
        p_y = p_y + v_y;
    }
    if(collision == 1 && control>1)
    {
        //if a collision is detected. Then its a "Game Over"
	glColor3f(0.768f,0.858f,0.937f);
        glBegin(GL_POLYGON);
        glVertex2i(-80,90);
        glVertex2i(180,90);
        glVertex2i(180,-15);
        glVertex2i(-80,-15);
        glEnd();
        glColor3f(1.0f,0.0f,0.0f);
        displayText(-70.0,50.0,"Sorry!! Game Over");
        displayText(-70.0,25.0,"Score:");
        char sc[5];
        if(score < 10)
        {
            sc[0] = '0';
            sc[1] = '\0';
        }
        else if(score < 100) 
        {
             sc[0] = '0' + (score/10);
             sc[1] = '0';
             sc[2] = '\0';
        }
        else if(score < 1000)
        {
             sc[0] = '0' + (score/100);
             sc[1] = '0' + (score - (score/100)*100)/10;
             sc[2] = '0';
             sc[3] = '\0';   
        }
        else 
        {
             sc[0] = '0' + (score/1000);
             sc[1] = '0' + (score - (score/1000)*1000)/100;
             sc[2] = '0' + (score - (score/100)*100)/10;
             sc[3] = '0';
             sc[4] = '\0';
        }
        displayText(0.0,25.0,sc);
        displayText(-70.0,0.0,"Press R to restart Game");
        glutSwapBuffers();
    	glFlush();
        collision = 2;
    }
    if(win_flag == 1 && control>1)
    {
        //if a victory is detected then there are two possibilities
        char sc[5];
        if(level == 35)
        {
             //if all levels are cleared then game is complete
             score += (level*10);
             glColor3f(0.768f,0.858f,0.937f);
             glBegin(GL_POLYGON);
             glVertex2i(-200,100);
             glVertex2i(200,100);
             glVertex2i(200,-30);
             glVertex2i(-200,-30);
             glEnd();
             glColor3f(1.0f,0.0f,0.0f);
             displayText(-60.0,50.0,"Game Cleared");
             displayText(-60.0,25.0,"Score:");
             if(score < 10)
             {
                 sc[0] = '0';
                 sc[1] = '\0';
             }
             else if(score < 100)
             {
                 sc[0] = '0' + (score/10);
                 sc[1] = '0';
                 sc[2] = '\0';
             }
             else if(score < 1000)
             {
                 sc[0] = '0' + (score/100);
                 sc[1] = '0' + (score - (score/100)*100)/10;
                 sc[2] = '0';
                 sc[3] = '\0';   
             }
             else 
             {
                  sc[0] = '0' + (score/1000);
                  sc[1] = '0' + (score - (score/1000)*1000)/100;
                  sc[2] = '0' + (score - (score/100)*100)/10;
                  sc[3] = '0';
                  sc[4] = '\0';
             }
             displayText(0.0,25.0,sc);
             displayText(-150.0,0.0,"Press R for replaying the game");
             collision = 2;
             glutSwapBuffers();
    	     glFlush();
             win_flag = 2;
        }
        else
        {
             //if all levels are not over then go to the next level
             glColor3f(0.768f,0.858f,0.937f);
             glBegin(GL_POLYGON);
             glVertex2i(-200,100);
             glVertex2i(200,100);
             glVertex2i(200,-30);
             glVertex2i(-200,-30);
             glEnd();
             glColor3f(1.0f,0.0f,0.0f);
             displayText(-90.0,50.0,"Level Cleared");
             score += (level*10);
             displayText(-90.0,25.0,"Score:");
             if(score < 10)
             {
                 sc[0] = '0';
                 sc[1] = '\0';
             }
             else if(score < 100)
             {
                 sc[0] = '0' + score/10;
                 sc[1] = '0';
                 sc[2] = '\0';
             }
             else if(score < 1000)
             {
                 sc[0] = '0' + (score/100);
                 sc[1] = '0' + (score - (score/100)*100)/10;
                 sc[2] = '0';
                 sc[3] = '\0';   
             }
             else 
             {
                  sc[0] = '0' + (score/1000);
                  sc[1] = '0' + (score - (score/1000)*1000)/100;
                  sc[2] = '0' + (score - (score/100)*100)/10;
                  sc[3] = '0';
                  sc[4] = '\0';
             }
             displayText(0.0,25.0,sc);
             displayText(-150.0,-0.0,"Press N for moving to next level");
             glutSwapBuffers();
    	     glFlush();
             win_flag = 2;
        }
    }
    for(int i=0;i<1.9e7;i++); //time delay loop
}
void init()
{
        //initialises the screen properties
	glClearColor(0.529, 0.807, 1.0, 0.5);
	glColor3f(1.0f, 0.0f, 0.0f);
	glPointSize(1.0f);
	gluOrtho2D(-screen_width/2, screen_width/2, -screen_height/2, screen_height/2);
}
void display(void)
{ 
    //our display callback function
    if(control == 0)
    {
        glClear(GL_COLOR_BUFFER_BIT);
        resetGame();
        initialiseExit();
        createObstacles();
	control++;
    }
    if(control!=0 && control!=4)
    {
         move_in_dir();
    }
    if(control == 4 && pause_c == 0)
    {
        glColor3f(0.768f,0.858f,0.937f);
        glBegin(GL_POLYGON);
        glVertex2i(-40,60);
        glVertex2i(80,60);
        glVertex2i(80,20);
        glVertex2i(-40,20);
        glEnd();
        glColor3f(1.0f,0.0f,0.0f);
        displayText(-30.0,30.0,"Paused.....");
        glutSwapBuffers();
        glFlush();
        pause_c = 1;
    }
}
void idle(void)
{
    //idle function; keeps on calling display callback function
    glutPostRedisplay();
}
void updateVelocity()
{
    //updates the velocity components
    v_x = velocity*cos(theta);
    v_y = velocity*sin(theta);
}
void updateTheta(int id)
{
    if(id == 0)
        theta += 0.05;
    else
        theta -= 0.05;
    updateVelocity();
}
void pause()
{
    control_buffer = control;
    control = 4;
}
void play()
{
    control = control_buffer;
    control_buffer = 0;
    pause_c = 0;
}
void myKeyFunc(unsigned char key,int x,int y)
{
    //senses the key presses from keyboard
    if((key == 'a' || key == 'A') && control!=4 && control!=1)
        updateTheta(0);  //this turns the plane left
    else if((key == 'd' || key == 'D') && control!=4 && control!=1)
        updateTheta(1);  //this turns the plane right 
    else if((key == 'w' || key == 'W') && control!=4 && control!=1)
    {  
            //this increases speed
            velocity += 0.4f;
            if(velocity>3.5)
               velocity = 3.5;
            updateVelocity();
    }
    else if((key == 's' || key == 'S') && control!=4 && control!=1)
    {
            //this decreases speed
            velocity -= 0.4f;
            if(velocity < -0.2)
               velocity = -0.2;
            updateVelocity();
    }
    else if(key == 'c' || key == 'C')
         cheat = 1; //enables cheat code
    else if(key == 'u' || key == 'U')
         cheat = 0; //disables cheat code
    else if((key == 'y' || key == 'Y') && control!=4)
         control = 2;  //launches the game
    else if((key == 'r' || key == 'R') && collision == 2 && control!=4)
    {
         //restarts the game only if there is a collison and game is not paused
         control = 0;   
         score = 0;
         level = 1;
    }
    else if(key == 'e' || key == 'E')
         exit(-1);  //exit the game
    else if((key == 'n' || key == 'N') && win_flag == 2 && control!=4)
    {
         //move to next level in case of win
         control = 0;
         level = level + 1;
    }
    else if((key == 'p' || key == 'P') && control!=4)
         pause();  //pause the game
    else if((key == 'q' || key == 'Q') && control == 4)
         play();   //resume the game
    else;
}
int main(int argc, char **argv) 
{
        //main function
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE|GLUT_RGB);
	glutInitWindowPosition(0, 0);
	glutInitWindowSize(screen_width, screen_height);
	glutCreateWindow("Dashed Out");
	glutDisplayFunc(display);
	glutKeyboardFunc(myKeyFunc);
        glutIdleFunc(idle);
	init();
	glutMainLoop();
	return 0;
}
