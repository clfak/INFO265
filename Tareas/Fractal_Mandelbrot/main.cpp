#include <iostream>
#include <fstream>
#include <GL/glew.h>
#include <GLFW/glfw3.h> 

// Metodo de Callback que sera llamado cada vez que se presione una tecla.
static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);

const char *vertex_shader = "#version 130\n"
		"in vec3 vp;"
		"void main () {"
		"  gl_Position = vec4 (vp, 1.0);"
		"}";

// Variables a utilizar para el control de la camara.
double zoom = 1.0;
double cx = 0.0;
double cy = 0.0;

// Tamaño de la ventana
int width = 800;
int height = 600;

int main() {
	GLFWwindow *window = NULL;
	const GLubyte *renderer;
	const GLubyte *version;

	GLuint vao;
	GLuint vbo;
	GLuint ebo;
	GLuint vert_shader, frag_shader;
	GLuint shader_programme;
	
	GLfloat puntos[] = {
                        -1.0f, -1.0f, 0.0f, 
                         1.0f, -1.0f, 0.0f, 
                         1.0f,  1.0f, 0.0f,
                        -1.0f,  1.0f, 0.0f 
                        };

	GLuint indices[] = { 
                         0, 1, 2, 
                         0, 3, 2 
                        };

	if ( !glfwInit() ) {
		fprintf( stderr, "ERROR: could not start GLFW3\n" );
		return 1;
	}

	/*glfwWindowHint( GLFW_CONTEXT_VERSION_MAJOR, 4 );
	glfwWindowHint( GLFW_CONTEXT_VERSION_MINOR, 1 );
	glfwWindowHint( GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE );
	glfwWindowHint( GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE );*/

	window = glfwCreateWindow( width, height, "Fractal de Mandelbrot @ INFO263", NULL, NULL );
	if ( !window ) {
		fprintf( stderr, "ERROR: could not open window with GLFW3\n" );
		glfwTerminate();
		return 1;
	}
	glfwMakeContextCurrent( window );
	glewExperimental = GL_TRUE;
	glewInit();

	renderer = glGetString( GL_RENDERER ); 
	version = glGetString( GL_VERSION );	
	printf( "Renderer: %s\n", renderer );
	printf( "OpenGL version supported %s\n", version );

	glEnable( GL_DEPTH_TEST ); 
	glDepthFunc( GL_LESS );
    
	// Creacion del VAO
	glGenVertexArrays( 1, &vao );
	glBindVertexArray( vao );
	
	// Creacion del VBO de vertices
    glGenBuffers( 1, &vbo );
	glBindBuffer( GL_ARRAY_BUFFER, vbo );
	glBufferData( GL_ARRAY_BUFFER, 12 * sizeof( GLfloat ), puntos, GL_STATIC_DRAW );
	glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, 0, NULL );
	glEnableVertexAttribArray( 0 );

	// Creacion del EBO
    glGenBuffers(1, &ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData( GL_ELEMENT_ARRAY_BUFFER, 6 * sizeof( GLuint ), indices, GL_STATIC_DRAW );

	glfwSetKeyCallback(window, key_callback);

	// Compilacion del fragment shader en el archivo shader.glsl
  	std::ifstream t("shader.glsl");
	if(!t.is_open()) {
		std::cerr << "Cannot open shader.glsl!" << std::endl;
		return 1;
	}
	std::string str((std::istreambuf_iterator<char>(t)),
					 std::istreambuf_iterator<char>());
	const char *fragment_shader  = str.c_str();

	vert_shader = glCreateShader( GL_VERTEX_SHADER );
	glShaderSource( vert_shader, 1, &vertex_shader, NULL );
	glCompileShader( vert_shader );

	frag_shader = glCreateShader( GL_FRAGMENT_SHADER );
	glShaderSource( frag_shader, 1, &fragment_shader, NULL );
	glCompileShader( frag_shader );

	shader_programme = glCreateProgram();
	glAttachShader( shader_programme, frag_shader );
	glAttachShader( shader_programme, vert_shader );
	glLinkProgram( shader_programme );

	// Se guardan las direcciones de las variables tipo Uniform dentro del shader
	GLuint shaderCenter = glGetUniformLocation(shader_programme, "center");
	GLuint shaderZoom = glGetUniformLocation(shader_programme, "zoom");
	GLuint shaderScreenSize = glGetUniformLocation(shader_programme, "screen_size");

	// Se comprueba si el fragment shader tuvo algun error al compilar. Util para debuggear.
	int success;
	glGetShaderiv(frag_shader, GL_COMPILE_STATUS, &success);
	if(!success) {
		int s;
		glGetShaderiv(frag_shader, GL_INFO_LOG_LENGTH, &s);

		char *buf = new char[s];
		glGetShaderInfoLog(frag_shader, s, &s, buf);

		std::cerr << buf << std::endl;
		delete [] buf;
		return 2;
	}

	printf("Controles:\nA: izquierda\nS: abajo\nD: derecha\nW: arriba\nZoom+: punto\nZoom-: coma\n");
	
	while ( !glfwWindowShouldClose( window ) ) {
		glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
		glUseProgram( shader_programme );

		// A las direcciones de las variables tipo Uniform se envian los sgtes datos
		glUniform2f(shaderCenter, cx, cy);
		glUniform1f(shaderZoom, zoom);
		glUniform2f(shaderScreenSize, width, height);

		glBindVertexArray( vao );
		glDrawElements( GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

		glfwPollEvents();
		glfwSwapBuffers( window );
	}

	glfwTerminate();
	return 0;
}

/*
Controles:
A: izquierda
S: abajo
D: derecha
W: arriba
Zoom+: punto
Zoom-: coma
*/
static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	const double d = 0.01 / zoom;

	if(key == GLFW_KEY_ESCAPE) {
		glfwSetWindowShouldClose(window, 1);
	} if(key == GLFW_KEY_A) {
		cx -= d;
	} if(key == GLFW_KEY_D) {
		cx += d;
	} if(key == GLFW_KEY_W) {
		cy += d;
	} if(key == GLFW_KEY_S) {
		cy -= d;
	} if(key == GLFW_KEY_PERIOD) {
		zoom += 0.2*zoom;
	} if(key == GLFW_KEY_COMMA) {
		zoom -= 0.2*zoom;
	}
	

}
