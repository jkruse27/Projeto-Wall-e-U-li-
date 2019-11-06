#include "api_robot2.h"
int main(){
	int k;
	const char* a = "-";
	const char *b = "\n";
	
	Vector3 *n;
	get_gyro_angles(n);
	set_torque(30, -30);
	int x = n->x%360;
	int y = n->y%360;
	int z = n->z%360;
	Vector3 *e;
	while(1){
	get_gyro_angles(e);
	k = (e->y)%360;
	print_num(k);
	if(k  (y+90)%360)
		set_torque(0,0);
//	get_current_position(e);
//	k = (((e->x)-x)*((e->x)-x)) + (((e->y)-y)*((e->y)-y)) +(((e->z)-z)*((e->z)-z));
	//print_num(k);
	//if((e->y)%360 == (y+90))
		//set_torque(0,0);	
//	if(n->x < 0){
//		n->x = n->x * -1;
//		puts(a);
//	}
//	print_num(n->x);
//	if(n->y < 0){
//		n->y = n->y * -1;
//		puts(a);
//		}
//	print_num(n->y);
//	if(n->z < 0){
//		n->z = n->z * -1;
//		puts(a);
//		}
//	print_num(n->z);
//	puts(b);
//	k = get_us_distance();
//	if(k<0){
//		k = k*-1;
//		puts(a);
//		}
//	print_num(k);
//	get_gyro_angles(n);
//	if(n->x < 0)
//		n->x = n->x * -1;
//	if(n->y < 0)
//		n->y = n->y * -1;
//	if(n->z < 0)
//		n->z = n->z * -1;
//	print_num(n->x);
//	print_num(n->y);
//	print_num(n->z);
	}
	while(1);
	return 0;
}
