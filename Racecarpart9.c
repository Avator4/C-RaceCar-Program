/***********************************Racecar Program******************************************/
/*  							Part 9 - Linked List										*/
/*									Due 8/13/2001											*/
/*									 Byron Austin											*/
/*										CS320												*/
/********************************************************************************************/

#include <time.h>
#include <stdio.h>				//Program uses standard io header file
#include <stdlib.h>				//Header file used for random number generator
#include <math.h>

#define ROWSIZE 500				//Declares constant value for maximum array rows
#define COLSIZE 500				//Declares constant value for maximum array columns
#define WAITTIME	100000000		//Sets the delay can be adjusted for each computer
#define SQR(X) (X)*(X)

typedef struct checkpoints checkpoint;

/* declare a self-referential structure */
typedef struct nodes {
int row;
int col;
int count;
int prev;
struct NODE* next;
} NODE;
struct checkpoints
	{
		int rlow;
		int rhigh;
		int clow;
		int chigh;
		char passed;
	};

NODE* get_nodes(int, int, NODE*, int);
print_nodes(NODE* ptr, int track[][COLSIZE],NODE* start,int,int,int);
char steering(int*,int*,int,char,int,int,int track[ROWSIZE][COLSIZE]);		//function reads the keyboard input for steering the car
int newdirection(int, char, int*, int*,int*,int*,int,int); // determines new direction of travel
void loadtrack (FILE *Infile, int track[ROWSIZE][COLSIZE], int, int);//loads track from file
int trackdraw (int track[ROWSIZE][COLSIZE], int, int, int, int, int*,int,int*);	 //draws track on screen
int lookahead (int, char, int, int, int*,int track[ROWSIZE][COLSIZE]); //checks ahead
int quadcheck(int,int,int,int,int,checkpoint*);//checks quadrant
char finishline ();
void winner (checkpoint*,int track[ROWSIZE][COLSIZE],int,int,NODE*,NODE*);
//**************************************MAIN PROGRAM***************************************/
int main ()

{
	NODE* start;
	int track[ROWSIZE][COLSIZE],newqtest;		//Declares track array
	int carrow,carcolumn,travel,rows,cols,wall,lastquad;		//Declares integer variables
	int confidence,startcol,startrow,count,quadrant,inittvl;
	int reset = 0;
	int /*attempts,*/move,speed;
	char direction,quit,finish;					//Declares character variable for direction
	struct checkpoints arraycheck[4];
	FILE *Infile;								//Declares use of external file for track
	printf("enter the number of attempts before quitting (recommend 10)\n");//	scanf ("%d",&attempts);
	Infile = fopen("track.txt","r");			//opens track file - text file 
	fscanf (Infile,"%d%d",&rows, &cols);			//reads track size from text file
	srand( (unsigned)time( NULL ) );	//generates random seed for random number generator
	wall= move =0;										//clears the wall marker variable
	count = 0;									//initializes attempts count
	confidence = 0;								//clears the car confidence variable
	speed=1;
	quadrant=0;
	lastquad=0;
	carrow = (rows/4)-1;						//sets value for initial car row location
	carcolumn = (cols/2)-1;						//sets value for initial car column location
	startrow = carrow;							//stores starting car row position in startrow
	quit = ' ';									//clears stop flag variable
	startcol = carcolumn;						//stores starting car column position in startcol
	finish ='n';
	inittvl=travel=2;							//sets initial direction of travel WEST
	loadtrack (Infile,track,rows,cols);					//load track from text file
	trackdraw (track,rows,cols,carrow,carcolumn,&wall,reset, &move);	//draws track to screen
	arraycheck[0].rlow=0;
	arraycheck[0].rhigh=(rows/2)-1;
	arraycheck[0].clow=0;
	arraycheck[0].chigh=(cols/2)-1;
	arraycheck[1].rlow=(rows/2)-1;
	arraycheck[1].rhigh=rows;
	arraycheck[1].clow=0;
	arraycheck[1].chigh=(cols/2)-1;
	arraycheck[2].rlow=(rows/2)-1;
	arraycheck[2].rhigh=rows;
	arraycheck[2].clow=(cols/2)-1;
	arraycheck[2].chigh=cols;
	arraycheck[3].rlow=0;
	arraycheck[3].rhigh=(rows/2)-1;
	arraycheck[3].clow=(cols/2)-1;
	arraycheck[3].chigh=cols;

do
{
	do
	{
		
		direction = steering(&confidence,&wall,travel,direction,carrow,carcolumn,track);						//checks steering command 
		{
			travel = newdirection (travel,direction,&carrow,&carcolumn,&wall,&confidence,rows,
						cols);							//sets new direction of travel
			speed=trackdraw(track,rows,cols,carrow,carcolumn,&wall,reset,&move);
														//draws track after steering action
			start = get_nodes(carrow,carcolumn,start,reset);
			newqtest=quadrant;
			quadrant = quadcheck (rows,cols,carrow,carcolumn,wall,arraycheck);		
														//returns quadrant car is in
			if (wall==-5)
			{
				quadrant=lastquad;
				reset = 0;
				print_nodes(start,track,start,wall,move,speed);
			}
			else 
			{
				reset = 1;
				if (newqtest < quadrant)//checks for quadrant change
				{
					print_nodes( start,track,start,wall,move,speed );
					reset = 0;
					speed=trackdraw (track,rows,cols,carrow,carcolumn,&wall,reset,&move);
				}
			}
			if (quadrant != lastquad)
			{
			printf("quadrant %d\n",quadrant);				//prints quadrant
			if ((quadrant<lastquad)||(quadrant-lastquad!=1)||//tests for wrong
				((quadrant==3)&&(lastquad==0)))	//direction over quadrant boundary
				{
					if ((quadrant==0)&&(lastquad==3))
					{
						finish=finishline(arraycheck);
					
					}					
						if (finish !='y')
						{
						printf("WRONG WAY!!-RESTART\n");
						direction='Q';
						reset = 0;
						}
					
					else
					{
						if (!((quadrant==3)&&(lastquad==0)))
						{
						reset = 0;
						winner (arraycheck,track,carrow,carcolumn,start,start);
						speed=trackdraw (track,rows,cols,carrow,carcolumn,&wall,reset,&move);
						}
					}
					if ((quadrant==3)&&(lastquad==0))direction='Q';
				}
			}
			lastquad=quadrant;
		}
	}
	while ((direction != 'Q') && (direction !='q'));	//ends do loop when quitting program
	if (direction=='q')
	{
		printf ("CRASH!!!!!!!!\n ");			//enter q to exit\n enter y to restart\n");
		reset = 0;
	}
	if (reset==0)move=0;
//	track[carrow][carcolumn]=wall;		//resets the wall to its wall value after a crash
	carrow = startrow;					//resets the car to its starting row
	carcolumn = startcol;				//resets the car to its starting column
	direction = 's';					//sets inital direction to strait
	travel = inittvl;					//Resets initial direction of travel to north				
	wall = 0;							//resets the wall value to zero from -5 after a crash
	count++;							//increments attempts counter
	quadrant=lastquad=0;
	printf("tries =%d\n",count);		//Displays number of attempts
}
while (count>0);// < attempts);						//restarts the program if y or exits if q selected
	fclose (Infile);					//closes external file
	return 0;
}
/*******************************Load track array function **********************************/
/* Parameters passed in: text file, track array, row size and column size. Return: nothing */
/*******************************************************************************************/
void loadtrack (FILE *Infile, int track[ROWSIZE][COLSIZE], int rows, int cols)
{
	int i, j;							//Declares local counter variables for filling array
	for (i=0;i<rows;i++)				//Array row filling loop
	{
		for (j=0;j<cols;j++)			//Array column filling loop
		{
			fscanf (Infile, "%d",&track[i][j]);	//Reads and stores data from track file
		}
	}
}
/********************************Trackdraw array function ***********************************/
/*Parameters passed in: track array, rowsize, column size                   Return: nothing*/
/*******************************************************************************************/
int trackdraw (int track[ROWSIZE][COLSIZE], int rows, int cols, int carrow, int carcolumn, int* wall, int reset, int* move)
{
	int i, j, s, confidence,temp;		//initializes variables for filling track and confidence variable
	if (reset == 0) move = 0;
	for (i=0;i<20;i++)							//Clears screen
	{
		printf("\n");
	}
	for ( i=0; i < rows; i++)					//Loop for drawing row from array
	{
		for (s=0;s<((35-rows)/2);s++)			//Centers track vertically
		{
			printf("  ");
		}
		for ( j=0; j < cols; j++ )				//Loop for drawing column from array
		{
			if (i == carrow && j == carcolumn)	//checks for current car postion
			{
				*move++;
				printf ( " X");				//places X in current car position
				temp = track[i][j];				//read track value to test for wall
			if (*wall != -5)
			{
				*wall = temp;
			}
				temp = track[i][j];			//loads confidence value of current car position
				temp++;						//increments confidence
				if (temp >= 5)
				temp = 5;						//limits confidence to maximum of 5
				confidence = temp;				//stores confidence value in track array
			}
				else
					if (track[i][j] == -5)
					{
						printf ("**");
					}
				/*	else
					{
						if ((i==(rows/2)-1) || (j==(cols/2)-1))
						{
							printf("  ");//printf(" +");
						}*/
						else
						{
							if (track[i][j] > 0)
							{
							printf/*(" .");//*/("%2d",track[i][j]);
							}
							else
							printf("  ");		//prints track array values to screen
						}
					//}
			}
		printf ("\n");
	}
return(confidence);
}
/********************************Steering function *******************************************/
/*Parameters passed in: confidence, wall                              Return: char direction input*/
/*********************************************************************************************/
/*This function uses a random number generator to determine the new steering direction			*/					
/*********************************************************************************************/
char steering (int* confidence, int* wall, int travel,char direction,int carrow, int carcolumn,
			   int track[ROWSIZE][COLSIZE])
{
	int preview,i;
	char steer;	

	for(i=0;i<WAITTIME;i++);					//Pauses screen for viewing
	if (*wall == -5)							//Checks for wall impact of out of track
		{
			preview = 4;						//When preview=4 anounces crash
		}
	else
		{
		preview =	lookahead (travel,direction,carrow,carcolumn,wall,track);
		}
		
	switch (preview)
	{
		
	case 1: steer = 's';		//straight
		break;
	case 2: steer = 'r';		//left
		break;
	case 3: steer = 'l';		//right
		break;
	case 4: steer = 'q';		//crash
		break;
	}

	printf ("\n");										//performs a carriage return
	return steer;										//returns character to main program
}

/********************************New direction function ************************************/
/*Parameters passed in: prior direction, steering value, carrow, carcolumn                 */
/*Return: int new travel direction														   */
/*******************************************************************************************/
/*The speed portion of this function uses the confidence at the cars location and the confidence */
/*along with the speed the car is traveling. Speed is an integer between 1 and 5.			*/
int newdirection (int travel, char direction, int* carrow, int* carcolumn, int* wall,int* confidence,
				  int rows, int cols)
{
	int newrow, newcol,crash,temp;
	static speed=1;
	temp = *wall;
	crash = 0;
	speed = 1;//*wall;
/*	if ((speed > 5) || (speed < 1))		//Checks to see if speed is within limits
	{
		speed = temp + 1;
	}
	if (speed < temp)		//Slow down 
	{
		speed = rand() * (speed - 1) / (RAND_MAX + 1);//decreases speed randomly between 0 and speed -1
	}
	if (speed > temp)
	{
		speed = 1 + rand() * temp / (RAND_MAX + 1);//increase speed
	}
	if (speed == 0)
	{
		speed =1;
	}*/
	newrow = *carrow;
	newcol = *carcolumn;
	switch(travel)
	{
	case 0: switch(direction)//Direction of travel North
			{
		case 's': newrow -= speed; break;
		case 'r': newrow -= speed; newcol += speed; travel=7; break;
		case 'l': newrow -= speed; newcol -= speed; travel++; break;
			}
		break;
	case 1: switch(direction)//Direction of travel NorthWest
			{
		case 's':newrow -= speed;newcol -= speed;break;
		case 'r':newrow -= speed;travel--;break;
		case 'l':newcol -= speed;travel++;break;
			}
		break;
	case 2: switch (direction)//Direction of travel West
			{
		case 's':newcol -= speed;break;
		case 'r':newrow -= speed;newcol -= speed;travel--;break;
		case 'l':newrow += speed;newcol -= speed;travel++;break;
			}
		break;
	case 3:	switch (direction)//Direction of travel SouthWest
			{
		case 's':newcol -= speed;newrow += speed;break;
		case 'r':newcol -= speed;travel--;break;
		case 'l':newrow += speed;travel++;	break;
			}
		break;
	case 4:	switch (direction)//Direction of travel South
			{
		case 's':newrow += speed;break;
		case 'r':newrow += speed;newcol -= speed;travel --;break;
		case 'l':newrow += speed;newcol += speed;travel++;break;
			}
		break;
	case 5: switch (direction)//Direction of travel SouthEast
			{
		case 's':newcol += speed;newrow += speed;break;
		case 'r':newrow += speed;travel--;break;
		case 'l':newcol += speed;travel++;break;
			}
		break;
	case 6:	switch (direction)//Direction of travel East
			{
		case 's':newcol += speed;break;
		case 'r':newrow += speed;newcol += speed;travel--;break;
		case 'l':newrow -= speed;newcol += speed;travel++;break;
			}
		break;
	case 7:	switch (direction)//Direction of travel NorthEast
			{
		case 's':newcol += speed;newrow -= speed;break;
		case 'r':newcol += speed;travel--;break;
		case 'l':newrow -= speed;travel=0;break;
			}
		break;
}
	if ((newrow < 0) || (newrow > rows) || (newcol < 0) || (newcol > cols))
	{
		*wall = -5;
	}
*carrow = newrow;
*carcolumn = newcol;
return (travel);
}
/***************************Look Ahead Function*********************************************/
/*This function checks the next three possible choices of travel. It passes in values for travel
,direction, carrow and car column(car locations),the value for the wall variable and the track variables.
if all choices (choice[0],choice[1],and choice[2]) are equal, a random number between 1 and three is 
generated and returned as path to the k value in the steering function. The variables big1 and
small1 are temporary variables that store the values of choice[1,2,3] that are largest and smallest.
big1 then represents the most selected path, small1 represents the lease selected path. small1 is checked 
for a negative five (wall) value, and if not -5, it is subtracted from big1 and the difference
(diff) is used to determine a return value for path*/ 
int lookahead (int travel, char direction, int carrow, int carcolumn, int* wall,int track[ROWSIZE][COLSIZE])
{
	int newrow, newcol,path,path1,toward,choice[3],choiceb,choices,big1,small1,diff;
	toward = travel;
	big1=0;
	small1=0;
	for (path1 = 0; path1 <3; path1++)	//loop loads possible path choices into choice array
	{
		newrow = carrow;
		newcol = carcolumn;
		switch(toward)
		{
	case 0: switch(path1)				//Direction of travel North
			{
		case 0: newrow--; choice[path1] = track[newrow][newcol];break;
		case 1: newrow--; newcol++; choice[path1] = track[newrow][newcol]; break;
		case 2: newrow--; newcol--;choice[path1] = track[newrow][newcol]; break;
			}
		break;
	case 1: switch(path1)//path of travel NorthWest
			{
		case 0:newrow--;newcol--;choice[path1] = track[newrow][newcol];break;
		case 1:newrow--;choice[path1] = track[newrow][newcol];break;
		case 2:newcol--;choice[path1] = track[newrow][newcol];break;
			}
		break;
	case 2: switch (path1)//path of travel West
			{
		case 0:newcol--;choice[path1] = track[newrow][newcol];break;
		case 1:newrow--;newcol--;choice[path1] = track[newrow][newcol];break;
		case 2:newrow++;newcol--;choice[path1] = track[newrow][newcol];break;
			}
		break;
	case 3:	switch (path1)//path of travel SouthWest
			{
		case 0:newcol--;newrow++;choice[path1] = track[newrow][newcol];break;
		case 1:newcol--;choice[path1] = track[newrow][newcol];break;
		case 2:newrow++;choice[path1] = track[newrow][newcol];break;
			}
		break;
	case 4:	switch (path1)//path of travel South
			{
		case 0:newrow++;choice[path1] = track[newrow][newcol];break;
		case 1:newrow++;newcol--;choice[path1] = track[newrow][newcol];break;
		case 2:newrow++;newcol++;choice[path1] = track[newrow][newcol];break;
			}
		break;
	case 5: switch (path1)//path of travel SouthEast
			{
		case 0:newcol++;newrow++;choice[path1] = track[newrow][newcol];break;
		case 1:newrow++;choice[path1] = track[newrow][newcol];break;
		case 2:newcol++;choice[path1] = track[newrow][newcol];break;
			}
		break;
	case 6:	switch (path1)//path of travel East
			{
		case 0:newcol++;choice[path1] = track[newrow][newcol];break;
		case 1:newrow++;newcol++;choice[path1] = track[newrow][newcol];break;
		case 2:newrow--;newcol++;choice[path1] = track[newrow][newcol];break;
			}
		break;
	case 7:	switch (path1)//path of travel NorthEast
			{
		case 0:newcol++;newrow--;choice[path1] = track[newrow][newcol];break;
		case 1:newcol++;choice[path1] = track[newrow][newcol];break;
		case 2:newrow--;choice[path1] = track[newrow][newcol];break;
			}
		break;
		}
	}
	
	/*if paths (choice[0,1,2]) are equal, choose random direction (path)*/

	if ((choice[0] == choice[1]) && (choice[0] == choice[2]))
	{
		path = 1 + rand() * 3 / (RAND_MAX + 1);
	}

	/*if paths not equal find difference between biggest and smallest, randomly 
					select number between zero and the difference				*/
	
	else		
	{
		big1=0;									//initializes the big1 value with 0
		small1=5;								//initializes the small value with 5
		choices=4;								//initiales choice small value with 4

		/*this loop compares choice[0,1,2] to see which is bigger and stores the biggest
											value in big1 variable*/
		
		for (path1=0;path1<3;path1++)
		{
			if(choice[path1] != -5)
			{
			if(big1<choice[path1])
			{
				big1=choice[path1];
				choiceb=path1;	//stores biggest path in choiceb(choice big) variable
			}
			else
				if(big1==0)
				{
					choiceb=path1;
					big1=choice[path1];
				}
			}
		/*this part of the loop compares choice[0,1,2] and stores the smallest value 
												in small1 variable*/
		
			if(small1>choice[path1])
			{
				if (choice[path1] >= 0)		//checks to see if smallest value is positive
				{
					small1=choice[path1];
/*The path with the smallest direction is stored in choices(choice small) variable location*/

					choices=path1;
				}

/*If the smallest choice is less than 0, then a 3 is stored in the choices variable, if this 
value remains 3, then 2 choices are negative (walls) and the third choice is selected - big1*/

				else
				{
					choices=3;
				}
			}
		}
											//end of for loop that compares choices of travel
			if(choices==3)					//if choices = 3 then two choices are walls
			{
				path=choiceb+1;				//the third choice is the best choice
			}
			else

/*This finds a random integer between 0 and the difference between the biggest choice and the*/ 
/*smallest positive choice. If the difference is 0, a random value is generated for the choice*/
/*of directions*/

			{
				diff =  rand() * (big1-small1) / (RAND_MAX + 1);
				if(diff=0)
				{
					path =  rand() * 3 / (RAND_MAX + 1);
				//	printf("%d",path);
				}

/*If the random of the difference is not 0, then choiceb (big choice) is selected*/

				else
				{
					path = choiceb+1;
				}
			}
		}
return (path);
}

/***********************************Checkpoint Function***************************************/
/*returns integer value for quadrant 1 thru 4		*/
/*		*/
/*********************************************************************************************/
int quadcheck(int rows,int cols,int carrow,int carcolumn,int wall,checkpoint* arraycheck)
{
	int quadrant,i;


	if (wall==-5)
	{
		for(i=0;i<4;i++)				//clears passed function on crash
		{
			arraycheck[i].passed='n';
		}
		
	}
	else
	{
		for (i=0;i<4;i++)
		{
		if ((carrow > arraycheck[i].rlow) && (carrow <= arraycheck[i].rhigh) && (carcolumn > arraycheck[i].clow) && (carcolumn <= arraycheck[i].chigh))
		{
			quadrant=i;
			arraycheck[i].passed='y';
		}
	}
}
return (quadrant);
}
/****************************************Finish Line Check Function**************************/
/*		*/
/*		*/
/********************************************************************************************/
char finishline(struct checkpoints arraycheck[])
{
	int i;
	char finish;
	for (i=0;i<4;i++)
	{
		if (arraycheck[i].passed=='y')
		{
			finish = 'y';
		}
		else
		{
			finish = 'n';
		}
	}
	return (finish);
}
/****************************************Winner Function*************************************/
/*		*/
/*		*/
/********************************************************************************************/
void winner (checkpoint* arraycheck,int track[][COLSIZE],int carrow,int carcolumn,NODE* ptr,NODE* start)
{
	int i,temp;
	track[carrow][carcolumn]=9;
	if(ptr->prev != NULL)
	{
		ptr=ptr->prev;
		i=5;	
		do
		{
			track[ptr->row][ptr->col]=i;
			temp=ptr->prev;							/*frees allocated memory*/
			free (ptr);
			ptr=temp;
			i--;
			if(i<1)i=1;
		}
		while (ptr->prev!=NULL);
	}
	for(i=0;i<4;i++)
	{
		arraycheck[i].passed='n';
	}
	printf("Lap completed!!!   REFUELING\n");
	for(i=0;i<WAITTIME*4;i++);
//	scanf("%s");
	return;
}
/*************************************Linked List function**********************************/
/*		*/
/*		*/
/*******************************************************************************************/
/*	get_nodes allocates run-time storage for nodes. It builds 
	the linked list and stores user-supplied names in the name 
	members of the nodes. It returns a pointer to the first such 
	node. */

NODE* get_nodes(int carrow,int carcolumn,NODE* start,int reset)
{
	NODE *current, *first;
	/* allocate first node */
	int jump=reset;
	static steps=1;
	static onenode =0;
//	int temprow, tempcol;
	int temp;
	if (jump == 0)
	{
	steps=1;
	first = current =/* onenode =*/malloc( sizeof ( NODE ) );
	jump = 1;
	current->count=steps;
	current -> row = carrow;		//moves carrow to structure storage
	current -> col= carcolumn;		//moves carcolumn to structure storage
	current->prev=NULL;
	temp=current;
	current -> next = malloc( sizeof ( NODE ) );
	}
	else
	{
	current = start;
	current->count=steps;
	temp=current;
	current->next=malloc( sizeof ( NODE ) );
	current=current->next;
	current->prev=temp;
	current->count=steps;
	temp=current;
	current=current->prev;
	current=temp;
	current -> row = carrow;
	current -> col= carcolumn;
	first = current;
	current -> next = malloc( sizeof ( NODE ) );
	current -> next=NULL;
	
	}
	steps++;
	return first;
}
/***************************************Draw path function**********************************/
/*		*/
/*		*/
/*******************************************************************************************/
print_nodes( NODE * ptr, int track[][COLSIZE],NODE* start,int wall,int move,int speed)
{

	int distance,moves,temp,step,i,row,col,flag;
	flag=0;
	temp=ptr;							//stores current address
	moves=1;
	
	if(ptr->prev!=NULL)
	{	
		ptr=ptr->prev;
		speed=track[ptr->row][ptr->col];
		ptr=temp;
		do						//finds start row/col
		{
			ptr=ptr->prev;	
			row=ptr->row;
			col=ptr->col;
			moves++;
		}
		while (ptr->prev!=NULL);
	}
	ptr=temp;									//restores pointer location
	if(ptr->prev!=NULL)
	{
		step=3;
		if(moves<3)step=moves;
	//	if(speed<step)step=speed;
		if (wall==-5)
		{						/*decrements track on crash*/
			for(i=step;i>0;i--)
			{
				while ( ptr->prev != NULL ){
				temp=ptr->prev;
				free (ptr);
				ptr=temp;
				track[ptr->row][ptr->col]=track[ptr->row][ptr->col]-i;
				if(track[ptr->row][ptr->col]<0)track[ptr->row][ptr->col]=0;
				}
			}
		}
		else								/*increments successful route*/
		{
			distance =(int)sqrt((SQR(row-ptr->row))+(SQR(col-ptr->col)));
			while ( ptr->prev != NULL ) 
			{ /*insert track speed calculation here*/
				if(wall != -5)
				{
					if(track[ptr->row][ptr->col]>5)flag=1;
					
					track[ptr->row][ptr->col]=track[ptr->row][ptr->col]
						+(distance*speed/moves)+1;
					if(flag==0)
					{
					if (track[ptr->row][ptr->col]>5)track[ptr->row][ptr->col]=5;
					}
					if(flag==1)
					{
					if (track[ptr->row][ptr->col]>9)track[ptr->row][ptr->col]=9;
					flag=0;
					}
				}

			temp=ptr->prev;							/*frees allocated memory*/
			free (ptr);
			ptr=temp;
			}
		}
	}
}
