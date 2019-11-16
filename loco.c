#include "api_robot.h"
//#define MINDIFF 2.2250738585072014e-308   // smallest positive double
#define MINDIFF 2.25e-308                   // use for convergence check
#define SPEED 15

int abs(int a);
void move(int d);
void viraPara(int grau);
int maisPerto(int tamanho, int *visitados, int num_visitados);
void vira(int grau);
float raiz_quadrada(float square);
int arccosseno100(float cosseno);
void alinha(int x, int z);
int amigoProx();
int inimigoProx();
void montanha();
int object_in_front(int i);


int is_friend_near(int f_num){
	int x = friends_locations[f_num].x;
	int z = friends_locations[f_num].z;
	Vector3 k;
	get_current_GPS_position(&k);
	int d = ((x-k.x)*(x-k.x))+((z-k.z)*(z-k.z));
	if(d < 25)
		puts("ola companheiro\n");
	return d < 25;
}

int abs(int a){
	return a < 0 ? -a : a;
}

int object_in_front(int i){
	int ret = 0;
	short a = get_us_distance();
	if(a < 600 && a != -1){
		Vector3 k;
		int x = friends_locations[i].x;
		int z = friends_locations[i].z;
		get_current_GPS_position(&k);
		int d = ((x-k.x)*(x-k.x))+((z-k.z)*(z-k.z));
		if(d != (a*a)){
			set_torque(-SPEED, -SPEED);
			ret = 1;
			puts("fodeu\n");
			set_torque(0, 0);
		}
	}
	return ret; 
}

void move(int d){
	Vector3 v;
	Vector3 a;
	int l;
	l = get_time();
	get_current_GPS_position(&v);
	set_torque(10, 10);
	get_current_GPS_position(&a);
	while((((a.z-v.z)*(a.z-v.z)) + ((a.x-v.x)*(a.x-v.x))) < (d*d)){
		get_current_GPS_position(&a);
		if(get_time() - l > 3000)
			break;
	}
	set_torque(0,0);
	return; 
}

float raiz_quadrada(float square){
    float root=square/3, last, diff=1;
    if (square <= 0) return 0;
    do {
        last = root;
        root = (root + square / root) / 2;
        diff = root - last;
    } while (diff > MINDIFF || diff < -MINDIFF);
    return root;
}

int arccosseno100(float cosseno){
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

int inimigoProx(){
    int tamanho = sizeof(dangerous_locations) / sizeof(dangerous_locations[0]);
    Vector3 posAtual;
    int vetX, vetZ, dist;
    for (int i = 0; i < tamanho; i++){
        get_current_GPS_position(&posAtual);
        vetX = (dangerous_locations[i].x - posAtual.x);
        vetZ = (dangerous_locations[i].z - posAtual.z);
        dist = raiz_quadrada((vetX*vetX) + (vetZ*vetZ));
        if(dist <= 12){
            return 1;
        }
    }
    return 0;
}

void vira(int grau){
	Vector3 atual;
	Vector3 guarda;
	int speed = SPEED;
	get_gyro_angles(&guarda);
	int dest = (guarda.y+grau)%360;
	
	if(dest<0)
		dest = 360 + dest;
	
	get_gyro_angles(&atual);
	if(abs(dest - atual.y) < 180){
		if(dest < atual.y)
			speed = -speed;
		set_torque(speed, -speed);
		while(atual.y != dest){
			get_gyro_angles(&atual);
		}
		
	}else{
		if(dest < atual.y)
			speed = -speed;
		set_torque(-speed, speed);
		while(atual.y != dest){
			get_gyro_angles(&atual);
		}
	}
	set_torque(0, 0);
	return;
}

void viraPara(int grau){
	Vector3 atual;
	int speed = SPEED;
	get_gyro_angles(&atual);
	if(abs(grau - atual.y) < 180){
		if(grau < atual.y)
			speed = -speed;
		set_torque(speed, -speed);
		while(atual.y != grau){
			get_gyro_angles(&atual);
		}
		
	}else{
		if(grau < atual.y)
			speed = -speed;
		set_torque(-speed, speed);
		while(atual.y != grau){
			get_gyro_angles(&atual);
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
	Vector3 posAtual;
	get_current_GPS_position(&posAtual);
	int vetX = (x - posAtual.x);
	int vetZ = (z - posAtual.z);
	float modulo = raiz_quadrada((vetX*vetX)+(vetZ*vetZ));
	float cosseno = (vetZ/(modulo));
	int angulo;
	Vector3 ang;
	get_gyro_angles(&ang);
	if(cosseno < 0){
		angulo = 180 - arccosseno100((-cosseno));
	}else{
		angulo = arccosseno100(cosseno);
	}
	if(vetX < 0){
		angulo = 360 - angulo;
	}
	viraPara(angulo);

	return;
}



void montanha(){
	int leitura;
	leitura = get_us_distance();
	int direita, esquerda;
	Vector3 atual;
	get_gyro_angles(&atual);
	if(leitura != -1){
		vira(10);
		direita = get_us_distance();
		vira(-20);
		esquerda = get_us_distance();
		if(esquerda > direita){
			vira(20);
		}
	}
	set_torque(20,20);
	return;
}

int main(){
	int tamanho = sizeof(friends_locations) / sizeof(friends_locations[0]);
	Vector3 l;
	Vector3 k;
	for(int i = 0; i < tamanho; i++){	
		while(1){
			get_current_GPS_position(&l);
			alinha(friends_locations[i].x, friends_locations[i].z);
			if(object_in_front(i)){
				vira(-90);
				if(is_friend_near(i))
					break;
			}
			if(object_in_front(i)){
				vira(90);
				vira(90);
				if(is_friend_near(i))
					break;
			}
			move(2);
			if(is_friend_near(i))
				break;
			get_current_GPS_position(&k);
			if(k.x == l.x && k.z == l.z){
				vira(180);
				move(2);
				puts("que merda\n");
			}
		}
	}
	
	puts("Acabou Porra\n");
	while(1);
	return 0;
}
