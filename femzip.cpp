// console.cpp : Defines the entry point for the console application.
//

#include "femzip.h"
#include "stdio.h"
#include "string.h"
#include "stdlib.h"

#pragma comment(lib,"femunziplib_standard_dyna.lib") 
#pragma comment(lib,"ifmodintr.lib") 
#pragma comment(lib,"libifcoremt.lib")
#pragma comment(lib,"ifwin.lib")
#pragma comment(lib,"ifconsol.lib")
#pragma comment(lib,"libifport.lib")
#pragma comment(lib,"libmmt.lib")
#pragma comment(lib,"libirc.lib")
#pragma comment(lib,"svml_dispmt.lib")
#pragma comment(lib,"libdecimal.lib")

#pragma comment(lib,"ipp_zlib.lib")
#pragma comment(lib,"ippcoremt.lib")
#pragma comment(lib,"ippdcmt.lib")
#pragma comment(lib,"ippsmt.lib")
#pragma comment(lib,"ippvmmt.lib")

#pragma comment(lib,"libiomp5md.lib")

#pragma comment(lib,"libircmt.lib")
#pragma comment(lib,"libmatmul.lib")



int main(int argc, char* argv[])
{
	int i = 0, j=0;
	float version = 0.0;
	int type1;
	int ier;
	float file_version;
	int Size_geo,Size_state,Size_disp,Size_activity;
    int Size_post;
    int dynatype,adjust;
	int countera = 0;
	FEMUNZIPLIB_VERSION(&version);
	printf("argc:%d,argv:%s,%s",argc,argv[0],argv[1]);
	if(argc <2)
	{
		printf("Please give femzip file full name. %s",argv[0]);
		return 0;
	}
	//char *fname = "C:\\Users\\luoliangfeng\\Desktop\\FEMZIP_8.08_dyna_standard\\console\\Debug\\z805L4plot";
	char* fname = argv[1];
	FEMUNZIPLIB_VERSION_FILE(fname, &type1, &file_version, &ier); 
	printf("Femunziplib version :%f File version: %f\n",version,file_version);

	dynatype = 1;
    adjust = 5;

    GET_SIZE(fname,dynatype,adjust, &Size_geo, &Size_state,&Size_disp, &Size_activity, &Size_post, &ier);
	if (ier!=0){
		printf("Problem in get_Size : %d\n",ier);
		return 0;
	}

	printf("size_geo:%d,size_state:%d,size_disp:%d,size_activity:%d,size_post:%d\n",Size_geo,Size_state,Size_disp,Size_activity,Size_post);

    char *zargv[4];
	char zfile[256];
	int  zargc;
	int pos = 0;
	int p1arry[512],p2arry[512],p1size,p2size;
	char pname[32] = {"lsprepost"};
	int* buffer_femzip = NULL;

	   
    strcpy(zfile,fname);
    zargv[0] = pname;
    zargv[1] = zfile;
    zargc = 2;
    wrapinput(zargc, zargv, p1arry,p2arry,&p1size, &p2size);

	buffer_femzip = (int*)malloc(Size_geo*sizeof(int));

	//geometry_read returns binary data corresponding to the header(control), geometry(elements shell solid tshell beam) and connectivity information in the array buffer.
	GEOMETRY_READ(p1arry,p2arry,&p1size,&p2size, &ier, &pos, buffer_femzip, &Size_geo);

	if(ier!=0){
		printf("Problem in geometry_read: %d\n",ier);
		free(buffer_femzip);
		buffer_femzip = NULL;
		CLOSE_READ(&ier);
		return 0;
	}

    printf("geometry pos:%d, size_geo:%d\n",pos,Size_geo);
	free(buffer_femzip);
	buffer_femzip = NULL;

   int size;
   int ntime;
   float* timese = NULL;
   size = 5000*sizeof(float);
   pos = 0;
   timese = (float*)malloc(size);
   
   //ctimes_read returns the number of time steps that are available.
   CTIMES_READ(&ier, &pos, &ntime, timese, &size);
   if(ier!=0){
	   printf("Problem in CTIMES_READ: %d\n",ier);
	   free(timese);
	   timese = NULL;
   }

   for(i = 0; i < ntime; i++){
	   printf("ntime:%d, time value %d: %f\n",ntime,i, timese[i]);
   }
   free(timese);
   timese = NULL;




   //In a loop over time steps state_read, disp_read, activity_read and
   //post_read may not be used together. They require separate loops over timesteps. 
   buffer_femzip = (int*)malloc(Size_disp*sizeof(int));
   
   countera = 0;
   for(i = 0 ; i < ntime; i++)
   {
	   countera  = i+1;
	   pos = 0;
	   //disp_read - Decompresses only time stamp, global data and nodal information into DSY/d3plot/.Annn data for a state.
	   DISP_READ(&ier,&pos,&countera,buffer_femzip,&Size_disp);
	   printf("DISP_READ ier:%d,pos:%d,countera:%d,Size_disp:%d\n",ier, pos, countera,Size_disp);


   }
   free(buffer_femzip);
   buffer_femzip = NULL;

   int* activitybuffer = (int*)malloc(Size_activity*sizeof(int));
   for(i = 0 ; i < ntime; i++)
   {
	   countera = i+1;
	   pos = 0;
	   //activity_read - Decompresses only data about active/inactive elements into DSY/d3plot/.Annn data per state.
	   ACTIVITY_READ(&ier,&pos,&countera,activitybuffer,&Size_activity);
	   printf("ACTIVITY_READ ier:%d,pos:%d,countera:%d,Size_activity:%d\n",ier, pos, countera,Size_activity);
   }
   free(activitybuffer);
   activitybuffer = NULL;

   
   buffer_femzip = (int*)malloc(Size_post*sizeof(int));
   
   countera = 0;
   for(i = 0 ; i < ntime; i++)
   {
	   countera  = i+1;
	   pos = 0;
	   //post_read - decompress only element, FPM, SPH and CPM data into DSY/d3plot/.Annn data for each state.
	   POST_READ(&ier,&pos,&countera,buffer_femzip,&Size_post);
	   printf("POS_READ ier:%d,pos:%d,countera:%d,Size_post:%d\n",ier, pos, countera,Size_post);
   }
   POST_CLOSE(&ier,&pos,buffer_femzip,&Size_post);
   free(buffer_femzip);
   buffer_femzip = NULL;



   countera = 0;   
   buffer_femzip = (int*)malloc(Size_state*sizeof(int));
   for(i = 0 ; i < ntime; i++)
   {
	   countera +=1;
	   pos = 0;
	   //Decompresses a section of the compressed file and returns all data objects for a single state via the buffer starting at position pos.
	   STATES_READ(&ier,&pos,&countera,buffer_femzip,&Size_state);
	   printf("STATES_READ ier:%d,pos:%d,countera:%d,size_state:%d\n",ier, pos, countera,Size_state);
   }
   STATES_CLOSE(&ier,&pos,buffer_femzip,&Size_state);
   free(buffer_femzip);
   buffer_femzip= NULL;
   
   CLOSE_READ(&ier);
   if(ier!=0){
	    printf("close_read ier:%d",ier);
   }
   return 1;
}

