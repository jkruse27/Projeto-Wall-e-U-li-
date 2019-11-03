#include "api_robot2.h"
int main(){
	const char* oi = "a\n";
	Vector3 *n;
	Vector3 *c;
	get_current_GPS_position(n);
	int s = (n->x*n->x) + (n->y*n->y) + (n->z*n->z);
	int a = set_torque(10, 10);
	int t = s;
	while(t - s < 100){
		get_current_GPS_position(c);
		t = (c->x*c->x) + (c->y*c->y) + (c->z*c->z);
		puts(oi);
	}
	set_torque(0, 0);
	return 0;
}
