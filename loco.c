#include "api_robot2.h"
//#define MINDIFF 2.2250738585072014e-308   // smallest positive double
#define MINDIFF 2.25e-308                   // use for convergence check

double raiz_quadrada(double square){
    double root=square/3, last, diff=1;
    if (square <= 0) return 0;
    do {
        last = root;
        root = (root + square / root) / 2;
        diff = root - last;
    } while (diff > MINDIFF || diff < -MINDIFF);
    return root;
}

int arccosseno100(double cosseno){
    cosseno = cosseno * 100;
    int arccos = 0;
    if(cosseno < 60){
        arccos = -0.6*cosseno + 90;
    }else if(cosseno < 90){
        arccos = -0.9*(cosseno-60)+54;
    }else if(cosseno < 99){
        arccos = -2*(cosseno-90)+27;
    }else if(cosseno <= 100){
        arccos = -9*(cosseno-99)+9;
    }
    return arccos;
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
    print_num(dest);
	if(atual->y < dest){
		while(atual->y < dest){
				set_torque(15, -15);
				get_gyro_angles(atual);
				// print_num(atual->y);
			}
	}else{
		while(atual->y > dest){
				set_torque(-15, 15);
				get_gyro_angles(atual);
				// print_num(atual->y);
			}
	}
	set_torque(0, 0);
	return;

}
void viraPara(int grau){
	Vector3 *atual;
	get_gyro_angles(atual);
	if(atual->y < grau){
		while(atual->y < grau){
				set_torque(20, -20);
				get_gyro_angles(atual);
				// print_num(atual->y);
			}
	}else{
		while(atual->y > grau){
				set_torque(-20, 20);
				get_gyro_angles(atual);
				// print_num(atual->y);
			}
	}
    print_num(atual->y);
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
    int vetX = (x - posAtual->x);
	int vetZ = (z - posAtual->z);
    double modulo = raiz_quadrada((vetX*vetX)+(vetZ*vetZ));
    double cosseno = (vetX/(modulo));
    int angulo;
    if(cosseno < 0){
        angulo = 180 - arccosseno100(cosseno);
    }else{
        angulo = arccosseno100(cosseno);
    }
    //vetor unitario que pra a qual o robo esdta virado eh o vetor (1,0)

    if(z >= posAtual->z){
        viraPara(0);
        if(x > posAtual->x){
            vira(angulo);
        }else{
            vira(-angulo);
        }
    }
    if(z < posAtual->z){
        viraPara(180);
        if(x < posAtual->x){
            vira(angulo);
        }else{
            vira(-angulo);
        }
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
    // vira(180);
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
