#version 130

out vec4 colorOut;

// Variables que se recibiran desde CPU
uniform vec2 screen_size;
uniform vec2 center;
uniform float zoom;

#define n 2000

void main()
{
	vec2 c;
	float screen_ratio = screen_size.x/screen_size.y;
	c.x = screen_ratio * (gl_FragCoord.x / screen_size.x - 0.5);
	c.y = (gl_FragCoord.y / screen_size.y - 0.5);
	c.x /= zoom;
	c.y /= zoom;
	c.x += center.x;
	c.y += center.y;

	//C.A 
	vec2 z = c;
	float i;
	for(i = 1.0; i < n; i++){
		float t = 2.0 * z.x * z.y + c.y;
		z.x = z.x * z.x - z.y * z.y +c.x;
		z.y = t;
		if(z.x * z.x + z.y * z.y > 4.0){
			break;
		}
	}
	if(i < n - 1)
	{
		float diff = log(float(i)) / log(float(n));
		colorOut = vec4(diff) * vec4(1.0, 0.0, 1.0, 1.0);
	}else{
		colorOut = vec4(0.0, 0.0, 1.0, 1.0);
	}	
}
