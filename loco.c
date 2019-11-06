#include "api_robot2.h"


void rotY(int grau){
	Vector3 *atual;
	get_gyro_angles(atual);
	while(atual->y != grau){
		set_torque(10, -10);
		get_gyro_angles(atual);
	}
	set_torque(0, 0);
	return;

}
void gira180(){
	Vector3 *atual;
	Vector3 *ver;
	get_gyro_angles(atual);
	get_gyro_angles(ver);
	while(ver->y != atual->y+180){
		set_torque(10, -10);
		get_gyro_angles(ver);
	}
	set_torque(0, 0)
}
int main(){
	rotY(0);


	while(1){
		continue;
	}
	return 0;
}
