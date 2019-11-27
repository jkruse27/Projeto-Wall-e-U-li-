#include "api_robot.h"
//#define MINDIFF 2.2250738585072014e-308   // smallest positive double
#define MINDIFF 2.25e-308                   // use for convergence check
#define SPEED 15
#define abs(x)  x < 0 ? -x : x

void move(int d);
void viraPara(int grau);
int maisPerto(int tamanho, int *visitados, int num_visitados);
void vira(int grau);
float raiz_quadrada(float square);
int arccosseno100(float cosseno);
void alinha(int x, int z);
int amigoProx();
int inimigoProx();
int montanha();
int object_in_front();


// Verifica se o angulo c esta entre a e b (pelo lado positivo)
int between(int a, int b, int c){
	if (a < b){
		return c >= a && b >= c;
	}else{
		return (b >= c && c >= 0) || (c >= a && c <= 360);
	}
}


//Alinha o robo a 90 graus com o inimigo proximo
void avoidEnemy(int pos){
	int x = dangerous_locations[pos].x;
	int z = dangerous_locations[pos].z;
	Vector3 posAtual;
	get_current_GPS_position(&posAtual);
	int vetX = (x - posAtual.x);
	int vetZ = (z - posAtual.z);

	//Encontra o angulo absoluto do amigo em relacao a onde estamos
	float modulo = raiz_quadrada((vetX*vetX)+(vetZ*vetZ));
	float cosseno = (vetZ/(modulo));
	int angulo;
	Vector3 ang;
	get_gyro_angles(&ang);
	if(cosseno < 0)
		angulo = 180 - arccosseno100((-cosseno));
	else
		angulo = arccosseno100(cosseno);
	
	if(vetX < 0)
		angulo = 360 - angulo;
	
	if(!(between(ang.y, (ang.y + 90)%360, angulo) || between((ang.y + 270)%360, ang.y, angulo)))
		return;
	// Se alinha tangentemente ao inimigo
	if(between(ang.y, (ang.y + 180)%360, angulo))
		angulo = (angulo-85)%360;	
	else
		angulo = (angulo+85)%360;	
	if(angulo < 0)
		angulo = 360 + angulo;
	viraPara(angulo);
}


//Verifica se o amigo que estamos buscando esta a menos de 5dm
int is_friend_near(int f_num){
	int x = friends_locations[f_num].x;
	int z = friends_locations[f_num].z;
	Vector3 k;
	get_current_GPS_position(&k);
	int d = ((x-k.x)*(x-k.x))+((z-k.z)*(z-k.z));
	return d < 25;
}

//Verifica se ha algum objeto na frente
int object_in_front(){
	int ret = 0;
	short a = get_us_distance();

	//Se estiver vendo algo a menos de 600dm verifica se eh amigo 
	if(a < 600 && a != -1){
		set_torque(-SPEED, -SPEED);
		ret = 1;
		set_torque(0, 0);
	}

	//retorna 1 se tiver obstaculo e 0 caso nao
	return ret; 
}

//Anda uma distancia d
void move(int d){
	Vector3 v;
	Vector3 a;
	int l;
	l = get_time();
	get_current_GPS_position(&v);
	set_torque(10, 10);
	get_current_GPS_position(&a);

	//Anda uma distancia d
	while((((a.z-v.z)*(a.z-v.z)) + ((a.x-v.x)*(a.x-v.x))) < (d*d)){
		get_current_GPS_position(&a);
		if(object_in_front())
			break;	
		//Se demora mais de 3 segundos sai, pois deve ter ficado preso
		if(get_time() - l > 3000)
			break;
	}
	set_torque(0,0);
	return; 
}

//Calcula a raiz de um numero
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

//Calcula o arccosseno de um cosseno, retornando um valor entre  0 e 90 graus
int arccosseno100(float cosseno){
    cosseno = cosseno * 100;

    //Aproximacao de arcocosseno por retas
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

//Verifica se ha um inimigo proximo
//Se tiver retorna o seu numero, caso nao retorna 0
int inimigoProx(){
    int tamanho = sizeof(dangerous_locations) / sizeof(dangerous_locations[0]);
    Vector3 posAtual;
    int vetX, vetZ, dist;
    for (int i = 0; i < tamanho; i++){
        get_current_GPS_position(&posAtual);
        vetX = (dangerous_locations[i].x - posAtual.x);
        vetZ = (dangerous_locations[i].z - posAtual.z);
        dist = raiz_quadrada((vetX*vetX) + (vetZ*vetZ));
        if(dist <= 15){
            return i+1;
        }
    }
    return 0;
}

//Gira um angulo relativo ao angulo atual do robo
void vira(int grau){
	Vector3 atual;
	Vector3 guarda;
	int speed = SPEED;
	
	//Encontra o angulo absoluto
	get_gyro_angles(&guarda);
	int dest = (guarda.y+grau)%360;
	
	if(dest<0)
		dest = 360 + dest;
	
	//Gira pelo lado com menor angulo
	get_gyro_angles(&atual);
	if(abs(dest - atual.y) < 180){
		if(dest < atual.y)
			speed = -speed;
		set_torque(speed, -speed);
		while(abs((atual.y - dest)) > 2){
			get_gyro_angles(&atual);
		}
		
	}else{
		if(dest < atual.y)
			speed = -speed;
		set_torque(-speed, speed);
		while(abs((atual.y-dest)) > 2){
			get_gyro_angles(&atual);
		}
	}
	set_torque(0, 0);
	return;
}

//Gira um angulo absoluto
void viraPara(int grau){
	Vector3 atual;
	int speed = SPEED;
	get_gyro_angles(&atual);
	//Gira pelo lado com menor angulo
	if(abs((grau - atual.y)) < 180){
		if(grau < atual.y)
			speed = -speed;
		set_torque(speed, -speed);
		while(abs((atual.y - grau)) > 2){
			get_gyro_angles(&atual);
		}
		
	}else{
		if(grau < atual.y)
			speed = -speed;
		set_torque(-speed, speed);
		while(abs((atual.y - grau)) > 2){
			get_gyro_angles(&atual);
		}
	}
	set_torque(0, 0);
	return;
}

int montanha(){
	Vector3 a;
	get_gyro_angles(&a);
	int z = a.z;
	if(z > 180)
		z = abs((z - 360));
	if(z > 10)
		return 1;
	return 0;
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

//Alinha o robo com o amigo que esta sendo buscado
void alinha(int x, int z){
	Vector3 posAtual;
	get_current_GPS_position(&posAtual);
	int vetX = (x - posAtual.x);
	int vetZ = (z - posAtual.z);

	//Encontra o angulo absoluto do amigo em relacao a onde estamos
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

	//Vira o angulo encontrado
	viraPara(angulo);

	return;
}

int main(){
	int tamanho = sizeof(friends_locations) / sizeof(friends_locations[0]);
	Vector3 l;
	Vector3 k;	

	//Para cada amigo
	for(int i = 0; i < tamanho; i++){
		//Enquanto nao tiver chegado no amigo	
		while(1){
			get_current_GPS_position(&l);	
			if(!inimigoProx())
				alinha(friends_locations[i].x, friends_locations[i].z);
			if(is_friend_near(i))
				break;
			if(inimigoProx()){
		        	avoidEnemy(inimigoProx() - 1);
				if(!object_in_front())
					move(2);
			}
			//Se tiver algo na frente vira 90 graus e anda um pouco
			else if(object_in_front()){
				if(is_friend_near(i))
					break;
				vira(-55);
				if(is_friend_near(i))
					break;
				
				if(object_in_front())
					vira(-55);
				move(5);
			}else	
				//Anda um pouco
				move(2);

			//Se tiver um amigo perto vai pro proximo amigo
			if(is_friend_near(i))
				break;
			
			get_current_GPS_position(&k);
			
			//Se nao tiver se mexido quer dizer que esta preso, vira 180 e and um pouco
			if(k.x == l.x && k.z == l.z){
				vira(180);
				if(!object_in_front(i))
					move(1);
			}
		}
	}

	while(1);
	return 0;
}
