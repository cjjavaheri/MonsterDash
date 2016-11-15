/// OpenGL callback function prototypes
///// Display handler
void display(void);
///// Handler for keyboard events
void keyboard(const byte key, const int x, const int y);
///// Handler for mouse motion (i.e. drag) events
void mouseMotion(const int x, const int y);
///// Handler for mouse click events
void mouseClick(const int button, const int state, const int x, const int y);
///// Handler for reshape events
void reshape(const int w, const int h);
//// other function prototypes
///// Function to do the initialization of the openGL event handlers
void initOpenGL(char *name, int msec, const int rows, const int cols);
void ticktock(int);
