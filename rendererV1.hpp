
// my windows renderer

#pragma once
#ifndef _GGE2d_01
#define _GGE2d_01

// libraries needed by default
#pragma comment(lib, "user32.lib")
#pragma comment(lib, "Gdi32.lib")
#pragma comment(lib, "shell32.lib")
#pragma comment(lib, "opengl32.lib")

#ifndef __glew_h__
    // getting glew if not imported already
    #define GLEW_STATIC
    #include <GL/glew.h>
    #pragma comment(lib, "/opengl/glew/lib/Release/x64/glew32s.lib")
#endif
#ifndef _glfw3_h_
    // getting glfw if not imported already
    #include <GLFW/glfw3.h>
    #pragma comment(lib, "/opengl/glfw/lib/glfw3_mt.lib")
#endif

#include <cmath>
#include <vector>
#include <iostream>

// our namespace so that renderer created with groover::Renderer();
// well when you inherit the class

void _update_fps_counter(GLFWwindow* window, const char* title) {
  static double previous_seconds = glfwGetTime();
  static int frame_count;
  double current_seconds = glfwGetTime();
  double elapsed_seconds = current_seconds - previous_seconds;
  if (elapsed_seconds > 0.25) {
    previous_seconds = current_seconds;
    double fps = (double)frame_count / elapsed_seconds;
    char tmp[100];
    sprintf(tmp, "fps: %.2f", fps);
    glfwSetWindowTitle(window, tmp);
    frame_count = 0;
  }
  frame_count++;
}

namespace groover {
    // use this struct in future for points etc
    struct point {
    float x;
    float y;
    float z=0.0;
    float w=1.0;
    };

    void norm_dev(int width, int height, point* p) {
        // directly changes the point struct
        p->x = (2.0 * p->x) / width - 1;
        p->y = 1 - (2.0 * p->y) / height;
        // p->z = ((2.0 * z) - f - n) / (f - n)) - 1.0; //(f and n are far and near)
        // p->w = 1.0f
    }

    class Renderer {
        // TODO: Support Text
        // ie new shader etc?
        private:
            void _compile_shaders() {
                // maybe just use one shader overall
                static const char* vs_p[] = {
                "#version 450 core                              \n"
                "layout(location=0) in vec4 vec_points;         \n"
                "layout(location=1) in vec4 in_color;           \n"
                "out vec4 color;                                \n"
                "void main() {                                  \n"
                "   color = in_color;                           \n"
                "   gl_Position = vec_points;                   \n"
                "}                                              \n"
                };

               static const char* fs_p[] = {
                "#version 450 core                               \n"
                "in vec4 color;                                  \n"
                "out vec4 out_color;                             \n"
                "void main() {                                   \n"
                "   out_color = color;                           \n"
                "}                                               \n"
                };

                GLuint vertexshader_p = glCreateShader(GL_VERTEX_SHADER);
                GLuint fragshader = glCreateShader(GL_FRAGMENT_SHADER);

                glShaderSource(vertexshader_p, 1, vs_p, NULL);
                glShaderSource(fragshader, 1, fs_p, NULL);

                glCompileShader(vertexshader_p);
                glCompileShader(fragshader);

                GLint result;
                int InfoLogLength;

                program = glCreateProgram();
                glAttachShader(program, vertexshader_p);
                glAttachShader(program, fragshader);
                glLinkProgram(program);

                glGetProgramiv(program, GL_VALIDATE_STATUS, &result);
                glGetProgramiv(program, GL_INFO_LOG_LENGTH, &InfoLogLength);
                if ( InfoLogLength > 0 ){
                    std::vector<char> ProgramErrorMessage(InfoLogLength+1);
                    glGetProgramInfoLog(program, InfoLogLength, NULL, &ProgramErrorMessage[0]);
                    fprintf(stdout, "%s\n", &ProgramErrorMessage[0]);
                }

                glDeleteShader(vertexshader_p);
                glDeleteShader(fragshader);
            }

        public:

            GLFWwindow* window;
            int winWidth, winHeight;
            const char* name = "null";
            std::vector<float> points;
            std::vector<float> tris;
            std::vector<float> circles;
            std::vector<float> lines;
            // unfortunately each one needs its own color vector the way im doing it
            std::vector<float> color_p;
            std::vector<float> color_t;
            std::vector<float> color_c;
            float set_color[4] = { 1.0f, 0.0f, 0.0f, 1.0f };
            GLuint program;
            float bgc[4] = { 0.0f, 0.0f, 0.0f, 1.0f };

            Renderer() { }
            ~Renderer() { }
            virtual bool Update(float deltaTime) { return false; }
            virtual bool Startup() { return false; }
            void CreateWin(const char* title, int width, int height, bool bFullScreen) {
                this->CreateWin(title, width, height, bFullScreen ? glfwGetPrimaryMonitor() : NULL, NULL);
            }
            void CreateWin(const char* title, int width, int height, GLFWmonitor* monitor, GLFWwindow* share) {
                glfwInit();
                winWidth = width; winHeight = height; name = title;
                window = glfwCreateWindow(winWidth, winHeight, name, monitor, share);

                glfwMakeContextCurrent(window);
                //glewExperimental = GL_TRUE;
                glewInit();


                _compile_shaders(); // get our shaders
            }

            void DrawPoint(point p) {
                groover::norm_dev(winWidth, winHeight, &p);
                points.push_back(p.x); points.push_back(p.y); points.push_back(p.z); points.push_back(p.w);
                color_p.push_back(set_color[0]); color_p.push_back(set_color[1]);
                color_p.push_back(set_color[2]); color_p.push_back(set_color[3]);
            }

            void DrawTriangle(point p1, point p2, point p3) {
                groover::norm_dev(winWidth, winHeight, &p1);
                groover::norm_dev(winWidth, winHeight, &p2);
                groover::norm_dev(winWidth, winHeight, &p3);
                tris.push_back(p1.x); tris.push_back(p1.y); tris.push_back(p1.z); tris.push_back(p1.w);
                tris.push_back(p2.x); tris.push_back(p2.y); tris.push_back(p2.z); tris.push_back(p2.w);
                tris.push_back(p3.x); tris.push_back(p3.y); tris.push_back(p3.z); tris.push_back(p3.w);
                for (int i=0; i < 3; ++ i){
                    color_t.push_back(set_color[0]); color_t.push_back(set_color[1]);
                    color_t.push_back(set_color[2]); color_t.push_back(set_color[3]);
                }
            }

            void DrawRec(point p1, point p2) {
                // this draws rectangeles with top left point, and bottom right point
                // generates other points automatic since it draws it as 2 triangles
                // this->DrawTriangle will norm_dev for us :)
                point p3 = point{ p1.x, p2.y };
                point p4 = point{ p2.x, p1.y };
                this->DrawTriangle(p3, p1, p4);
                this->DrawTriangle(p3, p2, p4);
            }

            void DrawCircle(point p, float radius) {
                static const float PI = 3.14159265358979f;
                int nTri = 30;
                float twoPi = 2.0f * PI;
                groover::norm_dev(winWidth, winHeight, &p);
                circles.push_back(p.x); circles.push_back(p.y);
                circles.push_back(p.z); circles.push_back(p.w);
                color_c.push_back(set_color[0]); color_c.push_back(set_color[1]);
                color_c.push_back(set_color[2]); color_c.push_back(set_color[3]);
                for (int i=0; i <= nTri; ++i) {
                    float theta_x = (radius * cos(i *  twoPi / nTri)) / winWidth;
                    float theta_y = (radius * sin(i *  twoPi / nTri)) / winHeight;
                    circles.push_back(p.x + theta_x);
                    circles.push_back(p.y + theta_y);
                    circles.push_back(p.z); circles.push_back(1.0f);
                    color_c.push_back(set_color[0]); color_c.push_back(set_color[1]);
                    color_c.push_back(set_color[2]); color_c.push_back(set_color[3]);
                }
            }

            void SetBackgroundColor(int r, int g, int b, int a=255) {
                bgc[0] = r / 255.0f; bgc[1] = g / 255.0f; bgc[2] = b / 255.0f; bgc[3] = a / 255.0f;
            }
            // user needs to define color before using otherwise will be black
            void SetColor(int r, int g, int b, int a=255) {
                set_color[0] = r / 255.0f; set_color[1] = g / 255.0f; set_color[2] = b / 255.0f; set_color[3] = a / 255.0f;
            }

            float GetTime() {
                return (float)glfwGetTime();
            }
            // TODO: figure out what type a mouse would be/return state of mouse
            //bool GetMouseInput() as a private (do a callback like key?)




            bool Run() {
                if (!window) {
                    fprintf(stderr, "Window not created, did you call CreateWin?");
                    return false;
                }

                GLuint vbp; // points buffer
                glGenBuffers(1, &vbp);
                GLuint vap; // point
                glGenVertexArrays(1, &vap);

                GLuint vbt; // tri buffer
                glGenBuffers(1, &vbt);
                GLuint vat; // tri
                glGenVertexArrays(1, &vat);

                GLuint vbc; // circle buff
                glGenBuffers(1, &vbc);
                GLuint vac; // circle
                glGenVertexArrays(1, &vac);

                GLuint vbcolor; // color buffer
                glGenBuffers(1, &vbcolor);

                if(!this->Startup()) { fprintf(stderr, "You need to define the StartUp method and have it return true"); }
                // we are doing startup :( why isnt this working
                float lastT = glfwGetTime();

                while(!glfwWindowShouldClose(window)) {
                    float currentT = glfwGetTime();
                    float deltaTime = (currentT - lastT);
                    _update_fps_counter(window, name);

                    glfwGetWindowSize(window, &winWidth, &winHeight);
                    glViewport(0, 0, winWidth, winHeight); // fixes resizing things when we change screen size
                    this->Update(deltaTime); // we need to do update before drawing

                    glClearColor(bgc[0], bgc[1], bgc[2], bgc[3]);
                    glClear(GL_COLOR_BUFFER_BIT); //  | GL_DEPTH_BUFFER_BIT

                    glUseProgram(program);

                    if (points.size() > 0) {
                        glBindVertexArray(vap);
                        glEnableVertexAttribArray(0);
                        glBindBuffer(GL_ARRAY_BUFFER, vbp);  // binding buffer (setting as current buffer)
                        glBufferData(GL_ARRAY_BUFFER, points.size() * sizeof(float), &points.front(), GL_STATIC_DRAW);
                        glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, NULL);

                        glEnableVertexAttribArray(1);
                        glBindBuffer(GL_ARRAY_BUFFER, vbcolor);
                        glBufferData(GL_ARRAY_BUFFER, color_p.size() * sizeof(float), &color_p.front(), GL_STATIC_DRAW);
                        glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, NULL);
                        // needed a 1 where i put a 0 above

                        glDrawArrays(GL_POINTS, 0, points.size() / 4);
                        color_p.clear(); // forgot this :)
                        points.clear(); // this is extremely important!!!!
                    }

                    if (tris.size() > 0) {
                        glBindVertexArray(vat);
                        glEnableVertexAttribArray(0);
                        glBindBuffer(GL_ARRAY_BUFFER, vbt);
                        glBufferData(GL_ARRAY_BUFFER, tris.size() * sizeof(float), &tris.front(), GL_STATIC_DRAW);
                        glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, NULL);

                        glEnableVertexAttribArray(1);
                        glBindBuffer(GL_ARRAY_BUFFER, vbcolor);
                        glBufferData(GL_ARRAY_BUFFER, color_t.size() * sizeof(float), &color_t.front(), GL_STATIC_DRAW);
                        glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, NULL);

                        glDrawArrays(GL_TRIANGLES, 0, tris.size() / 4);
                        color_t.clear();
                        tris.clear();
                    }

                    if (circles.size() > 0) {
                        glBindVertexArray(vac);
                        glEnableVertexAttribArray(0);
                        glBindBuffer(GL_ARRAY_BUFFER, vbc);
                        glBufferData(GL_ARRAY_BUFFER, circles.size() * sizeof(float), &circles.front(), GL_STATIC_DRAW);
                        glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, NULL);

                        glEnableVertexAttribArray(1);
                        glBindBuffer(GL_ARRAY_BUFFER, vbcolor);
                        glBufferData(GL_ARRAY_BUFFER, color_c.size() * sizeof(float), &color_c.front(), GL_STATIC_DRAW);
                        glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, NULL);

                        glDrawArrays(GL_TRIANGLE_FAN, 0, circles.size() / 4);
                        color_c.clear();
                        circles.clear();
                    }

                    glfwSwapBuffers(window);

                    glfwPollEvents();

                    // points.clear(); // this is extremely important!!!! moved inside respective location
                    // lines.clear(); // unused currently

                    // note setting fps does make it so it doesn't use 100% of gpu
                    // Sleep function is defined in windows.h so find another?
                    // can do the std::thread::this version
                    lastT = currentT;

                }
                return true;
            }
    };
}
#endif