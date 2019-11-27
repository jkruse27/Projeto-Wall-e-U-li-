#include "api_robot.h"
//#define MINDIFF 2.2250738585072014e-308   // smallest positive double
#define MINDIFF 2.25e-308                   // use for convergence check
#define SPEED 15
#define abs(x)  x < 0 ? -x : x

//codigo eh mais constante a 60 fps

void move(int d);
void viraPara(int grau);
void vira(int grau);
float raiz_quadrada(float square);
int arccosseno100(float cosseno);
void alinha(int x, int z);
int amigoProx();
int inimigoProx();
int object_in_front();
int between(int a, int b, int c);


// Verifica se o angulo c esta entre a e b (pelo lado positivo)
int between(int a, int b, int c){
	if (a < b){
		return c >= a && b >= c;
	}else{
		return (b >= c && c >= 0) || (c >= a && c <= 360);
	}
}


//Funcao para desviar dos inimigos
//recebe a posicao no vetor de inimigos e nao retorna nada,
//ela ira verificar se a posicao atual do
//robo esta perto do raio do inimigo, se estiver ela fara o robo gira em 90
//graus, assim a o robo ficara tangente ao raio do inimigo
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


//Funcao que verifica se o robo esta perto de um dos amigos
//Ela recebe a posicao do vetor de amigos, e retorna 1 caso esteja perto do amigo
//e 0 caso contrario
int is_friend_near(int f_num){
	int x = friends_locations[f_num].x;
	int z = friends_locations[f_num].z;
	Vector3 k;
	get_current_GPS_position(&k);
	int d = ((x-k.x)*(x-k.x))+((z-k.z)*(z-k.z));
	return d < 25;
}

//Funcao que verifica o sensor ultra sonico
//Se o sensor ler alguma coisa entao a funcao retorna ele reduz a velocidade
//e retorna 1 se tiver algu obstaculo, e 0 caso contrario
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

//Funcao que recebe uma distancia d
//ela fara o robo andar essa distancia d bna direcao ja apontada
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

//Funcao que recebe um numero e retorna a sua raiz quadrada
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

//Funcao que calculo o arccosseno de um valor de cosseno
//Essa funcao eh uma reducao da funcao de arccosseno em 4 retas
//link da reducao: https://www.desmos.com/calculator/woex89ybek
//ela retorna o angulo do cosseno
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

//Funcao que verifica se ha algum inimigo proximo
//ela ira verificar se ha algum inimigo proximo, se houver ela ira retornar
//a posicao do vetor de inimigos do inimigo proximo
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

//Funcao que recebe um angulo e faz um giro relativo
//Ela ira girar o robo em um angulo relativo ao que ele esta, entao se ela
//ta virado para o angulo 0, e receber 30, entao ele ira virar 30 graus
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

//Funcao que recebe um angulo e gira para o angulo absoluto do plano
//Essa funcao rebcebe um angulo e fara com que o robo gire para esse angulo
//tal angulo e baseado nos dados do plano
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


//Funcao que alinha o robo com uma coordenada passada
//Essa funcao recebera as coordenadas para onde ele quer ir e girara para que o
//vetor para onde o robo esta apontando chegue no vetor para onde queremos ir
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
