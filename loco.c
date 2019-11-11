#include "api_robot2.h"
//#define MINDIFF 2.2250738585072014e-308   // smallest positive double
#define MINDIFF 2.25e-308                   // use for convergence check

double raiz_quadrada(double square)
{
    double root=square/3, last, diff=1;
    if (square <= 0) return 0;
    do {
        last = root;
        root = (root + square / root) / 2;
        diff = root - last;
    } while (diff > MINDIFF || diff < -MINDIFF);
    return root;
}
void vira(int grau){
	Vector3 *atual;
	Vector3 *guarda;
	get_gyro_angles(guarda);
	get_gyro_angles(atual);
	float dest = (guarda->y+grau)%360;
	if(dest<0){
		dest = 360 - dest;
	}

	if(atual->y < dest){
		while(atual->y < dest){
				set_torque(20, -20);
				get_gyro_angles(atual);
				// print_num(atual->y);
			}
	}else{
		while(atual->y > dest){
				set_torque(-20, 20);
				get_gyro_angles(atual);
				// print_num(atual->y);
			}
	}
	set_torque(0, 0);
	return;

}
int maisPerto(int tamanho, int *visitados, int num_visitados){
    int novo;
    int menor = 2147483647;
    int menor_val = 0;
    for(int i=0; i < tamanho; i++){
        int val = 1;
        for(int j=0; j < num_visitados; j++){
            if(visitados[j] == i){
                val = 0;
            }
        }
        if(val){
            novo = (friends_locations[i].z*friends_locations[i].z)+(friends_locations[i].z*friends_locations[i].z);
            if(novo<menor){
                menor_val = i;
                menor = novo;
            }
        }
    }
    if(menor == 2147483647){
        return tamanho;
    }
    return menor_val;
}

void alinha(int x, int z){
	Vector3 *posAtual;
	get_current_GPS_position(posAtual);
	Vector3 *m;
	get_gyro_angles(m);
	int vetX = (x - posAtual->x);
	int vetZ = (z - posAtual->z);
	double moduloDest = raiz_quadrada((x*x)+(z*z));
	double moduloAtual = raiz_quadrada((posAtual->x*posAtual->x)+(posAtual->z*posAtual->z));
	double escalar = (x*posAtual->x)+(z*posAtual->x);
	double cosseno = escalar/(moduloDest * moduloAtual);
	float printa;
	// while(cosseno > 0.99 || cosseno < (-0.99)){
	while(cosseno != 1){
		printa = cosseno*100;
		if(printa < 0){
			printa = -printa;
		}
		print_num(printa);
		set_torque(20, -20);
		get_current_GPS_position(posAtual);
		moduloDest = raiz_quadrada((x*x)+(z*z));
		moduloAtual = raiz_quadrada((posAtual->x*posAtual->x)+(posAtual->z*posAtual->z));
		escalar = (x*posAtual->x)+(z*posAtual->x);
		cosseno = escalar/(moduloDest * moduloAtual);
	}
	return;

}

void montanha(){
	int leitura;
	leitura = get_us_distance();
	int direita, esquerda;
	Vector3 *atual;
	get_gyro_angles(atual);
	if(leitura != -1){
		vira(10);
		direita = get_us_distance();
		vira(-20);
		esquerda = get_us_distance();
		if(esquerda > direita){
			vira(20);
		}
		print_num(leitura);
	}
	set_torque(20,20);
	return;
}
int main(){
	int k = 0;
	// const char* a = "-";
	// const char *b = "\n";

	int tamanho = sizeof(friends_locations) / sizeof(friends_locations[0]);
	// int visitados[tamanho];
	// int num_visitados = 0;
	// int proximoDest;
	// proximoDest = maisPerto(tamanho, visitados, num_visitados);
	// print_num(proximoDest);
	int i = 0;
	// print_num(friends_locations[i].x);
	// print_num(friends_locations[i].y);
	// print_num(-friends_locations[i].z);
	alinha(friends_locations[i].x, friends_locations[i].z);
	// for(int i=0; i<tamanho; i++){
	// 	proximoDest = maisPerto(tamanho, visitados, num_visitados);
	// 	alinha(friends_locations[proximoDest].x, friends_locations[proximoDest].z);
	// 	visitados[i] = proximoDest;
	// 	num_visitados++;
	// // }
    //



	// while(1){
	// 	// set_torque(20, 20);
	// 	montanha();
	// 	k = 0;
	// 	while(k < 10000){
	// 		k++;
	// 	}
	// }
	print_num(1);
	while(1);
	return 0;
}
