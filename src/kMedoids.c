#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
//definition structure pour le groupe
typedef struct Cluster {
	int medoid;
	int taille;
}Cluster;
typedef Cluster* P_Cluster ;
//definition structure point
typedef struct Point {
	float x;
	float y;
	P_Cluster cluster;
}Point;
//definition structure couleur
typedef struct Color{
    int r;
    int g;
    int b;
}Color;

typedef Point* P_Point ;
//prototypes des fonctions
int min(int,int);
int max(int,int);
float getDistance(Point,Point);
void initialisePoints(P_Point,int);
void initialiseClusters(P_Point,P_Cluster,int,int);
void initialiseDistance(float *distance[],P_Point,int);
void affecterPointsMedoideProche(P_Point,P_Cluster, float *distance[], int, int);
Cluster selectBestClusterMedoids(P_Point, float *distance[], Cluster , int , SDL_Renderer * ,Color);
void pam( int, int, SDL_Window*);






//Definition des fonctions
//retourne le min de 2 valeur
int min(int i , int j){
    int m=(i<j) ? i:j;
	return m;
}
//retourne le max de 2 valeur
int max(int i , int j){
    int m=(i>j) ? i:j;
	return m;
}
//fonction qui retourne distance entre 2points
float getDistance(Point a,Point b){
    float s=sqrt(pow(a.x-b.x,2)+pow(a.y-b.y,2));
	return s;
}
//fonction pour l'intialisation de n points
void initialisePoints(P_Point points,int n){
	int i;
	for(i=0;i<n;i++){
        points[i].x=rand()%(40 + 1) + 1;
        points[i].y=rand()%(30 + 1) + 1;
    }
}
//fonction d'initialisation de k groupes
void initialiseClusters(P_Point points ,P_Cluster clusters ,int n ,int k){
	int i;
	for(i=0 ;i<k ;i++){
		clusters[i].medoid=i;
		clusters[i].taille=0;
	}
}

//fonction initialisation des distances de touts les points
void initialiseDistance(float *distance[],P_Point points,int n){
	int i,j;
	for(i=0;i<n;i++){
			for(j=0;j<i+1;j++){
				distance[i][j]=getDistance(points[i],points[j]);
			}
	}
}

//fonction affectation des points au medoid le plus proche
void affecterPointsMedoideProche(P_Point points, P_Cluster clusters, float *distance[], int n, int k){
	int i,j;
	for(i=0;i<k;i++)
		clusters[i].taille=0;
	for(i=0;i<n;i++){
		Cluster *cluster=&clusters[0];
		int mid = cluster->medoid;
		for(j=1;j<k;j++){
			Cluster *tmpCluster=&clusters[j];
			int tmpmid=tmpCluster->medoid;
			if(distance[max(i,tmpmid)][min(i,tmpmid)]<distance[max(i,mid)][min(i,mid)]){
				cluster=tmpCluster;
				mid=cluster->medoid;
			}
		}
		points[i].cluster=cluster;
		cluster->taille++;
	}
}
//fonction retournant le meilleur group
Cluster selectBestClusterMedoids(P_Point points, float *distance[], Cluster cluster , int n, SDL_Renderer * renderer,Color color){
	int i,j=0;
	float totalDist=0;
	float tmpTotalDist=0;
	int* medoidPoints=(P_Point)malloc(sizeof(Point)*cluster.taille);
	int medoid=cluster.medoid;
	for(i=0;i<n;i++){
        SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b,255);
		if(points[i].cluster->medoid==cluster.medoid){
            SDL_RenderDrawLine(renderer,points[medoid].x*15,points[medoid].y*15,points[i].x*15,points[i].y*15);
            SDL_RenderPresent(renderer);
			medoidPoints[j]=i;
			j++;
		}
	}
	for(i=0;i<n;i++)
		totalDist+=distance[max(i,medoid)][min(i,medoid)];

	for(i=0;i<cluster.taille;i++){
		tmpTotalDist=0;
		int tmpMedoid=medoidPoints[i];

		for(j=0;j<n;j++)
			tmpTotalDist+=distance[max(j,tmpMedoid)][min(j,tmpMedoid)];

		if(tmpTotalDist<totalDist){
			cluster.medoid=tmpMedoid;
			totalDist=tmpTotalDist;
		}
	}
	free(medoidPoints);
	return cluster;
}
//fonction qui applique l'algorithme KMedoids (PAM)
void pam( int n, int k, SDL_Window * window){
    //Code--
    SDL_Renderer * renderer = SDL_CreateRenderer(window, -1, 0);
    P_Point points=(P_Point)malloc(n*sizeof(Point));
	P_Cluster clusters=(P_Cluster)malloc(k*sizeof(Cluster));
    int i,j,changeTest;
    float *distance[n];
	for(i=0;i<n;i++){
			distance[i]=(float*)malloc((i+1)*sizeof(float));
	}
    Color color[k];
	for(i=0,j=k;i<k;i++,j--)
        {
            color[i].r=j*200;
            color[i].g=i*80;
            color[i].b=j*130;
        }
	initialisePoints(points,n);
	initialiseClusters(points, clusters , n, k);
	initialiseDistance(distance,points,n);
	do{
		changeTest=0;
		P_Cluster tmpClusters=(P_Cluster)malloc(sizeof(Cluster)*k);
		affecterPointsMedoideProche(points,clusters,distance,n,k);
		for(i=0;i<k;i++){
		tmpClusters[i]=selectBestClusterMedoids(points,distance,clusters[i],n,renderer,color[i]);
		}
		for(i=0;i<k;i++)
			if(tmpClusters[i].medoid != clusters[i].medoid){
				changeTest=1;
				break;
			}
		if(changeTest){
            clusters=tmpClusters;
            SDL_SetRenderDrawColor(renderer, 0, 0, 0,255);
            SDL_RenderClear(renderer);
		}

	}while(changeTest==1);

    for(i=0;i<k;i++){
            int medoid=clusters[i].medoid;
            SDL_SetRenderDrawColor(renderer, color[i].r, color[i].g, color[i].b,255);
            for(j=0;j<n;j++){
                if(points[j].cluster->medoid==medoid){
                    SDL_Rect rectangle;
                    rectangle.x = points[j].x*15;
                    rectangle.y = points[j].y*15;
                    rectangle.w = 4;
                    rectangle.h = 4;
                    SDL_RenderFillRect(renderer, &rectangle);
                }
            }
        }
    SDL_RenderPresent(renderer);
}
int main(int argc, char** argv)
{
    int quit = 0;
    SDL_Event event;
    SDL_Init(SDL_INIT_VIDEO);
    SDL_Window * window = SDL_CreateWindow("ayoub.MOUSTAID - PAM",100, 100, 800, 600, 0);

	int n=150;//points
	int k=50;//groupes
	if(k>n)exit(0);

    pam(n,k,window);

    while (!quit)
    {
        SDL_WaitEvent(&event);

        switch (event.type)
        {
        case SDL_QUIT:
            quit = 1;
            break;
        }
    }

    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
