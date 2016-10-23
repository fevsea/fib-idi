#version 330 core

in vec3 fcolor;
out vec4 FragColor;


void main() {
    FragColor = vec4(fcolor, 0);

    /** 4 colors with franges
    if(int(gl_FragCoord.y) % 16 < 8) discard;

	if(gl_FragCoord.x<355 && gl_FragCoord.y<355){
	    FragColor = vec4(1,1,0,0);
	}else if(gl_FragCoord.x<355 && gl_FragCoord.y>355){
		FragColor = vec4(1,0,0,0);
	}else if(gl_FragCoord.x>355 && gl_FragCoord.y<355){
		FragColor = vec4(0,1,0,0);
	}else{
		FragColor = vec4(0,0,1,0);
	}
	**/
}

