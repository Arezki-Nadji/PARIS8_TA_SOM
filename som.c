#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>

#define SQR(x) ((x)*(x)) //Racine carrée

struct Net_Config
{
	int data_in; // taille du vecteur de donnée
	int n_ligne_out; // nombre de ligne de la map
	int n_colonne_out ; // nombre de colonne de la maap
	int n_out ; // total du nombre des neuron de la map (n_ligne_out x n_colonne_out)
	int nb_it;//noombre d'itération
	double minAlpha; //taux d'apprentissage initial
	int train; //combien de couche
	int ftrain ; // taux d'apprentissage 1/4 puis 3/4 
}Net_Config;

struct node //Neuron
{
	double activation ; // distance euclidienne
	char *etiquette;
	double *w; // poid (vecteur de memoir)
};

typedef struct node t_node;

struct bmu {
	double activation; //distance euclidienne
	int ligne;
	int coloonne;

};
typedef struct bmu t_bmu;
t_bmu *bmu = NULL;

int bmu_size =1;

struct net{
	int nhd_r; //neighbourhood radius 
	t_node **map; //grille 
	double *captors; // actuel vecteur de donnée 
	double alpha ; // coeficient d'apprentissage 
	char *etiquette;
}Net;

struct vec
{
	double *arr; //Donnée 
	char *nom;
	double norm; // valeur de la normalisation 
};

struct vec * array_vec;

void alloc_array_struct(int n)
{
	array_vec=malloc(n*sizeof(struct vec));
	int i;
	for(i=0;i<n;i++){
		array_vec[i].arr=malloc(Net_Config.data_in*sizeof(double));
		array_vec[i].nom=malloc(20*sizeof(char));

	}

}

double *moyenne, *min, *max;

void moyenne_vec(int n)
{
	moyenne = malloc(Net_Config.data_in*sizeof(double));
	memset(moyenne,0,Net_Config.data_in*sizeof(double));
	int i,j;

	for(i=0;i<Net_Config.data_in;i++)
	{
		for(j=0;j<n;j++)
		{
			moyenne[i]+=array_vec[j].arr[i];
			moyenne[i]/=n;
		}

	}
}

void min_vec(double k)
{
	min=malloc(Net_Config.data_in*sizeof(double));
	int i;
	for(i=0;i<Net_Config.data_in;i++)
		min[i]=moyenne[i]-k;
}

void max_vec(double k)
{
	max=malloc(Net_Config.data_in*sizeof(double));
	int i;
	for(i=0;i<Net_Config.data_in;i++)
		max[i]=moyenne[i]+k;
}

void norm_array_vec(int i,int size)
{
	double somme=0.;
	int j;
	for(j=0;j<size;j++){
		somme+=SQR(array_vec[i].arr[j]);
	array_vec[i].norm =sqrt(somme);
	}
}

double* init_rand_w()
{
	int i;
	double k=(double)rand()/RAND_MAX;
	double *tmp_w = malloc(Net_Config.data_in*sizeof(double));

	for(i=0;i<Net_Config.data_in;i++)
	{
		tmp_w[i]=k*(max[i]-min[i])+min[i];
	}
	double norm =0.;

	for(i=0;i<Net_Config.data_in;i++)
	{
		norm+=SQR(tmp_w[i]);
	}
	for(i=0;i<Net_Config.data_in;i++)
	{
		tmp_w[i]/=norm;
	}
	return tmp_w;
}
int *index_array;

void init_shuffle(int n)
{
	index_array=malloc(sizeof(int)*n);
	int i;
	for(i=0;i<n;i++)
	{
		index_array[i]=i;
	}
}

void array_suffle(int n)
{
	int i,r_and,k;
	srand(time(NULL)); //s'assure d'obtenir les meme nombre randomisé a chaque éxecution
	for(i=0;i<n;i++)
	{
		r_and=rand()%n;
		k=index_array[i];
		index_array[i]=index_array[r_and];
		index_array[r_and]=k;
	}
}

double distance_euclidienne(double *a1,double *a2,int n)
{
	double somme = 0.;
	int i;
	for(i=0;i<n;i++){
		somme+=(SQR(a1[i]-a2[i]));
	}
	return sqrt(somme);
}

void calc_alpha(int it_n,int total_it)
{
	Net.alpha = Net_Config.minAlpha*(1-((double)it_n/(double)total_it));
}
void read_data()
{
	FILE*in;

	char *str=malloc(sizeof(char)*500);
	in=fopen("iris.data","r");

	int i,j;
		for(i=0;i<150;i++)
		{
			fscanf(in,"%s",str);
			char *tok=strtok(str,",");

			for(j=0;j<Net_Config.data_in;j++)
			{
				array_vec[i].arr[j]=atof(tok);
				tok=strtok(NULL,",");

			}
			if(strcmp(tok,"Iris-setosa")==0)
				strcpy(array_vec[i].nom,"1");
			else if(strcmp(tok,"Iris-versicolor")==0)
				strcpy(array_vec[i].nom,"2");
			else
				strcpy(array_vec[i].nom,"3");
			norm_array_vec(i,Net_Config.data_in);
		}

		fclose(in);
	free(str);
}

void create_neuron_map()
{
	int i,j;
	Net.map=malloc(Net_Config.n_ligne_out*sizeof(t_node*));
		for(i=0;i<Net_Config.n_ligne_out;i++)
		{
			Net.map[i]=malloc(Net_Config.n_colonne_out*sizeof(t_node));
		}
		for(i=0;i<Net_Config.n_ligne_out;i++)
		{
			for(j=0;j<Net_Config.n_colonne_out;j++)
			{
				Net.map[i][j].w=(double*)malloc(sizeof(double)*Net_Config.data_in);
					Net.map[i][j].w=init_rand_w(0);
					Net.map[i][j].etiquette=malloc(20*sizeof(char));
					strcpy(Net.map[i][j].etiquette,".");

			}
		}
}
;
void print_map()
{
	int i,j;
	for(i=0;i<Net_Config.n_ligne_out;i++)
	{
		for(j=0;j<Net_Config.n_colonne_out;j++)
		{
			printf("%s",Net.map[i][j].etiquette);
		}
		printf("\n");
	}
};
void init_Net_Confing()
	{
		Net_Config.n_ligne_out=6;
		Net_Config.n_colonne_out=10;
		Net_Config.n_out=Net_Config.n_ligne_out*Net_Config.n_colonne_out;
		Net_Config.data_in=4;
		Net_Config.nb_it=2000;
		Net_Config.minAlpha=0.7;
		Net_Config.ftrain=Net_Config.nb_it/4;
		Net_Config.train=2;
	}
struct voisinageTaille{
	int x1;
	int x2;
	int y1;
	int y2;
}voisinageTaille;

struct voisinageTaille carre;

void update(t_bmu* b_mu)
{
    int nr=Net.nhd_r;
    int i,j,x1,x2,y1,y2;

    for(;nr>=0;nr--)
    {
        if(b_mu->ligne-nr<0)
            x1=0;
        else
           x1=b_mu->ligne-nr;
        if(b_mu->coloonne-nr<0)
            y1=0;
        else
            y1=b_mu->coloonne-nr;

        if(b_mu->ligne+nr>Net_Config.n_ligne_out-1)
            x2=Net_Config.n_ligne_out-1;
        else
            x2=b_mu->ligne+nr;
        if(b_mu->coloonne+nr>Net_Config.n_colonne_out-1)
            y2=Net_Config.n_colonne_out-1;
        else
            y2=b_mu->coloonne+nr;

        for(i=x1;i<=x2;i++)
            for(j=y1;j<=y2;j++)
            {

                int k;

                for(k=0;k<Net_Config.data_in;k++)
                    {

                        Net.map[i][j].w[k]+=Net.alpha*(Net.captors[k]-Net.map[i][j].w[k]);
                    }
            }
    }
}
void training()
{
    int i,j,p,u,it;
    double min_d,dist;

    bmu=malloc(sizeof(t_bmu));

    for(p=0;p<Net_Config.train;p++)
    {
        int courrant_n_it;
        if(p==0)
        {
            courrant_n_it=Net_Config.ftrain;//1/4 750
        }
        else
        {
            courrant_n_it=Net_Config.nb_it-Net_Config.ftrain;//3/4 2250
            Net_Config.minAlpha=0.07;
            Net.nhd_r=1;
        }

        for(it=0;it<courrant_n_it;it++)
        {
            calc_alpha(it,courrant_n_it);
            array_suffle(150);

            for(u=0;u<150;u++)
            {

                Net.captors=array_vec[index_array[u]].arr;//les données d'index U(randomisé) array_vec
                min_d=1000.;
                for(i=0;i<Net_Config.n_ligne_out;i++)
                {
                    for(j=0;j<Net_Config.n_colonne_out;j++)
                    {
                        dist=distance_euclidienne(Net.captors,Net.map[i][j].w,Net_Config.data_in);
                        Net.map[i][j].activation=dist;
                        if(dist<min_d)
                        {
                            min_d=dist;
                            if(bmu_size>1)
                            {
                                bmu_size=1;
                                bmu=realloc(bmu,bmu_size*sizeof(t_bmu));
                            }
                            bmu[0].activation=dist;
                            bmu[0].ligne=i;
                            bmu[0].coloonne=j;
                        }
                        else if(dist==min_d)
                        {

                            bmu_size++;
                            bmu=realloc(bmu,bmu_size*sizeof(t_bmu));
                            bmu[bmu_size-1].activation=dist;
                            bmu[bmu_size-1].ligne=i;
                            bmu[bmu_size-1].coloonne=j;

                        }
                    }
                }

                if(bmu_size>1)
                {
                    int t=rand()%(bmu_size);
                    bmu[0]=bmu[t];
                }

                strcpy(Net.map[bmu[0].ligne][bmu[0].coloonne].etiquette, array_vec[index_array[u]].nom);
                update(bmu);
            }
        }
    }
}
void write_resutl(){
	FILE*fp;
	fp= fopen("outresult.txt","w+");
	int i,j;
	for(i=0;i<Net_Config.n_ligne_out;i++)
	{
		for(j=0;j<Net_Config.n_colonne_out;j++)
		{
			fprintf(fp,"%s,",Net.map[i][j].etiquette);
		}
	}
	fclose(fp);
}
	
int main()
{
	init_Net_Confing();

	alloc_array_struct(150);
	read_data();
	moyenne_vec(150);
	min_vec(0.005);
	max_vec(0.005);

	init_shuffle(150);

		create_neuron_map();
		printf("Intialisé la map:\n");
		print_map();
		printf("\n");

		training();
		printf("résultat : \n");
		print_map();
		write_resutl();
		system("python visualisation.py");
		free(moyenne);
		free(min);
		free(max);

		return 0;

}
